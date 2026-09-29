#!/usr/bin/env zsh
# form_cli_source_list.sh — one source-identity order for every form-cli
# bootstrap publisher.  Source this only after the caller has cd'd to form/.
#
# Roots name the Form program, its compilers and its host build carriers.
# Native dependency expansion publishes the complete bootstrap manifest.
# Its bytes and every referenced source participate in identity and genesis.
# Non-Form inputs are carried and hashed; they are never parsed as Form.

form_cli_source_roots() {
    cat <<'EOF'
build-form-cli.sh
scripts/form_cli_bootstrap_proof.sh
scripts/regen_form_cli_bootstrap.sh
../runtime/fkwu-uni.c
../runtime/fkwu-optable.h
../runtime/fkwu-node-word.h
form-stdlib/bml/native-node-word.bml
../observe/native-node-word-generate.bml
../observe/native-node-word-verify.bml
form-stdlib/home-index.txt
native/metal/fk-metal-carrier.m
form-stdlib/bml/metal-ask.bml
form-stdlib/bml/metal-ask-request.bml
form-stdlib/bml/json-wire-admission.bml
../observe/metal-ask-run.bml
../observe/metal-ask-files-run.bml
form-stdlib/bml/sha256-owned.bml
form-stdlib/bml/metal-jit.bml
form-stdlib/llama32-3b-resident-state.bml
form-stdlib/llama3-tokenize.fk
form-stdlib/llama3-detokenize.fk
native/metal/llama-token-handle.fk
native/metal/ask-declared-cost.fk
native/metal/first-token.fk
native/metal/whole-tensor-residency.fk
form-stdlib/minimal-surface.fk
form-stdlib/hati-os-kernel.fk
form-stdlib/host-io-fs-fkwu-emit.fk
form-stdlib/form-table-text.fk
form-stdlib/fkc-table-serialize.fk
form-stdlib/hati-os-kernel-emit.fk
form-stdlib/form-parse.fk
form-stdlib/bmf-core.fk
form-stdlib/bmf-grammar.fk
form-stdlib/host-effect-grammar.fk
form-stdlib/form-flatten.fk
form-stdlib/bml/native-table-compile.bml
form-stdlib/bml/native-table-sources.bml
form-stdlib/bml/form-cli-source-closure.bml
form-stdlib/bml-floor-compile.fk
form-stdlib/form-ontology-loader.fk
form-stdlib/bml.fk
form-stdlib/bml-source.fk
form-stdlib/source-compiler.fk
form-stdlib/grammars/form-bml.fk
form-stdlib/form-bml-lower.fk
form-stdlib/source-compiler-text-lens.fk
form-stdlib/fourth-shim.fk
form-stdlib/core.fk
form-stdlib/grammars/sanskrit-roots.fk
form-stdlib/line-grammar.fk
form-stdlib/str-byte-at.fk
form-stdlib/sha256.fk
form-stdlib/hmac-sha256.fk
form-stdlib/hex.fk
form-stdlib/resource-port.fk
form-stdlib/bml-native-interface-package-import.fk
form-stdlib/hati-os-targets.fk
form-stdlib/form-native-resource-interfaces.fk
form-stdlib/form-fs.fk
form-stdlib/storage-port.fk
form-stdlib/host-kernel-carrier.fk
form-stdlib/fnri-standin.fk
form-stdlib/fnri-receipt.fk
form-stdlib/http-client.bml
form-stdlib/format-arith.fk
form-stdlib/f16-decode.fk
form-stdlib/q6k-dequant.fk
form-stdlib/q6k-msl.fk
form-stdlib/q4k-msl.fk
form-stdlib/equireach.fk
form-stdlib/equireach-gguf.fk
form-stdlib/gguf-meta.fk
form-stdlib/model-discovery.fk
form-stdlib/q4k-dequant.fk
form-stdlib/weight-load.fk
form-stdlib/transformer-numerics.fk
form-stdlib/transformer-block.fk
form-stdlib/llama-numerics.fk
form-stdlib/trig.fk
form-stdlib/tensor-ir.fk
form-stdlib/jit-tensor-emit.fk
form-stdlib/llama-decode-msl.fk
form-stdlib/qk-matvec-split.fk
form-stdlib/qk-matvec-lane.fk
form-stdlib/qk-matvec-slot.fk
form-stdlib/qk-matmul-batch.fk
form-stdlib/voice-traits.fk
form-stdlib/nearest-shape.fk
form-stdlib/co-learning.fk
form-stdlib/co-learning-stream.fk
form-stdlib/mesh-dispatch.fk
form-stdlib/surprise-salience.fk
form-stdlib/host-sense-organ.fk
form-stdlib/speech-organ.fk
form-stdlib/native-host-instance.fk
form-stdlib/text-tokenize.fk
form-stdlib/rag-embed.fk
form-stdlib/rag-index-codec.fk
form-stdlib/rag-retrieve.fk
form-stdlib/rag-ask.fk
form-stdlib/ask-cost-receipt.fk
form-stdlib/ask-native-lane.fk
form-stdlib/form-cli-ask.fk
form-stdlib/form-cli-local-law.fk
form-stdlib/form-cli-router.fk
form-stdlib/form-cli-judge.fk
form-stdlib/confidence-weighted-vote.fk
form-stdlib/lineage-discounted-vote.fk
form-stdlib/form-cli-oracle-loop.fk
form-stdlib/form-cli-sufficiency.fk
form-stdlib/form-freq-check.fk
form-stdlib/trust-row.fk
form-stdlib/form-cli-ask-gate.fk
form-stdlib/form-cli-staged-trace.fk
form-stdlib/form-cli-request.fk
form-stdlib/form-cli-carrier.fk
form-stdlib/form-cli-ask-plus.fk
form-stdlib/form-cli-surface-inquiry.fk
form-stdlib/current-branch-landing.fk
form-stdlib/form-cli-inquiry-edge-ledger.fk
form-stdlib/form-cli-inquiry.fk
form-stdlib/relational-inquiry-metabolism.fk
form-stdlib/native-model-native-hierarchy.fk
form-stdlib/ds4-query-channel.fk
form-stdlib/form-cli.fk
form-stdlib/bml/form-core-health.bml
form-stdlib/bml/organ-health-discovery.bml
form-stdlib/bml/organ-health-stream.bml
form-stdlib/organ-health.bml
form-stdlib/organ-care.bml
form-stdlib/bml/form-cli-heal.bml
form-stdlib/bml/form-cli-heal-native-process.bml
form-stdlib/native-session-memory.bml
form-stdlib/native-session-experience.bml
form-stdlib/form-glass-gift-frame.bml
form-stdlib/native-model-control-plane.fk
form-stdlib/ask-lane-router.fk
form-stdlib/form-cli-gguf-cell.fk
form-stdlib/dsv4-tokenizer.fk
form-stdlib/qwen35-tokenizer.fk
form-stdlib/qwen35-tokfast-v2-live-reader.fk
form-stdlib/kernel-http-header.fk
form-stdlib/kernel-http.fk
form-stdlib/form-asm.fk
form-stdlib/metal-door.fk
native/metal/sha256-arm64-jit.fk
form-stdlib/qwen35-artifact-seal-reader.fk
form-stdlib/q8-0-msl.fk
form-stdlib/q3k-msl.fk
form-stdlib/gated-deltanet-msl.fk
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
form-stdlib/bml/form-cli-microthought.bml
form-stdlib/form-nodeid-mastery-cell-categories.fk
form-stdlib/form-nodeid-mastery-cell-loop.fk
form-stdlib/form-cli-repl.fk
scripts/form_cli_source_list.sh
EOF
}

# Native startup and companion images use the current source runtime. These
# metadata readers validate host artifacts without invoking a Form compiler.
form_cli_native_load_attestation() {
    local artifact_path="$1" line key value seen='|'
    FORM_CLI_NATIVE_SCHEMA="" FORM_CLI_NATIVE_SOURCE_SHA256="" FORM_CLI_NATIVE_SOURCE_STAMP=""
    FORM_CLI_NATIVE_STARTUP_SHA256="" FORM_CLI_NATIVE_AUTHOR_BINARY_SHA256="" FORM_CLI_NATIVE_RUNTIME_SOURCE_SHA256=""
    FORM_CLI_NATIVE_BOOTSTRAP_ATTESTATION_SHA256="" FORM_CLI_NATIVE_PLATFORM_SLUG=""
    FORM_CLI_NATIVE_BINARY_SHA256="" FORM_CLI_NATIVE_IMAGE_SHA256="" FORM_CLI_NATIVE_SYMBOLS_SHA256=""
    [[ -f "$artifact_path" && ! -L "$artifact_path" ]] || return 1
    while IFS= read -r line || [[ -n "$line" ]]; do
        [[ "$line" == *=* ]] || return 1
        key="${line%%=*}" value="${line#*=}"
        [[ "$seen" != *"|$key|"* && -n "$value" ]] || return 1
        seen="$seen$key|"
        case "$key" in
            schema) FORM_CLI_NATIVE_SCHEMA="$value" ;;
            source_sha256) FORM_CLI_NATIVE_SOURCE_SHA256="$value" ;;
            source_stamp) FORM_CLI_NATIVE_SOURCE_STAMP="$value" ;;
            startup_sha256) FORM_CLI_NATIVE_STARTUP_SHA256="$value" ;;
            author_binary_sha256) FORM_CLI_NATIVE_AUTHOR_BINARY_SHA256="$value" ;;
            runtime_source_sha256) FORM_CLI_NATIVE_RUNTIME_SOURCE_SHA256="$value" ;;
            bootstrap_attestation_sha256) FORM_CLI_NATIVE_BOOTSTRAP_ATTESTATION_SHA256="$value" ;;
            platform_slug) FORM_CLI_NATIVE_PLATFORM_SLUG="$value" ;;
            binary_sha256) FORM_CLI_NATIVE_BINARY_SHA256="$value" ;;
            image_sha256) FORM_CLI_NATIVE_IMAGE_SHA256="$value" ;;
            symbols_sha256) FORM_CLI_NATIVE_SYMBOLS_SHA256="$value" ;;
            *) return 1 ;;
        esac
    done < "$artifact_path"
    form_cli_generation_hash_valid "$FORM_CLI_NATIVE_SOURCE_SHA256" && form_cli_generation_stamp_valid "$FORM_CLI_NATIVE_SOURCE_STAMP" || return 1
    case "$FORM_CLI_NATIVE_SCHEMA" in
        form-cli-native-attestation-v1)
            form_cli_generation_hash_valid "$FORM_CLI_NATIVE_STARTUP_SHA256" \
                && form_cli_generation_hash_valid "$FORM_CLI_NATIVE_AUTHOR_BINARY_SHA256" \
                && form_cli_generation_hash_valid "$FORM_CLI_NATIVE_RUNTIME_SOURCE_SHA256" \
                && [[ -z "$FORM_CLI_NATIVE_PLATFORM_SLUG$FORM_CLI_NATIVE_BOOTSTRAP_ATTESTATION_SHA256$FORM_CLI_NATIVE_BINARY_SHA256$FORM_CLI_NATIVE_IMAGE_SHA256$FORM_CLI_NATIVE_SYMBOLS_SHA256" ]]
            ;;
        form-cli-native-platform-attestation-v1)
            form_cli_generation_hash_valid "$FORM_CLI_NATIVE_BOOTSTRAP_ATTESTATION_SHA256" \
                && [[ "$FORM_CLI_NATIVE_PLATFORM_SLUG" =~ ^[A-Za-z0-9._-]+$ ]] \
                && form_cli_generation_hash_valid "$FORM_CLI_NATIVE_BINARY_SHA256" \
                && form_cli_generation_hash_valid "$FORM_CLI_NATIVE_IMAGE_SHA256" \
                && form_cli_generation_hash_valid "$FORM_CLI_NATIVE_SYMBOLS_SHA256" \
                && [[ -z "$FORM_CLI_NATIVE_STARTUP_SHA256$FORM_CLI_NATIVE_AUTHOR_BINARY_SHA256$FORM_CLI_NATIVE_RUNTIME_SOURCE_SHA256" ]]
            ;;
        *) return 1 ;;
    esac
}

form_cli_native_write_attestation() {
    local artifact_path="$1" source_sha="$2" source_stamp="$3" startup="$4" author_sha="$5" runtime_source="$6"
    form_cli_generation_hash_valid "$source_sha" && form_cli_generation_stamp_valid "$source_stamp" && form_cli_generation_hash_valid "$author_sha" || return 1
    local startup_sha runtime_sha
    startup_sha="$(form_cli_generation_sha256_file "$startup")" || return 1
    runtime_sha="$(form_cli_generation_sha256_file "$runtime_source")" || return 1
    {
        printf '%s\n' 'schema=form-cli-native-attestation-v1'
        printf 'source_sha256=%s\nsource_stamp=%s\nstartup_sha256=%s\nauthor_binary_sha256=%s\nruntime_source_sha256=%s\n' "$source_sha" "$source_stamp" "$startup_sha" "$author_sha" "$runtime_sha"
    } > "$artifact_path" || return 1
    form_cli_native_verify_attestation "$artifact_path" "$source_sha" "$source_stamp" "$startup" "$runtime_source" "$author_sha"
}

form_cli_native_verify_attestation() {
    local artifact_path="$1" source_sha="$2" source_stamp="$3" startup="$4" runtime_source="$5" author_sha="${6:-}"
    form_cli_native_load_attestation "$artifact_path" || return 1
    [[ "$FORM_CLI_NATIVE_SCHEMA" == form-cli-native-attestation-v1 \
        && "$FORM_CLI_NATIVE_SOURCE_SHA256" == "$source_sha" && "$FORM_CLI_NATIVE_SOURCE_STAMP" == "$source_stamp" \
        && "$FORM_CLI_NATIVE_STARTUP_SHA256" == "$(form_cli_generation_sha256_file "$startup")" \
        && "$FORM_CLI_NATIVE_RUNTIME_SOURCE_SHA256" == "$(form_cli_generation_sha256_file "$runtime_source")" \
        && ( -z "$author_sha" || "$FORM_CLI_NATIVE_AUTHOR_BINARY_SHA256" == "$author_sha" ) ]]
}

form_cli_native_write_platform_attestation() {
    local artifact_path="$1" bootstrap="$2" slug="$3" binary="$4" image="$5" symbols="$6"
    form_cli_native_load_attestation "$bootstrap" || return 1
    [[ "$FORM_CLI_NATIVE_SCHEMA" == form-cli-native-attestation-v1 && "$slug" =~ ^[A-Za-z0-9._-]+$ ]] || return 1
    local source_sha="$FORM_CLI_NATIVE_SOURCE_SHA256" source_stamp="$FORM_CLI_NATIVE_SOURCE_STAMP" bootstrap_sha binary_sha image_sha symbols_sha
    bootstrap_sha="$(form_cli_generation_sha256_file "$bootstrap")" || return 1
    binary_sha="$(form_cli_generation_sha256_file "$binary")" || return 1
    image_sha="$(form_cli_generation_sha256_file "$image")" || return 1
    symbols_sha="$(form_cli_generation_sha256_file "$symbols")" || return 1
    {
        printf '%s\n' 'schema=form-cli-native-platform-attestation-v1'
        printf 'source_sha256=%s\nsource_stamp=%s\nbootstrap_attestation_sha256=%s\nplatform_slug=%s\nbinary_sha256=%s\nimage_sha256=%s\nsymbols_sha256=%s\n' "$source_sha" "$source_stamp" "$bootstrap_sha" "$slug" "$binary_sha" "$image_sha" "$symbols_sha"
    } > "$artifact_path" || return 1
    form_cli_native_verify_platform_attestation "$artifact_path" "$source_sha" "$source_stamp" "$bootstrap" "$slug" "$binary" "$image" "$symbols"
}

form_cli_native_verify_platform_attestation() {
    local artifact_path="$1" source_sha="$2" source_stamp="$3" bootstrap="$4" slug="$5" binary="$6" image="$7" symbols="$8"
    form_cli_native_load_attestation "$artifact_path" || return 1
    [[ "$FORM_CLI_NATIVE_SCHEMA" == form-cli-native-platform-attestation-v1 \
        && "$FORM_CLI_NATIVE_SOURCE_SHA256" == "$source_sha" && "$FORM_CLI_NATIVE_SOURCE_STAMP" == "$source_stamp" \
        && "$FORM_CLI_NATIVE_PLATFORM_SLUG" == "$slug" \
        && "$FORM_CLI_NATIVE_BOOTSTRAP_ATTESTATION_SHA256" == "$(form_cli_generation_sha256_file "$bootstrap")" \
        && "$FORM_CLI_NATIVE_BINARY_SHA256" == "$(form_cli_generation_sha256_file "$binary")" \
        && "$FORM_CLI_NATIVE_IMAGE_SHA256" == "$(form_cli_generation_sha256_file "$image")" \
        && "$FORM_CLI_NATIVE_SYMBOLS_SHA256" == "$(form_cli_generation_sha256_file "$symbols")" ]]
}

form_cli_source_list() {
    local manifest="form-stdlib/bootstrap/form-cli.dependencies"
    [[ -f "$manifest" && -s "$manifest" && ! -L "$manifest" ]] || {
        printf 'form-cli source dependency manifest missing: %s\n' "$manifest" >&2
        return 1
    }
    printf '%s\n' "$manifest"
    cat "$manifest"
}

form_cli_load_sources() {
    local source sources
    sources="$(form_cli_source_list)" || return 1
    FORM_CLI_SRCS=()
    while IFS= read -r source; do
        [[ -z "$source" ]] || FORM_CLI_SRCS+=("$source")
    done <<< "$sources"
}

# Bootstrap and platform publication share one lock. A platform trio is
# meaningful relative to one exact startup and its authoring attestation,
# so neither publisher may move those artifacts while
# the other is between verification and publication.
form_cli_publish_lock_acquire() {
    local lock_dir="${1:-form-stdlib/bootstrap/.form-cli-publish.lock}"
    local owner_file="$lock_dir/owner.pid" owner_pid=""

    if ! mkdir "$lock_dir" 2>/dev/null; then
        # `mkdir` is the atomic ownership claim.  There is necessarily a
        # tiny interval before its winner can publish owner.pid.  Treat an
        # uninitialised or malformed lock as busy, rather than reclaiming it:
        # otherwise a second publisher can remove a live winner's directory
        # in that interval and both publishers proceed.
        [[ -r "$owner_file" ]] || {
            printf 'form-cli publish: lock %s is initialising or ownerless; refusing concurrent reclaim\n' \
                "$lock_dir" >&2
            return 75
        }
        IFS= read -r owner_pid < "$owner_file" || true
        [[ "$owner_pid" =~ ^[0-9]+$ ]] || {
            printf 'form-cli publish: lock %s has malformed owner; refusing concurrent reclaim\n' \
                "$lock_dir" >&2
            return 75
        }
        if kill -0 "$owner_pid" 2>/dev/null; then
            printf 'form-cli publish: another publisher owns %s (pid %s)\n' \
                "$lock_dir" "$owner_pid" >&2
            return 75
        fi
        # Automatic stale-PID reclamation re-opens the same race: two
        # contenders can both observe a dead owner and each recreate the
        # directory before the other writes its new owner.  Recovery is an
        # explicit maintainer action after inspecting the stopped publisher;
        # canonical publication chooses a visible pause over a mixed carrier.
        printf 'form-cli publish: lock %s names stopped pid %s; manual recovery required\n' \
            "$lock_dir" "$owner_pid" >&2
        return 75
    fi
    printf '%s\n' "$$" > "$owner_file" || {
        # Redirection may already have created a partial owner file. This
        # process owns the directory from mkdir, so release that file first.
        if ! rm -f "$owner_file" || ! rmdir "$lock_dir"; then
            printf 'form-cli publish: cannot release incomplete owned lock %s\n' "$lock_dir" >&2
        fi
        printf 'form-cli publish: cannot initialise lock %s\n' "$lock_dir" >&2
        return 75
    }
    FORM_CLI_PUBLISH_LOCK_DIR="$lock_dir"
    FORM_CLI_PUBLISH_LOCK_OWNER="$owner_file"
}

form_cli_publish_lock_release() {
    [[ -n "${FORM_CLI_PUBLISH_LOCK_OWNER:-}" ]] || return 0
    rm -f "$FORM_CLI_PUBLISH_LOCK_OWNER"
    rmdir "${FORM_CLI_PUBLISH_LOCK_DIR:-}" 2>/dev/null || true
    FORM_CLI_PUBLISH_LOCK_DIR=""
    FORM_CLI_PUBLISH_LOCK_OWNER=""
}

# Every artifact identity below is SHA-256 over one regular file.
form_cli_generation_sha256_file() {
    local file_path="$1"
    [[ -f "$file_path" && ! -L "$file_path" ]] || {
        printf 'form-cli generation attestation: regular file required: %s\n' "$file_path" >&2
        return 1
    }
    if command -v shasum >/dev/null 2>&1; then
        shasum -a 256 "$file_path" | awk '{print $1}'
    elif command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$file_path" | awk '{print $1}'
    else
        printf '%s\n' 'form-cli generation attestation: SHA-256 tool unavailable' >&2
        return 1
    fi
}

form_cli_generation_hash_valid() {
    [[ "$1" =~ ^[0-9a-f]{64}$ ]]
}

form_cli_generation_stamp_valid() {
    [[ "$1" =~ ^[0-9a-f]{16}$ ]]
}

# The host platform names the published bundle form-cli-<slug>.
form_cli_platform_slug() {
    local os arch
    os="$(uname -s 2>/dev/null | tr '[:upper:]' '[:lower:]')"
    arch="$(uname -m 2>/dev/null)"
    case "$arch" in
        x86_64|amd64) arch="amd64" ;;
        aarch64|arm64) arch="arm64" ;;
    esac
    case "$os" in
        darwin) printf 'darwin-%s' "$arch" ;;
        linux) printf 'linux-%s' "$arch" ;;
        mingw*|msys*|cygwin*) printf 'windows-%s' "$arch" ;;
        *) printf '%s-%s' "$os" "$arch" ;;
    esac
}

# The source stamp: the first 16 hex of a digest over the listed files' bytes,
# CR stripped so LF and CRLF checkouts agree; an absent file folds as a named
# line. The closure door writes the same fold as stamp.bytes.
form_cli_hash_stdin() {
    if command -v shasum >/dev/null 2>&1 && printf test | shasum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | shasum | cut -c1-16
    elif command -v sha1sum >/dev/null 2>&1 && printf test | sha1sum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | sha1sum | cut -c1-16
    elif command -v sha256sum >/dev/null 2>&1 && printf test | sha256sum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | sha256sum | cut -c1-16
    elif command -v cksum >/dev/null 2>&1 && printf test | cksum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | cksum | cut -c1-16
    else
        printf '%s\n' 'form-cli stamp: need shasum, sha1sum, sha256sum, or cksum' >&2
        return 1
    fi
}

form_cli_hash16() {
    local f
    for f in "$@"; do
        if [[ -f "$f" ]]; then cat "$f"; else printf 'absent:%s\n' "$f"; fi
    done | form_cli_hash_stdin
}
