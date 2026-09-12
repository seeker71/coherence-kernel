// Node carrier for the platform-neutral TypeScript kernel.
//
// This is the only module in the package that imports Node facilities.  The
// browser export never references it; CLI callers opt in explicitly through
// createNodeKernelHost().

import {
  appendFileSync,
  closeSync,
  existsSync,
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
import { dirname, isAbsolute, join, relative, resolve } from "node:path";
import { Worker } from "node:worker_threads";
import type {
  KernelHost,
  KernelHttpRequest,
  KernelHttpResult,
  KernelPgAnswer,
  KernelPgOperation,
  KernelSocketOperation,
  SourceInventoryEntry,
} from "./host.ts";

const UTF8_DECODER = new TextDecoder();
const SOCKET_BYTES = 65_536;
const HTTP_MAX_BODY_BYTES = 25 << 20;
const HTTP_RESULT_BYTES = 64 << 20;
const PG_ANSWER_BYTES = 64 << 20;

// Where a host path names a file for a read-side door, the same way on every kernel. A path that
// stands where the kernel runs names itself. Otherwise the walk tries dir/p, dir/form/p and
// dir/form/form/p from the working directory upward and stops at the checkout that holds it, the
// first directory with a .git entry, so one checkout never reads another's files. The kernel's
// read-side doors ask for it through Host.resolveReadPath; doors that create or change a file
// never walk, and the host primitives below name paths as given.
function resolveHostReadPath(path: string): string {
  if (path.length === 0 || isAbsolute(path) || existsSync(path)) return path;
  let directory = process.cwd();
  while (true) {
    for (const candidate of [
      join(directory, path),
      join(directory, "form", path),
      join(directory, "form", "form", path),
    ]) {
      if (existsSync(candidate)) return candidate;
    }
    if (existsSync(join(directory, ".git"))) break;
    const parent = dirname(directory);
    if (parent === directory) break;
    directory = parent;
  }
  return path;
}

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

// The pg carrier: a Postgres v3 wire client, sibling to the Go (pgx) and Rust (postgres) natives. It asks
// for TimeZone UTC and UTF8 at startup, answers trust, cleartext, md5 and SCRAM-SHA-256, and runs SQL by
// the simple Query protocol, or by Parse/Bind/Execute when parameters travel as text. Each operation is
// answered exactly once, as JSON in the shared buffer.
const PG_WORKER_SOURCE = String.raw`
const { parentPort, workerData } = require("node:worker_threads");
const net = require("node:net");
const crypto = require("node:crypto");
const ctrl = new Int32Array(workerData.ctrl);
const data = Buffer.from(workerData.data);
const connections = new Map();
let nextId = 1;
function answer(result) {
  let encoded = Buffer.from(JSON.stringify(result), "utf8");
  if (encoded.length > data.length) {
    encoded = Buffer.from(JSON.stringify({ error: "pg: the answer exceeded the shared buffer" }), "utf8");
  }
  encoded.copy(data, 0);
  Atomics.store(ctrl, 1, encoded.length);
  Atomics.store(ctrl, 0, 1);
  Atomics.notify(ctrl, 0);
}
function u16(n) { const b = Buffer.alloc(2); b.writeUInt16BE(n, 0); return b; }
function i32(n) { const b = Buffer.alloc(4); b.writeInt32BE(n, 0); return b; }
function cstr(s) { return Buffer.concat([Buffer.from(s, "utf8"), Buffer.from([0])]); }
function frameOut(tag, body) { return Buffer.concat([Buffer.from(tag, "latin1"), i32(body.length + 4), body]); }
// a connection's incoming bytes, taken as whole frames
function reader(socket) {
  const r = { buffer: Buffer.alloc(0), closed: false, wake: null };
  const wake = () => { const w = r.wake; r.wake = null; if (w) w(); };
  socket.on("data", (chunk) => { r.buffer = Buffer.concat([r.buffer, chunk]); wake(); });
  socket.on("close", () => { r.closed = true; wake(); });
  socket.on("error", () => { r.closed = true; wake(); });
  return r;
}
async function frameIn(r) {
  for (;;) {
    if (r.buffer.length >= 5) {
      const size = r.buffer.readUInt32BE(1);
      if (size < 4) return null;
      if (r.buffer.length >= size + 1) {
        const frame = { type: r.buffer[0], body: r.buffer.subarray(5, size + 1) };
        r.buffer = r.buffer.subarray(size + 1);
        return frame;
      }
    }
    if (r.closed) return null;
    await new Promise((ok) => { r.wake = ok; });
  }
}
// an ErrorResponse as the Go and Rust natives word it: the message, then its detail and hint
function errorText(body) {
  const fields = {};
  let i = 0;
  while (i < body.length && body[i] !== 0) {
    const end = body.indexOf(0, i + 1);
    if (end < 0) break;
    fields[String.fromCharCode(body[i])] = body.toString("utf8", i + 1, end);
    i = end + 1;
  }
  const parts = [fields.M || "the server reported an error without a message"];
  if (fields.D) parts.push("detail: " + fields.D);
  if (fields.H) parts.push("hint: " + fields.H);
  return parts.join(" | ");
}
function hmac(key, text) { return crypto.createHmac("sha256", key).update(text).digest(); }
function attr(message, key) {
  for (const part of message.split(",")) if (part.startsWith(key + "=")) return part.slice(key.length + 1);
  return "";
}
function refused(socket, error) { socket.destroy(); return { error }; }
async function connect(m) {
  const socket = net.connect(m.port, m.host);
  const reached = await new Promise((ok) => {
    const timer = setTimeout(() => ok(false), 5000);
    socket.once("connect", () => { clearTimeout(timer); ok(true); });
    socket.once("error", () => { clearTimeout(timer); ok(false); });
  });
  if (!reached) return refused(socket, "pg_connect: cannot reach " + m.host + ":" + m.port);
  const r = reader(socket);
  const params = Buffer.concat([cstr("user"), cstr(m.user), cstr("database"), cstr(m.database), cstr("TimeZone"), cstr("UTC"), cstr("client_encoding"), cstr("UTF8"), Buffer.from([0])]);
  socket.write(Buffer.concat([i32(params.length + 8), i32(196608), params]));
  let scram = null;
  for (;;) {
    const f = await frameIn(r);
    if (f === null) return refused(socket, "pg_connect: the connection ended during startup, or sent a frame this client cannot read");
    if (f.type === 90) break;
    if (f.type === 69) return refused(socket, errorText(f.body));
    if (f.type !== 82) continue;
    const code = f.body.readUInt32BE(0);
    if (code === 0) continue;
    if (code === 3) { socket.write(frameOut("p", cstr(m.password))); continue; }
    if (code === 5) {
      const inner = crypto.createHash("md5").update(m.password + m.user).digest("hex");
      const outer = crypto.createHash("md5").update(Buffer.concat([Buffer.from(inner), f.body.subarray(4, 8)])).digest("hex");
      socket.write(frameOut("p", cstr("md5" + outer)));
      continue;
    }
    if (code === 10 && f.body.subarray(4).toString("utf8").split("\0").includes("SCRAM-SHA-256")) {
      const nonce = crypto.randomBytes(18).toString("base64");
      scram = { nonce, bare: "n=,r=" + nonce, signature: "" };
      const first = Buffer.from("n,," + scram.bare, "utf8");
      socket.write(frameOut("p", Buffer.concat([cstr("SCRAM-SHA-256"), i32(first.length), first])));
      continue;
    }
    if (code === 11 && scram !== null) {
      const serverFirst = f.body.toString("utf8", 4);
      const serverNonce = attr(serverFirst, "r");
      if (!serverNonce.startsWith(scram.nonce)) return refused(socket, "pg_connect: the server's SCRAM nonce does not extend ours");
      const salted = crypto.pbkdf2Sync(m.password, Buffer.from(attr(serverFirst, "s"), "base64"), Number(attr(serverFirst, "i")), 32, "sha256");
      const withoutProof = "c=biws,r=" + serverNonce;
      const auth = scram.bare + "," + serverFirst + "," + withoutProof;
      const clientKey = hmac(salted, "Client Key");
      const signature = hmac(crypto.createHash("sha256").update(clientKey).digest(), auth);
      const proof = Buffer.alloc(clientKey.length);
      for (let i = 0; i < proof.length; i++) proof[i] = clientKey[i] ^ signature[i];
      scram.signature = hmac(hmac(salted, "Server Key"), auth).toString("base64");
      socket.write(frameOut("p", Buffer.from(withoutProof + ",p=" + proof.toString("base64"), "utf8")));
      continue;
    }
    if (code === 12 && scram !== null) {
      if (attr(f.body.toString("utf8", 4), "v") !== scram.signature) return refused(socket, "pg_connect: the server's SCRAM signature does not match");
      continue;
    }
    return refused(socket, "pg_connect: authentication method " + code + " is not carried; trust, password, md5 and SCRAM-SHA-256 are");
  }
  const id = nextId++;
  connections.set(id, { socket, r });
  return { handle: id };
}
async function run(m) {
  const c = connections.get(m.h);
  if (c === undefined) return { error: "unknown connection handle" };
  if (m.params === null) {
    c.socket.write(frameOut("Q", cstr(m.sql)));
  } else {
    const values = m.params.map((p) => {
      if (p === null) return i32(-1);
      const bytes = Buffer.from(p, "utf8");
      return Buffer.concat([i32(bytes.length), bytes]);
    });
    c.socket.write(Buffer.concat([
      frameOut("P", Buffer.concat([cstr(""), cstr(m.sql), u16(0)])),
      frameOut("B", Buffer.concat([cstr(""), cstr(""), u16(0), u16(values.length), ...values, u16(0)])),
      frameOut("D", Buffer.concat([Buffer.from("P", "latin1"), cstr("")])),
      frameOut("E", Buffer.concat([cstr(""), i32(0)])),
      frameOut("S", Buffer.alloc(0)),
    ]));
  }
  let fields = [];
  const rows = [];
  let tag = "";
  let error = "";
  for (;;) {
    const f = await frameIn(c.r);
    if (f === null) return { error: "the connection ended, or sent a frame this client cannot read" };
    if (f.type === 90) break;
    if (f.type === 84) {
      fields = [];
      let i = 2;
      for (let n = f.body.readUInt16BE(0); n > 0; n--) {
        const end = f.body.indexOf(0, i);
        fields.push({ name: f.body.toString("utf8", i, end), oid: f.body.readUInt32BE(end + 7) });
        i = end + 19;
      }
    } else if (f.type === 68) {
      const row = [];
      let i = 2;
      for (let n = f.body.readUInt16BE(0); n > 0; n--) {
        const size = f.body.readInt32BE(i);
        i += 4;
        if (size < 0) { row.push(null); continue; }
        row.push(f.body.toString("utf8", i, i + size));
        i += size;
      }
      rows.push(row);
    } else if (f.type === 67) {
      const end = f.body.indexOf(0);
      tag = f.body.toString("utf8", 0, end < 0 ? f.body.length : end);
    } else if (f.type === 69) {
      error = errorText(f.body);
    }
  }
  return error.length > 0 ? { error } : { fields, rows, tag };
}
function close(m) {
  const c = connections.get(m.h);
  if (c === undefined) return { error: "unknown connection handle" };
  try { c.socket.end(frameOut("X", Buffer.alloc(0))); } catch {}
  connections.delete(m.h);
  return {};
}
const OPERATIONS = { connect, run, close };
parentPort.on("message", (m) => {
  const operation = OPERATIONS[m.op];
  Promise.resolve()
    .then(() => (operation === undefined ? { error: "pg: unknown operation " + m.op } : operation(m)))
    .then(answer, (e) => answer({ error: String(e && e.message ? e.message : e) }));
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
  private pgWorker?: Worker;
  private pgControl?: Int32Array;
  private pgData?: Uint8Array;

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

  pgCall(operation: KernelPgOperation): KernelPgAnswer {
    this.ensurePg();
    const control = this.pgControl!;
    Atomics.store(control, 0, 0);
    this.pgWorker!.postMessage(operation);
    Atomics.wait(control, 0, 0);
    const length = Atomics.load(control, 1);
    if (length <= 0) return { error: "pg: the carrier failed" };
    return JSON.parse(UTF8_DECODER.decode(this.pgData!.subarray(0, length))) as KernelPgAnswer;
  }

  shutdown(): void {
    if (this.socketWorker !== undefined) void this.socketWorker.terminate();
    if (this.httpWorker !== undefined) void this.httpWorker.terminate();
    if (this.pgWorker !== undefined) void this.pgWorker.terminate();
    this.socketWorker = undefined;
    this.socketControl = undefined;
    this.socketData = undefined;
    this.httpWorker = undefined;
    this.httpControl = undefined;
    this.httpData = undefined;
    this.pgWorker = undefined;
    this.pgControl = undefined;
    this.pgData = undefined;
  }

  private ensurePg(): void {
    if (this.pgWorker !== undefined) return;
    const controlBuffer = new SharedArrayBuffer(8);
    const dataBuffer = new SharedArrayBuffer(PG_ANSWER_BYTES);
    this.pgControl = new Int32Array(controlBuffer);
    this.pgData = new Uint8Array(dataBuffer);
    this.pgWorker = new Worker(PG_WORKER_SOURCE, {
      eval: true,
      workerData: { ctrl: controlBuffer, data: dataBuffer },
    });
    this.pgWorker.unref();
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
    // Decode STRICTLY. `readFileSync(path, "utf8")` decodes with replacement, so a binary
    // file came back as a plausible string: measured 2026-07-30, a 19936-byte raw PCM clip
    // read as str_len 37558 with byte 255 arriving as 191 — a cell computing audio levels
    // off that would get confident numbers from text that never existed. Rust's read_file
    // (fs::read_to_string) already yields Null on invalid UTF-8; this matches it, and the
    // caller's existing catch turns the throw into that same Null. Valid UTF-8 is untouched.
    readTextFile: (path) =>
      new TextDecoder("utf-8", { fatal: true }).decode(
        readFileSync(path),
      ),
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
    makeDirectory: (path) => mkdirSync(path),
    removeDirectory: (path) => rmSync(path, { recursive: true, force: true }),
    removePath: (path) => unlinkSync(path),
    renamePath: (from, to) => renameSync(from, to),
    listDirectory: (path) => readdirSync(path),
    resolveReadPath: resolveHostReadPath,
    sourceInventory: inventory,
    randomBytes: (length) => Uint8Array.from(nodeRandomBytes(length)),
    processId: () => process.pid,
    monotonicMs: () => performance.now(),
    workingDirectory: () => process.cwd(),
    homeDirectory: () => process.env.HOME ?? "",
    tempDirectory:
      options.tempDirectory ??
      (() => tmpdir().replace(/\/+$/, "") || "/tmp"),
    httpGet: (request) => workers.httpGet(request),
    socketCall: (operation) => workers.socketCall(operation),
    pgCall: (operation) => workers.pgCall(operation),
    shutdown: () => workers.shutdown(),
  };
}
