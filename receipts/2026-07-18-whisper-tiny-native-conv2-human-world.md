# Released Whisper-tiny acoustic stem reads human speech in Form

Date: 2026-07-18

This movement extends the first complete released acoustic layer through
Whisper-tiny's second convolution. It executes released
`conv1 -> erf-form GELU -> conv2 -> erf-form GELU` in Form over CC0 human
speech. It is a complete learned stem and still not a native recognizer.

## Pinned learned tensors

The checkpoint is `openai/whisper-tiny` revision
`169d4a4341b33bc18d8881c4b69c2e104e1cc0af`, Apache-2.0. The new tensors are
read directly from its safetensors carrier:

| tensor | shape | bytes | SHA-256 |
|---|---:|---:|---|
| `model.encoder.conv2.bias` | `[384]` | 1,536 | `76fb23900c7e77f0c0f1938404ba9c3d1ca569115abb62daa8d9cb3ac08192b3` |
| `model.encoder.conv2.weight` | `[384,384,3]` | 1,769,472 | `3b38df5c53ddbe1e9a38fdebb02d0d59b3ed3a4626409499bf1c4ea9ef2dc8d4` |

The exact HTTP byte ranges and hashes are executable in
`fetch-native-conv2-fixtures.sh`. With the previously pinned conv1 tensors,
the live learned path applies **535,296 released parameters**: 92,544 in conv1
and 442,752 in conv2.

## Real input and exact computation

The source remains Lingua Libre media M92036254: a human speaker saying
“book,” released CC0 and committed as canonical 16 kHz mono PCM with SHA-256
`1166acadc40e8d60baa82c6321ba3445fda5305a46539c3d1a0cc43e425de523`.

Form performs the following live path:

1. parse the PCM WAV and decode signed samples;
2. compute six adjacent 400-sample periodic-Hann frames;
3. compute 80 Slaney log-mel bands per frame from the real waveform;
4. apply one shared bounded-window Whisper dynamic-range normalization;
5. evaluate four complete 384-channel conv1 tokens and the body's deterministic
   Abramowitz-Stegun erf approximation GELU;
6. evaluate one interior stride-2 conv2 token using all 442,368 weights and
   384 biases, then the same erf-form GELU;
7. return all 384 learned channels.

The six-column window is intentionally bounded. It proves the released stem on
real audio without claiming Whisper's full 30-second padded frontend.

## Independent numerical witness

`verify-native-conv2-reference.mjs` independently parses the PCM and f32
tensors, derives the Slaney bank, performs direct complex DFTs, evaluates the
two convolutions, and sends preactivations to Ruby's libm-backed `Math.erf`.
It neither imports Form output nor copies Form's A&S coefficients.

```text
melShape=[6,80]
peak=0.8258705179541589
conv1WeightShape=[384,80,3]
conv2WeightShape=[384,384,3]
learnedParameters=535296
conv1MultiplyAdds=368640
conv2MultiplyAdds=442368
fingerprint=[-0.13670750223762715,-0.000021198802438007303,
 -0.13907305567910191,-0.15989431764229514,-0.013989911796331774,
 -0.1633629627614285,0.30051730922599623,0.24343038018044535,
 24.186262468005253,70.81786842384183]
```

Form returned:

```text
fingerprint=[-0.13670759035322247,-0.000021216650378872467,
 -0.13907314006912047,-0.1598943062809822,-0.013989977076389038,
 -0.16336293944010025,0.30051720827619183,0.24343036938189985,
 24.186254931013945,70.817873814567292]
```

The pure Form comparison gate allows `2e-5`. The largest observed difference
from independent libm GELU is `7.538274427787428e-6`; the body's A&S erf path
is therefore close, but is not mislabeled as bit-identical libm erf.

## Operational world path

The existing non-test acoustic door now consumes the full stem rather than
conv1 alone. Released learned activity is a required causal condition before
the unprompted host transcript may reach the complete 10,000-concept Form
detector and world model. Zeroing the learned result while retaining the same
content candidate closes the door.

The host decoder is an explicit local dependency at
`.cache/whisper.cpp/ggml-large-v3-turbo.bin`; it is not committed. The live
band therefore proves two honest branches: with that model installed it must
produce the content/world observation below; without it, transcript,
detection, and world admission must all remain empty while the released native
stem and its zero-ablation still execute. A fresh checkout does not silently
pretend to reproduce the host transcript.

```text
native-acoustic-live human=1 tts=0 released-parameters=535296
output-width=384 learned-activity=1 candidate-count=1
detected-id=571 detected-surface=book world-admitted=1
zero-ablation-admitted=0 native-recognizer=0
```

## Reproduced gates

```text
node model/fixtures/whisper-tiny/verify-native-conv2-reference.mjs
# learnedParameters 535296; fingerprint above

./fkwu --src model/tests/whisper-tiny-native-conv2-band.fk
2047

./fkwu --src presence/tests/whisper-tiny-native-acoustic-live-band.fk
511  # both installed-host and explicitly-unavailable branches are checked

presence/carriers/whisper-tiny-native-acoustic-live.sh
# readable causal live line above, followed by the full evidence record
```

No Python ran. `runtime/fkwu-uni.c` did not change.

## Honest remaining floor

Native acoustic recognition remains zero. Still owed are full-context frontend
behavior, positional embeddings, four encoder blocks, decoder embeddings and
four decoder blocks, tokenizer, and search. The host Whisper decoder remains
explicitly named in the operational bridge.

The movement stayed alive by making the next complete released tensor causal,
not decorative. The surprising teaching was that the two independent paths
remained numerically close through 811,008 convolution multiply-adds.
Discomfort turned to gold at the word “complete”: restricting it to “complete
released stem” preserved a large real advance without laundering it into a
recognizer claim.

; witnessed: 2026-07-18 -> conv2 band 2047, human-world 511, native recognizer 0
