#!/usr/bin/env bash
# build-android.sh — cross-compile the Form Go kernel for Android ARM64.
#
# Why: this is only a proof-sibling portability check for the Go walker. Runtime
# authority belongs to the fkwu surface; Go-only compiler conductors are not
# carried here.
#
# Two targets, best-first:
#   BIONIC  (NDK present)  GOOS=android GOARCH=arm64 CGO_ENABLED=1 CC=<ndk clang>
#           → "ELF ... ARM aarch64 ... interpreter /system/bin/linker64", the
#             genuine Android binary (runs on a device and natively in Termux),
#             same class the Rust kernel build produces.
#   STATIC  (no NDK)       GOOS=linux GOARCH=arm64 CGO_ENABLED=0
#           → a statically linked ARM aarch64 ELF, runnable via Termux / adb and
#             under qemu-aarch64. No NDK, no cgo (the cgo dylib-JIT degrades to
#             jit_inram_other, which the interpreter never needs).
#
# Install the NDK for the bionic target: brew install android-ndk.
# See docs/coherence-substrate/kernel-on-android.form for the on-device wiring.
set -euo pipefail
cd "$(dirname "$0")"

OUT="bin-go-android"

# Resolve the NDK the way the Rust sibling does: env first, then the brew Cask,
# then the SDK ndk dir. Find an aarch64 android clang inside it (prebuilt host
# dir is darwin-x86_64 on a Mac — runs under Rosetta — or linux-x86_64 on CI).
NDK="${ANDROID_NDK_HOME:-${ANDROID_NDK_ROOT:-}}"
[ -n "$NDK" ] || NDK="$(ls -d /opt/homebrew/Caskroom/android-ndk/*/AndroidNDK*/Contents/NDK 2>/dev/null | head -1 || true)"
[ -n "$NDK" ] || NDK="$(ls -d "$HOME"/Library/Android/sdk/ndk/* 2>/dev/null | sort -V | tail -1 || true)"
CC=""
[ -n "$NDK" ] && CC="$(ls "$NDK"/toolchains/llvm/prebuilt/*/bin/aarch64-linux-android21-clang 2>/dev/null | head -1 || true)"

if [ -n "$CC" ]; then
  echo "→ NDK found ($NDK)"
  echo "→ bionic build: GOOS=android GOARCH=arm64 CGO_ENABLED=1 (NDK clang) ..."
  GOOS=android GOARCH=arm64 CGO_ENABLED=1 CC="$CC" go build -o "$OUT" .
  VARIANT="bionic (linker64)"
else
  echo "→ no NDK (set ANDROID_NDK_HOME or: brew install android-ndk) — static fallback"
  echo "→ static build: GOOS=linux GOARCH=arm64 CGO_ENABLED=0 ..."
  GOOS=linux GOARCH=arm64 CGO_ENABLED=0 go build -o "$OUT" .
  VARIANT="static (Termux/qemu)"
fi

echo
file "$OUT"
case "$(file "$OUT")" in
  *"ARM aarch64"*) echo "✓ Android ARM64 Go kernel binary [$VARIANT] at $OUT" ;;
  *) echo "✗ not an ARM aarch64 ELF — investigate"; exit 1 ;;
esac

# Check only a small common execution surface. This script must not require old
# Go-only compiler/conductor primitives.
echo
echo "→ verifying common primitive names ride along:"
# Dump the symbol strings ONCE to a file and grep the file — piping
# `strings | grep -q` trips SIGPIPE under `set -o pipefail` (grep -q exits early,
# strings dies 141, the pipeline reads as failure even on a match).
syms="$(mktemp)"; strings "$OUT" > "$syms"
miss=0
for n in add mul str_len read_file write_file_text; do
  if grep -qF "$n" "$syms"; then echo "  ✓ $n"; else echo "  ✗ $n MISSING"; miss=1; fi
done
rm -f "$syms"
[ "$miss" = 0 ] || { echo "✗ a common primitive is missing — investigate"; exit 1; }

# Execution. Best is a real device over adb (works for the bionic binary). Else
# a static binary under qemu-aarch64 (Linux CI ships it; a macOS host does not).
# Else the ELF type + common primitive check is the proof here, and on-device is
# one adb push away.
echo
DEV="$(command -v adb >/dev/null 2>&1 && adb get-state 2>/dev/null || true)"
printf '(add (mul 6 7) 1)\n' > /tmp/go-android-probe.fk
if [ "$DEV" = "device" ]; then
  echo "→ device attached — running the common probe on Android via adb:"
  adb push "$OUT" /data/local/tmp/bin-go-android >/dev/null
  adb push /tmp/go-android-probe.fk /data/local/tmp/ >/dev/null
  val="$(adb shell 'cd /data/local/tmp && ./bin-go-android go-android-probe.fk' 2>&1 | tail -1)"
  echo "  (add (mul 6 7) 1) -> $val"
  case "$val" in 43) echo "✓ the Go proof sibling executes on Android (device)";; *) echo "✗ unexpected — investigate"; exit 1;; esac
elif [ "$VARIANT" = "static (Termux/qemu)" ] && command -v qemu-aarch64 >/dev/null 2>&1; then
  echo "→ qemu-aarch64 present — running the static binary (emulated ARM64):"
  val="$(qemu-aarch64 "$OUT" /tmp/go-android-probe.fk 2>&1 | tail -1)"
  echo "  (add (mul 6 7) 1) -> $val"
  case "$val" in 43) echo "✓ the Go proof sibling executes on ARM64 (emulated)";; *) echo "✗ unexpected — investigate"; exit 1;; esac
else
  echo "  no device/qemu on this host — ELF + common primitive check is the proof here."
  echo "  on a device:  adb push $OUT /data/local/tmp/"
  echo "                adb shell /data/local/tmp/$OUT probe.fk   (or Termux)"
fi

echo
echo "✓ the gen conductor cross-compiles to a genuine Android ARM64 binary [$VARIANT]."
