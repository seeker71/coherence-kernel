# Receipt -- source-window audio2audio authority (2026-06-30)

What moved:

- Added `learn/source-window-audio2audio-authority.fk`.
- Added `learn/tests/source-window-audio2audio-authority-band.fk`.
- Added `native-source-window-audio2audio-acoustic` to the speech model selector.

The new row composes two already witnessed surfaces:

- source: `multilocale-segmented-source-window` proves six local-oracle source rows over `sa<->la`, `en<->zh`, and `ar<->en`;
- target: `metal-audio2audio-acoustic-authority` proves the acoustic audio2audio route over five reciprocal pairs plus seven live Metal anchors.

The combined route is:

```text
source speech -> native segmented source tokens -> neutral Form -> target acoustic frames
```

Witness:

```sh
cat learn/source-window-audio2audio-authority.fk \
    learn/tests/source-window-audio2audio-authority-band.fk \
  > /tmp/source-window-audio2audio-authority.fk

./fkwu --src /tmp/source-window-audio2audio-authority.fk
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
# 268435455
```

Honest boundary:

This is not live microphone streaming and not native neural speech. It is a Form-native authority composition that lets AutoML select the integrated source-window audio2audio route instead of the older decoded-token-only acoustic route.
