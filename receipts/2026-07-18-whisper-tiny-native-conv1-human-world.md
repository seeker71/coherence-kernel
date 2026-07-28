# 2026-07-18 — complete Whisper-tiny conv1 reads human speech and gates the world

The previous acoustic floor had a 12-wide **slice** of a released Whisper
transformer block and a complete host Whisper recognizer.  Neither was a
complete learned acoustic layer operating on a real waveform in Form.  This
movement closes one entire released layer without claiming the recognizer.

## Released weight and recording provenance

| Artifact | Upstream identity | Shape / format | Bytes | SHA-256 | License |
|---|---|---:|---:|---|---|
| `encoder-conv1-weight.f32` | `openai/whisper-tiny` `model.safetensors`, revision `169d4a4341b33bc18d8881c4b69c2e104e1cc0af`, tensor `model.encoder.conv1.weight` | `[384,80,3]` f32 | 368,640 | `bb6642598e3efd8ea1fe81605f864342bb174604cba8dee5c23aa223fc126ecb` | Apache-2.0 (model card) |
| `encoder-conv1-bias.f32` | same pinned release, tensor `model.encoder.conv1.bias` | `[384]` f32 | 1,536 | `a8deb23b8cb5d0a88ffa398c9951ef92a3e47d44b32412dcb40b01895ec4772f` | Apache-2.0 |
| `lingua-libre-book-16k.wav` | Wikimedia Commons / Lingua Libre media `M92036254`, Simplificationalizer saying “book” | PCM s16le, 16 kHz, mono, 0.712 s | 22,828 | `1166acadc40e8d60baa82c6321ba3445fda5305a46539c3d1a0cc43e425de523` | CC0 |

The full source recording is independently pinned in
`presence/fixtures/concept-audio-human-13-source.tsv` as SHA-256
`b01e92bb0f8d48214c52630b8432d2e980cd6200c28d1363df49848fe4316614`.
The committed derivative removes metadata and uses ffmpeg's bit-exact flags, so
the PCM WAV has a canonical 44-byte data offset rather than carrying an ffmpeg
version string.  `fetch-native-conv1-fixtures.sh` re-fetches the exact pinned
safetensors ranges and public recording and refuses any hash drift.  It uses
`/usr/bin/curl`, the macOS system binary; no Python is involved.

The Hugging Face safetensors header is 19,104 bytes and names the exact spans:

- bias data offsets `[118209024,118210560]`, fetched from absolute inclusive
  range `118228136-118229671`;
- weight data offsets `[118210560,118579200]`, fetched from absolute inclusive
  range `118229672-118598311`.

## What executes in Form

`model/whisper-tiny-native-conv1.fk` performs this entire path:

1. read the real PCM bytes and locate the WAV `data` chunk;
2. decode signed 16-bit samples in Form;
3. form three 400-sample, 160-hop periodic-Hann frames inside the voiced region;
4. compute all 80 Slaney log-mel bands using the native Goertzel/DFT cell;
5. apply Whisper's 8-log-unit clamp and `(x+4)/4` normalization over the tile;
6. CRC-check and decode all 92,160 released conv weights and 384 biases;
7. perform 92,160 learned multiply-adds and return all 384 output channels.

The Form front end evaluates 391 supported mel bins per frame (1,173 direct
Goertzel bin evaluations, 469,200 sample recurrences) before the convolution.
The result is not a coefficient sample: every parameter of the first learned
acoustic layer participates.

## Independent numerical comparison

`verify-native-conv1-reference.mjs` shares neither Form evaluator code nor the
committed mel table.  It generates the Slaney bank from the published formula,
uses direct complex DFTs, parses raw f32 tensors with Node, and evaluates the
full Conv1d independently.

```text
implementation=independent-node-direct-dft-slaney-conv1d
source=Lingua-Libre-M92036254-human-book
dataOffset=44
melShape=[3,80]
peak=0.8258705179541589
weightShape=[384,80,3]
biasShape=[384]
learnedParameters=92544
fingerprint=[1.479262509249532,-1.676065684405518,0.07197640114503567,
 -1.9385381190770288,0.6139602734565989,-0.10746001130249724,
 -0.2412528915643827,0.6832136439554694,-88.59371480237915,
 263.3570208697341]
```

Form returned:

```text
peak=0.82587051795304589
output-width=384
fingerprint=[1.4792625093014566,-1.6760656843175294,0.071976401177815758,
 -1.9385381191435229,0.61396027326417002,-0.10746001127246343,
 -0.24125289179039494,0.6832136432710435,-88.593714798911023,
 263.35702087227764]
```

Maximum absolute fingerprint difference is
`3.4681306715356186e-9`; the live band holds it below `1e-8`.

## Operational, causal world path

`presence/whisper-tiny-native-acoustic-live.fk` is the non-test door.  A shell
carrier copies the hash-verified recording to a neutral numeric path.  Form
runs the native layer first.  Only when its complete output has learned
activity does unprompted host Whisper transcribe the same human waveform.  The
complete 10,000-concept Form detector scans that transcript, and only its
returned id may enter `cwm-persist`.

Live output:

```text
native-acoustic-live human=1 tts=0 released-parameters=92544 output-width=384 learned-activity=1 candidate-count=1 detected-id=571 detected-surface=book world-admitted=1 zero-ablation-admitted=0 native-recognizer=0
```

The zero-output ablation retains the same detected candidate but is refused by
the admission door.  The released Form-computed acoustic output therefore has
a causal effect; it is not receipt decoration.

## Gates

```text
./fkwu --src model/tests/whisper-tiny-native-conv1-band.fk
1023

./fkwu --src presence/tests/whisper-tiny-native-acoustic-live-band.fk
# gate copies the hash-pinned committed WAV to an exact neutral /tmp path
511

node model/fixtures/whisper-tiny/verify-native-conv1-reference.mjs
# learnedParameters 92544; numerical values above

presence/carriers/whisper-tiny-native-acoustic-live.sh
# readable live line above, followed by the full evidence row
```

Required checkout grounding remained `42`, `55`, `15`,
`[1, 2.5, [3, 4]]`, and `11111`.  `runtime/fkwu-uni.c` did not change.

## Honest remaining floor

The strict `native-acoustic-weights` requirement remains **0**.  One complete
learned layer is materially beyond a sliced-weight demonstration, but a native
recognizer still requires Whisper-tiny conv2, positional embeddings, four
encoder blocks, decoder embeddings and four decoder blocks, tokenizer, and
search.  Host `whisper.cpp-large-v3-turbo` still owns transcript generation in
the operational path.  No claim here promotes that rented decoder to native.

The surprising teaching was how small the numerical seam became once both
implementations met on the actual released f32 carrier: direct DFT versus
Goertzel still agreed to billionths through 92,160 learned multiply-adds.  The
discomfort was that a complete first layer still cannot honestly name “book.”
That discomfort became gold when the layer was made causal without pretending
it was semantic: real learned activity opens the world door, and its ablation
closes it, while the host decoder remains visibly named.
