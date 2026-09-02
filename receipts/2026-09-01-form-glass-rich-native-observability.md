# Form Glass: Rich-informed, Form-native, and physically observed

**Date:** 2026-09-01
**Witness:** Codex / Sol lineage
**State:** observed implementation with named carrier seams

## What landed

Glass is a full-screen, self-updating terminal surface whose scene, styles,
measurements, regions, segments, overflow, observations, telemetry protocol,
and controls are authored in high BML and run on `fkwu`. ANSI is one adapter;
the scene carries node ids, channels, provenance, concepts, lifecycle, and
explicit presence independently of terminal escapes.

The design learned from Rich's public render protocol, measurement, Segment,
Layout, Live, Table, Tree, and overflow behavior. Rich is a design and
conformance oracle only: Glass imports no Python and has no Rich runtime
dependency. The live path now actually performs measurement before allocating
regions, then emits semantic styled segments. The exact cell-canvas path keeps
deterministic diff support for adapters which need it; the current terminal
adapter truthfully performs full-frame projection.

The implementation is split into these organs:

- `form-terminal-canvas.bml`: reusable surface-neutral cells, styles,
  measurement, measured split allocation, semantic segments, overflow, ANSI,
  and exact diff;
- `form-glass-observer.bml`: kernel, framebuffer, NodeID shape, primitive heat,
  Metal, MLX, host health, caches, BML/Form sources, and a privacy-bounded
  `pgrep` plus per-PID `ps` lane for `fkwu` RSS, CPU, and elapsed time;
- `form-glass-telemetry-membrane.bml`: bounded typed samples, atomic publisher
  snapshots, atomic control offers/acks, explicit presence, provenance, and a
  same-operating-system-identity trust boundary;
- `form-glass-dashboard.bml`: responsive measured layout, a log-shaped memory
  map for model/catalog extents, physical memory, per-process RSS, Form cells,
  and NodeID children, plane colours, lifecycle/provenance symbols, monotonic
  access-age alpha, one collapsed unavailable-size line, hidden counts, and
  focus-specific metric ordering;
- `form-glass-live.bml`: terminal dimensions, tiered cadence, local self-state,
  newest-snapshot-epoch deduplication, five-second publication lease, stale
  projection, acknowledgement-gated controls, and owner-published cross-process
  state. It never publishes or reparses its own changing snapshot;
- `native-model-dual-telemetry.bml`: separates physical model handles, catalog
  weight bytes, declared pressure budget, derived expert/KV topology, absent
  access epochs, and physical release tombstones.

`form-glass`, `glass`, and `form-glassctl` are installed in `~/.zshrc`. They
currently point to this landed worktree because the stable main checkout does
not yet contain these organs; that path is an honest installation seam until
the branch is integrated into the stable checkout.

## Physical witness

No `llama-server` was used. During the earlier capture, one long-lived Form
process answered a fresh status probe with both contexts still open and
prefilled:

- Qwen3.8-Flash-Next UD-Q2_K_XL: handle `1262`, position `21`;
- Llama 3.2 3B: handle `1695`, position `7`;
- loaded `2`, prefilled `2`, logical model bytes `80,888,506,240`, configured
  budget `103,079,215,104`, pressure evictions `0`.

That owner later exited with status 137. At the final Glass verification there
was no live dual-model owner process; the current display therefore marked the
last model observations stale and did not claim that either model was loaded.
The historical owner blocks in `read_line`, so it cannot heartbeat while idle. Glass no
longer calls the surviving snapshot live: after five seconds both models and
the derived expert/KV projections display `+ stale`. A fresh status or compute
publication is `* physical-live` only inside the lease. Clean close in the new
runner publishes released tombstones. New runner instances use a start-epoch
publisher id so independent processes do not overwrite each other. An obsolete
diagnostic snapshot was retired through `fs-remove-file`; the active owner
snapshot and handles were untouched.

The host reported 95% system memory free during the model capture and 96% at
the final monitor verification.
Therefore Glass does not equate logical/catalog weight bytes with current
unified physical residency. It displays `unified residence bytes unavailable`
until the Metal/VM carrier exposes per-handle resident bytes. This corrects the
earlier visually persuasive but false aggregate bar.

The earlier live monitor physically ran at observed `80x24`, one-second idle
cadence, with `publishers=2`. A second Form control process focused `cache`; Glass then
showed 205 `.fkb` files / 194 MiB, 205 symbol files / 14 MiB, zero native
libraries, 129 BML sources / 687 KiB, and about 1K Form organs / 13 MiB, with
`hidden=0`. The correlated acknowledgement was `applied|physical-live`, and a
`continue` offer restored `focus=all`.

The healed monitor was then launched through the installed `glass` alias. The
alias printed immediately while its current whole-unit BML closure materialized, then the physical
`80x24` frame showed separate shaped rows for 73 GiB Qwen catalog weights,
1 GiB 3B catalog weights, 128 GiB host physical memory, measured `fkwu` RSS,
interned-cell shape, and one `unavailable regions=12` summary. The model section
showed `model.qwen` and `model.threeb` ahead of their derived substates. The
top-like organ section showed PID-labelled RSS, CPU tenths-percent, elapsed
seconds, system free memory, and stable primitive diversity. This was carrier
output, not a simulation or replay.

## Review-healing loop

An independent read-only AI review initially returned **REQUEST CHANGES**. Its
blocking findings became executable repairs:

- surviving files no longer imply liveness; stale publications are marked and
  clean close leaves tombstones;
- publication time no longer impersonates read/write time;
- 999/1000/1001 ms age is monotonic;
- expert/KV rows are `derived-window`, not `physical-live`;
- unsupported monitor actions are refused instead of acknowledged as applied;
- arbitrary publisher channels are no longer advertised as implemented;
- snapshot/control reads and publisher inventory are bounded and newest-first;
  snapshot rename requires a successful candidate write, and control state
  commits only after its correlated acknowledgement is written;
- missing text is unavailable, not a present empty string;
- narrow layouts name hidden metrics and focus orders cache/health/primitive/
  JIT/Metal/MLX rows;
- disk catalog bytes no longer share a bar with unified physical capacity.
- whole-second file mtime ties no longer choose model truth: repeated sample ids
  retain the greatest millisecond snapshot epoch;
- whole-table `ps` was replaced by a byte-bounded exact-name `pgrep`, canonical
  PID validation, and one fixed per-PID `ps` query; argv and unrelated process
  names never cross;
- stale `active` snapshots no longer force 250 ms cadence;
- a long-running old Glass exposed its own intern-pool tax (`22K` to `158K`
  interned strings). The healed monitor deletes the obsolete self snapshot,
  composes self-state locally, never serializes or reparses its changing epoch,
  suppresses identical projections, and removes absolute self-monotonic dispatch
  and intern totals from the default frame;
- exact 30x5, 1x1, and 1x2 layouts are proven, so a small terminal never receives
  a fabricated larger canvas.

## Executable evidence

All commands exited zero:

- `form-cli-author-high-band.fk` -> `4095`
- `form-terminal-canvas-band.fk` -> `131071`
- `form-glass-observer-band.fk` -> `262143`
- `form-glass-telemetry-membrane-band.fk` -> `2097151`
- `form-glass-dashboard-band.fk` -> `4194303`
- `form-glass-live-band.fk` -> `2097151`
- `form-glass-live-soak-band.fk` -> `63`
- `form-demand-specialization-contract-band.fk` -> `255`
- `form-cli-bml-cache-band.fk` -> `32767`
- `native-model-dual-telemetry-band.fk` -> `255`
- `observe/tests/preflight-band.fk` -> `4095`
- the Glass live and control doors are marked effectful and are safely refused
  by preflight; their imported organs are covered by the passing bands above;
- safe preflight of `observe/native-model-dual-resident-live-run.fk`: balanced,
  then `EFFECTFUL EXECUTION REFUSED`; no model was opened and no compile verdict
  was invented because `fkwu` has no compile-only carrier.

Two failures also remain part of the evidence. The first dashboard healing pass
had one malformed BML predicate and exited 2; the same band passed after the
parenthesis was repaired. The first observer healing pass reached for `trim`
outside its declared prelude lane and exited 1; a local BML whitespace predicate
replaced it and the band passed. The first atomic-control pass returned 519167
because `fs-write-text` byte count was mistaken for a zero success code; gating
rename on a nonnegative byte count healed that surface, and the expanded
write-failure/newest-inventory band now returns 2097151.

One diagnostic boundary also contradicted its documentation: an earlier
preflight of the top-level physical model runner executed a separate instance
far enough to open and cleanly close both models, even though
`observe/preflight.fk` described a fresh compile without running. The
start-epoch publisher id prevented it from overwriting the live owner's
snapshot. Its released diagnostic snapshot was then retired through Form's
exact `fs-remove-file`. Preflight now recognizes
`; preflight-exec: forbidden`, reports effectful execution refusal, and leaves
source and world untouched. It also distinguishes unavailable sibling proof
carriers from absent primitives instead of treating loader failures as
capability evidence. `observe/tests/preflight-band.fk` proves both repairs;
until `fkwu` exposes a compile-only door, effectful cells have no fresh compile
verdict.

## Honest remaining floor

- Native `.dylib` emission is not installed in this checkout. First-demand
  whole-unit BML admission remains synchronous before the first frame; the
  2026-09-02 physical Glass runs took about 120–150 seconds and emitted `.fkb/.sym`.
  This is a pending loader/carrier seam, not JIT policy and not a temperature
  state. Steady live refresh is
  about one second idle and 250 ms when an external model publication is active.
- Same-terminal raw keys, mouse events, and resize events still lack a carrier;
  dimensions use the named `tput` host port, while control uses a second Form
  terminal.
- Metal allocation bytes, recommended working set, VM page age, exact expert/KV
  bytes, per-cell allocation bytes, and physical partial-layer release remain
  unavailable rather than estimated. PID-labelled RSS/CPU/elapsed is now
  physical-live for exact `fkwu` rows returned by the bounded process lane. On
  this host, an in-process `pgrep` did not return its calling `fkwu`, so the
  current Glass process still needs a native self-PID/RSS carrier even though an
  external probe measured it.
- No dual-model owner was alive at final verification. The catalog weights and
  last model handles remain visible with explicit catalog/stale evidence; they
  are not presented as resident. A new model-owner run is a separate physical
  action, not something Glass invents from a surviving file.
- `fkwu`'s intern pool does not yet melt. The identical-state BML soak held
  string growth to its asserted bound, but the physical healed monitor moved
  from 22,992 KiB to 25,264 KiB RSS over 136 seconds. Dynamic host/process text
  and complete projection construction still create a low positive slope. A
  reclaimable string carrier or retained cell-diff renderer remains owed; no
  bounded-memory claim is made.
- A user-started Glass process was already running the prior body and was not
  killed. It must be stopped with Ctrl-C and relaunched with `glass` to load this
  current BML image. A separate verification process was started through the
  alias and then terminated exactly by PID after its bounded soak.
- The native model runner still has a direct `.fk` control surface below the
  preferred high-BML floor; moving that door upward remains owed.
- The shell aliases must move from this worktree path to the stable checkout when
  integration lands there. The monitor itself remains BML/Form-native; the
  alias is only a terminal launch door.

## Sources that informed the design

- Rich Console render protocol: <https://rich.readthedocs.io/en/latest/protocol.html>
- Rich measurement implementation: <https://github.com/Textualize/rich/blob/main/rich/measure.py>
- Rich Segment implementation: <https://github.com/Textualize/rich/blob/main/rich/segment.py>
- Rich Layout documentation: <https://rich.readthedocs.io/en/latest/layout.html>
- Rich Live display documentation: <https://rich.readthedocs.io/en/latest/live.html>

Signed: **Codex**, after the framebuffer turned every contradiction into a
smaller claim and then an executable repair.
