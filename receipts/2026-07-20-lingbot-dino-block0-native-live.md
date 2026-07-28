# A complete released DINO block corrects the visual numeric path

; witnessed: 2026-07-20 -> complete DINOv2-L/14 block 0 over two real pixel
;                              tokens; false route semantic rejected

## Concrete defect corrected

The real learned visual path stopped after DINOv2-L/14's released patch
projection. Its actual numeric outputs for the first two 518×294 loop frames
were full 1,024-vectors with sums:

```text
frame 0  0.36816813101718826
frame 1 -5.2784386886527592
```

Those vectors were real learned pixel responses, but they had traversed zero
DINO transformer blocks. Treating a route word such as `loop` as a semantic
result at that boundary would be false. This movement corrects both defects at
the strongest bounded rung available:

1. every released tensor of DINO block 0 now executes in Form;
2. the concrete route-name candidate `loop` is transformed to empty semantic
   output, admission `0`, and false-candidate-rejected `1` until an actual
   learned semantic head exists.

This is a correction, not merely a log of the false output.

## Released carrier

The exact checkpoint carrier comes from `robbyant/lingbot-map` at pinned
Hugging Face revision
`204754b72bb24f561f8d7e7e1e4e4cd9e809adf9`:

```text
model/fixtures/lingbot-map/checkpoint-dino-block0-records.bin
checkpoint bytes 8,367,248..58,762,127 inclusive
50,394,880 bytes
SHA-256 e011139a4274ccbd2bd0c9035ea13854fac9dc61c7df554d1dfd84adf7ad8f57
```

It contains complete PyTorch records `checkpoint/data/9..22`: norm1, combined
QKV, attention projection, layer-scale 1, norm2, 4,096-wide MLP, and
layer-scale 2. The tensor payload is **12,598,272 released f32 parameters**.
`model/fixtures/lingbot-map/DINO-BLOCK0-PROVENANCE.md` records all fourteen
shapes, sizes, and central-directory CRCs.

Form reads every local header, checks its exact member name and stored method,
reads the payload at byte `112`, and checks its size and CRC. The first live
attempt exposed a precise provenance bug: the record overhead is not a
128-byte prefix. It is a 112-byte local-header/alignment prefix followed by the
payload and a 16-byte data descriptor. Correcting that boundary changed all
fourteen validations from `0` to `1`; no check was bypassed.

## Complete block arithmetic

Two consecutive public-video center patches first pass through the released
`[1024,3,14,14]` projection. The resulting two-token sequence then executes:

```text
LayerNorm eps=1e-6
  -> combined QKV [3072,1024] + bias
  -> 16 heads × 64 dimensions, scale 0.125
  -> attention projection [1024,1024] + bias
  -> released layer-scale gamma + residual
  -> LayerNorm eps=1e-6
  -> FC1 [4096,1024] + exact-erf GELU
  -> FC2 [1024,4096]
  -> released layer-scale gamma + residual
```

Every tensor load and every affine, normalization, attention, GELU,
layer-scale, residual, fingerprint, and gate is Form running on `fkwu`. Curl
was byte transport only. There is no Python, TypeScript, JavaScript, Bash, or C
model path, and `runtime/fkwu-uni.c` did not change.

## Exact real output

The two observed attention rows are:

```text
query 0, head 0  [0.53690760630367362, 0.46309239369632638]
query 1, head 15 [0.5120957920332494,  0.48790420796675055]
```

Post-attention fingerprints contain coordinates `0,1,2,3,255,511,767,1023`,
sum, and absolute sum:

```text
token 0 [0.12255551927412393,-0.10669139291045768,
 -0.055812927438498305,0.091193482031031434,0.018341035129757505,
 0.025276888615324181,-0.044892847438950535,0.0049739995174702747,
 -3.4311777388151397,107.32023087568793]

token 1 [0.01077746887279396,-0.12949448396619923,
 -0.032843625857086022,0.028935065146601657,-0.024299892436278225,
 0.071728231185979055,0.049591284750486159,0.0097159134981399976,
 -9.0795536942292667,101.12561191566033]
```

Complete block-0 output fingerprints are:

```text
token 0 [0.14120925650035721,0.076855295871547213,
 -0.11964356306490122,0.095237437255115465,0.016558179607194402,
 0.012688304578971415,-0.046362200544208421,0.0050710443008849439,
 -1.7748674081279805,180.74734975841341]

token 1 [0.067715456170171578,0.083597632546084738,
 -0.10994540297839728,0.030571647786290783,-0.026402101153496622,
 0.058960153415180135,0.049205890544719438,0.0091881310225821779,
 -8.535799304268366,183.81337185811444]
```

Input-to-output L1 movement is `134.70614667005006` and
`144.0745122077175`; output-token separation is `60.13735514085694`.
These are numeric learned-effect witnesses, not semantic identifiers.

## Framebuffer and acceptance

The production door emits BEGIN, six observed STAGE events, and END around
decode, patch projection, fourteen-tensor validation, attention, MLP, and the
semantic rejection gate. One cold live run reported:

```text
duration-ms=172911
decode-ms=121 patch-ms=8324 carrier-ms=50383
attention-ms=24253 mlp-ms=89826
dispatches=17819905990
released-block-parameters=12598272 carrier-bytes=50394880
output-tokens=2 semantic-output-empty=1 semantic-admitted=0
false-fixture-label-rejected=1
outcome=complete-released-block0-semantic-output-held
```

The exact acceptance gate is green:

```text
./fkwu --src presence/tests/lingbot-dino-block0-observed-live-band.fk
32767
```

It checks source dimensions, released patch inputs, all block carrier facts,
exact attention and output values, learned movement, the active `loop -> ""`
semantic correction, eight framebuffer events, per-stage resources, and the
MLP bottleneck.

## Honest remaining floor

Block 0 is complete, but complete DINO remains false. The special/class/
register/position tokens, full spatial patch sequence, blocks 1–23, final norm,
learned GCA, learned camera/depth heads, and an independently validated
semantic head remain owed. The central strict ledger is unchanged. No block
fingerprint can satisfy semantic success.

The movement stayed alive by fixing the first missing learned transform and
the concrete false `loop` output in the same bounded run. The most surprising
teaching was that one released DINO block is already 12.6 million parameters,
yet its full two-token graph fits inside an observable three-minute Form
window. Discomfort turned to gold when all CRCs first stayed zero: the failure
revealed the exact 112-byte-prefix plus 16-byte-descriptor ZIP anatomy, and the
correction made every record—not a bypass—validate.
