# 2026-06-30 -- native vocoder + closed-set ASR loopback

## Ground

The repo was grounded before the change:

```sh
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
11111
```

## What Changed

Added the first native speech loopback cells:

- `presence/formant-vocoder.fk` -- source-filter/formant integer waveform from phoneme frames.
- `observe/asr-prompt-id.fk` -- closed-set prompt recognition by nearest measured feature vector.
- `presence/native-speech-loopback.fk` -- vocoder fingerprint -> native prompt ASR -> WER route gate.

This intentionally starts smaller than open ASR or a neural vocoder. The borrowed shape is from
formant/source-filter synthesis and local-ASR prompt classification: explicit data, measurable samples,
and receipts that can shift authority only when native succeeds.

## Witnesses

Formant vocoder:

```sh
cat presence/formant-vocoder.fk presence/tests/formant-vocoder-band.fk > /tmp/formant-vocoder.fk
./fkwu --src /tmp/formant-vocoder.fk
```

Witness:

```text
511
```

Closed-set ASR prompt ID:

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    observe/asr-prompt-id.fk \
    observe/tests/asr-prompt-id-band.fk > /tmp/asr-prompt-id.fk
./fkwu --src /tmp/asr-prompt-id.fk
```

Witness:

```text
255
```

Native loopback route:

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    observe/asr-prompt-id.fk \
    presence/native-speech-loopback.fk \
    presence/tests/native-speech-loopback-band.fk > /tmp/native-speech-loopback.fk
./fkwu --src /tmp/native-speech-loopback.fk
```

Witness:

```text
1023
```

## Honest Boundary

This is not natural TTS and not open dictation. It is the first native speech loopback:
generate inspectable waveform samples, classify a closed prompt set from measured features,
score WER, and route native only when the receipt passes. Real speaker/mic capture still
belongs to a host loopback carrier; local Whisper/Piper/say can be teachers, not authority.
