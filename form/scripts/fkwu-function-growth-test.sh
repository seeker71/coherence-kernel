#!/bin/sh
# Witness that direct-source function identity is demand-grown, including a
# higher-order function value whose index is beyond the retired 4096 boundary.
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
work_dir=$(mktemp -d "${TMPDIR:-/tmp}/fkwu-function-growth.XXXXXX")
trap 'rm -rf "$work_dir"' EXIT HUP INT TERM
source_file="$work_dir/function-growth.fk"

awk 'BEGIN {
    print "(do"
    for (i = 1; i <= 5000; i++)
        printf "  (defn growth_%d () %d)\n", i, i
    print "  (defn growth_call_0 (f) (f))"
    print "  (growth_call_0 growth_5000))"
}' > "$source_file"

first=$("$repo_root/fkwu" "$source_file")
second=$("$repo_root/fkwu" "$source_file")
if [ "$first" != 5000 ] || [ "$second" != 5000 ]; then
    printf 'function-growth mismatch: first=%s second=%s\n' "$first" "$second" >&2
    exit 1
fi
printf 'function-growth direct-source=5000 cached-image=5000 dynamic=1\n'
