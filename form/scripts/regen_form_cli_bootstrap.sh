#!/usr/bin/env zsh
# regen_form_cli_bootstrap.sh — refresh the committed form-cli table and emitted
# C carrier through the Form-native fkwu path.
set -euo pipefail
export LC_ALL=C

FORM="$(cd -P "$(dirname "$0")/.." && pwd)"
cd "$FORM"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
# shellcheck source=scripts/form_cli_bootstrap_proof.sh
source scripts/form_cli_bootstrap_proof.sh

# shellcheck source=scripts/form_cli_source_list.sh
source scripts/form_cli_source_list.sh
form_cli_load_sources

mkdir -p form-stdlib/bootstrap
# The bootstrap and platform publishers share this lock.  Holding it through
# stamp publication prevents a platform carrier from being linked against one
# table/C pair and attested against another.
form_cli_publish_lock_acquire "form-stdlib/bootstrap/.form-cli-publish.lock"
work_dir="$(mktemp -d)"
cleanup() {
    local exit_status=$?
    # A failed candidate is normally transient.  An explicit diagnostic run may
    # retain its private work directory so the exact table and canary evidence
    # can be re-observed before any repair is attempted.  Successful
    # publications always clean it, and the publisher lock is always released.
    if [[ "$exit_status" -ne 0 && "${FORM_CLI_RETAIN_WORKDIR:-0}" == "1" ]]; then
        printf 'regen: retained failed work dir %s\n' "$work_dir" >&2
    else
        rm -rf "$work_dir"
    fi
    form_cli_publish_lock_release
    return 0
}
# Zsh ERR_EXIT from a failed function does not run EXIT. ZERR releases the
# publisher lock on that path while the shell preserves the failed status.
trap cleanup ZERR EXIT INT TERM

# The recorded author identity is the exact compiler executable used below,
# not a mutable ignored path that could change between hashing and emission.
source_fkwu="${FORM_FOURTH_SOURCE_FKWU:-$FORM/../fkwu}"
[[ -f "$source_fkwu" && -x "$source_fkwu" && ! -L "$source_fkwu" ]] || {
    printf 'regen: native source runner is missing or not a regular executable: %s\n' "$source_fkwu" >&2
    exit 1
}
FOURTH_SOURCE_FKWU="$work_dir/fkwu-source"
cp "$source_fkwu" "$FOURTH_SOURCE_FKWU"
chmod 700 "$FOURTH_SOURCE_FKWU"
bml_compiler_sha256="$(form_cli_generation_sha256_file "$FOURTH_SOURCE_FKWU")"

want_cli_stamp="$(fourth_hash16 "${FORM_CLI_SRCS[@]}")"
want_source_sha256="$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")"
source_identity_still_current() {
    [[ "$(fourth_hash16 "${FORM_CLI_SRCS[@]}")" == "$want_cli_stamp" \
        && "$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")" == "$want_source_sha256" ]]
}
carrier_src="$work_dir/form-cli-carrier.fk"
sed "s/FORM_CLI_SOURCE_SHA256_PLACEHOLDER/$want_source_sha256/g" \
    form-stdlib/form-cli-carrier.fk > "$carrier_src"

source_cache="form-stdlib/.cache/source-compiled"
mkdir -p "$source_cache"
# The source compiler inputs participate in cache identity.
SOURCE_COMPILE_CHAIN=(
    form-stdlib/engine-constants.fk
    form-stdlib/compiler-objects.fk
    form-stdlib/form-ontology-bp.fk
    # source-categories joined 2026-08-27: the loader's top-level fol-cat calls
    # resolve here; the drift hid behind a warm compile cache until a bin-go
    # rebuild cold-started it (same family as the 2026-08-19 fol-core-row note).
    form-stdlib/form-ontology-source-categories.fk
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
    local src="$1" key cached out driver lens_fkwu lens_rc
    if ! grep -Eq '^[[:space:]]*section \[' "$src"; then
        printf '%s\n' "$src"
        return 0
    fi

    lens_fkwu="${FOURTH_SOURCE_FKWU:-${FKWU:-}}"
    [[ -n "$lens_fkwu" && -x "$lens_fkwu" ]] || {
        printf 'regen: fkwu lens unavailable for %s\n' "$src" >&2
        exit 1
    }

    key="$(fourth_hash16 "$src" "${SOURCE_COMPILE_CHAIN[@]}" "$lens_fkwu")"
    cached="$source_cache/$key.fk"
    if [[ ! -s "$cached" ]]; then
        out="$(mktemp "$source_cache/.tmp.XXXXXX")"
        driver="$work_dir/compile-fkwu.fk"
        {
            printf '; generated lens driver -- the closure is the preludes graph.\n'
            printf '; preludes: form-stdlib/source-compiler-text-lens.fk\n'
            printf '(do (form-source-compile-file "%s" "%s") 0)\n' "$src" "$out"
        } > "$driver"
        lens_rc=0
        "$lens_fkwu" "$driver" > "$work_dir/compile-fkwu.log" 2>&1 || lens_rc=$?
        if [[ $lens_rc -eq 0 && -s "$out" ]]; then
            mv -f "$out" "$cached"
        else
            printf 'regen: fkwu lens failed for %s (rc=%s): %s\n' \
                "$src" "$lens_rc" "$(grep -v 'warning:' "$work_dir/compile-fkwu.log" | head -1)" >&2
            rm -f "$out" "$driver" "${driver%.fk}.fkb" "${driver%.fk}.sym" 2>/dev/null
            exit 1
        fi
        rm -f "$out" "$driver" "${driver%.fk}.fkb" "${driver%.fk}.sym" 2>/dev/null
    fi
    printf '%s\n' "$cached"
}

stamp="$(fourth_fkwu_cache_stamp)"
cached_fkwu="$FOURTH_DIR/fkwu-$stamp"
if [[ -x "$cached_fkwu" ]]; then
    FKWU="$cached_fkwu"
else
    build_fourth
fi

# Program order for native self-host flattening. The request writer prepends
# fourth-shim.fk; the REPL entry rides last.
FORM_CLI_SELFHOST_ORDER=(
    form-stdlib/core.fk form-stdlib/grammars/sanskrit-roots.fk form-stdlib/resource-port.fk
    form-stdlib/bml-native-interface-package-import.fk form-stdlib/hati-os-targets.fk
    form-stdlib/form-native-resource-interfaces.fk form-stdlib/form-fs.fk
    form-stdlib/storage-port.fk form-stdlib/host-kernel-carrier.fk
    form-stdlib/fnri-standin.fk form-stdlib/fnri-receipt.fk
    form-stdlib/http-client.bml form-stdlib/line-grammar.fk form-stdlib/str-byte-at.fk
    form-stdlib/sha256.fk form-stdlib/hmac-sha256.fk form-stdlib/hex.fk
    form-stdlib/format-arith.fk form-stdlib/f16-decode.fk form-stdlib/q6k-dequant.fk form-stdlib/equireach.fk form-stdlib/equireach-gguf.fk form-stdlib/gguf-meta.fk form-stdlib/model-discovery.fk
    form-stdlib/q4k-dequant.fk form-stdlib/weight-load.fk
    form-stdlib/voice-traits.fk form-stdlib/nearest-shape.fk
    form-stdlib/co-learning.fk form-stdlib/co-learning-stream.fk form-stdlib/mesh-dispatch.fk
    form-stdlib/surprise-salience.fk form-stdlib/host-sense-organ.fk form-stdlib/speech-organ.fk
    form-stdlib/native-host-instance.fk form-stdlib/text-tokenize.fk form-stdlib/rag-embed.fk
    form-stdlib/rag-index-codec.fk form-stdlib/rag-retrieve.fk form-stdlib/rag-ask.fk
    form-stdlib/ask-cost-receipt.fk form-stdlib/ask-native-lane.fk
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
    form-stdlib/dsv4-tokenizer.fk
    form-stdlib/qwen35-tokenizer.fk
    form-stdlib/qwen35-tokfast-v2-live-reader.fk
    form-stdlib/kernel-http-header.fk
    form-stdlib/kernel-http.fk
    form-stdlib/form-asm.fk
    form-stdlib/metal-door.fk
    native/metal/sha256-arm64-jit.fk
    form-stdlib/qwen35-artifact-seal-reader.fk
    form-stdlib/q6k-msl.fk
    form-stdlib/q8-0-msl.fk
    form-stdlib/q3k-msl.fk
    form-stdlib/q4k-msl.fk
    form-stdlib/gated-deltanet-msl.fk
    form-stdlib/transformer-numerics.fk
    form-stdlib/trig.fk
    form-stdlib/tensor-ir.fk
    form-stdlib/jit-tensor-emit.fk
    form-stdlib/llama-decode-msl.fk
    form-stdlib/mla-msl.fk
    form-stdlib/moe-route-wide-msl.fk
    form-stdlib/gguf-tensor-index.fk
    form-stdlib/q3k-dequant.fk
    form-stdlib/q3k-equireach.fk
    form-stdlib/kat-coder-embed.fk
    native/metal/kat-token-handle.fk
    native/metal/qwen35-linear-span-layout-contract.fk
    native/metal/qwen35-dense-token-handle.fk
    native/metal/qwen35-crystal.fk
    native/metal/model-bandwidth.fk
    form-stdlib/form-teach-layer.fk
    form-stdlib/qwen35-form-layer.fk
    form-stdlib/active-learning-tier-cycle.fk
    form-stdlib/local-model-choice.fk
    form-stdlib/substrate-phase.fk
    form-stdlib/source-resonance-stream.fk
    form-stdlib/local-generate-organ.fk
    form-stdlib/language-template.fk
    form-stdlib/language-model.fk
    form-stdlib/form-cli-heedmark.bml
    form-stdlib/form-cli-qwen-teach-layer.fk
    form-stdlib/form-cli-heed-cursor.fk
    form-stdlib/form-ontology-bp.fk
    form-stdlib/form-cli-heed-telemetry.fk
    form-stdlib/form-knowledge-query-token.fk
    form-stdlib/file-byte-window.fk
    form-stdlib/form-knowledge-source-search.fk
    form-stdlib/form-cli-heed-current-source.fk
    form-stdlib/form-cli-heed-grounded.fk
    form-stdlib/qwen35-tokenizer-live-cursor.fk
    form-stdlib/form-cli-model-generate.fk
    form-stdlib/form-cli-model-session.fk
    form-stdlib/bmf-byte-cursor.fk
    form-stdlib/public-source-concept-index.fk
    form-stdlib/public-source-concept-shards.fk
    form-stdlib/form-nodeid-knowledge-query.fk
    form-stdlib/form-cli-nodeid-knowledge-session.fk
    form-stdlib/public-source-concept-key-routes.fk
    form-stdlib/form-nodeid-knowledge-routed-query.fk
    form-stdlib/form-cli-nodeid-knowledge-door.fk
    form-stdlib/form-recipe-birth-token.fk
    form-stdlib/form-recipe-exec-token.fk
    form-stdlib/form-cli-recipe-exec-cursor.fk
    form-stdlib/form-recipe-exec-token-live.fk
    form-stdlib/form-cli-recipe-exec-session.fk
    form-stdlib/form-cli-resident-recipe-birth-exec-categories.fk
    form-stdlib/form-cli-resident-recipe-birth-exec.fk
    form-stdlib/form-nodeid-mastery-cell-categories.fk
    form-stdlib/form-nodeid-mastery-cell-loop.fk
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

table_tmp="$work_dir/form-cli-table.txt"
flatten_candidate="$work_dir/form-cli-table.candidate"
flatten_err="$work_dir/form-cli-flatten.err"
flattener_kind=""
flattener_binary_sha256=""
{
    # One extra shim and one trailing entry: module count equals this array's count.
    printf '%s\n' "${#FORM_CLI_FLATTEN_SRCS[@]}" "$FORM/$FOURTH_SHIM"
    for src in "${FORM_CLI_FLATTEN_SRCS[@]}"; do
        if [[ "$src" == /* ]]; then printf '%s\n' "$src"; else printf '%s\n' "$FORM/$src"; fi
    done
} > "$work_dir/flatten.request"
if (cd "$FORM/.." && "$FOURTH_SOURCE_FKWU" form/form-stdlib/bml/native-table-compile.bml) \
            < "$work_dir/flatten.request" > "$flatten_candidate" 2> "$flatten_err" \
        && form_cli_validate_table "$flatten_candidate" >/dev/null; then
    mv -f "$flatten_candidate" "$table_tmp"
    flattener_kind="fkwu-source"
    flattener_binary_sha256="$bml_compiler_sha256"
    printf '%s\n' 'regen: Form-native source compiler (form-cli table)'
else
    printf '%s\n' 'regen: Form-native table compilation failed' >&2
    [[ -f "$flatten_err" ]] && sed 's/^/  /' "$flatten_err" >&2
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
    printf '%s\n' 'regen: voice canary requires the native table runner' >&2
    exit 1
fi
stamp_tmp="$work_dir/form-cli.stamp"
printf '%s\n' "$want_cli_stamp" > "$stamp_tmp"
source_digest_tmp="$work_dir/form-cli.source.sha256"
printf '%s\n' "$want_source_sha256" > "$source_digest_tmp"

EMIT_CHAIN=(
    form-stdlib/minimal-surface.fk
    form-stdlib/hati-os-kernel.fk
    form-stdlib/host-io-fs-fkwu-emit.fk
    form-stdlib/form-table-text.fk
    form-stdlib/fkc-table-serialize.fk
    form-stdlib/hati-os-kernel-emit.fk
)
printf '; preludes: %s\n(fkc-emit-combined-repl (read_file "%s"))\n' \
    "${EMIT_CHAIN[*]}" "$table_tmp" > "$work_dir/emit.fk"
emitted_tmp="$work_dir/form-cli-emitted.c"
"$FOURTH_SOURCE_FKWU" "$work_dir/emit.fk" > "$emitted_tmp"
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
source_identity_still_current || {
    printf '%s\n' 'regen: canonical source identity changed during generation; refusing publication' >&2
    exit 1
}
generation_attestation_tmp="$work_dir/form-cli.generation.attestation"
form_cli_write_generation_attestation \
    "$generation_attestation_tmp" "$want_source_sha256" "$want_cli_stamp" \
    "$table_tmp" "$emitted_tmp" "$bml_compiler_sha256" \
    "$flattener_kind" "$flattener_binary_sha256" \
    "not-applicable" "not-applicable" "not-applicable" "fkwu-source"
form_cli_verify_generation_attestation \
    "$generation_attestation_tmp" "$want_source_sha256" "$want_cli_stamp" \
    "$table_tmp" "$emitted_tmp" "not-applicable"

# Publish the stamp last.  Readers either see the prior coherent carrier or a
# stale stamp while the two payloads move; they never accept a mixed carrier.
mv -f "$emitted_tmp" form-stdlib/bootstrap/form-cli-emitted.c
mv -f "$table_tmp" form-stdlib/bootstrap/form-cli-table.txt
mv -f "$source_digest_tmp" form-stdlib/bootstrap/form-cli.source.sha256
mv -f "$generation_attestation_tmp" form-stdlib/bootstrap/form-cli.generation.attestation
mv -f "$stamp_tmp" form-stdlib/bootstrap/form-cli.stamp

printf 'regen: form-cli-emitted.c (%s bytes) stamp=%s %s attestation=form-stdlib/bootstrap/form-cli.generation.attestation\n' \
    "$(wc -c < form-stdlib/bootstrap/form-cli-emitted.c | tr -d ' ')" \
    "$(cat form-stdlib/bootstrap/form-cli.stamp)" "$table_shape"
