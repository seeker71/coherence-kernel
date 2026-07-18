#!/bin/sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
id=${1:?usage: concept-audio-open-10000-13-address.sh ID CODE [SUFFIX]}
code=${2:?usage: concept-audio-open-10000-13-address.sh ID CODE [SUFFIX]}
suffix=${3:--open-address}

case "$id" in
  ''|*[!0-9]*) printf 'invalid concept id: %s\n' "$id" >&2; exit 2 ;;
esac
if [ "$id" -gt 9999 ]; then
  printf 'invalid concept id: %s; expected 0..9999\n' "$id" >&2
  exit 2
fi
case "$code" in
  en|id|es|fr|pt-br|sw|de|ru|zh|ja|ar|hi|tr) ;;
  *) printf 'invalid NL lens: %s\n' "$code" >&2; exit 2 ;;
esac
case "$suffix" in
  ''|*[!A-Za-z0-9_-]*) printf 'invalid suffix: %s\n' "$suffix" >&2; exit 2 ;;
esac

model=${SEMA_WHISPER_MODEL:-"$repo_root/.cache/whisper.cpp/ggml-large-v3-turbo.bin"}
expected=1fc70f774d38eb169993ac391eea357ef47c88757ef72ee5943879b7e8e2bc69
actual=$(shasum -a 256 "$model" | awk '{print $1}')
if [ "$actual" != "$expected" ]; then
  printf 'Whisper model hash mismatch expected=%s actual=%s path=%s\n' \
    "$expected" "$actual" "$model" >&2
  exit 3
fi
SEMA_WHISPER_MODEL=$model
CASR13_MODEL_HASH_VERIFIED=$actual
export SEMA_WHISPER_MODEL CASR13_MODEL_HASH_VERIFIED

run_dir=$(mktemp -d /tmp/sema-cao10-address.XXXXXX)
trap 'rm -rf "$run_dir"' EXIT HUP INT TERM
source_file="$run_dir/address.fk"
sed -n 'p' "$repo_root/presence/concept-audio-open-10000-13-live.fk" > "$source_file"
printf '\n(cao10l-address-execute %s "%s" "%s")\n' "$id" "$code" "$suffix" >> "$source_file"

cd "$repo_root"
./fkwu --src "$source_file" 2>/dev/null
