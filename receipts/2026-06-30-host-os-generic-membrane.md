# 2026-06-30 -- host OS generic membrane and C-seed shrink guard

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
11111
```

## What Changed

Added `form/form-stdlib/host-os-membrane.fk`, the Form-owned membrane that records:

- supported target rows for macOS arm64, Windows amd64, Windows arm64, and Android arm64;
- which rows have device-metal evidence and which still need fresh receipts;
- host-resource doors for file, HTTP, audio, video, and speech loopback;
- the rule that host drivers are allowed carriers, while body law stays in Form;
- the C-seed shrink path from checkout witness to native walker and per-platform emitters.

Added `docs/coherence-substrate/host-os-generic-membrane.form` so the design is discoverable next to the other
substrate specs.

## Bounded C-Seed Repair

Closed the Android staged-input EOF gap named in `receipts/2026-06-29-android-runtime.md`: `input_byte` now returns
`0` when the requested byte is outside the staged input length, and `fk_run` records `fk_src_len` when it loads
`argv[3]`.

This is not a new runtime feature in C. It is a checkout-witness repair for an already named platform parity bug.
The destination remains `form-owned-staged-input` inside the native source runner.

## Witness

```sh
cat form/form-stdlib/hati-os-targets.fk \
    form/form-stdlib/host-os-membrane.fk \
    form/form-stdlib/tests/host-os-membrane-band.fk > /tmp/host-os-membrane.fk
./fkwu --src /tmp/host-os-membrane.fk
```

Witness:

```text
8191
```

Local table-loop EOF smoke after the guard:

```sh
printf '(add 40 2)\n' > /tmp/loop-input.fk
./fkwu flatten/form-eval-cli-loop.tbl 0 /tmp/loop-input.fk
```

The first value is `42`, followed by the normal arm counters. Empty staged input now starts with `0`, not an
unbounded noise stream:

```sh
printf '' > /tmp/empty-loop-input.fk
./fkwu flatten/form-eval-cli-loop.tbl 0 /tmp/empty-loop-input.fk
```

First value: `0`.

## Android Metal Follow-Up

Fresh phone-metal witness on a shared Android device used the pinned USB transport only:

- device: Galaxy S23 Ultra `SM-S918U1`, `arm64-v8a`, Android SDK `36`
- adb serial: `R5CW20DK17A`
- isolated device dir: `/data/local/tmp/codex-408b-c8GKyC`
- build: `/Users/ursmuff/Library/Android/ndk/android-ndk-r27c/.../aarch64-linux-android34-clang -O2 -pthread runtime/fkwu-uni.c -o /tmp/fkwu-android-408b`

Witnesses from the on-device binary:

```text
native-vs-rented direct source: 11111
host-os membrane direct source: 8191
speech-loopback carrier law:    4095
table loop, (add 40 2):         42
table loop, empty input EOF:    0
```

This promotes the Android arm64 membrane row from "fresh direct-source receipt pending" to observed. The shared
device was not modified globally: no adb server reset, no install, no port-forward, and writes stayed inside the
isolated temp directory.

## Honest Boundary

This does not claim fresh device-metal receipts for Windows arm64, or concrete local CoreAudio/WASAPI/AAudio plus
Whisper/Piper speech loopback carriers. The EOF repair has now been smoked on macOS and Android phone metal. This
gives those carriers one Form-native membrane and one receipt law to satisfy, without growing the C seed into their
home.
