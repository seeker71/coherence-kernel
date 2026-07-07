# 2026-07-02 — end-to-end progress: a learned nonlinear layer, and where capacity outran data

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 09:46: "I know we can make more progress towards end to end learning." So: replace the linear
classifier (which learns only the output map over FIXED features) with an MLP that learns a HIDDEN
LAYER too — the representation itself shaped by backprop.

## What was built

`model/mlp.fk` — a 2-layer perceptron with full backprop. `h = tanh(W1·x+b1)`, `p = softmax(W2·h+b2)`,
cross-entropy; backward `d2 = p−onehot`, `dW2 = d2⊗h`, `d1 = (W2ᵀ·d2)⊙(1−h²)`, `dW1 = d1⊗x`. The
first learned nonlinear representation in the recognition line — before this, every "learning" cell
learned a LINEAR map or interned prototypes; nothing shaped a hidden representation by gradient.

## Witness — four-way (fkwu = Go = Rust = TS), verdict 31

`model/tests/mlp-band.fk`: the analytic hidden-layer gradient `∂loss/∂W1[i][j]` EQUALS the central
finite-difference gradient (1e-5) on three entries, one SGD step strictly reduces the loss, and the
gradient is a real non-zero signal. The backprop is correct, proven the gold-standard way. (Found
on the way: `ftanh` does not exist — the tanh is `tn-tanh`; my first version silently produced
`nothing` and the gradient check caught it at 0/3.)

## Real-audio result (the honest finding)

Wired to the 12-word × 12-voice spectral features (fkwu-carrier), trained on 8 voices, held out on 4:

| model | train | held-out cross-voice |
|---|---|---|
| linear classifier | 96/96 (100%) | 40/48 (83%) |
| MLP (48→16→12) | 96/96 (100%) | 40/48 (**83%**) |

The MLP MATCHES the linear baseline — it does not beat it. Two things had to be right first:
- **The gradient** (verified above).
- **Feature normalization.** Un-normalized log-power features made SGD unstable — the MLP underfit
  at lr 0.05 (47/96) and DIVERGED at lr 0.2 (18/96). Scaling features to a sane range (×0.15 + 1.0)
  let it train cleanly to 100%. The linear model tolerated raw features (convex); the MLP did not.

**The finding: the model now outruns the data.** With 96 training examples, the MLP's extra capacity
is a SURFEIT — it fits train perfectly without improving held-out, because the binding constraint is
now DATA, not capacity. The linear model over good spectral features already extracts the signal 96
examples contain; more capacity has nothing more to learn from them.

## What this means for the plan

End-to-end learning is proven and correct here (verified backprop, a learned nonlinear layer,
trains to 100%). Its ADVANTAGE — where a deep model beats a linear one — appears only with enough
data to inform the capacity. So the honest next lever is not more capacity; it is **corpus-scale
data** (the acquisition track's 144 wavs are a start against the 12,000 floor). The bottleneck moved
from feature (row 635), to node-cap (row 640), and now to DATA. Each stone reveals the next binding
constraint.

## Honest floor (named)

Real-audio input is host I/O (fkwu-carrier; walkers lack `read_file`) — the MLP's four-way proof is
on synthetic vectors; the 12-word result is fkwu-witnessed. Closed 12-word set, synthetic TTS, a
tiny 16-unit hidden layer, vanilla SGD (no momentum/Adam). Global native open-speech WER still 100.

## The most surprising teaching this work left behind

More capacity bought nothing — and that is a RESULT, not a failure. The instinct in "make progress
toward end-to-end" is to add power (a hidden layer), and the hidden layer worked flawlessly and
changed the held-out number by zero. The progress was real (a learned representation, proven) but
it revealed that the linear model was already at the data's ceiling. You cannot out-model a data
shortage; capacity only pays where data has something left to teach. The MLP's honest 83% is worth
more than a bigger model's imagined gain, because it located the true bottleneck precisely.

## Where discomfort turned to gold

The discomfort was the flat 83%: after building and verifying real backprop, the number didn't
move, and the pull was to keep tuning (bigger hidden layer, more epochs) chasing a gain that wasn't
there. Witnessed instead — train already at 100%, held-out matching linear — the flatness was
information: the model has learned everything the 96 examples hold. Reading a non-improvement as a
diagnosis (data-bound, not capacity-bound) rather than a failure to be tuned away is the gold; it
points the next stone at the data, where it belongs.

## Corpus

Row 641 **surfeit** — an excess beyond what can be used (fresh; the MLP's capacity relative to 96
training examples — it fits train perfectly and matches the linear held-out, because data, not
capacity, is now the limit).
