# Native Audio2Audio Acoustic Bridge

Date: 2026-06-30

This change composes the decoded source-audio side with the new
text-conditioned acoustic vocoder. The new Form cell consumes decoded source
tokens plus a neutral meaning, renders target-locale tokens as acoustic frames,
and gates authority with local oracle transcripts on both the source and target
side.

## Witness

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    observe/speech-token-stream.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/text-conditioned-acoustic-vocoder.fk \
    learn/native-audio2audio-acoustic-bridge.fk \
    learn/tests/native-audio2audio-acoustic-bridge-band.fk \
  > /tmp/native-audio2audio-acoustic-bridge.fk
./fkwu --src /tmp/native-audio2audio-acoustic-bridge.fk
```

Observed:

```text
32767
```

The speech selector now sees the concrete audio2audio bridge candidate:

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
8388607
```

## Boundary

The source side is a decoded-token row from upstream ASR/segmentation, not live
open microphone authority. The bridge makes the audio2audio target side
executable and reciprocal: `sa->la` and `la->sa` both have to pass before the
window routes `native-audio2audio-acoustic-vocoder`.
