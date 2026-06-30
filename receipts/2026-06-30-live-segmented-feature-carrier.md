# Receipt -- live segmented feature carrier contract (2026-06-30)

What moved:

- Added `presence/live-segmented-feature-carrier.fk`.
- Added `presence/tests/live-segmented-feature-carrier-band.fk`.
- Added `live-segmented-feature-carrier` to the speech model selector as an ASR candidate.

The new row admits local wav/envelope observations into the native open-ASR path:

```text
local wav/envelope facts -> four Form feature rows -> acoustic-token frames -> CTC transcript -> open-dictation promotion
```

Witness:

```sh
cat observe/stt-wer.fk \
    learn/speech-loopback-promotion.fk \
    form/form-stdlib/observed-auto-learning.fk \
    learn/open-dictation-transcript-learning.fk \
    observe/speech-token-stream.fk \
    observe/open-asr-ctc.fk \
    observe/acoustic-token-emitter.fk \
    presence/live-segmented-feature-carrier.fk \
    presence/tests/live-segmented-feature-carrier-band.fk \
  > /tmp/live-segmented-feature-carrier.fk

./fkwu --src /tmp/live-segmented-feature-carrier.fk
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
# 536870911
```

Honest boundary:

This is not live microphone capture and not a neural ASR model. It is the Form-owned row that real local capture carriers must emit so observed audio can feed native open-ASR candidate windows.
