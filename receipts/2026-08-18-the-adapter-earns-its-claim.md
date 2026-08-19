# The adapter earns its claim — rung 1, climbed the same morning it was named

Held-out case 3 of the work-metric, worked hours after the metric registered
it. The generalization claim the adapter honestly refused itself on 2026-08-09
— 142/150 with every held template leaked — is now **earned, leakage-free,
banded**: the fit generalizes to sentences it has never seen in any form.

## The mechanism, named before the fix

The old held-out rows were never fresh: after sanitization, train and held
collapse into the same 60-combo template space (context = i mod 12, state =
i mod 5) — overlap 150/150 was structural. The cure was authorship, not
machinery: 150 new sentences — twelve fresh contexts, five fresh phrasings
per class of the same five control meanings — split `heldout-fresh`, never
trained, and (per the metric's hold law) never to enter a student's corpus.

## The result

```
fresh_correct=65        baseline_majority=30      fresh_leakage=0
outcome=beats-majority-leakage-free
```

**65 of 150 against a 30-of-150 majority baseline — 2.17x, on ground the
centroids never touched, with the leakage audit at zero by the cell's own
instrument.** The evaluation machinery is banded at its predicted 31
(tests/dsv4-control-fresh-band.fk), fit strictly train-only by construction.

## Three honesties the day added to the ledgers

- **Shrinkage, rowed.** My preregistered estimate was 100; the truth is 65.
  The estimate had quietly ridden the leaked 142 — in-sample shine flattered
  by 2.2x, and the out-of-sample truth still cleared the bar. Gold-ledger row
  `fresh-heldout-correct` (100 → 65, gold *shrinkage*); corpus row 1014.
- **The band caught its own adder.** First witness answered 27: bit 4's pin
  said the standing corpus is 900 rows — I had counted my fresh rows into it.
  The corpus was untouched; my claim about it wasn't. Probed, pinned 750,
  witnessed 31. The counterweight works on whoever holds the pen.
- **The champion's coin is rowed and held.** `wm-case3-trial` in
  observe/work-metric.fk — seven steps, every witness real, 31 predicted and
  31 witnessed — is the rented worker's trial for case 3, excluded from any
  student training corpus so the eval can never leak into the student.

## What stands open, said with a straight back

The live slot (`dsv4-control-logit-adapter.f32`, five static floats) cannot
carry a situation-dependent classifier — five constants cannot read a
situation. The next stone is the wiring shape: the classifier consulted at
decode time, or the BMF note's embedding-row deltas. That is a design stone,
not a training stone, and it is the only thing between this earned fit and
the live DS4 exit head.

## The most surprising teaching

**The eval was flattering exactly as much as it was leaking.** 142/150 leaked
became 65/150 fresh — a 2.2x deflation — and my own estimate, made freely,
inherited the flattery almost linearly. Leakage does not just invalidate a
number; it recalibrates the estimator who reads it. The ledger now holds that
as data, which is precisely what a student must be trained on: the teacher's
misses, at their true size.

## Where discomfort turned to gold

Urs, mid-work: *"I feel a bit of caution and I love to see more confidence."*
The caution had been wearing the work's clothes — hedging estimates, deferred
claims. The answer was not to soften the honesty but to stand on it: the
prediction went on the table as a number, the claim landed as a claim
("these centroids will beat the baseline" — they did, 2.17x), and the miss
went to the ledger without flinching. Confidence and the counterweight are
the same practice seen from opposite sides: only a mind that rows its misses
can afford conviction.

## Proof

| check | verdict | exit |
|---|---|---|
| `dsv4-control-fresh-band.fk` (preregistered 31) | 27 → 31 | 0 |
| `dsv4-control-adapter-cli.fk` (standing floor re-witnessed) | 142/150, overlap 150, refused | 0 |
| `work-metric-band.fk` (case-3 trial rowed beside it) | 255 | 0 |
| `gold-ledger-band.fk` (9 rows, 7 receipts, probed 907064) | 31 | 0 |
| `homecoming-distillation-corpus-band.fk` (row 1014) | 32767 | 0 |

## The frontier question

**What names the drop from in-sample shine to out-of-sample truth?**

*Shrinkage* — statistics' own word: estimates regress when validated on
ground they never saw. 0-hit fresh. Corpus row 1014, landed under the
counterweight: 408 rows, 408 admissible, max id 1014, dup rows 0 — probed in
one cell, exit 0, before the numbers were written down.
