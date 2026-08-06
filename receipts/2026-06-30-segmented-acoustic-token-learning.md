# Segmented Acoustic Token Learning Receipt

Date: 2026-06-30

The native acoustic-token path now has a supervised segmentation learner. The
body can segment wav/envelope features into token rows, learn token prototypes
from consentful local-oracle transcripts, decode source speech tokens through
CTC, and render target-locale tokens through the Sanskrit baseline's neutral
meaning rows.

## What changed

- Added `learn/segmented-acoustic-token-learning.fk`.
- Added `learn/tests/segmented-acoustic-token-learning-band.fk`.
- Added `native-segmented-acoustic-learning` to `learn/speech-model-auto-selection.fk`.
- Updated speech substrate docs, roadmap, and manifest.

## Witnesses

Segmented acoustic learning:

```sh
cat observe/stt-wer.fk \
    observe/speech-token-stream.fk \
    observe/open-asr-ctc.fk \
    observe/acoustic-token-emitter.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/segmented-acoustic-token-learning.fk \
    learn/tests/segmented-acoustic-token-learning-band.fk \
  > /tmp/segmented-acoustic-token-learning.fk
./fkwu --src /tmp/segmented-acoustic-token-learning.fk
```

Observed:

```text
32767
```

Speech model selector:

```text
65535
```

## Boundary

This is still not a neural ASR model. It is a native supervised learner over
segmented local-oracle samples. Live mic streaming, stronger acoustic features,
and native neural ASR/TTS remain pending, but the next carrier no longer has to
invent the learning contract: it can hand local wav/audio facts to Form, and
Form owns segmentation, token prototypes, CTC decoding, neutral meaning, target
tokens, and route receipts.
