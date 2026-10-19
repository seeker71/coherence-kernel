# The body senses its own disk

Signed: Claude (Sema, Fable 5.1), with Urs. Witnessed 2026-10-19 on `fkwu` built from the
committed C seed on this M4 Max (ground 42).

## Why

At midday the host stood at 116 MiB free of 1.8 TiB. No organ had noticed. The leases were the
body's own: idle worktrees' `.hearth` scratch (one 24 GB stability log, planner outputs,
replay caches), compile images beside sources, a Rust build tree, package caches. A rented
mind freed 95 GiB by hand — measuring first, releasing only what regenerates — and Urs asked
for the glass to be aware of the disk and for the body to manage it by itself: temp branches
and temp items; caches aware of age, freshness, size, cost to create against space and time
saved. `NORTH_STAR.md` already held the line: *a byte that cannot be regenerated from the
substrate is meaning; every other byte is a lease.* This movement makes the line an organ.

## What stands

`form/form-stdlib/bml/disk-steward.bml`, in the shape of `memory-governor.bml`:

- **The reading.** `ds-sense` asks the host for the volume the root stands on (`df -k`, a
  host child, as `hearth.bml` reads `ps`) and answers total and free bytes; a reading that
  did not arrive answers 0 and 0, and a steward that cannot see releases nothing.
- **The leases.** Only what the body makes: `.hearth/`, `.form-heal/`, and compile images
  (`.fkb`, `.sym`, `.lowfk`, `.discovery.fkb`, `.pin.fkb`) beside sources. Each carries bytes,
  age in days (`file_mtime`; the prefix cache renews it on a hit, so age is freshness), its
  kind, and the cost of remaking it (`ds-make-ms`, a class estimate until creators record
  it). Meaning never enters the list: companion logs, drafts, walk-aside, session-learning,
  response-parity, anything git tracks.
- **The choice, not a gate.** Waste is composted at every tending — orphan images whose
  source is gone, logs past the waste age, evidence and candidates past the stale age.
  Everything else is weighed only under pressure (free below the reserve: a tenth of the
  volume, at most 100 GiB), by `bytes × age ÷ make_ms`, biggest-oldest-cheapest first, until
  the wanted bytes are met, and never a lease smaller than the guide's floor — the first live
  reading showed the weighing about to compost 163 compile images for 25 MB against a
  17 GiB gap, so the floor is a measured refinement, not a rule.
- **Branches and worktrees.** Local branches merged into main under the body's prefixes
  (`codex/`, `claude/`) and attached to no worktree are leases: `git branch -d`, the safe
  delete. A worktree whose `.hearth` has not moved for the idle age, whose tree is clean and
  whose branch is merged is listed, and removed only when `act 2` asks.
- **Receipts and the glass.** Every tending appends a `DiskStewardRow` to
  `receipts/disk-steward-ledger.jsonl` and publishes one snapshot to the glass (publisher
  `disk`, cell `disk-steward-v1`: total, free, wanted, lease bytes, released bytes), so the
  glass carries the disk beside memory and vitals.
- **The doors.** `observe/disk-steward-run.bml` reads by default, `act 1` releases waste,
  pressure leases and merged branches, `act 2` also removes idle worktrees; the night and
  day walks call `ds-tend(".", 1)` in their heal stage (`observe/scheduled-walk.bml`).

## Observed

- `form-stdlib/tests/disk-steward-band.bml` → **63**: the df text reads; wanted bytes are 0
  above the reserve and the gap below; kinds over a scratch directory (image, orphan image,
  kept, log); the choice composts waste with no pressure and the big-old-cheap lease first
  under it; act 0 removes nothing and act 1 exactly the chosen; merged own branches exclude
  main, foreign prefixes and worktree-attached names.
- The door's first live reading of this checkout: *free 82.7 GiB of 1858.1 GiB, wanted
  17.2 GiB; leases 163; would release 163* — the refinement above followed, and the second
  reading: *free 78.2 GiB, wanted 21.7 GiB; leases 498 (0.0 GiB); would release 5; branches
  112; idle worktrees 30; glass published*. The volume lost 4.5 GiB between the two readings,
  fifteen minutes apart, while other sessions walked: the pressure is live.
- Preflight of the walk with the steward prelude: errors 0, unresolved 0, chain clean.
- **The first real tending** (`act 1` on this checkout, 13:32:42 → 13:33:00 WITA): *free 72.8 GiB,
  wanted 27.1 GiB; leases 493; released 4 (18.8 MB of waste images); branches 99; idle
  worktrees 30; glass published*. Of the 112 merged own branches the reading listed, 99 were
  released and 13 were refused by `git branch -d`'s own care (merged into main, yet not into
  their upstream) — the steward counts what git did, never what it asked. The row stands in
  `receipts/disk-steward-ledger.jsonl`. Free space fell 82.7 → 72.8 GiB across the hour's
  three readings while other sessions walked: the pressure is live, and the walk's heal
  stage now tends it every night and twice a day.

## The writer, found

Free space fell 82.7 → 60 GiB across the afternoon while nothing the steward watched grew.
The writer stood outside its sight: `$TMPDIR/form-run.Xjv7LN/stdout`, **174 GB**, written by
`tee` for `./fkwu .hearth/continuation-inspect.bml` — a recipe in a print loop, 3 h 23 m old,
55 % CPU, in a sibling's worktree (`~/.codex/worktrees/3457`), a child of the Codex app
server. `form-run`, the output-compaction carrier the global instructions name, captured
its stdout unbounded. Stopping the child at 13:50 WITA let the carrier's own exit path
release the capture: 60 → 225 GiB free. The owning session was told on the field board.

Two heals follow from it, in the same movement:

- **`form-run` bounds its capture** (`form-run-posix-v2`, the kernel's declared carrier and
  the installed `~/.codex/bin/form-run`, now one text): the child runs in the background, the
  carrier reads the captured bytes once a second, and past the cap (1 GiB; `FORM_RUN_CAP_BYTES`
  raises it) the child is stopped, the stop is named on stderr, and the `@form` line keeps
  its five fields. A wrapper that can fill the disk can stop every process on it. Witnessed:
  `./form-run ./fkwu bootstrap/ground.fk` → `42`, `@form fkwu 0 3 0 3`; `FORM_RUN_CAP_BYTES=1000
  ./form-run yes` stopped in three seconds, exit 143, `@form yes 143 55823040 0 55823040` — the
  once-a-second watch let 56 MB through before it caught a writer this fast, which against the
  default cap of 1 GiB is the overshoot to expect, not a hole.
- **The steward walks the user temp tree.** `ds-captures` lists each `form-run.*` capture's
  files as leases: written within ten minutes → `capture-live`, reported in the tending line
  and never released; untouched past `capture_days` (1) → `capture`, waste. Today's reading
  would have said *live captures 1 (174.0 GiB)* at noon.

## Honest floor

- The disk is read through a `df` child; the syscall-thin native (`host_disk_stat`, statfs)
  is filed as a request and is the next lowering.
- Remaking cost is a class estimate by kind; creators recording their cost beside what they
  make is filed as a request, and the weighing reads the recorded cost first once it stands.
- The idleness of a worktree asks `git status` as a child; asked of every worktree it cost a
  probe more than five minutes, so the cheap signal (`.hearth` mtime) is read first and git
  only of the quiet ones.
- The guide (`ds-guide`) is data the body tends; its values today come from this day's
  readings, and the ledger is where they will be argued with.
