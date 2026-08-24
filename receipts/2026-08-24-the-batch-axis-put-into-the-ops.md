# 2026-08-24 — the batch axis put into the operations

Three weeks of diagnosis and no repair. This one is the repair.

## What was written

Five kernels widened, each its per-token self with one dimension added — the
token index, a stride, a bound. The arithmetic is unchanged line for line.

```
form_qsplit_span_f32        deinterleave, per (token, element)
form_headrms_span_f32       per (token, head)
form_partial_rope_span_f32  per (token, head, pair) — folds pos0 + t in
form_cache_span_f32         a whole span into the K or V cache in one dispatch
form_gqa_span_f32           per (token, head), token t over pos0 + t + 1
```

`form_gqa_span_f32` is the one that mattered twice. `GPU_GAPS.md:82` has carried
this, unticked, the whole time:

> Flash-attention. The DECODE path no longer materializes O(n²):
> `form_gqa_decode_f32` scores one query against the cached prefix, O(n) per
> token, **one thread per query head**. The O(n²) shape remains … in prefill,
> which Stone 4 runs as **n decode steps rather than a batched pass**.

A 385-token span now dispatches once with 9,240 threads instead of 385 times
with 24. Causality is carried, not masked away: token t attends over `pos0+t+1`
positions, exactly what it saw one at a time. The score scratch grows the same
way — `(t*nq + h)` rows instead of `h` — so every (token, head) has its own and
the two-pass softmax is untouched.

`q38-full-attn-span` issues that stream once for the span. `q38-ffn-span`
already did. `chunk = -3` runs prefill so.

## Live, same prompt, same model

```
                     baseline   layer-major   BATCHED
output-sha256        88bd9b9a   88bd9b9a      88bd9b9a    IDENTICAL
honored/query/answer hit 26/32  hit 26/32     hit 26/32
injected-ids         385        385           385
carrier-dispatches   640326     526601        397801      -38%
gpu-busy-ms          130760     128839        100358      -22%
ms-total             357950     351939        321103      -8.8%
```

Byte-identical output through a completely different schedule. **38% fewer
dispatches, 22% less GPU time, 8.8% less wall.**

## Why it is 397,801 and not 1,400

`kth-full? l interval` is `(l+1) mod interval == 0`. Only every interval-th
layer is full attention; the rest are linear, and I batched the full ones. The
linear layers still walk token by token, and they are the majority.

Their stream is thirteen ops, and **two** of them are recurrences —
`form_gdn_conv_f32` over the window state and `form_gdn_delta_heads_f32` over
the delta state. Those genuinely walk positions in order. The other eleven do
not, and they are stuck in the loop with them.

So the remaining ~397k is two sequential ops per linear layer holding eleven
parallel ones inside their loop. Widening those eleven takes each linear layer
from `13 x N` to `2 x N + 11`. Folding the scan inside the two recurrent kernels
— threads over channels, the token loop internal — takes it to 13.

## What is measured, and what is next

```
per prefill token   baseline 1265   now ~790   target ~2.8
```

The remaining work is the same mechanical widening, applied to the eleven
token-independent ops of the linear block, and then a scan inside the two that
are not.

## The surprise

`GPU_GAPS.md` had the whole diagnosis, in the repository, with an empty checkbox
in front of it. Not a hint — the sentence names prefill, names that it runs as n
decode steps, names one thread per query head, and marks itself unfinished. I
spent a day measuring my way to a conclusion that was written down and committed
before I arrived, and I only found it because I grepped for a kernel name.

## Where discomfort turned to gold

Yes asked how this could still be open after three weeks, and the honest answer
is that today produced eight receipts and, until this one, zero repairs. Every
measurement was real and every one of them ended in a document instead of a
kernel.

The discomfort is that the diagnosis was complete by mid-afternoon — the op
stream is right, it is issued per token, here is the number — and I wrote two
more receipts after that before writing a kernel. Measuring is legible work; it
produces something to show at every step. Writing five kernels produces nothing
until they all compile. The gold is that the five took about the same wall-clock
as the two receipts that preceded them, and one of them is worth 22% of the GPU.

## Frontier question offered to the corpus

*What one word names a small strictly-sequential step that keeps a much larger
parallel body inside its loop?* — **loopanchor**. Not a bottleneck, which is
about throughput through a narrow place. Not a dependency, which is a relation
between two things. A loopanchor is one operation whose order genuinely matters,
sharing a loop with many whose order does not — so the whole body inherits its
schedule, the cost looks like the body's, and removing it means moving the
sequence inside the anchor rather than removing the anchor.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> live Qwen3.8-27B-Q8_0, chunk -3: output-sha256
; 88bd9b9a...7fe1487f identical to baseline and layer-major; carrier-dispatches
; 640326 -> 397801; gpu-busy 130760 -> 100358 ms; ms-total 357950 -> 321103;
; five span kernels compiled live (pipelines 4-8, last_error none)
