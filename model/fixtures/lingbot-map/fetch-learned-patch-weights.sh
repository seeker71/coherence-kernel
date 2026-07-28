#!/bin/sh
set -eu

# Byte ranges from the released LingBot-Map balanced checkpoint.  These are
# complete PyTorch ZIP local records for the DINOv2-L/14 patch projection
# weight (`checkpoint/data/7`) and bias (`checkpoint/data/8`).
root='model/fixtures/lingbot-map'
revision='204754b72bb24f561f8d7e7e1e4e4cd9e809adf9'
url="https://huggingface.co/robbyant/lingbot-map/resolve/${revision}/lingbot-map.pt"

command -v curl >/dev/null 2>&1 || {
  echo 'curl is required to fetch the pinned checkpoint ranges' >&2
  exit 1
}

curl -L --fail --silent --show-error --max-time 300 \
  --range 5954448-8363023 --max-filesize 3000000 \
  -o "${root}/checkpoint-data7-record.bin" "${url}"
curl -L --fail --silent --show-error --max-time 120 \
  --range 8363024-8367247 --max-filesize 10000 \
  -o "${root}/checkpoint-data8-record.bin" "${url}"

actual_weight=$(shasum -a 256 "${root}/checkpoint-data7-record.bin" | awk '{print $1}')
actual_bias=$(shasum -a 256 "${root}/checkpoint-data8-record.bin" | awk '{print $1}')
test "$actual_weight" = '2c07e9f1d118d54358dc10eb56b16b8d4b81f3f0da11b2712133f1b8d1b54880'
test "$actual_bias" = '1bc851cacd9e6532372dafd9b4a3195ade615843e5c4d5d5bd6fd4bd33df94fe'

printf '%s\n' 'LingBot-Map learned patch records fetched and SHA-256 verified'
