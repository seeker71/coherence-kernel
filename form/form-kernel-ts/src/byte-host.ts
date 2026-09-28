// byte-host.ts — this kernel's strings hold bytes, as fkwu's do.
//
// A Form string is a byte string: one JS code unit per byte, 0..255. str_len is
// its length, str_byte_at its code unit, a substring a slice, byte_to_str one
// unit, and a cut inside a multi-byte character is the same bytes fkwu holds.
// Source files are read as latin1, so a literal "Ω" is the two bytes 206 169.
//
// Text meets bytes only at the host: every string the kernel hands the host
// (a path, a URL, SQL, socket text) is decoded from its UTF-8 bytes, and every
// string the host hands back (a directory name, a working directory, an HTTP
// body, a pg cell) is encoded to its UTF-8 bytes. File contents cross as
// bytes, never decoded: read_file answers a file's own bytes, write_file writes
// the string's own bytes. A string holding a unit above 255 was made by the
// kernel itself as JS text (a message with an em dash); it crosses as text.

import type {
  KernelHost,
  KernelHttpRequest,
  KernelHttpResult,
  KernelPgAnswer,
  KernelPgOperation,
  KernelSocketOperation,
  SourceInventoryEntry,
} from "./host.ts";

const ENCODER = new TextEncoder();
const DECODER = new TextDecoder("utf-8");

export function bytesToBstr(bytes: Uint8Array): string {
  let out = "";
  const CHUNK = 0x8000;
  for (let i = 0; i < bytes.length; i += CHUNK) {
    out += String.fromCharCode(...bytes.subarray(i, i + CHUNK));
  }
  return out;
}

export function bstrToBytes(s: string): Uint8Array {
  const out = new Uint8Array(s.length);
  for (let i = 0; i < s.length; i++) out[i] = s.charCodeAt(i) & 0xff;
  return out;
}

// a unit above 255 marks JS text the kernel made itself
export function isWide(s: string): boolean {
  for (let i = 0; i < s.length; i++) if (s.charCodeAt(i) > 255) return true;
  return false;
}

export function textToBstr(text: string): string {
  return bytesToBstr(ENCODER.encode(text));
}

export function bstrToText(s: string): string {
  return isWide(s) ? s : DECODER.decode(bstrToBytes(s));
}

// JSON leaves arrive as text; each string leaf becomes its bytes.
export function jsonLeavesToBstr(v: unknown): unknown {
  if (typeof v === "string") return textToBstr(v);
  if (Array.isArray(v)) return v.map(jsonLeavesToBstr);
  if (v !== null && typeof v === "object") {
    const out: Record<string, unknown> = {};
    for (const [k, x] of Object.entries(v)) out[textToBstr(k)] = jsonLeavesToBstr(x);
    return out;
  }
  return v;
}

function opt<A extends unknown[], R>(
  f: ((...a: A) => R) | undefined,
  wrap: (g: (...a: A) => R) => (...a: A) => R,
): ((...a: A) => R) | undefined {
  return f === undefined ? undefined : wrap(f);
}

export function byteHost(host: KernelHost): KernelHost {
  const p = bstrToText;
  const t = textToBstr;
  const out = (write: ((text: string) => void) | undefined,
    writeBytes: ((bytes: Uint8Array) => void) | undefined) =>
    write === undefined && writeBytes === undefined ? undefined : (s: string) => {
      if (!isWide(s) && writeBytes !== undefined) writeBytes(bstrToBytes(s));
      else write?.(bstrToText(s));
    };
  return {
    writeStdout: out(host.writeStdout, host.writeStdoutBytes),
    writeStderr: out(host.writeStderr, host.writeStderrBytes),
    resolveReadPath: opt(host.resolveReadPath, (g) => (path) => t(g(p(path)))),
    readTextFile: host.readBinaryFile !== undefined
      ? (path) => bytesToBstr(host.readBinaryFile!(p(path)))
      : opt(host.readTextFile, (g) => (path) => t(g(p(path)))),
    readBinaryFile: opt(host.readBinaryFile, (g) => (path) => g(p(path))),
    readBinarySlice: opt(host.readBinarySlice, (g) => (path, offset, length) => g(p(path), offset, length)),
    writeTextFile: host.writeBinaryFile !== undefined
      ? (path, text) => isWide(text)
        ? host.writeBinaryFile!(p(path), ENCODER.encode(text))
        : host.writeBinaryFile!(p(path), bstrToBytes(text))
      : opt(host.writeTextFile, (g) => (path, text) => g(p(path), p(text))),
    writeBinaryFile: opt(host.writeBinaryFile, (g) => (path, bytes) => g(p(path), bytes)),
    appendBinaryFile: opt(host.appendBinaryFile, (g) => (path, bytes) => g(p(path), bytes)),
    fileSize: opt(host.fileSize, (g) => (path) => g(p(path))),
    fileMtimeSeconds: opt(host.fileMtimeSeconds, (g) => (path) => g(p(path))),
    pathExists: opt(host.pathExists, (g) => (path) => g(p(path))),
    pathIsDirectory: opt(host.pathIsDirectory, (g) => (path) => g(p(path))),
    makeDirectory: opt(host.makeDirectory, (g) => (path) => g(p(path))),
    removeDirectory: opt(host.removeDirectory, (g) => (path) => g(p(path))),
    removePath: opt(host.removePath, (g) => (path) => g(p(path))),
    renamePath: opt(host.renamePath, (g) => (from, to) => g(p(from), p(to))),
    listDirectory: opt(host.listDirectory, (g) => (path) => g(p(path)).map(t)),
    sourceInventory: opt(host.sourceInventory, (g) => (root, suffix, skip) =>
      g(p(root), p(suffix), new Set([...skip].map(p))).map(
        (e): SourceInventoryEntry => ({ path: t(e.path), lines: e.lines }))),
    randomBytes: host.randomBytes,
    tempDirectory: opt(host.tempDirectory, (g) => () => t(g())),
    processId: host.processId,
    monotonicMs: host.monotonicMs,
    workingDirectory: opt(host.workingDirectory, (g) => () => t(g())),
    homeDirectory: opt(host.homeDirectory, (g) => () => t(g())),
    httpGet: opt(host.httpGet, (g) => (request: KernelHttpRequest): KernelHttpResult => {
      const headers: Record<string, readonly string[]> = {};
      for (const [k, vs] of Object.entries(request.headers)) headers[p(k)] = vs.map(p);
      const r = g({ url: p(request.url), headers, timeoutMs: request.timeoutMs });
      return {
        statusCode: r.statusCode,
        body: t(r.body),
        error: t(r.error),
        durationMs: r.durationMs,
        headers: r.headers.map(([i, k, v]) => [i, t(k), t(v)] as const),
      };
    }),
    socketCall: opt(host.socketCall, (g) => (op: KernelSocketOperation) => {
      const r = g(op.op === "send" ? { ...op, text: p(op.text) }
        : op.op === "connect" ? { ...op, host: p(op.host) } : op);
      return typeof r === "string" ? t(r) : r;
    }),
    pgCall: opt(host.pgCall, (g) => (op: KernelPgOperation): KernelPgAnswer => {
      const r = g(op.op === "connect"
        ? { ...op, host: p(op.host), user: p(op.user), password: p(op.password), database: p(op.database) }
        : op.op === "run"
          ? { ...op, sql: p(op.sql), params: op.params === null ? null : op.params.map((x) => x === null ? null : p(x)) }
          : op);
      return {
        ...r,
        error: r.error === undefined ? undefined : t(r.error),
        fields: r.fields?.map((f) => ({ name: t(f.name), oid: f.oid })),
        rows: r.rows?.map((row) => row.map((c) => c === null ? null : t(c))),
        tag: r.tag === undefined ? undefined : t(r.tag),
      };
    }),
    shutdown: host.shutdown,
  };
}
