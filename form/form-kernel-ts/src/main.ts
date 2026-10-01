// form-kernel-ts CLI. It reads plain Form files as given and follows no
// directive; a unit's whole closure comes from `./fkwu --closure <unit> <out>`
// run at the repo root.
//
// Usage:
//   tsx src/main.ts --binary file.fkb
//   tsx src/main.ts --emit-binary out.fkb file.fk...
//   tsx src/main.ts --expr "(add 1 2)"
//   tsx src/main.ts file.fk...

import { mkdir, readFile, writeFile } from "node:fs/promises";
import { existsSync } from "node:fs";
import { join } from "node:path";
import { totalmem } from "node:os";
import { isMainThread, Worker, workerData } from "node:worker_threads";
import {
  deserializeRecipeArtifact,
  Frame,
  Kernel,
  serializeRecipeArtifact,
  type Value,
  walkUnit,
} from "./kernel.ts";
import { createNodeKernelHost } from "./node-host.ts";
import { textToBstr } from "./byte-host.ts";
import { readAll, readForm } from "./reader.ts";

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
    source_bytes: Buffer.byteLength(source, "latin1"),
    source_line_count: sourceLineCount(source),
    source_head: source.slice(0, 2000),
    source_tail: source.slice(Math.max(0, source.length - 2000)),
    js_stack: stack ?? null,
    // Innermost frame first — the Form-level call chain live at the crash.
    form_stack: crashKernel === null ? [] : crashKernel.formStackLabels().reverse(),
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
      "usage: tsx src/main.ts (--binary file.fkb | --emit-binary out.fkb file.fk... | --expr <expr> | file.fk...); a unit's closure as one plain Form file: ./fkwu --closure <unit> <out> (repo root)",
    );
    process.exit(2);
  }

  const k = new Kernel(createNodeKernelHost());
  crashKernel = k;
  const frame = new Frame(null);

  if (args[0] === "--binary") {
    const path = args[1];
    if (path === undefined) {
      console.error("--binary requires a path");
      process.exit(2);
    }
    setCrashTraceContext("binary", args);
    const root = deserializeRecipeArtifact(k, await readFile(path));
    writeResult(k, walkUnit(k, root, frame));
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
      await Promise.all(paths.map((path) => readFile(path, "latin1")))
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
    // argv arrives as text; the kernel's strings are its UTF-8 bytes (byte-host.ts)
    const src = textToBstr(expr);
    setCrashTraceContext("expr", args, src);
    writeResult(k, walkUnit(k, readForm(k, src), frame));
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
  const parts = await Promise.all(paths.map((path) => readFile(path, "latin1")));
  // Line map: each file's first global line in the joined source, so
  // read-time attribution names the file:line (+1 per join newline).
  let nextLine = 1;
  for (let i = 0; i < paths.length; i++) {
    k.readingFiles.push({ file: paths[i]!, startLine: nextLine });
    nextLine += (parts[i]!.match(/\n/g)?.length ?? 0) + 1;
  }
  const src = parts.join("\n");
  setCrashTraceContext("source", args, src);
  const node = readAll(k, src);
  k.readingFiles = [];
  writeResult(k, walkUnit(k, node, frame));
}

// The result line is the value's own bytes, written the way print writes them (byte-host.ts).
function writeResult(k: Kernel, value: Value): void {
  k.host.writeStdout?.(`${k.render(value)}\n`);
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
// rc=0 through a pipeline (the aphonia family). The honest carrier
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

// V8 caps an isolate's heap near 4 GiB whatever the machine holds, while the Go
// and Rust kernels grow until the host says no. TS's lists copy on tail as
// Rust's do, so a band can hold more than V8's cap; the worker's heap may take
// half of physical memory, and running out of it is still a loud rc=1.
function kernelHeapMb(): number {
  return Math.floor(totalmem() / 1048576 / 2);
}

function kernelWorker(execArgv: string[]): Worker {
  return new Worker(new URL(import.meta.url), {
    workerData: {
      marker: KERNEL_WORKER_MARKER,
      argv: process.argv.slice(2),
    } satisfies KernelWorkerData,
    resourceLimits: { stackSizeMb: kernelStackMb(), maxOldGenerationSizeMb: kernelHeapMb() },
    execArgv,
  });
}

function runOnDeepStack(): void {
  let online = false;
  // Inherited --stack-size flags would re-lift the worker's V8 limit away
  // from its real stack and re-open the silent-SIGSEGV door; scrub them,
  // keep everything else (loader registrations ride execArgv).
  const inherited = process.execArgv.filter((a) => !/^--stack[-_]size/.test(a));
  let worker: Worker;
  try {
    worker = kernelWorker(inherited);
  } catch {
    // a per-process flag (--prof, a log file) a worker refuses stays with this process;
    // the worker keeps only the loader registrations
    worker = kernelWorker(inherited.filter((a) => /^--(import|loader|experimental-|require|conditions)/.test(a)));
  }
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
