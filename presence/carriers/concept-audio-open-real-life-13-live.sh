#!/bin/sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
model=${SEMA_WHISPER_MODEL:-"$repo_root/.cache/whisper.cpp/ggml-large-v3-turbo.bin"}
expected=1fc70f774d38eb169993ac391eea357ef47c88757ef72ee5943879b7e8e2bc69
actual=$(shasum -a 256 "$model" | awk '{print $1}')
test "$actual" = "$expected"
SEMA_WHISPER_MODEL=$model
CASR13_MODEL_HASH_VERIFIED=$actual
export SEMA_WHISPER_MODEL CASR13_MODEL_HASH_VERIFIED

run_dir=$(mktemp -d /tmp/sema-cao10-real-life.XXXXXX)
trap 'rm -rf "$run_dir"' EXIT HUP INT TERM
evidence=${CAO10_REAL_LIFE_EVIDENCE_FILE:-"$run_dir/evidence.log"}
: > "$evidence"

success=0
miss=0
index=0
while [ "$index" -lt 13 ]; do
  source_file="$run_dir/row-$index.fk"
  sed -n 'p' "$repo_root/presence/concept-audio-open-real-life-13-live.fk" > "$source_file"
  printf '\n(cao10rl-execute %s)\n' "$index" >> "$source_file"
  output=$(cd "$repo_root" && ./fkwu --src "$source_file" 2>/dev/null)
  printf '%s\n' "$output" >> "$evidence"
  summary=$(printf '%s\n' "$output" | sed -n '1p')
  printf '%s\n' "$summary"
  case "$summary" in
    *' status=success '*) success=$((success + 1)) ;;
    *' status=miss '*) miss=$((miss + 1)) ;;
    *) printf 'unreadable evidence for fixture %s\n' "$index" >&2; exit 4 ;;
  esac
  index=$((index + 1))
done
printf 'situations=13 locales=13 success=%s miss=%s speech-only=13 detector-limit=10000 evidence=%s\n' \
  "$success" "$miss" "$evidence"
test $((success + miss)) -eq 13
