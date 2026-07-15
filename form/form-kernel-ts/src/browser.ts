// Canonical browser entry for the TypeScript proof sibling.
//
// This entry is the only browser implementation: Kernel, reader, binary
// codec, and Field Model Form all come from their canonical source modules.
// Node effects are injected through KernelHost and no Node adapter enters this
// graph.

import {
  Frame,
  Kernel,
  Trace,
  nodeKey,
  walk,
  type NodeID,
} from "./kernel.ts";
import { readAll } from "./reader.ts";
import type { KernelHost } from "./host.ts";

export * from "./host.ts";
export * from "./kernel.ts";
export * from "./reader.ts";
export * from "./field.ts";

export interface LocalFormTrace {
  readonly total_walks: number;
  readonly arms: readonly {
    readonly arm_ty: number;
    readonly arm_name: string;
    readonly count: number;
  }[];
  readonly variants: readonly {
    readonly arm_ty: number;
    readonly arm_inst: number;
    readonly arm_name: string;
    readonly arm_variant_name: string;
    readonly count: number;
  }[];
  readonly choice_attempts: number;
  readonly choice_successes: number;
  readonly choice_failures: number;
  readonly choice_success_rate: number;
}

export interface LocalFormRun {
  readonly source: string;
  readonly result: string;
  readonly root: string;
  readonly rootCategory: string;
  readonly stdout: string;
  readonly stderr: string;
  readonly elapsedMs: number;
  readonly trace: LocalFormTrace;
}

function formatNodeID(node: NodeID): string {
  return `@${nodeKey(node)}`;
}

export function runLocalFormBinary(
  source: string,
  host: KernelHost = {},
): LocalFormRun {
  const stdout: string[] = [];
  const stderr: string[] = [];
  const kernel = new Kernel({
    ...host,
    writeStdout: (text) => {
      stdout.push(text);
      host.writeStdout?.(text);
    },
    writeStderr: (text) => {
      stderr.push(text);
      host.writeStderr?.(text);
    },
  });
  kernel.trace = new Trace();
  const start = globalThis.performance.now();
  const root = readAll(kernel, source);
  const value = walk(kernel, root, new Frame(null));
  const elapsedMs = globalThis.performance.now() - start;
  return {
    source,
    result: kernel.render(value),
    root: formatNodeID(root),
    rootCategory: formatNodeID(kernel.category(root)),
    stdout: stdout.join(""),
    stderr: stderr.join(""),
    elapsedMs,
    trace: kernel.trace.toJSON() as unknown as LocalFormTrace,
  };
}
