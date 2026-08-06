# 2026-07-30 (late) — the stream matches, and the reason it didn't was that we were too accurate

Urs: *"yes we need to match order."*

Done, and measured. **24 of 24 tokens identical to ds4's greedy continuation.** What it took was not
what I expected.

## There is no single "ds4 order"

Read by line, never guessed:

```
ds4.c:10246  dot_f32              two NEON accumulators, 8 elements a step, FUSED multiply-add,
                                  reduced by vaddvq_f32(vaddq_f32(acc0, acc1)) — a pairwise tree
ds4.c:6664   dot_f16_row          the same two-accumulator FMA shape over widened halves
ds4.c:3480   ds4_vec_dot_q2_K_f32 a plain ASCENDING SCALAR SUM. No NEON. No FMA. No blocking.
ds4.c:6814   dot_q8_0_row         not an f32 fold at all — see below
```

Four types, four different associations. And our metallibs are compiled `-ffp-contract=off`, so we had
**no FMA anywhere** while ds4 fuses on every product it can. A fused multiply-add rounds once where
ours rounds twice; over 4096 terms that is not something the tail absorbs.

## The one that mattered was not floating-point at all

`dot_q8_0_row` **quantises the activation to int8** (`quantize_q8_0_activation`, ds4.c:7051), dots each
32-element block **exactly in int32** via `vdotq_s32`, and lets only the per-block scale product back
into f32 through an FMA.

Our kernel kept `x` at full f32 precision. **Strictly more accurate — and that is exactly why it could
not reproduce the reference.** A reference's losses are part of its identity. Corpus row 951,
`betterwrong`.

That path is `attn_q_a`, `attn_q_b`, `attn_kv`, both output projections, the shared experts, and
`output.weight` — which is why `blk.0 q` was the very first stage to diverge.

## What the mirror bought

blk.0 against `ds4 --head-test`, with `FORM_DS4_MATCH_ORDER=1`:

```
             ds4                         ours
q         -10.9571  15.8696  0.999515    -10.9571  15.8696  0.999515   identical
kv         -4.15078  5.76746 0.634615     -4.15078  5.76746 0.634615   identical
attn_heads -4.15165  5.76466 0.474495     -4.15165  5.76466 0.474495   identical
```

Every printed digit. And the number that decides tokens:

```
                            max |delta| over ds4's top 64
ds4-cpu vs ds4-metal                  0.4835      <- the reference against itself
ours, fast folds                      0.6942      -> picked " It"   (wrong)
ours, MATCH_ORDER=1                   0.3915      -> picks " The"   (ds4's token)
```

**We are now inside the reference's own spread**, which is the bar I set in the previous receipt
(0.6942 → ≤ 0.4835) and beat.

## The stream

```
ds4   [11111, 16, 455, 6102, 294, 8760, 344, 11111, 16, 455, 6102, 294, 8760, 344, 11111, 16, 455, 6102, 294, 8760, 344, 11111, 16, 455]
ours  [11111, 16, 455, 6102, 294, 8760, 344, 11111, 16, 455, 6102, 294, 8760, 344, 11111, 16, 455, 6102, 294, 8760, 344, 11111, 16, 455, 6102]
```

Identical for all 24 of ds4's tokens (ours ran one step further). Decoded:
`" Paris. The capital of France is"`, repeating. 190 gates, VERDICT PASS, 43 layers, 3m15s for 29
steps.

## What this costs, said plainly

The mirror is **one thread per row** — 32 lanes and a `simd_sum` become a single thread walking a row
in ds4's order. Fidelity and throughput are the same dial. Both lanes stay, behind
`FORM_DS4_MATCH_ORDER`, because which end you want is a question and not a default.

## What is honestly still open toward 1000

- This continuation is a **period-7 cycle**. A cycle is the easy case: the same states recur, so it
  exercises far fewer distinct ties than 1000 varied tokens would. The claim is 24 matched tokens on
  this prompt, not 1000 on any prompt.
- **Past position 127 the ratio-128 layers begin emitting compressed rows.** That branch of
  `dsv4-compressor.fk` is written and banded but has never run on the device — no test here reached
  pos 127.
- Three folds are still un-mirrored: the **grouped Q8_0** output projection (`attn_out` still differs
  in the 3rd digit), **IQ2_XXS** (the routed gate/up, so `routed_moe` still differs at 0.16%), and
  the MXFP4/MXFP8 paths this file does not use.
- Our transcendentals are still ours: `mla_exp` is a range-reduced Taylor series where ds4 calls libm
  `expf`. Matching association does not match those, and no amount of order work will.

## The most surprising teaching

**Being more accurate made us wrong.** I spent the day making the lane faithful and the last barrier
was a place where we were *better* than the reference — full-precision activations against ds4's int8
quantisation. To reproduce a reference you must reproduce its losses, and an implementation can be too
good to agree. That is the exact mirror of `overfine` (row 934), where I demanded a precision the
arithmetic could not hold; here I supplied a precision the reference did not want.

## Where discomfort turned to gold

Reading `dot_q8_0_row` and realising the whole day's assumption — that our f32 lane and ds4's f32 lane
were computing the same quantity with different roundings — was false. They were computing *different
quantities*. Every "close but not equal" number I had produced since morning was measuring a gap I had
misdescribed. The discomfort was that nothing was wrong with our kernel, so nothing pointed at it; the
gold is the rule that follows: **when two implementations of the same formula disagree beyond their
noise, check whether they are implementing the same formula at all** — not the same intent, the same
arithmetic.

## Ground stamp

```
ds4.c:10246 dot_f32 / :6664 dot_f16_row / :6814 dot_q8_0_row / :7051 quantize_q8_0_activation /
  :3480 ds4_vec_dot_q2_K_f32 — all read whole
new cell form-stdlib/ds4-order-match.fk: q8a quantiser, ordered Q8_0 / F16 / Q2_K matvecs
blk.0 q, kv, attn_heads match ds4 to every printed digit under FORM_DS4_MATCH_ORDER=1
max |delta| over ds4's top 64 at prompt 7: ours 0.6942 -> 0.3915; ds4-cpu-vs-metal 0.4835
greedy stream from "The capital of France is": 24/24 tokens identical to ds4 --temp 0 --raw-prompt
harness 190 gates VERDICT PASS, 29 steps in 3m15s, 43 layers
corpus 345 rows, max-mid 950, field 3453452950, band 32767 — counts asked of the body
still open: period-7 cycle only; pos>=127 ratio-128 compressed rows never run on device; grouped
  Q8_0 and IQ2_XXS folds un-mirrored; our transcendentals are not libm's
```
