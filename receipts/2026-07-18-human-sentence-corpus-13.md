# Human sentence corpus across 13 lenses — observed floor

Date: 2026-07-18

Historical floor: this receipt records the first 147-row landing. The current
1,300-row, 100-per-locale corpus and its exhaustive replay are witnessed in
`receipts/2026-07-18-human-sentence-corpus-13-scale.md`.

## What entered the body

The text detector now has a bounded public-data consumer over real contributed
sentences in English, Indonesian, Spanish, French, Brazilian Portuguese,
Swahili, German, Russian, Mandarin Chinese, Japanese, Arabic, Hindi, and
Turkish. The source is Tatoeba's per-language detailed exports, licensed CC BY
2.0 FR. The exact 13 archive URLs, retrieval time, licenses, and compressed
SHA-256 values live in
`cognition/fixtures/human-corpus-13/ARCHIVES.tsv`. Each selected row also keeps
its sentence ID/page, contributor, source dates, and exact source-row SHA-256.

The deterministic Node builder walked the complete pinned exports and selected
147 rows:

- 122 domain observations across work, money, family, school, doctor, water,
  food, hospital, bus, and rain;
- 12 explicitly retained cross-concept surface collisions;
- 13 honest zero-detection rows, one in every locale.

The independently generated expectation set contains 1,898 detections covering
828 distinct concept IDs. The live Form gate does not trust that set as an
oracle: it submits every sentence, without a target surface, to the existing
full 10,000-label detector and compares the resulting candidates with the
stored counts and expected IDs.

## Operational interfaces

`hcnl13-runtime-detect-index` performs live unprompted full-10k detection for a
source row and returns rich semantic candidates.
`hcnl13-runtime-detect-index-ids` uses the same exact occurrence law and walks
all 10,000 labels while omitting rich candidate construction, so
`hcnl13-runtime-scan` can perform the complete walk for all 147 rows and return exact
row, pass/failure, detection, unique-concept, role, attribution, and review
totals. `hcnl13-runtime-quote-for-concept` retrieves a source sentence only
when that locale/concept pairing exists in the selected domain rows; its result
begins with `attributed-human-quote-not-novel-generation` and carries the
author, license, URL, review state, and row hash.

`hcnl13-runtime-world-admit-index` runs the independent sentence detector first,
then consults the source row's expected ID only to select from the candidates
that actually came back. It persists the ID read from that returned candidate
through `cwm-persist`; on a miss it returns an explicit abstention and creates
no world entity. The live band exercises both an Indonesian water admission and
an Indonesian zero-detection abstention.

The stable completion-ledger row is available from
`hcnl13-completion-evidence`. It keeps attribution (147), human review (0), and
novel fluent generation (0) as separate fields.

## Reproduction and observation

```sh
./cognition/concept-human-corpus-13-fetch.sh
./fkwu --src cognition/tests/concept-human-corpus-13-live-band.fk
./fkwu --src cognition/tests/concept-human-corpus-13-live.fk
```

The fetch script uses `curl`, `bzip2`, and Node, stores full archives only in a
temporary directory, verifies their hashes, and requires the selected TSV,
manifest, Form offsets, and Form metadata to regenerate byte-for-byte. Python
is not invoked. The kernel C seed was not changed.

Observed results:

- exhaustive sentence gate: `1023` — 147/147 valid, 0 failures, 1,898
  detections, 828 unique concepts, and exact 122/12/13 role totals;
- detection-to-world gate: `15` — Indonesian water admitted from returned
  candidate 377, while the Indonesian zero-detection row abstained.

An earlier exhaustive run returned `1007`: every content/count bit passed, but
the test had wrongly asserted that Arabic had no selected bus quote. The public
corpus really contains Arabic bus sentence 1593949. Correcting the absence probe
to the genuinely absent Arabic money pairing produced `1023`; no corpus result
was removed to make the gate pass.

## Honest boundary

Tatoeba's detailed export establishes contributor attribution, not
native-speaker status or human review. Therefore all 147 rows remain
`human-contributed-unreviewed`, with human-reviewed coverage exactly 0. These
are independent authentic corpus rows, not asserted parallel translations.
Quoted sentence retrieval is useful grounded language material, but it is not
a generative language model and does not close the 10,000 × 13 fluent-generation
gap.

The movement stayed alive by replacing invented fixture prose with attributable
voices while keeping their uncertainty attached. The surprising teaching was
that real sentences expanded observed concept coverage to 828 even though only
ten everyday domains guided selection. Discomfort turned to gold at the zero:
instead of hiding sentences the detector cannot read, one miss per language is
now permanent executable evidence of where the body still needs to grow.
