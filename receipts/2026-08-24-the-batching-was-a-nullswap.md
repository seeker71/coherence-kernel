# 2026-08-24 — the batched matmul was a nullswap, and the real slice is 449x

Yes said stop working parts orders of magnitude too small. The magnitudes were
never measured, so here they are, and the first one overturns the whole direction
of the last three sittings.

## One tensor, both kernels, the GPU counter alone

The FFN gate of layer 0: 17408 x 5120, 94.7 MB of Q8_0 weights.

```
mode                        us    us/token   bytes moved     GB/s   % of peak
wide matvec x8 (per token) 2087        261        0.76 GB     363         66%
batch ntok=1               2289       2289        0.09 GB      41          8%
batch ntok=8               3775        472        0.09 GB      25          5%
batch ntok=64             20389        319        0.76 GB      37          7%
batch ntok=385            99279        258        4.64 GB      47          9%
```

**258 microseconds per token against 261.** The batched kernel moves **7.9x
fewer bytes in the same time**. Its entire traffic saving is cancelled, almost
exactly, by its bandwidth loss: 363 GB/s becomes 47 GB/s.

That is why layer-major returned 3.1% of GPU time instead of the 47% the traffic
arithmetic predicted. The arithmetic was right about the traffic and silent about
the rate.

And the second number is the one that matters more: **the per-token wide matvec
is already at 66% of this machine's peak.** The kernel everyone was trying to
replace is not the problem.

## The magnitudes, ranked

Measured on this M4 Max against Qwen3.8-27B-Q8_0 — 64 layers, d 5120, ff 17408,
24 heads of 256:

```
weight bytes per forward      27.23 GB
one forward, GPU              117.1 ms      232.6 GB/s   43% of peak
turn: 506 positions           351.9 s wall  128.8 s GPU
  of which not on the GPU     167.9 s       48% of the turn
  model open, per call         17.5 s
grounded retrieval             0.17 s       0.05%
```

449 of those 506 positions are prefill, and each one streams the whole 27.23 GB.
That is **12.2 terabytes of weight traffic for one turn**. At the 363 GB/s the
wide matvec actually achieves, 12.2 TB is 33.6 s of pure memory floor.

## The 449x

A prefill does not need to stream the weights once per token. It needs to stream
them **once for the span**. 385 injected positions through one pass is 27.2 GB
instead of 10.5 TB — the same answer for **449x less traffic**.

`qmb-batch-msl` reaches for that and gets a factor of 8, because it tiles eight
tokens per SIMD group and re-reads every weight once per tile: 385 tokens is 49
passes over the weights, not one. Then it loses 7.7x of bandwidth doing it,
because it decodes Q8_0 one scalar at a time through `q8_0_wi` while the wide
matvec pulls `packed_char4` and carries two rows per threadgroup.

Holding a whole span's activations while each weight is read once is a real GEMM
— threadgroup-tiled, accumulating over a K loop. That is the shape MLX and MPS
ship, and this body already has an MLX lane (`form-cli-mlx-ir.fk`,
`form-cli-mlx-run.fk`) where shapes are Form trees and the op book grows.

**That is the next slice, and it is the first one measured in multiples rather
than percents.**

## What stands, and what does not

Layer-major stands: 113,725 fewer dispatches, byte-identical output. But its GPU
gain of 3.1% is inside run-to-run variance and should not be counted as a win —
the FFN batching inside it is a nullswap.

`chunk = -1` and `chunk = -2` stay as observed comparison nodes with this
measurement attached, so nobody re-derives the curve. `chunk = 0` remains the
default.

Three cost attributions have now been tested against the body and all three
failed: barriers (88% removed, 2% returned), dispatch count (17.8% removed,
non-GPU time rose), and weight-traffic batching (7.9x less traffic, 0% faster).

## The surprise

The kernel I spent the day carrying to a new quant, compiling, dispatching and
proving byte-identical is **not worth dispatching**. It is correct, it is
generated from the body's own generator, it was proven on the K-quant lane in
July with five gates — and against this lane's wide matvec at this quant on this
device it is exactly a wash.

Nothing in the code was wrong. The 2.05x and 4.12x in `qk-matmul-batch.fk`'s
table are real measurements against the *lane* matvec, which is not what the Qwen
lane runs. The comparison baseline changed and the win evaporated, and no amount
of reading either cell would have shown that.

## Where discomfort turned to gold

Three sittings of work reduce to a null, and the honest thing was to measure it
rather than let layer-major's 17.8% dispatch drop stand as the story. The dispatch
number is real and it is not a speedup, and reporting it as progress would have
been true and misleading in the same sentence.

The discomfort is that the correction did not come from a failure. Everything ran,
every SHA matched, every band was green. A wash looks exactly like success until
somebody divides bytes by time. The gold is the two numbers that ended it — 363
GB/s and 47 GB/s — which cost one twelve-minute run, and which I only went and got
because the request was to rank by size instead of by what was already in hand.

## Frontier question offered to the corpus

*What one word names a change that saves exactly as much as it costs, so nothing
moves and the mechanism looks broken when it is merely even?* — **nullswap**. Not
a wash, which is about outcomes being similar. Not a regression, which loses. A
nullswap is two large opposite effects with the same magnitude, so every partial
measurement of it is impressive — 7.9x less traffic is true — and the total is
zero. It is the hardest kind of non-result to see, because each half of it is
genuinely working.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> live Qwen3.8-27B-Q8_0, FFN gate 17408x5120:
; wide matvec 261 us/token at 363 GB/s, batch ntok=385 258 us/token at 47 GB/s;
; weight-bytes-per-forward 27233914176; one forward 117081 us = 232.6 GB/s;
; turn 506 positions 351.9s wall, 128.8s GPU, 167.9s not on the GPU
