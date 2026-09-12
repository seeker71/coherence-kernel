# The road without a compiler

2026-09-12, around noon, M4 Max, Hati Suci. Receipt 39 left the platform carrier trailing the bootstrap
it serves, form-cli-darwin-arm64 still holding its 2026-08-25 identity.

## Carried

- **The standard lane builds again** (cbb7341ce). build-form-cli.sh's standard lane, the road with no Go
  and no clang, copies the committed carrier only when its stamp matches the table's. It answered
  "bootstrap form-cli-darwin-arm64 missing or stale" and exited 1. The carrier's stamp was last written
  on 2026-08-25, and none of the 16 table stamps committed since has matched it. The fkwu platform
  binary trailed the same way, holding 7b8cf7c523edc0da where the fourth arm now wants
  0a0caa6f83242129. scripts/regen_standard_lane_binaries.sh built fkwu from the current bootstrap
  uni.c and linked and attested the form-cli carrier from the table receipt 39 regenerated, in 29 s:
  fkwu-darwin-arm64 at 108320 bytes (stamp 0a0caa6f83242129) and form-cli-darwin-arm64 at 6696536
  bytes (stamp 7d9d241f8618dbd2). The tracked form/form-cli carries the carrier's bytes.
- **Readings.** With FORM_STANDARD_LANE=1, build-form-cli.sh copies the carrier with no compiler, and
  the copy answers ping with pong. The fourth arm's standard lane copies fkwu from the bootstrap with no
  compile, and that fkwu walks the form-cli table to pong as well. TestFkwu passes.
- **detourgreen is row 1483** (d249ad6c3).

Witnessed at cbb7341ce: both carrier stamps equal to what the lane wants; the standard-lane build and
its pong; go test TestFkwu ok; freshness 31, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **The carriers follow the table only when the bridge runs**: a change to any of the 185 sources
  leaves the table behind until regen_form_cli_bootstrap.sh runs, and the carriers behind until
  regen_standard_lane_binaries.sh does. TestFkwu reads the table and links with clang, so nothing reads
  the standard lane but a build without one.
- **Rust writes "?" for uuid, interval and arrays** (receipt 37).
- **fkwu reads `true` as the integer 1**; its integers wrap at 2^63; max, min and pow have no home
  there.
- **Rust's and TS's socket_recv decode bytes as UTF-8** (textsieve, row 1474).
- **The five vk live lanes** wait on the Vulkan door through host-exec.
- The open items of receipts 12 to 39 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the check watching form-cli passed the whole time the road it guards was closed.
TestFkwu builds with clang, so a stale carrier costs it nothing: it links from the table and carries
on. The standard lane exists for the one who has no compiler, and no check walks it without one.

The discomfort was committing two binaries, 6.7 MB and 108 KB, into history for a lane no test reads,
weight the repository takes on each time the carriers follow the table. What turned it was reading what
the lane is for: build-form-cli.sh's own header names it "no Go, no clang, no shell in the receipt
path". The binaries were already tracked, and the only one who meets a stale carrier is the person the
lane was built for, who met an exit 1.

Frontier word, row 1483: **detourgreen**, a check that stays green by taking a road the one it serves
cannot take.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
