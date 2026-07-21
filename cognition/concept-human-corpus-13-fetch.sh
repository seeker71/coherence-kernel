#!/bin/sh
# Fetch the exact upstream corpus archives and prove that they regenerate the
# committed bounded snapshot.  Requires curl, bzip2, and Node; never Python.
set -eu

repo_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
curl_bin=$(command -v curl)
task_tmp=$(mktemp -d "${TMPDIR:-/tmp}/coherence-human-corpus.XXXXXX")
trap 'rm -rf "$task_tmp"' EXIT HUP INT TERM

tail -n +2 "$repo_dir/cognition/fixtures/human-corpus-13/ARCHIVES.tsv" |
while IFS="$(printf '\t')" read -r locale lang retrieved hash url license license_url; do
  "$curl_bin" -L --fail --silent --show-error "$url" -o "$task_tmp/$lang.tsv.bz2"
done

node "$repo_dir/cognition/concept-human-corpus-13-build.mjs" "$task_tmp" --verify
