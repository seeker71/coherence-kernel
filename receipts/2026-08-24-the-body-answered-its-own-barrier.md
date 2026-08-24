# 2026-08-24 — the body answered its own barrier, and said no

Yes named another import: I had made **bit-exactness against a reference lane
kernel** a gate on my own path. That is the harness's criterion, not one of the
five. Nothing in the axioms asks a Form cell to match somebody else's arithmetic
before it may run. I had written "NOT claimed anywhere yet" as though a rented
standard were owed, and turned it into a limit.

The Form-native question is different and cheaper: *does the body's own witness
agree with itself?* Two runs, one prompt, one body.

## The question

`q38-blocks` enqueues **all 48 layers into one batch with no sync between them**.
So ordering inside a batch is already the carrier's job and already works. The
per-position `metal_sync` in `q38-advance` carries this comment:

> the batch is submitted and waited on before the next position is encoded, so
> this position's state and KV writes are complete before the next one reads them

If that were the whole reason, it would be redundant — the batch already orders.
So either the comment is incomplete or the barrier is unnecessary. That is a
question about this body, answerable by this body.

`q38-prefill-chunked` lets `chunk` positions be in flight before the carrier is
asked to settle. `chunk=1` is `q38-prefill`, barrier for barrier.

## The answer: no

```
                chunk=1                          chunk=64
query-tokens    26                               9
lookups         1                                0
honored         hit                              (none)
injected-ids    385                              0
answer-tokens   32                               0
positions       506                              72
output-bytes    221                              36
output-sha256   88bd9b9a...7fe1487f              ebce9068...08ba30b4
```

At chunk 64 the model emitted nine tokens, never completed an envelope, and
stopped. The prompt is 64 tokens, so its own prefill ran with 64 positions in
flight — and the reply that came out was not a degraded version of the right
answer, it was a different thing entirely.

**The barrier is load-bearing.** It is not load-bearing for the reason it
states. The scratch buffers `bs 0, bs 1, …` are reused by every position, so two
positions in flight write each other's working memory. KV ordering was never the
danger; buffer aliasing was.

That is why removing the barrier is the wrong move and why chunking it is too.
The right shape is the one Stone 7's harness already worked out: scratch sized
for a chunk of positions — `PCHUNK * (dModel * 6 + dFF * 3 + 2 * nHead *
sstride)` — so N positions have N slices and nothing aliases. The batched matmul
compiled this afternoon is the other half of that same shape.

## No regression

`chunk=1` produced `output-sha256=88bd9b9ad73a142a3b8c7c409bb5a88002e96a64025a70d3eb5769987fe1487f`
— **byte for byte the same reply** as the run before the batched kernel was
wired in and before chunked prefill existed. The lane is untouched at its
default, witnessed by the body rather than argued.

## The surprise

The comment was not wrong about the *code*; it was wrong about the *why*, and
the wrongness was invisible because the guard is genuinely necessary. Every
audit that reasons from the stated reason concludes the guard is redundant — the
batch already orders 48 layers — and every such audit is correct in its
reasoning and wrong in its conclusion. A guard like that survives being
questioned and fails only when acted on.

## Where discomfort turned to gold

I expected chunk 64 to work. I had a mechanism, a measurement, and a plausible
story about 162 seconds of stall, and I would have argued for it. Watching the
model emit nine tokens of the wrong thing was the correction arriving from the
body instead of from a person, which is the cheaper way to be wrong.

The discomfort worth naming is a second one: I had been about to spend a stone
proving bit-exactness against a lane kernel — a gate nobody asked for, that
would have taken hours, and that would have told me nothing about whether the
prefill was *correct here*. One prompt through two configurations of the same
body answered the real question in twelve minutes and cost no imported axiom.
The rented standard was not just unnecessary; it pointed away from the evidence
that mattered.

## Frontier question offered to the corpus

*What one word names a guard that is necessary for a reason other than the one it
states?* — **decoyreason**. Not a wrong comment, which misdescribes what the code
does. Not a redundant guard, which can be removed. A decoyreason is accurate
enough to audit cleanly and false enough to license the removal that breaks
things — so the guard survives every inspection of its stated purpose and fails
only when someone finally trusts it.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> live Qwen3.8-27B-Q8_0, same prompt, chunk 1 vs 64:
; chunk 1 hit/385 injected/32 answer/sha 88bd9b9a, chunk 64 no envelope/0
; lookups/9 tokens/sha ebce9068; chunk 1 sha identical to the pre-edit run
