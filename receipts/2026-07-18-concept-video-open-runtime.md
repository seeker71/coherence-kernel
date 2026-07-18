# Content-only concept video generation and sensing

Date: 2026-07-18

## What changed

`presence/concept-video-open-runtime-live.fk` now accepts any valid
`(concept-id, NL-lens)` address. Form obtains the attributed localized surface,
English anchor, and WordNet gloss. A narrow Swift carrier receives only those
UTF-8 strings and produces a four-frame 960×540 semantic card. It receives no
concept id or lens id and draws no barcode, identity band, filename label, or
hidden metadata. Form encodes the frames as lossless FFV1 and requests decoded
first and last BMPs.

A second carrier runs Apple Vision OCR on those decoded pixels. The OCR text
enters the complete 10,000-concept detector for the requested lens. Only a
candidate containing the expected concept id plus an exact OCR observation of
the visible surface counts as localized semantic evidence. If that fails, the
separately visible English anchor may enter the complete English scan; this is
reported as anchor evidence, never localized evidence. Apple Vision's raw
pretrained image labels are retained as an independent lane.

The runtime explicitly reports requested address and sensed content in
different fields. Decoded address evidence is **zero by construction**.

## Broad held-out generated-video matrix

The live gate predeclares thirteen distinct concepts and uses every NL lens
exactly once. None of the ids or expected strings occur in the renderer, OCR
carrier, classifier, or acceptance functions.

| lens | id | visible localized surface | generated | animated | expected id from content |
|---|---:|---|---:|---:|---:|
| en | 377 | water | yes | yes | yes |
| id | 959 | kamera | yes | yes | yes |
| es | 270 | familia | yes | yes | yes |
| fr | 365 | musique | yes | yes | yes |
| pt-br | 786 | trem | yes | yes | yes |
| sw | 537 | mbwa | yes | yes | yes |
| de | 1520 | Brücke | yes | yes | yes |
| ru | 5037 | демократия | yes | yes | yes |
| zh | 628 | 医院 | yes | yes | yes |
| ja | 327 | 学校 | yes | yes | yes |
| ar | 1494 | حرية | yes | yes | yes |
| hi | 1098 | संगणक | yes | yes | yes through visible English anchor |
| tr | 1595 | dağ | yes | yes | yes |

Observed totals:

- generated lossless videos: **13/13**;
- decoded first-to-last pixel change: **13/13**;
- expected concept present from visible content on both frames: **13/13**;
- localized-lens OCR present on both frames: **12/13**;
- decoded address bits: **0/13**;
- exact pretrained-label target: **1/13** (`computer`);
- the `computer` raw-label row also survives a complete English 10k scan.

Hindi is the preserved miss: Vision did not read visible `संगणक`, while it did
read the separately visible `computer` anchor. The runtime therefore reports
`localized=0, anchor=1`; it does not relabel that fallback as Hindi sensing.

An exploratory aggregate scan over all 260 raw pretrained-label rows returned
33 Form candidates and one of the thirteen requested targets. The stable gate
scans the positive `computer` label row instead: rescanning a much longer union
does not strengthen per-card target attribution and adds several minutes of
Form traversal.

## Real-world visual content remains a separate lane

Semantic cards prove arbitrary text-conditioned visual generation and OCR
sensing. They do **not** prove that a learned vision model understands an
arbitrary depicted object. Real-world content evidence remains separate:

- `presence/tests/concept-video-open-label-live-band.fk` runs ten attributed
  Wikimedia Commons photographs through pretrained labels and a complete 10k
  Form scan. Nine positive targets survive original, bottom-band intervention,
  and center crop; guitar remains a named hard negative.
- `presence/tests/concept-video-content-sensing-live-band.fk` senses office,
  university, and courthouse from held-out real video frames, survives a
  magenta identity-band intervention, and abstains on independent Oxford
  footage.

This split prevents OCR-address success from inflating real visual-content
success.

## Gates

```sh
./fkwu --src model/tests/concept-video-open-runtime-band.fk
# 2047

./fkwu --src presence/tests/concept-video-open-runtime-live-band.fk
# 2047

./fkwu --src presence/tests/concept-video-open-label-live-band.fk
# 1023

./fkwu --src presence/tests/concept-video-content-sensing-live-band.fk
# 511
```

The first gate is pure and four-way. The second is intentionally slower: it
performs thirteen complete ambiguity-preserving 10k scans, live OCR on two
decoded frames per row, and live pretrained classification.

## Honest boundary

- The 130,000 valid `(concept, lens)` addresses are generatable lazily; thirteen
  widely separated live addresses were observed, not all 130,000 videos.
- Arbitrary generation is semantic typography and motion, not learned or
  photorealistic text-to-video synthesis.
- OCR is host-pretrained and does not currently read the Hindi sample.
- Pretrained visual labels do not have 10k parity: one of thirteen semantic
  cards and nine of ten real-photo targets were observed in the relevant gates.
- Real video content sensing remains three accepted concepts plus a preserved
  negative, not learned open-vocabulary 10k video understanding.
- No Python ran. `runtime/fkwu-uni.c` was not changed.

The exchange stayed alive by refusing to let a requested address count as a
detection. The surprising teaching was that twelve scripts/lenses survived the
same content-only pixel path without any identity carrier. Discomfort turned
to gold when Hindi OCR failed: keeping the localized miss separate from the
successful visible-English fallback made the evidence more useful, not less.
