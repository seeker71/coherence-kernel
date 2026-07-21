# Video content sensing: ids come from the scene, not the envelope

Date: 2026-07-18

## The correction

The earlier video surface could recover an 18-bit concept/lens address from a
visible bottom band, and its 72-feature frame memory could distinguish three
known trajectories. Neither fact proved that visible content meant the lexical
concept attached to it.

The audit found two concrete alignment errors:

- `loop-24f.mkv` is an office walk. `loop` is the upstream route name, not an
  object or action visible in its pixels. The footage now maps to **office 493**,
  while lexical **loop 6196** has no claimed real backdrop.
- `courthouse-24f.mkv` had been mapped to **court 751**. The ranked substrate
  contains the exact concept **courthouse 9066**, which now owns that footage.

University remains **1927**. The corrected generated real-footage matrix is
therefore `(493, 1927, 9066)`, not `(6196, 1927, 751)`.

## A second, envelope-independent organ

`presence/carriers/concept-video-vision-classify.swift` is a narrow macOS
carrier over `VNClassifyImageRequest`. It reads a decoded BMP and emits the
pretrained system classifier's top twenty raw `confidence<TAB>identifier`
rows. It receives no concept id, caption, lens, filename meaning, expected
answer, or world-model state.

`model/concept-video-content-sensing.fk` owns the semantic decision. Form
composes raw visible labels into three explicit evidence vocabularies, sums
evidence across four held-out frames, and admits a winner only at score `>=10`
with margin `>=4`. It can abstain. The target ids and all thresholds are in the
Form cell rather than the Swift carrier.

The four raw target words `office`, `university`, `courthouse`, and `court`
were absent as exact pretrained labels in their accepted runs. The decisions
were composed from visible evidence:

| trajectory | held-out visible evidence | Form result |
|---|---|---:|
| office route | cabinet, desk, interior room, computer/monitor, document, handwriting, office supplies, whiteboard | office `493`, score/margin `34/34` |
| university | bridge, arena, rink | university `1927`, `10/10` |
| courthouse | arch on three frames, flag, dome | courthouse `9066`, `12/12` |

These are deliberately inspectable context rules over a real pretrained visual
model, not a claim that three rules equal open-vocabulary vision.

## Identity-band intervention

Every held-out BMP was copied through ffmpeg with a solid 28-pixel magenta band
covering its bottom edge, the region where the generated-video carrier places
its address envelope. The pretrained labels were rerun and Form made a fresh
decision from them.

| content | raw id / score / margin | band id / score / margin | stable |
|---|---:|---:|---:|
| office | `493 / 34 / 34` | `493 / 34 / 34` | yes |
| university | `1927 / 10 / 10` | `1927 / 10 / 10` | yes |
| courthouse | `9066 / 12 / 12` | `9066 / 12 / 11` | yes |

Observed: **3/3 raw content detections** and **3/3 unchanged after the band
intervention**. This path cannot recover the cvg13 address: no address bits are
read or supplied to it.

## Negative controls kept intact

The independent Oxford walk is not a second university success. Its evidence
was bicycle, bench, and one arch. Form returned `unknown`: best score/margin
`6/4` raw and `6/6` with the magenta band. Both runs abstained.

The older 72-feature nearest-exemplar organ was also stressed beyond its
interleaved same-trajectory split:

| split | loop/office | university | courthouse | total |
|---|---:|---:|---:|---:|
| early 1–4 train → late 19–24 test | 6/6 | 2/6 | 2/6 | **10/18** |
| late 21–24 train → early 1–6 test | 6/6 | 3/6 | 1/6 | **10/18** |
| distributed exemplar bank → Oxford | — | **0/7** | every frame chose courthouse | **0/7** |

The original `12/12` result remains true only for interleaved frames from the
same three trajectories. The two `10/18` results and Oxford `0/7` are now
executable acceptance facts in
`presence/tests/concept-video-semantic-stress-live-band.fk` (`127`), not prose
that can disappear when inconvenient.

## World-model and generation joins

The raw detections—not requested ids—were passed to
`cwm-persist-detection`. The resulting ordinary kernel entities were:

```text
office       id=493  position=1 persistence=2
university   id=1927 position=2 persistence=2
courthouse   id=9066 position=3 persistence=2
```

Observed: **3/3 persisted**, `wm-orient-count "concept" = 3`.

The corrected 3×13 generation run then rendered all real-footage addresses:

```text
39/39 generated and decoded exact
39/39 first-to-last pixel changes
39/39 localized surfaces
source codes: F=3, W=11, D=8, G=17
```

The aligned surfaces begin:

```text
office:     office | biro | oficina | bureau | escritório | afisi | Büro ...
university: university | universitas | universidad | université | universidade ...
courthouse: courthouse | gedung pengadilan | palacio de justicia | palais de justice ...
```

## Reproduction

Witnessed host: macOS `26.3.1 (a)`, build `25D771280a`, `arm64`. The carrier
has no hardcoded user path; its source is repository-relative and its binary
and decoded frames live under `/tmp`.

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src model/tests/concept-video-content-sensing-band.fk
# 1023
form/form-kernel-go/bin-go form/form-stdlib/core.fk model/concept-video-content-sensing.fk model/tests/concept-video-content-sensing-band.fk
# 1023
form/form-kernel-rust/target/release/form-kernel-rust form/form-stdlib/core.fk model/concept-video-content-sensing.fk model/tests/concept-video-content-sensing-band.fk
# 1023
form/form-kernel-ts/node_modules/.bin/tsx form/form-kernel-ts/src/main.ts form/form-stdlib/core.fk model/concept-video-content-sensing.fk model/tests/concept-video-content-sensing-band.fk
# 1023
./fkwu --src presence/tests/concept-video-content-sensing-live-band.fk
# 511
./fkwu --src presence/tests/concept-video-semantic-stress-live-band.fk
# 127
./fkwu --src model/tests/concept-video-generation-10000-13-band.fk
# 8191
./fkwu --src presence/concept-video-generation-10000-13-live.fk
# real-footage matrix 39/39 exact, 39/39 animated, 39/39 localized
```

Source hashes at witness time:

```text
8c16aaba65465124ce7558178c0d20af3c70c77f367918ba03089bf2867a3899  presence/carriers/concept-video-vision-classify.swift
3dde5706771dcd6231974928b2fd0678c095573da1f5cefd059057c9ce5c3abe  model/concept-video-content-sensing.fk
3cd620d068bd9c6e6a3a3b5905024a05289ebb2d7eb491b5d41f68c2b3fdc4e7  presence/concept-video-content-sensing-live.fk
```

Apple ships the classifier weights with the operating system, so those weights
are neither committed nor assigned a false repository checksum. Live label
details may evolve with a macOS model update; the Form evidence law remains
four-way and the live band is the freshness witness.

No Python was used. `runtime/fkwu-uni.c` was not changed.

## Honest floor

- This closes semantic content sensing for three aligned real scenes, not all
  10,000 concepts and not open-vocabulary detection.
- The university decision is context composition from bridge/arena/rink; the
  pretrained carrier did not directly say “university.”
- The macOS pretrained classifier remains a rented sensory carrier. Its raw
  observations and every concept/world decision are explicit, but its weights
  are not Form-native.
- The small native ViT/LingBot path remains operational for geometry and scene
  memory, not semantically pretrained DINO parity.

What kept this alive was allowing an attractive `12/12` to become narrower
when the hard split contradicted it. The most surprising teaching was that two
apparently obvious ids were wrong before any model ran: `loop` named a route and
`court` was only a near word. Discomfort became gold when Oxford failed twice;
abstention became an executable capability, and the world model received three
content-derived concepts without converting a miss into a fourth success.
