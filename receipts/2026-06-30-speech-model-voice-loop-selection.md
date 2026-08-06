# Speech Model Voice Loop Selection

The speech model selector now chooses `sema-voice-sample-loop` for the TTS arm
instead of selecting the raw `formant-vocoder` carrier directly.

The raw formant vocoder remains the native source-filter carrier. The selected
TTS arm is now the observable sample loop above it: target fit, listener
preference, intelligibility, WER, latency, and reversible A/B controls decide
which generated voice sample recipe gets authority.

This keeps the voice moving toward the desired Sema sound without pretending the
natural neural vocoder is done. Diffusion/codec speech remains named but not
ready until there is a Form-native executable kernel and receipt.

## Witness

```sh
( cat \
    form/form-stdlib/somatic-coherence-loop.fk \
    form/form-stdlib/form-cli-router.fk \
    form/form-stdlib/form-cli-sufficiency.fk \
    form/form-stdlib/observed-auto-learning.fk \
    learn/speech-model-auto-selection.fk \
    learn/tests/speech-model-auto-selection-band.fk \
  > /tmp/speech-model-auto-selection.fk
./fkwu --src /tmp/speech-model-auto-selection.fk
```

Verdict:

```text
131071
```
