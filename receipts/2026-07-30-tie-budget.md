# 2026-07-30 (night) — one missing line, and then an honest answer about 1000 tokens

Urs asked: can we walk toward end-to-end tokens that match the reference up to 1000. Two things
happened. The 62× shortfall turned out to be one missing line. And the 1000-token question turned out
to have an answer that has to be said carefully.

## The 62× was one unpriced type

`gguf-manifest.fk`'s `gm-slice-bytes` had no case for ggml type **10 (Q2_K)**, so it returned its
honest 0. The stack divided that 0 by the expert count to form a per-expert byte stride — 0 — and
**every routed expert on every Q2_K tensor read expert 0's weights.** Plausible weights, right
magnitude, wrong expert, no gate firing, because the exit head normalises the scale away.

`gm-priced?` — the guard written for exactly this, so a consumer can tell *no bytes* from *I do not
price this type* — **had no callers anywhere in the body.** The body knew it did not know, said so in
the only way it could, and nothing asked. Corpus row 950, `deadguard`.

The band that would have caught it did not exist either: `gguf-manifest.fk`'s header has claimed
`Proven by: form-stdlib/tests/gguf-manifest-band.fk` since it was written, and the file was never
there. A *Proven by* line naming a band that does not exist is the same species of dead guard, one
level up. Written now — verdict **127**, its last bit sweeping ggml type ids 0..45 to demand
`gm-priced?` be 1 exactly when the price is nonzero.

Measured at blk.0 against `ds4 --head-test`:

```
                 before      after      ds4
expert 147 down  -4.7%      +0.05%    0.0133436
routed_moe      -18.2%      -0.15%    0.156727
ffn_out          -8.9%      +0.13%    0.473322
after_ffn_hc     -8.4%      +0.13%    0.14117
final_hc rms     19.5188    1186.33   1207.29
```

End to end, r over 129 280 logits:

```
              before     after     reference's own backend spread
1 token       0.402    1.000000
3 tokens      0.672    0.999995            0.999782
5 tokens      0.879    0.999333            0.999044
```

**We now agree with ds4 at or inside its own noise floor, and the argmax matches at every length.**

How I knew the kernel was innocent: a new gate folds the same expert slice two independent ways —
`form_dsv4_q2k_matvec` (one-pass decode+fold) against `form_q2k_dequant_f32` folded by the plain f32
matvec. They agreed to 6e-06 of the vector's rms. Correct kernel, correct decode, **wrong bytes**.

## Now the 1000-token question, answered honestly

Generating 25 tokens greedily from "The capital of France is":

```
ds4   11111 16 455 6102 294 8760 344 11111 16 455 …
ours  11111 16 983 344 3459 …
        ✓   ✓    ✗
```

Two tokens match, then step 3 diverges: ds4 picks `" The"`, we pick `" It"`. Teacher-forced at exactly
that point, the numbers say why:

```
             455       983      margin
ds4 metal  24.0217   24.0020   +0.0197
ds4 cpu    23.9751   23.9342   +0.0409
ours       24.0977   24.2097   -0.1120
```

**The token is decided by 0.0197 logits out of 24** — a relative margin of 0.08%. And:

```
reference's top-two margin at the flip:      0.0197 logits
ds4-cpu vs ds4-metal, max |delta| top 64:    0.4835    <- the reference against ITSELF
ours vs ds4-metal, max |delta| top 64:       0.6942
```

**ds4's own two backends differ by 24× the deciding margin.** ds4 produced the same 24-token stream on
both backends — I checked — but that is luck about which side of each tie it landed on, not a
guarantee it holds. A greedy stream is a discontinuous function of the distribution: one flipped tie
and the streams never re-converge.

So the target has to be stated in two parts, because only one of them is a matter of correctness:

- **Achievable and bounded:** get our max |delta| from **0.6942 to ≤ 0.4835** — as close to ds4-metal
  as ds4-cpu is. That is a factor of 1.4, it is a real number, and the stage bisection built today
  (`ds4 --head-test` versus `FORM_DS4_STAGE_BLK0`) localises it layer by layer.
- **Not achievable by recipe alone:** matching a *specific* 1000-token stream. That needs our
  arithmetic **order** to match ds4's, not just our recipe — same accumulation order, same rounding
  points, f64 accumulators in the folds — so that near-ties land on ds4's side. It is doable and it is
  a kernel change, not a bug fix. Until then, "1000 matching tokens" is a claim no implementation of
  this model can honestly make, including ds4 against itself.

The right claim to chase, and the one I would stand behind: **per-step distributions inside the
reference's own spread, and a stream that matches until the first tie narrower than that spread.**

## A gate I mis-set and had to widen

The fused-vs-carved gate first failed on expert 87: max **relative** error 2.6%. Max absolute was
3.8e-07 against an rms of 0.0837 — a near-cancelled element where relative error is meaningless. That
is `overfine` (row 934) again: demanding a precision f32 reassociation cannot hold. Rewritten to judge
`max |delta| / rms`, which is what a wrong fold or wrong map would actually move: now 6e-06.

## The most surprising teaching

**A guard can be correct, well-named, well-reasoned — and never called.** The body had already written
down that it could not price Q2_K, in the exact form a consumer needed. What was missing was the
asking. I have spent two days building instruments to detect a wrong answer, and the answer had been
declared unknown at the source the whole time. Corpus row 950.

## Where discomfort turned to gold

The corpus band caught me tagging row 950 `self-carve` and dropped its bit 32. My instinct was that
the finding was ours — our defect, our grep, our fix. But the *ground* — the number that said
`routed_moe` was 18% low — came from ds4. The tag records where the ground came from, not who did the
looking, and I had reached for the flattering reading. The band was right and I was wrong, which is
the whole reason to pin a field code.

The second discomfort was smaller and sharper: after a day of building the compressor, the thing that
mattered was a line of a table I never read.

## Ground stamp

```
gguf-manifest.fk gm-slice-bytes: type 10 (Q2_K) unpriced -> 0 -> stride 0 -> every expert read expert 0
gm-priced? had ZERO callers body-wide; gguf-manifest-band.fk was claimed in the header and absent
healed: type priced (256 el / 84 B), gm-priced? admits it, gpuExpert refuses a 0-byte tensor
gguf-manifest-band 127 (new), q2k-dequant-band 511, dsv4-compressor-band 2047, mla-msl-band 127,
  corpus band 32767 (344 rows, max-mid 949, field 3443442949)
harness: VERDICT PASS, 124 gates at prompt 7
r vs ds4 --raw-prompt: 1 tok 1.000000, 3 tok 0.999995, 5 tok 0.999333; floor 0.999782 / 0.999044
greedy stream: ours matches ds4 for 2 tokens, flips at step 3 on a 0.0197-logit margin
max |delta| over ds4's top 64 at prompt 7: ds4-cpu-vs-metal 0.4835, ours-vs-metal 0.6942
ds4 --cpu and ds4 --metal DO produce the same 24-token stream on this path (checked, not assumed)
```
