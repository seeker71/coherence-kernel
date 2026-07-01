# 2026-07-01 -- diagnosing the n=60 dip in the corrected learning curve

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
```

Witness:

```text
42
55
```

## Source Observation

`receipts/2026-07-01-nl-meaning-net-corrected.md` measured the corrected (100-epoch, normalized) learning
curve as `34/50 -> 26/50 -> 32/50 -> 36/50` across training sizes 20/60/100/154 -- named as "a real, unresolved
artifact" at the time. Each sweep point trains an INDEPENDENT model from scratch (deterministic init, no
continual learning across sizes), so the dip is fully reproducible in principle, not run-to-run noise --
worth actually explaining rather than leaving as an open question.

## Diagnosis

Per-class held-out breakdown (totals: class0=13, class1=13, class2=12, class3=12; sums to 50):

| class | n=20 correct | n=60 correct | change |
|---|---|---|---|
| 0 (wellbeing) | 10/13 | 9/13 | -1 |
| 1 (truth triumphs) | 9/13 | **4/13** | **-5** |
| 2 (self/existence) | 8/12 | 7/12 | -1 |
| 3 (world peace) | 7/12 | 6/12 | -1 |

The entire dip is class 1. Every other class moves by exactly the noise-level amount (-1). This is not a
diffuse regression across the sweep -- it is one class collapsing.

**Why class 1 specifically:** the training pool is a class-balanced round-robin of each class's OWN list
order, so the n=60 point trains on roughly the first 15 of each class's ~40 training examples (post-split).
Class 1's hand-authored list (`learn/nl-meaning-corpus.fk`) is ordered short-simple paraphrases first ("truth
alone triumphs," "truth alone wins," ...), then a long run of "victory/falsehood/lies/conquers" vocabulary
paraphrases, with the multi-locale renderings (`satyam eva jayate`, `die wahrheit allein siegt`, ...) and a
second later-added batch ("truth outlasts every lie," "no deception can outlive the truth," ...) arriving only
near the END of the ~52-item list -- well past position 15. The held-out set (every 4th item) draws from the
WHOLE list, including those later, more lexically-varied items. At n=60, the model has seen only the narrow
early "victory/falsehood" vocabulary slice of class 1 -- enough to overfit a decision boundary around THAT
vocabulary, which then generalizes worse to the later, differently-worded held-out examples than either seeing
almost none of class 1 (n=20, less overfitting pressure) or eventually seeing most of its diversity (n=100/154,
enough to generalize past any one vocabulary slice). This is the standard non-monotonic-learning-curve
pattern for ordered, non-shuffled training data under a fixed epoch budget -- not a bug, a real property of
training on a hand-ordered corpus without randomizing example order first.

## What This Does Not Fix

The corpus/sweep code is unchanged. A concrete, well-specified next step (not implemented here, to avoid
another expensive multi-minute re-verification pass in the same sitting): apply a deterministic
index-permutation (e.g. a fixed-stride reorder, coprime with each class's list length) to each class's list
before splitting/round-robining, so any prefix length draws a representative slice of each class's vocabulary
diversity instead of a narrow early-list slice. This is a real, actionable fix, named rather than silently
deferred -- not yet built.

## Witness

Per-class counts were extracted via two additional queries against the already-shipped
`learn/tests/nl-meaning-net-band.fk` machinery (`nml-train-on-n`, `nml-held-cases`, `nmn-correct?`), packed
into single integers (`class0*1e6 + class1*1e4 + class2*100 + class3`) since this interpreter's default value
printer does not render nested lists legibly:

```text
n=20 totals-independent per-class held-out sizes: 13 13 12 12  (packed 13131212)
n=20 per-class correct: 10 9 8 7   (packed 10090807, sums to 34 -- matches the original sweep)
n=60 per-class correct: 9 4 7 6    (packed 9040706, sums to 26 -- matches the original sweep)
```
