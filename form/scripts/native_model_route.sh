#!/bin/sh
# One registered local route with a Form-owned privacy-safe occurrence.
# Prompt and output live only in mode-600 temporary files and stdout; the
# durable ledger receives hashes and bounded metadata only.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

kind=inference
if [ "${1:-}" = "--probe" ]; then
    kind=integration-probe
elif [ "$#" -ne 0 ]; then
    printf 'usage: %s [--probe] < prompt\n' "$0" >&2
    exit 2
fi

nm_require_command curl
nm_require_command jq
nm_require_command shasum

if [ ! -x "$NM_FKWU" ]; then
    printf 'missing executable kernel: %s\n' "$NM_FKWU" >&2
    exit 1
fi

ollama_url=${OLLAMA_URL:-http://127.0.0.1:11434}
ollama_model=llama3.2:3b
registered_model=base.llama32-3b-local
ledger="$NM_STATE_DIR/events.jsonl"

temp_dir=$(nm_new_temp_dir)
before="$temp_dir/artifacts-before"
after="$temp_dir/artifacts-after"
prompt_file="$temp_dir/prompt"
request_file="$temp_dir/request.json"
response_file="$temp_dir/response.json"
output_file="$temp_dir/output"
show_file="$temp_dir/show.json"
canonical_show="$temp_dir/show-canonical.json"
show_after_file="$temp_dir/show-after.json"
canonical_show_after="$temp_dir/show-after-canonical.json"
event_result="$temp_dir/event-result"
nm_snapshot_generated "$before"

cleanup() {
    nm_remove_new_generated "$before" "$after"
    rm -rf "$temp_dir"
}
trap cleanup EXIT HUP INT TERM

cat > "$prompt_file"
if [ ! -s "$prompt_file" ]; then
    printf 'refusing an empty prompt\n' >&2
    exit 2
fi

input_sha=$(nm_sha256_file "$prompt_file")
jq -n --arg name "$ollama_model" '{name:$name}' |
    curl --silent --show-error --fail --max-time 30 \
        -H 'Content-Type: application/json' --data-binary @- \
        "$ollama_url/api/show" > "$show_file"
jq -cS . "$show_file" > "$canonical_show"
artifact_sha=$(nm_sha256_file "$canonical_show")

jq -Rs --arg model "$ollama_model" \
    '{model:$model,prompt:.,stream:false,options:{temperature:0}}' \
    "$prompt_file" > "$request_file"

started=$(date +%s)
observed_success=1
if curl --silent --show-error --fail --max-time 120 \
        -H 'Content-Type: application/json' --data-binary @"$request_file" \
        "$ollama_url/api/generate" > "$response_file" &&
        jq -er '.response | strings' "$response_file" > "$output_file"
then
    :
else
    observed_success=0
    : > "$output_file"
fi
ended=$(date +%s)
latency_ms=$(( (ended - started) * 1000 ))
if [ "$observed_success" -eq 1 ]; then
    latency_ms=$(jq -r '((.total_duration // 0) / 1000000 | round)' "$response_file")
fi
output_sha=$(nm_sha256_file "$output_file")
units=$(wc -c < "$output_file" | tr -d ' ')
day=$(date -u +%Y%m%d)
epoch_ms=$(( $(date +%s) * 1000 ))

identity_stable=0
identity_after=0
artifact_after_sha=$(printf '' | shasum -a 256 | awk '{print $1}')
if jq -n --arg name "$ollama_model" '{name:$name}' |
    curl --silent --show-error --fail --max-time 30 \
        -H 'Content-Type: application/json' --data-binary @- \
        "$ollama_url/api/show" > "$show_after_file" &&
        jq -cS . "$show_after_file" > "$canonical_show_after"
then
    identity_after=1
    artifact_after_sha=$(nm_sha256_file "$canonical_show_after")
    if [ "$artifact_sha" = "$artifact_after_sha" ]; then
        identity_stable=1
    fi
fi

{
    printf '%s\n' "$ledger"
    printf '%s\n' "$day"
    printf '%s\n' "$epoch_ms"
    printf '%s\n' "$registered_model"
    printf '%s\n' "$kind"
    printf '%s\n' "$observed_success"
    printf '%s\n' "$artifact_sha"
    printf '%s\n' "$artifact_after_sha"
    printf '%s\n' 1
    printf '%s\n' "$identity_after"
    printf '%s\n' "$input_sha"
    printf '%s\n' "$output_sha"
    printf '%s\n' "$latency_ms"
    printf '%s\n' "$units"
    printf '\n'
    printf '\n'
} | "$NM_FKWU" --src form/form-stdlib/native-model-event-cli.fk > "$event_result"

if ! grep -q '^append_ok=1$' "$event_result"; then
    printf 'Form rejected the occurrence; model output withheld\n' >&2
    cat "$event_result" >&2
    exit 1
fi

event_sha=$(awk -F= '$1 == "event_sha256" { print $2; exit }' "$event_result")
event_success=$(awk -F= '$1 == "event_success" { print $2; exit }' "$event_result")
identity_stable=$(awk -F= '$1 == "identity_stable" { print $2; exit }' "$event_result")
printf 'route_kind=%s model_id=%s latency_ms=%s identity_stable=%s event_sha256=%s\n' \
    "$kind" "$registered_model" "$latency_ms" "$identity_stable" \
    "$event_sha" >&2

if [ "$event_success" -ne 1 ]; then
    exit 1
fi
cat "$output_file"
