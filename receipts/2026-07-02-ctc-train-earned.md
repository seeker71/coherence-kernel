# 2026-07-02 — earning it: a CTC decoder trained from wrong to zero error, four-way

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 07:18, on the proven-but-untrained CTC forward: "why not earn it." So earned: the loss
becomes a training signal, and a decoder learns its target — from wrong output to zero error,
natively, four-way-proven.

## What was built

`model/ctc-train.fk` closes the loop `model/ctc-loss.fk` opened. Logits → softmax → the CTC
forward loss; the gradient `∂loss/∂logit` is taken by **central finite difference** (numerically
exact to O(eps²), no hand-derived formula to get wrong); gradient descent drives the greedy
decode to the target.

## Witness — four-way, verdict 127 (fkwu = Go = Rust = TS)

`model/tests/ctc-train-band.fk`:

**Single-label — target "a", T=2, SYMMETRIC init favoring blank, 300 steps, no init trick:**
- before: greedy decode `""` → **WER 100**
- after: greedy decode `"a"` → **WER 0**
- loss strictly decreased.

**Ordered — target "ab", T=4, ADVERSARIAL init (frame 0 nudged toward b, frame 3 toward a — the
WRONG order), 400 steps:**
- before: decode `""` → WER 100
- after: decode `"ab"` → **WER 0**, loss down, trained loss < 0.7.
- The tiny fixed asymmetry only breaks the exact-symmetry saddle (the deterministic stand-in for
  the random init real training uses; `Math.random` is forbidden here for reproducible receipts).
  That it converges to "ab" *from the wrong-direction nudge* proves the **gradient** learned the
  correct ordering and the skip transition — the init did not hint the answer.

```
fkwu: 127   go: 127   rust: 127   ts: 127
```

The whole training run — softmax (`tn-exp`), the alignment-sum forward, `-ln` (`fln`), 700 total
gradient-descent steps — is bit-exact across four independent kernels. Go took 3.6s tree-walking;
fkwu 0.42s.

## What this earns, and what it does not

Earned: **the CTC objective demonstrably TRAINS a decoder to zero error, natively and
four-way** — not a shelf ornament. The forward that `ctc-loss.fk` proved is now a live learning
signal that moves a real error number from 100 to 0.

NOT earned (the honest floor, unmoved): the decoder is **free logits** — a per-frame class-score
lookup table, NOT a neural encoder over real signal. So this earns "the loss trains a decoder,"
NOT "the body decodes real speech." The real native open-speech WER is still 100. And the
gradient is **numerical** (O(params) loss-evals/step) — correct and fine for toys, but the
analytic forward-backward gradient (`softmax − alignment-posterior` via α·β) is the scaling rung,
still ahead. Two rungs now stand where last night there were none: the forward objective
(`ctc-loss.fk`) and that it trains (`ctc-train.fk`). The climb to a real WER is the encoder and
real signal between them.

## The most surprising teaching this work left behind

The gradient beat the initialization. Trained from a nudge pointing at the *wrong* answer, the
decoder still learned "ab" — the clearest proof that the learning signal, not the setup, did the
work. In a body whose every prior "learning" cell was pre-aligned prototype interning (the
adversarial ingest pass's exact finding), this is the first cell where a WRONG starting guess is
*corrected by its own objective*. That is the difference between a lookup table and a learner,
and tonight the body crossed it.

## Where discomfort turned to gold

"Why not earn it" landed as a rebuke of the honest-floor comfort — I had proven a forward and
stopped, calling the gradient "the next rung" as if naming it were the same as building it. The
discomfort was that the honest-floor language can become a hiding place: *pending is honest*, but
"pending" repeated over a thing you could build tonight is just deferral wearing honesty's coat.
Witnessed, it became the rule this receipt earns: name the floor, then take the next step off it
in the same breath. The floor is honest; standing on it forever is not.
