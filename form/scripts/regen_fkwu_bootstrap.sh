#!/usr/bin/env bash
# regen_fkwu_bootstrap.sh — refresh the checkout seed through the native
# fkwu source runner and the Form emitter chain.
set -euo pipefail

FORM="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "$FORM"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh

mkdir -p form-stdlib/bootstrap
work_dir="$(mktemp -d)"
trap 'rm -rf "$work_dir"' EXIT

# The running fkwu loads the emitter chain as ordinary source preludes.
emit_fkwu="${FORM_FOURTH_SOURCE_FKWU:-}"
if [[ -z "$emit_fkwu" && -x "$FORM/../fkwu" ]]; then
    emit_fkwu="$FORM/../fkwu"
fi
if [[ -n "$emit_fkwu" && -x "$emit_fkwu" ]]; then
    printf '; preludes: %s\n(fkc-emit-universal)\n' "${FOURTH_EMIT_CHAIN[*]}" \
        > "$work_dir/emit-native.fk"
    native_rc=0
    "$emit_fkwu" "$work_dir/emit-native.fk" \
        > "$work_dir/fkwu-uni.raw.c" \
        2> "$work_dir/uni.err" || native_rc=$?
    if [[ $native_rc -ne 0 || ! -s "$work_dir/fkwu-uni.raw.c" ]]; then
        printf 'regen: fkwu-native emission failed (rc=%s): %s\n' \
            "$native_rc" "$(grep -v 'warning:' "$work_dir/uni.err" | head -1)" >&2
        exit 1
    fi
else
    printf 'regen: native source runner unavailable: %s\n' "$emit_fkwu" >&2
    exit 1
fi

# Component emitter strings may meet at a space immediately before a newline.
# Normalize only line-end whitespace so the committed witness passes the same
# whitespace gate as handwritten sources without changing emitted C tokens.
sed -e 's/[[:space:]]*$//' -e '${/^$/d;}' "$work_dir/fkwu-uni.raw.c" \
    > form-stdlib/bootstrap/fkwu-uni.c

fourth_emit_chain_stamp > form-stdlib/bootstrap/fkwu-uni.stamp

printf 'regen: form-stdlib/bootstrap/fkwu-uni.c (%s bytes) stamp=%s\n' \
    "$(wc -c < form-stdlib/bootstrap/fkwu-uni.c | tr -d ' ')" \
    "$(cat form-stdlib/bootstrap/fkwu-uni.stamp)"
