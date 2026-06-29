# Receipt — c-bootstrap fkwu runs Form on Android metal (Galaxy S23 Ultra)

**2026-06-29.** The Android row of the standard receipt — observed, with one honest platform difference found by running on metal.

## What was observed

The clean kernel's own C bootstrap (`runtime/fkwu-uni.c`) cross-compiled to Android arm64 (NDK r27c → `ELF 64-bit arm64, for Android 34`), pushed to a **Galaxy S23 Ultra (SM-S918U1, arm64-v8a)**, and ran the `form-eval-cli-loop` read-eval-print loop on-device:

```
$ adb shell '/data/local/tmp/fkwu /data/local/tmp/loop-table.txt 0 /data/local/tmp/loop-input.txt'
42        # (add 40 2)
100       # (do (let x 10) (mul x x))
99        # (if (le 3 5) 99 0)
...
```

All three Form expressions evaluated correctly, in order, on Android metal. The eval underneath is the four-way-proven `form-eval-full` grammar; the loop is the fkwu-native host-io shell.

## Toolchain-free — confirmed

`command -v go rustc clang gcc python node` on the device: **none found.** The device carries no toolchain; only the fkwu binary + the platform-independent node table run. Go/Rust/TS walkers were not in the loop.

## Honest floor (per standard-receipt.form)

- **toolchain-free RUNTIME: yes** — observed, no rented toolchain in the run on Android metal.
- **c-bootstrap: clang-built** — the binary is NDK-clang-compiled from the C bootstrap. The fully-clang-free path (form-asm → form-elf bytes) is the separate pending row, exactly as the standard receipt names it. This is a real rung; not the top one.

## The platform difference found (a real bug, named not hidden)

After the three correct values the loop kept emitting noise. Root cause: on Android, `input_byte` (tag 17) reading **past end-of-file returns garbage**; on Mac it returns `0` (EOF), so the loop's `(if (eq (input_byte i) 0) 0 ...)` terminator fires cleanly on Mac and not on Android. The *evaluation* is identical and correct on both platforms (42/100/99); the *staged-input bounds-check* in `fk_src` reads past length on Android. The fix is a bounds-guard in the input_byte read (return 0 when `i >= fk_src_len`) — a one-line c-bootstrap correction, and a four-way-relevant one since the loop's EOF contract should hold on every platform.

## Build + run, reproducible

```
# build (off-device, the one cc seed — here NDK clang):
aarch64-linux-android34-clang -O2 -pthread runtime/fkwu-uni.c -o fkwu-android
# table (off-device, bin-go flatten of the merged form-eval-cli-loop recipe)
# run (on-device, toolchain-free):
adb push fkwu-android /data/local/tmp/fkwu && adb shell chmod 755 /data/local/tmp/fkwu
adb shell '/data/local/tmp/fkwu /data/local/tmp/loop-table.txt 0 /data/local/tmp/loop-input.txt'
```

The body runs on the phone. Next: the input_byte EOF bounds-guard (so the loop terminates cleanly everywhere), then the same observation on Windows, then the clang-free form-asm build that turns the c-bootstrap row from clang-built to fully sovereign.
