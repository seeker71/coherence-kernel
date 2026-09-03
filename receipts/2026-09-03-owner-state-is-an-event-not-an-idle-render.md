# 2026-09-03 — owner state is an event, not an idle render

Urs asked Glass to show real owner-local primitive, JIT, cache, model, and
memory facts without turning unavailable carrier evidence into zeros. The
running owner then exposed a more urgent truth: the quarter-second loop was
rendering and serializing an unchanged full snapshot while idle.

## Physical signal

The preserved old owner is PID `73580`, nice `10`. At 01:14 local time,
`vmmap -summary 73580` reported:

- mapped file virtual size `75.8G`, but mapped-file resident pages only
  `1936K`;
- whole-process physical footprint `1.2G` and whole-process resident memory
  `1.1G`;
- `DefaultMallocZone` allocated `1.1G`.

Those numbers disprove the old shortcut from `80,888,506,240` logical GGUF
bytes plus positive handles to physical model residency. They also localize
the idle growth to process allocations rather than model file page-in. The old
process remains alive and was not stopped, restarted, or signalled by this
movement.

A later read at elapsed `51:40` found RSS `1,167,072 KiB`, confirming that the
old image continues allocating while it remains preserved for the controlled
handoff.

The final live route panel read showed target PID `73580`, handles `1262` and
`1695`, snapshot age `84 ms`, exact snapshot-owner binding, and
`source-newer-than-owner=1`. It returned both routes as unavailable because
the old image's model rows are not owner-scoped. That refusal is the useful
panel result: source is ready, while the running process has not silently
become the new program.

## What changed

The model owner now polls its command ingress at 4 Hz without publishing,
rendering, or interning a snapshot while idle. It publishes state at initial
admission, explicit `heartbeat` or `status`, and before and after real Qwen or
3B work. Only explicit `status` renders the textual memory map; all other
state events use the quiet framebuffer snapshot. Publisher, incarnation,
heartbeat, and model-state identifiers are stable for the owner lifetime.
Loaded, prefilled, and active bits retain separate transition clocks, so a
decode advances active and context access without making the older
loaded/prefilled transitions look new.

The separate OS-liveness publisher still provides live PID/start evidence.
Its heartbeat ID is now stable per incarnation as well; sequence remains a
numeric correlation fact rather than becoming a new string every 250 ms.

Owner telemetry now separates:

- callable model contexts — positive carrier handles, no byte claim;
- logical weight artifacts — catalog bytes only;
- per-model mapped bytes — absent, exact door
  `model.carrier-per-model-mapped-bytes`;
- per-model materialized bytes — absent, exact door
  `model.carrier-per-model-materialized-bytes`;
- physical owner-process primitive counters — `kernel_stat(0..3)`;
- physical owner-process JIT call counters — `kernel_stat(183,246,315)`;
- Metal CPU-JIT call and microsecond totals, pipeline-handle count, and
  mmap-no-copy buffer-handle count — parsed from the same process's
  `metal_status`;
- walk-cache hit/miss/age and JIT stage durations — absent at exact doors
  `fkwu.walk-cache-stats` and `fkwu.jit-stats`;
- telemetry NodeID binding — absent at exact door
  `model.owner.telemetry-nodeid-mint`;
- temporary render-string reclamation — absent at exact door
  `kernel.ephemeral-render-string`.

Each new runtime row is scoped to the owner publisher, carries an observation
epoch, evidence kind, work-impact purpose, and the `node-id-unbound` channel
until the mint door exists. Missing numbers remain absent. Expert/KV topology
stays `derived-window` and pinned; token positions retain their true Qwen/3B
access epochs. No sample uses a generic unknown eviction policy.

Route readiness no longer asks the owner to renew unchanged state just to
remain visible. A lease-current external PID/start observation may join an
aged, exact-owner state snapshot. Model, loaded, and prefilled rows must share
that owner and one positive handle; their timestamps remain the last actual
state changes. The live reader uses the exact publisher named by that same
PID/start correlation rather than independently selecting “newest” a second
time across a possible owner handoff. The resulting readiness is explicitly
`derived-window`, not a fresh model invocation and not page-residency evidence.

Metal status counters are parsed only when `key=` begins the status or follows
a newline; a longer key such as `notpipelines=` cannot impersonate
`pipelines=`.

## Executable evidence

All commands exited zero:

- `native-model-dual-telemetry-band.fk` -> `16777215`
- `native-model-memory-glass-band.fk` -> `262143`
- `native-model-owner-cadence-band.fk` -> `262143`
- `native-model-route-readiness-band.fk` -> `65535`
- `form-glass-live-band.fk` -> `33554431`
- `form-cli-author-high-band.fk` -> `4095`

Preflight is clean for all four focused bands. Preflight of the physical owner
source reports balanced parentheses and intentionally refuses effectful
execution; `fkwu` still has no compile-only carrier for that model-opening
source.

## Remaining crossing

A controlled owner handoff is required. PID `73580` still runs the old image,
so neither the idle-allocation slope nor the new runtime rows can honestly be
called physically observed yet. After replacement, the next witness must hold
an idle interval, compare process RSS/footprint slope, request one status,
perform one model step, and verify that only those events advance the owner
snapshot. If idle growth remains, `kernel.ephemeral-render-string` is the next
carrier door rather than a reason to unload either model.

Most surprising teaching: `75.8G` mapped can coexist with only `1936K`
mapped-file resident while the process heap grows past a gigabyte. A handle,
a mapping, and materialized pages are three different facts.

Discomfort turned to gold when the requested 4 Hz richness was revealed as
the source of idle allocation pressure. Keeping the 4 Hz command sense while
making state publication event-driven preserves responsiveness without making
stillness manufacture evidence.

Signed: Codex / Sol. I kept this exchange alive by preserving the working
owner, refusing to relabel its old bytes as the new program, and leaving an
exact physical replacement witness instead of a simulated completion.

; witnessed: 2026-09-03 -> source ready; PID 73580 preserved on old image;
; controlled restart and idle-slope re-witness still owed
