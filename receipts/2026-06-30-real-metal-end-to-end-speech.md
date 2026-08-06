# 2026-06-30 -- real-metal end-to-end speech grounding

## Ground

`AGENTS.md` now names the C bootstrap as a temporary checkout witness and shrink
target, not the future direction. This pass still used the required local witness
because that is the runnable checkout floor today.

Machine:

```text
Darwin Urss-MacBook-Pro.local 25.3.0 Darwin Kernel Version 25.3.0: Wed Jan 28 20:51:28 PST 2026; root:xnu-12377.91.3~2/RELEASE_ARM64_T6041 arm64
./fkwu: Mach-O 64-bit executable arm64
```

Build:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
```

Warnings observed:

```text
runtime/fkwu-uni.c:22:2010: warning: declaration of built-in function 'fread' requires inclusion of the header <stdio.h>
runtime/fkwu-uni.c:431:194: warning: passing 'int *' to parameter of type 'unsigned int *' converts between pointers to integer types with different sign
```

## End-to-End Witnesses

Ground:

```sh
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
11111
```

Observed learning:

```sh
cat form/form-stdlib/somatic-coherence-loop.fk \
    form/form-stdlib/form-cli-router.fk \
    form/form-stdlib/form-cli-sufficiency.fk \
    form/form-stdlib/observed-auto-learning.fk \
    form/form-stdlib/tests/observed-auto-learning-band.fk > /tmp/oal.fk
./fkwu --src /tmp/oal.fk
```

Witness:

```text
4095
```

Real socket loopback:

```sh
./fkwu --src form/form-stdlib/tests/fkwu-src-socket-loopback-band.fk
```

Witness:

```text
111111111
```

STT agreement:

```sh
cat observe/stt-agree.fk observe/tests/stt-agree-band.fk > /tmp/stt.fk
./fkwu --src /tmp/stt.fk
```

Witness:

```text
127
```

TTS / voice pre-acoustic pieces:

```text
observe/stt-wer.fk + observe/tests/stt-wer-band.fk                -> 255
presence/text-normalize.fk + presence/tests/text-normalize-band.fk -> 255
presence/voice-prosody.fk + (voice-prosody-check)                  -> 11111
presence/g2p.fk + (g2p-check)                                      -> 11111
presence/phoneme-timing.fk + (phoneme-timing-check)                -> 11111
presence/voice-phrasing.fk + (voice-phrasing-check)                -> 11111
presence/prosody-contour.fk + (prosody-contour-check)              -> 11111
presence/speaker-embed.fk + presence/tests/speaker-embed-band.fk   -> 255
observe/presence-feature.fk + observe/tests/presence-feature-band.fk -> 15
```

Composed speech stack:

```sh
cat observe/stt-agree.fk \
    observe/stt-wer.fk \
    presence/text-normalize.fk \
    presence/g2p.fk \
    presence/phoneme-timing.fk \
    presence/prosody-contour.fk \
    presence/voice-phrasing.fk \
    presence/voice-prosody.fk \
    presence/speaker-embed.fk \
    presence/native-speech-stack.fk \
    presence/tests/native-speech-stack-band.fk > /tmp/native-speech-stack.fk
./fkwu --src /tmp/native-speech-stack.fk
```

Witness:

```text
2047
```

## What Changed

- Wrapped `observe/tests/stt-agree-band.fk`, `presence/tests/speaker-embed-band.fk`, and
  `observe/tests/presence-feature-band.fk` in named check functions so they witness on the
  direct `fkwu --src` lane instead of returning `0` from top-level local bindings.
- Added `presence/tests/text-normalize-band.fk` for the first TTS pre-g2p normalization layer.
- Added `observe/stt-wer.fk` and `observe/tests/stt-wer-band.fk` for sequence-aligned
  STT word error rate.
- Added `presence/native-speech-stack.fk` and `presence/tests/native-speech-stack-band.fk`
  to compose STT agreement, WER, TTS pre-acoustic logic, and speaker decision into one receipt.
- Added `docs/coherence-substrate/native-speech-stack.form` to name what is native now
  and what remains pending.

## Honest Boundary

This proves the native decision/control/pre-acoustic stack on real local metal. It does
not prove a finished native ASR model, acoustic model, or vocoder. STT still needs a live
audio -> mel -> transcript candidate path and promotion gates fed by WER/agreement windows.
TTS still needs lexical stress/heteronym data, acoustic modeling beyond the first formant
floor, higher-quality waveform generation, and a perception receipt that earned confidence
is heard as intended.
