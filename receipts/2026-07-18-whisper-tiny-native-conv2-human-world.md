# Released Whisper-tiny acoustic stem reads human speech in Form

Date: 2026-07-18

This movement extends the first complete released acoustic layer through
Whisper-tiny's second convolution. It executes released
`conv1 -> exact-erf GELU -> conv2 -> exact-erf GELU` in Form over CC0 human
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
5. evaluate four complete 384-channel conv1 tokens and exact-erf GELU;
6. evaluate one interior stride-2 conv2 token using all 442,368 weights and
   384 biases, then exact-erf GELU;
7. return all 384 learned channels.

The six-column window is intentionally bounded. It proves the released stem on
real audio without claiming Whisper's full 30-second padded frontend.

## Independent numerical witness

`verify-native-conv2-reference.mjs` independently parses the PCM and f32
tensors, derives the Slaney bank, performs direct complex DFTs, and evaluates
the two convolutions. It imports no Form output.

```text
melShape=[6,80]
peak=0.8258705179541589
conv1WeightShape=[384,80,3]
conv2WeightShape=[384,384,3]
learnedParameters=535296
conv1MultiplyAdds=368640
conv2MultiplyAdds=442368
fingerprint=[-0.13670759035634225,-0.000021216650380269864,
 -0.13907314009051322,-0.15989430626988155,-0.013989977078766876,
 -0.1633629394711227,0.3005172083509489,0.24343036918689595,
 24.186254929730826,70.81787381359106]
```

Form returned:

```text
fingerprint=[-0.13670759035322247,-0.000021216650378872467,
 -0.13907314006912047,-0.1598943062809822,-0.013989977076389038,
 -0.16336293944010025,0.30051720827619183,0.24343036938189985,
 24.186254931013945,70.817873814567292]
```

The pure Form comparison gate allows `2e-5`; the observed differences are far
smaller.

## Operational world path

The existing non-test acoustic door now consumes the full stem rather than
conv1 alone. Released learned activity is a required causal condition before
the unprompted host transcript may reach the complete 10,000-concept Form
detector and world model. Zeroing the learned result while retaining the same
content candidate closes the door.

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
511

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
