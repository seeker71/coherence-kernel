# 2026-07-22 — labels became evidence, not identity

## Ground

The fresh C seed returned `42`, `55`, freshness `15`, the numeric-list witness,
and native-vs-rented `11111` before this slice changed.

## Build

`cognition/language-neutral-symbol-identity.fk` derives a canonical symbol from
four numeric composition coordinates. Alias rows carry plane, relation,
dialect, surface spelling, and target node; no spelling enters the node
calculation. The same spelling may therefore resolve differently under a
different relation without changing either canonical symbol.

The executable band observes two natural-language surfaces from committed,
attributed sources (`peace`, `lokah`) and two programming-language surfaces
from checked-in programs (`add`, `+`). English/Sanskrit converge on the peaceful-state node;
Form/Python converge on arithmetic-add. Python `+` under string-combine resolves
to a different node, demonstrating relation-sensitive divergence.

TEI lexical observation is limited to `<body>` coordinates; USFM observation
starts at the first chapter record. Synthetic headers containing the monitored
aliases yield zero content-plane hits. Lexical evidence remains distinct from a
semantic-frequency claim, which is explicitly `nothing`.

## Witness

```text
./fkwu --src cognition/tests/language-neutral-symbol-identity-band.fk
[nothing, 0, 1, 1001099001, 1001010001, 1001020001, 1001020002, 47, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, nothing, nothing, 1]
```

The portable evidence is 47 exact `peace` surfaces in the committed NL meaning
corpus and two exact `lokah` surfaces in the committed Sanskrit locale baseline;
one Form `add`; and one Python infix ` + `. No untracked corpus is required.

The first implementation copied a large semantic body through repeated
concatenation and produced no result in bounded observation. The framebuffer
retains that lesson while its portable witness measures the committed NL corpus
and selects an in-place range view:

```text
./fkwu --src observe/language-neutral-symbol-no-result-framebuffer.fk
[nothing, 0, 1, 99002, 14491, 1, 1, 1, 1, 1, 1, 1]
```

The range implementation then completed the full witness in about 0.23 seconds.

## Next layer: computed locale identities enter inquiry

The first witness still contained two curated NL alias rows. The next layer does
not add aliases. `cognition/language-neutral-inquiry-induction.fk` resolves each
complete locale token sequence through the baseline's neutral meaning coordinate,
derives its canonical node numerically, and encodes that node as a valid recursive
inquiry subject. The same resolver automatically traverses every ready locale and
meaning row already present in the attributed body.

```text
./fkwu --src cognition/tests/language-neutral-inquiry-induction-band.fk
[nothing, 0, 1, 1001099002, 2, 44, 44, 308, 308, 1, 3, nothing, 1]
```

Measured coverage rose from 2 curated NL surfaces to 44 computed phrase surfaces.
All 44 converged on their expected neutral identities. Each entered what, where,
when, who, how, which, and why; all 308 results replayed with valid alternative
nodes. The framebuffer witnessed the coverage trace diverge at step 1 and retained
three live observations. An unknown token sequence remained `nothing`.

## Honest floor

The original alias equivalences remain a small curated vertical slice. The new
resolver generalizes over aligned rows and locales but does not yet infer meanings
from unaligned open text; it is not a general multilingual ontology. Exact lexical
occurrence is not semantic frequency.

## Next level: held-out text is learned, then inquired

`learn/nl-meaning-inquiry-learning.fk` removes the exact-alignment requirement for
the four meanings currently grounded by this body. It trains the existing native
two-block residual learner on 160 paraphrases and evaluates a disjoint 44-row tail.
No held-out row is admitted to training. Each predicted class becomes a numeric
neutral identity; every prediction then enters and replays all seven inquiry lanes.

```text
./fkwu --src learn/tests/nl-meaning-inquiry-learning-band.fk
[nothing, 0, 1, 1001099003, 160, 44, 23, 31, 308, 308, 1, 3, 1]
```

Training reduced corpus loss and moved held-out correctness from 23/44 (52.3%) to
31/44 (70.5%): eight additional unseen paraphrases were resolved correctly. All
44 predictions—correct or incorrect—remained inspectable rather than disappearing;
their 308 inquiry receipts replayed with valid alternatives. The framebuffer saw
the accuracy trace diverge at step 1 and retained three live observations.

The honest floor moved: open paraphrases now enter the graph through learned model
output, but authority is limited to four trained meanings and measured accuracy is
70.5%, not certainty. The remaining 13 errors must stay visible as model error;
inquiry replay proves derivation integrity, not semantic correctness.

## Next layer: mistakes teach without replacing the ground

The 44-row unseen tail is split again: 22 rows expose adaptation mistakes and 22
remain outside every update as adjudication. The base learner misses 8 adaptation
rows. A first live attempt trained only on those errors and made untouched accuracy
worse, from 17/22 to 16/22; reducing the learning rate and epochs produced the same
aggregate result. Those observations established harm, not its cause.

`learn/nl-meaning-error-adaptation.fk` repairs that failure with rehearsal. Each
update epoch sees the original 160-row ground plus the 8 witnessed corrections.

```text
./fkwu --src learn/tests/nl-meaning-error-adaptation-band.fk
[nothing, 0, 1, 1001099004, 160, 22, 8, 22, 17, 21, 154, 154, 1, 4, 1]
```

On the untouched partition, correctness rises from 17/22 (77.3%) to 21/22 (95.5%).
All 154 inquiry and replay paths retain valid alternatives. Four framebuffer events
hold the error count, before accuracy, after accuracy, and inquiry completion; the
accuracy trace diverges at step 1.

This is measured transfer, not a claim of general language mastery. It covers four
meanings and one deterministic partition. The one remaining error stays visible.

## Per-row observation: what actually changed

The aggregate result did not establish catastrophic forgetting. A second witness
therefore retains the behavioral transition of every changed evaluation row under
the base, error-only, and rehearsal models. Rows are identified by a native content
hash plus byte length, then resolved back to committed source text.

```text
./fkwu --src learn/tests/nl-meaning-error-observation-band.fk
[nothing, 0, 1, 1001099005, 22,
 [2, 1, 15, 4], [0, 4, 17, 1],
 [[860066,46,0,2,2,0],[909361,39,0,2,2,0],
  [386159,49,0,2,2,0],[133025,47,1,2,1,1],
  [211502,49,1,1,0,1],[258787,47,3,3,2,3]], 4, 1]
```

Transition vectors are `[correct->wrong, wrong->correct, correct->correct,
wrong->wrong]`. Error-only learning produced `[2,1,15,4]`; rehearsal produced
`[0,4,17,1]`. Thus two behavioral regressions are directly observed under the
error-only update, while rehearsal has zero regressions and four recoveries on
the same evaluation rows.

The two regressions were:

- `the triumph of truth over falsehood is inevitable`: truth/base/error-only
  classes `1/1/0`;
- `peace for all the worlds that have ever existed`: `3/3/2`.

This witnesses behavioral forgetting—previously correct predictions becoming
wrong. It does not yet observe gradients, weights, or an internal causal mechanism.
TEI and USFM helpers remain available as structural exclusion planes, but the
portable success gate does not depend on an untracked scripture checkout.
