#!/usr/bin/env bash
# regen_fkwu_bootstrap.sh — maintainer bridge that refreshes the committed
# fkwu checkout seed from the Form emitter chain via the Go proof sibling.
# This carrier retires when the Form-native bootstrap owns emission directly.
set -euo pipefail

FORM="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
GO_KERNEL="$FORM/form-kernel-go/bin-go"

cd "$FORM"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
export GO_BIN="$GO_KERNEL"

mkdir -p form-stdlib/bootstrap
work_dir="$(mktemp -d)"
trap 'rm -rf "$work_dir"' EXIT

# Source-native first (2026-08-27): the running fkwu emits the seed through
# its own source-runner — no Go build, no flatten, the emit chain loaded as
# ordinary preludes. Witnessed byte-identical to the go-lane emission the day
# this door opened. The go sibling stays as the SPOKEN fallback.
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
        printf 'regen: fkwu-native emission fell through (rc=%s): %s -- go sibling carries\n' \
            "$native_rc" "$(grep -v 'warning:' "$work_dir/uni.err" | head -1)" >&2
        : > "$work_dir/fkwu-uni.raw.c"
    fi
fi

if [[ ! -s "$work_dir/fkwu-uni.raw.c" ]]; then
    # The proof sibling is rebuilt for the fallback so a stale ignored
    # binary can never attest fresh bootstrap bytes.
    (cd "$FORM/form-kernel-go" && go build -o bin-go .)
    printf '%s\n' '(fkc-emit-universal)' > "$work_dir/emit.fk"
    "$GO_KERNEL" "${FOURTH_EMIT_CHAIN[@]}" "$work_dir/emit.fk" \
        > "$work_dir/fkwu-uni.raw.c" \
        2> "$work_dir/uni.err"
fi

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
