# 2026-06-30 -- speech loopback carrier run row

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

Added `presence/speech-loopback-carrier-run.fk`, the Form-owned run row for live speech loopback carriers.

The row accepts platform carrier facts: target, render, capture, oracle, local-only, sample rate, channels, and
shared-device safety. It checks those against `form/form-stdlib/host-os-membrane.fk`, lowers native loopback into
`presence/speech-loopback-carrier-receipt.fk`, and gates `learn/speech-loopback-recipe-ab.fk` so missing capture
or unsafe carrier state becomes fail-control instead of a native cutover.

## Witness

Carrier facts -> native loopback receipt:

```sh
cat form/form-stdlib/hati-os-targets.fk \
    form/form-stdlib/host-os-membrane.fk \
    observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    observe/asr-prompt-id.fk \
    presence/native-speech-loopback.fk \
    learn/speech-loopback-promotion.fk \
    presence/speech-loopback-carrier-receipt.fk \
    presence/speech-loopback-carrier-run.fk \
    presence/tests/speech-loopback-carrier-run-band.fk > /tmp/speech-loopback-carrier-run.fk
./fkwu --src /tmp/speech-loopback-carrier-run.fk
```

Witness:

```text
511
```

Carrier-gated recipe A/B:

```sh
cat form/form-stdlib/hati-os-targets.fk \
    form/form-stdlib/host-os-membrane.fk \
    observe/stt-wer.fk \
    learn/speech-loopback-promotion.fk \
    presence/speech-loopback-carrier-receipt.fk \
    learn/speech-loopback-recipe-ab.fk \
    presence/speech-loopback-carrier-run.fk \
    learn/tests/speech-loopback-carrier-run-ab-band.fk > /tmp/speech-loopback-carrier-run-ab.fk
./fkwu --src /tmp/speech-loopback-carrier-run-ab.fk
```

Witness:

```text
2047
```

## Android Metal Follow-Up

The same two bundles were pushed to the shared Galaxy S23 Ultra through pinned USB serial `R5CW20DK17A`, inside
`/data/local/tmp/codex-408b-next-PuMYUf`, using a fresh NDK r27c arm64 build of `runtime/fkwu-uni.c`.

On-device witnesses:

```text
speech-loopback-carrier-run:    511
speech-loopback-carrier-run-ab: 2047
```

The temp directory was removed after the run. No adb server reset, install, global audio setting, or port-forward
was used.

## Honest Boundary

This is not yet a concrete microphone/speaker capture receipt. It proves the exact Form row that AAudio,
AudioRecord, AudioTrack, CoreAudio, WASAPI, Piper, say, whisper.cpp, or another local carrier must emit. The next
live step is to make one thin carrier produce a nonzero audio hash and feed this row without changing the body law.

The two carrier witnesses above are intentionally bounded slices. Concatenating every speech dependency into one
mega-run crosses the current checkout C seed's function table. That is not a reason to grow the C seed; it is a
native-walker pressure point.
