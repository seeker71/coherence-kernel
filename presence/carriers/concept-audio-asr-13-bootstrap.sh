#!/bin/sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
model=${SEMA_WHISPER_MODEL:-"$repo_root/.cache/whisper.cpp/ggml-large-v3-turbo.bin"}
expected=1fc70f774d38eb169993ac391eea357ef47c88757ef72ee5943879b7e8e2bc69
url=https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-large-v3-turbo.bin

if ! command -v whisper-cli >/dev/null 2>&1 || ! command -v ffmpeg >/dev/null 2>&1; then
  if ! command -v brew >/dev/null 2>&1; then
    printf '%s\n' 'missing whisper-cli or ffmpeg; install whisper-cpp and ffmpeg, then rerun' >&2
    exit 1
  fi
  brew install whisper-cpp ffmpeg
fi
command -v say >/dev/null 2>&1
command -v shasum >/dev/null 2>&1
test -x /usr/bin/curl

mkdir -p "$(dirname -- "$model")"
if [ -f "$model" ]; then
  actual=$(shasum -a 256 "$model" | awk '{print $1}')
  if [ "$actual" = "$expected" ]; then
    printf 'model=%s\nsha256=%s\nstatus=already-valid\n' "$model" "$actual"
    exit 0
  fi
  backup="$model.invalid.$(date +%Y%m%d%H%M%S)"
  mv "$model" "$backup"
  printf 'moved invalid model to %s\n' "$backup" >&2
fi

partial="$model.partial"
/usr/bin/curl -L --fail --retry 3 -C - "$url" -o "$partial"
actual=$(shasum -a 256 "$partial" | awk '{print $1}')
if [ "$actual" != "$expected" ]; then
  printf 'hash mismatch expected=%s actual=%s partial=%s\n' "$expected" "$actual" "$partial" >&2
  exit 1
fi
mv "$partial" "$model"
printf 'model=%s\nsha256=%s\nstatus=downloaded-and-verified\n' "$model" "$actual"
