#!/usr/bin/env zsh
# form_cli_source_list.sh — one source-identity order for every form-cli
# bootstrap publisher.  Source this only after the caller has cd'd to form/.
#
# These paths describe the direct emitted-carrier identity: its Form program,
# the Form lowering/emission chain that shapes that program, and its fixed
# carrier/link scripts. They are intentionally *not* a Form flatten list:
# non-Form inputs are hashed and carried in genesis, never parsed as Form.

form_cli_source_list() {
    cat <<'EOF'
build-form-cli.sh
scripts/fourth-arm.sh
scripts/form_cli_bootstrap_proof.sh
scripts/regen_form_cli_bootstrap.sh
scripts/regen_standard_lane_binaries.sh
native/metal/fk-metal-carrier.m
native/metal/metal_ask.sh
native/metal/metal_first_token.sh
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
form-stdlib/http-client.fk
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
form-stdlib/form-cli-repl.fk
scripts/form_cli_source_list.sh
EOF
}

form_cli_load_sources() {
    local source
    FORM_CLI_SRCS=()
    while IFS= read -r source; do
        [[ -z "$source" ]] || FORM_CLI_SRCS+=("$source")
    done < <(form_cli_source_list)
}

# Bootstrap and platform-carrier publication share one lock.  A platform
# binary is meaningful only relative to one exact bootstrap table/C and its
# authoring attestation, so neither publisher may move those artifacts while
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
        rmdir "$lock_dir" 2>/dev/null || true
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

# The emitted C/table pair is generated by an authoring toolchain that is not
# present on every runtime host.  Keep its concrete identity beside the
# bootstrap artifacts rather than pretending a source stamp proves which
# author binary made them.  This is deliberately a small, line-oriented
# format: it can be checked by the standard lane without Go, Rust, TypeScript,
# or a Form compiler installed.
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

form_cli_generation_flattener_kind_valid() {
    case "$1" in
        rust-form-kernel|typescript-form-kernel|fkwu-selfhost)
            return 0
            ;;
        *)
            return 1
            ;;
    esac
}

form_cli_generation_reset() {
    FORM_CLI_GENERATION_SCHEMA=""
    FORM_CLI_GENERATION_SOURCE_SHA256=""
    FORM_CLI_GENERATION_SOURCE_STAMP=""
    FORM_CLI_GENERATION_TABLE_SHA256=""
    FORM_CLI_GENERATION_EMITTED_C_SHA256=""
    FORM_CLI_GENERATION_BOOTSTRAP_ATTESTATION_SHA256=""
    FORM_CLI_GENERATION_BML_COMPILER_KIND=""
    FORM_CLI_GENERATION_BML_COMPILER_SHA256=""
    FORM_CLI_GENERATION_FLATTENER_KIND=""
    FORM_CLI_GENERATION_FLATTENER_BINARY_SHA256=""
    FORM_CLI_GENERATION_PLATFORM_SLUG=""
    FORM_CLI_GENERATION_PLATFORM_CARRIER_SHA256=""
}

form_cli_generation_load_attestation() {
    local file_path="$1" line key value
    form_cli_generation_reset
    [[ -f "$file_path" && ! -L "$file_path" ]] || {
        printf 'form-cli generation attestation: missing regular file: %s\n' "$file_path" >&2
        return 1
    }

    while IFS= read -r line || [[ -n "$line" ]]; do
        [[ "$line" == *=* ]] || {
            printf 'form-cli generation attestation: malformed line in %s\n' "$file_path" >&2
            return 1
        }
        key="${line%%=*}"
        value="${line#*=}"
        case "$key" in
            schema)
                [[ -z "$FORM_CLI_GENERATION_SCHEMA" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_SCHEMA="$value"
                ;;
            source_sha256)
                [[ -z "$FORM_CLI_GENERATION_SOURCE_SHA256" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_SOURCE_SHA256="$value"
                ;;
            source_stamp)
                [[ -z "$FORM_CLI_GENERATION_SOURCE_STAMP" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_SOURCE_STAMP="$value"
                ;;
            table_sha256)
                [[ -z "$FORM_CLI_GENERATION_TABLE_SHA256" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_TABLE_SHA256="$value"
                ;;
            emitted_c_sha256)
                [[ -z "$FORM_CLI_GENERATION_EMITTED_C_SHA256" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_EMITTED_C_SHA256="$value"
                ;;
            bootstrap_attestation_sha256)
                [[ -z "$FORM_CLI_GENERATION_BOOTSTRAP_ATTESTATION_SHA256" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_BOOTSTRAP_ATTESTATION_SHA256="$value"
                ;;
            bml_compiler_kind)
                [[ -z "$FORM_CLI_GENERATION_BML_COMPILER_KIND" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_BML_COMPILER_KIND="$value"
                ;;
            bml_compiler_sha256)
                [[ -z "$FORM_CLI_GENERATION_BML_COMPILER_SHA256" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_BML_COMPILER_SHA256="$value"
                ;;
            flattener_kind)
                [[ -z "$FORM_CLI_GENERATION_FLATTENER_KIND" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_FLATTENER_KIND="$value"
                ;;
            flattener_binary_sha256)
                [[ -z "$FORM_CLI_GENERATION_FLATTENER_BINARY_SHA256" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_FLATTENER_BINARY_SHA256="$value"
                ;;
            platform_slug)
                [[ -z "$FORM_CLI_GENERATION_PLATFORM_SLUG" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_PLATFORM_SLUG="$value"
                ;;
            platform_carrier_sha256)
                [[ -z "$FORM_CLI_GENERATION_PLATFORM_CARRIER_SHA256" ]] || {
                    printf 'form-cli generation attestation: duplicate field %s in %s\n' "$key" "$file_path" >&2
                    return 1
                }
                FORM_CLI_GENERATION_PLATFORM_CARRIER_SHA256="$value"
                ;;
            *)
                printf 'form-cli generation attestation: unknown field %s in %s\n' "$key" "$file_path" >&2
                return 1
                ;;
        esac
    done < "$file_path"

    [[ "$FORM_CLI_GENERATION_SCHEMA" == "form-cli-generation-attestation-v1" ]] || {
        printf 'form-cli generation attestation: unsupported or missing schema in %s\n' "$file_path" >&2
        return 1
    }
    form_cli_generation_hash_valid "$FORM_CLI_GENERATION_SOURCE_SHA256" \
        && form_cli_generation_stamp_valid "$FORM_CLI_GENERATION_SOURCE_STAMP" \
        && form_cli_generation_hash_valid "$FORM_CLI_GENERATION_TABLE_SHA256" \
        && form_cli_generation_hash_valid "$FORM_CLI_GENERATION_EMITTED_C_SHA256" \
        && form_cli_generation_hash_valid "$FORM_CLI_GENERATION_BML_COMPILER_SHA256" \
        && form_cli_generation_hash_valid "$FORM_CLI_GENERATION_FLATTENER_BINARY_SHA256" || {
        printf 'form-cli generation attestation: invalid required hash in %s\n' "$file_path" >&2
        return 1
    }
    [[ "$FORM_CLI_GENERATION_BML_COMPILER_KIND" == "go-form-kernel" ]] || {
        printf 'form-cli generation attestation: unexpected BML compiler kind in %s\n' "$file_path" >&2
        return 1
    }
    form_cli_generation_flattener_kind_valid "$FORM_CLI_GENERATION_FLATTENER_KIND" || {
        printf 'form-cli generation attestation: unknown flattener kind in %s\n' "$file_path" >&2
        return 1
    }
    if [[ "$FORM_CLI_GENERATION_PLATFORM_SLUG" == "not-applicable" ]]; then
        [[ "$FORM_CLI_GENERATION_PLATFORM_CARRIER_SHA256" == "not-applicable" \
            && "$FORM_CLI_GENERATION_BOOTSTRAP_ATTESTATION_SHA256" == "not-applicable" ]] || {
            printf 'form-cli generation attestation: bootstrap platform fields disagree in %s\n' "$file_path" >&2
            return 1
        }
    else
        [[ "$FORM_CLI_GENERATION_PLATFORM_SLUG" =~ ^[A-Za-z0-9._-]+$ ]] \
            && form_cli_generation_hash_valid "$FORM_CLI_GENERATION_PLATFORM_CARRIER_SHA256" \
            && form_cli_generation_hash_valid "$FORM_CLI_GENERATION_BOOTSTRAP_ATTESTATION_SHA256" || {
            printf 'form-cli generation attestation: invalid platform carrier fields in %s\n' "$file_path" >&2
            return 1
        }
    fi
}

form_cli_write_generation_attestation() {
    local file_path="$1" source_sha256="$2" source_stamp="$3" table="$4" emitted_c="$5"
    local bml_compiler_sha256="$6" flattener_kind="$7" flattener_binary_sha256="$8"
    local bootstrap_attestation_sha256="$9" platform_slug="${10}" platform_carrier_sha256="${11}"
    local table_sha256 emitted_c_sha256

    form_cli_generation_hash_valid "$source_sha256" \
        && form_cli_generation_stamp_valid "$source_stamp" \
        && form_cli_generation_hash_valid "$bml_compiler_sha256" \
        && form_cli_generation_flattener_kind_valid "$flattener_kind" \
        && form_cli_generation_hash_valid "$flattener_binary_sha256" || {
        printf '%s\n' 'form-cli generation attestation: refusing invalid author identity' >&2
        return 1
    }
    table_sha256="$(form_cli_generation_sha256_file "$table")"
    emitted_c_sha256="$(form_cli_generation_sha256_file "$emitted_c")"
    if [[ "$platform_slug" == "not-applicable" ]]; then
        [[ "$platform_carrier_sha256" == "not-applicable" \
            && "$bootstrap_attestation_sha256" == "not-applicable" ]] || {
            printf '%s\n' 'form-cli generation attestation: bootstrap platform carrier must be not-applicable' >&2
            return 1
        }
    else
        [[ "$platform_slug" =~ ^[A-Za-z0-9._-]+$ ]] \
            && form_cli_generation_hash_valid "$platform_carrier_sha256" \
            && form_cli_generation_hash_valid "$bootstrap_attestation_sha256" || {
            printf '%s\n' 'form-cli generation attestation: refusing invalid platform identity' >&2
            return 1
        }
    fi

    (
        umask 077
        {
        printf '%s\n' 'schema=form-cli-generation-attestation-v1'
        printf 'source_sha256=%s\n' "$source_sha256"
        printf 'source_stamp=%s\n' "$source_stamp"
        printf 'table_sha256=%s\n' "$table_sha256"
        printf 'emitted_c_sha256=%s\n' "$emitted_c_sha256"
        printf 'bootstrap_attestation_sha256=%s\n' "$bootstrap_attestation_sha256"
        printf '%s\n' 'bml_compiler_kind=go-form-kernel'
        printf 'bml_compiler_sha256=%s\n' "$bml_compiler_sha256"
        printf 'flattener_kind=%s\n' "$flattener_kind"
        printf 'flattener_binary_sha256=%s\n' "$flattener_binary_sha256"
        printf 'platform_slug=%s\n' "$platform_slug"
        printf 'platform_carrier_sha256=%s\n' "$platform_carrier_sha256"
        } > "$file_path"
    )
}

form_cli_verify_generation_attestation() {
    local file_path="$1" expected_source_sha256="$2" expected_source_stamp="$3" table="$4" emitted_c="$5"
    local expected_platform_slug="$6" platform_carrier="${7:-}" bootstrap_attestation="${8:-}"
    local actual

    form_cli_generation_load_attestation "$file_path" || return 1
    [[ "$FORM_CLI_GENERATION_SOURCE_SHA256" == "$expected_source_sha256" ]] || {
        printf 'form-cli generation attestation: source digest mismatch in %s\n' "$file_path" >&2
        return 1
    }
    [[ "$FORM_CLI_GENERATION_SOURCE_STAMP" == "$expected_source_stamp" ]] || {
        printf 'form-cli generation attestation: source stamp mismatch in %s\n' "$file_path" >&2
        return 1
    }
    actual="$(form_cli_generation_sha256_file "$table")"
    [[ "$FORM_CLI_GENERATION_TABLE_SHA256" == "$actual" ]] || {
        printf 'form-cli generation attestation: table hash mismatch in %s\n' "$file_path" >&2
        return 1
    }
    actual="$(form_cli_generation_sha256_file "$emitted_c")"
    [[ "$FORM_CLI_GENERATION_EMITTED_C_SHA256" == "$actual" ]] || {
        printf 'form-cli generation attestation: emitted C hash mismatch in %s\n' "$file_path" >&2
        return 1
    }
    if [[ "$expected_platform_slug" == "not-applicable" ]]; then
        [[ "$FORM_CLI_GENERATION_PLATFORM_SLUG" == "not-applicable" \
            && "$FORM_CLI_GENERATION_PLATFORM_CARRIER_SHA256" == "not-applicable" \
            && "$FORM_CLI_GENERATION_BOOTSTRAP_ATTESTATION_SHA256" == "not-applicable" ]] || {
            printf 'form-cli generation attestation: expected bootstrap attestation in %s\n' "$file_path" >&2
            return 1
        }
        return 0
    fi
    [[ "$FORM_CLI_GENERATION_PLATFORM_SLUG" == "$expected_platform_slug" ]] || {
        printf 'form-cli generation attestation: platform slug mismatch in %s\n' "$file_path" >&2
        return 1
    }
    actual="$(form_cli_generation_sha256_file "$platform_carrier")"
    [[ "$FORM_CLI_GENERATION_PLATFORM_CARRIER_SHA256" == "$actual" ]] || {
        printf 'form-cli generation attestation: platform carrier hash mismatch in %s\n' "$file_path" >&2
        return 1
    }
    actual="$(form_cli_generation_sha256_file "$bootstrap_attestation")"
    [[ "$FORM_CLI_GENERATION_BOOTSTRAP_ATTESTATION_SHA256" == "$actual" ]] || {
        printf 'form-cli generation attestation: bootstrap attestation hash mismatch in %s\n' "$file_path" >&2
        return 1
    }
}

form_cli_write_platform_generation_attestation() {
    local bootstrap_attestation="$1" output="$2" platform_slug="$3" platform_carrier="$4"
    local table="$5" emitted_c="$6"
    local platform_carrier_sha256 bootstrap_attestation_sha256
    form_cli_generation_load_attestation "$bootstrap_attestation" || return 1
    [[ "$FORM_CLI_GENERATION_PLATFORM_SLUG" == "not-applicable" ]] || {
        printf 'form-cli generation attestation: bootstrap source is not a bootstrap attestation: %s\n' \
            "$bootstrap_attestation" >&2
        return 1
    }
    platform_carrier_sha256="$(form_cli_generation_sha256_file "$platform_carrier")"
    bootstrap_attestation_sha256="$(form_cli_generation_sha256_file "$bootstrap_attestation")"
    form_cli_write_generation_attestation \
        "$output" "$FORM_CLI_GENERATION_SOURCE_SHA256" \
        "$FORM_CLI_GENERATION_SOURCE_STAMP" "$table" "$emitted_c" \
        "$FORM_CLI_GENERATION_BML_COMPILER_SHA256" \
        "$FORM_CLI_GENERATION_FLATTENER_KIND" "$FORM_CLI_GENERATION_FLATTENER_BINARY_SHA256" \
        "$bootstrap_attestation_sha256" "$platform_slug" "$platform_carrier_sha256"
}
