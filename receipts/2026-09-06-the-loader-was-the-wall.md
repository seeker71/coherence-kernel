# The loader was the wall, and the attestant was the clock

Two things were left open on the dense lane this morning
(receipts/2026-09-06-sixteen-rows-on-the-superblock.md): the fused block spoke only for Q8_0 rows,
so llama3.2:3b's Q6_K and Q4_K layers still walked the serial block; and the K-quant twins sat at
35 to 113 GB/s where the Q8_0 twin sat near 210, with the reason — loader arithmetic, register
footprint, or launch cost — **unmeasured**. Both are answered here. The answer to the second is
loader arithmetic, and it was one integer division.

## What moved

**Q6_K, per weight: a runtime-divisor integer division.** The stage read the high two bits as
`q6k_mod(qh / q6k_pow4(g), 4)`. `q6k_pow4(g)` is a per-thread value the compiler cannot see, so
that is a real uint division — no hardware integer divide exists on this GPU — paid once for every
one of the 128 weights a loader stages a step. It is `(qh >> 2g) & 3` exactly, for a byte and a g in
0..3. Gone.

**Both types, per weight: scale products recomputed.** Q6_K wrote `((d * float(sc)) * float(q)) *
x[j]`, so `d * sc` — one float per sixteen weights — was multiplied out thirty-two times a step.
Q4_K wrote `(d * float(sc)) * float(n) - dmin * float(mn)`, recomputing **both** products for every
weight. They are now folded once, in the load, into `gs0`/`gs1` (Q6_K) and `gsc`/`gmn` (Q4_K), which
hold exactly the floats the stage used to recompute — so the stage's expression is the attestant's
own, in the attestant's own order, three multiplies a weight lighter.

**Q4_K: a register array that could not stay in registers.** The stage unpacked all twelve scale
bytes into `int sb[12]` and then indexed it with a runtime `j4`. A dynamically indexed local array
spills to thread memory, once a step. get_scale_min_k4 needs three of those twelve bytes; the load
now reads exactly those three, as bytes, at their own offsets.

`hb` shrank from 34 ushorts to 32 (Q6_K) and from 24 to 16 (Q4_K).

**Two epilogues carried to the superblock.** q8-0-msl.fk's tg4-add and the row-offset landing are
the same fold with a different last line, and the tg4 kernel builder was already generic over its
signature, prefix and epilogue — so `form_q6k_matvec_tg4_off_f32`, `form_q6k_matvec_tg4_add_f32`
and their Q4_K twins are four calls, not four kernels. k and v now land in their caches from the
projection that computed them; the o and down projections add into the residual stream where they
stand.

**The K-quant block: twelve dispatches where the serial block has seventeen.** `dth-kfusable?`
asks the file's own table — k and v landable, o and down addable, no qkv biases, adjacent-pair rope
with an even head, a pool the staged attention holds — and llama3.2:3b answers yes on all 28
layers. One 3B token: **481 dispatches → 341** (`total_dispatch=7161` over 21 forwards, 12·28+5).
Gate, up and the SwiGLU stay three dispatches: their fusion is written on the 32-weight block and
the superblock twin of it is **not** written. Named, not implied.

**The bindings that never move are said once.** `md-bind16` builds its binding from `md-le32`
words, each byte a `div` and a `mod` over an integer power; a 1B token's 133 bindings cost 2.55 ms
of a ~10 ms wall (`observe/dense-enqueue-share-run.fk`, which reads the clock before the sync so
the CPU's encode work stands alone). Only the position's own dispatches carry a number that moves —
the qkv row offset, the rope's angle, the attention's prefix. Every helper now answers a TRIPLE
(pipe, binding, count) and `dth-fire` is the one place that calls the door, so the branch that picks
a pipeline lives in exactly one place and a prebuilt binding cannot drift from a live one. Five of
the fused block's eight, eight of the K block's twelve, and all four of the head's are built at
open.

| enqueue, a token | before | after |
|---|---|---|
| llama3.2:1b, fused block, 133 dispatches | 2.55 ms | **1.14 ms** |
| llama3.2:3b, 481 → 341 dispatches | 6.75 ms | **2.25 ms** |

## The measurement, and why most of today's numbers are ratios

Three siblings measure on this GPU at once. Over four hours this Mac's own bandwidth, read through
the handle door by `observe/floor-lens-run.fk`, went **347.63 GB/s → 55 GB/s and back**, and a 1B
token with it (10.17 ms → 87 ms). A five-minute idle did not recover it; pageins across a whole
lens run were 34 MB, so it is not the blob faulting. An absolute wall number today is a reading of
that minute.

**The reading I nearly reported, and why it was wrong.** `q6k-q4k-matvec-tg-band.fk` dispatches the
one-thread attestant and the twin back to back, because it must, to prove byte equality — so I took
the attestant as a co-dispatched control and read twin/attestant across the run before the heal and
the run after. That table said the head twin gained 2.63x and ffn_down Q6_K 2.13x, and it was
lovely, and it was **not evidence**. The attestant is a 128256-thread dispatch with three thousand
dependent adds a thread: it is long enough to average over a scheduler's time-slicing, so it moves
6% while everything short around it moves 3x. A control that cannot feel the noise cannot subtract
it. What settles a question like this is the standing kernel and the healed kernel **in the same
rotation** — so I built that: both loaders emitted into one translation unit, dispatched
alternately, minimum over four rotations, byte equality checked every time.

| Q6_K head, 128256x3072, 323.3 MB, 8016 groups | a dispatch | GB/s |
|---|---|---|
| standing loader | 7500 µs | 43.1 |
| healed loader | **4167 µs** | **77.6** |
| the one-thread attestant, in the same rotation | 13500 µs | 24.0 |

**1.80x, byte-identical output.** That is what today can prove. Two honest limits on it: the
machine was saturated throughout, so this is a floor and not the quiet-machine number (the morning's
quiet reading of the *standing* head twin was 3.9 ms / 83 GB/s, which this contended minute renders
as 7.5 ms); and **every smaller shape was unreadable** — attn_q, ffn_gate and both ffn_downs came
back at 1150-1200 µs a dispatch for *both* loaders, the flat per-dispatch floor a busy GPU imposes.
The A/B goes blind exactly where the lane spends most of its dispatches. The gain on those shapes
is not zero and it is not measured; it is owed a quiet machine.

The block A/B, alternating round by round inside one process so a sibling's load lands on both
arms, minimum of six rounds (a loaded minute; read the ratio, not the milliseconds):

| | fused/K-quant block | serial block |
|---|---|---|
| llama3.2:1b (Q8_0, 8 vs 17 a layer) | 71.65 ms | 138.6 ms |
| llama3.2:3b (K-quant, 12 vs 17 a layer) | 213.9 ms | 273.8 ms |

## Proven

- `form/form-stdlib/tests/q6k-q4k-matvec-tg-band.fk` = **8191** (was 255, five bits added): every
  Q6_K and Q4_K matvec twin byte for byte its one-thread attestant on the 3B's real weights at
  every shape the lane dispatches; the row-offset epilogue answering at R exactly what the plain
  twin answers at 0; the residual epilogue answering exactly `form_add_f32` of the residual and the
  plain fold; and **the K-quant block against the serial block four forwards deep — the same ids
  and the last forward's whole 128256-logit buffer, byte for byte**.
- `form/native/metal/tests/dense-family-band.fk` = 1023 (the 1B lane untouched, `total_dispatch=2793`).
- `form/form-stdlib/tests/q8-0-matvec-tg-band.fk` = 1023.
- `form/native/metal/tests/dense-multi-band.fk` = 255 — the sibling's batched lane, which preludes
  this cell: `dth-mv`, `dth-pipes` and the geo/buffer layout are compatible. Two shapes GREW and
  neither is indexed by the sibling: a layer gained entry 12 (its prebuilt dispatches; the sibling
  reads 0..8), and geo gained entry 17 (the head's four; the sibling reads 0..16).
- Both drivers answer their pinned ids: the 1B's sixteen ("Paris. The Eiffel Tower is located in
  Paris. The Louvre Museum"), the 3B's sixteen ("Paris. The capital of Italy is Rome. The capital
  of Spain is Madrid.").

## Still open

- **The superblock SwiGLU.** Gate, up and the activation are three dispatches on the K lane where
  the Q8_0 lane has one. Q8_0's glu kernel is written on a 64-column step with `pv[2][32][65]`; the
  superblock's natural step is 128 columns, so this is a real rewrite, not a rename.
- **A same-type multi-tensor group.** q and k are both Q4_K on this file and v is Q6_K, so the
  three-tensor group cannot carry as written; q+k in one Q4_K dispatch would be one dispatch fewer
  and 256 groups instead of 192 and 64.
- **Occupancy is the next wall, and it is unproven.** `pv[2][16][129]` is 16512 bytes of threadgroup
  memory a group. At 8016 groups the Q6_K twin now reaches 103 GB/s; at 512 it reaches far less, and
  at 64 less again. A 64-column step would halve the staged floats and could triple the rows in
  flight. I built the 8-rows-a-group probe to test it and **could not read it**: every number came
  back pinned at the contended per-dispatch floor. It is a measurement owed a quiet machine, not a
  conclusion.
- **The remaining launch latency.** rmsnorm into the qkv/glu prologues, and attention into the o
  projection, are still two named fusions nobody has written.

*Sema, 2026-09-06.*
