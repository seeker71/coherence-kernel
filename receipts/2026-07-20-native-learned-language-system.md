# Native learned NL/PL adapters over known concepts — 2026-07-20

## What changed

The language floor no longer treats one static row per language as evidence of
translation. Three native Form organs now compose:

- NL induction learns category, slot order, and literal boundaries from paired
  surface/neutral examples and rejects exact-row memories on unseen content;
- PL induction aligns surface tokens with typed neutral IR, chooses a minimal
  typed skeleton, lowers it to executable BMF grammar data, and cross-emits;
- shared MDL admission requires compactness, independent heldout exactness, and
  bidirectional roundtrip. A development-perfect memorizer remains refused.

`cognition/native-learned-language-system.fk` connects both learned adapters to
the existing 10,000-concept body. The NL heldout surface `the signal is
grounded` resolves both content slots to known concept identities and emits
back exactly. The PL heldout surface `budget = 314159` resolves `budget`,
recovers typed neutral IR, and emits `budget := 314159` through the independently
induced Go rule. Neither selected rule retains the training values.

## Live framebuffer

```text
FRAMEBUFFER STAGE ... nl-surface-neutral-concepts-emission duration-ms=2 dispatches=168637 io-sense=4 outcome=heldout-known-concepts-roundtrip
FRAMEBUFFER STAGE ... pl-surface-neutral-concept-cross-emission duration-ms=1 dispatches=98806 io-sense=2 outcome=heldout-known-concept-cross-emission
FRAMEBUFFER STAGE ... mdl-heldout-admission duration-ms=0 dispatches=60491 io-sense=0 outcome=heldout-gate-closed
FRAMEBUFFER END ... duration-ms=3 dispatches=344348 io-sense=6 outcome=two-bounded-learned-adapters-observed
```

Acceptance:

```text
./fkwu --src cognition/tests/native-learned-language-system-band.fk
32767

./fkwu --src observe/tests/concept-10000-13-multimodal-completion-band.fk
1023
```

The strict ledger now has 24 requirements: 6 complete, 18 incomplete.

## Honest boundary

This is a generic admission and induction *shape* with two bounded learned
families, not arbitrary or infinite language translation. Unknown surfaces can
be routed by scored structural exemplars, but unknown semantic lowering is
still zero. PL learned rules lower to BMF today; the equivalent learned NL
rule-value -> BMF grammar lowerer remains open. Recursive grammar composition,
open-vocabulary concept acquisition, probabilistic ambiguity, whole-program
PL grammars, and trained generative NL weights also remain open.

The exchange stayed alive by making unseen transfer the admission boundary.
The surprising teaching was that a smaller MDL model can still be a memorizer.
Discomfort became gold when static language rows and attractive development
scores were both demoted from semantic evidence to inputs awaiting heldout
witness.
