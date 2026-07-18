#!/bin/sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
count=${CAO10_SAMPLE_COUNT:-20}
case "$count" in
  ''|*[!0-9]*) printf 'invalid sample count: %s\n' "$count" >&2; exit 2 ;;
esac
if [ "$count" -lt 1 ] || [ "$count" -gt 130000 ]; then
  printf 'sample count must be 1..130000\n' >&2
  exit 2
fi

run_dir=$(mktemp -d /tmp/sema-cao10-sample.XXXXXX)
trap 'rm -rf "$run_dir"' EXIT HUP INT TERM
evidence=${CAO10_EVIDENCE_FILE:-"$run_dir/evidence.log"}
: > "$evidence"

success=0
miss=0
i=0
while [ "$i" -lt "$count" ]; do
  id=$(((503 + 7919 * i) % 10000))
  case $((i % 13)) in
    0) code=en ;; 1) code=id ;; 2) code=es ;; 3) code=fr ;;
    4) code=pt-br ;; 5) code=sw ;; 6) code=de ;; 7) code=ru ;;
    8) code=zh ;; 9) code=ja ;; 10) code=ar ;; 11) code=hi ;; 12) code=tr ;;
  esac
  output=$("$repo_root/presence/carriers/concept-audio-open-10000-13-address.sh" \
    "$id" "$code" "-sample-$i")
  printf '%s\n' "$output" >> "$evidence"
  summary=$(printf '%s\n' "$output" | sed -n '1p')
  printf 'sample=%03d %s\n' "$i" "$summary"
  case "$summary" in
    *' status=success '*) success=$((success + 1)) ;;
    *' status=miss '*) miss=$((miss + 1)) ;;
    *) printf 'unreadable evidence for sample %s\n' "$i" >&2; exit 4 ;;
  esac
  i=$((i + 1))
done

printf 'sampled=%s success=%s miss=%s detector-limit=10000 address-domain=130000 evidence=%s\n' \
  "$count" "$success" "$miss" "$evidence"
test $((success + miss)) -eq "$count"
