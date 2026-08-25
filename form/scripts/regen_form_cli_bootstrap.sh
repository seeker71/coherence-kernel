#!/usr/bin/env zsh
# regen_form_cli_bootstrap.sh — refresh the committed form-cli table and emitted
# C carrier.  The Rust and TypeScript proof siblings can author the same
# flattened table without the Go sibling's larger peak on memory-tight hosts;
# the Form-native fkwu self-host remains the destination carrier.
set -euo pipefail
export LC_ALL=C

FORM="$(cd -P "$(dirname "$0")/.." && pwd)"
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
    return "$exit_status"
}
trap cleanup EXIT INT TERM

# The recorded author identity is the exact compiler executable used below,
# not a mutable ignored path that could change between hashing and emission.
[[ -f "$GO_KERNEL" && -x "$GO_KERNEL" && ! -L "$GO_KERNEL" ]] || {
    printf 'regen: Go BML compiler is missing or not a regular executable: %s\n' "$GO_KERNEL" >&2
    exit 1
}
go_kernel_snapshot="$work_dir/go-bml-compiler"
cp "$GO_KERNEL" "$go_kernel_snapshot"
chmod 700 "$go_kernel_snapshot"
GO_KERNEL="$go_kernel_snapshot"
export GO_BIN="$GO_KERNEL"
bml_compiler_sha256="$(form_cli_generation_sha256_file "$GO_KERNEL")"

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
# The go lane's explicit closure (walkers do not read `; preludes:` lines).
# engine-constants / compiler-objects / form-ontology-bp joined 2026-08-19 —
# their Form births crashed this hand-held mirror at fol-core-row, the same
# drift the other three copies of this list had already suffered.
SOURCE_COMPILE_CHAIN=(
    form-stdlib/engine-constants.fk
    form-stdlib/compiler-objects.fk
    form-stdlib/form-ontology-bp.fk
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
    # fkwu first: the thin driver's closure is the body's `; preludes:` graph,
    # so no list here can drift for this lane. go stays as the fallback below.
    if [[ ! -s "$cached" && -n "${FKWU:-}" && -x "${FKWU:-}" ]]; then
        out="$(mktemp "$source_cache/.tmp.XXXXXX")"
        driver="$work_dir/compile-fkwu.fk"
        {
            printf '; generated lens driver -- the closure is the preludes graph.\n'
            printf '; preludes: form-stdlib/source-compiler-text-lens.fk\n'
            printf '(do (form-source-compile-file "%s" "%s") 0)\n' "$src" "$out"
        } > "$driver"
        if "$FKWU" "$driver" >/dev/null 2>&1 && [[ -s "$out" ]]; then
            mv -f "$out" "$cached"
        fi
        rm -f "$out" "$driver" "${driver%.fk}.fkb" "${driver%.fk}.sym" 2>/dev/null
    fi
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
    form-stdlib/form-cli-heedmark-xtal.fk
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
# WITNESSED 2026-08-17 — the failure this note already described, arriving.
# One identical tree, one identical source stamp 8c2fcc728bab0c97, TWO programs:
#   fkwu self-host arm  -> functions=1933  nodes=59609  strings=1975
#   Rust sibling arm    -> functions=1924  nodes=58946  strings=1938
# Same declared identity, nine fewer callable functions, and ds4-query-channel.fk
# defines exactly nine. It was named only by a ${form_modules/pat/rep} line whose
# pattern carried bash-style \/ and \" escapes; in zsh that pattern matches
# NOTHING, so the substitution was a silent no-op and the cell never entered the
# program. It stayed in the stamp, in the genesis text and in `form-cli source`,
# quotable and uncallable — the shape this note has warned about since
# 2026-08-03. The stamp could not see it: the stamp hashes the source LIST, and
# the list had the cell.
#
# Two things kept it hidden. A substitution that fails is indistinguishable from
# one that succeeds unless you compare the before and after. And the arm is
# chosen by whichever flattener happens to be built on the host — Rust first —
# so a checkout without form-kernel-rust silently produced the CORRECT carrier
# and never disagreed with anything. This session built the Rust kernel mid-run
# for a four-way band; the next regen changed arms, and only then did the number
# move.
#
# The repair is not better escaping. The cell is named in $modules directly, in
# the position the substitution meant, and the substitution line is gone. A list
# that says what is in it cannot silently not-say it.
#
# fourth-shim.fk is in $modules and NOT in FORM_CLI_SELFHOST_ORDER, and that is
# correct rather than a seventh gap: it is the flattener's standing prelude and
# fourth_band_request prepends it on the fkwu path. Both arms carry it; only one
# has to say so. Checked before touching it — its 85 defns would otherwise have
# looked like the biggest gap here and it is not a gap at all.
core_src="$(compile_bml "$stdlib/core.fk")"
http_client_src="$(compile_bml "$stdlib/http-client.fk")"
form_cli_ask_src="$(compile_bml "$stdlib/form-cli-ask.fk")"
form_modules="(list (read_file \"$stdlib/fourth-shim.fk\") (read_file \"$core_src\") (read_file \"$stdlib/grammars/sanskrit-roots.fk\") (read_file \"$stdlib/resource-port.fk\") (read_file \"$stdlib/bml-native-interface-package-import.fk\") (read_file \"$stdlib/hati-os-targets.fk\") (read_file \"$stdlib/form-native-resource-interfaces.fk\") (read_file \"$stdlib/form-fs.fk\") (read_file \"$stdlib/storage-port.fk\") (read_file \"$stdlib/host-kernel-carrier.fk\") (read_file \"$stdlib/fnri-standin.fk\") (read_file \"$stdlib/fnri-receipt.fk\") (read_file \"$http_client_src\") (read_file \"$stdlib/line-grammar.fk\") (read_file \"$stdlib/str-byte-at.fk\") (read_file \"$stdlib/sha256.fk\") (read_file \"$stdlib/hmac-sha256.fk\") (read_file \"$stdlib/hex.fk\") (read_file \"$stdlib/format-arith.fk\") (read_file \"$stdlib/f16-decode.fk\") (read_file \"$stdlib/q6k-dequant.fk\") (read_file \"$stdlib/equireach.fk\") (read_file \"$stdlib/equireach-gguf.fk\") (read_file \"$stdlib/gguf-meta.fk\") (read_file \"$stdlib/model-discovery.fk\") (read_file \"$stdlib/q4k-dequant.fk\") (read_file \"$stdlib/weight-load.fk\") (read_file \"$stdlib/voice-traits.fk\") (read_file \"$stdlib/nearest-shape.fk\") (read_file \"$stdlib/co-learning.fk\") (read_file \"$stdlib/co-learning-stream.fk\") (read_file \"$stdlib/mesh-dispatch.fk\") (read_file \"$stdlib/surprise-salience.fk\") (read_file \"$stdlib/host-sense-organ.fk\") (read_file \"$stdlib/speech-organ.fk\") (read_file \"$stdlib/native-host-instance.fk\") (read_file \"$stdlib/text-tokenize.fk\") (read_file \"$stdlib/rag-embed.fk\") (read_file \"$stdlib/rag-index-codec.fk\") (read_file \"$stdlib/rag-retrieve.fk\") (read_file \"$stdlib/rag-ask.fk\") (read_file \"$stdlib/ask-cost-receipt.fk\") (read_file \"$stdlib/ask-native-lane.fk\") (read_file \"$form_cli_ask_src\") (read_file \"$stdlib/form-cli-router.fk\") (read_file \"$stdlib/form-cli-judge.fk\") (read_file \"$stdlib/confidence-weighted-vote.fk\") (read_file \"$stdlib/lineage-discounted-vote.fk\") (read_file \"$stdlib/form-cli-oracle-loop.fk\") (read_file \"$stdlib/form-cli-sufficiency.fk\") (read_file \"$stdlib/form-freq-check.fk\") (read_file \"$stdlib/trust-row.fk\") (read_file \"$stdlib/form-cli-ask-gate.fk\") (read_file \"$stdlib/form-cli-staged-trace.fk\") (read_file \"$stdlib/form-cli-request.fk\") (read_file \"$carrier_src\") (read_file \"$stdlib/form-cli-ask-plus.fk\") (read_file \"$stdlib/form-cli-surface-inquiry.fk\") (read_file \"$stdlib/current-branch-landing.fk\") (read_file \"$stdlib/form-cli-inquiry.fk\") (read_file \"$stdlib/ds4-query-channel.fk\") (read_file \"$stdlib/form-cli.fk\") (read_file \"$stdlib/form-cli-gguf-cell.fk\"))"
form_modules="${form_modules%)} (read_file \"$stdlib/relational-inquiry-metabolism.fk\") (read_file \"$stdlib/native-model-native-hierarchy.fk\") (read_file \"$stdlib/native-model-control-plane.fk\") (read_file \"$stdlib/ask-lane-router.fk\"))"
# The model generate door and its full prelude closure — the cells `generate`
# actually calls.  Until 2026-08-19 form-cli-repl.fk named form-cli-model-generate.fk
# in its preludes and NO build list carried it, so the built binary answered
# `generate` from an unresolved name: instant, modelless, and wrong.
form_modules="${form_modules%)} (read_file \"form-stdlib/dsv4-tokenizer.fk\") (read_file \"form-stdlib/qwen35-tokenizer.fk\") (read_file \"form-stdlib/qwen35-tokfast-v2-live-reader.fk\") (read_file \"form-stdlib/kernel-http-header.fk\") (read_file \"form-stdlib/kernel-http.fk\") (read_file \"form-stdlib/form-asm.fk\") (read_file \"form-stdlib/metal-door.fk\") (read_file \"native/metal/sha256-arm64-jit.fk\") (read_file \"form-stdlib/qwen35-artifact-seal-reader.fk\") (read_file \"form-stdlib/q6k-msl.fk\") (read_file \"form-stdlib/q8-0-msl.fk\") (read_file \"form-stdlib/q3k-msl.fk\") (read_file \"form-stdlib/q4k-msl.fk\") (read_file \"form-stdlib/gated-deltanet-msl.fk\") (read_file \"form-stdlib/transformer-numerics.fk\") (read_file \"form-stdlib/trig.fk\") (read_file \"form-stdlib/tensor-ir.fk\") (read_file \"form-stdlib/jit-tensor-emit.fk\") (read_file \"form-stdlib/llama-decode-msl.fk\") (read_file \"form-stdlib/mla-msl.fk\") (read_file \"form-stdlib/moe-route-wide-msl.fk\") (read_file \"form-stdlib/gguf-tensor-index.fk\") (read_file \"form-stdlib/q3k-dequant.fk\") (read_file \"form-stdlib/q3k-equireach.fk\") (read_file \"form-stdlib/kat-coder-embed.fk\") (read_file \"native/metal/kat-token-handle.fk\") (read_file \"native/metal/qwen35-linear-span-layout-contract.fk\") (read_file \"native/metal/qwen35-dense-token-handle.fk\") (read_file \"native/metal/qwen35-crystal.fk\") (read_file \"native/metal/model-bandwidth.fk\") (read_file \"form-stdlib/form-cli-qwen-teach-layer.fk\") (read_file \"form-stdlib/qwen35-tokenizer-live-cursor.fk\") (read_file \"form-stdlib/form-cli-model-generate.fk\"))"
form_modules="${form_modules%)} (read_file \"$stdlib/form-teach-layer.fk\") (read_file \"$stdlib/qwen35-form-layer.fk\") (read_file \"$stdlib/active-learning-tier-cycle.fk\") (read_file \"$stdlib/local-model-choice.fk\") (read_file \"$stdlib/substrate-phase.fk\") (read_file \"$stdlib/source-resonance-stream.fk\") (read_file \"$stdlib/local-generate-organ.fk\") (read_file \"$stdlib/language-template.fk\") (read_file \"$stdlib/language-model.fk\") (read_file \"$stdlib/form-cli-heedmark-xtal.fk\"))"
form_modules="${form_modules%)} (read_file \"$stdlib/form-cli-heed-cursor.fk\") (read_file \"$stdlib/form-ontology-bp.fk\") (read_file \"$stdlib/form-cli-heed-telemetry.fk\") (read_file \"$stdlib/form-knowledge-query-token.fk\") (read_file \"$stdlib/file-byte-window.fk\") (read_file \"$stdlib/form-knowledge-source-search.fk\") (read_file \"$stdlib/form-cli-heed-current-source.fk\") (read_file \"$stdlib/form-cli-heed-grounded.fk\"))"
form_modules="${form_modules%)} (read_file \"$stdlib/form-cli-model-session.fk\") (read_file \"$stdlib/bmf-byte-cursor.fk\") (read_file \"$stdlib/public-source-concept-index.fk\") (read_file \"$stdlib/public-source-concept-shards.fk\") (read_file \"$stdlib/form-nodeid-knowledge-query.fk\") (read_file \"$stdlib/form-cli-nodeid-knowledge-session.fk\") (read_file \"$stdlib/public-source-concept-key-routes.fk\") (read_file \"$stdlib/form-nodeid-knowledge-routed-query.fk\"))"
form_modules="${form_modules%)} (read_file \"$stdlib/form-cli-nodeid-knowledge-door.fk\"))"
form_modules="${form_modules%)} (read_file \"$stdlib/form-recipe-birth-token.fk\") (read_file \"$stdlib/form-recipe-exec-token.fk\") (read_file \"$stdlib/form-cli-recipe-exec-cursor.fk\") (read_file \"$stdlib/form-recipe-exec-token-live.fk\") (read_file \"$stdlib/form-cli-recipe-exec-session.fk\") (read_file \"$stdlib/form-cli-resident-recipe-birth-exec-categories.fk\") (read_file \"$stdlib/form-cli-resident-recipe-birth-exec.fk\"))"
form_modules="${form_modules%)} (read_file \"$stdlib/form-nodeid-mastery-cell-categories.fk\") (read_file \"$stdlib/form-nodeid-mastery-cell-loop.fk\"))"
band="(read_file \"$stdlib/form-cli-repl.fk\")"
FLATTEN_CHAIN=(
    form-stdlib/minimal-surface.fk
    form-stdlib/hati-os-kernel.fk
    form-stdlib/host-io-fs-fkwu-emit.fk
    form-stdlib/form-table-text.fk
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
    "$form_modules" "$band" "$form_modules" "$band" > "$work_dir/flatten.fk"

flatten_candidate="$work_dir/form-cli-table.candidate"
flatten_err="$work_dir/form-cli-flatten.err"
flattener_kind=""
flattener_binary_sha256=""
snapshot_flattener_artifact() {
    local source="$1" destination="$2"
    [[ -f "$source" && ! -L "$source" ]] || return 1
    cp "$source" "$destination"
    [[ -f "$destination" && ! -L "$destination" ]]
}

rust_flattener="$work_dir/rust-flattener"
typescript_flattener="$work_dir/typescript-flattener.mjs"
fkwu_flattener="$work_dir/fkwu-flattener"
if [[ -x "$RS_KERNEL" && ! -L "$RS_KERNEL" ]] \
        && snapshot_flattener_artifact "$RS_KERNEL" "$rust_flattener" \
        && chmod 700 "$rust_flattener" \
        && "$rust_flattener" "${FLATTEN_CHAIN[@]}" "$work_dir/flatten.fk" \
            > "$flatten_candidate" 2> "$flatten_err" \
        && form_cli_validate_table "$flatten_candidate" >/dev/null; then
    mv -f "$flatten_candidate" "$table_tmp"
    flattener_kind="rust-form-kernel"
    flattener_binary_sha256="$(form_cli_generation_sha256_file "$rust_flattener")"
    printf '%s\n' 'regen: flatten Rust proof sibling (form-cli table)'
elif [[ -f "$TS_KERNEL" && ! -L "$TS_KERNEL" ]] \
        && snapshot_flattener_artifact "$TS_KERNEL" "$typescript_flattener" \
        && node "$typescript_flattener" \
            "${FLATTEN_CHAIN[@]}" "$work_dir/flatten.fk" \
            > "$flatten_candidate" 2> "$flatten_err" \
        && form_cli_validate_table "$flatten_candidate" >/dev/null; then
    mv -f "$flatten_candidate" "$table_tmp"
    flattener_kind="typescript-form-kernel"
    flattener_binary_sha256="$(form_cli_generation_sha256_file "$typescript_flattener")"
    printf '%s\n' 'regen: flatten TypeScript proof sibling (form-cli table)'
elif fourth_selfhost \
        && [[ -f "${FKWU:-}" && -x "${FKWU:-}" && ! -L "${FKWU:-}" ]] \
        && snapshot_flattener_artifact "$FKWU" "$fkwu_flattener" \
        && chmod 700 "$fkwu_flattener" \
        && FKWU="$fkwu_flattener" fourth_flatten_sources \
            form-cli-bootstrap fks "$flatten_candidate" "${FORM_CLI_FLATTEN_SRCS[@]}" \
        && form_cli_validate_table "$flatten_candidate" >/dev/null; then
    mv -f "$flatten_candidate" "$table_tmp"
    flattener_kind="fkwu-selfhost"
    flattener_binary_sha256="$(form_cli_generation_sha256_file "$fkwu_flattener")"
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
    form-stdlib/form-table-text.fk
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
source_identity_still_current || {
    printf '%s\n' 'regen: canonical source identity changed during generation; refusing publication' >&2
    exit 1
}
generation_attestation_tmp="$work_dir/form-cli.generation.attestation"
form_cli_write_generation_attestation \
    "$generation_attestation_tmp" "$want_source_sha256" "$want_cli_stamp" \
    "$table_tmp" "$emitted_tmp" "$bml_compiler_sha256" \
    "$flattener_kind" "$flattener_binary_sha256" \
    "not-applicable" "not-applicable" "not-applicable"
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
