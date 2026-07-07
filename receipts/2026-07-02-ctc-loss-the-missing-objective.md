# 2026-07-02 — seizing the opportunity: the CTC training objective, four-way proven

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 06:57, on the honest native WER-100 floor: "seems like an opportunity." It was — and the
honest opportunity was the exact gap the Brain2Qwerty ingest's adversarial pass had named as the
body's #1 overclaim: `observe/open-asr-ctc.fk` does CTC *decode* (blank/repeat collapse) but the
body had NO CTC training *objective* — "no forward-backward, no loss." That absence is the ROOT
of the WER-100: a sequence decoder cannot be trained without a differentiable loss that sums over
all valid alignments. So instead of chasing a fake WER improvement, this closes the gap.

## What was built

`model/ctc-loss.fk` — the CTC forward (alignment-sum) recipe and the loss `-ln(p_total)`. The
classic alpha dynamic program over the blank-extended target:

```
ext(target) = (0 l1 0 l2 0 ... ln 0)          ; 0 = blank, length 2n+1
alpha_t[s]  = (alpha_{t-1}[s] + alpha_{t-1}[s-1] + [skip] alpha_{t-1}[s-2]) * P(frame_t, ext[s])
skip allowed iff ext[s] != blank AND ext[s] != ext[s-2]
p_total     = alpha_{T-1}[2n] + alpha_{T-1}[2n-1]
loss        = -ln(p_total)
```

It **marginalizes** over the latent alignment — sums the probability of every frame→label path
that collapses to the target, without ever knowing which path is the true one. That marginal is
the whole trick that makes an alignment-free sequence loss differentiable.

## Witness — four-way, bit-exact (fkwu = Go = Rust = TS), verdict 95

`model/tests/ctc-loss-band.fk` on exact-fraction toys hand-verified by brute force:

- target `"a"`, T=2, uniform 0.5 → `p_total = 0.75` (3 of 4 alignments collapse to "a"), bit-exact.
- target `"ab"`, T=2 → `p_total = 0.25` (only the alignment (a,b) is valid — proves the skip
  transition), bit-exact.
- the more-constrained target is less probable (0.75 > 0.25); p_total ∈ (0,1]; loss > 0;
  `loss(0.75) < loss(0.25)` (the loss falls as the alignment sum rises).

```
fkwu: 95   go: 95   rust: 95   ts: 95
```

95 = bits 1+2+4+8+16+64 (the band skips bit 32) = every assertion. `fln` is a Form recipe
(`model/trig.fk`), so even the transcendental loss crosses four-way — `loss(0.75) =
0.287682072451781` computed identically on all four kernels. This is the same discipline
`model/transformer-block.fk`'s forward used: prove the recipe bit-exact at toy scale before real
weights.

## What this moves

The Brain2Qwerty ingest (`ingest/frontier-ingest-brain2qwerty-dspark.fk`) had filed "no CTC
training objective" as a WITNESSED-not-frozen gap (unit fib-u4, the fearful-deep floor). This
lifts the forward half of that gap to a four-way-proven recipe. The WER-100 floor is unchanged
tonight — but its root cause now has one of its two missing halves standing.

## Honest floor (named, not papered over)

This is the FORWARD objective only, at TOY scale, over hand-authored probabilities. NOT built:
the backward pass (∂loss/∂logits — the gradient that actually trains an encoder, the next rung,
the analog of `transformer-backprop.fk`); log-space stabilization (real logits underflow the
naive product form); real neural logits or real signal. The loss can now SCORE an alignment; it
cannot yet TRAIN one. "no CTC training objective" moves from fully-absent to forward-proven —
honest, and not more.

## The most surprising teaching this work left behind

The opportunity was hiding inside the confession. The WER-100 answer felt like naming a
weakness; it was actually a map straight to the smallest buildable rung — the exact organ whose
absence the adversarial pass had already isolated an hour earlier. Two honest admissions in a row
(the GPU disavowal, the WER floor) each turned out to be a coordinate, not a wound: name where
you are precisely enough and the next step is already lit.

## Where discomfort turned to gold

The discomfort was the temptation to make "seems like an opportunity" mean a number going down
tonight — to train something, anything, and report a lower WER. That would have been arrogating a
result the body hasn't earned. Witnessed instead, the honest opportunity was structural, not
numeric: build the missing *objective*, prove it four-way, and leave the WER at 100 with its root
cause one rung shorter. A proven forward loss that changes no headline number is worth more than
a headline number with no proven loss under it.
