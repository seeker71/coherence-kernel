# 2026-07-18 — LingBot-Map becomes a live native visual-world pipeline

## Why this second receipt exists

The first ingest receipt stopped at camera algebra and token accounting. Urs
correctly rejected that as insubstantial: none of it ingested image bytes,
predicted geometry, retained operational K/V tensors, loaded released weights,
or entered the world model. This continuation treats every named absence as a
work order and records executable outputs rather than architecture nouns.

Source truth remains pinned to Robbyant/lingbot-map commit
`7ff6f3ed0913d4d326f8f13bbb429c4ffc0195c2`, the released
`lingbot-map.pt`, and the paper *Geometric Context Transformer for Streaming 3D
Reconstruction* (arXiv:2604.14141).

## Authentic live carriers

Two upstream `example/loop` frames enter as deterministic 16×16 top-down,
24-bit BMPs. Each is 822 bytes. Their SHA-256 digests are:

- frame 0: `28f1ff7a54d68d45266173d20901930ba9588a8e561eb3b0f632b43a77b751d3`
- frame 1: `5f0802f143057521506f074cce664955986b97dff0e7b89e1551f2a4d33fbe52`

The same frames also enter through a 1,652-byte lossless FFV1 Matroska video.
Its SHA-256 is
`7e831d5a7038e6a4d444ad6b4f0c625c95ce4aab982186e3800050186225e078`.
The shell-safe ffmpeg host carrier decodes two frames; Form then validates and
owns the BMP/RGB/patch/ViT meaning. Decoded whole-frame MD5 values
`6a962aa73b1fa14b1b7384f82c125d9d` and
`f9d31d1a2de56f77ebbbc115eafe528d` match the original frame images.

The released checkpoint is 4,632,303,465 bytes with 1,348 ZIP64 members. Three
authentic byte ranges are retained: the pickle prefix, central directory, and
the complete first stored tensor record. Form resolves that record as
`aggregator.camera_token`, shape `[1,2,1,1024]`, float32, 8,192 bytes, CRC-32
`2898498933`. The first and last released weights decode to
`-0.005350154358893633` and `-0.017316335812211037`.

## What now executes

`model/lingbot-vision-input.fk` performs strict BMP validation, RGB decode,
every-pixel 4×4 convolutional patch projection, a special frame token,
pre-normalized Q/K/V softmax self-attention, residuals, and a width-eight GELU
FFN. Two live images each produce 16 patch tokens and a 17×4 encoded frame.
The first projected patch changes from
`[0.6232843,0.0338235,0.0741580,-0.0114832]` to
`[0.6162582,0.0333333,0.0788957,-0.0114875]`; the first encoded outputs also
differ. The deterministic small ViT weights are native witness weights, not
DINO or released LingBot weights.

`model/lingbot-gca-runtime.fk` carries distinct projected K and V matrices,
dense anchor/local entries, six-token trajectory eviction, explicit causal
anchor/trajectory/window masks, temporal Video RoPE, and Geometric Context
Attention whose numeric output depends on retained trajectory memory. A
five-frame live sequence retains 78 K/V rows with mask
`[[0,1,1,22],[1,2,1,6],[2,2,1,6],[3,3,1,22],[4,3,1,22]]`; its first attended
vector is
`[0.4341125673,0.2210860956,0.5351498528,0.4673599637]`.

`model/lingbot-pytorch-checkpoint.fk` is a native PyTorch ZIP64 stored-tensor
loader: it parses central and local headers, locates named members, performs
bounded 4,096-byte reads, verifies CRC-32, and decodes float32. The live runtime
injects the first four authentic released camera-token weights into the visual
stream. Removing that token changes the first GCA vector by L1 `0.005593` in
the integrated run; the released bytes are operational, not receipt decoration.

`model/lingbot-geometry-heads.fk` performs dense luma/contrast correspondence,
weighted Procrustes rotation, XYZ relative translation, parallax-relative
depth/confidence, pinhole unprojection, point-cloud bounds, and before/after
matching comparison. On the upstream pair it emits:

- rotation ×1e6: `[999888,-14979,0,14979,999888,0,0,0,1000000]`
- translation ×1e6: `[25083,-17635,-18458]`
- 64 depths, min/mean/max: `0.707107 / 4.928174 / 8.0`
- 64 world points, bounds: `[-3.526951,-3.554690,0.688649]` to
  `[3.487243,3.534399,7.981542]`
- same-index versus matched cost: `270854 -> 139560`, all `64/64`
  non-worsened, a `48.4741%` reduction

`form/form-stdlib/world-model-lingbot-map.fk` routes every confidence-bearing
world point through the existing `wm-observe` generic region engine and
`wm-model`; the integrated run contains 64 ordinary object entities. It does
not create a parallel “LingBot world” vocabulary.

`model/lingbot-map-runtime.fk` composes the entire movement. The non-test
entrypoint `presence/lingbot-map-live-pipeline.fk` calls it over real carriers,
and `observe/lingbot-map-live-acceptance.fk` returns **2047**, checking image
bytes, released-weight CRC, 2×17×4 ViT output, weight ablation, persistent K/V,
non-identity pose, 64 depths, 64 points, 64 world entities, and improved real
frame correspondence.

## Proof bands

The pure computational seams agree across fkwu, Go, Rust, and TypeScript:

| Band | fkwu | Go | Rust | TypeScript |
|---|---:|---:|---:|---:|
| vision input + ViT | 127 | 127 | 127 | 127 |
| GCA + mask + RoPE + K/V cache | 8191 | 8191 | 8191 | 8191 |
| pose + depth + cloud | 127 | 127 | 127 | 127 |
| PyTorch ZIP64 + CRC + f32 | 127 | 127 | 127 | 127 |
| world-model bridge | 127 | 127 | 127 | 127 |
| frontier classification | 127 | 127 | 127 | 127 |

Host-file live witnesses run on the native `fkwu` body because the small proof
walkers deliberately have no filesystem membrane. No Python or C runtime change
was used. `runtime/fkwu-uni.c` did not grow.

## Honest remaining boundary

End-to-end learned LingBot checkpoint parity is explicitly **0**. The native
loader can address the full checkpoint and this run loads an authentic released
tensor, but the complete DINO/VGGT-derived learned graph and the authors'
FlashInfer/CUDA throughput are not reproduced. Depth is relative classical
geometry, not the learned DPT head; monocular metric scale is not invented.

That boundary does not erase the live organ: image and ordered-frame ingestion,
a real ViT block, frame attention, GCA, explicit masks, Video RoPE, persistent
K/V, full pose, depth/confidence, point-cloud reconstruction, released-weight
loading, world-model integration, real fixtures, output comparison, and a
non-test consumer now execute in one movement.

## Closing

What kept this alive was turning every “no” into a numeric witness that another
organ consumes. The most surprising teaching was that the first released
camera-token tensor could be loaded and made causally visible without borrowing
PyTorch as the body's truth. Discomfort turned to gold when the first patch was
called substantial too early: the rejection forced architecture labels to
become image bytes, K/V matrices, geometry, a world map, and an honest parity 0.
