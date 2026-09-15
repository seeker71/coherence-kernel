#!/usr/bin/env zsh
# Regenerate the current native startup and its sealed Form source identity.
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

work_dir=""
cleanup() {
    local exit_status="${1:-$?}"
    # A failed candidate is normally transient.  An explicit diagnostic run may
    # retain its private work directory so the exact candidate and compiler evidence
    # can be re-observed before any repair is attempted.  Successful
    # publications always clean it, and the publisher lock is always released.
    if [[ -n "$work_dir" ]]; then
        if [[ "$exit_status" -ne 0 && "${FORM_CLI_RETAIN_WORKDIR:-0}" == "1" ]]; then
            printf 'regen: retained failed work dir %s\n' "$work_dir" >&2
        else
            rm -rf "$work_dir"
        fi
        work_dir=""
    fi
    form_cli_publish_lock_release
    return 0
}
# Zsh ERR_EXIT from a failed function does not run EXIT. ZERR releases the
# publisher lock on that path while the shell preserves the failed status.
trap cleanup ZERR EXIT
trap 'cleanup 130; exit 130' INT
trap 'cleanup 143; exit 143' TERM
mkdir -p form-stdlib/bootstrap
# Bootstrap and platform publishers share ownership through final stamps.
form_cli_publish_lock_acquire "form-stdlib/bootstrap/.form-cli-publish.lock"
work_dir="$(mktemp -d)"

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

# Form derives the manifest, digest and admitted carrier from the same held
# bytes. Checked private files, not stdout, acknowledge complete publication.
seal_dir="$work_dir/source-seal"
{ printf '%s\n' FCSC1 "$FORM" "$seal_dir" ""; form_cli_source_roots; } > "$work_dir/source-roots"
if ! (cd "$FORM/.." && "$FOURTH_SOURCE_FKWU" form/form-stdlib/bml/form-cli-source-closure.bml) \
        < "$work_dir/source-roots" > "$work_dir/source-closure.log" 2> "$work_dir/source-closure.err"; then
    printf '%s\n' 'regen: native source dependency closure refused' >&2
    cat "$work_dir/source-closure.err" >&2
    exit 1
fi
[[ -s "$seal_dir/ready" && -s "$seal_dir/sources.json" && -s "$seal_dir/source.sha256" ]] || {
    printf '%s\n' 'regen: native source seal completion is absent' >&2
    exit 1
}
source_seal="$seal_dir/sources.json"
source_seal_sha256="$(cat "$seal_dir/ready")"
form_cli_generation_hash_valid "$source_seal_sha256"
printf '%s\n' FCSI1 "$source_seal" "$source_seal_sha256" END > "$work_dir/source-install.request"
if ! (cd "$FORM/.." && "$FOURTH_SOURCE_FKWU" form/form-stdlib/bml/form-cli-source-closure.bml) \
        < "$work_dir/source-install.request" > "$work_dir/source-install.log" 2> "$work_dir/source-install.err"; then
    printf '%s\n' 'regen: checked source manifest publication refused' >&2
    cat "$work_dir/source-install.err" >&2
    exit 1
fi
form_cli_load_sources

want_cli_stamp="$(_fourth_hash_stdin < "$seal_dir/stamp.bytes")"
want_source_sha256="$(cat "$seal_dir/source.sha256")"
form_cli_generation_hash_valid "$want_source_sha256"
source_identity_still_current() {
    printf '%s\n' FCSV1 "$source_seal" "$source_seal_sha256" END > "$work_dir/source-verify.request"
    if ! (cd "$FORM/.." && "$FOURTH_SOURCE_FKWU" form/form-stdlib/bml/form-cli-source-closure.bml) \
            < "$work_dir/source-verify.request" > "$work_dir/source-verify.log" 2> "$work_dir/source-verify.err"; then
        cat "$work_dir/source-verify.err" >&2
        return 1
    fi
    [[ "$(fourth_hash16 "${FORM_CLI_SRCS[@]}")" == "$want_cli_stamp" \
        && "$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")" == "$want_source_sha256" ]]
}
# Form emits the native startup, baked source identity and exact genesis bytes
# from the sealed snapshot. The normal builder admits the companion recipe.
emission_dir="$work_dir/native-entry"
printf '%s\n' FCSE1 "$source_seal" "$source_seal_sha256" "$emission_dir" END > "$work_dir/emission.request"
if ! (cd "$FORM/.." && "$FOURTH_SOURCE_FKWU" form/form-stdlib/bml/form-cli-source-closure.bml) \
        < "$work_dir/emission.request" > "$work_dir/emission.log" 2> "$work_dir/emission.err"; then
    printf '%s\n' 'regen: sealed native startup emission refused' >&2
    cat "$work_dir/emission.err" >&2
    exit 1
fi
[[ -s "$emission_dir/ready" && -s "$emission_dir/form-cli-native.c" && -s "$emission_dir/runtime-source.c" ]] || {
    printf '%s\n' 'regen: native startup completion is absent' >&2
    exit 1
}
candidate_bootstrap="$work_dir/bootstrap"
mkdir "$candidate_bootstrap"
emission_sha256="$(cat "$emission_dir/ready")"
form_cli_generation_hash_valid "$emission_sha256"
emitted_source_check() {
    printf '%s\n' "$1" "$emission_dir/emission.json" "$emission_sha256" "$candidate_bootstrap/form-cli-native.c" END > "$work_dir/emission-check.request"
    if ! (cd "$FORM/.." && "$FOURTH_SOURCE_FKWU" form/form-stdlib/bml/form-cli-source-closure.bml) \
            < "$work_dir/emission-check.request" > "$work_dir/emission-check.log" 2> "$work_dir/emission-check.err"; then
        cat "$work_dir/emission-check.err" >&2
        return 1
    fi
}
emitted_source_check FCEP1
cp "$seal_dir/source.sha256" "$candidate_bootstrap/form-cli.source.sha256"
printf '%s\n' "$want_cli_stamp" > "$candidate_bootstrap/form-cli.stamp"
form_cli_native_write_attestation \
    "$candidate_bootstrap/form-cli.native.attestation" "$want_source_sha256" "$want_cli_stamp" \
    "$candidate_bootstrap/form-cli-native.c" "$bml_compiler_sha256" "$emission_dir/runtime-source.c"
source_identity_still_current || {
    printf '%s\n' 'regen: canonical source identity changed before candidate admission' >&2
    exit 1
}

# The candidate build uses these exact owned originals. Its compiler children
# stay within the snapshot; no root expression executes during compilation.
FORM_CLI_NATIVE_BOOTSTRAP_DIR="$candidate_bootstrap" \
FORM_CLI_FORCE_LINK=1 \
FORM_FOURTH_SOURCE_FKWU="$FOURTH_SOURCE_FKWU" \
FORM_CLI_NATIVE_SOURCE_SNAPSHOT="$seal_dir/body" \
FORM_CLI_NATIVE_SOURCE_SEAL="$source_seal" \
FORM_CLI_NATIVE_SOURCE_SEAL_SHA256="$source_seal_sha256" \
    ./build-form-cli.sh "$work_dir/form-cli"
form_cli_native_verify_platform_attestation \
    "$work_dir/form-cli.native.attestation" "$want_source_sha256" "$want_cli_stamp" \
    "$candidate_bootstrap/form-cli.native.attestation" "$(fourth_platform_slug)" \
    "$work_dir/form-cli" "$work_dir/form-cli.fkb" "$work_dir/form-cli.sym"
emitted_source_check FCEV1
form_cli_native_verify_attestation \
    "$candidate_bootstrap/form-cli.native.attestation" "$want_source_sha256" "$want_cli_stamp" \
    "$candidate_bootstrap/form-cli-native.c" "$emission_dir/runtime-source.c" "$bml_compiler_sha256"
source_identity_still_current || {
    printf '%s\n' 'regen: canonical source or snapshot identity changed during candidate execution' >&2
    exit 1
}

# Readers reject any mixed generation. Publish the complete attestation before
# its final stamp; no active reader uses the historical table runtime.
mv -f "$candidate_bootstrap/form-cli-native.c" form-stdlib/bootstrap/form-cli-native.c
mv -f "$candidate_bootstrap/form-cli.source.sha256" form-stdlib/bootstrap/form-cli.source.sha256
mv -f "$candidate_bootstrap/form-cli.native.attestation" form-stdlib/bootstrap/form-cli.native.attestation
mv -f "$candidate_bootstrap/form-cli.stamp" form-stdlib/bootstrap/form-cli.stamp
printf 'regen: native startup and sealed recipe accepted, source=%s stamp=%s\n' "$want_source_sha256" "$want_cli_stamp"
