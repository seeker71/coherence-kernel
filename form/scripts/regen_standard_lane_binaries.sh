#!/usr/bin/env zsh
# regen_standard_lane_binaries.sh — maintainer bridge that refreshes fkwu and
# form-cli platform bootstrap binaries from the canonical kernel checkout.
# Runtime and standard-lane receipts remain fkwu-native and toolchain-free.
set -euo pipefail

FORM="$(cd -P "$(dirname "$0")/.." && pwd)"
cd "$FORM"
export GO_BIN="$FORM/form-kernel-go/bin-go"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
# shellcheck source=scripts/form_cli_bootstrap_proof.sh
source scripts/form_cli_bootstrap_proof.sh
# shellcheck source=scripts/form_cli_source_list.sh
source scripts/form_cli_source_list.sh
form_cli_load_sources

command -v clang >/dev/null 2>&1 || {
    printf '%s\n' 'maintainer regen requires clang; runtime/standard lane does not' >&2
    exit 1
}

slug="$(fourth_platform_slug)"
mkdir -p form-stdlib/bootstrap
# Hold the same lock as bootstrap regeneration through fkwu, link, attestation,
# and platform stamp publication.  The platform carrier is only valid for the
# exact bootstrap identity it was linked from.
form_cli_publish_lock_acquire "form-stdlib/bootstrap/.form-cli-publish.lock"
platform_attestation_tmp=""
cleanup_standard_lane_regen() {
    if [[ -n "$platform_attestation_tmp" ]]; then
        rm -f "$platform_attestation_tmp"
    fi
    form_cli_publish_lock_release
}
trap cleanup_standard_lane_regen EXIT INT TERM

FORM_STANDARD_LANE=0 build_fourth
fkwu_stamp="$(fourth_fkwu_cache_stamp)"
fkwu_out="$FOURTH_DIR/fkwu-$fkwu_stamp"
[[ -x "$fkwu_out" ]] || {
    printf '%s\n' 'fkwu build failed' >&2
    exit 1
}
cp "$fkwu_out" "form-stdlib/bootstrap/fkwu-${slug}"
printf '%s\n' "$fkwu_stamp" > "form-stdlib/bootstrap/fkwu-${slug}.stamp"
printf 'regen: fkwu-%s (%s bytes) stamp=%s\n' \
    "$slug" \
    "$(wc -c < "form-stdlib/bootstrap/fkwu-${slug}" | tr -d ' ')" \
    "$fkwu_stamp"

form_cli_stamp="$(fourth_hash16 "${FORM_CLI_SRCS[@]}")"
want_form_cli_source_sha256="$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")"
bootstrap_table="form-stdlib/bootstrap/form-cli-table.txt"
bootstrap_emitted_c="form-stdlib/bootstrap/form-cli-emitted.c"
bootstrap_attestation="form-stdlib/bootstrap/form-cli.generation.attestation"
source_identity_still_current() {
    [[ "$(fourth_hash16 "${FORM_CLI_SRCS[@]}")" == "$form_cli_stamp" \
        && "$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")" == "$want_form_cli_source_sha256" ]]
}
verify_bootstrap_current() {
    source_identity_still_current \
        && form_cli_verify_bootstrap \
        "$bootstrap_table" "$bootstrap_emitted_c" \
        form-stdlib/bootstrap/form-cli.stamp "$form_cli_stamp" \
        && form_cli_verify_source_digest \
            form-stdlib/bootstrap/form-cli.source.sha256 "$want_form_cli_source_sha256" \
        && form_cli_verify_generation_attestation \
            "$bootstrap_attestation" "$want_form_cli_source_sha256" "$form_cli_stamp" \
            "$bootstrap_table" "$bootstrap_emitted_c" "not-applicable"
}
verify_bootstrap_current || {
    printf '%s\n' 'regen: bootstrap generation attestation missing or mixed; refresh bootstrap first' >&2
    exit 1
}

# A maintainer regeneration must relink the carrier even when the Form program
# stamp is unchanged: the binary's self-source genesis also includes the build
# and behavioral-proof scripts, whose bytes are intentionally outside the
# executable Form table hash.
# Recheck at the last safe point before link; the shared publisher lock keeps
# the identity fixed until the platform attestation and stamp land.
verify_bootstrap_current || {
    printf '%s\n' 'regen: bootstrap changed before platform link; refusing mixed carrier' >&2
    exit 1
}
FORM_STANDARD_LANE=0 FORM_CLI_FORCE_LINK=1 FORM_CLI_CANONICAL_PUBLISH=1 \
    ./build-form-cli.sh
[[ -x form-cli ]] || {
    printf '%s\n' 'form-cli build failed' >&2
    exit 1
}
form_cli_verify_binary_identity form-cli "$want_form_cli_source_sha256"
verify_bootstrap_current || {
    printf '%s\n' 'regen: bootstrap changed during platform link; refusing mixed carrier' >&2
    exit 1
}
cp form-cli "form-stdlib/bootstrap/form-cli-${slug}"
form_cli_verify_binary_identity "form-stdlib/bootstrap/form-cli-${slug}" "$want_form_cli_source_sha256"
platform_attestation_tmp="$(mktemp "form-stdlib/bootstrap/.form-cli-${slug}.generation.attestation.XXXXXX")"
# Recheck immediately before the platform evidence is written.  The attestation
# pins this exact bootstrap attestation file, not only inherited field values.
verify_bootstrap_current || {
    printf '%s\n' 'regen: bootstrap changed before platform attestation; refusing mixed carrier' >&2
    exit 1
}
form_cli_write_platform_generation_attestation \
    "$bootstrap_attestation" "$platform_attestation_tmp" "$slug" \
    "form-stdlib/bootstrap/form-cli-${slug}" "$bootstrap_table" "$bootstrap_emitted_c"
form_cli_verify_generation_attestation \
    "$platform_attestation_tmp" "$want_form_cli_source_sha256" "$form_cli_stamp" \
    "$bootstrap_table" "$bootstrap_emitted_c" "$slug" "form-stdlib/bootstrap/form-cli-${slug}" \
    "$bootstrap_attestation"
verify_bootstrap_current || {
    printf '%s\n' 'regen: bootstrap changed before platform stamp; refusing mixed carrier' >&2
    exit 1
}
mv -f "$platform_attestation_tmp" "form-stdlib/bootstrap/form-cli-${slug}.generation.attestation"
platform_attestation_tmp=""
form_cli_verify_generation_attestation \
    "form-stdlib/bootstrap/form-cli-${slug}.generation.attestation" \
    "$want_form_cli_source_sha256" "$form_cli_stamp" "$bootstrap_table" "$bootstrap_emitted_c" \
    "$slug" "form-stdlib/bootstrap/form-cli-${slug}" "$bootstrap_attestation"
verify_bootstrap_current || {
    printf '%s\n' 'regen: bootstrap changed before platform stamp; refusing mixed carrier' >&2
    exit 1
}
printf '%s\n' "$form_cli_stamp" > "form-stdlib/bootstrap/form-cli-${slug}.stamp"
printf 'regen: form-cli-%s (%s bytes) stamp=%s attestation=form-stdlib/bootstrap/form-cli-%s.generation.attestation\n' \
    "$slug" \
    "$(wc -c < "form-stdlib/bootstrap/form-cli-${slug}" | tr -d ' ')" \
    "$form_cli_stamp" "$slug"
