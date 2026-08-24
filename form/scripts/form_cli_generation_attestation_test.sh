#!/usr/bin/env bash
# form_cli_generation_attestation_test.sh — content-blind checks for the
# bootstrap/platform attestation parser and its mixed-artifact refusal.
set -euo pipefail

cd "$(dirname "$0")/.."
# shellcheck source=scripts/form_cli_source_list.sh
source scripts/form_cli_source_list.sh

work_dir="$(mktemp -d "${TMPDIR:-/tmp}/form-cli-generation-attestation.XXXXXX")"
cleanup() {
    rm -rf "$work_dir"
}
trap cleanup EXIT INT TERM

table="$work_dir/table.txt"
emitted_c="$work_dir/form-cli-emitted.c"
platform="$work_dir/form-cli-darwin-arm64"
bootstrap_attestation="$work_dir/bootstrap.attestation"
platform_attestation="$work_dir/platform.attestation"
duplicate_attestation="$work_dir/duplicate.attestation"
alternate_bootstrap_attestation="$work_dir/alternate-bootstrap.attestation"
lock_dir="$work_dir/publish.lock"
source_list_file="$work_dir/source-list.txt"

printf '%s\n' '1 0 1 0 0 0 0' > "$table"
printf '%s\n' 'static const char fk_prog[] = "1 0 1 0 0 0 0";' > "$emitted_c"
printf '%s\n' '#!/bin/sh' 'exit 0' > "$platform"
chmod 700 "$platform"

source_sha256='aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
source_stamp='aaaaaaaaaaaaaaaa'
bml_compiler_sha256='bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb'
flattener_sha256='cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc'

# The Metal route's declared static closure belongs to the emitted identity;
# host blobs, environment selection, and SDK contents remain outside this
# claim until they are separately staged and bound.
form_cli_source_list > "$source_list_file"
form_table_count="$(awk '$0 == "form-stdlib/form-table-text.fk" { n = n + 1 } END { print n + 0 }' "$source_list_file")"
host_emit_line="$(awk '$0 == "form-stdlib/host-io-fs-fkwu-emit.fk" { print NR; exit }' "$source_list_file")"
form_table_line="$(awk '$0 == "form-stdlib/form-table-text.fk" { print NR; exit }' "$source_list_file")"
serializer_line="$(awk '$0 == "form-stdlib/fkc-table-serialize.fk" { print NR; exit }' "$source_list_file")"
if [[ "$form_table_count" != 1 || -z "$host_emit_line" || -z "$form_table_line" || -z "$serializer_line" \
        || "$host_emit_line" -ge "$form_table_line" || "$form_table_line" -ge "$serializer_line" ]]; then
    printf '%s\n' 'form_cli_generation_attestation_test: form-table-text source identity/order is invalid' >&2
    exit 1
fi
for required_source in \
    native/metal/metal_ask.sh native/metal/metal_first_token.sh \
    native/metal/ask-declared-cost.fk native/metal/first-token.fk native/metal/whole-tensor-residency.fk \
    form-stdlib/q6k-msl.fk form-stdlib/q4k-msl.fk form-stdlib/transformer-numerics.fk \
    form-stdlib/transformer-block.fk form-stdlib/llama-numerics.fk form-stdlib/trig.fk \
    form-stdlib/tensor-ir.fk form-stdlib/jit-tensor-emit.fk form-stdlib/llama-decode-msl.fk \
    form-stdlib/qk-matvec-split.fk form-stdlib/qk-matvec-lane.fk \
    form-stdlib/qk-matvec-slot.fk form-stdlib/qk-matmul-batch.fk \
    form-stdlib/form-table-text.fk; do
    grep -Fxq "$required_source" "$source_list_file" || {
        printf 'form_cli_generation_attestation_test: source identity misses %s\n' "$required_source" >&2
        exit 1
    }
done

form_cli_write_generation_attestation \
    "$bootstrap_attestation" "$source_sha256" "$source_stamp" "$table" "$emitted_c" \
    "$bml_compiler_sha256" rust-form-kernel "$flattener_sha256" \
    not-applicable not-applicable not-applicable
form_cli_verify_generation_attestation \
    "$bootstrap_attestation" "$source_sha256" "$source_stamp" "$table" "$emitted_c" \
    not-applicable

form_cli_write_platform_generation_attestation \
    "$bootstrap_attestation" "$platform_attestation" darwin-arm64 "$platform" \
    "$table" "$emitted_c"
form_cli_verify_generation_attestation \
    "$platform_attestation" "$source_sha256" "$source_stamp" "$table" "$emitted_c" \
    darwin-arm64 "$platform" "$bootstrap_attestation"

# A current source identity cannot accept an otherwise well-shaped old table.
printf '%s\n' '1 0 1 1 0 0 0' > "$table"
if form_cli_verify_generation_attestation \
        "$bootstrap_attestation" "$source_sha256" "$source_stamp" "$table" "$emitted_c" \
        not-applicable >/dev/null 2>&1; then
    printf '%s\n' 'form_cli_generation_attestation_test: mixed table unexpectedly accepted' >&2
    exit 1
fi
printf '%s\n' '1 0 1 0 0 0 0' > "$table"

# Matching source/table/C hashes are not enough: platform evidence must bind
# the exact bootstrap attestation, including the recorded author identity.
form_cli_write_generation_attestation \
    "$alternate_bootstrap_attestation" "$source_sha256" "$source_stamp" "$table" "$emitted_c" \
    'dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd' \
    rust-form-kernel "$flattener_sha256" \
    not-applicable not-applicable not-applicable
if form_cli_verify_generation_attestation \
        "$platform_attestation" "$source_sha256" "$source_stamp" "$table" "$emitted_c" \
        darwin-arm64 "$platform" "$alternate_bootstrap_attestation" >/dev/null 2>&1; then
    printf '%s\n' 'form_cli_generation_attestation_test: changed author identity unexpectedly accepted' >&2
    exit 1
fi

# Duplicate fields are not a harmless alternate spelling of the identity.
cp "$bootstrap_attestation" "$duplicate_attestation"
printf 'source_sha256=%s\n' "$source_sha256" >> "$duplicate_attestation"
if (
    form_cli_generation_load_attestation "$duplicate_attestation"
) >/dev/null 2>&1; then
    printf '%s\n' 'form_cli_generation_attestation_test: duplicate field unexpectedly accepted' >&2
    exit 1
fi

# A platform attestation cannot be substituted for the bootstrap evidence.
if form_cli_verify_generation_attestation \
        "$platform_attestation" "$source_sha256" "$source_stamp" "$table" "$emitted_c" \
        not-applicable >/dev/null 2>&1; then
    printf '%s\n' 'form_cli_generation_attestation_test: platform attestation accepted as bootstrap' >&2
    exit 1
fi

# A second publisher cannot enter while the first holds the shared lock.
form_cli_publish_lock_acquire "$lock_dir"
if (
    source scripts/form_cli_source_list.sh
    form_cli_publish_lock_acquire "$lock_dir"
) >/dev/null 2>&1; then
    printf '%s\n' 'form_cli_generation_attestation_test: shared publisher lock unexpectedly accepted' >&2
    exit 1
fi
form_cli_publish_lock_release

# A contender that arrives after mkdir but before owner publication must not
# reclaim the directory.  This emulates the only ownership-publication window
# and keeps the test from accepting two overlapping publishers.
mkdir "$lock_dir"
if (
    source scripts/form_cli_source_list.sh
    form_cli_publish_lock_acquire "$lock_dir"
) >/dev/null 2>&1; then
    printf '%s\n' 'form_cli_generation_attestation_test: ownerless shared lock unexpectedly reclaimed' >&2
    exit 1
fi
[[ -d "$lock_dir" && ! -e "$lock_dir/owner.pid" ]] || {
    printf '%s\n' 'form_cli_generation_attestation_test: ownerless shared lock was altered' >&2
    exit 1
}
rmdir "$lock_dir"

# A stopped owner is not automatically reclaimed: two publishers could both
# see the same dead PID and recreate each other's fresh lock.  Maintenance
# recovery is deliberate, after the stopped publisher has been inspected.
mkdir "$lock_dir"
printf '%s\n' 999999999 > "$lock_dir/owner.pid"
if (
    source scripts/form_cli_source_list.sh
    form_cli_publish_lock_acquire "$lock_dir"
) >/dev/null 2>&1; then
    printf '%s\n' 'form_cli_generation_attestation_test: dead-owner shared lock unexpectedly reclaimed' >&2
    exit 1
fi
[[ -r "$lock_dir/owner.pid" ]] || {
    printf '%s\n' 'form_cli_generation_attestation_test: dead-owner shared lock was altered' >&2
    exit 1
}
rm -f "$lock_dir/owner.pid"
rmdir "$lock_dir"

# The canonical publisher cannot inherit arbitrary extra objects or linker
# flags.  This check deliberately reaches the build entry point only far
# enough to exercise the fail-closed gate; it does not compile or publish.
for extra_name in FORM_CLI_EXTRA_SRC FORM_CLI_EXTRA_LDFLAGS; do
    if env "$extra_name=$work_dir/unbound-extra" FORM_CLI_CANONICAL_PUBLISH=1 \
        ./build-form-cli.sh "$work_dir/noncanonical-output" >/dev/null 2>&1; then
        printf 'form_cli_generation_attestation_test: canonical publish accepted %s\n' \
            "$extra_name" >&2
        exit 1
    fi
done

printf '%s\n' 'form_cli_generation_attestation_test: PASS'
