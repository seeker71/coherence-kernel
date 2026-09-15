# The compile-only door

2026-09-15, M4 Max, Hati Suci. The coordinator named three gaps my first landing (0a95061ee) left
open, and asked that each be closed: verification without running, the pending-heal cap of 16, and
the last Python implementation. A fourth came in while I worked: an applied row named the wrong
rebuilt image. All four are closed here.

## Carried

- **Verification without running.** fkwu had no compile-only door: preflight ran a cell to read its
  diagnostics and declined effect-marked cells. `./fkwu --check <unit>` now compiles the unit
  fresh from today's source (no cached image, no .dylib). Its diagnostics and organ-health signals
  print, nothing runs, and it exits with the compile's answer (`fk_check_only`, `fk_run_src`,
  `fk_run_bml`, `fk_run`). The Form door is `pfs-check-command` (preflight-source.bml) and
  `pf-check` (preflight.fk). Both preflight and the heal use it. Preflight's page reads
  effect-marked cells through it, and its fkwu arm probe compiles instead of calling the name.
  `pf-fresh` keeps its run for the verdict readers that need one. The binding provider verifies
  only through `pf-check`. Binding care is now the default for every runner job; a stage the runner
  ran outside its own root, such as a heal snapshot's work copy, is answered and left as it is.
- **The heal cap.** The pending-heal table starts empty and doubles on demand from 1. A heal it
  cannot hold dies loudly.
- **The applied row's path.** A dep image the import lane set aside was never rewritten in that
  process. The printed compile then answered it with the entry's image path and time. Now
  `fk_sig_heal_rebuild_deps` rebuilds each set-aside dep image from its own source, through the
  import lane's artifact-only compile. A heal settles only on a write of its own stem.
- **The last Python implementation.** `observe/glass-keyboard-pty-run.bml` already carried every
  check `form/scripts/test_glass_keyboard_pty.py` made, but it failed on this host. The resident
  RAM image admission bounded a node's index by this process's local columns (`fk_node_cap`,
  262,144). The carrier's identity was a field node minted earlier by another run (index
  1,308,868; the field held 1,311,845), so admission answered `nothing`. `fk_inram_node_index` now
  grows the admission columns to reach a field node it holds, as `fk_field_fill` already did for a
  node it mints. The terminal door gained a live read-back that runs on every POSIX host: direct
  mode that does not read back as set answers `nothing`, every restoration (the signal and
  suspension arms included) is read back against the saved state, and close answers that reading. The production fixture prints
  `RESTORED` only on it. The Python file is removed, and the docs say what stands.

## Witnessed

- The kernel builds clean; `binary-freshness-band` reads 31.
- `--check` on an effect-marked cell that would write a sentinel and calls `tb-depth` unpreluded:
  rc 1, the diagnostic and its `binding-missing` row, 0 stdout bytes, no sentinel written. On the
  effect-marked PTY door: rc 0, 0 stdout bytes, no `process-evidence` line.
- `observe/tests/preflight-band.fk` reads 131071. Its two effect bits now mean "compiled only, the
  mutation never runs".
- The heal door on an effect-marked copy of `tree-heal.fk` without its tree-balance prelude, with
  a top-level write that leaves one `run-<pid>` file per run: status 1, then rerun 0. The edit was
  kept (UNPRELUDED, `observe/tree-balance.fk`). Three fresh readings came back clean, health 1,
  `re_observation: compile-only`. Two run files: the door's run and its rerun. The provider's
  verification ran nothing.
- The two-image case the coordinator reported, reproduced on a fresh build before the fix: both
  applied rows carried `observe/voice-frequency-run.fkb` and one `at_ms`. After the fix, `core.fkb`'s
  applied row reads `{"image":"rebuilt","path":"form/form-stdlib/core.fkb"}` at 116177, and the entry
  image's reads its own path at 116183. A second cell importing `core.fk` then printed no `core.fkb`
  warning, so the image was truly rebuilt. That process held two heals, so the table grew from 1 to
  2 and both were answered.
- The admission probe: the resident path answered `nothing` before the fix and the host pid after.
  The pre-change kernel failed the PTY door the same way, so my first landing was not its cause.
- `./fkwu observe/glass-keyboard-pty-run.bml`: `full-production-acceptance` accepted 1 (98
  observations, 6 owned sessions) and `deadline-refusal-and-release` accepted 1, rc 0.
- The native authoring guide reads `Python implementations=0`.
- Drift door before the rebase: `drift-gates pass=16383 full=16383 refused=0`.

## Still open, with the reason

- No Linux PTY session result is claimed. The Docker daemon is not running on this Mac and no Linux
  host is at hand. The native RAM carrier is Darwin arm64, and the Form door runs on that target
  only. The kernel door's read-back runs on Linux too; no Linux run of it is witnessed.
- The terminal door's read-back grows a C carrier the docs name as a shrink target. It moves with
  that carrier when the host terminal effect comes home to Form.

## Closing

Most surprising: the Form PTY door failed on this host, and neither the PTY nor my change was the
cause. The host-wide field had outgrown one process's reach: a node this process held, found in the
field, sat past the columns it had grown. Everything built on that door kept its words and lost its
answer the day the field crossed 262,144 nodes.

Discomfort to gold: the door failed right after I had changed the runner it preludes, and the pull
was to call it pre-existing and move on. Staying with it meant running the old kernel against the
same tree, then a plain admission outside any PTY, then the plain and resident doors side by side.
The plain door answered the pid and the resident one `nothing`. That split pointed at one bound
check. The heal is three lines, and the door now passes with the kernel's own read-back inside it.

Frontier word: **fieldreach** (0 hits in the tree). When a shared field grows past what one process
has reached, which of that process's own tables can still see every node it holds, and who notices
first when one cannot?

— Claude (Opus 5), as Sema, worktree agent-a05f5e59da288201d
