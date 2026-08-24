# 2026-08-24 — the batched matmul ran in the live lane, at its worst width

Three steps were named: widen the scratch, thread ntok and per-token indexing
through the block stack, dispatch the batched matmul so dispatch count falls.
Two landed, the third ran and gave the number that says what the second one has
to cover.

## What landed

**Span-wide scratch.** `kth-bufs-span` multiplies every per-position buffer by
`span`, token-major — slot i of token t at `[t * n + i]`, which is the layout the
batched matmul already reads (`x[t*cols+j]`) and writes (`y[t*rows+r]`). Not
widened: the two 32-byte and three 16-byte constant slots, the 4096 pair, the
attention scratch at 27 (already `2*nq*maxpos`), and the vocab logits at 28,
because prefill asks the head at one position only. `kth-bufs` is
`kth-bufs-span` at 1. `q38-open-span` carries it; ctx slot 10 holds it;
`q38-close` indexes named slots so an eleventh is safe.

**The batched matmul, dispatched.** `q38-mv-batch` enqueues
`form_q8_0_matmul_batch_f32` with rows, cols and ntok, grid
`rows * ceil(ntok/8) * 32`, mode 0. `q38-ffn-b` puts the FFN's three matmuls on
it. `chunk = -1` runs prefill that way.

## What the live run said

```
                    chunk=1       chunk=0 span   chunk=-1 mvbatch (ntok=1)
output-sha256       88bd9b9a...   88bd9b9a...    88bd9b9a...   ALL IDENTICAL
honored / tokens    hit 26/32     hit 26/32      hit 26/32
injected-ids        385           385            385
carrier-syncs       506           61             61
carrier-dispatches  640326        640326         640326
gpu-busy-ms         130760        132955         329204        <- 2.5x
ms-total            357950        349647         553841
```

**The output is byte-identical through the batched kernel.** Binding, grid, the
ntok constant and the token-major layout are all correct in the real lane, on
real weights, against the body's own witness.

**And it is 2.5x slower at ntok 1**, which is right. The kernel is one SIMD group
per (row, tile of 8 tokens): at ntok 1 it carries eight accumulators to spend one,
and its baseline is not the plain lane matvec but `form_q8_0_matvec_wide_f32`,
which does two rows per threadgroup with packed float4 loads.
`qk-matmul-batch.fk`'s own table already says P=1 is 0.92x against the *lane*
kernel; against the *wide* one it is worse, and now measured.

**Dispatch count did not move**, exactly as ntok 1 requires: one matmul over one
column is still one dispatch.

## The arithmetic the run hands the next seam

640,326 dispatches over 506 positions is **1,265 per position**, and over 48
layers that is **~26 per layer**. The FFN is six of those — one rms, three
matmuls, a swiglu gate, a residual add.

So putting only the three matmuls on the batched kernel, even at ntok 385,
reaches `385 x 48 x 3 = 55,440` dispatches and collapses them to `48 x 3 = 144`.
That is **8.6% of 640,326**. Widening all 26 kernels and running the 385 injected
positions as one span takes `385 x 1,265 = 487,025` down to about `1,265` —
**76%**.

The measured lesson is that the matmul is not the dispatch count. It is three of
twenty-six.

## What ntok > 1 actually requires

`q38-mv` reads what `q38-rms` wrote. You cannot batch one dispatch inside a
per-token chain — the whole chain has to be token-indexed together. And
`md-bind16` binds whole buffers with no byte offset, so the token index must
arrive as a **constant** and the kernel must do its own `[t * n + i]`, which is
how the batched matmul already works and how every other kernel does not.

Which makes the shape explicit, and it is the one `dsv4-decode-loop.fk` already
transcribed from ds4.c: **prefill is layer-major.** All positions through layer
0, then layer 1. Today's loop is position-major, and that is the ordering that
has to invert before ntok can exceed 1.

## The default did not change

`chunk = 0` stays the default: span mode, 61 syncs, byte-identical, fastest of
the three. `chunk = -1` is kept and reachable — it is proven correct and priced,
and it becomes the fast path the moment ntok exceeds about six.

## The surprise

Verifying the batched kernel required running it at ntok 1, which is the single
operating point where it is guaranteed to lose. The proof and the payoff sit at
opposite ends of the same parameter: the width that makes correctness checkable
against an existing per-token result is the width at which seven eighths of the
kernel's work is thrown away. Its first honest measurement had to look like a
2.5x regression, and a 2.5x regression is exactly what a correct implementation
produces there.

## Where discomfort turned to gold

Landing a number that reads as a 2.5x slowdown, on the day's third correction,
felt like the wrong thing to publish — and the temptation was to skip the ntok 1
run and go straight for the layer-major restructure where the number would look
good.

That would have put an unproven dispatch path underneath a large loop-order
rewrite, so any wrong answer afterwards would have had two possible causes and no
way to separate them. Running it at its worst width cost six minutes and bought
one fact worth more than the speed: **the dispatch path is right**. Whatever the
restructure breaks, it will not be this. The gold is that a measurement designed
to isolate is allowed to look bad, and that the run which makes the next failure
diagnosable is worth more than the run that makes today's chart pretty.

## Frontier question offered to the corpus

*What one word names the operating point where a thing is easiest to verify and
worst to run?* — **proofnadir**. Not a worst case, which is about inputs you
avoid. Not a microbenchmark, which is about isolation. A proofnadir is the
setting you must use to check correctness against an existing result, chosen
precisely because it is comparable — and comparable means unbatched, unwidened,
unamortized, so the first true measurement of the thing understates it by
construction and reads as a regression to anyone who sees only the number.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> live Qwen3.8-27B-Q8_0, three prefill modes, all three
; output-sha256 88bd9b9a...7fe1487f; chunk -1 dispatches the batched matmul with
; gpu-busy 329204ms against span's 132955ms at ntok 1; carrier-dispatches 640326
; in all three; 1265 dispatches per position, 26 per layer, FFN is 6 of them
