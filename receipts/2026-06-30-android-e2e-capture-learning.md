# 2026-06-30 -- Android end-to-end capture learning toward oracle

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

Added `learn/speech-loopback-capture-learning.fk`, a Form-native learning receipt for real on-device audio
capture. The carrier renders a closed prompt locally, captures from the same device, computes measured facts, and
writes a Form call. The body checks audio presence, loopback envelope evidence, feature-distance improvement,
WER improvement toward the local oracle label, and native routing over a clean learned window.

This receipt uses a closed-prompt oracle: the rendered prompt label is the local teacher. It does not claim open
dictation or a general ASR model.

## Android Metal Run

Device:

```text
Galaxy S23 Ultra SM-S918U1
arm64-v8a
Android SDK 36
adb serial R5CW20DK17A
```

The shared device was handled with the same sibling-safe constraints: pinned USB serial, isolated temp dir, no
install, no adb server reset, no port-forward, and no global audio setting changes.

Temp dir:

```text
/data/local/tmp/codex-e2e-capture-SOe4xk
```

The temporary AAudio carrier opened both framework streams:

```text
input=1 rate=16000 channels=1 output=1 rate=16000 channels=1 amp=9000
```

Measured capture facts:

```text
frames=23200
nonzero=23109
energy=3006571
avg=129
peak=561
hash_mod=811960308
best_lag=4
on=1491283
off=1045058
score=446225
loopback=1
```

The probe wrote this Form call on-device:

```text
(scl-on-device-check 16000 1 23200 23109 3006571 561 811960308 1491283 1045058 4 446225 129 9000 1)
```

Then Android `fkwu` ran the measured receipt:

```sh
toybox cat speech-loopback-capture-learning-base.fk capture-call.fk > capture-learning-run.fk
fkwu-android --src capture-learning-run.fk
```

Witness:

```text
8191
```

Both carrier and Form run exited cleanly:

```text
probe_rc=0 form_rc=0
```

The temp directory was removed afterward. The probe did not write or retain raw audio; it retained only measured
facts in the Form call.

## What 8191 Proves

- Captured audio metadata is present.
- Local render amplitude is present.
- Loopback envelope evidence is present: prompt-on energy exceeded prompt-off energy.
- The untrained native model starts away from the observed feature.
- The learned center reaches the observed feature.
- WER improves toward the oracle prompt.
- Learned native WER reaches zero.
- The clean learned sample carries native success and no control debt.
- A clean learned window routes native.
- The receipt preserves loopback evidence and nonzero audio hash.

## Honest Boundary

This is the first real Android end-to-end capture learning receipt. It is not open ASR, not natural TTS, and not
whisper.cpp. The oracle is the local prompt label for a generated closed prompt. The next step is to replace the
closed prompt oracle with a local open ASR oracle while keeping this same receipt law.
