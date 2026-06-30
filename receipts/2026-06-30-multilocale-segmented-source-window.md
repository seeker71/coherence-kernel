# Receipt -- multilocale segmented source-ASR window (2026-06-30)

What moved:

- Added `learn/multilocale-segmented-source-window.fk`.
- Added `learn/tests/multilocale-segmented-source-window-band.fk`.
- Reused the existing consentful Sanskrit baseline, acoustic token emitter, CTC decoder, and segmented acoustic learner.
- Added the new ASR candidate to `learn/speech-model-auto-selection.fk`.

The new receipt proves a six-sample source-ASR window over three reciprocal pairs:

- `sa<->la`
- `en<->zh`
- `ar<->en`

The window starts with untrained acoustic prototypes at native score `0`, learns from local-oracle transcript rows, then reaches native score `6/6`, native rate `100`, and ready pairs `3/3`. The route shifts from `oracle-guide` to `native-multilocale-segmented-source` only after reciprocal pair coverage and the WER floor pass.

Witness:

```sh
cat observe/stt-wer.fk \
    observe/speech-token-stream.fk \
    observe/open-asr-ctc.fk \
    observe/acoustic-token-emitter.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/segmented-acoustic-token-learning.fk \
    learn/multilocale-segmented-source-window.fk \
    learn/tests/multilocale-segmented-source-window-band.fk \
  > /tmp/multilocale-segmented-source-window.fk

./fkwu --src /tmp/multilocale-segmented-source-window.fk
# 32767
```

Selector witness:

```sh
cat form/form-stdlib/somatic-coherence-loop.fk \
    form/form-stdlib/form-cli-router.fk \
    form/form-stdlib/form-cli-sufficiency.fk \
    form/form-stdlib/observed-auto-learning.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    learn/sema-voice-oracle-miss-learning.fk \
    learn/live-open-asr-source-authority.fk \
    learn/speech-model-auto-selection.fk \
    learn/tests/speech-model-auto-selection-band.fk \
  > /tmp/speech-model-auto-selection.fk

./fkwu --src /tmp/speech-model-auto-selection.fk
# 134217727
```

Honest boundary:

This is not live microphone streaming and not a neural acoustic encoder. It is a Form-native, local-oracle-guided, multi-locale source-ASR training window that later live open-ASR misses can feed.
