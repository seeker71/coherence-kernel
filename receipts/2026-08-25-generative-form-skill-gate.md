# Generative Form skill is now executable before it is believable

Date: 2026-08-25
Witness: Codex / Sema sibling movement

## What crossed

`form/form-stdlib/generative-form-skill-gate.fk` now accepts three generated
artifacts and gives no credit for vocabulary alone:

1. BML source is parsed by the repository's scannerless BML cursor, executed
   by a bounded AST walker, lowered into fourth-kernel `fk-*` cells, and run by
   `fk-run`.  Both executions must return the held-out challenge value.
2. A custom BMF grammar is lowered directly from raw bytes.  Its probe must
   make a first alternative fail, restore the immutable cursor, cut a later
   full-byte winner, and return first-class `nothing` under a one-step budget.
3. A Form-native micro-thought reconstructs the registered recipe NodeID and
   executes typed choice/cut/undo/timeout lifecycle observations.  It must also
   restore a checkpoint, route a learned thought kernel, earn a JIT plan
   without claiming native installation, and keep `nothing`, successful `0`,
   and successful `1` distinct.

The adversarial band includes malformed BML carrying the right nouns, a BML
program returning the wrong value, non-executable BMF vocabulary, an alt whose
first branch wins without undo, trailing BMF bytes, a wrong NodeID, a boolean-
collapsed zero plan, a wrong checkpoint, duplicate transport markers, and the
one positive framed candidate.

## Observation

```text
preflight form/form-stdlib/generative-form-skill-gate.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean

preflight form/form-stdlib/tests/generative-form-skill-gate-band.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean

./fkwu form/form-stdlib/tests/generative-form-skill-gate-band.fk
1048575
```

The first band run answered `523543`, honestly refusing BML and Form candidates
whose intended text did not yet cross their runtime identities.  Typed
observations exposed two distinct seams: `nil?` was not a valid NodeID tag test,
and the recipe request had named the recipe category rather than the recipe's
content-addressed NodeID.  Repairing the observations—not weakening the gate—
produced the full witness.

## Leakage boundary

The three evaluation situations are compared for exact equality against every
existing `bml-bmf-stream-curriculum` and `bml-bmf-control-curriculum` train and
held-out prompt/target.  Exact leakage is `0`.

This is deliberately narrow evidence.  It does not establish absence of
paraphrase, repository-RAG, base-model-pretraining, or human-memory
contamination.  The evaluation prompt is public, not a secret held-out set.

## Physical model gate still owed

`observe/qwen38-generative-form-skill-gate-live-run.fk` is a dormant effectful
driver.  It was neither preflighted nor run while the V3 evaluator owned the
shared local-model carrier.  After that carrier is released, the exact owed
observation is:

```sh
./fkwu observe/qwen38-generative-form-skill-gate-live-run.fk
```

A physical pass requires `overall=1`, `bml-executed=1`,
`bmf-scannerless-executed=1`, `form-nodeid-control-executed=1`,
`exact-row-leakage=0`, and `residence-released=1` from one actual local-Qwen
generation.  Until that run exists, the verifier is proven and the model's
generative embodiment remains pending.

## Closing

I kept this exchange alive by letting the first refusal remain evidence and
following its typed identities into the implementation.  The surprising
teaching was that the requested recipe coordinate had to be born from content,
not copied from its category.  Discomfort turned to gold when the broad BML
prelude carried unresolved lane seams: shrinking onto the native cursor and
fourth-kernel walker made the proof both smaller and stronger.
