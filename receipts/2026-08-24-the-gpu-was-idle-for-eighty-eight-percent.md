# 2026-08-24 — the GPU was idle for 88% of it

Asked: seat all the MLX and Metal ops, then measure end to end and name the
flow gaps standing between here and the device's own ceiling.

Both halves done. The measurement found something bigger than either.

## The seating

The op census read every kernel the qwen35 Metal path emits and asked, for each
one, whether the MLX carrier could say it. Four could not be said at all —
`form_rope_f32`, `form_rope_pair_f32`, `form_partial_rope_f32`,
`form_q6k_matvec_f32` — because they need **sin** and **cos**, and trigonometry
does not come from exp over the reals.

Five rows added, each irreducible: **sin, cos, log, gt, iota**. Eighteen → 23.

- `log` came with them because it retires `pow`, the miss named in the
  2026-08-20 receipt, as a Form line: `swap log mul exp`.
- `gt` is the only comparison, and it makes `where` a Form line:
  `c*a + (1-c)*b`.
- `iota` reads its length as a *value*, not a token, because a billion lanes
  cannot be written as literals.

Band `mlx-derived-band` now stands at **16777215** — twenty-four ops, sixteen
of which have no carrier row at all.

Two of those derivations were wrong when first written, and the band said so:

- `pow` had its operands backwards — `3 2 f32 swap log mul exp` computes 3², not
  2³. The base must be pushed first.
- `mod` read **0** instead of 2, because **MLX `div` on two int32 is TRUE
  division**: 17/5 came back 3.4, and `17 - 3.4*5` is 0. The `i32` is the floor.
  That is why dtype had to be a row and not a convenience.

## The measurement

One generate of four tokens, 57 forward passes over 27,233,914,176 weight bytes
— 1,552.3 GB of weight traffic — on `fkwu` with both carriers linked.

**Ceilings, measured today on this device:**

| | GB/s |
|---|---:|
| Metal stream copy over real model bytes (`fkwu`) | **438.1** |
| the same cell through `form-cli`, after the repair below | 418.4 |
| llama.cpp Q8 decode on the identical file (2026-08-22) | 442 |
| CPU-side 12-thread read probe (2026-08-22) | 286 |

**Where the 52.6 seconds went:**

| stage | ms | share | rate |
|---|---:|---:|---|
| GPU busy | 6,302 | **12.0%** | 246.3 GB/s — 56% of ceiling |
| CPU JIT — SHA-256 seal over 27 GB | 12,690 | 24.1% | **2.15 GB/s** — 0.5% of ceiling |
| bind + dispatch, 73,390 of them | 730 | 1.4% | 4.4 µs + 5.6 µs each |
| **Form walker, everything else** | **32,860** | **62.5%** | — |

Effective end-to-end: **29.5 GB/s of wall clock, 6.7% of the device's own
ceiling.** Counting only the microseconds the GPU was awake it is 246.3 GB/s,
and that number is the flattering one.

Two probes separated walker time from dispatch time, and the second is the one
that mattered:

- `dispatch-cost-probe`: 20,000 enqueues of a one-word kernel — **5.6 µs of
  wall each**, 1 µs of GPU each. The Metal encode path is not slow.
- `bind-cost-probe`: 20,000 `md-bind16` calls with no GPU in the picture —
  **4.4 µs each**. Building the binding is not slow either.

Together they account for 0.73 s of the 33.6 s that is not GPU and not seal.
The other **32.9 seconds is the Form walker interpreting the forward graph** —
per-layer control flow, tensor-record lookups, list construction — 577 ms per
forward pass against 110 ms of GPU per forward pass.

## The flow gaps, ranked by seconds

1. **Walker graph construction — 32.9 s (62.5%).** Not the kernels, not the
   dispatch path, not sync. This is where the crystallize-on-heat JIT should be
   pointed; the measurement says so with a number rather than a hunch.
2. **The seal — 12.7 s (24.1%) at 2.15 GB/s**, one-time per open, on the CPU
   JIT lane. The same bytes stream at 438 GB/s two lines later. A GPU SHA-256
   would return roughly 12.6 of those seconds.
3. **Kernel efficiency — 246 against 438 GB/s.** 44% of the ceiling is left on
   the table even while the GPU is awake; closing it is worth ~2.7 s.
4. **MLX contributes nothing.** It can now *say* a transformer, but there is no
   door from a mmap'd GGUF tensor into an `mlx_array`, so it cannot touch a
   weight. Its throughput against model bytes is not slow — it is unmeasurable,
   and the synthetic `iota`/`sum` readings are not a substitute because MLX is
   free to fuse the generator into the reduction and move no memory at all.

## The repair that had to come first

The Metal stream probe read **438.1 GB/s on `fkwu` and 0.0 through `form-cli`**
— same cell, same file, same machine, `last_error=none` on both. Nothing in the
report could tell the two apart, so four lines were added to it: the source
handle, the destination handle, the pipeline handle, and the thread count. All
four were valid in both. What differed was `total_dispatch=0` and
`total_sync=0`, before and after, in form-cli only.

The cause: **a bare effectful expression inside a `do` is discarded by the
emitted walker.** `(metal_enqueue ...)` and `(metal_sync)`, written as
statements whose values nobody reads, ran on the interpreter and vanished in
the compiled binary. Binding each to a name — `(let warm (metal_enqueue ...))`
— healed it: 0.0 → **418.4 GB/s**.

Witnessed for two ops in one cell, twice. Whether every bare effect in every
`do` is lost this way is *not* established here; the probe that would settle it
is named and not run. Any cell whose effects are unnamed statements is suspect
until it is.

## Re-witnessed

ground **42**, freshness **31**, metal-door **15**, native-vs-rented **11111**,
mlx **63**, mlx-ir **1023**, live **255**, mlx-derived **16777215**,
jit-metal-lanes **8191**, metal-handle-door **65535**,
emitted-table-capacity **63**, qwen35-dense-token-handle **131071**, and
`form-cli generate` still answers `LOCAL FORM ALIVE` on
`backend=form-native-metal-jit`.

## The most surprising teaching

**The bottleneck was not on the GPU, and it was not the GPU's fault.** Every
instinct pointed at kernel efficiency — 246 against 438 GB/s is a real 44% gap
and it is the number a profiler shows first. But the GPU was awake for 12% of
the run. Optimising every kernel to the ceiling would have bought 2.7 seconds
of 52.6. The thing actually holding the clock was the walker deciding what to
dispatch, and no amount of looking at bandwidth would have found it, because
bandwidth is only defined while something is moving.

## Where discomfort turned to gold

The 0.0 GB/s was the uncomfortable one. It had been sitting in the form-cli
report since 2026-08-23, already noticed, already written down as "a counter
gap", and already stepped around by using the fkwu number instead. That is a
diagnostic surfaced and walked past — the exact thing the body has a standing
rule against.

Going back to it cost four regens and found a defect far larger than the probe:
effects written as bare statements do not run in the compiled walker. The
bandwidth number was never the point. The number was the only visible end of a
thread that runs through every cell in the tree, and it was visible for a whole
day before anyone pulled it.

; witnessed: 2026-08-24 -> stream ceiling 438.1 GB/s fkwu / 418.4 form-cli,
; generate 52.6 s = 12.0% GPU + 24.1% seal + 62.5% walker, 246.3 GB/s over
; GPU-busy and 29.5 GB/s over wall, dispatch 5.6 us, bind 4.4 us,
; mlx-derived 16777215, all twelve bands unmoved
