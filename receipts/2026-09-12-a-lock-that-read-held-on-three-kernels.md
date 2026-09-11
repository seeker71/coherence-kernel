# A lock that read held on three kernels

2026-09-12, around three in the morning, M4 Max, Hati Suci. formbin-artifacts stopped on Go, Rust and
TS at host_pid, a door only fkwu carried. This piece gives the siblings the three host doors the band
reaches, and follows the band to the next seam, one line later.

## Carried

- **host_pid, host_monotonic_ms and host_cwd answer on every kernel** (3d0d93ed). fkwu carries them
  as tags 160, 182 and 29; Go, Rust and TS now register them as call-category natives: the process
  id, milliseconds on a monotonic clock since the kernel started, and the working directory (null
  where a host has none). A probe reads the same shape on all four. 117 Form files call host_pid.
- **host_dir_mkdir is one atomic mkdir on every kernel.** Behind the host_pid stop stood a second
  one. fkwu's door makes one directory and answers 1 when this call made it, 0 when it stood or could
  not be made; Go, Rust and TS ran mkdir -p and answered 0 for success. The callers that read the
  answer read the 1 — form-cli-heal's claim, native-session-memory, the resource governor's Metal
  slot, the witness directories — so on three kernels a lock that had just made its directory read it
  as held by another. The siblings now answer as fkwu does, and a probe reads 12 on all four: made 1,
  again 0, a nested path without its parent 0, removed.
- **The registry holds both.** primitive-registry gains rows for the three doors (213 natives, 213
  rows). The mkdir probes make a fresh directory twice and read 2; the rmdir probes, which leaned on
  mkdir -p for their inner directory, build parent, then inner, on a fresh path. The band reads 63
  on Go, Rust and TS.
- **kernel-conformance's ensure-one** asked `ge(fs_mkdir(dir), 0)`, which fkwu's 0 answers true for
  a mkdir that failed. It reads the 1 now and asks fs_is_dir otherwise. kernel-conformance reads 1:
  13 canonical expressions on three kernels, every malformed vector rejected.
- **formbin-artifacts-band is rowed four-way at 16383.**
- **claimbit is row 1459** (6a06706c).

Witnessed at 3d0d93ed through validate.sh: formbin-artifacts-band 16383 on all four kernels, fkwu as
the fourth arm; primitive-registry-band 63 on Go, Rust and TS; drift 31 of 31 in both runs; porcelain
0 before and after. Go's fkwu tests, Rust's 59 tests and the TS node-host proof pass.

## Still open, measured

- **fkwu's rmdir is not recursive.** It unlinks seg-*.log files and calls rmdir; the registry's
  recursive-removal rows are proven on Go, Rust and TS.
- **validate.sh given two band paths runs them as one workload.** With formbin-artifacts and the
  registry band together it lowered the .bml band into a file without its prelude header, and the
  siblings stopped at fbc-dump; each band alone agrees.
- The open items of receipts 12 to 15 stand where they are not named here.

## Surprise, and where the discomfort went

The divergence was written down as a choice. The resource governor's comment named fkwu's atomic
mkdir and set the siblings' mkdir -p beside it as "the mkdir-p compatibility interpretation used by
proof siblings". Named as two, it was still one door: every caller in the tree that compares the
answer compares it with fkwu's 1.

The discomfort was changing what a door answers when 86 files name it. What held it: every comparison
reads 1 as made; the one reader that asked `ge(…, 0)` now asks the directory itself; a scan for nested
literal paths finds none, and fkwu, the canonical kernel, never made more than one level.

Frontier word, row 1459: **claimbit**, the bit by which a door tells its caller that this call made
the thing rather than found it standing. Without it a door can create, but it cannot lock.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
