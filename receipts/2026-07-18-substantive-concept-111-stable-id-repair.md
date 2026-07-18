# 2026-07-18 — the last 111 lexical debris slots become substantive concepts

## What changed

The previous 10,000-anchor surface had 111 rows with no English WordNet or
Wiktionary meaning. They were not an abstract tail: the exact residue included
truncated contractions (`didn`, `doesn`), subtitle disfluencies (`i-i-i`,
`wh-what`), punctuation fragments (`'the`, `right.`), confusables (`yöu`,
`yοu`, `tο`), and OCR-like debris (`chffffff`, `ch00ffff`). Counting those as
substantive concepts would have been false.

The stable IDs remain fixed. Each defective canonical surface was replaced by
the next source-ranked entry after rank 10,000 that passed all of these gates:

1. from pinned HermitDave/FrequencyWords OpenSubtitles2018 commit
   `525f9b560de45753a5ea01069454e72e9aa541c6`, full SHA-256
   `5351ff405b1126ef555791dd4d9798a48e3e9a501a9fc481a9da957752cfb458`;
2. unique against the 10,000 canonical surfaces;
3. lowercase alphabetic or hyphenated English, at most 20 UTF-8 bytes;
4. an exact lowercase English Wiktionary page with a revision-pinned common-POS
   substantive definition;
5. not a proper noun and not an inflection/form-only definition.

The builder inspected 180 ranked candidates to accept 111. The replacements
span true upstream ranks 10,001–10,182. Examples:

| stable ID | legacy alias | canonical concept | source rank | Wiktionary evidence |
|---:|---|---|---:|---|
| 104 | `didn` | `welcoming` | 10,001 | page 210182, revision 90576503 |
| 275 | `wouldn` | `precision` | 10,009 | page 70524, revision 90083028 |
| 2054 | `'clock` | `diabetes` | 10,020 | page 61586, revision 90771414 |
| 5019 | `yöu` | `damp` | 10,048 | page 22890, revision 91526511 |
| 9404 | `por` | `squid` | 10,164 | page 4870, revision 91388405 |
| 9965 | `waitin` | `plum` | 10,182 | page 4770, revision 91403072 |

`model/concept-10000-substantive-repair-migration.tsv` is the exact 111-row
before/after ledger. Every old token remains detectable through a separate
byte-sorted alias index and resolves to the new canonical record at its original
stable ID. Repaired records expose the true upstream frequency rank rather than
pretending their ID+1 is still the corpus rank.

## Meaning and 13-language projection

Each replacement has a page ID, revision ID, timestamp, SHA-256 of the complete
English section, POS section, and source definition. Method 5,
`substantive-id-repair-wiktionary`, has priority over old ID-keyed semantics, so
an old fragment's sense cannot leak into its replacement.

All 111 concepts have 13 nonempty NL cells: 111 exact English frequency anchors
(`F`) and 1,332 Google Translate carrier results (`G`) across Indonesian,
Spanish, French, Brazilian Portuguese, Swahili, German, Russian, Chinese,
Japanese, Arabic, Hindi, and Turkish. The `G` cells are explicitly
**machine-translated and unreviewed**; they are attributed breadth, not a claim
of human fluency. Total: 1,443/1,443 cells.

The UTF-8 live offset index was computed over bytes, not JavaScript characters.
That distinction was caught by the Japanese last-row witness (`plum` → `梅`)
and repaired before landing.

## Operational evidence

`presence/concept-substantive-repair-111-live.fk` is the non-test consumer. It
opens canonical and alias detection, repair meanings, and the NL overlay once;
canonical and legacy inputs converge on one stable identity. The separate
observation entrypoint exercises canonical input, legacy compatibility,
Indonesian generation, and NL detection.

Exact rebuild verification:

```text
node model/concept-10000-substantive-repair-verify.mjs
verified: 111 stable IDs, 111 aliases, 111 pinned meanings,
          1,443 attributed NL cells, 11 exact file hashes
```

Bounded native Form gate:

```text
./fkwu --src model/tests/concept-10000-substantive-repair-111-live-band.fk
4095
```

Literal live samples prove both canonical/alias convergence and the semantic/NL
bytes. `welcoming` and legacy `didn` both resolve to ID 104, upstream rank
10,001, frequency 2,510, Wiktionary adjective meaning, Indonesian `menyambut`.
`plum` and legacy `waitin` both resolve to ID 9965, rank 10,182, frequency
2,437, Wiktionary noun meaning, Japanese `梅`. The completion row is:

```text
[10000, 0, 997, 448, 111, 111, 1556, 0]
```

Existing gates remained green: concept mechanics `127`, historical lexical
audit `4095`, existing 13-NL runtime `8191`, and required checkout ground
`42 / 55 / 15 / [1, 2.5, [3, 4]] / 11111`.

## Honest boundary

This closes the **represented substantive English concept** defect in the
10,000-ID universe. It does not make the 1,332 new translations reviewed, does
not disambiguate meanings in running sentences, and does not by itself prove
audio/video generation or sensing for these 111 identities. Those remain
separate objective-level obligations.

## Closing

What kept this alive was retaining every displaced surface as an explicit alias
instead of silently rewriting identity. The surprising teaching was that only
180 post-cutoff candidates were needed to find 111 revision-pinned common
meanings. Discomfort turned to gold twice: the debris list made the old
“10,000 substantive” claim untenable, and a missing Japanese last-row value
exposed a character-offset/byte-offset error before it could become substrate.
