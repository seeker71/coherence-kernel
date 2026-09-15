#!/usr/bin/env zsh
# Publish the source-runtime CLI, native image and symbols as one platform unit.
# The shared lock holds bootstrap identity through the final platform stamp.
set -euo pipefail
export LC_ALL=C
FORM="$(cd -P "$(dirname "$0")/.." && pwd)"
BODY="$(dirname "$FORM")"
cd "$FORM"
source scripts/fourth-arm.sh
source scripts/form_cli_bootstrap_proof.sh
source scripts/form_cli_source_list.sh
form_cli_load_sources
slug="$(fourth_platform_slug)"
BOOT="$FORM/form-stdlib/bootstrap"
mkdir -p "$BOOT"
W=""
cleanup_standard_lane_regen() {
    local rc=$?
    if [[ -z "$W" ]]; then
        form_cli_publish_lock_release
        return 0
    elif [[ "$rc" -ne 0 && "${FORM_CLI_RETAIN_WORKDIR:-0}" == 1 ]]; then
        printf 'regen: retained failed platform directory %s\n' "$W" >&2
    else
        rm -rf "$W"
    fi
    W=""
    form_cli_publish_lock_release
    return 0
}
trap cleanup_standard_lane_regen ZERR EXIT
trap 'exit 130' INT
trap 'exit 143' TERM
form_cli_publish_lock_acquire "$BOOT/.form-cli-publish.lock"
W="$(mktemp -d "$BOOT/.native-platform.XXXXXX")"

want_stamp="$(fourth_hash16 "${FORM_CLI_SRCS[@]}")"
want_sha="$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")"
verify_bootstrap_current() {
    [[ "$(fourth_hash16 "${FORM_CLI_SRCS[@]}")" == "$want_stamp" \
        && "$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")" == "$want_sha" \
        && "$(cat "$BOOT/form-cli.stamp")" == "$want_stamp" \
        && "$(cat "$BOOT/form-cli.source.sha256")" == "$want_sha" ]] \
        && form_cli_native_verify_attestation \
            "$BOOT/form-cli.native.attestation" "$want_sha" "$want_stamp" \
            "$BOOT/form-cli-native.c" "$BODY/runtime/fkwu-uni.c"
}
verify_bootstrap_current || {
    printf '%s\n' 'regen: native bootstrap is missing or stale; regenerate it first' >&2
    exit 1
}
FORM_STANDARD_LANE=0 FORM_CLI_FORCE_LINK=1 ./build-form-cli.sh "$W/form-cli"
verify_bootstrap_current || {
    printf '%s\n' 'regen: native bootstrap changed during platform build' >&2
    exit 1
}
form_cli_native_verify_platform_attestation \
    "$W/form-cli.native.attestation" "$want_sha" "$want_stamp" \
    "$BOOT/form-cli.native.attestation" "$slug" \
    "$W/form-cli" "$W/form-cli.fkb" "$W/form-cli.sym"
printf '%s\n' "$want_stamp" > "$W/platform.stamp"
target="$BOOT/form-cli-$slug"
# Publication is per file; builder/platform attestation checks reject mixed files.
# Move the stamp last.
mv -f "$W/form-cli.fkb" "$target.fkb"
mv -f "$W/form-cli.sym" "$target.sym"
mv -f "$W/form-cli.native.attestation" "$target.native.attestation"
mv -f "$W/form-cli" "$target"
form_cli_native_verify_platform_attestation \
    "$target.native.attestation" "$want_sha" "$want_stamp" \
    "$BOOT/form-cli.native.attestation" "$slug" "$target" "$target.fkb" "$target.sym"
verify_bootstrap_current || {
    printf '%s\n' 'regen: native bootstrap changed before platform stamp' >&2
    exit 1
}
mv -f "$W/platform.stamp" "$target.stamp"
printf 'regen: native form-cli-%s with image and symbols; source=%s\n' "$slug" "$want_sha"
