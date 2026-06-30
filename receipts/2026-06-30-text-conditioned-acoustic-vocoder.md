# Text-Conditioned Acoustic Vocoder

Date: 2026-06-30

The live Sema WER-100 miss named `text-conditioned-acoustic-vocoder` as the
next trainable target. This change makes that target executable in Form.

`learn/text-conditioned-acoustic-vocoder.fk` now performs:

```text
target tokens -> G2P phones -> voice-meta-conditioned source-filter frames -> local-oracle route receipt
```

The bridge preserves voice-side metadata (`confidence`, `warmth`, `cadence`,
`hesitation`, `excitement`, `attunement`) and can target the Sanskrit/Latin
baseline through neutral meaning rows.

## Witness

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    observe/speech-token-stream.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/text-conditioned-acoustic-vocoder.fk \
    learn/tests/text-conditioned-acoustic-vocoder-band.fk \
  > /tmp/text-conditioned-acoustic-vocoder.fk
./fkwu --src /tmp/text-conditioned-acoustic-vocoder.fk
```

Observed:

```text
32767
```

The speech selector now points at the concrete Form bridge kernel:

```sh
cat form/form-stdlib/somatic-coherence-loop.fk \
    form/form-stdlib/form-cli-router.fk \
    form/form-stdlib/form-cli-sufficiency.fk \
    form/form-stdlib/observed-auto-learning.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    learn/sema-voice-oracle-miss-learning.fk \
    learn/speech-model-auto-selection.fk \
    learn/tests/speech-model-auto-selection-band.fk \
  > /tmp/speech-model-auto-selection.fk
./fkwu --src /tmp/speech-model-auto-selection.fk
```

Observed:

```text
4194303
```

## Boundary

This does not claim natural neural TTS and does not change the live WER-100
formant result. It makes the missing bridge executable and measurable: a failed
sample remains `oracle-guide`; an exact local transcript can promote
`native-acoustic-vocoder`.
