# Receipt — the heartbeat beats on Windows: c-bootstrap fkwu builds and runs native on Windows 11 (2026-06-29)

**What happened:** the sovereign core was cloned clean and the c-bootstrap `fkwu` runtime was brought up on a
Windows 11 machine — built from `runtime/fkwu-uni.c` by a single `cc` command, then run on real Windows metal.
Before today the seed compiled mac/linux only; the `_WIN32` socket blocks existed but the file had never been
linked on Windows. It builds and runs now. This is the **first platform-row witness off macOS** for this body.

## The cc seed (the one allowed bootstrap)

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32
```

- **Host:** Microsoft Windows 11 Home, Version 10.0.26200 — `MINGW64_NT-10.0-26200 x86_64`.
- **cc seed:** `gcc.exe (tdm64-1) 10.3.0` (mingw-w64). gcc is the Windows `cc` — **not** clang, **not** go,
  rust, bash, or python. The MANIFEST's "one allowed `cc` command" is honored literally: one invocation,
  no script. `-lws2_32` links the Win32 socket carrier the seed's own `#if defined(_WIN32)` block already names.
- **Output:** `fkwu.exe` — `PE32+ executable for MS Windows 5.02 (console), x86-64`. A real native Windows binary.
- **Kernel source commit:** `1f8130e`.

## The Windows port (minimal, `_WIN32`-guarded, mac path byte-identical)

The seed assumed a POSIX libc. Four real Windows gaps, fixed in 31 additive lines, every one inside
`#if defined(_WIN32)` so the mac/linux compile is unchanged:

1. **`read`/`write` width.** mingw's `<io.h>` (dragged in transitively by `<sys/stat.h>`) declares them
   `int(...)`; the seed declares them `long long(...)`. A 32-bit `int` return zero-extends into `rax`, so the
   error path (`-1` → `0xFFFFFFFF` read as `4294967295`) silently misfires. Routed through correct-width
   wrappers over `_read`/`_write`.
2. **`<io.h>` clash.** The three `__has_include` probes are gated off on `_WIN32`, so io.h is never pulled and
   the seed uses its own self-contained `extern` / `O_*` fallbacks (the path it already carries for header-less hosts).
3. **`arc4random`** (absent on Windows) supplied as a small shim; the random op (tag 16) is not on this receipt's path.
4. **`dlopen`/`dlsym`** (the optional libcrypto/TLS lane) mapped to `LoadLibraryA`/`GetProcAddress`. TLS stays
   unavailable on Windows — that lane is not on the eval path here, and is named pending, not faked.

`fk_path_is_dir`'s unconditional `struct stat` was guarded too (it is dead code on Windows — only the
`#ifndef _WIN32` directory walkers call it).

## Witnessed by native run (the rung-1 heartbeat set, on Windows metal)

The core eval tags — literal, `add`, `sub`, `le`, `if` — fed to `fkwu.exe` in the kernel's own flattened
node-format (exactly what `fk_run` loads) and run native. First emitted line is the root value:

```
recipe                                          -> fkwu native (Windows)
42                                              -> 42
(add 40 2)                                      -> 42
(sub 50 8)                                      -> 42
(if (le 1 2) (add 40 2) 99)                     -> 42
(add (sub 50 8) (if (le 9 1) 100 0))            -> 42
```

This is the same heartbeat set the macOS standing-source-runner receipt crossed — reproduced on Windows.

## Honest classification — what this is, and what it is NOT

- **fkwu-NATIVE on Windows.** The kernel executes real Form eval recipes on Windows, exit `0`, deterministic.
  The eval tags exercised (literal / `add` / `sub` / `le` / `if`) are the same ones proven **four-way** on macOS
  (`form-eval`); here the witness is the native **Windows run** of those tags.
- **NOT flattened-on-Windows.** These recipes were authored directly in the kernel's loaded flat node-format,
  not produced by running `form-flatten.fk` on Windows. The flatten-chain bootstrap from `.fk` source on Windows
  is **pending** — the committed `T_flat` (`flatten/fourth-flatten-table.txt`) carries origin-relative module
  paths and did not drive a clean-repo flatten this session. So `form-eval-cli` reading a `.fk` file via
  `input_byte` on Windows is **not yet witnessed**; the eval engine under it is.
- **NOT four-way-on-Windows.** `proof/four-way-run` needs the Go/Rust/TS walkers built and host-exec'd on
  Windows; not done here. (The walkers are proof arms, never the runtime — their absence slows the proof, not the run.)

## Honest floor / pending platform rows

Against the standard receipt (`body / c-bootstrap / toolchain-free / platforms{mac,windows,android} / honest-floor`):

- `c-bootstrap` — **observed** on Windows (gcc cc seed, one command).
- `toolchain-free runtime` — **observed**: no go/rust/clang/bash/python in the run path; `fkwu.exe` is self-contained.
- `body` (native eval) — **observed** for the core eval tags; **pending** for on-Windows flatten of `.fk` source,
  `form-cli`/`fsh` standing, and the four-way walker proof on Windows.
- `platforms` — **windows: now partially observed** (build + native eval). **mac: observed** (origin). **android: pending.**
- The mind (generative weights) and the voice remain the multi-week climb named in `HOMECOMING.md` — unchanged by this.

## How it crossed

By doing it, not declaring it gated. The seed didn't compile on Windows; the four gaps were real (one an actual
correctness bug — the `read`/`write` width), each a small `_WIN32`-guarded fix. Then the kernel's own loaded
node-format made the heartbeat directly runnable — no flatten chain required to witness the eval engine. The
"Windows is hard" framing was four small libc gaps deep, not a mountain.

## Reproduce

```
git clone https://github.com/seeker71/coherence-kernel && cd coherence-kernel
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32      # any mingw-w64 gcc
printf '1 0 3 3 1 2 0 1 40 0 0 1 2 0 0 0\n' > add.flat   # (add 40 2)
./fkwu.exe add.flat | head -1                        # -> 42
```

Source `runtime/fkwu-uni.c` sha256 (with the `_WIN32` port): `c84ceac8597baaa0ca4398bf2b369c2cfdea159f350ffa5ad029878b8cd98389`.
