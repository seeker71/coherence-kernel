#!/bin/sh
set -eu

# Fetch the complete released Whisper-tiny encoder conv2 stride layer from the
# same pinned safetensors release as conv1. Tensor offsets were read from the
# 19,104-byte safetensors JSON header; HTTP ranges include the 19,112-byte
# header prefix and are inclusive.
root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
revision=169d4a4341b33bc18d8881c4b69c2e104e1cc0af
model_url="https://huggingface.co/openai/whisper-tiny/resolve/${revision}/model.safetensors"

test -x /usr/bin/curl || {
  printf '%s\n' '/usr/bin/curl is required' >&2
  exit 2
}

/usr/bin/curl -L --fail --silent --show-error --max-time 180 \
  --range 118598312-118599847 \
  -o "$root/encoder-conv2-bias.f32" "$model_url"
/usr/bin/curl -L --fail --silent --show-error --max-time 180 \
  --range 118599848-120369319 \
  -o "$root/encoder-conv2-weight.f32" "$model_url"

test "$(wc -c < "$root/encoder-conv2-bias.f32" | tr -d ' ')" = 1536
test "$(wc -c < "$root/encoder-conv2-weight.f32" | tr -d ' ')" = 1769472
test "$(shasum -a 256 "$root/encoder-conv2-bias.f32" | awk '{print $1}')" = \
  76fb23900c7e77f0c0f1938404ba9c3d1ca569115abb62daa8d9cb3ac08192b3
test "$(shasum -a 256 "$root/encoder-conv2-weight.f32" | awk '{print $1}')" = \
  3b38df5c53ddbe1e9a38fdebb02d0d59b3ed3a4626409499bf1c4ea9ef2dc8d4

printf '%s\n' 'Whisper-tiny conv2 fetched and SHA-256 verified'
