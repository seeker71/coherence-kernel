#!/usr/bin/env bash
# regen_fkwu_bootstrap.sh — maintainer bridge that refreshes the committed
# fkwu checkout seed from the Form emitter chain on fkwu itself.
set -euo pipefail

FORM="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EMIT_CELL="$FORM/form-stdlib/fkwu-bootstrap-emit-cli.fk"

cd "$FORM"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
FKWU="$FORM/../fkwu"

[[ -x "$FKWU" ]] || {
    printf 'regen: fkwu is missing: %s\n' "$FKWU" >&2
    exit 1
}
[[ -s "$EMIT_CELL" ]] || {
    printf 'regen: Form emitter cell is missing: %s\n' "$EMIT_CELL" >&2
    exit 1
}

mkdir -p form-stdlib/bootstrap
work_dir="$(mktemp -d)"
trap 'rm -rf "$work_dir"' EXIT

"$FKWU" "$EMIT_CELL" > "$work_dir/fkwu-uni.raw.c" 2> "$work_dir/uni.err"

if [[ ! -s "$work_dir/fkwu-uni.raw.c" ]]; then
    sed -n '1,12p' "$work_dir/uni.err" >&2
    exit 1
fi

# Component emitter strings may meet at a space immediately before a newline.
# Normalize only line-end whitespace so the committed witness passes the same
# whitespace gate as handwritten sources without changing emitted C tokens.
sed 's/[[:space:]]*$//' "$work_dir/fkwu-uni.raw.c" \
    > form-stdlib/bootstrap/fkwu-uni.c

fourth_emit_chain_stamp > form-stdlib/bootstrap/fkwu-uni.stamp

printf 'regen: form-stdlib/bootstrap/fkwu-uni.c (%s bytes) stamp=%s\n' \
    "$(wc -c < form-stdlib/bootstrap/fkwu-uni.c | tr -d ' ')" \
    "$(cat form-stdlib/bootstrap/fkwu-uni.stamp)"
