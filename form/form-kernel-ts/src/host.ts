// Host carriers for the TypeScript proof sibling.
//
// The kernel, reader, and Form-domain modules are deliberately platform
// neutral.  External effects arrive through this contract; Node installs the
// adapter in node-host.ts, while browsers can provide only the effects they
// intentionally expose.  An absent carrier is an unavailable capability, not
// an implicit Node fallback.

export interface SourceInventoryEntry {
  readonly path: string;
  readonly lines: number;
}

export interface KernelHttpRequest {
  readonly url: string;
  readonly headers: Readonly<Record<string, readonly string[]>>;
  readonly timeoutMs: number;
}

export interface KernelHttpResult {
  readonly statusCode: number;
  readonly body: string;
  readonly error: string;
  readonly durationMs: number;
  readonly headers: readonly (readonly [number, string, string])[];
}

export type KernelSocketOperation =
  | { readonly op: "listen"; readonly port: number }
  | { readonly op: "port"; readonly h: number }
  | { readonly op: "accept"; readonly h: number }
  | { readonly op: "connect"; readonly host: string; readonly port: number }
  | { readonly op: "send"; readonly h: number; readonly text: string }
  | { readonly op: "recv"; readonly h: number; readonly max: number }
  | { readonly op: "close"; readonly h: number };

// The storage port's operations, as the host's pg carrier takes them: a connection by its parts, one SQL
// run on a connection (params: one text or null per $n, or null for a simple Query), and a close.
export type KernelPgOperation =
  | {
      readonly op: "connect";
      readonly host: string;
      readonly port: number;
      readonly user: string;
      readonly password: string;
      readonly database: string;
    }
  | {
      readonly op: "run";
      readonly h: number;
      readonly sql: string;
      readonly params: readonly (string | null)[] | null;
    }
  | { readonly op: "close"; readonly h: number };

// A pg carrier's answer: error when the server or the connection failed; otherwise the handle of a
// connect, or a run's last field list, every row's cells as the server's text (null for NULL) and the
// last command tag.
export interface KernelPgAnswer {
  readonly error?: string;
  readonly handle?: number;
  readonly fields?: readonly { readonly name: string; readonly oid: number }[];
  readonly rows?: readonly (readonly (string | null)[])[];
  readonly tag?: string;
}

export interface KernelHost {
  readonly writeStdout?: (text: string) => void;
  readonly writeStderr?: (text: string) => void;

  // Where a relative path names a file for a read-side door (see node-host's resolveHostReadPath);
  // a host without one leaves paths as given.
  readonly resolveReadPath?: (path: string) => string;
  readonly readTextFile?: (path: string) => string;
  readonly readBinaryFile?: (path: string) => Uint8Array;
  readonly readBinarySlice?: (
    path: string,
    offset: number,
    length: number,
  ) => Uint8Array;
  readonly writeTextFile?: (path: string, text: string) => void;
  readonly writeBinaryFile?: (path: string, bytes: Uint8Array) => void;
  readonly appendBinaryFile?: (path: string, bytes: Uint8Array) => number;
  readonly fileSize?: (path: string) => number;
  readonly fileMtimeSeconds?: (path: string) => number;
  readonly pathExists?: (path: string) => boolean;
  readonly pathIsDirectory?: (path: string) => boolean;
  // One directory level; throws when the path already stands or its parent is missing.
  readonly makeDirectory?: (path: string) => void;
  readonly removeDirectory?: (path: string) => void;
  readonly removePath?: (path: string) => void;
  readonly renamePath?: (from: string, to: string) => void;
  readonly listDirectory?: (path: string) => readonly string[];
  readonly sourceInventory?: (
    root: string,
    suffix: string,
    skipDirectoryNames: ReadonlySet<string>,
  ) => readonly SourceInventoryEntry[];

  readonly randomBytes?: (length: number) => Uint8Array;
  readonly tempDirectory?: () => string;
  // This process's id, a monotonic millisecond reading and its working directory
  // (host_pid, host_monotonic_ms, host_cwd); a host without them answers null.
  readonly processId?: () => number;
  readonly monotonicMs?: () => number;
  readonly workingDirectory?: () => string;
  // The home directory ($HOME), where the kernel config's own layer stands; "" when unset.
  readonly homeDirectory?: () => string;
  readonly httpGet?: (request: KernelHttpRequest) => KernelHttpResult;
  readonly socketCall?: (operation: KernelSocketOperation) => number | string;
  readonly pgCall?: (operation: KernelPgOperation) => KernelPgAnswer;

  readonly shutdown?: () => void;
}

export const EMPTY_KERNEL_HOST: KernelHost = Object.freeze({});
