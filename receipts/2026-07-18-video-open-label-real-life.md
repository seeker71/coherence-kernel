# Video sense expands from three scenes to nine real concepts

Date: 2026-07-18

## Work order and fixture ground

The existing semantic video lane had three accepted trajectories—office,
university, and courthouse—and one Oxford abstention. That was real content,
but it remained a hand-authored three-concept evidence vocabulary.

This movement adds ten attributable Wikimedia Commons photographs under
`model/fixtures/concept-vision-real-life/`. They total 3,091,245 bytes and span
animals, food, transport, infrastructure, a drink, and a flower. The committed
`PROVENANCE.tsv` gives the exact Commons page, author, license, license URL,
SHA-256, byte size, and dimensions for every file. `fetch.sh` downloads those
same derivatives with `/usr/bin/curl` and refuses any checksum change.

Nine photographs are positive targets. The tenth visibly contains a person
playing a guitar, but the witnessed carrier never names `guitar` in its top
twenty labels. That miss is preserved as a target-specific hard negative.

## The open-label bridge

The unchanged Swift carrier receives only an opaque image path and returns the
operating system classifier's top twenty `confidence<TAB>label` rows. It does
not receive a filename-derived label, target id, caption, provenance row, or
identity envelope.

`model/concept-video-open-label.fk` then joins those raw labels to
`ctd13-runtime-detect-sentence`, the complete 10,000-row English detector. For
every matched 10k candidate, Form recovers the exact raw classifier confidence
and keeps all observations at or above 100,000 ppm. Exact-line recovery rejects
substring-only accidents such as treating `span` as the raw label `spaniel`.

An accepted observation row is:

```text
(concept-id English-surface vision-confidence-ppm text-score source-code
 anchor primary-synset primary-gloss)
```

This is deliberately a set, not a forced single answer. A train photograph can
honestly evoke train, railroad, vehicle, machine, snow, sky, and water. The
fixture target is consulted only after the observation set exists.

## Live target scores

Each target was classified three times: original photograph, a fresh copy with
an opaque 80-pixel magenta bottom band, and an 80% center crop. All three runs
decode and classify fresh pixels.

| photograph | ranked 10k target | id | original ppm | bottom-band ppm | center-crop ppm | in full 10k scan |
|---|---|---:|---:|---:|---:|---:|
| B-17/B-52 flight | airplane | 5209 | 893066 | 901856 | 778809 | yes |
| bananas | banana | 4731 | 984375 | 952148 | 988281 | yes |
| Canberra bridge | bridge | 1520 | 539551 | 505371 | 373291 | yes |
| cat with lizard | cat | 1040 | 145024 | 132572 | 273682 | yes |
| coffee cup | coffee | 665 | 847168 | 834473 | 814941 | yes |
| golden retriever | dog | 537 | 931621 | 933656 | 932502 | yes |
| Margherita pizza | pizza | 2093 | 876465 | 823242 | 912109 | yes |
| sunflower | flower | 2304 | 625001 | 547853 | 348389 | yes |
| locomotive | train | 786 | 847656 | 845215 | 798828 | yes |
| guitarist | guitar | 2743 | 0 | 0 | 0 | no, retained negative |

`sunflower` is not itself an English anchor in this ranked 10k. The classifier
emitted both `flower` and `sunflower`; the bridge honestly admits the available
ranked concept `flower` 2304 rather than inventing a sunflower id.

Observed: **9/9 targets in original raw content**, **9/9 after bottom-band
occlusion**, **9/9 after center crop**, and **9/9 present in the complete Form
10k scan**. Guitar stays absent in all three variants.

## Full accepted original observation sets

These are every exact raw label at or above the 100,000 ppm floor that also has
an English concept in the ranked 10k—not only the expected target:

| photograph | accepted `(id surface ppm)` |
|---|---|
| airplane | `(377 water 605957)`, `(2262 vehicle 893066)`, `(4753 liquid 605957)`, `(1941 ocean 605957)`, `(5209 airplane 893066)`, `(996 machine 893079)`, `(4768 aircraft 893066)`, `(1285 sky 305671)` |
| banana | `(2435 fruit 984375)`, `(532 food 984375)`, `(4731 banana 984375)` |
| bridge | `(377 water 392325)`, `(4000 structure 661682)`, `(1520 bridge 539551)`, `(4753 liquid 392325)`, `(1793 lake 387695)`, `(1123 river 183105)`, `(700 land 183300)`, `(1285 sky 899034)` |
| cat | `(4000 structure 360131)`, `(1040 cat 145024)`, `(9561 leash 176270)`, `(1338 animal 384917)`, `(6463 cord 180640)`, `(2480 rocks 360107)` |
| coffee | `(1437 cup 951172)`, `(665 coffee 847168)`, `(2104 plate 630371)`, `(397 drink 847177)`, `(4753 liquid 847177)`, `(5857 spoon 783203)` |
| dog | `(700 land 387695)`, `(3042 grass 387695)`, `(537 dog 931621)`, `(1338 animal 931621)` |
| pizza | `(2104 plate 447022)`, `(2093 pizza 876465)`, `(532 food 876466)` |
| sunflower | `(3904 branch 103027)`, `(1958 plant 837803)`, `(2304 flower 625001)`, `(1285 sky 158936)` |
| train | `(2262 vehicle 847816)`, `(377 water 261475)`, `(4753 liquid 261475)`, `(3285 frozen 261475)`, `(6302 railroad 501483)`, `(4000 structure 501811)`, `(786 train 847656)`, `(1791 snow 261475)`, `(996 machine 847816)`, `(3417 fence 109863)`, `(1285 sky 303955)` |
| guitarist negative | `(2365 material 131934)`, `(117 people 171160)`, `(4000 structure 344498)`, `(2861 adult 171143)`; guitar absent |

This table matters: it shows where the pretrained model is broad, where scene
context accompanies the object, and where the visual target is too small to
make the carrier's top twenty.

## Executable witnesses

```sh
./fkwu --src model/tests/concept-video-open-label-band.fk
# 511
form/form-kernel-go/bin-go form/form-stdlib/core.fk cognition/concept-text-detection-13.fk model/concept-video-open-label.fk model/tests/concept-video-open-label-band.fk
# 511
form/form-kernel-rust/target/release/form-kernel-rust form/form-stdlib/core.fk cognition/concept-text-detection-13.fk model/concept-video-open-label.fk model/tests/concept-video-open-label-band.fk
# 511
form/form-kernel-ts/node_modules/.bin/tsx form/form-kernel-ts/src/main.ts form/form-stdlib/core.fk cognition/concept-text-detection-13.fk model/concept-video-open-label.fk model/tests/concept-video-open-label-band.fk
# 511
./fkwu --src presence/tests/concept-video-open-label-live-band.fk
# 1023
```

Witnessed host: macOS 26.3.1 (a), build 25D771280a, arm64. Source hashes:

```text
58d324d6c864a69f2f6f8a1696e5afcf1b8917b5eebd15509d59fa902e30419b  model/concept-video-open-label.fk
41f817835a12715020f0801d8cb26131091b04aca5940e6809c6da14d422fd2a  presence/concept-video-open-label-live.fk
8c16aaba65465124ce7558178c0d20af3c70c77f367918ba03089bf2867a3899  presence/carriers/concept-video-vision-classify.swift
```

No Python was used. `runtime/fkwu-uni.c` was not changed.

## Honest floor and closing

- This is open-label classification bridged into the complete 10k text surface,
  not learned 10k visual parity. Only labels emitted by Apple's top twenty can
  enter; there are no boxes, masks, relations, temporal tracks, or Form-native
  pretrained visual weights.
- The photographs are independent real examples, not video sequences. The
  existing real trajectories remain the temporal/geometry witness.
- The band/crop interventions show resilience for these nine targets; they do
  not establish general adversarial robustness.
- Apple owns the rented classifier weights, and an operating-system update may
  change live scores. The raw rows and live band are therefore the freshness
  witness.

What kept this alive was letting every high-confidence label enter the record
instead of collapsing a rich scene to one expected noun. The most surprising
teaching was that the complete 10k join exposed useful co-observations—bridge
with river and lake, train with railroad and snow—without any scene rule. The
discomfort was a clear guitar that the classifier would not name; it turned to
gold when the miss became an executable negative and a precise sensory limit,
not a hand-authored guitar success.
