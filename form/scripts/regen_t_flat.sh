#!/usr/bin/env bash
# regen_t_flat.sh — maintainer bridge that refreshes the committed
# fourth-arm self-host flattener table through the Go proof sibling.
# This carrier retires when fkwu owns the one-shot bootstrap flatten directly.
set -euo pipefail

FORM="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
GO_KERNEL="$FORM/form-kernel-go/bin-go"

# Rebuild the proof sibling so ignored local binaries never author a fresh
# table from stale source.
(cd "$FORM/form-kernel-go" && go build -o bin-go .)

cd "$FORM"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
export GO_BIN="$GO_KERNEL"

work_dir="$(mktemp -d)"
trap 'rm -rf "$work_dir"' EXIT

fourth_flatten_expr fks \
    form-stdlib/minimal-surface.fk \
    form-stdlib/hati-os-kernel.fk \
    form-stdlib/fkc-table-serialize.fk \
    form-stdlib/form-parse.fk \
    form-stdlib/form-flatten.fk \
    form-stdlib/fourth-flatten-driver.fk \
    > "$work_dir/expr.fk"

"$GO_KERNEL" "${FOURTH_FLATTEN_CHAIN[@]}" "$work_dir/expr.fk" \
    > "$work_dir/T.txt" \
    2> "$work_dir/go.err"

if [[ ! -s "$work_dir/T.txt" ]]; then
    sed -n '1,20p' "$work_dir/go.err" >&2
    exit 1
fi

build_fourth
sources=()
while IFS= read -r source; do
    sources+=("$source")
done < <(fourth_prep_srcs adler32)

if [[ "${#sources[@]}" -lt 1 ]]; then
    echo "regen_t_flat: source-text preparation failed for adler32" >&2
    exit 1
fi

{ printf '1\n'; fourth_band_request adler32 fks "${sources[@]}"; } \
    | "$FKWU" "$work_dir/T.txt" 0 \
        > "$work_dir/adler-framed.txt" \
        2> "$work_dir/adler-flatten.err"

sed -n '/^==T-adler32==$/,/^==T-END==$/p' "$work_dir/adler-framed.txt" \
    | sed -e '1d' -e '$d' \
    > "$work_dir/adler-table.txt"

if [[ ! -s "$work_dir/adler-table.txt" ]]; then
    echo "regen_t_flat: fkwu smoke failed — no adler32 table between markers" >&2
    exit 1
fi

"$FKWU" "$work_dir/adler-table.txt" 0 \
    > "$work_dir/adler-result.txt" \
    2> "$work_dir/adler-run.err"
verdict="$(sed -n '1p' "$work_dir/adler-result.txt")"
if [[ "$verdict" != "5" ]]; then
    echo "regen_t_flat: fkwu smoke failed — adler32 verdict=$verdict, expected 5" >&2
    exit 1
fi

mv -f "$work_dir/T.txt" "$FOURTH_FLATTEN_TABLE"
fourth_hash16 "${FOURTH_FLATTEN_CHAIN[@]}" form-stdlib/fourth-flatten-driver.fk \
    > form-stdlib/fourth-flatten-table.stamp
rm -f form-stdlib/.cache/fourth/t-*.txt 2>/dev/null || true

printf 'regen: %s (%s bytes) stamp=%s\n' \
    "$FOURTH_FLATTEN_TABLE" \
    "$(wc -c < "$FOURTH_FLATTEN_TABLE" | tr -d ' ')" \
    "$(cat form-stdlib/fourth-flatten-table.stamp)"
