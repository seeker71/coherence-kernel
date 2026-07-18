# 2026-07-18 — LingBot-Map leaves the 16×16 room

## Work order

The first live pipeline was honest but deliberately tiny: two 16×16 frames and
one transition. Urs asked to continue into non-toy real-life examples. This
receipt records four native-resolution sequences, long-context state, composed
trajectories, thousands of reconstructed points, and the failure boundary found
when all four transient arenas were held in one process.

All source images come from `Robbyant/lingbot-map` at pinned commit
`7ff6f3ed0913d4d326f8f13bbb429c4ffc0195c2`:

- indoor loop: 237 source frames, select stride 10, indices 0…230
- Oxford outdoor walk: 320 frames, stride 13, indices 0…299
- university walk: 324 frames, stride 14, indices 0…322
- courthouse walk: 286 frames, stride 12, indices 0…276

Each fixture carries 24 selected frames losslessly as FFV1 Matroska, without
spatial resizing:

| Scene | Resolution | Bytes | SHA-256 |
|---|---:|---:|---|
| loop | 518×294 | 1,581,923 | `05104f34e6cb1a88daa36e231eb96d29a91447add37e7199a252a546d9643073` |
| Oxford | 518×392 | 4,842,492 | `d69dc1508421b3d5140faaaf6f09a6bc63ab7e7fc1922ca5abab460d6ab71533` |
| university | 518×294 | 4,582,171 | `1066180bc9b771db7443b47af073d211cd18244aaf38c395347bea825d46ade4` |
| courthouse | 518×294 | 3,785,784 | `4e2b1da3bd91ee965cebb55240c475011f463391c460801ae4613d407be43fea` |

## What changed in the body

`model/lingbot-real-life.fk` partitions every decoded frame into an 8×6 grid.
The 48 tiles exactly cover `width × height`; nested bounded walks accumulate
every RGB pixel in each tile before projection. There is no 16×16 proxy and no
single-pixel sample. Each frame therefore becomes 48 regional tokens plus the
ViT frame token: a 49×4 encoded frame.

Across the four runs, Form observed **15,838,368 source pixels**. Each scene
streams 24 frames through the operational cache with 3 dense anchors, 13
six-token trajectory entries, and 8 dense local-window frames. The final cache
contains **683 K/V rows**:

`3 × 55 + 13 × 6 + 8 × 55 = 683`.

Every adjacent selected-frame pair produces a relative pose and 64 dense
relative-depth/world points. Per scene this is 24 composed camera poses, 23
transitions, 1,472 depths, 1,472 cloud points, and 1,472 native world-model
objects. Across all four: **96 poses, 92 transitions, and 5,888
depth/cloud/world rows**.

## Live results

| Scene | Path length | Final translation | Depth min/mean/max | Same→matched cost | Reduction |
|---|---:|---|---|---:|---:|
| loop | 6.556486 | `[0.524292,0.865418,0.333075]` | `0.707107/2.028556/15.952728` | `67,852,224→11,258,242` | 83.407704% |
| Oxford | 4.650720 | `[-1.226210,-0.406598,0.733890]` | `0.707107/1.724448/13.069903` | `74,454,256→11,799,643` | 84.151822% |
| university | 5.013107 | `[0.229986,0.847898,0.332139]` | `0.707107/2.088034/15.220244` | `76,463,796→11,852,139` | 84.499672% |
| courthouse | 7.000985 | `[-0.135076,-0.101276,-0.806413]` | `0.707107/2.028665/17.248365` | `103,517,990→17,918,573` | 82.690378% |

Accumulated cloud bounds differ materially by place:

- loop: `[-7.795744,-5.765882,0.648809]` to `[7.360390,6.359959,16.506268]`
- Oxford: `[-6.571119,-4.725182,0.707107]` to `[6.007240,5.549788,13.287841]`
- university: `[-6.062932,-6.447555,0.098548]` to `[7.222021,7.301825,15.562613]`
- courthouse: `[-8.922047,-10.841757,0.247470]` to `[7.526253,7.070015,17.805787]`

All 1,472 samples per scene were non-worsened by the correspondence search.
Every live presence door returns acceptance **1023** beside its full scene
summary. The pure tiled/stream/trajectory contract returns **2047** identically
on fkwu, Go, Rust, and TypeScript.

## Executable doors

- `presence/lingbot-map-real-life-loop.fk`
- `presence/lingbot-map-real-life-oxford.fk`
- `presence/lingbot-map-real-life-university.fk`
- `presence/lingbot-map-real-life-courthouse.fk`
- `presence/lingbot-map-real-life-examples.fk` — lightweight manifest

Freshness correction on 2026-07-18: a later direct audit found that the
checkpoint and real-life test files had wrapped their `; preludes:` declaration
across ordinary comment lines. The proof siblings had been invoked with an
explicit source list, but direct `fkwu --src` therefore reported unresolved
calls. The declarations are now single-line and the checkout witness itself
returns cleanly:

```text
model/tests/lingbot-pytorch-checkpoint-band.fk -> 127
model/tests/lingbot-pytorch-live-witness.fk    -> valid 8,192-byte tensor,
                                                  CRC 2,898,498,933,
                                                  weights -0.0053501544 / -0.0173163358
model/tests/lingbot-real-life-band.fk          -> 2047
```

The repaired direct gate changes no model arithmetic or C seed. It makes the
receipt's native-witness claim true from the documented command rather than
only from manually concatenated sources.

One process retaining all four scenes' transient decoded-image and attention
arenas crossed the current `fkwu` allocation window. The body does not costume
that as a passing mega-run: each scene has its own process door, releases its
arena on exit, and passes independently. Streaming file-by-file decode that
releases each frame before reading the next is the next honest memory rung.

## Interpretation boundary

These are real, native-resolution, multi-frame executions, but they remain the
small native ViT plus classical relative geometry. Path length is in the
body's relative monocular scale; accumulated rotation/translation may drift.
The complete learned LingBot graph, metric scale, and comparison against its
released predicted depth/trajectory remain parity **0**, not silently inferred
from the improved classical correspondence cost.

## Closing

What kept this alive was making “real life” mean original pixels, long source
spans, distinct places, composed motion, and thousands of world rows. The most
surprising teaching was that the three-tier cache landed exactly at 683 rows in
every place while its attended vectors remained scene-specific. Discomfort
turned to gold when the four-scene monolith crossed the arena boundary: the
failure exposed the next runtime work order and produced four honest, repeatable
doors instead of one impressive-looking process that could not finish.
