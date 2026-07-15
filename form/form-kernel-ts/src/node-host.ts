// Node carrier for the platform-neutral TypeScript kernel.
//
// This is the only module in the package that imports Node facilities.  The
// browser export never references it; CLI callers opt in explicitly through
// createNodeKernelHost().

import {
  appendFileSync,
  closeSync,
  mkdirSync,
  openSync,
  readdirSync,
  readFileSync,
  readSync,
  renameSync,
  rmSync,
  statSync,
  unlinkSync,
  writeFileSync,
} from "node:fs";
import { randomBytes as nodeRandomBytes } from "node:crypto";
import { tmpdir } from "node:os";
import { join, relative, resolve } from "node:path";
import { Worker } from "node:worker_threads";
import type {
  KernelHost,
  KernelHttpRequest,
  KernelHttpResult,
  KernelSocketOperation,
  SourceInventoryEntry,
} from "./host.ts";

const UTF8_DECODER = new TextDecoder();
const SOCKET_BYTES = 65_536;
const HTTP_MAX_BODY_BYTES = 25 << 20;
const HTTP_RESULT_BYTES = 64 << 20;

const SOCKET_WORKER_SOURCE = String.raw`
const { parentPort, workerData } = require("node:worker_threads");
const net = require("node:net");
const ctrl = new Int32Array(workerData.ctrl);
const data = Buffer.from(workerData.data);
const handles = new Map();
let nextId = 1;
function done(value) {
  Atomics.store(ctrl, 1, value);
  Atomics.store(ctrl, 0, 1);
  Atomics.notify(ctrl, 0);
}
parentPort.on("message", async (message) => {
  try {
    if (message.op === "listen") {
      const id = nextId++;
      const server = net.createServer();
      const record = { kind: "listener", object: server, backlog: [], port: -1 };
      server.on("connection", (socket) => { socket.pause(); record.backlog.push(socket); });
      await new Promise((ok, fail) => {
        server.once("error", fail);
        server.listen(message.port, "127.0.0.1", ok);
      });
      record.port = server.address().port;
      handles.set(id, record);
      done(id);
    } else if (message.op === "port") {
      const record = handles.get(message.h);
      done(record && record.kind === "listener" ? record.port : -1);
    } else if (message.op === "connect") {
      const id = nextId++;
      const socket = net.connect(message.port, message.host);
      await new Promise((ok, fail) => {
        socket.once("connect", ok);
        socket.once("error", fail);
      });
      const record = { kind: "connection", object: socket, received: Buffer.alloc(0), closed: false };
      handles.set(id, record);
      socket.on("data", (chunk) => { record.received = Buffer.concat([record.received, chunk]); });
      socket.on("close", () => { record.closed = true; });
      done(id);
    } else if (message.op === "accept") {
      const listener = handles.get(message.h);
      if (!listener || listener.kind !== "listener") { done(-1); return; }
      while (listener.backlog.length === 0) await new Promise((ok) => setTimeout(ok, 1));
      const socket = listener.backlog.shift();
      const id = nextId++;
      const record = { kind: "connection", object: socket, received: Buffer.alloc(0), closed: false };
      handles.set(id, record);
      socket.on("data", (chunk) => { record.received = Buffer.concat([record.received, chunk]); });
      socket.on("close", () => { record.closed = true; });
      socket.resume();
      done(id);
    } else if (message.op === "send") {
      const record = handles.get(message.h);
      if (!record || record.kind !== "connection") { done(-1); return; }
      const bytes = Buffer.from(message.text, "utf8");
      record.object.write(bytes);
      done(bytes.length);
    } else if (message.op === "recv") {
      const record = handles.get(message.h);
      if (!record || record.kind !== "connection") { done(0); return; }
      while (record.received.length === 0 && !record.closed) {
        await new Promise((ok) => setTimeout(ok, 1));
      }
      const count = Math.min(message.max, record.received.length, data.length);
      record.received.subarray(0, count).copy(data, 0);
      record.received = record.received.subarray(count);
      done(count);
    } else if (message.op === "close") {
      const record = handles.get(message.h);
      if (!record) { done(-1); return; }
      try { record.object.destroy(); } catch {}
      handles.delete(message.h);
      done(0);
    } else {
      done(-1);
    }
  } catch {
    done(-1);
  }
});
`;

const HTTP_WORKER_SOURCE = String.raw`
const { parentPort, workerData } = require("node:worker_threads");
const http = require("node:http");
const https = require("node:https");
const ctrl = new Int32Array(workerData.ctrl);
const data = Buffer.from(workerData.data);
const maxBody = workerData.maxBody;
function done(length) {
  Atomics.store(ctrl, 1, length);
  Atomics.store(ctrl, 0, 1);
  Atomics.notify(ctrl, 0);
}
function writeResult(result) {
  let encoded = Buffer.from(JSON.stringify(result), "utf8");
  if (encoded.length > data.length) {
    encoded = Buffer.from(JSON.stringify({
      statusCode: result.statusCode || 0,
      body: "",
      error: "http_get: response result exceeded shared buffer",
      durationMs: result.durationMs || 0,
      headers: result.headers || [],
    }), "utf8");
  }
  const length = Math.min(encoded.length, data.length);
  encoded.copy(data, 0, 0, length);
  done(length);
}
function headerRows(headers) {
  const rows = [];
  for (const name of Object.keys(headers).sort()) {
    const raw = headers[name];
    if (raw === undefined) continue;
    const values = Array.isArray(raw) ? raw.slice().sort() : [String(raw)];
    for (const value of values) rows.push([43001, name, value]);
  }
  return rows;
}
parentPort.on("message", (message) => {
  const started = Date.now();
  try {
    const url = new URL(message.url);
    const client = url.protocol === "https:" ? https : http;
    const request = client.request(url, {
      method: "GET",
      headers: message.headers || {},
      timeout: message.timeoutMs || 30_000,
    }, (response) => {
      const chunks = [];
      let size = 0;
      let tooLarge = false;
      response.on("data", (chunk) => {
        const remaining = maxBody - size;
        if (remaining > 0) {
          const take = chunk.length > remaining ? chunk.subarray(0, remaining) : chunk;
          chunks.push(take);
          size += take.length;
        }
        if (chunk.length > remaining) tooLarge = true;
      });
      response.on("end", () => writeResult({
        statusCode: response.statusCode || 0,
        body: Buffer.concat(chunks).toString("utf8"),
        error: tooLarge ? "http_get: response body exceeded " + maxBody + " bytes" : "",
        durationMs: Date.now() - started,
        headers: headerRows(response.headers),
      }));
    });
    request.on("timeout", () => request.destroy(new Error("http_get: timeout")));
    request.on("error", (error) => writeResult({
      statusCode: 0,
      body: "",
      error: String(error && error.message ? error.message : error),
      durationMs: Date.now() - started,
      headers: [],
    }));
    request.end();
  } catch (error) {
    writeResult({
      statusCode: 0,
      body: "",
      error: String(error && error.message ? error.message : error),
      durationMs: Date.now() - started,
      headers: [],
    });
  }
});
`;

function countLines(bytes: Uint8Array): number {
  if (bytes.length === 0) return 0;
  let lines = 0;
  for (const byte of bytes) if (byte === 10) lines++;
  return bytes[bytes.length - 1] === 10 ? lines : lines + 1;
}

function inventory(
  root: string,
  suffix: string,
  skip: ReadonlySet<string>,
): SourceInventoryEntry[] {
  const rootAbsolute = resolve(root);
  const rows: SourceInventoryEntry[] = [];
  const walk = (directory: string): void => {
    const entries = readdirSync(directory, { withFileTypes: true }).sort((a, b) =>
      a.name.localeCompare(b.name),
    );
    for (const entry of entries) {
      const path = join(directory, entry.name);
      if (entry.isDirectory()) {
        if (!skip.has(entry.name)) walk(path);
      } else if (entry.isFile() && (suffix === "" || entry.name.endsWith(suffix))) {
        rows.push({
          path: relative(rootAbsolute, path).split(/[\\/]+/).join("/"),
          lines: countLines(readFileSync(path)),
        });
      }
    }
  };
  walk(rootAbsolute);
  return rows;
}

class WorkerCarriers {
  private socketWorker?: Worker;
  private socketControl?: Int32Array;
  private socketData?: Uint8Array;
  private httpWorker?: Worker;
  private httpControl?: Int32Array;
  private httpData?: Uint8Array;

  socketCall(operation: KernelSocketOperation): number | string {
    this.ensureSocket();
    const control = this.socketControl!;
    Atomics.store(control, 0, 0);
    this.socketWorker!.postMessage(operation);
    Atomics.wait(control, 0, 0);
    const value = Atomics.load(control, 1);
    if (operation.op !== "recv") return value;
    if (value <= 0) return "";
    return UTF8_DECODER.decode(this.socketData!.subarray(0, value));
  }

  httpGet(request: KernelHttpRequest): KernelHttpResult {
    this.ensureHttp();
    const control = this.httpControl!;
    Atomics.store(control, 0, 0);
    this.httpWorker!.postMessage(request);
    Atomics.wait(control, 0, 0);
    const length = Atomics.load(control, 1);
    if (length <= 0) {
      return {
        statusCode: 0,
        body: "",
        error: "http_get: worker failed",
        durationMs: 0,
        headers: [],
      };
    }
    return JSON.parse(UTF8_DECODER.decode(this.httpData!.subarray(0, length))) as KernelHttpResult;
  }

  shutdown(): void {
    if (this.socketWorker !== undefined) void this.socketWorker.terminate();
    if (this.httpWorker !== undefined) void this.httpWorker.terminate();
    this.socketWorker = undefined;
    this.socketControl = undefined;
    this.socketData = undefined;
    this.httpWorker = undefined;
    this.httpControl = undefined;
    this.httpData = undefined;
  }

  private ensureSocket(): void {
    if (this.socketWorker !== undefined) return;
    const controlBuffer = new SharedArrayBuffer(8);
    const dataBuffer = new SharedArrayBuffer(SOCKET_BYTES);
    this.socketControl = new Int32Array(controlBuffer);
    this.socketData = new Uint8Array(dataBuffer);
    this.socketWorker = new Worker(SOCKET_WORKER_SOURCE, {
      eval: true,
      workerData: { ctrl: controlBuffer, data: dataBuffer },
    });
    this.socketWorker.unref();
  }

  private ensureHttp(): void {
    if (this.httpWorker !== undefined) return;
    const controlBuffer = new SharedArrayBuffer(8);
    const dataBuffer = new SharedArrayBuffer(HTTP_RESULT_BYTES);
    this.httpControl = new Int32Array(controlBuffer);
    this.httpData = new Uint8Array(dataBuffer);
    this.httpWorker = new Worker(HTTP_WORKER_SOURCE, {
      eval: true,
      workerData: {
        ctrl: controlBuffer,
        data: dataBuffer,
        maxBody: HTTP_MAX_BODY_BYTES,
      },
    });
    this.httpWorker.unref();
  }
}

export interface NodeKernelHostOptions {
  readonly writeStdout?: (text: string) => void;
  readonly writeStderr?: (text: string) => void;
  readonly tempDirectory?: () => string;
}

export function createNodeKernelHost(options: NodeKernelHostOptions = {}): KernelHost {
  const workers = new WorkerCarriers();
  return {
    writeStdout: options.writeStdout ?? ((text) => process.stdout.write(text)),
    writeStderr: options.writeStderr ?? ((text) => process.stderr.write(text)),
    readTextFile: (path) => readFileSync(path, "utf8"),
    readBinaryFile: (path) => readFileSync(path),
    readBinarySlice: (path, offset, length) => {
      const descriptor = openSync(path, "r");
      try {
        const bytes = new Uint8Array(length);
        const count = readSync(descriptor, bytes, 0, length, offset);
        return bytes.subarray(0, count);
      } finally {
        closeSync(descriptor);
      }
    },
    writeTextFile: (path, text) => writeFileSync(path, text, "utf8"),
    writeBinaryFile: (path, bytes) => writeFileSync(path, bytes),
    appendBinaryFile: (path, bytes) => {
      appendFileSync(path, bytes);
      return statSync(path).size;
    },
    fileSize: (path) => statSync(path).size,
    fileMtimeSeconds: (path) => Math.floor(statSync(path).mtimeMs / 1000),
    pathExists: (path) => {
      try { statSync(path); return true; } catch { return false; }
    },
    pathIsDirectory: (path) => statSync(path).isDirectory(),
    makeDirectory: (path) => mkdirSync(path, { recursive: true }),
    removeDirectory: (path) => rmSync(path, { recursive: true, force: true }),
    removePath: (path) => unlinkSync(path),
    renamePath: (from, to) => renameSync(from, to),
    listDirectory: (path) => readdirSync(path),
    sourceInventory: inventory,
    randomBytes: (length) => Uint8Array.from(nodeRandomBytes(length)),
    tempDirectory:
      options.tempDirectory ??
      (() => tmpdir().replace(/\/+$/, "") || "/tmp"),
    httpGet: (request) => workers.httpGet(request),
    socketCall: (operation) => workers.socketCall(operation),
    shutdown: () => workers.shutdown(),
  };
}
