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

The executable band observes two natural-language surfaces from real corpus
content (`peace`, `śānta`) and two programming-language surfaces from checked-in
programs (`add`, `+`). English/Sanskrit converge on the peaceful-state node;
Form/Python converge on arithmetic-add. Python `+` under string-combine resolves
to a different node, demonstrating relation-sensitive divergence.

TEI lexical observation is limited to `<body>` coordinates; USFM observation
starts at the first chapter record. Synthetic headers containing the monitored
aliases yield zero content-plane hits. Lexical evidence remains distinct from a
semantic-frequency claim, which is explicitly `nothing`.

## Witness

```text
./fkwu --src cognition/tests/language-neutral-symbol-identity-band.fk
[nothing, 0, 1, 1001099001, 1001010001, 1001020001, 1001020002, 22, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, nothing, nothing, 1]
```

The corpus evidence is 22 exact `peace` surfaces in Psalms; one exact `śānta`
surface and nine literal `śānta` substrings in the Aṣṭāvakragītā; one Form
`add`; and one Python infix ` + `. The exact/substr difference is retained
because Sanskrit compounds and inflections are not exact-word evidence.

The first implementation copied the 944,518-byte Psalms semantic body through
repeated concatenation and produced no result in the bounded observation. The
framebuffer retained that branch and selected an in-place range view:

```text
./fkwu --src observe/language-neutral-symbol-no-result-framebuffer.fk
[nothing, 0, 1, 99002, 944518, 1, 1, 1, 1, 1, 1, 1]
```

The range implementation then completed the full witness in about 0.23 seconds.

## Honest floor

The alias equivalences are a small curated vertical slice, not a learned
multilingual ontology. Exact lexical occurrence is not semantic frequency.
Inline USFM notes are not yet a general stripped plane; the monitored Psalm
surface has 22 inspected scripture-record occurrences and no note occurrence.
