#!/usr/bin/env bash
# Shared integrity checks for the committed form-cli bootstrap carrier.
#
# The source stamp proves which Form source set authored the bootstrap.  The
# table/C comparison separately proves that the emitted carrier embeds that
# exact table.  Both must hold: a matching stamp alone cannot make a truncated
# table or a stale emitted carrier usable.

form_cli_sha256_stream() {
    if command -v shasum >/dev/null 2>&1; then
        shasum -a 256 | awk '{print $1}'
    elif command -v sha256sum >/dev/null 2>&1; then
        sha256sum | awk '{print $1}'
    else
        echo "form-cli bootstrap: SHA-256 tool unavailable" >&2
        return 1
    fi
}

form_cli_source_sha256() {
    local source
    {
        for source in "$@"; do
            printf 'path:%s\nbytes:%s\n' "$source" "$(wc -c < "$source" | tr -d ' ')"
            command cat "$source"
            printf '\n'
        done
    } | form_cli_sha256_stream
}

form_cli_verify_binary_identity() {
    local binary="$1"
    local expected_source_sha256="$2"
    local nonce="000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"
    local known_digest="1cb41be978d307c11bf41602cd11bdac834046103d6fc89a6f69cd0c6c4efb21"
    local expected_id="carrier-id|form-cli-carrier-v2|${expected_source_sha256}|coherence-kernel-fkwu-native-v2"
    local expected_challenge="carrier-challenge-v1|${expected_source_sha256}|${nonce}|${known_digest}"
    local actual_id actual_challenge

    # A slashless output name (the build script default is `form-cli`) would
    # otherwise be resolved through PATH and could challenge an unrelated host
    # wrapper instead of the executable that was just linked in this directory.
    [[ "$binary" == */* ]] || binary="./$binary"

    [[ -x "$binary" ]] || {
        echo "form-cli bootstrap: binary is missing or not executable: $binary" >&2
        return 1
    }
    actual_id="$(printf 'carrier-id\n' | "$binary")"
    actual_challenge="$(printf 'carrier-challenge %s\n' "$nonce" | "$binary")"
    if [[ "$actual_id" != "$expected_id" ]]; then
        printf 'form-cli bootstrap: carrier-id mismatch\n  have=%s\n  want=%s\n' \
            "$actual_id" "$expected_id" >&2
        return 1
    fi
    if [[ "$actual_challenge" != "$expected_challenge" ]]; then
        printf 'form-cli bootstrap: carrier-challenge mismatch\n  have=%s\n  want=%s\n' \
            "$actual_challenge" "$expected_challenge" >&2
        return 1
    fi
}

form_cli_verify_source_digest() {
    local digest_file="$1"
    local expected="$2"
    local actual
    actual="$(cat "$digest_file" 2>/dev/null || true)"
    if [[ "$actual" != "$expected" ]]; then
        printf 'form-cli bootstrap: full source digest stale (have=%s want=%s)\n' \
            "${actual:-missing}" "$expected" >&2
        return 1
    fi
}

form_cli_validate_table() {
    local table="$1"
    LC_ALL=C awk '
        function die(message) {
            print "form-cli bootstrap: invalid table: " message > "/dev/stderr"
            failed = 1
            exit 1
        }
        {
            for (field = 1; field <= NF; field++) {
                if ($field !~ /^-?[0-9]+$/) {
                    die("non-numeric token")
                }
                token[++token_count] = $field + 0
            }
        }
        END {
            if (failed) {
                exit 1
            }
            cursor = 1
            if (token_count < 1) {
                die("empty")
            }
            function_count = token[cursor++]
            if (function_count < 1 || cursor + function_count - 1 > token_count) {
                die("function roots")
            }
            cursor += function_count
            if (cursor > token_count) {
                die("missing node count")
            }
            node_count = token[cursor++]
            if (node_count < 1 || cursor + (node_count * 4) - 1 > token_count) {
                die("node rows")
            }
            cursor += node_count * 4
            if (cursor > token_count) {
                die("missing string count")
            }
            string_count = token[cursor++]
            if (string_count < 1) {
                die("string count")
            }
            for (string_index = 0; string_index < string_count; string_index++) {
                if (cursor > token_count) {
                    die("missing string length")
                }
                string_length = token[cursor++]
                if (string_length < 0 || cursor + string_length - 1 > token_count) {
                    die("string bytes")
                }
                cursor += string_length
            }
            if (cursor != token_count + 1) {
                die("trailing tokens")
            }
            printf "functions=%d nodes=%d strings=%d tokens=%d\n", \
                function_count, node_count, string_count, token_count
        }
    ' "$table"
}

form_cli_extract_emitted_table() {
    local emitted_c="$1"
    local output_table="$2"
    local output_tmp
    output_tmp="$(mktemp "${output_table}.tmp.XXXXXX")"

    if ! LC_ALL=C awk '
        BEGIN {
            prefix = "static const char fk_prog[] = \""
            suffix = "\"; extern const unsigned char fk_genesis[]"
        }
        {
            start = index($0, prefix)
            if (start > 0) {
                payload = substr($0, start + length(prefix))
                finish = index(payload, suffix)
                if (finish < 1) {
                    exit 2
                }
                print substr(payload, 1, finish - 1)
                found = 1
                exit
            }
        }
        END {
            if (!found) {
                exit 1
            }
        }
    ' "$emitted_c" > "$output_tmp"; then
        rm -f "$output_tmp"
        echo "form-cli bootstrap: emitted C has no complete fk_prog carrier" >&2
        return 1
    fi

    if ! form_cli_validate_table "$output_tmp" >/dev/null; then
        rm -f "$output_tmp"
        return 1
    fi
    mv -f "$output_tmp" "$output_table"
}

form_cli_table_matches_emitted() {
    local table="$1"
    local emitted_c="$2"
    local extracted
    local result=0
    extracted="$(mktemp "${TMPDIR:-/tmp}/form-cli-carrier.XXXXXX")"

    if ! form_cli_extract_emitted_table "$emitted_c" "$extracted"; then
        result=1
    elif ! cmp -s "$table" "$extracted"; then
        echo "form-cli bootstrap: table does not exactly match emitted C fk_prog" >&2
        result=1
    fi
    rm -f "$extracted"
    return "$result"
}

form_cli_verify_bootstrap() {
    local table="$1"
    local emitted_c="$2"
    local stamp_file="$3"
    local expected_stamp="$4"
    local actual_stamp
    actual_stamp="$(cat "$stamp_file" 2>/dev/null || true)"

    if [[ "$actual_stamp" != "$expected_stamp" ]]; then
        printf 'form-cli bootstrap: source stamp stale (have=%s want=%s)\n' \
            "${actual_stamp:-missing}" "$expected_stamp" >&2
        return 1
    fi
    form_cli_validate_table "$table" >/dev/null || return 1
    form_cli_table_matches_emitted "$table" "$emitted_c" || return 1
}

form_cli_sha256_file() {
    local path="$1"
    if command -v shasum >/dev/null 2>&1; then
        shasum -a 256 "$path" | awk '{print $1}'
    elif command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$path" | awk '{print $1}'
    else
        echo "form-cli bootstrap: SHA-256 tool unavailable" >&2
        return 1
    fi
}

form_cli_hmac_sha256_hex() {
    local key_hex="$1"
    local message="$2"
    command -v openssl >/dev/null 2>&1 || {
        echo "form-cli bootstrap: openssl is required for behavioral HMAC proof" >&2
        return 1
    }
    printf '%s' "$message" \
        | openssl dgst -sha256 -mac HMAC -macopt "hexkey:${key_hex}" \
        | awk '{print $NF}'
}

form_cli_assert_file_eq() {
    local actual="$1"
    local expected="$2"
    local label="$3"
    if ! cmp -s "$actual" "$expected"; then
        printf 'form-cli bootstrap: behavioral proof failed: %s\n' "$label" >&2
        printf '%s\n' '--- actual (hex, first 512 bytes) ---' >&2
        od -An -v -tx1 -N 512 "$actual" >&2 || true
        printf '%s\n' '--- expected (hex, first 512 bytes) ---' >&2
        od -An -v -tx1 -N 512 "$expected" >&2 || true
        return 1
    fi
}

# Exercise the executable itself from an otherwise empty temporary working
# directory. This is intentionally broader than the recipe bands: it crosses
# native read_file/rename/remove/time, the baked program table, REPL framing,
# the production-sized streaming ranker, and request-bound HMAC verification.
form_cli_behavioral_proof() (
    set -euo pipefail

    local binary="$1"
    local expected_source_sha256="${2:-}"
    local script_dir form_dir root request_dir index_file source_file
    local persisted answer_key answer_hex answer_len output expected answer_file
    local node_id='@1.1.9.41'
    local content_node_id='@8.4.4.12'
    local source_path='docs/grounding.form'
    local query='grounding'
    local query_vec='[753942,1030707]'
    local key_hex='000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f'

    [[ "$binary" == */* ]] || binary="./$binary"
    binary="$(cd -P "$(dirname "$binary")" && pwd)/$(basename "$binary")"
    script_dir="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    form_dir="$(cd -P "$script_dir/.." && pwd)"
    if [[ -z "$expected_source_sha256" ]]; then
        expected_source_sha256="$(tr -d '\r\n' < "$form_dir/form-stdlib/bootstrap/form-cli.source.sha256")"
    fi
    form_cli_verify_binary_identity "$binary" "$expected_source_sha256"

    root="$(mktemp -d "${TMPDIR:-/tmp}/form-cli-behavior.XXXXXX")"
    trap 'rm -rf "$root"' EXIT
    request_dir="$root/.coherence-network/rag-requests"
    index_file="$root/.coherence-network/rag-index/index.jsonl"
    source_file="$root/$source_path"
    mkdir -p "$request_dir" "$(dirname "$index_file")" \
        "$(dirname "$source_file")" "$root/.coherence-network/attestation"
    printf '%s' 'grounding source fixture v2' > "$source_file"
    persisted="$(form_cli_sha256_file "$source_file")"

    write_index_for_answer() {
        answer_file="$1"
        answer_hex="$(od -An -v -tx1 "$answer_file" | tr -d ' \n')"
        answer_key="$(form_cli_sha256_file "$answer_file")"
        printf '{"id":"%s","node_id":"%s","content_node_id":"%s","source_path":"%s","kind":"cell","key":"%s","persisted_source_sha256":"%s","schema":"nodeid-rag-v2","embedding_kind":"form-semantic-v2","snippet":"grounding","answer_key":"%s","answer_hex":"%s","vec":%s}\n' \
            "$node_id" "$node_id" "$content_node_id" "$source_path" \
            "$persisted" "$persisted" "$answer_key" "$answer_hex" "$query_vec" \
            > "$index_file"
    }

    assert_answer_round_trip() {
        local label="$1"
        answer_file="$2"
        write_index_for_answer "$answer_file"
        answer_len="$(wc -c < "$answer_file" | tr -d ' ')"
        output="$root/${label}.out"
        expected="$root/${label}.expected"
        (cd "$root" && printf 'grounded %s\nquit\n' "$query" | "$binary") > "$output"
        {
            printf 'grounded:%s\n' "$node_id"
            printf 'content-node:%s\nsource-path:%s\nsource-key:%s\nanswer-key:%s\n' \
                "$content_node_id" "$source_path" "$persisted" "$answer_key"
            printf '%s\n' \
                'retrieval-score:2' \
                'retrieval-runner-score:0' \
                'retrieval-query-total:2' \
                'retrieval-threshold:2' \
                'retrieval-confidence:100' \
                'local-lane:fkwu-rag-grounded' \
                'synthesis-lane:fkwu-rag-grounded'
            printf 'answer-byte-length:%s\nanswer:' "$answer_len"
            command cat "$answer_file"
            printf '\n'
        } > "$expected"
        form_cli_assert_file_eq "$output" "$expected" "answer framing: $label"
    }

    # Exact answer bytes: no terminator, one/multiple terminal LF, an embedded
    # metadata-looking marker, UTF-8, and NUL all survive with one REPL framing LF.
    printf 'plain' > "$root/answer-plain.bin"
    assert_answer_round_trip plain "$root/answer-plain.bin"
    printf 'line\n' > "$root/answer-one-lf.bin"
    assert_answer_round_trip one-lf "$root/answer-one-lf.bin"
    printf 'line\n\n' > "$root/answer-two-lf.bin"
    assert_answer_round_trip two-lf "$root/answer-two-lf.bin"
    printf 'before\nanswer:inside · λ\0after' > "$root/answer-binary.bin"
    assert_answer_round_trip binary "$root/answer-binary.bin"

    # Restore a compact ASCII row for request and signed-trace proofs.
    printf 'A' > "$root/answer-A.bin"
    write_index_for_answer "$root/answer-A.bin"
    printf '%s' "$query" > "$request_dir/direct.query"
    (cd "$root" && printf 'ask-request direct\nquit\n' | "$binary") > "$root/direct.out"
    grep -aFq "grounded:${node_id}" "$root/direct.out" || {
        echo 'form-cli bootstrap: ask-request missed valid v2 row' >&2
        return 1
    }

    # A stale v1 row is absent rather than weakly grounded.
    sed 's/"schema":"nodeid-rag-v2"/"schema":"nodeid-rag-v1"/' \
        "$index_file" > "$root/index-v1.jsonl"
    mv "$root/index-v1.jsonl" "$index_file"
    (cd "$root" && printf 'grounded %s\nquit\n' "$query" | "$binary") > "$root/v1.out"
    printf 'grounded:miss\n' > "$root/v1.expected"
    form_cli_assert_file_eq "$root/v1.out" "$root/v1.expected" 'stale v1 rejection'
    write_index_for_answer "$root/answer-A.bin"

    # Canonical semantic-v2 authoring, a real batch, and the hard 16 MiB gate.
    printf '31|67726f756e64696e67\n' > "$request_dir/embed-one.embed"
    (cd "$root" && printf 'embed-request embed-one\nquit\n' | "$binary") > "$root/embed-one.out"
    printf '%s\n' '{"id_hex":"31","embedding_kind":"form-semantic-v2","vec":[753942,1030707]}' \
        > "$root/embed-one.expected"
    form_cli_assert_file_eq "$root/embed-one.out" "$root/embed-one.expected" 'embed-request canonical vector'
    : > "$request_dir/embed-batch.embed"
    local i id_hex
    for ((i = 1; i <= 1546; i++)); do
        id_hex="$(printf '%x' "$i")"
        (( ${#id_hex} % 2 == 0 )) || id_hex="0${id_hex}"
        printf '%s|67726f756e64696e67\n' "$id_hex" >> "$request_dir/embed-batch.embed"
    done
    (cd "$root" && printf 'embed-request embed-batch\nquit\n' | "$binary") > "$root/embed-batch.out"
    [[ "$(wc -l < "$root/embed-batch.out" | tr -d ' ')" == 1546 ]] || {
        echo 'form-cli bootstrap: embed-request batch row count mismatch' >&2
        return 1
    }
    dd if=/dev/zero of="$request_dir/embed-too-large.embed" bs=1048576 count=17 2>/dev/null
    (cd "$root" && printf 'embed-request embed-too-large\nquit\n' | "$binary") > "$root/embed-too-large.out"
    printf 'embed-request:error:input-too-large\n' > "$root/embed-too-large.expected"
    form_cli_assert_file_eq "$root/embed-too-large.out" "$root/embed-too-large.expected" 'embed-request size cap'

    # Production row count and approximate byte volume exercise bounded-memory
    # rank without constructing an in-memory list of all JSONL rows.
    local pad decoy_index decoy_content
    pad="$(dd if=/dev/zero bs=3200 count=1 2>/dev/null | tr '\000' x)"
    command cp "$index_file" "$root/index-large.jsonl"
    for ((i = 1; i < 1546; i++)); do
        decoy_index=$((1000 + i))
        decoy_content=$((4000 + i))
        printf '{"id":"@1.1.9.%s","node_id":"@1.1.9.%s","content_node_id":"@8.4.4.%s","source_path":"%s","kind":"cell","key":"%s","persisted_source_sha256":"%s","schema":"nodeid-rag-v2","embedding_kind":"form-semantic-v2","snippet":"%s","answer_key":"%s","answer_hex":"41","vec":[1,1000003]}\n' \
            "$decoy_index" "$decoy_index" "$decoy_content" "$source_path" \
            "$persisted" "$persisted" "$pad" "$answer_key" >> "$root/index-large.jsonl"
    done
    mv "$root/index-large.jsonl" "$index_file"
    [[ "$(wc -l < "$index_file" | tr -d ' ')" == 1546 ]] || return 1
    [[ "$(wc -c < "$index_file" | tr -d ' ')" -gt 5000000 ]] || return 1
    (cd "$root" && printf 'grounded %s\nquit\n' "$query" | "$binary") > "$root/large.out"
    grep -aFq "grounded:${node_id}" "$root/large.out" || {
        echo 'form-cli bootstrap: production-sized index missed target' >&2
        return 1
    }
    write_index_for_answer "$root/answer-A.bin"

    # One-use, request-bound dual HMAC attestation produces the complete native
    # trust row. A replay is rejected because rename-to-consuming + removal is
    # the state transition, not because the caller promises uniqueness.
    local request_id='signed'
    local query_sha issued expires grounding_fields grounding_canonical grounding_mac
    local frequency_evidence_canonical frequency_evidence
    local frequency_fields frequency_canonical frequency_mac receipt
    printf '%s\n' "$key_hex" > "$root/.coherence-network/attestation/grounding-v1.key"
    chmod 600 "$root/.coherence-network/attestation/grounding-v1.key"
    printf '%s' "$query" > "$request_dir/${request_id}.query"
    query_sha="$(form_cli_sha256_file "$request_dir/${request_id}.query")"
    issued="$(date +%s)"
    expires=$((issued + 300))
    grounding_fields="${node_id}|${content_node_id}|${persisted}|${source_path}|${answer_key}|${persisted}|${query_sha}|${issued}|${expires}"
    grounding_canonical="grounding-attestation-v2
${request_id}
${node_id}
${content_node_id}
${persisted}
${source_path}
${answer_key}
${persisted}
${query_sha}
${issued}
${expires}"
    grounding_mac="$(form_cli_hmac_sha256_hex "$key_hex" "$grounding_canonical")"
    frequency_evidence_canonical="frequency-cert-v1
${node_id}
${content_node_id}
${answer_key}
success"
    frequency_evidence="$(printf '%s' "$frequency_evidence_canonical" | form_cli_sha256_stream)"
    frequency_fields="${node_id}|${content_node_id}|${answer_key}|${frequency_evidence}|${issued}|${expires}|success"
    frequency_canonical="frequency-attestation-v2
${request_id}
${node_id}
${content_node_id}
${answer_key}
${frequency_evidence}
${issued}
${expires}
success"
    frequency_mac="$(form_cli_hmac_sha256_hex "$key_hex" "$frequency_canonical")"
    receipt="${grounding_fields}|${grounding_mac}|${frequency_fields}|${frequency_mac}"
    printf '%s\n' "$receipt" > "$request_dir/${request_id}.receipt"
    (cd "$root" && printf 'ask-request-trace %s\nquit\n' "$request_id" | "$binary") > "$root/signed.out"
    for signal in 'path:native' 'grounded:yes' 'freq:yes' 'suffic:yes' 'observed:yes'; do
        grep -aFq "$signal" "$root/signed.out" || {
            printf 'form-cli bootstrap: signed trace missing %s\n' "$signal" >&2
            command cat "$root/signed.out" >&2
            return 1
        }
    done
    grep -aFq "grounded:${node_id}" "$root/signed.out" || return 1
    (cd "$root" && printf 'ask-request-trace %s\nquit\n' "$request_id" | "$binary") > "$root/replay.out"
    printf 'ask-request-trace:error:missing-receipt\n' > "$root/replay.expected"
    form_cli_assert_file_eq "$root/replay.out" "$root/replay.expected" 'receipt replay rejection'

    printf 'form-cli behavioral proof: OK (identity, v2/v1, exact bytes, 1546-row rank, embed batch/cap, request HMAC/replay)\n'
)

if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
    if [[ $# -lt 1 || $# -gt 2 ]]; then
        echo "usage: $0 FORM_CLI_BINARY [EXPECTED_SOURCE_SHA256]" >&2
        exit 2
    fi
    form_cli_behavioral_proof "$@"
fi
