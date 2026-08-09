#!/bin/sh
set -eu

repo_root=$(CDPATH= cd "$(dirname "$0")/../.." && pwd)
cd "$repo_root"

passes=0

for band_file in form/form-stdlib/tests/living-world*-band.fk; do
    expected=$(sed -n \
        's/^; Verdict \([0-9][0-9]*\).*/\1/p' "$band_file" \
        | head -n 1)
    if [ -z "$expected" ]; then
        printf 'FAIL command=read-declared-verdict source=%s reason=missing-header\n' \
            "$band_file" >&2
        exit 1
    fi

    set +e
    actual=$(./fkwu "$band_file")
    child_status=$?
    set -e
    if [ "$child_status" -ne 0 ]; then
        printf 'FAIL command=./fkwu %s exit=%s output=%s\n' \
            "$band_file" "$child_status" "$actual" >&2
        exit "$child_status"
    fi
    if [ "$actual" != "$expected" ]; then
        printf 'FAIL command=./fkwu %s expected=%s actual=%s\n' \
            "$band_file" "$expected" "$actual" >&2
        exit 1
    fi

    passes=$((passes + 1))
    printf 'PASS index=%s source=%s verdict=%s\n' \
        "$passes" "$band_file" "$actual"
done

if [ "$passes" -ne 84 ]; then
    printf 'FAIL declared_living_world_bands expected=84 actual=%s\n' \
        "$passes" >&2
    exit 1
fi

printf 'declared_living_world_bands_passed=%s\n' "$passes"
