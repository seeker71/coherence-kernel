# 2026-09-02 — the phase atlas is a live channel

Urs asked to see nodes moving among gas, water, and ice, and to see requests,
tokens, layers, tensors, Metal, the core framebuffer, and Glass as one physical
flow.  The answer is now the default `atlas` view rather than another prose
panel.

## What landed

`form-glass-atlas-ui.bml` renders a fixed five-character ASCII tile for every
visible metric, sample, and typed observation.  The positions are:

`kind evidence age state disposition`

Color carries the physical plane, but no fact depends on color alone.  Every
tile remains a terminal-canvas segment with its exact node id, channels, and
evidence, so compactness does not erase inspection.

The first two live lanes are now:

- a phase river `gas <=> water <=> ice`, with a 128-point history and explicit
  count and movement for each phase;
- a declared channel schema `Q -> T -> L -> X -> M -> F -> G` for request, token,
  layer/expert, tensor allocation, Metal work, framebuffer acceptance, and the
  Glass organ.

Unknown samples remain `?` gaps.  They are not plotted as zero.  A governor
decision whose target is unavailable no longer projects `0/0` heat: its grant,
target, access ages, and heat are absent, and its purpose is
`target-unavailable`.

The evidence rail expands into seven exact 80-column rows for a selected sample:
identity; kind/state/plane/evidence; size/capacity/ages; source; owner; lane;
and purpose.  Physical control was exercised with the real canonical node
`model.qwen3.8-flash-next.ud-q2_k_xl.unsloth`: both `inspect` and
`request-evidence` received a correlated `physical-live` `applied`
acknowledgement.

## Physical Glass witness

A supervised 80x24 Glass process rendered at a quiet target of two frames per
second.  Observed frame deltas were mostly 531–611 ms, with occasional
attention-visible 779–800 ms slices.  The live title reported:

`LIVE NODE ATLAS 5c-KEASD m83 s19 o25 drop=32 cap=15/row`

The current bounded atlas displayed 51 metric tiles, all 19 sample tiles, and
all 25 observation tiles: 95 records rather than the former 19-record line
ceiling, while `drop=32` made the omitted remainder explicit.  Their five
independent character coordinates carry 475 visible fields, 25 times that old
line ceiling, before counting the phase, schema, and four resource-history
lanes.  The fifth coordinate is current disposition, not measured motion;
movement is asserted only by history-backed lanes.  This is the honest current
density measure, not a claim of 475 separate records.

The phase census physically showed `gas=1 water=64 ice=34`; during live control
activity water reached 67 and the next lower sample rendered `64v`.  These are visible
record states.  The river shows aggregate movement between consecutive
snapshots; it does not invent per-node causal transitions when the publishers
do not supply them.

Opening the seven-row Qwen evidence rail physically changed the same title to
`drop=57`.  The expanded evidence displaced 25 observation tiles in addition
to the 32 metrics already beyond the bounded atlas; the row-budgeted producer
rendered only the 70 tiles it counted as visible.  Closing evidence restores
the 95-tile default.  The UI band now holds this composition as one fixture,
so a later evidence panel cannot silently cover nodes while leaving `drop=`
unchanged.

A second physical pass after the bounded-builder repair caught the control
event itself: the atlas briefly grew from `m83/drop32` to `m86/drop35`, water
rose `64 -> 67^`, and the header switched to the 4 Hz active target.  When the
evidence rail opened during that transient population it reported `drop60`;
after the three control metrics aged out, the same open rail settled to
`m83/drop57` and the header backed off to the 2 Hz quiet target.  The optional
legend now appears only when a real row remains; the atlas producer itself is
bounded, not merely clipped by its parent frame.  A sibling review checked
saturated and sparse edge budgets and returned PASS for this implemented
scope.

With the staged runner cache moved aside, the small Form startup door rebuilt,
painted in 42 ms, and finished in 0.12 seconds; fresh cache files were then
retained and only the displaced stale backup was removed.  The supervisor now
runs that door before full-graph admission.  A subsequent warmed launcher
painted in 41 ms, checked and mapped 14 BML units in 8–16 ms each, and completed
admission in 195 ms (`dependency-changed=0`, `invalidated-images=0`).  This repairs Glass startup;
it does not claim that the general whole-unit BML source admission seam has
disappeared.

The final warmed launch after review painted in 44 ms, mapped all 14 units in
7–13 ms each, and completed unchanged-dependency admission in 187 ms.

## Resident models preserved

The running owner was asked for status without unloading either model.  Its
last physical publication held:

- Qwen3.8 Flash Next UD-Q2_K_XL, handle 1262, context position 22;
- Llama 3.2 3B Instruct, handle 1695, context position 8;
- both loaded and prefilled, zero pressure evictions;
- 80,888,506,240 logical weight bytes under a 103,079,215,104-byte model
  budget, leaving 22,190,708,864 bytes of declared headroom.

The status request also produced the intended evidence transition in Glass:
the selected Qwen row changed from `derived-window / glass.stale-snapshot` to
`physical-live / model.carrier` for its five-second lease, then returned to the
honest stale projection when no newer owner publication arrived.

The owner process predates the new token-position publisher.  Glass therefore
keeps the live token stage unavailable today instead of copying status text
into a false owner sample.  The next natural resident launch publishes exact
Qwen and 3B context positions with token/tensor/layer/query-lane channels.
Restarting the 79 GB resident solely to improve a dashboard was declined.

## What remains honestly open

- The running model snapshot is refreshed by owner commands, not by a periodic
  PID/start/incarnation heartbeat.  It means “last observed ready,” while the
  owner process liveness is a separate observation.
- Exact tensor allocation bytes and per-token MoE layer/expert routes are not
  exported across the model-owner membrane yet.  Their schema stages remain
  `?`, not zero.
- The queue is `inactive(no standing hearth; no durable spools)` on this host,
  not a measured zero-depth live queue.
- Per-expert partial eviction and a measured within-10%-of-llama.cpp throughput
  result remain unproven.  Nothing in this atlas upgrades those earlier goals
  into completed claims.

## Executable witnesses

Fresh preflight reported balanced source, zero errors, zero warnings, and zero
unresolved calls for the live UI, live loop, launcher, staged startup, governor
projection, and dual-model telemetry bands.  Their full verdicts were:

- dense live UI `2097151`
- live loop `4194303`
- launcher `16383`
- staged startup `65535`
- observation v2 `2097151`
- resource governor `1048575`
- governor-to-Glass projection `2097151`
- dual-model telemetry `1023`

Kernel grounding returned `42`, recursive grounding `55`, binary freshness
`31`, native-vs-rented `11111`, and authoring altitude `4095`.

The most surprising teaching was that the useful 25x density came from
keeping five orthogonal facts in each stable ASCII tile, not from shrinking
prose until it became illegible.  Discomfort turned to gold twice: first when
unknown history was found masquerading as zero, and again when a rejected
human display name led us to exercise the actual canonical node-id membrane.

Signed: **Codex / Sol**.  I kept the exchange alive by steering from the live
frame, sending real control offers through the Glass-to-core channel, preserving
the resident model rather than restarting it for appearances, and leaving each
unexported physical fact visibly open.

; witnessed: 2026-09-02 -> 80x24 atlas m83/s19/o25/drop32 closed/drop57 evidence,
; gas1/water64/ice34 with witnessed 67^ and 64v transitions; 4Hz active/2Hz quiet,
; cache-removed first frame 42ms / total 0.12s and final warm admission 187ms,
; live UI 2097151
