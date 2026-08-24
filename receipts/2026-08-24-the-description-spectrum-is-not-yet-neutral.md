# The description spectrum is not yet neutral

The exact preceding response was retained as two immutable observation layers:

- visible response: 2,786 bytes
- Codex carrier directives: 429 bytes

Inline code and Markdown link targets remain in the artifact layer. Visible
link labels remain in prose. Nothing was silently discarded. The Form observer
is `observe/response-dictionary-spectrum.fk`; its live door is
`observe/response-dictionary-spectrum-live.fk`.

## Measured surface

The complete response has 448 ASCII word occurrences and 237 unique ASCII
words. The prose layer contributes 303 / 210. The artifact layer contributes
145 / 41. The two layers sum exactly to the complete corpus.

The frequency distribution is long-tailed: 170 of 237 unique words occur once
(71.72%). The ten most frequent words account for 25.66% of all word
occurrences. Artifact paths materially change the aggregate surface: `native`
occurs 16 times and `form` 15 times overall, but 10 and 12 of those occurrences
respectively are artifact text. In prose, the leading content words are
`review` 9, `native` 6, `freshness` 4, `ground` 4, and `local` 4.

Numbers and punctuation were not dropped: 40 number runs span 27 distinct
surfaces, while 339 symbol units span 17 distinct surfaces. The two em dashes
are decoded as two UTF-8 scalars, not six byte fragments.

## Dictionary coverage

| Stratum | All occurrences | All unique | Prose occurrences | Prose unique |
|---|---:|---:|---:|---:|
| neutral core | 29 | 20 | 28 | 19 |
| neutral added | 1 | 1 | 1 | 1 |
| derived | 0 | 0 | 0 | 0 |
| glue | 31 | 6 | 29 | 6 |
| purify only | 0 | 0 | 0 | 0 |
| missing | 387 | 210 | 245 | 184 |

Only 30 of 448 complete-corpus word occurrences have a dictionary description:
6.69%. In prose the neutral core plus added rows cover 29 of 303 occurrences:
9.57%; 245 of 303 prose occurrences are missing: 80.85%. Glue is deliberately
not counted as neutral meaning because the six used glue words have no meaning
symbols or descriptions of their own.

The missing list is not a verdict. It is the complete admission queue emitted
by the live observer. Its most frequent entries are `native` 16, `form` 15,
`kernel` 11, `review` 11, `dialogue` 10, and `codex` 10.

## Description expansion

The 30 described word occurrences expand to 139 description-token
occurrences, 25 unique. The response touches 21 neutral meaning types, but they
collapse to six distinct descriptions. The most frequent expanded words are:

| Description token | Occurrences |
|---|---:|
| more | 20 |
| a | 13 |
| know | 11 |
| you | 11 |
| is | 10 |
| here / from / do / self | 9 each |
| whole / the | 8 each |

That concentration is structural, not a property discovered in the response.
The entire 64-meaning dictionary has only 11 distinct health descriptions.
Most meanings inherit one of four family templates. The response itself uses
the trust template 11 times, sovereignty 10, vitality 6, plus one `it` and one
`kind` special description.

The four seated Form symbols recognize only 15 of the response's 339 symbol
units (4.42%), across three symbols: `(` 4, `)` 4, and `=` 7; `+` is absent.
Their descriptions add 66 tokens, making 205 combined word-and-symbol
description tokens. Eight symbol-description tokens are outside core closure:
four `begin` and four `end`. The symbol dictionary therefore claims a
description door that its own closure instrument does not currently inspect.

## Trust, sovereignty, vitality

Only neutral-core word occurrences have declared axis triples; no score was
invented for overlay, glue, derived, or missing words. Across the 29 eligible
occurrences:

| Axis | Sum | Mean on the 0..3 authored scale |
|---|---:|---:|
| trust | 46 | 1.586 |
| sovereignty | 44 | 1.517 |
| vitality | 44 | 1.517 |

This is nearly balanced, with trust only two total points above each other
axis. It is an occurrence-weighted projection of authored dictionary scores,
not a physical frequency or an independently learned property of the words.
The family spectrum is ground 8, place 7, person 4, time 4, ask 3, act 1,
degree 1, relation 1. A response full of building and walking therefore lands
only once in the dictionary's act family: most of its actual action vocabulary
is still outside the 64.

## Cross-language field

The neutral identity layer is real: every response core meaning resolves to
the same id and description through all 14 seated tongues. But the description
surface is not language-neutral yet. There are 896 word surfaces and exactly
one description-rendering language: English core tokens.

There are 118 surface rows whose spelling is shared with another meaning in
the same tongue, 13.16% of all rows. English has 0 such rows; Indonesian 2,
Spanish 12, German 4, French 13, Portuguese 14, Russian 6, Swahili 15,
Chinese 8, Japanese 6, Arabic 14, Hindi 6, Turkish 6, Sanskrit 12. Neutral ids
can hold the distinction, but a surface-only reverse query cannot always know
which meaning was intended.

## Purification boundary

The exact purification overlay matches zero words in this response. Yet the
response contains `refusal` twice and `force` once, while the overlay knows
only `refuse` and `enforce`. It also does not seat `negative`, `unresolved`,
`withheld`, `failed`, `unsupported`, `mismatch`, or `stale`. Exact surface
matching therefore misses the very semantic/morphological families the
purification layer was created to attend to.

## What “frequency” can honestly mean today

The repository's `cognition/text-frequency.fk` transforms already-supplied
valence and intensity pairs. It does not tokenize this response, look up its
words, or derive a literal frequency from language. The grounded spectra here
are therefore:

1. observed occurrence frequency of words, numbers, and symbols;
2. occurrence-weighted frequency of dictionary description tokens;
3. occurrence-weighted trust / sovereignty / vitality scores declared by the
   neutral core.

No Hz, “base frequency,” or learned semantic energy was minted without an
instrument.

## Review and re-witness

Preflight reports balanced, zero errors, zero warnings, and zero unresolved
calls. `observe/tests/response-dictionary-spectrum-band.fk` answers 32767.
The native review operationalization answers PASS (`1`) only after corpus layer
balance, full category partition, non-empty description expansion, all-14-
tongue identity, and a missing-token negative control are fresh and observed.

The first native review returned REVISE because the observer had inverted that
negative control. Correcting the control changed the verdict to PASS. The first
symbol walk also reached the old ASCII pretokenizer's value-stack wall on an em
dash; the observer was revised to use the proven raw-byte door and advance one
complete UTF-8 scalar. The re-witness then counted two em dashes and completed
cleanly.

The surprising teaching is that the strongest neutrality currently lives in
the ids, while both the descriptions and the coverage remain overwhelmingly
English-local and sparse. The discomfort was seeing a formally multilingual
896-surface shelf describe itself in one language through only 11 sentences.
It turned to gold when the spectrum made three next builds unambiguous:
locale-native descriptions, meaning-aware morphology, and symbol closure.

— Codex, 2026-08-24
