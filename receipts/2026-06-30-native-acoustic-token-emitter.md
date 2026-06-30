# Native Acoustic Token Emitter Receipt

Date: 2026-06-30

The token stream and CTC decoder now have a native acoustic bridge. The new
cell is intentionally small: it learns oracle-aligned acoustic token prototypes
and emits CTC frame tokens by integer L1 distance plus earned confidence.

## What changed

- Added `observe/acoustic-token-emitter.fk`.
- Added `observe/tests/acoustic-token-emitter-band.fk`.
- Added `native-acoustic-token-emitter` to `learn/speech-model-auto-selection.fk`.
- Updated speech substrate docs and the voice roadmap.

## Witness

```sh
cat observe/wav-sense.fk \
    observe/stt-wer.fk \
    learn/speech-loopback-promotion.fk \
    form/form-stdlib/observed-auto-learning.fk \
    learn/open-dictation-transcript-learning.fk \
    observe/speech-token-stream.fk \
    observe/open-asr-ctc.fk \
    observe/acoustic-token-emitter.fk \
    observe/tests/acoustic-token-emitter-band.fk \
  > /tmp/acoustic-token-emitter.fk
./fkwu --src /tmp/acoustic-token-emitter.fk
```

Observed:

```text
32767
```

Speech model selector:

```text
32767
```

Live macOS open-dictation boundary check:

```text
modc-run            -> 511
modc-run-field-code -> 440000100
```

## Boundary

This is not a finished open-ASR model and not a neural acoustic encoder. It is a
native supervised frame-token emitter: consentful side-channel truth or a local
oracle can teach token prototypes, and the body can later emit blank/nonblank
CTC frames into `open-asr-ctc`. Live mic audio still needs a segmented feature
row carrier or native acoustic encoder before this can beat the selected
closed-prompt ASR route.
