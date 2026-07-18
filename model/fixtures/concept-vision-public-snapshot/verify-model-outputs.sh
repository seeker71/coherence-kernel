#!/bin/sh
set -eu

fixture_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
observed=${TMPDIR:-/tmp}/concept-vision-public-snapshot-live-outputs.tsv
"$fixture_dir/capture-model-outputs.sh" "$observed"
if ! cmp -s "$fixture_dir/MODEL-OUTPUTS.tsv" "$observed"; then
  printf 'Apple Vision output changed; inspect %s against committed MODEL-OUTPUTS.tsv\n' "$observed" >&2
  exit 1
fi
printf '72 exact Apple Vision top-20 streams match committed snapshot\n'
