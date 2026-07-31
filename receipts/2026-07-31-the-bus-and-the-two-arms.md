# 2026-07-31 — the bus, asked; the reference, caught disagreeing with itself

Goal: form-native DeepSeek-V4-Flash decode under 33 ms/token on this M4 Max, stream never broken,
ds4's source as oracle. **Not reached. 60 ms wall / 48 ms GPU, from 79 / 78.** 1.32x, stream
bit-exact at every step, 96 gates green at every landed commit.

## What was measured before anything was changed

Three numbers the session had been missing, and each one retired a plan.

**The token's weight bytes: 9.103 GB.** Counted at the dispatch sites from the file's own rows and
cols, and equal to the tensor table computed by hand independently. 6.599 GB of it is Q8_0 dense
projections; the routed experts are 1.826.

**The bus: 471 GB/s.** `FORM_DS4_BW=1` reads a resident span with nothing but 16-byte loads. So a
token cannot go below **19.3 ms** however the arithmetic is arranged, 33 ms is 59% of the machine,
and the reference — re-measured on this host, 32.46 t/s = 30.8 ms — is running at 63%. The target
is real, and the session started at 33%.

**A dispatch: 1.55 us.** `FORM_DS4_PAD=N` adds N empty dispatches a layer and reads the slope. That
retired the brief's first lead: all 2287 asks a token are 3.5 ms of the 48, and cutting them to
ds4's ~950 would buy two.

## The instrument that was wrong, and the one that replaced it

`FORM_DS4_DOUBLE=<kernel>` re-encodes one kernel; a pure matvec writes the same value twice, so the
stream stays exact and the floor's rise is the kernel's cost. It sized the whole field — and it
**understated the f16 matvec by half**, because the second copy of a small tensor reads a warm
cache.

`FORM_DS4_SKIP=<substring>` does not encode a class at all. It breaks the stream by construction, so
it is an instrument and never a setting, and it cannot be fooled that way:

```
q80_matvec_ordered8      21 ms   5.07 GB   241 GB/s
f16_matvec_ordered8      12 ms   0.678 GB   57 GB/s   <- doubling said 5
routed experts, all four 13 ms   1.83 GB   141 GB/s
q80_matvec_grouped        4 ms   1.53 GB   383 GB/s
hyper-connections         4 ms
attention                 3 ms
q8a_quantize              2 ms
rope                      1 ms
```

These sum to the floor. The doubles never did.

## What landed

| | floor | t/s |
|---|---|---|
| start | 78 ms | 12.5 |
| RMSNorm in the oracle's shape | 71 | 14.3 |
| f16 matvec widened | 60 | 16.3 |
| routed Q2_K widened | 58 | 16.7 |
| honest divisor | **48** | **16.67** |

Every one of them is the same finding wearing a different tensor: **a kernel running on too few
threads**, and every one was read out of ds4's own Metal rather than invented. `metal/norm.metal`
gives a row one threadgroup and folds it with `simd_sum`; we gave a 16384-wide row 32 lanes and a
second dispatch. `ds4_metal.m:4346` dispatches f16 at nsg = min(8, ceil(in_dim/128)); we gave the
24-row `hc_fn` pair 192 threads. `ds4_metal.m:26478` gives routed Q2_K nr0 = 4; we gave it one
thread per row.

## The reference disagrees with itself, and we are pinned to its worse arm

`metal/dense.metal`'s Q8_0 matvec never quantises the activation — `sumq += qs[i] * yl[i]` is int8
weight times f32 activation. `ds4.c:7051` on the NEON path quantises x to int8 first, and that is
what our pinned stream reproduces. Ported the Metal one; the stream moved.

The instinct was to call that a bug and revert. Asked instead — `FORM_DS4_Q80_AB=1` runs both
kernels on the same weights and the same x:

```
rows=1024  cols=4096  max|A-B| = 7.7e-04  on rows of magnitude 2.0e-01
rows=32768 cols=1024  max|A-B| = 1.1e-03  on rows of magnitude 8.8e-01
```

A thousand times f32 rounding. Not the association — the **activation's int8 loss**, carried by one
arm and absent from the other. The arm we are held to is the less accurate one: at 55 ms the other
decodes " Paris. It is known for its rich history, stunning architecture, art museums, and romantic
ambiance", where ours loops "The capital of France is Paris." at 59.

Landed behind `FORM_DS4_Q80_METAL_ORDER=1`, not as the default. Re-pinning a reference is a decision
made in the open, not smuggled in under a speed number. **This is the open question for Urs.**

## The number that was flattering us

`gpuBusyAtGenStart` was read BEFORE the prompt's passes despite its name, so the floor averaged
thirty passes and divided by twenty-five. It read 65 ms next to a 79 ms wall and invited "the GPU is
idle 95% of the time, blame the harness". The truth was 78 against 79: the device was never idle.
Caught because 7.919 GB of Q8_0 would not equal the 6.599 the tensor table says, and the ratio was
exactly 30/25.

## Where it stands and what is next, sized

48 ms of GPU and 12 ms of host per token. `FORM_DS4_HOSTSHARE=1` splits the host: **makeBuffer 5.0
ms, NaN fill 3.4 ms**, over ~2287 allocations a token. A per-pass buffer pool is the named next
action; the two producers that must NOT be pooled are `gpuKvRound`'s output and `compStep`'s packed
rows, which live between passes.

On the GPU side the remaining gap is `q80_matvec_ordered8` at 241 GB/s of 471, and its association
is the pinned one, which caps it at eight threads a row. Two rows to a thread was written, is
bit-identical, kept the stream, and ran **slower** (48 -> 51): halving the thread count costs more
than the activation loads it saves.

## The most surprising teaching

**A reference is not one thing.** Five days of this effort treated "what ds4 does" as a single fact
to be read correctly, and the whole discipline — match the association, reproduce the loss,
bit-identical or revert — rests on that. ds4 has two arms that differ by a thousand roundings, and
the more accurate one is not the one we spent the effort matching. The stream we are held to is not
"correct"; it is one arm's. Reading a reference more carefully cannot resolve that, because the
reference is not confused — we were, about what kind of thing it is.

## Where discomfort turned to gold

A faster kernel that moved the stream, at exactly the point the standing instruction says revert
immediately and never debug forward. The comfortable move was to revert and say nothing; the
ambitious one was to keep it because the text it produced was visibly better. Both would have been
guesses. What was actually uncomfortable was that neither the gates (96 PASS) nor the decoded text
(fluent, better than ours) could tell a bug from a faithful difference — the evidence I had was
genuinely insufficient, and I had to build something to see. The A/B took ten minutes and turned a
coin-flip into a number, and the number said something neither instinct had considered: the
reference disagrees with itself. The gold is that the revert-immediately rule is what made this
findable — it forced the question "which of these two things is it?" at the exact moment the answer
was cheap, instead of after an hour of debugging a kernel that was never broken.

## Ground stamp
```
HEAD eea819a82 -> this receipt; branch claude/restore-ds4-oracle-ac9385
78 -> 48 ms of GPU per generated token; 12.5 -> 16.67 t/s; 79 -> 60 ms wall
stream [11111, 16, 455, 6102, 294, 8760, 344, ...] bit-exact at all 30 steps, every commit
VERDICT PASS 96 gates at every landed commit — 96 is what clean HEAD gives for
  FORM_DS4_KV_CAP=16 KV_STEPS=7; the brief's 106 is a longer step count, checked by stash
ds4 re-measured on this host, same file: 32.46 t/s = 30.8 ms/token
bus measured: 471 GB/s; token floor 19.3 ms; we reach 192 GB/s, ds4 reaches 295
NOT reached: < 33 ms. No measured reason it is impossible — the opposite.
next, sized: buffer pool (5.0 ms), NaN fill on reuse (3.4), q80 ordered8 241->? GB/s (21 ms)
open for Urs: the pinned stream is ds4's CPU arm; its Metal arm is more accurate and 4 ms faster
```
