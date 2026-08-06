# Public real photographs expand the content world by 24 distinct cells

Date: 2026-07-18

## What became real

The earlier real-photo floor held 28 content concepts across small corpora.
This increment adds **24 provenance-pinned real photographs**, **24 domains**,
and **24 distinct top-10k target cells**. All original pixels, source
attribution, exact raw model outputs, perturbations, candidate sets, misses,
and world admissions are retained.

This is a post-sweep content corpus: 50 Wikimedia Commons Quality Images rows
were acquired through the authoritative MediaWiki API, the content-only model
was run, and 24 diverse decoded photographs were frozen. Consequently 24/24 is
**not held-out accuracy**. It is the honest count of selected public images for
which a semantic content observation entered the body.

## Public-source and license floor

`model/fixtures/concept-vision-public-snapshot/SOURCE-SNAPSHOT.json` records:

- the exact Commons API endpoint and `2026-07-18T06:52:00Z` retrieval time;
- the 50-row response identity
  `0a69b370f7f17aa24ec9c3a4a91fe98c0807390d916e9a5310d1cbff330fbd7e`;
- every selected page ID, title, Commons file page, derivative URL, author,
  license, license URL, pixel dimensions, byte size, and image SHA-256;
- the explicit rule that selection followed the model sweep and metadata never
  entered inference.

The 24 rows comprise 17 CC BY-SA 4.0, four CC BY 4.0, two CC0, and one
CC BY-SA 3.0 Poland image. `PROVENANCE.tsv` carries the flat attribution view.
The 24 committed derivatives total 5.7 MiB. `fetch.sh --verify-only` validates
all source bytes offline; network mode downloads only the pinned derivative
URLs and refuses a checksum mismatch.

Acquisition misses remain in `ACQUISITION-ATTEMPTS.tsv`: the official COCO
host presented a certificate subject mismatch, so no insecure bypass was
used; an exploratory Commons search reached HTTP 429 after 19 of 30 terms; and
the first ffmpeg crop command was malformed by an unquoted zsh glob. None of
those outputs was promoted. The corrected quoted intervention succeeded for
all 24 frozen images.

Artifact identities:

| artifact | SHA-256 | bytes / rows |
|---|---|---:|
| `SOURCE-SNAPSHOT.json` | `a931ff05828b9a21fb5ed37cbf1d576d533220fce17da1c624e294c346456af9` | 23,121 bytes |
| `PROVENANCE.tsv` | `e1ca9f425437b17cf77bd7aac9e6a9ad9c905a7741787b6b8f99e287570a1f4d` | 14,010 bytes / 24 rows |
| `MODEL-OUTPUTS.tsv` | `633a5fbf782c57be236f1be4f5ab82177a334b83ec604e829b72f28dc5189f84` | 40,590 bytes / 1,440 observations |

## Content-only model path

The unchanged thin Swift carrier receives one numeric local image path and
decoded pixels. It emits Apple's top 20 `confidence<TAB>label` observations.
It never reads `SOURCE-SNAPSHOT.json`, `PROVENANCE.tsv`, a Commons title,
caption, domain, target ID, target label, or address band.

For every image the live Form organ:

1. classifies the original numeric fixture;
2. creates and classifies an opaque 80-pixel bottom occlusion;
3. creates and classifies an 80% center crop;
4. retains all 72 exact raw top-20 streams;
5. extracts unique labels at the explicit 100,000-ppm visual floor;
6. runs one complete 10,000-anchor candidate sweep over those content labels;
7. filters that candidate bank back against each exact raw stream and its
   confidence, preventing a label from another photograph from leaking; and
8. consults the fixture target only after all content observations exist.

The complete candidate bank held 87 top-10k cells. Each public row carries its
exact original/occlusion/crop candidate lists and raw top-20 strings. This is a
complete Form text join over host labels; it is not a learned 10k visual head.

The exact model floor is Apple Vision `VNClassifyImageRequest` on macOS 26.3.1
build 25D771280a, arm64, with Swift 6.2.3. The carrier source SHA-256 was
`8c16aaba65465124ce7558178c0d20af3c70c77f367918ba03089bf2867a3899`.
Apple does not expose the rented learned weight bytes or a weight hash. That
absence is recorded rather than replaced by a carrier-source hash.

## Observed content, not filenames

Confidence order is original / bottom occlusion / center crop.

| domain | cell | content label | confidence ppm | states |
|---|---:|---|---:|---:|
| built heritage | 752 | building | 910645 / 966309 / 938477 | 1 / 1 / 1 |
| museum object | 8497 | vase | 352051 / 607422 / 687500 | 1 / 1 / 1 |
| road transport | 248 | car | 580566 / 649414 / 487793 | 1 / 1 / 1 |
| environmental action | 2365 | material | 175293 / 152345 / 155274 | 1 / 1 / 1 |
| mountain landscape | 1595 | mountain | 422852 / 318604 / 125977 | 1 / 1 / 1 |
| cultural performance | 4515 | jewelry | 587402 / 554199 / 625488 | 1 / 1 / 1 |
| signage | 4598 | document | 410507 / 406584 / 260973 | 1 / 1 / 1 |
| urban art | 998 | art | 858409 / 486334 / 492928 | 1 / 1 / 1 |
| maritime wreck | 883 | boat | 130307 / 155854 / 134737 | 1 / 1 / 1 |
| public sculpture | 4032 | statue | 614258 / 680176 / 847168 | 1 / 1 / 1 |
| ocean transit | 1941 | ocean | 562500 / 627441 / 480225 | 1 / 1 / 1 |
| harbor infrastructure | 5585 | dock | 502441 / 450928 / 364014 | 1 / 1 / 1 |
| archaeology | 1520 | bridge | 739258 / 738281 / 725098 | 1 / 1 / 1 |
| insect wildlife | 5547 | butterfly | 804199 / 629395 / 454102 | 1 / 1 / 1 |
| urban high-rise | 1020 | apartment | 114258 / 88135 / 78369 | 1 / 0 / 0 |
| recreation | 8249 | playground | 429200 / 379151 / 297623 | 1 / 1 / 1 |
| sound sculpture | 1215 | bell | 416504 / 368164 / 537109 | 1 / 1 / 1 |
| bamboo forest | 1874 | forest | 671875 / 731445 / 460449 | 1 / 1 / 1 |
| residential architecture | 1795 | roof | 642578 / 451172 / 733887 | 1 / 1 / 1 |
| civic access | 296 | door | 367432 / 425049 / 691406 | 1 / 1 / 1 |
| rural agriculture | 4091 | barn | 442383 / 666504 / 739258 | 1 / 1 / 1 |
| seascape | 377 | water | 732434 / 676311 / 869149 | 1 / 1 / 1 |
| material decay | 4000 | structure | 498297 / 514803 / 376338 | 1 / 1 / 1 |
| public landscape | 1776 | path | 731934 / 463135 / 392334 | 1 / 1 / 1 |

Totals are **24/24 original**, **23/24 occluded**, and **23/24 cropped**.
`apartment` remains the only perturbation miss; its observed confidences stayed
in the raw output and were not rounded upward.

## Negative claims and abstention

Eleven human-visible source claims are evaluated only after observations.
Seven remain exact three-way misses at the 100,000-ppm floor:

```text
mill  wreck  ship  bamboo  access  wood  sculpture
```

Four probes are intervention-sensitive or positive: `bottle` is 0/0/1,
`forest` 1/1/0, `costume` 1/1/1, and `sign` 0/0/1. These asymmetries are
retained. A `cruise_ship` host label does not manufacture exact `ship`; an
underscore-bearing adjacent label cannot count by substring.

## Ordinary world-model admission

`presence/concept-video-public-snapshot-world-live.fk` reruns the pixel path
and admits only targets found in each original content candidate set. All 24
distinct targets become ordinary `cwm` / `wm-model` entities. The seven
three-way claim misses create no entity. Positions `[1..24, 0, 0]` are explicit
collection observation slots, not inferred object centers, camera poses, depth,
or geometry.

The pure `model/concept-video-public-snapshot-observed.fk` exposes the exact
24/24/23/23/7/24 floor and the three artifact hashes to the central completion
ledger without rerunning host Vision. It explicitly says `held-out-accuracy 0`
and `native-visual-weights 0`; the live and world gates remain authority.

## Executable witnesses

```sh
model/fixtures/concept-vision-public-snapshot/fetch.sh --verify-only
# 24 Wikimedia Commons public-snapshot photographs verified

model/fixtures/concept-vision-public-snapshot/verify-model-outputs.sh
# 72 exact Apple Vision top-20 streams match committed snapshot

./fkwu --src model/tests/concept-video-public-snapshot-observed-band.fk
# 4095

./fkwu --src presence/tests/concept-video-public-snapshot-live-band.fk
# 4095

./fkwu --src presence/tests/concept-video-public-snapshot-world-live-band.fk
# 1023
```

No Python ran. `runtime/fkwu-uni.c` did not change.

## Honest boundary

- The 24 images were admitted after a model sweep; this is corpus expansion,
  not a held-out benchmark.
- The candidate join covers all 10,000 text anchors, but the rented visual head
  does not expose or learn 10,000 classes.
- The classifier is Apple-hosted and can change with an OS update. The exact
  output comparator makes such drift visible.
- There are no boxes, masks, temporal tracks, camera poses, depth estimates,
  native ViT/DINO weights, or Form-native visual learned weights here.
- These are real photographs, not videos; video-temporal parity remains open.

What kept this alive was refusing to let public captions become visual facts:
the path stayed numeric, and only raw pixel-derived candidates entered the
world. The most surprising teaching was intervention asymmetry—cropping made
`bottle` and `sign` visible while erasing `apartment`, so perturbations are not
monotone difficulty. Discomfort became gold when the first complete run showed
24 high-confidence labels but zero target states: the target IDs were one off
because the OMW table header had been counted. Correcting the IDs, then rerunning
the raw-pixel gate, turned an attractive but false zero into 24 auditable world
admissions.

; witnessed: 2026-07-18 -> offline source 24/24, exact raw streams 72/72,
;                            live 4095, world 1023, pure observed 4095
