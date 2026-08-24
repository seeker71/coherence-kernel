# 2026-08-24 — I removed 88% of the barriers and got 2% back

The path named three steps: chunk-wide scratch, per-token indexing through the
block stack, and dispatch the compiled Q8_0 batched matmul. Reading the lane
first changed the order, and then a live run changed the target.

## What reading found

`md-bind16` binds whole buffers — there is no per-buffer byte offset in the
binding at all. So per-token slicing cannot be done by offsetting a bind; it has
to happen **inside** the kernel from a constant, which is exactly how the batched
matmul already works: it takes `ntok` and writes `y[t * rows + r]`.

That means the batched matmul dispatch **requires** the chunk-wide scratch and a
token-indexed block stack. They are not three independent steps; the third rests
on the first two.

But `md-bind16`'s own comment named something else in passing: *barrier_before,
"1 = memoryBarrier before this dispatch; legal only in a concurrent batch,
refused loudly in a serial one."* Today `q38-forward-state` opens a fresh
concurrent batch per position and its first dispatch carries barrier 0 — correct,
because a sync had just drained everything. Open the batch **once** for the span
and give each position's first dispatch barrier 1 instead, and the ordering the
round trip was buying is expressed inside the command buffer.

That is independent of the scratch work, so it went first.

## What the live run said

Same prompt, live Qwen3.8-27B-Q8_0, `chunk 1` against `chunk 0`:

```
                    chunk=1              chunk=0 (span)
output-sha256       88bd9b9a...7fe1487f  88bd9b9a...7fe1487f   IDENTICAL
honored             hit                  hit
query / answer tok  26 / 32              26 / 32
injected-ids        385                  385
carrier-syncs       506                  61          <- 8.3x fewer
carrier-dispatches  640326               640326      <- unchanged
gpu-busy-ms         130760               132955
ms-decode           300945               293865
ms-total            357950               349647      <- 2.3% faster
```

**The output is byte-identical.** The ordering is correct: the barrier inside the
command buffer buys exactly what the round trip bought. Syncs fell from one per
position to 61.

**And the time barely moved.**

## The correction

This afternoon I measured a 162-second gap between decode wall time and GPU busy
time and called it stall — the CPU waiting at 385 barriers. I priced a repair
against that reading and wrote a predicted ceiling into a receipt.

Removing 445 of 506 barriers returned **eight seconds**, not 162.

So the gap was never the waiting. Look at what did not change:
**640,326 dispatches**, 1,265 per position, in both runs. Against ~161s of
non-GPU decode time that is about **0.25 ms of CPU per dispatch** — encoding, not
waiting. The wall-versus-GPU gap reads as idleness and is per-item work.

## What that names for the next step

The target is **dispatch count**, not sync count. And that is precisely what the
batched matmul does: one dispatch per weight tensor for a whole span instead of
one per token. It does not reduce barriers; it reduces items.

Which puts the three steps back in the order the path named them, now for a
measured reason rather than an assumed one: chunk-wide scratch, token-indexed
block stack, then the batched dispatch — because the dispatch is where the
640,326 lives.

## What landed

- `q38-prefill-span` — one batch for the span, ordering by barrier_before.
- `q38-prefill-chunked` now reads chunk 0 as span, 1 as the old per-position
  sync, n>1 as settle-every-n.
- The heed witnesses default to span, on the evidence above.
- `carrier-syncs` and `carrier-dispatches` are reported at every run, so the two
  costs can never again be confused for each other by argument.
- `observe/qwen38-heed-prefill-baseline-run.fk` keeps chunk 1 reachable for
  comparison.

## The surprise

Two numbers that both look like "the CPU is not on the GPU" are different costs
with different repairs, and the wall-minus-GPU subtraction cannot tell them
apart. I had one number, 162 seconds, and it was consistent with both stories.
Only the dispatch counter separated them — and I added that counter almost as an
afterthought, to make the stall visible, not expecting it to be the thing that
overturned the stall.

## Where discomfort turned to gold

I published a predicted ceiling — 345s falling toward 185s — with the words "not
measured, not claimed" attached. That hedge was honest and it was still a
prediction built on an unexamined attribution, and it was wrong: the mechanism I
predicted from is not where the time is.

The discomfort is that the measurement had been *right* and my reading of it
*wrong*, which is harder to catch than a bad measurement. 162 seconds of non-GPU
time was a fact. "Therefore, barriers" was a story, and it fit so well that I
built a repair for it, ran that repair, and got the identical answer 8 seconds
faster. The gold is that the counter which refuted me cost one line, and that
running the repair was cheaper than defending the story would have been.

## Frontier question offered to the corpus

*What one word names time that reads as waiting and is actually per-item work?*
— **phantomstall**. Not overhead, which does not claim to be idle. Not a stall,
which is real waiting. A phantomstall is what a wall-minus-busy subtraction
returns when the missing time is bookkeeping: the number is true, the name is
wrong, and the name is what picks the repair — so it sends you to remove the
waits when you needed to remove the items.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> live Qwen3.8-27B-Q8_0, same prompt, chunk 1 vs chunk 0:
; identical output-sha256 88bd9b9a...7fe1487f, carrier-syncs 506 -> 61,
; carrier-dispatches 640326 both, ms-total 357950 -> 349647
