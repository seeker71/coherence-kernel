#!/bin/sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
id=${1:?usage: concept-open-content-address.sh ID NL-CODE PL-LANGUAGE [POSITION] [SUFFIX]}
code=${2:?usage: concept-open-content-address.sh ID NL-CODE PL-LANGUAGE [POSITION] [SUFFIX]}
pl=${3:?usage: concept-open-content-address.sh ID NL-CODE PL-LANGUAGE [POSITION] [SUFFIX]}
position=${4:-0}
suffix=${5:--content-address}

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
case "$pl" in
  python|javascript|typescript|java|c|cpp|csharp|go|rust|ruby|php|swift|kotlin) ;;
  *) printf 'invalid PL lens: %s\n' "$pl" >&2; exit 2 ;;
esac
case "$position" in
  ''|*[!0-9]*) printf 'invalid world position: %s\n' "$position" >&2; exit 2 ;;
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

run_dir=$(mktemp -d /tmp/sema-content-address.XXXXXX)
trap 'rm -rf "$run_dir"' EXIT HUP INT TERM
source_file="$run_dir/address.fk"
cp "$repo_root/presence/concept-open-content-runtime.fk" "$source_file"
printf '\n(concept-open-content-runtime-execute %s "%s" "%s" %s "%s")\n' \
  "$id" "$code" "$pl" "$position" "$suffix" >> "$source_file"

cd "$repo_root"
./fkwu --src "$source_file"
