# Form-native NL surface grammar induction by MDL

Witnessed 2026-07-20 directly on `fkwu`.  No Python, TypeScript, Bash model
logic, generated C, or C-seed growth participates in the learner or emitter.

## What was actually learned

The input is nine authored pairs of:

```text
surface text <-> category|slot-a|slot-b
```

They cover three clause categories:

- property: `the source is native`
- question: `is the source native?`
- operational alert: `disk full: cache`

Each category has three training rows.  From the first row, the learner induces
both forward- and reverse-slot candidates by locating the two neutral content
slots in the surface and retaining the surrounding literal boundaries.  It
then requires exact reproduction of every same-category training row.

The learned rule is multi-level data:

1. clause category;
2. content-slot order;
3. prefix, infix, and suffix surface literals.

The alert category matters: it selects reverse order, proving the learner is
not merely returning one authored forward template.  Its neutral value
`alert|cache|disk full` emits `disk full: cache`.

## Candidate selection

For each category, the learner compares:

- an induced forward ordered-slot rule;
- an induced reverse ordered-slot rule;
- an exact row-memory table.

All candidates must cover every training row.  Conditional MDL encodes the
surface given the neutral row.  A structural rule costs category bytes,
learned literal bytes, and eight control symbols.  Row memory must retain each
complete neutral key, complete surface value, and two separators per row.

| category | selected candidate | rule MDL | row-memory MDL | coverage |
|---|---|---:|---:|---:|
| property | ordered-slot-rule-forward | 24 | 130 | 3/3 |
| question | ordered-slot-rule-forward | 25 | 133 | 3/3 |
| alert | ordered-slot-rule-reverse | 15 | 149 | 3/3 |

The minimum exact-coverage candidate wins without consulting held-out output.
Only a reusable winner may enter the grammar.  If row memory won, the grammar
door would return an invalid rule instead of promoting memorization.

## Held-out structural transfer

The words `signal`, `grounded`, `framebuffer`, and `budget exceeded` occur in no
training neutral row.  After selection, the grammar produces:

```text
property|signal|grounded          -> the signal is grounded
question|signal|grounded          -> is the signal grounded?
alert|framebuffer|budget exceeded -> budget exceeded: framebuffer
```

The learned inverse boundaries and slot order parse those three surfaces back
to their exact neutral values.  The three row-memory candidates return the
empty string for the same held-out values.  Composition over all selected
category rules emits:

```text
the signal is grounded; is the signal grounded?; budget exceeded: framebuffer
```

This is unseen-content generalization within the learned structure, not a
claim that the content itself was inferred or understood.

## Trace-first live observation

Command:

```text
./fkwu --src observe/nl-grammar-induction-observed-live.fk
```

The framebuffer emitted every boundary before the result:

| stage | ms | native dispatches | boxed floats | I/O | outcome |
|---|---:|---:|---:|---:|---|
| paired training observation | 1 | 157,206 | 0 | 0 | held content unseen |
| candidate induction | 0 | 122,174 | 0 | 0 | 9 candidates complete |
| MDL selection | 0 | 1,365 | 0 | 0 | 3 minimal reusable rules |
| held-out emission | 0 | 15,480 | 0 | 0 | 3 exact structural transfers |
| learned inverse | 0 | 8,024 | 0 | 0 | 3 exact roundtrips |
| memorized-row rejection | 0 | 1,587 | 0 | 0 | row memory cannot generalize |

Whole window: 2 ms, 337,647 dispatches, zero boxed floats, and zero I/O.  The
stage sum is 305,836 dispatches; the visible 31,811-dispatch remainder is
framebuffer and inter-stage observer work.  Outcome:
`bounded-two-slot-grammar-generalization-observed`.

## Proofs

The native band:

```text
./fkwu --src observe/tests/nl-grammar-induction-band.fk
# 33554431
```

The repository four-way validator also returns `33554431` with one agreement
and zero divergences for the same cells.

The first attempted compile appeared to consume one CPU for more than three
minutes.  The four-way source reader exposed the real problem as an unexpected
right parenthesis in the induction function.  Removing that extra delimiter
made `fkwu` complete in under a second.  The delay was a syntax defect, not
learner cost; recording it prevents a false performance story.

## Exact claim boundary and BMF seam

This proves deterministic induction, MDL selection, inverse emission/parsing,
and unseen-content transfer for fixed-literal, two-slot clauses in these three
authored English-shaped categories.  It does not prove general English,
unknown languages, morphology, lexical learning or translation, ambiguity
resolution, stochastic grammar induction, or generative language modeling.

The learned value already carries BMF-compatible concepts—clause category,
ordered captures, literal runs, and inverse template order—but a lowerer from
this learned value into executable `grammar`/`rule3` BMF data is not built in
this lane.  Runtime uses the learned value directly.  That conversion is the
named integration seam, not silently claimed compatibility.

The surprising teaching was that a reverse-order operational alert fell out of
the same induction machinery without a third template.  Discomfort turned to
gold when an apparently expensive native compile was traced to one delimiter:
the observation discipline separated malformed source from runtime cost.
