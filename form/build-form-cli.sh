#!/usr/bin/env zsh
# Build or copy the source-runtime CLI and its exact native recipe companions.
# Form owns source closure, checked snapshots, startup emission and compilation.
# This carrier owns host linking, artifact copies and publication ordering.
set -euo pipefail
export LC_ALL=C
FORM="$(cd -P "$(dirname "$0")" && pwd)"
BODY="$(dirname "$FORM")"
cd "$FORM"
source scripts/fourth-arm.sh
source scripts/form_cli_bootstrap_proof.sh
source scripts/form_cli_source_list.sh
(cd "$BODY" && ./fkwu observe/native-node-word-verify.bml)
form_cli_load_sources

OUT="${1:-form-cli}"
[[ "$OUT" == /* ]] || OUT="$FORM/$OUT"
BOOT="${FORM_CLI_NATIVE_BOOTSTRAP_DIR:-$FORM/form-stdlib/bootstrap}"
CC_BIN="${CC:-cc}"
slug="$(fourth_platform_slug)"
want_stamp="$(fourth_hash16 "${FORM_CLI_SRCS[@]}")"
want_sha="$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")"
W=""
publication_stages=()
cleanup() {
    local rc=$?
    if [[ "${#publication_stages[@]}" -gt 0 ]]; then
        rm -f "${publication_stages[@]}"
        publication_stages=()
    fi
    [[ -n "$W" ]] || return 0
    if [[ "$rc" -ne 0 && "${FORM_CLI_RETAIN_WORKDIR:-0}" == 1 ]]; then
        printf 'build: retained failed native CLI work directory %s\n' "$W" >&2
    else
        rm -rf "$W"
    fi
    W=""
    return 0
}
if [[ -n "${ZSH_VERSION:-}" ]]; then
    trap cleanup ZERR EXIT
else
    trap cleanup ERR EXIT
fi
trap 'exit 130' INT
trap 'exit 143' TERM
W="$(mktemp -d)"

regular_copy() {
    [[ -f "$1" && ! -L "$1" ]] || { printf 'build: missing regular artifact %s\n' "$1" >&2; return 1; }
    cp "$1" "$2"
}
source_current() {
    [[ "$(fourth_hash16 "${FORM_CLI_SRCS[@]}")" == "$want_stamp" \
        && "$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")" == "$want_sha" ]]
}
regular_copy "$BOOT/form-cli-native.c" "$W/startup.c"
regular_copy "$BOOT/form-cli.native.attestation" "$W/bootstrap.attestation"
regular_copy "$BODY/runtime/fkwu-uni.c" "$W/runtime-source.c"
regular_copy "$BODY/runtime/fkwu-optable.h" "$W/fkwu-optable.h"
regular_copy "$BODY/runtime/fkwu-node-word.h" "$W/fkwu-node-word.h"
[[ "$(cat "$BOOT/form-cli.source.sha256")" == "$want_sha" \
    && "$(cat "$BOOT/form-cli.stamp")" == "$want_stamp" ]] || {
    printf '%s\n' 'build: native CLI source generation is stale; regenerate bootstrap' >&2; exit 1;
}
form_cli_native_verify_attestation "$W/bootstrap.attestation" "$want_sha" "$want_stamp" "$W/startup.c" "$W/runtime-source.c" || {
    printf '%s\n' 'build: native startup identity refused' >&2; exit 1;
}

candidate="$W/form-cli"
platform="$FORM/form-stdlib/bootstrap/form-cli-$slug"
copy_platform() {
    regular_copy "$platform" "$candidate" \
        && regular_copy "$platform.fkb" "$candidate.fkb" \
        && regular_copy "$platform.sym" "$candidate.sym" \
        && regular_copy "$platform.native.attestation" "$W/platform.attestation" \
        && form_cli_native_verify_platform_attestation "$W/platform.attestation" "$want_sha" "$want_stamp" "$W/bootstrap.attestation" "$slug" "$candidate" "$candidate.fkb" "$candidate.sym"
}

if [[ "${FORM_STANDARD_LANE:-0}" == 1 ]]; then
    copy_platform || { printf '%s\n' 'build: standard native CLI bundle is missing or stale' >&2; exit 1; }
elif [[ "${FORM_CLI_FORCE_LINK:-0}" != 1 && -f "$platform.native.attestation" ]]; then
    copy_platform || { printf '%s\n' 'build: published native CLI bundle refused; explicit regeneration is required' >&2; exit 1; }
else
    [[ -z "${FORM_CLI_EXTRA_SRC:-}${FORM_CLI_EXTRA_LDFLAGS:-}" ]] || {
        printf '%s\n' 'build: native CLI admits host capabilities dynamically; linked extensions are outside this build identity' >&2; exit 1;
    }
    command -v "$CC_BIN" >/dev/null || { printf 'build: compiler required for native startup: %s\n' "$CC_BIN" >&2; exit 1; }
    runner="${FORM_FOURTH_SOURCE_FKWU:-$BODY/fkwu}"
    [[ -f "$runner" && -x "$runner" && ! -L "$runner" ]] || { printf 'build: native source runner required: %s\n' "$runner" >&2; exit 1; }
    regular_copy "$runner" "$W/source-fkwu"
    if [[ -n "${FORM_CLI_NATIVE_SOURCE_SNAPSHOT:-}${FORM_CLI_NATIVE_SOURCE_SEAL:-}${FORM_CLI_NATIVE_SOURCE_SEAL_SHA256:-}" ]]; then
        source_seal="${FORM_CLI_NATIVE_SOURCE_SEAL:?native source seal required}"
        source_snapshot="${FORM_CLI_NATIVE_SOURCE_SNAPSHOT:?native source snapshot required}"
        seal_sha="${FORM_CLI_NATIVE_SOURCE_SEAL_SHA256:?native source seal identity required}"
        source_dir="$(dirname "$source_seal")"
        [[ "$source_snapshot" == "$source_dir/body" ]] || { printf '%s\n' 'build: snapshot path disagrees with source seal owner' >&2; exit 1; }
    else
        { printf '%s\n' FCSC1 "$FORM" "$W/sources" ''; form_cli_source_roots; } > "$W/source.request"
        (cd "$BODY" && "$W/source-fkwu" form/form-stdlib/bml/form-cli-source-closure.bml) < "$W/source.request" > "$W/source.log"
        source_dir="$W/sources"
        source_seal="$source_dir/sources.json"
        source_snapshot="$source_dir/body"
        seal_sha="$(cat "$source_dir/ready")"
    fi
    form_cli_generation_hash_valid "$seal_sha" || { printf '%s\n' 'build: source snapshot completion absent' >&2; exit 1; }
    [[ "$(cat "$source_dir/source.sha256")" == "$want_sha" ]] || { printf '%s\n' 'build: held source snapshot identity changed' >&2; exit 1; }
    printf '%s\n' FCSV1 "$source_seal" "$seal_sha" END > "$W/verify.request"
    (cd "$BODY" && "$W/source-fkwu" form/form-stdlib/bml/form-cli-source-closure.bml) < "$W/verify.request" > "$W/verify-before.log"
    cmp "$W/fkwu-optable.h" "$source_snapshot/runtime/fkwu-optable.h"
    cmp "$W/fkwu-node-word.h" "$source_snapshot/runtime/fkwu-node-word.h"
    args=(-O2 -I "$W" -o "$candidate" "$W/startup.c")
    if [[ "$slug" == windows-* ]]; then
        args+=(-lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp)
    fi
    "$CC_BIN" "${args[@]}"
    recipe="$source_snapshot/form/form-stdlib/form-cli-repl.fk"
    (cd "$source_snapshot" && "$candidate" --compile-source "$recipe") > "$W/compile.out" 2> "$W/compile.err" || {
        cat "$W/compile.err" >&2; printf '%s\n' 'build: native recipe compilation refused' >&2; exit 1;
    }
    cat "$W/compile.err" >&2
    regular_copy "${recipe%.fk}.fkb" "$candidate.fkb"
    regular_copy "${recipe%.fk}.sym" "$candidate.sym"
    (cd "$BODY" && "$W/source-fkwu" form/form-stdlib/bml/form-cli-source-closure.bml) < "$W/verify.request" > "$W/verify-after.log"
    form_cli_native_write_platform_attestation "$W/platform.attestation" "$W/bootstrap.attestation" "$slug" "$candidate" "$candidate.fkb" "$candidate.sym"
fi

mkdir -p "$W/check/.hearth/session-learning"
printf '%s\n' 1 > "$W/check/.hearth/session-learning/paused"
(cd "$W/check" && form_cli_verify_binary_identity "$candidate" "$want_sha")
form_cli_native_verify_platform_attestation "$W/platform.attestation" "$want_sha" "$want_stamp" "$W/bootstrap.attestation" "$slug" "$candidate" "$candidate.fkb" "$candidate.sym"
source_current || { printf '%s\n' 'build: source generation changed before publication' >&2; exit 1; }
mkdir -p "$(dirname "$OUT")"
# Publish the executable last; existing processes retain their loaded recipe.
publication_stages=("$OUT.fkb.writing-$$" "$OUT.sym.writing-$$" "$OUT.native.attestation.writing-$$" "$OUT.writing-$$")
regular_copy "$candidate.fkb" "$OUT.fkb.writing-$$"
regular_copy "$candidate.sym" "$OUT.sym.writing-$$"
regular_copy "$W/platform.attestation" "$OUT.native.attestation.writing-$$"
regular_copy "$candidate" "$OUT.writing-$$"
chmod +x "$OUT.writing-$$"
mv -f "$OUT.fkb.writing-$$" "$OUT.fkb"
mv -f "$OUT.sym.writing-$$" "$OUT.sym"
mv -f "$OUT.native.attestation.writing-$$" "$OUT.native.attestation"
mv -f "$OUT.writing-$$" "$OUT"
printf 'built %s with native Form image and symbols; source=%s\n' "$OUT" "$want_sha"
