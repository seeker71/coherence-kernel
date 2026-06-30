# Audio2Audio Sema Voice Loop Selection

The speech model selector now chooses `prototype-asr-sema-voice-audio2audio`
for the audio-to-audio arm. The route is:

```text
audio A -> prototype ASR -> neutral Form meaning -> Sema voice sample loop -> audio B target
```

The previous raw `prototype-asr-formant-audio2audio` route remains present as
the deterministic carrier path underneath. The selected route now inherits
Sema's target-fit, listener preference, intelligibility, WER, latency, and
reversible A/B sample loop before generated target speech gets authority.

This is still not a natural neural vocoder. It is the current native
audio-to-audio model choice over the closed-set/local-oracle route, with
diffusion/codec speech still pending a Form-native executable kernel and
receipt.

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
262143
```
