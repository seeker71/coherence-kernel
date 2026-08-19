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

# The module list must carry form-flatten.fk's WHOLE prelude closure — the
# same modules FOURTH_FLATTEN_CHAIN walks — or the flattened flattener holds
# unresolved calls that axiom-5 recovers to literal 0. That exact gap shipped
# a T_flat whose host-effect-op? answered 0 for every op, so effect-bound
# do-lets inside defns lowered as inline re-evaluations: read_line re-fired
# once per USE and every carrier the self-host arm authored was aphonic
# (receipts/2026-07-17-regen-lane-aphonic-carrier.md).
fourth_flatten_expr fks \
    form-stdlib/minimal-surface.fk \
    form-stdlib/hati-os-kernel.fk \
    form-stdlib/form-table-text.fk \
    form-stdlib/fkc-table-serialize.fk \
    form-stdlib/core.fk \
    form-stdlib/form-parse.fk \
    form-stdlib/bmf-core.fk \
    form-stdlib/bmf-grammar.fk \
    form-stdlib/host-effect-grammar.fk \
    form-stdlib/form-flatten.fk \
    form-stdlib/fourth-flatten-driver.fk \
    > "$work_dir/expr.fk"

echo "regen_t_flat: emitting shared Form table" >&2
if ! "$GO_KERNEL" "${FOURTH_FLATTEN_CHAIN[@]}" "$work_dir/expr.fk" \
    > "$work_dir/T.raw" \
    2> "$work_dir/go.err"; then
    echo "regen_t_flat: Go bootstrap flatten failed" >&2
    sed -n '1,20p' "$work_dir/go.err" >&2
    exit 1
fi

if [[ ! -s "$work_dir/T.raw" ]]; then
    sed -n '1,20p' "$work_dir/go.err" >&2
    exit 1
fi

# fourth-flatten-driver prints the serialized numeric image as an effect and
# then returns Form nothing.  The Go proof sibling renders that final value as
# a separate `null` line.  It is not part of the table: the whole-image fkwu
# reader rightly rejects it.  Remove only that exact renderer residue; any
# other terminal shape stays in the candidate and is witnessed by the smoke.
if [[ "$(tail -n 1 "$work_dir/T.raw")" == "null" ]]; then
    sed '$d' "$work_dir/T.raw" > "$work_dir/T.txt"
else
    cp "$work_dir/T.raw" "$work_dir/T.txt"
fi

if [[ ! -s "$work_dir/T.txt" ]]; then
    echo "regen_t_flat: emitted table was empty after separating the Form return" >&2
    exit 1
fi

echo "regen_t_flat: building emitted walker" >&2
build_fourth
sources=()
while IFS= read -r source; do
    sources+=("$source")
done < <(fourth_prep_srcs adler32)

if [[ "${#sources[@]}" -lt 1 ]]; then
    echo "regen_t_flat: source-text preparation failed for adler32" >&2
    exit 1
fi

echo "regen_t_flat: flattening adler32 smoke" >&2
if ! { printf '1\n'; fourth_band_request adler32 fks "${sources[@]}"; } \
    | "$FKWU" "$work_dir/T.txt" 0 \
        > "$work_dir/adler-framed.txt" \
        2> "$work_dir/adler-flatten.err"; then
    echo "regen_t_flat: fkwu could not read the candidate shared table" >&2
    wc -c "$work_dir/T.txt" >&2
    printf 'candidate head: ' >&2
    head -c 200 "$work_dir/T.txt" >&2
    printf '\ncandidate tail: ' >&2
    tail -c 200 "$work_dir/T.txt" >&2
    printf '\n' >&2
    sed -n '1,20p' "$work_dir/adler-flatten.err" >&2
    sed -n '1,20p' "$work_dir/adler-framed.txt" >&2
    exit 1
fi

sed -n '/^==T-adler32==$/,/^==T-END==$/p' "$work_dir/adler-framed.txt" \
    | sed -e '1d' -e '$d' \
    > "$work_dir/adler-table.txt"

if [[ ! -s "$work_dir/adler-table.txt" ]]; then
    echo "regen_t_flat: fkwu smoke failed — no adler32 table between markers" >&2
    exit 1
fi

if ! "$FKWU" "$work_dir/adler-table.txt" 0 \
    > "$work_dir/adler-result.txt" \
    2> "$work_dir/adler-run.err"; then
    echo "regen_t_flat: emitted walker could not read the adler32 table" >&2
    sed -n '1,20p' "$work_dir/adler-run.err" >&2
    sed -n '1,20p' "$work_dir/adler-result.txt" >&2
    exit 1
fi
verdict="$(sed -n '1p' "$work_dir/adler-result.txt")"
if [[ "$verdict" != "5" ]]; then
    echo "regen_t_flat: fkwu smoke failed — adler32 verdict=$verdict, expected 5" >&2
    exit 1
fi

# Voice smoke — the aphonic-carrier class. An effect-bound do-let inside a
# defn must FRAME (read_line fires once per turn, not once per use). A T_flat
# missing host-effect-grammar's closure inlines it: the second use re-reads
# stdin, gets EOF, and the echo answers wrong.
printf '(defn echo-once ()\n    (do (let line (read_line))\n        (str_concat line (str_concat "/" line))))\n(echo-once)\n' \
    > "$work_dir/voice-band.fk"
echo "regen_t_flat: flattening single-read voice smoke" >&2
if ! { printf '1\n'; fourth_band_request voice fks form-stdlib/core.fk "$work_dir/voice-band.fk"; } \
    | "$FKWU" "$work_dir/T.txt" 0 \
        > "$work_dir/voice-framed.txt" \
        2> "$work_dir/voice-flatten.err"; then
    echo "regen_t_flat: fkwu could not flatten the voice smoke" >&2
    sed -n '1,20p' "$work_dir/voice-flatten.err" >&2
    sed -n '1,20p' "$work_dir/voice-framed.txt" >&2
    exit 1
fi
sed -n '/^==T-voice==$/,/^==T-END==$/p' "$work_dir/voice-framed.txt" \
    | sed -e '1d' -e '$d' \
    > "$work_dir/voice-table.txt"
if [[ ! -s "$work_dir/voice-table.txt" ]]; then
    echo "regen_t_flat: voice smoke failed — no table between markers" >&2
    exit 1
fi
if ! verdict="$(printf 'ping\n' | "$FKWU" "$work_dir/voice-table.txt" 0 2>"$work_dir/voice-run.err" | sed -n '1p')"; then
    echo "regen_t_flat: emitted walker could not read the voice table" >&2
    sed -n '1,20p' "$work_dir/voice-run.err" >&2
    exit 1
fi
if [[ "$verdict" != "ping/ping" ]]; then
    echo "regen_t_flat: voice smoke failed — effect-let answered '$verdict', expected 'ping/ping' (aphonic T_flat)" >&2
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
