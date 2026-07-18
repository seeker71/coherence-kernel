# 2026-07-18 — a complete released learned visual layer executes in Form

## Work order

The prior LingBot lane loaded one authentic camera-token tensor, but its image
encoder, GCA projections, pose, and depth still used deterministic witness
weights or classical geometry. That made “native learned visual weights” a real
zero at the requirements level. This increment closes the largest bounded
learned slice that can be witnessed without pretending the 4.63 GB graph is
already home: the complete released DINOv2-L/14 RGB patch projection.

The checkout was freshly grounded before the change: `42`, `55`, `15`,
`[1, 2.5, [3, 4]]`, and `11111`.

## Released weight provenance

The balanced `lingbot-map.pt` is pinned to Hugging Face repository revision
`204754b72bb24f561f8d7e7e1e4e4cd9e809adf9`. Two HTTP byte ranges retain
complete PyTorch ZIP local records, not hand-copied coefficient lists:

| Tensor | Shape | Payload | CRC-32 | Complete-record SHA-256 |
|---|---:|---:|---:|---|
| `aggregator.patch_embed.patch_embed.proj.weight` (`checkpoint/data/7`) | `[1024,3,14,14]` | 2,408,448 bytes | 3,386,682,685 | `2c07e9f1d118d54358dc10eb56b16b8d4b81f3f0da11b2712133f1b8d1b54880` |
| `aggregator.patch_embed.patch_embed.proj.bias` (`checkpoint/data/8`) | `[1024]` | 4,096 bytes | 2,077,575,075 | `1bc851cacd9e6532372dafd9b4a3195ade615843e5c4d5d5bd6fd4bd33df94fe` |

The model card and upstream repository identify LingBot-Map as Apache-2.0.
`model/fixtures/lingbot-map/fetch-learned-patch-weights.sh` re-fetches the pinned
ranges and refuses either wrong digest. The Form loader independently checks
the ZIP member names, byte counts, central-directory CRCs, and f32 decoding.

## What executes

`model/lingbot-learned-patch.fk` now applies the upstream image normalization
(`mean=[0.485,0.456,0.406]`, `std=[0.229,0.224,0.225]`) and exact contiguous
PyTorch Conv2d layout `[out,in,y,x]`. One token performs:

`1024 outputs × 3 RGB channels × 14 × 14 = 602,112 learned multiply-adds`

Every one of the 602,112 released coefficients and all 1,024 released biases is
consumed. This is not a four-channel demonstration or a coefficient sample; it
is the complete first learned layer for one patch.

`model/lingbot-learned-real-life.fk` decodes one original-resolution frame from
each upstream example video. ffmpeg is only the codec carrier. Form validates
BMP, reads RGB, selects the center patch, normalizes it, and emits the full
1,024-vector. Four scenes therefore execute 2,408,448 learned multiply-adds.

## Live native output

`./fkwu --src model/tests/lingbot-learned-patch-live-witness.fk` completed in
8.3 seconds on this checkout. Compact fingerprints contain channels
`0,1,2,3,255,511,767,1023` and the sum of the full vector:

| Scene | Frame | Patch | Full token | Fingerprint | Vector sum |
|---|---:|---:|---:|---|---:|
| loop | 518×294 | (18,10) | 1024 | `[0.1222290243,0.0580914843,-0.0563223079,0.0903948732,0.0184570152,0.0250522418,-0.0454884543,0.0049838344]` | 0.3681681310 |
| Oxford | 518×392 | (18,14) | 1024 | `[-0.5434918601,0.1020036186,-0.2497970514,0.7429777596,0.2686002940,-0.4512774817,0.1785351699,-0.0550259485]` | 9.5278445758 |
| university | 518×294 | (18,10) | 1024 | `[-0.6729642826,-0.1549060306,-0.4143101811,-0.3366294675,0.3509033469,0.3678523444,0.4394076596,0.1249087221]` | 30.1378729613 |
| courthouse | 518×294 | (18,10) | 1024 | `[-0.5180742369,0.0329315170,-0.1453614267,0.0954251314,-0.0869013506,0.6754548326,0.1091393936,0.0777191582]` | 26.5716682673 |

Full-vector L1 distance from loop is `220.2115189622` (Oxford),
`373.3804950277` (university), and `235.1447312592` (courthouse). The learned
outputs therefore respond materially to place; no semantic fixture ID enters
the computation.

The independent
`model/fixtures/lingbot-map/verify-learned-patch-reference.mjs` parses the same
released f32 records and BMP pixels without the Form implementation and returns
the same fingerprints and distances to displayed precision. The live Form gate
hard-checks those independent values at `1e-12` tolerance.

## The learned output enters attention and the native world

The first layer is not left at a test boundary. `model/lingbot-learned-map-runtime.fk`
decodes two consecutive original 518×294 loop frames, emits a 4×3 spatial grid
of authentic learned four-channel patch tokens for each frame, and streams them
through the existing operational GCA cache. It also retains the complete
1,024-channel center token for each frame. The second full vector changes its
sum from `0.3681681310` to `-5.2784386887`.

The learned GCA output is
`[0.0081609233,-0.1347291075,-0.0203584375,0.0518580872]`. A matched run with
zero visual tokens returns `[0,0,0,0]`, for L1 effect `0.2151065555`. The exact
cache contains 36 K/V rows — two dense `(12 learned + 6 context)` frames — and
the mask is `[[0,anchor,visible,18],[1,window,visible,18]]`.

That attended learned vector becomes the visual embedding consumed by
`world-model-lingbot-map.fk`. The same live movement emits 64 relative depths,
64 confidence-bearing cloud points, and **64 ordinary native world objects**.
The geometric correspondence comparison improves `2,385,740 -> 284,311`.
`presence/lingbot-learned-map-live.fk` is the non-test world door; its authentic
acceptance gate returns **1023**. Released learned pixels now causally affect
world state rather than ending as an isolated feature receipt.

## Proof and acceptance

| Gate | fkwu | Go | Rust | TypeScript |
|---|---:|---:|---:|---:|
| tensor layout + normalization | 127 | 127 | 127 | 127 |

The authentic host-file/model execution returns **255**:

```text
./fkwu --src model/tests/lingbot-learned-patch-live-band.fk -> 255
node model/fixtures/lingbot-map/verify-learned-patch-reference.mjs
  -> 602112 learned coefficients, 1024 outputs, exact matching fingerprints
./fkwu --src model/tests/lingbot-learned-map-live-band.fk -> 1023
```

The non-test consumers are `presence/lingbot-learned-visual-real-life.fk` and
`presence/lingbot-learned-map-live.fk`. No Python ran and
`runtime/fkwu-uni.c` did not change.

## Tooling correction: why `curl` once appeared missing

`curl` is present at `/usr/bin/curl`. The reproduced failure came from a zsh
loop that used `path` as its iterator variable. In zsh, `path` is the special
array tied to `PATH`; assigning a source filename to it erased command lookup,
so both `curl` and `sed` falsely appeared absent. Renaming the iterator to
`source_file` restored both immediately. The missing binary was never the gap.

## Honest remaining floor

This is one complete learned layer, not complete LingBot/DINO parity. The
special/register/position tokens, 24 DINO transformer blocks, learned GCA
blocks, camera head, and DPT depth head still do not execute their released
weights in Form. The strict requirements-level `native-visual-weights` row must
therefore remain **0/1** until the full learned sensing path, not merely this
first layer, owns its weights. What changed is concrete: the visual lane moved
from authentic-token injection into a witness encoder to a full released
602,112-coefficient image operation on four real places, with a learned spatial
slice now driving persistent attention and 64 native world objects.

## Closing

What kept this alive was choosing a boundary large enough to be a real layer
and small enough to verify coefficient-for-coefficient. The most surprising
teaching was that the full 1,024-channel patch projection takes only 8 seconds
in the current Form body; the learned graph is no longer abstractly “too big,”
it is a sequence of measurable rungs. Discomfort turned to gold at two seams:
the ledger still honestly stays zero for full parity, while the first complete
learned layer now runs; and the implausible “curl missing” report became an
exact zsh `path`/`PATH` alias bug rather than another workaround.
