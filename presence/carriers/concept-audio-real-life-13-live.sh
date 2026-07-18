#!/bin/sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
model=${SEMA_WHISPER_MODEL:?set SEMA_WHISPER_MODEL for this command}
expected_model=1fc70f774d38eb169993ac391eea357ef47c88757ef72ee5943879b7e8e2bc69
actual_model=$(shasum -a 256 "$model" | awk '{print $1}')
test "$actual_model" = "$expected_model"
CASR13_MODEL_HASH_VERIFIED=$actual_model
export CASR13_MODEL_HASH_VERIFIED

run_dir=$(mktemp -d /tmp/sema-carl13-live.XXXXXX)
trap 'rm -rf "$run_dir"' EXIT HUP INT TERM

start=${CARL13_START:-0}
count=${CARL13_COUNT:-13}
end=$((start + count))
if [ "$start" -lt 0 ] || [ "$count" -lt 1 ] || [ "$end" -gt 13 ]; then
  printf 'invalid batch start=%s count=%s; require 0 <= start and start+count <= 13\n' \
    "$start" "$count" >&2
  exit 1
fi

passed=0
index=$start
while [ "$index" -lt "$end" ]; do
  source_file="$run_dir/row-$index.fk"
  sed '$d' "$repo_root/presence/concept-audio-real-life-13-live.fk" > "$source_file"
  printf '\n(carl13-index-execute %s)\n' "$index" >> "$source_file"
  output=$(cd "$repo_root" && ./fkwu --src "$source_file" 2>/dev/null)
  printf '%s\n' "$output"
  verdict=$(printf '%s\n' "$output" | tail -n 1)
  printf 'row=%02d verdict=%s\n' "$index" "$verdict"
  if [ "$verdict" = 127 ]; then
    passed=$((passed + 1))
  fi
  index=$((index + 1))
done

printf 'semantic_content=%s/%s concepts=13 locales=13 heldout_voices=7 full_detector_limit=10000 address_envelope=0\n' \
  "$passed" "$count"
test "$passed" -eq "$count"
