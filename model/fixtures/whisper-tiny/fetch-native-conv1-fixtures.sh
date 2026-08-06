#!/bin/sh
set -eu

# Fetch the complete first learned acoustic layer from the pinned released
# openai/whisper-tiny safetensors file, plus the CC0 Lingua Libre recording used
# by the native live witness.  /usr/bin/curl is present on the supported macOS
# checkout even when a package-manager `curl` is not on PATH.
root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
revision=169d4a4341b33bc18d8881c4b69c2e104e1cc0af
model_url="https://huggingface.co/openai/whisper-tiny/resolve/${revision}/model.safetensors"
audio_url='https://upload.wikimedia.org/wikipedia/commons/f/f8/LL-Q1860_%28eng%29-Simplificationalizer-book.wav?download=1'

test -x /usr/bin/curl || {
  printf '%s\n' '/usr/bin/curl is required' >&2
  exit 2
}
for command_name in ffmpeg shasum awk; do
  command -v "$command_name" >/dev/null 2>&1 || {
    printf 'missing command: %s\n' "$command_name" >&2
    exit 2
  }
done

run_dir=$(mktemp -d /tmp/sema-whisper-conv1.XXXXXX)
trap 'rm -rf "$run_dir"' EXIT HUP INT TERM

# Safetensors header length is 19,104 bytes.  Tensor offsets in that header are
# relative to byte 19,112.  These inclusive HTTP ranges therefore select every
# f32 of model.encoder.conv1.bias [384] and weight [384,80,3].
/usr/bin/curl -L --fail --silent --show-error --max-time 180 \
  --range 118228136-118229671 \
  -o "$root/encoder-conv1-bias.f32" "$model_url"
/usr/bin/curl -L --fail --silent --show-error --max-time 180 \
  --range 118229672-118598311 \
  -o "$root/encoder-conv1-weight.f32" "$model_url"

/usr/bin/curl -L --fail --silent --show-error --max-time 120 \
  -o "$run_dir/source.wav" "$audio_url"
test "$(shasum -a 256 "$run_dir/source.wav" | awk '{print $1}')" = \
  b01e92bb0f8d48214c52630b8432d2e980cd6200c28d1363df49848fe4316614
ffmpeg -loglevel error -y -i "$run_dir/source.wav" -map_metadata -1 \
  -fflags +bitexact -flags:a +bitexact -ar 16000 -ac 1 -c:a pcm_s16le \
  "$root/lingua-libre-book-16k.wav"

test "$(wc -c < "$root/encoder-conv1-bias.f32" | tr -d ' ')" = 1536
test "$(wc -c < "$root/encoder-conv1-weight.f32" | tr -d ' ')" = 368640
test "$(shasum -a 256 "$root/encoder-conv1-bias.f32" | awk '{print $1}')" = \
  a8deb23b8cb5d0a88ffa398c9951ef92a3e47d44b32412dcb40b01895ec4772f
test "$(shasum -a 256 "$root/encoder-conv1-weight.f32" | awk '{print $1}')" = \
  bb6642598e3efd8ea1fe81605f864342bb174604cba8dee5c23aa223fc126ecb
test "$(shasum -a 256 "$root/lingua-libre-book-16k.wav" | awk '{print $1}')" = \
  1166acadc40e8d60baa82c6321ba3445fda5305a46539c3d1a0cc43e425de523

printf '%s\n' 'Whisper-tiny conv1 and Lingua Libre human recording fetched and SHA-256 verified'
