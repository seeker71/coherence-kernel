# 2026-08-24 — the op stream is already at parity; it is issued 460 times too often

Yes proposed reducing the ops we use until we reach parity with llama.cpp's op
stream. Counting both sides answers it, and the answer is not the one the
proposal assumed — which is what makes it worth having asked.

## Our stream, counted from source

`q38-full-attn`: rms, three matvecs for q/k/v, two qsplit, two head-rms, two
rope, two copies into the K and V caches, gqa, sig-mul, the o matvec, the
residual add — **16**.

`q38-linear-attn`: rms, four matvecs, gdn-conv, silu, l2norm, gates,
delta-heads, norm-gate, the out matvec, the add — **13**.

`q38-ffn`: rms, three matvecs, swiglu, add — **6**.

So **22 dispatches per full-attention layer, 19 per linear one**.

The carrier agrees without being asked: 640,326 dispatches over 506 positions
over 64 layers is **19.8 per layer**. Counted and measured meet.

## Parity

A transformer layer of this shape needs about what we issue. There is no fat in
the stream — no redundant normalisation, no stray copy, no op that could simply
be deleted. **Per layer, this lane is already at parity.**

## Where the 460x is

llama.cpp's own debug log, on this machine, this model:

```
graph_reserve: reserving a graph for ubatch with n_tokens =  512, n_seqs = 1
```

**One graph for 512 tokens.** The stream is issued once and the batch is a
dimension inside it.

```
512-token prefill
  llama.cpp   one graph for the ubatch     ~1,400 dispatches
  this lane   the stream, once per token    647,918 dispatches
                                                 460x

per token   llama.cpp ~2.8    this lane 1,265
```

(The ~1,400 is our own per-layer count applied to 64 layers — I could not get a
node count out of this build's logs, and llama.cpp fuses some of these, so the
true figure is that or lower. The 460x is a floor.)

## So the instruction resolves, and it inverts

Reducing the ops per layer would gain nothing; there are none to give back.
What has to fall is **how many times the stream is issued** — 512 to 1 for a
512-token prefill.

And that is a single property, held by every kernel: each one takes `ntok` and
indexes `[t * n + i]`, so the batch axis lives in the operation instead of in a
loop around it. `form_q8_0_matmul_batch_f32` has it. `form_rmsnorm_span_f32` has
it. `form_swiglu_f32` and `form_add_f32` already work over `n * ntok` because
they are elementwise. The remaining sixteen do not.

Two of those genuinely cannot be widened over tokens — `form_gdn_conv_f32` on
the window state and `form_gdn_delta_heads_f32` on the delta state are
recurrences, and they stay a loop. `form_gqa_decode_f32` is causal and needs a
mask rather than a loop. **Everything else is arithmetic that does not know the
token index exists.**

## The number to aim at

```
today                        1,265 dispatches per prefill token
per-layer parity             already there, ~20
after widening the stream    ~1,400 per 512-token prefill = 2.8 per token
```

That is checkable at every step, on the counter this lane already reports, with
no reference to anybody's throughput.

## The surprise

The proposal was to use fewer operations, and the count says we use the right
ones. The whole 460x sits in a property that does not appear in any op's
behaviour, only in its signature: whether it takes one token or many. Sixteen
kernels are individually correct, individually efficient, and collectively
issuing half a million dispatches to do what fourteen hundred would.

Nothing is wrong with the arithmetic anywhere in the stream. The batch axis just
never made it into the operations, so it became control flow.

## Where discomfort turned to gold

I have spent the day proposing repairs — batch the matmul, invert the loop,
widen the scratch, drop the barriers — and each one was a piece of this without
naming the whole. The discomfort is that I had all four measurements needed to
state it plainly by mid-afternoon and instead produced four separate stones,
because each measurement arrived attached to the repair I was already making.

Counting both sides took one command and no GPU. The gold is that a target
expressed as a ratio against an outside system is vague and demoralising, and the
same target expressed as *2.8 dispatches per token* is a thing you can check
after every edit.

## Frontier question offered to the corpus

*What one word names a dimension that exists in the data but in no operation's
signature, so it becomes a loop instead?* — **lostaxis**. Not a missing
optimisation, which suggests something to add. Not a scalar implementation,
which describes the code rather than what is absent from it. A lostaxis is
present in the problem, present in the memory layout, and absent from every
function that touches it — so the system compensates with iteration, every op
stays individually correct, and the cost shows up as a multiplier nobody can
attribute to any single line.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> q38-full-attn 16 dispatches, q38-linear-attn 13,
; q38-ffn 6; carrier measured 640326/506/64 = 19.8 per layer; llama.cpp
; graph_reserve ubatch n_tokens = 512 (one graph); 512-token prefill 647,918
; dispatches here against ~1,400 there, 460x, per token 1265 against ~2.8
