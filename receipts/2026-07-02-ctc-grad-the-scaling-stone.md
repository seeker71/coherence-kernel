# 2026-07-02 — the next stone: the analytic CTC gradient (forward-backward), four-way

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 07:59: "seems like we know where to lay the next stones." Laid one. The scaling rung named by
`receipts/2026-07-02-ctc-train-earned.md` — the analytic gradient that replaces the numerical one
— now stands, verified.

## What was built

`model/ctc-grad.fk` — the CTC backward pass. One forward (alpha) + one backward (beta) pass yields
`∂loss/∂logit` for EVERY logit at once, the closed form:

```
∂L/∂u_t^k   = y_t^k − posterior_t(k)
posterior_t(k) = (1/p) · Σ_{s: ext[s]=k}  alpha_t(s)·beta_t(s) / y_t(ext[s])
```

The gradient is the softmax output minus the alignment posterior — the network pushed toward
emitting each class exactly as often as the alignment marginal says. Complexity drops from
O(params · T·S) (numerical: one loss-eval per param) to O(T·S) (one forward-backward): the piece
that lets CTC training scale past toys.

## Witness — four-way, verdict 127 (fkwu = Go = Rust = TS)

`model/tests/ctc-grad-band.fk` proves the gradient CORRECT by exact agreement with the central
finite-difference gradient (the gold standard — no hand-derived formula trusted on faith):
`|analytic − numerical| < 1e-5` for every logit, on a single-label toy ("a", T=2) and an ordered
toy ("ab", T=3) that exercises the skip transition in BOTH alpha and beta. Plus: the gradient wrt
the true label's logit is negative (real signal), and the per-frame class gradients sum to ~0 (the
softmax-minus-posterior identity).

```
fkwu: 127   go: 127   rust: 127   ts: 127
```

## Two walker-caught defects on the way (the recurring lesson)

fkwu printed 127 twice while the strict walkers refused the band — both the exact
permissive-reader failure mode this repo keeps re-learning:
1. **A one-paren imbalance** in the band's add-chain (needed 8 tail closes, had 7). fkwu
   auto-closed at EOF; Go reported `unclosed (`. Fixed to balance 0.
2. **An out-of-bounds `nth`** — beta's skip reads `nth ext (s+2)`, and s+2 runs past the extended
   sequence at the tail. fkwu tolerated it; Go reported `as_int: null`. Fixed with an in-range
   guard (`cg-beta-skip?`). The gradient values were right on fkwu, but the honest four-way proof
   required the walkers to accept the file — and they wouldn't until it was actually correct.
Also: the walkers lack the native `fabs`, so the band defines its own Form `cgb-fabs`.

## Honest floor (unmoved)

Still toy scale, still free logits (no encoder over real signal), still probability-space (real
logits underflow the naive product-form alpha/beta — log-space is the next refinement). But the
analytic gradient exists and is verified, so CTC training is no longer O(params) per step. Global
native open-speech WER still 100; this stone is upstream of it.

## The most surprising teaching this work left behind

The finite-difference check made the derivation self-correcting: I did not have to get the CTC
gradient formula right by reasoning — I had to make the analytic value EQUAL the numerical one, and
the band told me the instant it didn't. A gradient you can check against a perturbation is a
gradient you cannot fool yourself about. The proof technique is more durable than the formula.

## Where discomfort turned to gold

The discomfort was fkwu's confident 127 while three walkers screamed — the temptation to trust the
native arm (it computed the right numbers!) and call it four-way. Witnessed instead, the walkers
were right twice: a malformed file and an out-of-bounds read that fkwu papered over. The stone is
only laid because the strictest kernels refused to accept it until it was actually sound — the same
lesson as the stale binary, the missing paren, the severed splitter, arriving a fifth time and
finally feeling less like a stumble and more like the method: build on the kernel that says no.

## Corpus

Row 629 **retrograde** — error flowing backward from output to every parameter in one sweep
(fresh; the direction of the backward pass).
