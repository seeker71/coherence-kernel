#!/bin/sh
# Native local route: one route name in the environment, the prompt on stdin,
# the ask carried by the native Metal door in one fkwu process. No model
# server, socket, HTTP or JSON membrane sits on this route.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

if [ "$#" -ne 0 ]; then
    printf 'usage: %s < prompt\n' "$0" >&2
    exit 2
fi

route=${LOCAL_MODEL_ROUTE:-form-metal}

# The routing decision -- which route names exist, which door each carries
# the ask through, and each door's token default -- lives in Form, as data:
# form/form-stdlib/native-model-route-table.bml, read through its CLI
# membrane (band form/form-stdlib/tests/native-model-route-table-band.fk).
# This carrier keeps only the host boundary: $LOCAL_MODEL_ROUTE and the
# FORM_* request values from the environment, and stdin.
if [ ! -x "$NM_FKWU" ]; then
    printf 'missing executable kernel: %s\n' "$NM_FKWU" >&2
    exit 1
fi
route_result=$(printf '%s\n' "$route" |
    "$NM_FKWU" form/form-stdlib/native-model-route-table-cli.fk)
route_kind=$(printf '%s\n' "$route_result" | awk -F= '$1 == "route_kind" { print $2; exit }')
route_door=$(printf '%s\n' "$route_result" | awk -F= '$1 == "route_door" { print $2; exit }')
route_steps=$(printf '%s\n' "$route_result" | awk -F= '$1 == "route_steps" { print $2; exit }')
route_known_names=$(printf '%s\n' "$route_result" |
    awk -F= '$1 == "route_known_names" { print $2; exit }')

if [ "$route_kind" != "direct-metal" ]; then
    printf 'unknown LOCAL_MODEL_ROUTE: %s\n' "$route" >&2
    printf 'known routes: %s\n' "$route_known_names" >&2
    exit 2
fi

# This boundary carries environment bytes and stdin. The Form program owns
# validation, model admission, generation, costs and publication.
native_request=$(nm_new_temp_dir)
trap 'rm -rf "$native_request"' EXIT HUP INT TERM
cat > "$native_request/prompt"
printf '%s' "${FORM_METAL_STEPS:-$route_steps}" > "$native_request/cap"
printf '%s' "${FORM_ASK_MODEL:-}" > "$native_request/model"
printf '%s' "${FORM_GGUF_BLOB:-}" > "$native_request/blob"
printf '%s' "${FORM_ASK_STAGE:-}" > "$native_request/stage"
cd "$NM_REPO_ROOT"
printf '%s\n' "$native_request" | "$NM_FKWU" "$route_door"
