#!/usr/bin/env bash
# regen_fkwu_bootstrap.sh — maintainer bridge that refreshes the committed
# fkwu checkout seed from the Form emitter chain via the Go proof sibling.
# This carrier retires when the Form-native bootstrap owns emission directly.
set -euo pipefail

FORM="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
GO_KERNEL="$FORM/form-kernel-go/bin-go"

# The proof sibling is rebuilt on every regeneration so a stale ignored
# binary can never attest fresh bootstrap bytes.
(cd "$FORM/form-kernel-go" && go build -o bin-go .)

cd "$FORM"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
export GO_BIN="$GO_KERNEL"

mkdir -p form-stdlib/bootstrap
work_dir="$(mktemp -d)"
trap 'rm -rf "$work_dir"' EXIT

printf '%s\n' '(fkc-emit-universal)' > "$work_dir/emit.fk"
"$GO_KERNEL" "${FOURTH_EMIT_CHAIN[@]}" "$work_dir/emit.fk" \
    > form-stdlib/bootstrap/fkwu-uni.c \
    2> "$work_dir/uni.err"

if [[ ! -s form-stdlib/bootstrap/fkwu-uni.c ]]; then
    sed -n '1,12p' "$work_dir/uni.err" >&2
    exit 1
fi

fourth_emit_chain_stamp > form-stdlib/bootstrap/fkwu-uni.stamp

printf 'regen: form-stdlib/bootstrap/fkwu-uni.c (%s bytes) stamp=%s\n' \
    "$(wc -c < form-stdlib/bootstrap/fkwu-uni.c | tr -d ' ')" \
    "$(cat form-stdlib/bootstrap/fkwu-uni.stamp)"
