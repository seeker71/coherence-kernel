#!/usr/bin/env bash
# regen_standard_lane_binaries.sh — maintainer bridge that refreshes fkwu and
# form-cli platform bootstrap binaries from the canonical kernel checkout.
# Runtime and standard-lane receipts remain fkwu-native and toolchain-free.
set -euo pipefail

FORM="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$FORM"
export GO_BIN="$FORM/form-kernel-go/bin-go"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh

command -v clang >/dev/null 2>&1 || {
    printf '%s\n' 'maintainer regen requires clang; runtime/standard lane does not' >&2
    exit 1
}

slug="$(fourth_platform_slug)"
mkdir -p form-stdlib/bootstrap

FORM_STANDARD_LANE=0 build_fourth
fkwu_stamp="$(fourth_fkwu_cache_stamp)"
fkwu_out="$FOURTH_DIR/fkwu-$fkwu_stamp"
[[ -x "$fkwu_out" ]] || {
    printf '%s\n' 'fkwu build failed' >&2
    exit 1
}
cp "$fkwu_out" "form-stdlib/bootstrap/fkwu-${slug}"
printf '%s\n' "$fkwu_stamp" > "form-stdlib/bootstrap/fkwu-${slug}.stamp"
printf 'regen: fkwu-%s (%s bytes) stamp=%s\n' \
    "$slug" \
    "$(wc -c < "form-stdlib/bootstrap/fkwu-${slug}" | tr -d ' ')" \
    "$fkwu_stamp"

FORM_CLI_SRCS=(
    form-stdlib/fourth-shim.fk form-stdlib/core.fk form-stdlib/line-grammar.fk
    form-stdlib/str-byte-at.fk form-stdlib/sha256.fk form-stdlib/hex.fk
    form-stdlib/resource-port.fk form-stdlib/bml-native-interface-package-import.fk form-stdlib/hati-os-targets.fk
    form-stdlib/form-native-resource-interfaces.fk form-stdlib/form-fs.fk
    form-stdlib/storage-port.fk form-stdlib/host-kernel-carrier.fk
    form-stdlib/fnri-standin.fk form-stdlib/fnri-receipt.fk form-stdlib/http-client.fk
    form-stdlib/format-arith.fk form-stdlib/f16-decode.fk
    form-stdlib/q6k-dequant.fk form-stdlib/q4k-dequant.fk form-stdlib/weight-load.fk
    form-stdlib/voice-traits.fk form-stdlib/nearest-shape.fk
    form-stdlib/co-learning.fk form-stdlib/co-learning-stream.fk
    form-stdlib/mesh-dispatch.fk form-stdlib/surprise-salience.fk form-stdlib/host-sense-organ.fk
    form-stdlib/speech-organ.fk form-stdlib/native-host-instance.fk
    form-stdlib/text-tokenize.fk form-stdlib/rag-embed.fk
    form-stdlib/rag-index-codec.fk form-stdlib/rag-retrieve.fk
    form-stdlib/rag-ask.fk form-stdlib/form-cli-ask.fk
    form-stdlib/current-branch-landing.fk form-stdlib/form-cli.fk
    form-stdlib/form-cli-gguf-cell.fk form-stdlib/form-cli-repl.fk
)
form_cli_stamp="$(fourth_hash16 "${FORM_CLI_SRCS[@]}")"

FORM_STANDARD_LANE=0 ./build-form-cli.sh
[[ -x form-cli ]] || {
    printf '%s\n' 'form-cli build failed' >&2
    exit 1
}
cp form-cli "form-stdlib/bootstrap/form-cli-${slug}"
printf '%s\n' "$form_cli_stamp" > "form-stdlib/bootstrap/form-cli-${slug}.stamp"
printf 'regen: form-cli-%s (%s bytes) stamp=%s\n' \
    "$slug" \
    "$(wc -c < "form-stdlib/bootstrap/form-cli-${slug}" | tr -d ' ')" \
    "$form_cli_stamp"
