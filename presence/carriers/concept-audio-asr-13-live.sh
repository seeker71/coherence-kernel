#!/bin/sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
run_dir=$(mktemp -d /tmp/sema-casr13-live.XXXXXX)
trap 'rm -rf "$run_dir"' EXIT HUP INT TERM

model=${SEMA_WHISPER_MODEL:-"$repo_root/.cache/whisper.cpp/ggml-large-v3-turbo.bin"}
expected=1fc70f774d38eb169993ac391eea357ef47c88757ef72ee5943879b7e8e2bc69
actual=$(shasum -a 256 "$model" | awk '{print $1}')
test "$actual" = "$expected"
CASR13_MODEL_HASH_VERIFIED=$actual
export CASR13_MODEL_HASH_VERIFIED

start=${CASR13_START:-0}
count=${CASR13_COUNT:-20}
end=$((start + count))
if [ "$start" -lt 0 ] || [ "$count" -lt 1 ] || [ "$end" -gt 20 ]; then
  printf 'invalid batch start=%s count=%s; require 0 <= start and start+count <= 20\n' "$start" "$count" >&2
  exit 1
fi

passed=0
index=$start
while [ "$index" -lt "$end" ]; do
  source_file="$run_dir/row-$index.fk"
  sed '$d' "$repo_root/presence/concept-audio-asr-13-live.fk" > "$source_file"
  printf '\n(casr13l-index-verdict %s)\n' "$index" >> "$source_file"
  verdict=$(cd "$repo_root" && ./fkwu --src "$source_file" 2>/dev/null | tail -n 1)
  printf 'row=%02d verdict=%s\n' "$index" "$verdict"
  if [ "$verdict" = 127 ]; then
    passed=$((passed + 1))
  fi
  index=$((index + 1))
done

printf 'semantic_content=%s/%s batch=%s..%s address_integrity_used=0 heldout_voices=7 locales=13 concepts=3\n' \
  "$passed" "$count" "$start" "$((end - 1))"
test "$passed" -eq "$count"
