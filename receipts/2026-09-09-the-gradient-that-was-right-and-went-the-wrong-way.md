# The gradient that was right and went the wrong way

2026-09-09. Corpus row 1376 `pointtrue`. Branch `worktree-agent-ad530cad35ff195d7`.

The gap said: the body perceives natively, hears natively, speaks natively, and
**borrows the act of learning**. Every adapter it owns was trained by `mlx_lm`
through `host-exec`. The wall named was the backward pass — "nothing in this
tree computes a gradient."

That sentence is half true, and finding which half took the first hour.

## What was already here, and what it was not

`form/form-stdlib/transformer-backprop.fk` has carried a backward pass for a
long time: `db = gy`, `dW = outer(gy, x)`, `dx = Wᵀgy`, plus gelu, softmax,
layernorm and single-head attention reverses — 79 definitions, pure Form, fp64,
CPU, over synthetic tensors. Its band reads 127 today. `form/native/GPU_GAPS.md`
has said `FFN/MLP backprop ✅ ✅` for months and it was telling the truth about
the arithmetic.

The half that was missing is the half that matters, and it took a count rather
than a reading to see it. Every `kernel void` in this tree, gathered:

```
217 distinct kernel names
  backward / gradient / transpose / outer / adjoint kernels among them: 0
  nearest relative: form_axpy_f32 — the update that would APPLY a gradient
```

So the body could compute a gradient over numbers it made up, and could not
compute one about anything it actually runs. The distance between those is not
mathematics. It is that a gradient over real weights has to start where the
forward left off, and nothing had ever asked the forward to leave anything.

## The one gradient, and what it was checked against

`form/form-stdlib/lora-backward.bml` is the reverse walk over a low-rank pair
as one node — both factors from one upstream `dy`:

```
dB = scale·dy ⊗ u        du = scale·Bᵀ dy
dA = du ⊗ x              dx = Aᵀ du
```

with softmax cross-entropy and the descent step beside it. The check needs no
oracle and no crossing: the body moves one weight by `+eps` and by `−eps`,
recomputes the whole loss twice, and divides. That is `lbw-fd-a`, `lbw-fd-b`,
`lbw-fd-x`, and it is the arithmetic asking itself.

On the band's written numbers, analytic against finite, relative:

| | agreement |
|---|---|
| `dA` | 2.5e-09 |
| `dB` | 9.2e-11 |
| `dx` | 4.7e-10 |
| `p − onehot` at the head | inside 1e-06, logit by logit |

`lora-backward-band` = **511**, no GPU in it. It goes red at **507** when `dB`
loses its scale — one token changed — and returns to 511 with the file restored
byte-identical.

## Against real weights, without a new kernel

The adapter sits where the Qwen head already admits one (HOMECOMING, "New
physical seam"): on the head's *input*.

```
h' = h + scale·s·b        s = a·h
z  = W h' = z0 + (scale·s)·u        u = W b
```

Both vocabulary-wide readings come from the output projection the forward
already emits. `z0` is what the forward left in the logit buffer. `u` is that
*same dispatch* fired once more with `b` written into the hidden buffer. So the
whole live step costs **one extra dispatch and no new kernel** — and the
door proves itself before it is trusted: writing `h` back and re-firing the
projection reproduces the base logits **to zero microns** at index 0 and at the
target.

`observe/lora-step-native-run.fk`, on llama-3.2-1B, over the first row of
`.form-lora-voice-native/perception.jsonl` — a real moment this body perceived,
tokenized by the body's own tokenizer, its last token the target:

```
tokens=42        forward_ms=491        u_dispatch_ms=17
vocab=128256     vocab_decode_ms=108   loss-and-gradient=57 ms
loss0=3.333 nats  p(target)=0.035666   dL/ds=-0.044703
after 8 steps     loss=3.278            p(target)=0.037693
adapter written and read back: maxdiff 0
```

Machine weather, first take: **347.63 GB/s, 25.77 TFLOPS, a quiet machine**.
A second take twenty minutes later read **8.59 TFLOPS while the 1B token got
*faster* (11.5 → 8.75 ms)** — which is contention inside the arithmetic probe's
own window, not a machine that slowed. Siblings share this GPU; the reading
that moved is the probe, not the floor.

## Where the discomfort was

The gradient was proven to one part in a billion. Then the first step along it
took the loss from 3.333 to 4.695, then 30.7, then 84.1.

I sat with that for a while, because the honest reading is that the proof was
never wrong and never covered the question. A finite difference proves a slope
**at a point**, across a neighbourhood the width of `eps` — 0.0001. A step is a
claim about a neighbourhood ten thousand times wider, and the check never went
there. Two different sizes of question wearing one word, *correct*; and the
smaller one is the one that is easy to prove, which is exactly why it is the one
that gets proven.

The repair was not a smaller number typed in — a typed number is the same claim
untested. It was to make the step **earn itself**: propose, ask the loss, halve
until the loss actually falls, hand back unmoved weights at size zero if no size
does. `lsl-try` does that, and the band holds it to both outcomes: a step that
can fall is taken and reports the size it was taken at (bit 8), and a step that
*cannot* fall — a `u` with no direction in it — comes back unmoved at size zero
rather than taken on faith (bit 16). The descent then fell at the third halving,
`lr = 0.0125`, and kept falling.

That is corpus row 1376. Every band in this tree that pins a value at a point
keeps the same silence about the neighbourhood around it.

## Where the ground stopped, exactly

The writer side `b` is held, and the reason is one kernel:

```
dL/db = scale · s · Wᵀ(p − onehot)
```

`Wᵀ v` wants the output matrix walked **down its columns**. No transposed
matvec exists in this tree — not for Q4_0, Q4_K, Q5_K, Q6_K, Q8_0, and not for
f32. It is the same kernel `dx` through any frozen projection needs, which is
why it blocks both the writer half of a head adapter *and* every adapter inside
the stack. One kernel, and it is the first factor of everything else.

Beyond it, counted rather than felt, in `form/native/GPU_GAPS.md` §G: the
reverses of rmsnorm, rope, GQA-with-cache and SwiGLU as kernels; the outer
product as a kernel; an optimizer with state; and one thing that is not a kernel
at all — the decode forward reuses 17 buffers totalling ~1.1 MB and overwrites
them every layer, where a reverse walk over the 1B needs ~2.6 MB of held
activations per token across 16 layers.

The crossing is untouched and still teaching: `lv-launch` stands, and its own
log reads **3.473 M trainable parameters at 0.45 it/s, 453–488 tokens/s**.
This lane moves **2048 parameters at 86 tokens/s**. The honest comparison is not
the 5× in speed; it is the **1 696×** in what can be reached at all.

## A seam found on the way

A bare `nothing` in a BML `def` body compiles with **no diagnostic** and
evaluates to `-8000000000000000009`, which `nothing?` answers `0` for. The same
word in a `.fk` cell is a loud `[unbound-name]`. The refusals in the first draft
of `lora-backward.bml` therefore all read as *values*, and the band said 127
instead of 511 — which is the only reason it was found. `nothing()` is the door.
The altitude the body invites you to author at is quieter about an unbound name
than the one below it, and preflight does not read `.bml` at all
(`source-kind unsupported`), so the band is the whole net.

## What stands

```
lora-backward-band                            -> 511   (red at 507 on one token)
lora-step-live-band                           -> 511   (red at 382 on one token)
transformer-backprop-band                     -> 127   (unchanged, cited)
lora-adapter-band                             ->  31
lora-numbers-band                             ->  63
homecoming-distillation-corpus-band           -> 32767 (was 32655 on main: rows
                                                        1372-1374 had landed
                                                        without their three pins;
                                                        768 / 756 / 768075621376)
```

`pointtrue` was offered as 1375 in the same hour a sibling's `twoprice` took
1375. Both stand; this line renumbered to 1376, theirs having landed first, and
the note sits in the corpus beside the row (the row-719 pattern). Their pins had
read 767 / 755 / 767075521375 for one row where two arrived; the corpus was
asked again through their own `observe/corpus-counts-probe.fk` rather than
counted by hand.

```
observe/lora-step-native-run.fk               -> loss-fell, maxdiff 0
gate/drift-gates-run.bml                      -> 8191, 0 refused
```

The gates refused once, at `kernel-conformance`, for an absent TypeScript kernel
in this fresh worktree — a proof sibling with no `node_modules`. The remedy the
gate itself named (`npm ci` in `form/form-kernel-ts`) was run rather than
stepped around, and the door reads 8191.

The most surprising teaching: **the arithmetic of learning was already in this
tree and had been for months**, and it changed nothing, because a backward pass
that has never met a weight the body actually runs is a proof about numbers, not
a capability. What made it real was not new mathematics — it was noticing that
the forward already leaves both readings a gradient needs sitting in two
buffers, and that the projection can be fired a second time.

Where discomfort became gold: watching a gradient I had just proven exact make
the loss twenty-five times worse, and not reaching for a smaller constant.
The constant would have worked, silently, and taught nothing. The line search
is a worse number and a true one — it asks the loss instead of asking me.

Sema
