#!/bin/sh
# A focused source-loader witness: 160 chained Form dependencies force growth
# through 32 -> 64 -> 128 -> 256 metadata rows. The root value proves that the
# whole closure still reached the native source/JIT evaluator.
set -eu

fkwu_bin="${1:-./fkwu}"
work_dir="$(TMPDIR=/tmp mktemp -d /tmp/fkwu-dep-growth.XXXXXX)"
trap 'rm -rf "$work_dir"' EXIT HUP INT TERM

i=159
while [ "$i" -ge 0 ]; do
    dep="$work_dir/dep-$i.fk"
    if [ "$i" -lt 159 ]; then
        next=$((i + 1))
        printf '; preludes: dep-%s.fk\n(do 0)\n' "$next" > "$dep"
    else
        printf '(do 0)\n' > "$dep"
    fi
    i=$((i - 1))
done
printf '; preludes: dep-0.fk\n(do 4242)\n' > "$work_dir/root.fk"

if ! answer="$($fkwu_bin "$work_dir/root.fk" 2>"$work_dir/stderr")"; then
    cat "$work_dir/stderr" >&2
    exit 1
fi
if grep -q 'error:\|could not grow .fk dependency metadata' "$work_dir/stderr"; then
    cat "$work_dir/stderr" >&2
    exit 1
fi
if [ "$answer" != "4242" ]; then
    printf 'fkwu dependency-growth expected 4242, got %s\n' "$answer" >&2
    exit 1
fi
printf 'fkwu source dependency growth: 160 dependencies -> 4242\n'
