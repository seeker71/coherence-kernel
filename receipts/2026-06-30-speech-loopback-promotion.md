# 2026-06-30 -- speech loopback promotion

## Ground

```sh
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
11111
```

## What Changed

Added `learn/speech-loopback-promotion.fk`, a rolling authority window for native speech.
Each sample carries native success, oracle success, fail, timeout, and undo. Native receives
credit only on clean samples. Authority shifts to native only when the window is long enough,
native reaches oracle within margin, and control debt stays below the ceiling. Regression or
control debt routes back to oracle.

This is the missing bridge between a single native loopback receipt and actual oracle-to-native
shift over time.

## Witness

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    observe/asr-prompt-id.fk \
    presence/native-speech-loopback.fk \
    learn/speech-loopback-promotion.fk \
    learn/tests/speech-loopback-promotion-band.fk > /tmp/speech-loopback-promotion.fk
./fkwu --src /tmp/speech-loopback-promotion.fk
```

Witness:

```text
2047
```

## What 2047 Proves

- A native loopback receipt becomes a native-success sample.
- Native and oracle scores count clean samples.
- Clean long windows promote native.
- Short windows do not promote.
- Failed windows accumulate control debt and route oracle.
- Native regression routes oracle.
- Promotion receipts record native authority and oracle fallback.

## Honest Boundary

This promotes only the closed-set native speech lane. Live microphone/speaker capture, local
Whisper/Piper oracle carriers, open ASR, and natural acoustic/vocoder quality still need their
own receipts.
