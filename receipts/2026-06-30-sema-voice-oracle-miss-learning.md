# Sema Voice Oracle Miss Learning

Date: 2026-06-30

The live Sema formant probe gave a real local Metal result:

```text
verdict: 479
field_code: 110100002
oracle_wer: 100
route: oracle-guide
```

This change makes that miss executable. `learn/sema-voice-oracle-miss-learning.fk`
turns the WER-100 result into an AutoML action:

```text
train-text-conditioned-acoustic-vocoder
```

The selected next candidate is:

```text
text-conditioned-acoustic-vocoder
```

The recipe is explicit: `g2p`, `phoneme-timing`, `prosody-contour`,
`acoustic-token-emitter`, `segmented-acoustic-learning`, the Sema voice sample
loop, and the same local-oracle WER bar.

## Witness

```sh
cat learn/sema-voice-local-oracle-receipt.fk \
    learn/sema-voice-oracle-miss-learning.fk \
    learn/tests/sema-voice-oracle-miss-learning-band.fk \
  > /tmp/sema-voice-oracle-miss-learning.fk
./fkwu --src /tmp/sema-voice-oracle-miss-learning.fk
```

Observed:

```text
32767
```

The speech selector now includes the live formant probe and the acoustic-vocoder
candidate, and exposes the voice miss action in its receipt.

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
2097151
```

## Boundary

This is not a passing voice sample. It is the algorithm changing because of live
observation: the current formant route stays `oracle-guide`, and the next
trainable native target is a text-conditioned acoustic/vocoder bridge.
