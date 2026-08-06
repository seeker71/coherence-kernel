#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
source_wav="$root/model/fixtures/whisper-tiny/lingua-libre-book-16k.wav"
expected=1166acadc40e8d60baa82c6321ba3445fda5305a46539c3d1a0cc43e425de523
test "$(shasum -a 256 "$source_wav" | awk '{print $1}')" = "$expected"

run_dir=$(mktemp -d /tmp/sema-native-acoustic.XXXXXX)
trap 'rm -rf "$run_dir"' EXIT HUP INT TERM
neutral_wav="$run_dir/000.wav"
cp "$source_wav" "$neutral_wav"

program="$run_dir/live.fk"
sed -n 'p' "$root/presence/whisper-tiny-native-acoustic-live.fk" > "$program"
printf '\n(wtal-execute-file "%s")\n' "$neutral_wav" >> "$program"
cd "$root"
exec ./fkwu --src "$program"
