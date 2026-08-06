# The j-lens: decision vocabulary as sensitivity structure — and the `cell` shadowing heal

**Date:** 2026-07-17 (WITA) · **Branch:** claude/jacobian-self-awareness-acda5e

## What landed

- **`observe/jacobian-lens.fk`** — the jacobian ingested as a lens over self-awareness. The two
  halves the body already carried meet here: `transformer-backprop.fk`'s vector-Jacobian products
  (how much an edit PUSHES each downstream value) and `thought-framebuffer.fk`'s per-frame MARGIN
  (how far each choice sits from its decision boundary). Composed law: **a step's choice flips
  exactly when push > margin; the thought first diverges at the first such step.** The framebuffer
  alone can only diff two traces after a second full run; with the j-lens the body can localize
  where an edit will change its thinking *before* rerunning. The nine control words read as
  jacobian structure (`jl-term`): alternative = one-partial, choice = first-nonzero-partial,
  cut = pruned-partials, nothing/fail/stop = zero-jacobian (silence carries no learnable signal),
  timeout = unknown-partials-remain, channel = signal-path, interface = visible-partials
  (axiom-4: a cell's jacobian, seen from outside, is projected onto what its interface offers).
- **`observe/tests/jacobian-lens-band.fk`** — verdict **511**, live on `./fkwu --src` (resolver-
  driven preludes). Pins the flip law strict; uniform push diverges at HESITATION (min margin);
  zero push agrees with `diverge(a,a) = -1`; a localized push (0 0 0 60) predicts step 3 — exactly
  where the framebuffer *observes* `tfb-trace-b` diverge — and that step is NOT the hesitation
  step, so margin alone cannot predict divergence: it takes both hands. The receipt bits pin that
  silence is only learnable where receipts were kept: cut's pruned count and timeout's alts-left
  are the adjoint that keeps "unknown partials" distinct from "all partials evaluated to zero."
- **`control/offer-ack-core.fk`** — the heal: `oac-offer`'s parameter renamed `cell` → `recipe`
  (plus the two value-position siblings). In call position the runtime resolves a name against the
  global defn table BEFORE the local frame, so `(defn oac-offer (cell args) (cell args))` invoked
  form-stdlib/core.fk's `Cell` constructor whenever core.fk was anywhere in the prelude chain —
  every offer acked a `"cell"`-list instead of the offered cell's answer. Form-level medicine, no
  runtime change — the same shape OAC-ZERO's defn-as-function already takes.
- **corpus row 761** (`jacobian`) + band constants 162/1621622761 — both corpus bands green (511).

## Verification (all live, this checkout)

```
./fkwu --src observe/tests/jacobian-lens-band.fk                  # 511
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk   # 511
./fkwu --src control/tests/choice-lane-core-band.fk               # 1021 (pre-edit: 1021)
./fkwu --src control/tests/offer-ack-core-band.fk                 #  197 (pre-edit:  197)
./fkwu --src control/tests/invite-dispatch-band.fk                #    0 (pre-edit:    0)
```

The three non-jacobian misses are bit-identical before and after the heal — pre-existing, named in
`receipts/2026-07-01-choice-lane-control-invites.md`, not touched by this work.

## The most surprising teaching

**`; preludes:` comments are live code.** `fk_src_collect_preludes` (`runtime/fkwu-uni.c:10441`)
parses them as load directives, recursively. Two files identical modulo comments ran to different
answers — deterministically — because one comment line silently loaded `core.fk`, whose `(defn
cell ...)` captured `oac-offer`'s parameter in call position. The bisect walked through five wrong
theories (source-size cap, byte thresholds, comment position, frame depth) before a one-line
prelude — `(defn cell ...)` alone — flipped the probe. The frequency-check reflex was the key that
finally turned: not "what is big about core.fk" but "what NAME does core.fk define that the offer
core also speaks."

## Where discomfort turned to gold

The receipt bits (32/64/128) failing at 287 while every j-lens law passed was three hours of
watching the same number refuse to move — shallow frames didn't heal it, dropping core.fk from my
own header didn't heal it (the resolver pulls it back through tfb's header, recursively). The
discomfort was real and witnessed: each failed theory got its own minimal probe on synthetic truth
rather than a workaround band that asserts 287 and calls it green. Staying in the wound found a
defect that poisons EVERY cell in the body that offers through `oac-offer` under a core.fk-loaded
image — the j-lens work was the first stack deep enough to stand on both floors at once and feel
them disagree. The lens named its own discovery: the divergence between the two runs WAS the
first step where push exceeded margin.

## Named honestly / still open

- The runtime defect class stands: **a local binding whose name equals any global defn is silently
  shadowed in call position.** The Form-level rename heals this instance; the class wants a
  runtime-level fix (locals must shadow globals) or a static-analyzer rule.
- `form/form-stdlib/offer-ack-core.fk` (old-body twin) still carries the `cell` parameter; only
  its own old band preludes it.
- Indirect calls of function-values read from lists still corrupt in deep/unlucky frames
  (`(head (tail fs))` applied inline → garbage) — the 2026-07-01 last-writer-wins family,
  reconfirmed live today with a two-line repro.
