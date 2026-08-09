#!/usr/bin/env bash
# regen_t_flat.sh — refresh the committed fourth-arm self-host flattener table
# by executing its Form recipe directly on fkwu.
set -euo pipefail

FORM="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "$FORM"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh

work_dir="$(mktemp -d)"
trap 'rm -rf "$work_dir"' EXIT

build_fourth
[[ -n "$FKWU" && -x "$FKWU" ]] || {
    echo "regen_t_flat: native fkwu carrier is unavailable" >&2
    exit 1
}

# T_flat is already a Form flattener running on fkwu.  Ask that living image to
# compile the current flattener sources into its successor.  The new primitive
# vocabulary is ordinary data in form-flatten.fk, so the previous image can
# carry it without knowing or executing the added host opcodes itself.
if ! fourth_flatten_sources t-flat-refresh fks "$work_dir/T.txt" \
    form-stdlib/minimal-surface.fk \
    form-stdlib/hati-os-kernel.fk \
    form-stdlib/fkc-table-serialize.fk \
    form-stdlib/core.fk \
    form-stdlib/form-parse.fk \
    form-stdlib/bmf-core.fk \
    form-stdlib/bmf-grammar.fk \
    form-stdlib/host-effect-grammar.fk \
    form-stdlib/form-flatten.fk \
    form-stdlib/fourth-flatten-driver.fk; then
    echo "regen_t_flat: fkwu self-rebuild did not produce T_flat" >&2
    exit 1
fi

sources=()
while IFS= read -r source; do
    sources+=("$source")
done < <(fourth_prep_srcs native-vs-rented)

if [[ "${#sources[@]}" -lt 1 ]]; then
    echo "regen_t_flat: source preparation failed for native-vs-rented" >&2
    exit 1
fi

{ printf '1\n'; fourth_band_request native-vs-rented fks "${sources[@]}"; } \
    | "$FKWU" "$work_dir/T.txt" 0 \
        > "$work_dir/native-framed.txt" \
        2> "$work_dir/native-flatten.err"

sed -n '/^==T-native-vs-rented==$/,/^==T-END==$/p' "$work_dir/native-framed.txt" \
    | sed -e '1d' -e '$d' \
    > "$work_dir/native-table.txt"

if [[ ! -s "$work_dir/native-table.txt" ]]; then
    echo "regen_t_flat: fkwu smoke failed — no native-vs-rented table between markers" >&2
    exit 1
fi

"$FKWU" "$work_dir/native-table.txt" 0 \
    > "$work_dir/native-result.txt" \
    2> "$work_dir/native-run.err"
verdict="$(sed -n '1p' "$work_dir/native-result.txt")"
if [[ "$verdict" != "11111" ]]; then
    echo "regen_t_flat: fkwu smoke failed — native-vs-rented verdict=$verdict, expected 11111" >&2
    exit 1
fi

# Voice smoke — the aphonic-carrier class. An effect-bound do-let inside a
# defn must FRAME (read_line fires once per turn, not once per use). A T_flat
# missing host-effect-grammar's closure inlines it: the second use re-reads
# stdin, gets EOF, and the echo answers wrong.
printf '(defn echo-once ()\n    (do (let line (read_line))\n        (str_concat line (str_concat "/" line))))\n(echo-once)\n' \
    > "$work_dir/voice-band.fk"
{ printf '1\n'; fourth_band_request voice fks form-stdlib/core.fk "$work_dir/voice-band.fk"; } \
    | "$FKWU" "$work_dir/T.txt" 0 \
        > "$work_dir/voice-framed.txt" \
        2> "$work_dir/voice-flatten.err"
sed -n '/^==T-voice==$/,/^==T-END==$/p' "$work_dir/voice-framed.txt" \
    | sed -e '1d' -e '$d' \
    > "$work_dir/voice-table.txt"
if [[ ! -s "$work_dir/voice-table.txt" ]]; then
    echo "regen_t_flat: voice smoke failed — no table between markers" >&2
    exit 1
fi
verdict="$(printf 'ping\n' | "$FKWU" "$work_dir/voice-table.txt" 0 2>/dev/null | sed -n '1p')"
if [[ "$verdict" != "ping/ping" ]]; then
    echo "regen_t_flat: voice smoke failed — effect-let answered '$verdict', expected 'ping/ping' (aphonic T_flat)" >&2
    exit 1
fi

# Composite-effect smoke — a lexical let must snapshot its whole value even
# when the effect is nested below a pure constructor. Root-op classification
# once inlined `(list (metal_buf_alloc ...) ...)` at every use, so the writer
# and reader received different handles while every primitive reported success.
{ printf '1\n'; fourth_band_request let-composite-effect-once fks \
    form-stdlib/core.fk \
    form-stdlib/tests/let-composite-effect-once-band.fk; } \
    | "$FKWU" "$work_dir/T.txt" 0 \
        > "$work_dir/composite-framed.txt" \
        2> "$work_dir/composite-flatten.err"
sed -n '/^==T-let-composite-effect-once==$/,/^==T-END==$/p' "$work_dir/composite-framed.txt" \
    | sed -e '1d' -e '$d' \
    > "$work_dir/composite-table.txt"
if [[ ! -s "$work_dir/composite-table.txt" ]]; then
    echo "regen_t_flat: composite-effect smoke failed — no table between markers" >&2
    exit 1
fi
"$FKWU" "$work_dir/composite-table.txt" 0 \
    > "$work_dir/composite-result.txt" \
    2> "$work_dir/composite-run.err"
verdict="$(sed -n '1p' "$work_dir/composite-result.txt")"
if [[ "$verdict" != "111" ]]; then
    echo "regen_t_flat: composite-effect smoke failed — verdict=$verdict, expected 111" >&2
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
