# Released visual pixels become an observed runtime event

; witnessed: 2026-07-20 -> real video, released patch projection, GCA,
;                              classical cloud, world, and framebuffer live;
;                              semantic and strict visual parity remain zero

## Gap closed

The released LingBot/DINO patch projection already affected the native world,
but its production door returned only model outputs. It did not say which
boundary was active, where the event came from, how long each boundary took,
or which boundary owned the runtime cost. The strict ledger therefore remained
right to hold `native-visual-weights` at `0/1`, but operators still lacked a
trace of the partial path that really exists.

`presence/lingbot-learned-map-observed-live.fk` now runs this exact Form path:

```text
pinned 518x294 video frames
  -> released DINOv2-L/14 patch projection
  -> operational witness-weight GCA with a zero-pixel ablation
  -> classical relative pose/depth and 64-point cloud
  -> 64 generic objects in the native world model
  -> one attributed runtime-event in that same world model
```

Form places BEGIN/END plus six stage events in the native framebuffer. Every
stage carries an exact snapshot delta for dispatches, nodes, strings, arena
cells, value-stack high-water, boxed floats, and I/O/sense operations. The
markers recover their source as
`presence/lingbot-learned-map-observed-live.fk:1:1`. There is no C growth and
no Bash, TypeScript, JavaScript, or Python model path. `ffmpeg` only decodes the
committed Matroska carrier to BMP; Form validates BMP/RGB, loads weights,
normalizes pixels, and owns all model and geometry arithmetic.

## Real source and released weights

The two frames come from the committed lossless 24-frame loop fixture sourced
from `Robbyant/lingbot-map` commit
`7ff6f3ed0913d4d326f8f13bbb429c4ffc0195c2`:

```text
model/fixtures/lingbot-map/real-life/loop-24f.mkv
1,581,923 bytes
SHA-256 05104f34e6cb1a88daa36e231eb96d29a91447add37e7199a252a546d9643073
decoded frames 518x294, 518x294
```

The learned carrier remains the released balanced LingBot-Map checkpoint at
pinned Hugging Face revision
`204754b72bb24f561f8d7e7e1e4e4cd9e809adf9`. Form validates and executes:

| Tensor | Parameters | Form-checked CRC-32 |
|---|---:|---:|
| DINO patch projection `[1024,3,14,14]` | 602,112 | 3,386,682,685 |
| patch bias `[1024]` | 1,024 | 2,077,575,075 |
| total released parameters | 603,136 | — |

## Live data

The directly executable production door returned:

```text
./fkwu --src presence/lingbot-learned-map-observed-live-run.fk

framebuffer-runtime-observation trace=lingbot-learned-map-real-video
duration-ms=7649 decode-ms=47 weights-ms=2980 learned-patch-ms=4575
gca-ms=23 geometry-ms=23 world-ms=1
dispatches=1563349255 boxed-floats=43832144 io-sense-dispatches=1189
learned-parameters=603136 retained-kv-rows=36 cloud-points=64
generic-world-objects=64 semantic-success=0 native-visual-weights=0
outcome=released-patch-to-world-observed-full-visual-parity-held
```

The live numeric payload also carries, rather than hiding in prose:

- center-token widths `[1024,1024]`;
- first-frame fingerprint sum `0.36816813101718826` and second-frame sum
  `-5.2784386886527592`;
- attended released-pixel witness
  `[0.0081609232671660432,-0.13472910751433817,
  -0.020358437528677702,0.051858087169651026]`;
- zero-input ablation L1 `0.21510655547983293`;
- 36 retained K/V rows across two frames;
- 64 relative depths, 64 cloud points, and geometric match cost
  `2,385,740 -> 284,311`;
- 64 generic world objects and one runtime-event world object.

Those fingerprints prove that the released coefficients responded to real
pixels and that their output causally affected attention/world state. They do
**not** identify what the scene means. No filename, fixture ID, caption, or
fingerprint becomes learned semantic success.

## Gate and strict floor

```text
./fkwu --src presence/tests/lingbot-learned-map-observed-live-band.fk -> 32767
```

The gate checks exact source dimensions and byte count, both released tensor
CRCs, two real learned fingerprints, ablation movement, GCA retention,
geometry/cloud/world counts, eight attributed events, whole-window resources,
per-stage resources, the learned-patch boxed-float bottleneck, and the runtime
event's world admission.

The strict truth returned as live data remains:

```text
released patch projection 1
DINO 24 transformer blocks 0
learned GCA 0
learned pose/depth 0
native visual weights 0
learned semantic success 0
```

The requirement ledger in
`observe/concept-10000-13-multimodal-completion.fk` is unchanged. Its
`native-visual-weights` row remains honestly incomplete until the complete
released DINO/GCA/camera/depth graph executes and the required semantic floor
is independently observed.

The movement stayed alive by turning the strongest real partial visual path
into an attributable event without inflating its meaning. The most surprising
teaching was that the trace made the released patch projection visible as the
boxed-float bottleneck while the GCA and geometry stages were tiny beside it.
Discomfort turned to gold when the tempting scene-specific fingerprint was
kept at semantic success zero: it became causal provenance instead of a fake
recognition result.
