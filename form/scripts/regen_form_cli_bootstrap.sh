#!/usr/bin/env bash
# regen_form_cli_bootstrap.sh — refresh the committed form-cli table and emitted
# C carrier.  The Rust and TypeScript proof siblings can author the same
# flattened table without the Go sibling's larger peak on memory-tight hosts;
# the Form-native fkwu self-host remains the destination carrier.
set -euo pipefail
export LC_ALL=C

FORM="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
GO_KERNEL="$FORM/form-kernel-go/bin-go"
RS_KERNEL="$FORM/form-kernel-rust/target/release/form-kernel-rust"
TS_KERNEL="$FORM/form-kernel-ts/dist/main.mjs"

# Rebuild the proof sibling so ignored local binaries never author a fresh
# bootstrap artifact from stale source.
(cd "$FORM/form-kernel-go" && go build -o bin-go .)

cd "$FORM"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
# shellcheck source=scripts/form_cli_bootstrap_proof.sh
source scripts/form_cli_bootstrap_proof.sh
export GO_BIN="$GO_KERNEL"

FORM_CLI_SRCS=(
    form-stdlib/fourth-shim.fk form-stdlib/core.fk form-stdlib/grammars/sanskrit-roots.fk form-stdlib/line-grammar.fk
    form-stdlib/str-byte-at.fk form-stdlib/sha256.fk form-stdlib/hmac-sha256.fk form-stdlib/hex.fk
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
    form-stdlib/form-cli-ask.fk form-stdlib/form-cli-router.fk form-stdlib/form-cli-judge.fk
    form-stdlib/confidence-weighted-vote.fk form-stdlib/lineage-discounted-vote.fk
    form-stdlib/form-cli-oracle-loop.fk
    form-stdlib/form-cli-sufficiency.fk form-stdlib/form-freq-check.fk
    form-stdlib/trust-row.fk form-stdlib/form-cli-ask-gate.fk
    form-stdlib/form-cli-staged-trace.fk form-stdlib/form-cli-request.fk
    form-stdlib/form-cli-carrier.fk form-stdlib/form-cli-ask-plus.fk form-stdlib/form-cli-surface-inquiry.fk
    form-stdlib/current-branch-landing.fk form-stdlib/form-cli-inquiry.fk form-stdlib/form-cli.fk
    form-stdlib/form-cli-gguf-cell.fk form-stdlib/form-cli-repl.fk
)

mkdir -p form-stdlib/bootstrap
work_dir="$(mktemp -d)"
trap 'rm -rf "$work_dir"' EXIT

want_cli_stamp="$(fourth_hash16 "${FORM_CLI_SRCS[@]}")"
want_source_sha256="$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")"
carrier_src="$work_dir/form-cli-carrier.fk"
sed "s/FORM_CLI_SOURCE_SHA256_PLACEHOLDER/$want_source_sha256/g" \
    form-stdlib/form-cli-carrier.fk > "$carrier_src"

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

stamp="$(fourth_fkwu_cache_stamp)"
cached_fkwu="$FOURTH_DIR/fkwu-$stamp"
if [[ -x "$cached_fkwu" ]]; then
    FKWU="$cached_fkwu"
else
    build_fourth >/dev/null 2>&1 || true
fi

# Self-host flatten list — mirrors the $modules order below EXACTLY so every
# arm flattens the same program. fourth_band_request prepends the shim itself,
# so it is NOT listed here: the old list started from FORM_CLI_SRCS (stamp
# order, shim first) and shipped a double-ridden shim (+77 duplicate fn rows)
# in a different module order than the Rust/TS arm. The band
# (form-cli-repl.fk) rides last.
FORM_CLI_SELFHOST_ORDER=(
    form-stdlib/core.fk form-stdlib/grammars/sanskrit-roots.fk form-stdlib/resource-port.fk
    form-stdlib/bml-native-interface-package-import.fk form-stdlib/hati-os-targets.fk
    form-stdlib/form-native-resource-interfaces.fk form-stdlib/form-fs.fk
    form-stdlib/storage-port.fk form-stdlib/host-kernel-carrier.fk
    form-stdlib/fnri-standin.fk form-stdlib/fnri-receipt.fk
    form-stdlib/http-client.fk form-stdlib/line-grammar.fk form-stdlib/str-byte-at.fk
    form-stdlib/sha256.fk form-stdlib/hmac-sha256.fk form-stdlib/hex.fk
    form-stdlib/format-arith.fk form-stdlib/f16-decode.fk form-stdlib/q6k-dequant.fk
    form-stdlib/q4k-dequant.fk form-stdlib/weight-load.fk
    form-stdlib/voice-traits.fk form-stdlib/nearest-shape.fk
    form-stdlib/co-learning.fk form-stdlib/co-learning-stream.fk form-stdlib/mesh-dispatch.fk
    form-stdlib/surprise-salience.fk form-stdlib/host-sense-organ.fk form-stdlib/speech-organ.fk
    form-stdlib/native-host-instance.fk form-stdlib/text-tokenize.fk form-stdlib/rag-embed.fk
    form-stdlib/rag-index-codec.fk form-stdlib/rag-retrieve.fk form-stdlib/rag-ask.fk
    form-stdlib/form-cli-ask.fk form-stdlib/form-cli-router.fk form-stdlib/form-cli-judge.fk
    form-stdlib/confidence-weighted-vote.fk form-stdlib/lineage-discounted-vote.fk
    form-stdlib/form-cli-oracle-loop.fk
    form-stdlib/form-cli-sufficiency.fk form-stdlib/form-freq-check.fk
    form-stdlib/trust-row.fk form-stdlib/form-cli-ask-gate.fk
    form-stdlib/form-cli-staged-trace.fk form-stdlib/form-cli-request.fk
    form-stdlib/form-cli-carrier.fk form-stdlib/form-cli-ask-plus.fk form-stdlib/form-cli-surface-inquiry.fk
    form-stdlib/current-branch-landing.fk form-stdlib/form-cli-inquiry.fk form-stdlib/form-cli.fk
    form-stdlib/form-cli-gguf-cell.fk
)
FORM_CLI_FLATTEN_SRCS=()
for src in "${FORM_CLI_SELFHOST_ORDER[@]}"; do
    if [[ "$src" == "form-stdlib/form-cli-carrier.fk" ]]; then
        FORM_CLI_FLATTEN_SRCS+=("$carrier_src")
    else
        FORM_CLI_FLATTEN_SRCS+=("$(compile_bml "$src")")
    fi
done
FORM_CLI_FLATTEN_SRCS+=(form-stdlib/form-cli-repl.fk)

stdlib=form-stdlib
core_src="$(compile_bml "$stdlib/core.fk")"
http_client_src="$(compile_bml "$stdlib/http-client.fk")"
form_cli_ask_src="$(compile_bml "$stdlib/form-cli-ask.fk")"
modules="(list (read_file \"$stdlib/fourth-shim.fk\") (read_file \"$core_src\") (read_file \"$stdlib/grammars/sanskrit-roots.fk\") (read_file \"$stdlib/resource-port.fk\") (read_file \"$stdlib/bml-native-interface-package-import.fk\") (read_file \"$stdlib/hati-os-targets.fk\") (read_file \"$stdlib/form-native-resource-interfaces.fk\") (read_file \"$stdlib/form-fs.fk\") (read_file \"$stdlib/storage-port.fk\") (read_file \"$stdlib/host-kernel-carrier.fk\") (read_file \"$stdlib/fnri-standin.fk\") (read_file \"$stdlib/fnri-receipt.fk\") (read_file \"$http_client_src\") (read_file \"$stdlib/line-grammar.fk\") (read_file \"$stdlib/str-byte-at.fk\") (read_file \"$stdlib/sha256.fk\") (read_file \"$stdlib/hmac-sha256.fk\") (read_file \"$stdlib/hex.fk\") (read_file \"$stdlib/format-arith.fk\") (read_file \"$stdlib/f16-decode.fk\") (read_file \"$stdlib/q6k-dequant.fk\") (read_file \"$stdlib/q4k-dequant.fk\") (read_file \"$stdlib/weight-load.fk\") (read_file \"$stdlib/voice-traits.fk\") (read_file \"$stdlib/nearest-shape.fk\") (read_file \"$stdlib/co-learning.fk\") (read_file \"$stdlib/co-learning-stream.fk\") (read_file \"$stdlib/mesh-dispatch.fk\") (read_file \"$stdlib/surprise-salience.fk\") (read_file \"$stdlib/host-sense-organ.fk\") (read_file \"$stdlib/speech-organ.fk\") (read_file \"$stdlib/native-host-instance.fk\") (read_file \"$stdlib/text-tokenize.fk\") (read_file \"$stdlib/rag-embed.fk\") (read_file \"$stdlib/rag-index-codec.fk\") (read_file \"$stdlib/rag-retrieve.fk\") (read_file \"$stdlib/rag-ask.fk\") (read_file \"$form_cli_ask_src\") (read_file \"$stdlib/form-cli-router.fk\") (read_file \"$stdlib/form-cli-judge.fk\") (read_file \"$stdlib/confidence-weighted-vote.fk\") (read_file \"$stdlib/lineage-discounted-vote.fk\") (read_file \"$stdlib/form-cli-oracle-loop.fk\") (read_file \"$stdlib/form-cli-sufficiency.fk\") (read_file \"$stdlib/form-freq-check.fk\") (read_file \"$stdlib/trust-row.fk\") (read_file \"$stdlib/form-cli-ask-gate.fk\") (read_file \"$stdlib/form-cli-staged-trace.fk\") (read_file \"$stdlib/form-cli-request.fk\") (read_file \"$carrier_src\") (read_file \"$stdlib/form-cli-ask-plus.fk\") (read_file \"$stdlib/form-cli-surface-inquiry.fk\") (read_file \"$stdlib/current-branch-landing.fk\") (read_file \"$stdlib/form-cli-inquiry.fk\") (read_file \"$stdlib/form-cli.fk\") (read_file \"$stdlib/form-cli-gguf-cell.fk\"))"
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

table_tmp="$work_dir/form-cli-table.txt"
printf '(fks-table-file (flt-band-sources-fns %s %s) (flt-band-sources-pool %s %s))\n' \
    "$modules" "$band" "$modules" "$band" > "$work_dir/flatten.fk"

flatten_candidate="$work_dir/form-cli-table.candidate"
flatten_err="$work_dir/form-cli-flatten.err"
if [[ -x "$RS_KERNEL" ]] \
        && "$RS_KERNEL" "${FLATTEN_CHAIN[@]}" "$work_dir/flatten.fk" \
            > "$flatten_candidate" 2> "$flatten_err" \
        && form_cli_validate_table "$flatten_candidate" >/dev/null; then
    mv -f "$flatten_candidate" "$table_tmp"
    printf '%s\n' 'regen: flatten Rust proof sibling (form-cli table)'
elif [[ -f "$TS_KERNEL" ]] \
        && node "$TS_KERNEL" \
            "${FLATTEN_CHAIN[@]}" "$work_dir/flatten.fk" \
            > "$flatten_candidate" 2> "$flatten_err" \
        && form_cli_validate_table "$flatten_candidate" >/dev/null; then
    mv -f "$flatten_candidate" "$table_tmp"
    printf '%s\n' 'regen: flatten TypeScript proof sibling (form-cli table)'
elif fourth_selfhost && fourth_flatten_sources \
        form-cli-bootstrap fks "$flatten_candidate" "${FORM_CLI_FLATTEN_SRCS[@]}" \
        && form_cli_validate_table "$flatten_candidate" >/dev/null; then
    mv -f "$flatten_candidate" "$table_tmp"
    printf '%s\n' 'regen: flatten fkwu self-host (form-cli table)'
else
    printf '%s\n' 'regen: bounded-memory flatten carriers failed' >&2
    sed 's/^/  /' "$flatten_err" >&2
    exit 1
fi
[[ -s "$table_tmp" ]] || {
    printf '%s\n' 'regen: form-cli table is empty' >&2
    exit 1
}
table_shape="$(form_cli_validate_table "$table_tmp")"

# Voice canary — the carrier must ANSWER, not merely validate. Walk the
# candidate table on the cached fkwu and expect pong: an aphonic table
# (receipts/2026-07-17-regen-lane-aphonic-carrier.md) dies here instead of
# shipping. Shape validation alone cannot catch a table that runs mute.
if [[ -n "${FKWU:-}" && -x "${FKWU:-}" ]]; then
    voice="$(printf 'ping\n' | "$FKWU" "$table_tmp" 0 2>/dev/null | sed -n '1p')"
    if [[ "$voice" != "pong" ]]; then
        printf "regen: voice canary failed — ping answered '%s', not pong (aphonic carrier)\n" "$voice" >&2
        exit 1
    fi
    printf '%s\n' 'regen: voice canary — ping answers pong'
else
    printf '%s\n' 'regen: WARNING voice canary skipped (no cached fkwu on this host)' >&2
fi
stamp_tmp="$work_dir/form-cli.stamp"
printf '%s\n' "$want_cli_stamp" > "$stamp_tmp"
source_digest_tmp="$work_dir/form-cli.source.sha256"
printf '%s\n' "$want_source_sha256" > "$source_digest_tmp"

EMIT_CHAIN=(
    form-stdlib/minimal-surface.fk
    form-stdlib/hati-os-kernel.fk
    form-stdlib/host-io-fs-fkwu-emit.fk
    form-stdlib/fkc-table-serialize.fk
    form-stdlib/hati-os-kernel-emit.fk
)
printf '(fkc-emit-combined-repl "%s")\n' \
    "$(cat "$table_tmp")" > "$work_dir/emit.fk"
emitted_tmp="$work_dir/form-cli-emitted.c"
"$GO_KERNEL" "${EMIT_CHAIN[@]}" "$work_dir/emit.fk" > "$emitted_tmp"
[[ -s "$emitted_tmp" ]] || {
    printf '%s\n' 'regen: emitted form-cli C is empty' >&2
    exit 1
}
grep -q 'fk_prog' "$emitted_tmp" || {
    printf '%s\n' 'regen: emitted form-cli C is missing its baked program' >&2
    exit 1
}
form_cli_verify_bootstrap "$table_tmp" "$emitted_tmp" "$stamp_tmp" "$want_cli_stamp"
form_cli_verify_source_digest "$source_digest_tmp" "$want_source_sha256"

# Publish the stamp last.  Readers either see the prior coherent carrier or a
# stale stamp while the two payloads move; they never accept a mixed carrier.
mv -f "$emitted_tmp" form-stdlib/bootstrap/form-cli-emitted.c
mv -f "$table_tmp" form-stdlib/bootstrap/form-cli-table.txt
mv -f "$source_digest_tmp" form-stdlib/bootstrap/form-cli.source.sha256
mv -f "$stamp_tmp" form-stdlib/bootstrap/form-cli.stamp

printf 'regen: form-cli-emitted.c (%s bytes) stamp=%s %s\n' \
    "$(wc -c < form-stdlib/bootstrap/form-cli-emitted.c | tr -d ' ')" \
    "$(cat form-stdlib/bootstrap/form-cli.stamp)" "$table_shape"
