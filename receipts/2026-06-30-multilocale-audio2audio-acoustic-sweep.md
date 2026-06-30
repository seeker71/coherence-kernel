# Multilocale Audio2Audio Acoustic Sweep

Date: 2026-06-30

The native audio2audio acoustic bridge now runs over five reciprocal baseline
pairs instead of only the focused `sa<->la` proof:

```text
en<->de
en<->es
zh<->ar
fr<->id
sa<->la
```

Each row consumes decoded source-audio tokens, routes through the neutral
baseline meaning, renders target-locale acoustic frames through
`text-conditioned-acoustic-vocoder`, and requires local-oracle transcript
agreement on both source and target sides.

## Witness

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    observe/speech-token-stream.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/text-conditioned-acoustic-vocoder.fk \
    learn/native-audio2audio-acoustic-bridge.fk \
    learn/multilocale-audio2audio-acoustic-sweep.fk \
    learn/tests/multilocale-audio2audio-acoustic-sweep-band.fk \
  > /tmp/multilocale-audio2audio-acoustic-sweep.fk
./fkwu --src /tmp/multilocale-audio2audio-acoustic-sweep.fk
```

Observed:

```text
32767
```

The speech selector now sees the sweep-backed audio2audio candidate:

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
16777215
```

## Boundary

The sweep does not claim live open microphone authority. It proves that decoded
source-token rows from the ASR/segmentation side can now reach target acoustic
frames across diverse locale pairs, including Chinese and Arabic script tokens.
