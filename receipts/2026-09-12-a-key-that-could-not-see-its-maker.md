# A key that could not see its maker

2026-09-12, around a quarter past eight in the morning, M4 Max, Hati Suci. Receipts 25 to 29
carried Go reading form-cli in 135 s where Rust and TS took 1 s.

## Carried

- **One lowering key on Go, Rust and TS** (77d3dad1). Each sibling lowers a BML prelude by running
  the Form compiler in a throwaway kernel. Go kept nothing, because its own comment said source
  bytes alone cannot identify a lowering; Rust and TS kept every lowering under exactly that key,
  so an edit to the compiler never reached them while the BML stood still. All three now key a
  lowering over the BML's path and bytes, every source in the loaded compiler closure, and the
  sibling's own executable, write it as `<kernel>-<path>-<key>.fk` by renaming it into place, and
  prune their older lowering of the same path. The driver is appended text rather than a file, so
  an interrupted run leaves none behind; 76 stood in this worktree's cache.
- **The cache witness primes its own manifest.** observe/native-source-cache-witness.bml copies the
  prepare door's dependency closure from the door's `.sym`, and a checkout where the door has never
  run holds none: at HEAD the witness died at that first read, `str_len: nothing has no length`,
  before a single case. It runs the door once first now. With the door's image and manifest
  removed, it reads 1023, Go ignoring the planted source-only lowering among its ten bits.
- **Readings.** form-cli-band reads 2097151 on each sibling: Go 127 s cold and 1 s warm, Rust 109 s
  and 0 s, TS 320 s and 1 s. On an entry already in place, each sibling reads it warm under the same
  name; an edit to source-compiler-text-lens.fk gives it a new key, restoring the file brings the
  first key back, and the path holds one entry throughout. The 580 lowerings named by source bytes
  alone, and the 76 drivers, were cleared from this worktree's cache.
- **The doors** that described the old keys, docs/native-source-preparation.md and
  form/kernel-roadmap.md, say what is.
- **makerblind is row 1473** (18286291).

Witnessed on the tree committed as 77d3dad1: validate.sh on form-cli-band 2097151 and on
preflight-source-band 65535, go, rust and typescript exiting 0 and drift 31 of 31 in each; tsc with
no errors; freshness 31, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **Six rowed bands reach a door fkwu does not carry**: pg_exec, pg_query and pg_connect (Go and
  Rust carry them). They read their registered verdicts on fkwu with that call unresolved.
- **The five vk live lanes** read on fkwu apart from their registration, staged on the Vulkan door
  through host-exec, which this host is not carrying now.
- The open items of receipts 12 to 29 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was what the gap between the siblings was made of. Go read form-cli in 135 s where
Rust and TS read it in 1 s, and it looked like a slow kernel. Under a key that sees the compiler,
Rust lowers the same band cold in 109 s and TS in 320 s. The sibling that was slow was the one that
could see what made its answer; the two that were fast were fast because their key could not.

The discomfort was my own TS rewrite. It called stat, rename and readdir without importing them,
esbuild bundled it without a word, and the first witness read TS rc 1 with no entries: `stat is not
defined`. Two things I had taken as checks were not: a bundle that builds, and a witness that had
never run in this checkout. What turned it was letting each one say what it does. tsc reads the
source, and answered no errors once the imports stood; the witness now makes the manifest it reads,
and read 1023 from a checkout without one.

Frontier word, row 1473: **makerblind**, a cache key that names what went in but not what made the
answer, so a change to the maker leaves the old answer standing.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
