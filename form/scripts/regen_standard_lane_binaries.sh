#!/usr/bin/env bash
# regen_standard_lane_binaries.sh — maintainer bridge that refreshes fkwu and
# form-cli platform bootstrap binaries from the canonical kernel checkout.
# Runtime and standard-lane receipts remain fkwu-native and toolchain-free.
set -euo pipefail

FORM="$(cd -P "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$FORM"
export GO_BIN="$FORM/form-kernel-go/bin-go"
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh

command -v clang >/dev/null 2>&1 || {
    printf '%s\n' 'maintainer regen requires clang; runtime/standard lane does not' >&2
    exit 1
}

slug="$(fourth_platform_slug)"
mkdir -p form-stdlib/bootstrap

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

# A maintainer regeneration must relink the carrier even when the Form program
# stamp is unchanged: the binary's self-source genesis also includes the build
# and behavioral-proof scripts, whose bytes are intentionally outside the
# executable Form table hash.
FORM_STANDARD_LANE=0 FORM_CLI_FORCE_LINK=1 ./build-form-cli.sh
[[ -x form-cli ]] || {
    printf '%s\n' 'form-cli build failed' >&2
    exit 1
}
# build-form-cli.sh has already compared the complete canonical source set with
# the freshly regenerated table.  Reuse that witnessed stamp instead of
# maintaining a second source list here: a drifted duplicate can correctly
# relink a carrier and then falsely label the platform copy stale.
form_cli_stamp="$(tr -d '\r\n' < form-stdlib/bootstrap/form-cli.stamp)"
[[ "$form_cli_stamp" =~ ^[0-9a-f]{16}$ ]] || {
    printf '%s\n' 'form-cli canonical bootstrap stamp is missing or malformed' >&2
    exit 1
}
cp form-cli "form-stdlib/bootstrap/form-cli-${slug}"
printf '%s\n' "$form_cli_stamp" > "form-stdlib/bootstrap/form-cli-${slug}.stamp"
printf 'regen: form-cli-%s (%s bytes) stamp=%s\n' \
    "$slug" \
    "$(wc -c < "form-stdlib/bootstrap/form-cli-${slug}" | tr -d ' ')" \
    "$form_cli_stamp"
