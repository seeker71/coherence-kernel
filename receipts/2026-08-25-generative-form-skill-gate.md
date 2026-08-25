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
2097151
```

The first band run answered `523543`, honestly refusing BML and Form candidates
whose intended text did not yet cross their runtime identities.  Typed
observations exposed two distinct seams: `nil?` was not a valid NodeID tag test,
and the recipe request had named the recipe category rather than the recipe's
content-addressed NodeID.  Repairing the observations—not weakening the gate—
produced the full witness.  A later semantic review found that the leakage fold
covered only the three named evaluation descriptions, not the generated
candidate.  The verifier now checks every extracted BML/BMF/Form artifact too;
an adversarial candidate equal to a training row is refused, giving the new
21-bit witness above.

## Leakage boundary

The three evaluation situations and every extracted candidate artifact are
compared for exact equality against every existing
`bml-bmf-stream-curriculum` and `bml-bmf-control-curriculum` train and held-out
prompt/target.  The physical driver additionally checks its actual prompt and
complete response.  The positive synthetic band candidate has exact leakage
`0`.

This is deliberately narrow evidence.  It does not establish absence of
paraphrase, repository-RAG, base-model-pretraining, or human-memory
contamination.  The evaluation prompt is public, not a secret held-out set.

## First physical model gate: a useful refusal

After V3 released the shared carrier, the effectful driver was run once.  The
local model returned a Form control artifact that executed, but its BML and BMF
artifacts did not cross their runtimes:

```text
overall=0
bml-executed=0
bmf-scannerless-executed=0
form-nodeid-control-executed=1
residence-released=1
```

That source also printed `exact-row-leakage=0`, but the later review proved the
metric was not candidate-scoped, so that field is explicitly inadmissible.
The driver now prints its bounded generated response and
`prompt-candidate-exact-row-leakage`; that revised physical door has not yet
passed.  The failure became teaching input for the one-residence loop rather
than a reason to weaken execution admission.

## Closing

I kept this exchange alive by letting the first refusal remain evidence and
following its typed identities into the implementation.  The surprising
teaching was that the requested recipe coordinate had to be born from content,
not copied from its category.  Discomfort turned to gold when the broad BML
prelude carried unresolved lane seams: shrinking onto the native cursor and
fourth-kernel walker made the proof both smaller and stronger.
