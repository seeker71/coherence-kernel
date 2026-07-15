#!/usr/bin/env bash
# regen_form_cli_bootstrap.sh — maintainer bridge that refreshes the committed
# form-cli table and emitted C carrier through the Go proof sibling.
# This carrier retires when the Form-native self-host path owns emission.
set -euo pipefail
export LC_ALL=C

FORM="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
GO_KERNEL="$FORM/form-kernel-go/bin-go"

# Rebuild the proof sibling so ignored local binaries never author a fresh
# bootstrap artifact from stale source.
(cd "$FORM/form-kernel-go" && go build -o bin-go .)

cd "$FORM"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
export GO_BIN="$GO_KERNEL"

FORM_CLI_SRCS=(
    form-stdlib/fourth-shim.fk form-stdlib/core.fk form-stdlib/line-grammar.fk
    form-stdlib/str-byte-at.fk form-stdlib/sha256.fk form-stdlib/hex.fk
    form-stdlib/resource-port.fk form-stdlib/bml-native-interface-package-import.fk form-stdlib/hati-os-targets.fk
    form-stdlib/form-native-resource-interfaces.fk form-stdlib/form-fs.fk
    form-stdlib/storage-port.fk form-stdlib/host-kernel-carrier.fk form-stdlib/fnri-standin.fk
    form-stdlib/fnri-receipt.fk form-stdlib/http-client.fk
    form-stdlib/format-arith.fk form-stdlib/f16-decode.fk form-stdlib/q6k-dequant.fk
    form-stdlib/q4k-dequant.fk form-stdlib/weight-load.fk
    form-stdlib/voice-traits.fk form-stdlib/nearest-shape.fk
    form-stdlib/co-learning.fk form-stdlib/co-learning-stream.fk form-stdlib/mesh-dispatch.fk
    form-stdlib/surprise-salience.fk form-stdlib/host-sense-organ.fk form-stdlib/speech-organ.fk
    form-stdlib/native-host-instance.fk form-stdlib/text-tokenize.fk form-stdlib/rag-embed.fk
    form-stdlib/rag-index-codec.fk form-stdlib/rag-retrieve.fk form-stdlib/rag-ask.fk
    form-stdlib/form-cli-ask.fk form-stdlib/current-branch-landing.fk form-stdlib/form-cli.fk
    form-stdlib/form-cli-gguf-cell.fk form-stdlib/form-cli-repl.fk
)

mkdir -p form-stdlib/bootstrap
work_dir="$(mktemp -d)"
trap 'rm -rf "$work_dir"' EXIT

source_cache="form-stdlib/.cache/source-compiled"
mkdir -p "$source_cache"
SOURCE_COMPILE_CHAIN=(
    form-stdlib/form-ontology-loader.fk
    form-stdlib/line-grammar.fk
    form-stdlib/bmf-core.fk
    form-stdlib/bmf-grammar.fk
    form-stdlib/bml.fk
    form-stdlib/bml-source.fk
    form-stdlib/source-compiler.fk
    form-stdlib/grammars/form-bml.fk
    form-stdlib/form-bml-lower.fk
    form-stdlib/source-compiler-text-lens.fk
)

compile_bml() {
    local src="$1" key cached out driver
    if ! grep -Eq '^[[:space:]]*section \[' "$src"; then
        printf '%s\n' "$src"
        return 0
    fi

    key="$(fourth_hash16 "$src" "${SOURCE_COMPILE_CHAIN[@]}" "$GO_KERNEL")"
    cached="$source_cache/$key.fk"
    if [[ ! -s "$cached" ]]; then
        out="$(mktemp "$source_cache/.tmp.XXXXXX")"
        driver="$work_dir/compile.fk"
        printf '(do (form-source-compile-file "%s" "%s"))\n' "$src" "$out" > "$driver"
        if "$GO_KERNEL" "${SOURCE_COMPILE_CHAIN[@]}" "$driver" \
            >/dev/null 2> "$work_dir/source-compile.err" && [[ -s "$out" ]]; then
            mv -f "$out" "$cached"
        else
            printf 'regen: failed to source-compile %s\n' "$src" >&2
            sed 's/^/  /' "$work_dir/source-compile.err" >&2
            rm -f "$out"
            exit 1
        fi
    fi
    printf '%s\n' "$cached"
}

printf '%s\n' 'regen: flatten bin-go (form-cli table)'
stdlib=form-stdlib
core_src="$(compile_bml "$stdlib/core.fk")"
http_client_src="$(compile_bml "$stdlib/http-client.fk")"
form_cli_ask_src="$(compile_bml "$stdlib/form-cli-ask.fk")"
modules="(list (read_file \"$stdlib/fourth-shim.fk\") (read_file \"$core_src\") (read_file \"$stdlib/resource-port.fk\") (read_file \"$stdlib/bml-native-interface-package-import.fk\") (read_file \"$stdlib/hati-os-targets.fk\") (read_file \"$stdlib/form-native-resource-interfaces.fk\") (read_file \"$stdlib/form-fs.fk\") (read_file \"$stdlib/storage-port.fk\") (read_file \"$stdlib/host-kernel-carrier.fk\") (read_file \"$stdlib/fnri-standin.fk\") (read_file \"$stdlib/fnri-receipt.fk\") (read_file \"$http_client_src\") (read_file \"$stdlib/line-grammar.fk\") (read_file \"$stdlib/str-byte-at.fk\") (read_file \"$stdlib/sha256.fk\") (read_file \"$stdlib/hex.fk\") (read_file \"$stdlib/format-arith.fk\") (read_file \"$stdlib/f16-decode.fk\") (read_file \"$stdlib/q6k-dequant.fk\") (read_file \"$stdlib/q4k-dequant.fk\") (read_file \"$stdlib/weight-load.fk\") (read_file \"$stdlib/voice-traits.fk\") (read_file \"$stdlib/nearest-shape.fk\") (read_file \"$stdlib/co-learning.fk\") (read_file \"$stdlib/co-learning-stream.fk\") (read_file \"$stdlib/mesh-dispatch.fk\") (read_file \"$stdlib/surprise-salience.fk\") (read_file \"$stdlib/host-sense-organ.fk\") (read_file \"$stdlib/speech-organ.fk\") (read_file \"$stdlib/native-host-instance.fk\") (read_file \"$stdlib/text-tokenize.fk\") (read_file \"$stdlib/rag-embed.fk\") (read_file \"$stdlib/rag-index-codec.fk\") (read_file \"$stdlib/rag-retrieve.fk\") (read_file \"$stdlib/rag-ask.fk\") (read_file \"$form_cli_ask_src\") (read_file \"$stdlib/current-branch-landing.fk\") (read_file \"$stdlib/form-cli.fk\") (read_file \"$stdlib/form-cli-gguf-cell.fk\"))"
band="(read_file \"$stdlib/form-cli-repl.fk\")"
FLATTEN_CHAIN=(
    form-stdlib/minimal-surface.fk
    form-stdlib/hati-os-kernel.fk
    form-stdlib/host-io-fs-fkwu-emit.fk
    form-stdlib/fkc-table-serialize.fk
    form-stdlib/hati-os-kernel-emit.fk
    form-stdlib/core.fk
    form-stdlib/form-parse.fk
    form-stdlib/bmf-core.fk
    form-stdlib/bmf-grammar.fk
    form-stdlib/host-effect-grammar.fk
    form-stdlib/form-flatten.fk
)

printf '(fks-table-file (flt-band-sources-fns %s %s) (flt-band-sources-pool %s %s))\n' \
    "$modules" "$band" "$modules" "$band" > "$work_dir/flatten.fk"
"$GO_KERNEL" "${FLATTEN_CHAIN[@]}" "$work_dir/flatten.fk" \
    > form-stdlib/bootstrap/form-cli-table.txt
[[ -s form-stdlib/bootstrap/form-cli-table.txt ]] || {
    printf '%s\n' 'regen: form-cli table is empty' >&2
    exit 1
}
fourth_hash16 "${FORM_CLI_SRCS[@]}" > form-stdlib/bootstrap/form-cli.stamp

EMIT_CHAIN=(
    form-stdlib/minimal-surface.fk
    form-stdlib/hati-os-kernel.fk
    form-stdlib/host-io-fs-fkwu-emit.fk
    form-stdlib/fkc-table-serialize.fk
    form-stdlib/hati-os-kernel-emit.fk
)
printf '(fkc-emit-combined-repl "%s")\n' \
    "$(cat form-stdlib/bootstrap/form-cli-table.txt)" > "$work_dir/emit.fk"
"$GO_KERNEL" "${EMIT_CHAIN[@]}" "$work_dir/emit.fk" \
    > form-stdlib/bootstrap/form-cli-emitted.c
[[ -s form-stdlib/bootstrap/form-cli-emitted.c ]] || {
    printf '%s\n' 'regen: emitted form-cli C is empty' >&2
    exit 1
}
grep -q 'fk_prog' form-stdlib/bootstrap/form-cli-emitted.c || {
    printf '%s\n' 'regen: emitted form-cli C is missing its baked program' >&2
    exit 1
}

printf 'regen: form-cli-emitted.c (%s bytes) stamp=%s\n' \
    "$(wc -c < form-stdlib/bootstrap/form-cli-emitted.c | tr -d ' ')" \
    "$(cat form-stdlib/bootstrap/form-cli.stamp)"
