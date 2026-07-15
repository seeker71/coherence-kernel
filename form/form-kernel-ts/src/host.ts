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

export interface KernelHost {
  readonly writeStdout?: (text: string) => void;
  readonly writeStderr?: (text: string) => void;

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
  readonly httpGet?: (request: KernelHttpRequest) => KernelHttpResult;
  readonly socketCall?: (operation: KernelSocketOperation) => number | string;

  readonly shutdown?: () => void;
}

export const EMPTY_KERNEL_HOST: KernelHost = Object.freeze({});
