#!/usr/bin/env bash
# regen_form_cli_bootstrap.sh — refresh the committed form-cli table and emitted
# C carrier.  Both transformations are walked by the local Form-native fkwu:
# no proof sibling and no JavaScript generator enters the publication path.
set -euo pipefail

FORM="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SOURCE_FKWU="$FORM/../fkwu"

[[ -x "$SOURCE_FKWU" ]] || {
    printf 'regen: source-running fkwu is missing: %s\n' "$SOURCE_FKWU" >&2
    exit 1
}

cd "$FORM"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
# shellcheck source=scripts/form_cli_bootstrap_proof.sh
source scripts/form_cli_bootstrap_proof.sh

FORM_CLI_SRCS=(
    form-stdlib/fourth-shim.fk form-stdlib/core.fk form-stdlib/grammars/sanskrit-roots.fk form-stdlib/line-grammar.fk
    form-stdlib/str-byte-at.fk form-stdlib/sha256.fk form-stdlib/hmac-sha256.fk form-stdlib/hex.fk
    form-stdlib/resource-port.fk form-stdlib/bml-native-interface-package-import.fk form-stdlib/hati-os-targets.fk
    form-stdlib/form-native-resource-interfaces.fk form-stdlib/form-fs.fk
    form-stdlib/storage-port.fk form-stdlib/host-kernel-carrier.fk form-stdlib/fnri-standin.fk
    form-stdlib/fnri-receipt.fk form-stdlib/http-client.fk
    form-stdlib/format-arith.fk form-stdlib/f16-decode.fk form-stdlib/q6k-dequant.fk form-stdlib/equireach.fk form-stdlib/equireach-gguf.fk form-stdlib/gguf-meta.fk form-stdlib/model-discovery.fk
    form-stdlib/q4k-dequant.fk form-stdlib/weight-load.fk
    form-stdlib/q6k-msl.fk form-stdlib/q4k-msl.fk form-stdlib/q8-0-msl.fk form-stdlib/q5-msl.fk
    form-stdlib/transformer-numerics.fk form-stdlib/transformer-block.fk form-stdlib/llama-numerics.fk form-stdlib/trig.fk
    form-stdlib/tensor-ir.fk form-stdlib/jit-tensor-emit.fk form-stdlib/llama-decode-msl.fk form-stdlib/qk-matvec-split.fk
    form-stdlib/qk-matvec-lane.fk form-stdlib/metal-door.fk form-stdlib/llama3-detokenize.fk form-stdlib/keyed-map.fk form-stdlib/dsv4-tokenizer.fk
    form-stdlib/mxfp4-plane-dequant.fk form-stdlib/mxfp8-plane-dequant.fk form-stdlib/gguf-manifest.fk form-stdlib/iq2xxs-dequant.fk form-stdlib/q2k-dequant.fk
    form-stdlib/mxfp4-msl.fk form-stdlib/mxfp8-msl.fk form-stdlib/iq2xxs-msl.fk form-stdlib/q2k-msl.fk
    form-stdlib/rope.fk form-stdlib/transformer-mh.fk form-stdlib/mla-attn.fk form-stdlib/dsv4-hc.fk form-stdlib/dsv4-forward.fk form-stdlib/dsv4-router-msl.fk form-stdlib/dsv4-moe-msl.fk
    form-stdlib/dsv4-kv-cache.fk form-stdlib/dsv4-compressor.fk form-stdlib/ds4-order-match.fk form-stdlib/mla-msl.fk form-stdlib/dsv4-hc-msl.fk
    form-stdlib/dsv4-validation-protocol.fk form-stdlib/dsv4-validation-calibrator.fk
    native/metal/windowed-residency.fk native/metal/windowed-residency-emit.fk
    native/metal/dsv4-token.fk native/metal/dsv4-mla-real.fk native/metal/dsv4-forward-real.fk native/metal/dsv4-layer-real.fk
    native/metal/dsv4-stack-real.fk native/metal/dsv4-decode-form.fk native/metal/dsv4-decode-stack.fk native/metal/dsv4-validation-live.fk
    native/metal/dense-token-handle.fk
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
    form-stdlib/current-branch-landing.fk form-stdlib/form-cli-inquiry.fk form-stdlib/relational-inquiry-metabolism.fk form-stdlib/native-model-native-hierarchy.fk form-stdlib/ds4-query-channel.fk form-stdlib/form-cli.fk
    form-stdlib/native-model-control-plane.fk form-stdlib/ask-lane-router.fk
    form-stdlib/form-cli-gguf-cell.fk form-stdlib/form-cli-repl.fk
)

mkdir -p form-stdlib/bootstrap
# Work files are private, but the final carrier paths are shared.  Serialize
# publishers so a second regeneration cannot replace a coherent carrier with
# an older candidate between validation and its final stamp publication.
regen_lock_dir="form-stdlib/bootstrap/.regen-form-cli.lock"
regen_lock_owner="$regen_lock_dir/owner.pid"
if ! mkdir "$regen_lock_dir" 2>/dev/null; then
    regen_lock_pid=""
    if [[ -r "$regen_lock_owner" ]]; then
        IFS= read -r regen_lock_pid < "$regen_lock_owner" || true
    fi
    if [[ "$regen_lock_pid" =~ ^[0-9]+$ ]] && kill -0 "$regen_lock_pid" 2>/dev/null; then
        printf 'regen: another form-cli regeneration owns %s (pid %s)\n' \
            "$regen_lock_dir" "$regen_lock_pid" >&2
        exit 75
    fi
    rm -f "$regen_lock_owner"
    rmdir "$regen_lock_dir" 2>/dev/null || {
        printf 'regen: cannot reclaim stale form-cli regeneration lock %s\n' \
            "$regen_lock_dir" >&2
        exit 75
    }
    mkdir "$regen_lock_dir" || {
        printf 'regen: cannot acquire form-cli regeneration lock %s\n' \
            "$regen_lock_dir" >&2
        exit 75
    }
fi
printf '%s\n' "$$" > "$regen_lock_owner"
work_dir="$(mktemp -d)"
cleanup() {
    rm -rf "$work_dir"
    rm -f "$regen_lock_owner"
    rmdir "$regen_lock_dir" 2>/dev/null || true
}
trap cleanup EXIT INT TERM

# Program identity includes both its source and the self-hosted compiler image
# that lowered it. Source-only identity reused a table whose flattened lets
# predated evaluate-once semantics even after T_flat itself had been healed.
want_cli_stamp="$(fourth_hash16 "${FORM_CLI_SRCS[@]}" form-stdlib/fourth-flatten-table.stamp)"
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
    local src="$1" key lowered cached out driver
    if ! grep -Eq '^[[:space:]]*section \[' "$src"; then
        printf '%s\n' "$src"
        return 0
    fi

    key="$(fourth_hash16 "$src" "${SOURCE_COMPILE_CHAIN[@]}")"
    # The lowered Form body is a content-addressed bootstrap cell.  Its name
    # includes both the BML source and compiler-chain digest, so editing either
    # makes it ineligible instead of silently reusing stale meaning.  It is
    # executed by fkwu like every other module; no sibling runtime is involved.
    lowered="form-stdlib/bootstrap/lowered/$key.fk"
    if [[ -s "$lowered" ]]; then
        printf '%s\n' "$lowered"
        return 0
    fi
    cached="$source_cache/$key.fk"
    if [[ ! -s "$cached" ]]; then
        out="$(mktemp "$source_cache/.tmp.XXXXXX")"
        driver="$work_dir/compile.fk"
        printf '(do (form-source-compile-file "%s" "%s"))\n' "$src" "$out" > "$driver"
        if "$SOURCE_FKWU" "${SOURCE_COMPILE_CHAIN[@]}" "$driver" \
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
    form-stdlib/format-arith.fk form-stdlib/f16-decode.fk form-stdlib/q6k-dequant.fk form-stdlib/equireach.fk form-stdlib/equireach-gguf.fk form-stdlib/gguf-meta.fk form-stdlib/model-discovery.fk
    form-stdlib/q4k-dequant.fk form-stdlib/weight-load.fk
    form-stdlib/q6k-msl.fk form-stdlib/q4k-msl.fk form-stdlib/q8-0-msl.fk form-stdlib/q5-msl.fk
    form-stdlib/transformer-numerics.fk form-stdlib/transformer-block.fk form-stdlib/llama-numerics.fk form-stdlib/trig.fk
    form-stdlib/tensor-ir.fk form-stdlib/jit-tensor-emit.fk form-stdlib/llama-decode-msl.fk form-stdlib/qk-matvec-split.fk
    form-stdlib/qk-matvec-lane.fk form-stdlib/metal-door.fk form-stdlib/llama3-detokenize.fk form-stdlib/keyed-map.fk form-stdlib/dsv4-tokenizer.fk
    form-stdlib/mxfp4-plane-dequant.fk form-stdlib/mxfp8-plane-dequant.fk form-stdlib/gguf-manifest.fk form-stdlib/iq2xxs-dequant.fk form-stdlib/q2k-dequant.fk
    form-stdlib/mxfp4-msl.fk form-stdlib/mxfp8-msl.fk form-stdlib/iq2xxs-msl.fk form-stdlib/q2k-msl.fk
    form-stdlib/rope.fk form-stdlib/transformer-mh.fk form-stdlib/mla-attn.fk form-stdlib/dsv4-hc.fk form-stdlib/dsv4-forward.fk form-stdlib/dsv4-router-msl.fk form-stdlib/dsv4-moe-msl.fk
    form-stdlib/dsv4-kv-cache.fk form-stdlib/dsv4-compressor.fk form-stdlib/ds4-order-match.fk form-stdlib/mla-msl.fk form-stdlib/dsv4-hc-msl.fk
    form-stdlib/dsv4-validation-protocol.fk form-stdlib/dsv4-validation-calibrator.fk
    native/metal/windowed-residency.fk native/metal/windowed-residency-emit.fk
    native/metal/dsv4-token.fk native/metal/dsv4-mla-real.fk native/metal/dsv4-forward-real.fk native/metal/dsv4-layer-real.fk
    native/metal/dsv4-stack-real.fk native/metal/dsv4-decode-form.fk native/metal/dsv4-decode-stack.fk native/metal/dsv4-validation-live.fk
    native/metal/dense-token-handle.fk
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
    form-stdlib/current-branch-landing.fk form-stdlib/form-cli-inquiry.fk form-stdlib/ds4-query-channel.fk form-stdlib/form-cli.fk
    form-stdlib/form-cli-gguf-cell.fk form-stdlib/relational-inquiry-metabolism.fk form-stdlib/native-model-native-hierarchy.fk
    form-stdlib/native-model-control-plane.fk form-stdlib/ask-lane-router.fk
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
# $modules below is THE list that becomes the program. FORM_CLI_SRCS only
# authors the stamp and the source digest; FORM_CLI_SELFHOST_ORDER is the
# fallback arm that runs only when both Rust and TypeScript are absent. On any
# host carrying form-kernel-rust, the Rust branch fires first and flattens
# $modules — so a cell added to the other two lists and not to this one lands
# in the stamp, in the genesis text, and in `form-cli source`, while the
# program cannot call a single one of its recipes. That is how equireach.fk,
# equireach-gguf.fk, gguf-meta.fk and model-discovery.fk arrived on 2026-08-03
# as 52 KB of quotable text and zero callable functions: the `functions=` count
# printed at the end moved by 1 (the band's own fc-models) where ~100 were due.
# Read that number as the witness.
#
# SIX lists, not four — the count grew on 2026-08-03 when ask-lane-router.fk was
# wired in. build-form-cli.sh carries two more of its own: FORM_CLI_SRCS (~:73)
# authors the stamp the build compares against the one THIS script wrote, and
# FORM_CLI_SELFHOST_SRCS (~:160) is its no-Go flatten arm. Mirror all six, and
# mirror the ORDER too: the stamp is a hash over the list, so the same files in
# a different place hash differently and the build stops with "source stamp
# stale" naming two hashes and no file. That message is a mirroring gap, not a
# stale tree. Six lists, one program: keep them mirrored.
core_src="$(compile_bml "$stdlib/core.fk")"
http_client_src="$(compile_bml "$stdlib/http-client.fk")"
form_cli_ask_src="$(compile_bml "$stdlib/form-cli-ask.fk")"
modules="(list (read_file \"$stdlib/fourth-shim.fk\") (read_file \"$core_src\") (read_file \"$stdlib/grammars/sanskrit-roots.fk\") (read_file \"$stdlib/resource-port.fk\") (read_file \"$stdlib/bml-native-interface-package-import.fk\") (read_file \"$stdlib/hati-os-targets.fk\") (read_file \"$stdlib/form-native-resource-interfaces.fk\") (read_file \"$stdlib/form-fs.fk\") (read_file \"$stdlib/storage-port.fk\") (read_file \"$stdlib/host-kernel-carrier.fk\") (read_file \"$stdlib/fnri-standin.fk\") (read_file \"$stdlib/fnri-receipt.fk\") (read_file \"$http_client_src\") (read_file \"$stdlib/line-grammar.fk\") (read_file \"$stdlib/str-byte-at.fk\") (read_file \"$stdlib/sha256.fk\") (read_file \"$stdlib/hmac-sha256.fk\") (read_file \"$stdlib/hex.fk\") (read_file \"$stdlib/format-arith.fk\") (read_file \"$stdlib/f16-decode.fk\") (read_file \"$stdlib/q6k-dequant.fk\") (read_file \"$stdlib/equireach.fk\") (read_file \"$stdlib/equireach-gguf.fk\") (read_file \"$stdlib/gguf-meta.fk\") (read_file \"$stdlib/model-discovery.fk\") (read_file \"$stdlib/q4k-dequant.fk\") (read_file \"$stdlib/weight-load.fk\") (read_file \"$stdlib/q6k-msl.fk\") (read_file \"$stdlib/q4k-msl.fk\") (read_file \"$stdlib/q8-0-msl.fk\") (read_file \"$stdlib/q5-msl.fk\") (read_file \"$stdlib/transformer-numerics.fk\") (read_file \"$stdlib/transformer-block.fk\") (read_file \"$stdlib/llama-numerics.fk\") (read_file \"$stdlib/trig.fk\") (read_file \"$stdlib/tensor-ir.fk\") (read_file \"$stdlib/jit-tensor-emit.fk\") (read_file \"$stdlib/llama-decode-msl.fk\") (read_file \"$stdlib/qk-matvec-split.fk\") (read_file \"$stdlib/qk-matvec-lane.fk\") (read_file \"$stdlib/metal-door.fk\") (read_file \"$stdlib/llama3-detokenize.fk\") (read_file \"$stdlib/keyed-map.fk\") (read_file \"$stdlib/dsv4-tokenizer.fk\") (read_file \"native/metal/dense-token-handle.fk\") (read_file \"$stdlib/voice-traits.fk\") (read_file \"$stdlib/nearest-shape.fk\") (read_file \"$stdlib/co-learning.fk\") (read_file \"$stdlib/co-learning-stream.fk\") (read_file \"$stdlib/mesh-dispatch.fk\") (read_file \"$stdlib/surprise-salience.fk\") (read_file \"$stdlib/host-sense-organ.fk\") (read_file \"$stdlib/speech-organ.fk\") (read_file \"$stdlib/native-host-instance.fk\") (read_file \"$stdlib/text-tokenize.fk\") (read_file \"$stdlib/rag-embed.fk\") (read_file \"$stdlib/rag-index-codec.fk\") (read_file \"$stdlib/rag-retrieve.fk\") (read_file \"$stdlib/rag-ask.fk\") (read_file \"$form_cli_ask_src\") (read_file \"$stdlib/form-cli-router.fk\") (read_file \"$stdlib/form-cli-judge.fk\") (read_file \"$stdlib/confidence-weighted-vote.fk\") (read_file \"$stdlib/lineage-discounted-vote.fk\") (read_file \"$stdlib/form-cli-oracle-loop.fk\") (read_file \"$stdlib/form-cli-sufficiency.fk\") (read_file \"$stdlib/form-freq-check.fk\") (read_file \"$stdlib/trust-row.fk\") (read_file \"$stdlib/form-cli-ask-gate.fk\") (read_file \"$stdlib/form-cli-staged-trace.fk\") (read_file \"$stdlib/form-cli-request.fk\") (read_file \"$carrier_src\") (read_file \"$stdlib/form-cli-ask-plus.fk\") (read_file \"$stdlib/form-cli-surface-inquiry.fk\") (read_file \"$stdlib/current-branch-landing.fk\") (read_file \"$stdlib/form-cli-inquiry.fk\") (read_file \"$stdlib/form-cli.fk\") (read_file \"$stdlib/form-cli-gguf-cell.fk\"))"
modules="${modules%)} (read_file \"$stdlib/relational-inquiry-metabolism.fk\") (read_file \"$stdlib/native-model-native-hierarchy.fk\") (read_file \"$stdlib/native-model-control-plane.fk\") (read_file \"$stdlib/ask-lane-router.fk\"))"
modules="${modules/ (read_file \"$stdlib\/form-cli.fk\")/ (read_file \"$stdlib\/ds4-query-channel.fk\") (read_file \"$stdlib\/form-cli.fk\")}"
modules="${modules%)} (read_file \"$stdlib/mxfp4-plane-dequant.fk\") (read_file \"$stdlib/mxfp8-plane-dequant.fk\") (read_file \"$stdlib/gguf-manifest.fk\") (read_file \"$stdlib/iq2xxs-dequant.fk\") (read_file \"$stdlib/q2k-dequant.fk\") (read_file \"$stdlib/mxfp4-msl.fk\") (read_file \"$stdlib/mxfp8-msl.fk\") (read_file \"$stdlib/iq2xxs-msl.fk\") (read_file \"$stdlib/q2k-msl.fk\") (read_file \"$stdlib/rope.fk\") (read_file \"$stdlib/transformer-mh.fk\") (read_file \"$stdlib/mla-attn.fk\") (read_file \"$stdlib/dsv4-hc.fk\") (read_file \"$stdlib/dsv4-forward.fk\") (read_file \"$stdlib/dsv4-router-msl.fk\") (read_file \"$stdlib/dsv4-moe-msl.fk\") (read_file \"$stdlib/dsv4-kv-cache.fk\") (read_file \"$stdlib/dsv4-compressor.fk\") (read_file \"$stdlib/ds4-order-match.fk\") (read_file \"$stdlib/mla-msl.fk\") (read_file \"$stdlib/dsv4-hc-msl.fk\") (read_file \"$stdlib/dsv4-validation-protocol.fk\") (read_file \"$stdlib/dsv4-validation-calibrator.fk\") (read_file \"native/metal/windowed-residency.fk\") (read_file \"native/metal/windowed-residency-emit.fk\") (read_file \"native/metal/dsv4-token.fk\") (read_file \"native/metal/dsv4-mla-real.fk\") (read_file \"native/metal/dsv4-forward-real.fk\") (read_file \"native/metal/dsv4-layer-real.fk\") (read_file \"native/metal/dsv4-stack-real.fk\") (read_file \"native/metal/dsv4-decode-form.fk\") (read_file \"native/metal/dsv4-decode-stack.fk\") (read_file \"native/metal/dsv4-validation-live.fk\"))"
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
if [[ "$(cat form-stdlib/bootstrap/form-cli.stamp 2>/dev/null || true)" == "$want_cli_stamp" ]] \
        && [[ "$(cat form-stdlib/bootstrap/form-cli.source.sha256 2>/dev/null || true)" == "$want_source_sha256" ]] \
        && form_cli_validate_table form-stdlib/bootstrap/form-cli-table.txt >/dev/null; then
    cp form-stdlib/bootstrap/form-cli-table.txt "$table_tmp"
    printf '%s\n' 'regen: reuse content-addressed fkwu table (source and shape match)'
elif fourth_selfhost && fourth_flatten_sources \
        form-cli-bootstrap fks "$flatten_candidate" "${FORM_CLI_FLATTEN_SRCS[@]}" \
        && form_cli_validate_table "$flatten_candidate" >/dev/null; then
    mv -f "$flatten_candidate" "$table_tmp"
    printf '%s\n' 'regen: flatten fkwu self-host (form-cli table)'
else
    printf '%s\n' 'regen: fkwu self-host flatten failed; no proof-sibling or MJS generation fallback is permitted' >&2
    [[ -s "$flatten_err" ]] && sed 's/^/  /' "$flatten_err" >&2
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
        cp "$table_tmp" /tmp/form-cli-aphonic-current.txt
        printf "regen: voice canary failed — ping answered '%s', not pong (aphonic carrier)\n" "$voice" >&2
        printf '%s\n' 'regen: preserved aphonic candidate at /tmp/form-cli-aphonic-current.txt' >&2
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

emitted_tmp="$work_dir/form-cli-emitted.c"
# The fixed Form emitter reads the unpublished table path from stdin.  Its own
# source image stays small/cacheable; the full table crosses read_file as data.
printf '%s\n' "$table_tmp" \
    | "$SOURCE_FKWU" form-stdlib/form-cli-bootstrap-emit-cli.fk \
    > "$emitted_tmp"
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
