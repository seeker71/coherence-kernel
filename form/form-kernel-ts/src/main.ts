// form-kernel-ts CLI.
//
// Usage:
//   tsx src/main.ts --binary file.fkb
//   tsx src/main.ts --emit-binary out.fkb path/to/file.fk
//   tsx src/main.ts --expr "(+ 1 2)"
//   tsx src/main.ts --bench
//   tsx src/main.ts path/to/file.fk

import { mkdir, readFile, realpath, writeFile, rm } from "node:fs/promises";
import { existsSync } from "node:fs";
import { createHash } from "node:crypto";
import { dirname, isAbsolute, join } from "node:path";
import { isMainThread, Worker, workerData } from "node:worker_threads";
import {
  deserializeRecipeArtifact,
  Frame,
  Kernel,
  serializeRecipeArtifact,
  Trace,
  walk,
} from "./kernel.ts";
import { createNodeKernelHost } from "./node-host.ts";
import { readAll, readForm } from "./reader.ts";
import { runBench } from "./bench.ts";
import { compileNode } from "./compiler.ts";
import { runNumericBench } from "./numeric-bench.ts";

type CrashTraceContext = {
  mode: string;
  args: string[];
  source: string;
};

const crashTraceContext: CrashTraceContext = {
  mode: "startup",
  args: [],
  source: "",
};

// The kernel whose Form call stack the top-level catch surfaces. Set as
// soon as the CLI kernel exists; the frames live at the crash answer
// "which Form source line produced this".
let crashKernel: Kernel | null = null;

type FormSourcePart = {
  path: string;
  source: string;
};

// Scans one source line for a "; preludes: a.fk b.fk ..." directive the way
// fkwu's own fk_src_collect_preludes does: find the literal "preludes:"
// token after a comment marker, then walk whitespace/comma-separated
// tokens until one doesn't look like a real dependency. A token counts
// only when it is the "none" sentinel (declares an explicit empty prelude
// list, e.g. tests/now-unix-ms-band.fk) or ends in ".fk"/".bml" -- anything
// else silently STOPS the scan instead of erroring, so a doc comment that
// merely mentions the word "preludes:" (this tree has several) is never
// misread as a directive. Sibling parity with the fkwu C kernel; unlike
// fkwu, a ".bml" dependency can't be lowered here yet, so the caller
// reports and skips it rather than silently dropping the symbols it would
// have defined.
function formPreludeDeps(line: string): { fkDeps: string[]; bmlDeps: string[] } {
  const fkDeps: string[] = [];
  const bmlDeps: string[] = [];
  // ".fk" comments are ";"-led; ".bml" comments are "//"-led (confirmed:
  // bml-demand-jit-glass.bml declares "// preludes: ..."), and
  // form-source-compile-file's lowering preserves a .bml's original
  // comment lines verbatim, so the lowered text this scanner sees still
  // carries "//", not ";". Recognize whichever marker starts first.
  const semi = line.indexOf(";");
  const slashes = line.indexOf("//");
  let start: number;
  if (semi < 0 && slashes < 0) return { fkDeps, bmlDeps };
  else if (semi < 0) start = slashes + 2;
  else if (slashes < 0) start = semi + 1;
  else if (semi < slashes) start = semi + 1;
  else start = slashes + 2;
  const comment = line.slice(start);
  const needle = "preludes:";
  const idx = comment.indexOf(needle);
  if (idx < 0) return { fkDeps, bmlDeps };
  let rest = comment.slice(idx + needle.length);
  for (;;) {
    rest = rest.replace(/^[ \t,]+/, "");
    if (rest.length === 0) return { fkDeps, bmlDeps };
    const m = rest.match(/[ \t,]/);
    const end = m ? m.index! : rest.length;
    const tok = rest.slice(0, end);
    rest = rest.slice(end);
    if (tok.toLowerCase() === "none" || tok.toLowerCase() === "(none)") {
      return { fkDeps, bmlDeps };
    }
    if (tok.endsWith(".fk")) {
      fkDeps.push(tok);
    } else if (tok.endsWith(".bml")) {
      bmlDeps.push(tok);
    } else {
      return { fkDeps, bmlDeps };
    }
  }
}

function formImportPath(line: string): string | null {
  let source = line.trim();
  if (source.endsWith(";")) source = source.slice(0, -1).trim();
  if (!source.startsWith('import "') || !source.endsWith('"')) return null;
  const path = source.slice(8, -1);
  return path.length > 0 ? path : null;
}

function resolveFormImport(owner: string, imported: string): string {
  const candidates = isAbsolute(imported)
    ? [imported]
    : [
        join(dirname(owner), imported),
        imported,
        ...(imported.startsWith("form/") ? [imported.slice(5)] : []),
      ];
  if (!isAbsolute(imported)) {
    let directory = dirname(owner);
    while (true) {
      candidates.push(join(directory, imported));
      candidates.push(join(directory, "form", imported));
      const parent = dirname(directory);
      if (parent === directory) break;
      directory = parent;
    }
  }
  for (const candidate of candidates) {
    if (existsSync(candidate)) return candidate;
  }
  throw new Error(
    "import \"" + imported + "\" from " + owner + ": file not found",
  );
}

// The fixed set of Form units that implement source-compiler.fk's
// "section [form.bml]" -> plain Form lowering pass (validate.sh's own
// compiler_chain, same order). fkwu cannot run this chain itself --
// source-compiler.fk needs host-I/O natives fkwu lacks -- which is why bare
// fkwu can never parse a raw BML section and validate.sh's prepare_sources
// instead shells out to a Go kernel to run this chain externally. This
// kernel doesn't need to shell out to anything: it already implements
// every native the chain needs, so it runs the lowering on itself, in a
// throwaway Kernel, the moment it meets a ".bml" prelude it can't
// otherwise read.
const FORM_BML_SOURCE_COMPILE_CHAIN = [
  "form-stdlib/engine-constants.fk",
  "form-stdlib/compiler-objects.fk",
  "form-stdlib/form-ontology-bp.fk",
  "form-stdlib/form-ontology-source-categories.fk",
  "form-stdlib/form-ontology-loader.fk",
  "form-stdlib/line-grammar.fk",
  "form-stdlib/bmf-core.fk",
  "form-stdlib/bmf-grammar.fk",
  "form-stdlib/bml.fk",
  "form-stdlib/bml-source.fk",
  "form-stdlib/source-compiler.fk",
  "form-stdlib/grammars/form-bml.fk",
  "form-stdlib/grammars/form-lift.fk",
  "form-stdlib/form-bml-lower.fk",
  "form-stdlib/source-compiler-text-lens.fk",
];

// Lowers one whole ".bml" prelude file into plain Form text by running
// form-source-compile-file (source-compiler.fk) in a fresh, throwaway
// Kernel -- entirely separate from the kernel the CLI eventually builds to
// run the caller's own program. Cached by content hash under
// form-stdlib/.cache/kernel-bml-lowered/ (its own namespace: the key here
// is a plain content hash, not validate.sh's compiler_stamp-qualified one,
// so the two caches don't collide but also don't need to agree bit for
// bit) so a repeated run doesn't re-pay the several-second compile.
async function lowerBmlSource(bmlAbsPath: string): Promise<string> {
  const body = await readFile(bmlAbsPath, "utf8");
  const key = createHash("sha256").update(body).digest("hex").slice(0, 24);

  const chainPaths = FORM_BML_SOURCE_COMPILE_CHAIN.map((rel) => {
    try {
      return resolveFormImport(bmlAbsPath, rel);
    } catch (error) {
      throw new Error(
        `resolve BML compiler chain ${rel} (needed to lower ${bmlAbsPath}): ${(error as Error).message}`,
      );
    }
  });
  const cacheDir = join(dirname(chainPaths[0]), ".cache", "kernel-bml-lowered");
  const cachePath = join(cacheDir, `${key}.fk`);
  try {
    const cached = await readFile(cachePath, "utf8");
    if (cached.length > 0) return cached;
  } catch {
    // not cached yet
  }
  await mkdir(cacheDir, { recursive: true });

  const outPath = join(cacheDir, `.out-${key}-${process.pid}.fk`);
  const driverPath = `${outPath}.driver.fk`;
  const driverSrc = `(do (form-source-compile-file ${JSON.stringify(bmlAbsPath)} ${JSON.stringify(outPath)}))\n`;
  await writeFile(driverPath, driverSrc);
  try {
    const loaded = await loadFormSourceClosure([...chainPaths, driverPath]);
    const compilerSrc = loaded.map((part) => part.source).join("\n");
    const lowerKernel = new Kernel(createNodeKernelHost());
    const lowerFrame = new Frame(null);
    const root = readAll(lowerKernel, compilerSrc);
    walk(lowerKernel, root, lowerFrame);

    const lowered = await readFile(outPath, "utf8").catch(() => "");
    if (lowered.length === 0) {
      throw new Error(`form-source-compile-file produced no output for ${bmlAbsPath}`);
    }
    await writeFile(cachePath, lowered).catch(() => {}); // best-effort cache
    return lowered;
  } finally {
    await rm(driverPath, { force: true });
    await rm(outPath, { force: true });
  }
}

async function loadFormSourceFile(
  path: string,
  displayPath: string,
  seen: Set<string>,
  parts: FormSourcePart[],
): Promise<void> {
  const canonical = await realpath(path);
  if (seen.has(canonical)) return;
  seen.add(canonical);
  const source = await readFile(canonical, "utf8");
  await loadFormSourceText(displayPath, canonical, source, seen, parts);
}

// Mirrors loadFormSourceFile for a ".bml" dependency: same
// dedup-by-canonical-path, same recursive directive handling on the result
// -- just sourced from lowerBmlSource's in-memory text instead of a
// byte-identical read of the path on disk.
async function loadFormSourceBmlPrelude(
  path: string,
  displayPath: string,
  seen: Set<string>,
  parts: FormSourcePart[],
): Promise<void> {
  const canonical = await realpath(path);
  if (seen.has(canonical)) return;
  seen.add(canonical);

  // form-source-compile-file's lowering does NOT preserve a .bml file's own
  // "// preludes:"/import header the way ";"-comment .fk lowering preserves
  // its header (verified: the lowered text opens straight on defns, no
  // comment survives) -- so THIS source's own directives have to be found
  // and recursed on the RAW file, before lowering discards them, rather
  // than by scanning the lowered output the way every other dependency
  // kind is scanned.
  const rawBody = await readFile(canonical, "utf8");
  for (const line of rawBody.split("\n")) {
    const imported = formImportPath(line);
    if (imported !== null) {
      const dependency = resolveFormImport(canonical, imported);
      await loadFormSourceFile(dependency, dependency, seen, parts);
      continue;
    }
    const { fkDeps, bmlDeps } = formPreludeDeps(line);
    for (const tok of fkDeps) {
      const dependency = resolveFormImport(canonical, tok);
      await loadFormSourceFile(dependency, dependency, seen, parts);
    }
    for (const tok of bmlDeps) {
      const dependency = resolveFormImport(canonical, tok);
      await loadFormSourceBmlPrelude(dependency, dependency, seen, parts);
    }
  }

  const lowered = await lowerBmlSource(canonical);
  // The lowered text carries no directives of its own to (re-)scan, but
  // running it through loadFormSourceText anyway keeps this path exactly
  // as defensive as every other loader -- a directive that DID somehow
  // survive lowering would still be honored, not silently ignored.
  await loadFormSourceText(displayPath, canonical, lowered, seen, parts);
}

// Walks one source's lines for import/prelude directives (recursing into
// each dependency) and appends the remaining body as one part. Shared by
// the on-disk (.fk) and lowered-in-memory (.bml) loading paths so both get
// identical directive handling.
async function loadFormSourceText(
  displayPath: string,
  canonical: string,
  source: string,
  seen: Set<string>,
  parts: FormSourcePart[],
): Promise<void> {
  for (const line of source.split("\n")) {
    if (line.trimStart().startsWith("section [")) {
      throw new Error(
        `${displayPath}: carries a raw "section [form.bml]" block -- this kernel runs plain Form, ` +
          "not BML, so it can't parse that block directly. It must be lowered through " +
          "form-stdlib/source-compiler.fk first (validate.sh's prepare_sources does this " +
          'automatically; see form-stdlib/AUTHORING.md\'s "two-layer trap")',
      );
    }
  }
  const body: string[] = [];
  for (const line of source.split("\n")) {
    const imported = formImportPath(line);
    if (imported !== null) {
      const dependency = resolveFormImport(canonical, imported);
      await loadFormSourceFile(dependency, dependency, seen, parts);
      continue;
    }
    const { fkDeps, bmlDeps } = formPreludeDeps(line);
    if (fkDeps.length > 0 || bmlDeps.length > 0) {
      for (const tok of fkDeps) {
        const dependency = resolveFormImport(canonical, tok);
        await loadFormSourceFile(dependency, dependency, seen, parts);
      }
      for (const tok of bmlDeps) {
        const dependency = resolveFormImport(canonical, tok);
        await loadFormSourceBmlPrelude(dependency, dependency, seen, parts);
      }
    }
    body.push(line);
  }
  parts.push({ path: displayPath, source: body.join("\n") });
}

async function loadFormSourceClosure(paths: string[]): Promise<FormSourcePart[]> {
  const seen = new Set<string>();
  const parts: FormSourcePart[] = [];
  for (const path of paths) {
    await loadFormSourceFile(path, path, seen, parts);
  }
  return parts;
}

function setCrashTraceContext(mode: string, args: string[], source?: string): void {
  crashTraceContext.mode = mode;
  crashTraceContext.args = [...args];
  if (source !== undefined) crashTraceContext.source = source;
}

function sourceLineCount(source: string): number {
  return source.length === 0 ? 0 : source.split("\n").length;
}

async function writeKernelCrashTrace(err: unknown): Promise<string | null> {
  const dir = join(".cache", "form-kernel-ts");
  try {
    await mkdir(dir, { recursive: true });
  } catch {
    return null;
  }
  const when = new Date();
  const safeStamp = when.toISOString().replace(/[:.]/g, "");
  const path = join(dir, `crash-${safeStamp}-${process.pid}.json`);
  const message = err instanceof Error ? err.message : String(err);
  const stack = err instanceof Error ? err.stack : undefined;
  const source = crashTraceContext.source;
  const report = {
    when_utc: when.toISOString(),
    pid: process.pid,
    mode: crashTraceContext.mode,
    args: crashTraceContext.args,
    error: message,
    source_bytes: Buffer.byteLength(source, "utf8"),
    source_line_count: sourceLineCount(source),
    source_head: source.slice(0, 2000),
    source_tail: source.slice(Math.max(0, source.length - 2000)),
    js_stack: stack ?? null,
    // Innermost frame first — the Form-level call chain live at the crash.
    form_stack: crashKernel === null ? [] : [...crashKernel.formStack].reverse(),
  };
  try {
    await writeFile(path, `${JSON.stringify(report, null, 2)}\n`);
    return path;
  } catch {
    return null;
  }
}

async function main(): Promise<void> {
  const args = cliArgs();
  setCrashTraceContext("startup", args);
  if (args.length === 0) {
    console.error(
      "usage: tsx src/main.ts (--binary file.fkb | --emit-binary out.fkb file.fk... | --expr <expr> | --bench | --compiled <expr> | trace ... | <file.fk>)",
    );
    process.exit(2);
  }

  if (args[0] === "--bench") {
    runBench();
    return;
  }

  if (args[0] === "--numeric-bench") {
    runNumericBench();
    return;
  }

  if (args[0] === "trace") {
    await runTrace(args.slice(1));
    return;
  }

  const k = new Kernel(createNodeKernelHost());
  crashKernel = k;
  // Install the Form→host-JS JIT hook so (jit_compile "name") from Form
  // code compiles the named closure's body through compiler.ts.
  k.jitCompileHook = compileNode;
  const frame = new Frame(null);

  if (args[0] === "--binary") {
    const path = args[1];
    if (path === undefined) {
      console.error("--binary requires a path");
      process.exit(2);
    }
    setCrashTraceContext("binary", args);
    const root = deserializeRecipeArtifact(k, await readFile(path));
    k.setActiveRoots([root]);
    const value = walk(k, root, frame);
    k.substrateGC([value], frame);
    console.log(k.render(value));
    return;
  }

  if (args[0] === "--emit-binary") {
    const outPath = args[1];
    const paths = args.slice(2);
    if (outPath === undefined || paths.length === 0) {
      console.error("--emit-binary requires an output path and one or more .fk files");
      process.exit(2);
    }
    const src = (
      await Promise.all(paths.map((path) => readFile(path, "utf8")))
    ).join("\n");
    setCrashTraceContext("emit-binary", args, src);
    const node = readAll(k, src);
    await writeFile(outPath, serializeRecipeArtifact(k, node));
    return;
  }

  if (args[0] === "--expr") {
    const expr = args[1];
    if (expr === undefined) {
      console.error("--expr requires an argument");
      process.exit(2);
    }
    setCrashTraceContext("expr", args, expr);
    const node = readForm(k, expr);
    k.setActiveRoots([node]);
    const value = walk(k, node, frame);
    k.substrateGC([value], frame);
    console.log(k.render(value));
    return;
  }

  if (args[0] === "--compiled") {
    const expr = args[1];
    if (expr === undefined) {
      console.error("--compiled requires an argument");
      process.exit(2);
    }
    setCrashTraceContext("compiled", args, expr);
    const node = readForm(k, expr);
    const compiled = compileNode(k, node);
    const value = compiled(frame);
    console.log(k.render(value));
    return;
  }

  const paths = args;
  if (paths.length === 0) {
    console.error("missing source file");
    process.exit(2);
  }
  // Pre-flight: a missing input path is a caller error (usually a wrong-relative
  // path), not a kernel fault. Fail with a fat, attributed error and a clean exit
  // BEFORE the Promise.all below — otherwise readFile rejects with a bare ENOENT
  // that reaches main()'s catch and writes a crash-trace, hiding which arg was
  // wrong behind a Node stack. Kernel input paths resolve relative to form/.
  const missingInputs = paths
    .map((path, i) => ({ path, i }))
    .filter(({ path }) => !existsSync(path));
  if (missingInputs.length > 0) {
    for (const { path, i } of missingInputs) {
      console.error(
        `form-kernel-ts: input file not found (arg ${i + 1}/${paths.length}): ${path}\n` +
          `  cwd ${process.cwd()} — kernel input paths resolve relative to the form/ ` +
          `directory (e.g. form-stdlib/core.fk, not form/form-stdlib/core.fk).`,
      );
    }
    process.exit(2);
  }
  const loaded = await loadFormSourceClosure(paths);
  const parts = loaded.map((part) => part.source);
  // Line map: each file's first global line in the joined source, so
  // read-time attribution names the ORIGINAL file:line (+1 per join newline).
  let nextLine = 1;
  for (let i = 0; i < loaded.length; i++) {
    k.readingFiles.push({ file: loaded[i]!.path, startLine: nextLine });
    nextLine += (parts[i]!.match(/\n/g)?.length ?? 0) + 1;
  }
  const src = parts.join("\n");
  setCrashTraceContext("source", args, src);
  const node = readAll(k, src);
  k.readingFiles = [];
  k.setActiveRoots([node]);
  const value = walk(k, node, frame);
  k.substrateGC([value], frame);
  console.log(k.render(value));
}

// runTrace — execute with arm-dispatch tracing enabled, emit JSON report
// with the result, elapsed time, and per-arm dispatch counts including
// native Blueprint attribution. Sibling-parity with Rust/Go kernels.
async function runTrace(args: string[]): Promise<void> {
  if (args.length === 0) {
    console.error("usage: tsx src/main.ts trace [--expr <expr> | <file.fk>]");
    process.exit(2);
  }
  let src: string;
  if (args[0] === "--expr") {
    if (args[1] === undefined) {
      console.error("--expr requires an argument");
      process.exit(2);
    }
    src = args[1];
  } else {
    src = await readFile(args[0]!, "utf8");
  }
  setCrashTraceContext("trace", args, src);

  const k = new Kernel(createNodeKernelHost());
  crashKernel = k;
  // Install the Form→host-JS JIT hook so (jit_compile "name") from Form
  // code compiles the named closure's body through compiler.ts.
  k.jitCompileHook = compileNode;
  k.trace = new Trace();
  const frame = new Frame(null);
  const node = readAll(k, src);
  k.setActiveRoots([node]);
  const start = process.hrtime.bigint();
  const value = walk(k, node, frame);
  k.substrateGC([value], frame);
  const elapsedNs = Number(process.hrtime.bigint() - start);

  const report = {
    result: k.render(value),
    elapsed_us: Math.round(elapsedNs / 1000),
    elapsed_human: `${(elapsedNs / 1000).toFixed(2)}µs`,
    trace: k.trace.toJSON(),
  };
  console.log(JSON.stringify(report, null, 2));
}

function runKernelCli(): void {
  main()
    .then(() => {
      // Terminate worker-backed native carriers so the process exits promptly;
      // socket net handles and HTTP worker state otherwise keep the loop alive.
      crashKernel?.shutdown();
    })
    .catch(async (err: unknown) => {
      const msg = err instanceof Error ? err.message : String(err);
      console.error(`form-kernel-ts: ${msg}`);
      // The Form-level call chain live at the crash, innermost first — the
      // line that produced the fatal is the innermost attributed frame.
      const formStack = crashKernel?.formStackDisplay(16) ?? "";
      if (formStack !== "") {
        console.error(`form-kernel-ts: form stack: ${formStack}`);
      }
      const tracePath = await writeKernelCrashTrace(err);
      if (tracePath !== null) {
        console.error(`form-kernel-ts: crash trace: ${tracePath}`);
      }
      crashKernel?.shutdown();
      process.exit(1);
    });
}

// The Form walk is plain recursion in walk()/walkFnCall — a source-length
// call chain (flt-scan advances one token per nested cycle), far past any
// main-thread stack the OS grants. A --stack-size flag cannot grow the real
// stack: a V8 limit set ABOVE it disables V8's overflow check and turns
// deep recursion into a SILENT SIGSEGV — zero output, and rc=139 masks to
// rc=0 through a pipeline (the aphonia family,
// receipts/2026-07-17-regen-lane-aphonic-carrier.md). The honest carrier
// mirrors the emitted C walker's stack door: FORM_KERNEL_STACK_MB names the
// stack, the CLI re-enters itself on a worker thread whose V8 limit MATCHES
// its real stack (Node derives both from resourceLimits.stackSizeMb), and
// overflow surfaces as a catchable RangeError -> loud rc=1 with the Form
// stack attributed.
const KERNEL_WORKER_MARKER = "form-kernel-ts:deep-stack-worker";

type KernelWorkerData = { marker: string; argv: string[] };

function isKernelWorker(): boolean {
  return (
    !isMainThread &&
    (workerData as KernelWorkerData | undefined)?.marker === KERNEL_WORKER_MARKER
  );
}

// CLI arguments: a deep-stack worker receives them via workerData —
// process.argv does not carry the parent CLI's arguments across the
// worker boundary.
function cliArgs(): string[] {
  if (isKernelWorker()) return (workerData as KernelWorkerData).argv;
  return process.argv.slice(2);
}

function kernelStackMb(): number {
  const raw = Number(process.env["FORM_KERNEL_STACK_MB"] ?? "");
  return Number.isFinite(raw) && raw >= 1 ? Math.floor(raw) : 2048;
}

function runOnDeepStack(): void {
  let online = false;
  const worker = new Worker(new URL(import.meta.url), {
    workerData: {
      marker: KERNEL_WORKER_MARKER,
      argv: process.argv.slice(2),
    } satisfies KernelWorkerData,
    resourceLimits: { stackSizeMb: kernelStackMb() },
    // Inherited --stack-size flags would re-lift the worker's V8 limit away
    // from its real stack and re-open the silent-SIGSEGV door; scrub them,
    // keep everything else (loader registrations ride execArgv).
    execArgv: process.execArgv.filter((a) => !/^--stack[-_]size/.test(a)),
  });
  worker.on("online", () => {
    online = true;
  });
  worker.on("error", (err: unknown) => {
    const msg = err instanceof Error ? err.message : String(err);
    if (!online) {
      // The worker never came up (a host without worker-loader support) —
      // fall back to the historical main-thread walk. V8's default limit
      // sits far below the real main stack, so overflow stays a loud
      // RangeError here, only capacity shrinks.
      console.error(
        `form-kernel-ts: deep-stack worker unavailable (${msg}); walking on the main thread`,
      );
      runKernelCli();
      return;
    }
    console.error(`form-kernel-ts: worker: ${msg}`);
    process.exitCode = 1;
  });
  worker.on("exit", (code) => {
    if (code !== 0 && process.exitCode === undefined) process.exitCode = code;
  });
}

if (isKernelWorker()) {
  runKernelCli();
} else {
  runOnDeepStack();
}
