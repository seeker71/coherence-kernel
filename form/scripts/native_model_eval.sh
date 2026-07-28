#!/bin/sh
# Run one fixed paired local diagnostic. Ollama performs inference and the
# shell observes process identity; Form alone normalizes, scores, compares,
# hashes the dataset/evaluator, and refuses promotion authority.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

nm_require_command curl
nm_require_command jq
nm_require_command shasum

if [ ! -x "$NM_FKWU" ]; then
    printf 'missing executable kernel: %s\n' "$NM_FKWU" >&2
    exit 1
fi

ollama_url=${OLLAMA_URL:-http://127.0.0.1:11434}
candidate_id=translation.hati-lora-q4
candidate_tag=hati-translator-q4:latest
incumbent_id=base.llama32-3b-local
incumbent_tag=llama3.2:3b
task_lane=translation
ledger="$NM_STATE_DIR/events.jsonl"
eval_corpus=${NATIVE_MODEL_EVAL_CORPUS:-"$HOME/.coherence-network/form-train-runs/translation-corpus/heldout.jsonl"}
eval_items=${NATIVE_MODEL_EVAL_ITEMS:-5}

temp_dir=$(nm_new_temp_dir)
before="$temp_dir/artifacts-before"
after="$temp_dir/artifacts-after"
prompt_file="$temp_dir/prompt"
expected_file="$temp_dir/expected"
form_result="$temp_dir/form-result"
summary="$temp_dir/summary"
case_stream="$temp_dir/heldout-cases"
nm_snapshot_generated "$before"

cleanup() {
    nm_remove_new_generated "$before" "$after"
    rm -rf "$temp_dir"
}
trap cleanup EXIT HUP INT TERM

case "$eval_items" in ''|*[!0-9]*|0) printf 'invalid NATIVE_MODEL_EVAL_ITEMS\n' >&2; exit 64;; esac
if [ ! -f "$eval_corpus" ]; then
    printf 'missing heldout evaluation corpus: %s\n' "$eval_corpus" >&2
    exit 1
fi
day=$(date -u +%Y%m%d)
corpus_rows=$(jq -s 'length' "$eval_corpus")
if [ "$corpus_rows" -lt "$eval_items" ]; then
    printf 'insufficient heldout rows: %s\n' "$corpus_rows" >&2
    exit 1
fi
offset=$((day % corpus_rows))
jq -c -s --argjson offset "$offset" --argjson count "$eval_items" \
    '. as $rows | ($rows[$offset:] + $rows[:$offset])[:$count] | .[] | {task,answer}' \
    "$eval_corpus" > "$case_stream"

observe_show() {
    observe_tag=$1
    observe_prefix=$2
    observe_payload="$temp_dir/$observe_prefix-show-request.json"
    observe_raw="$temp_dir/$observe_prefix-show.json"
    observe_canonical="$temp_dir/$observe_prefix-show-canonical.json"
    jq -nc --arg model "$observe_tag" '{model:$model,verbose:false}' \
        > "$observe_payload"
    if curl --silent --show-error --fail --max-time 30 \
        -H 'Content-Type: application/json' --data-binary @"$observe_payload" \
        "$ollama_url/api/show" > "$observe_raw" &&
        jq -cS . "$observe_raw" > "$observe_canonical"
    then
        printf '1\n' > "$temp_dir/$observe_prefix-identity"
    else
        : > "$observe_canonical"
        printf '0\n' > "$temp_dir/$observe_prefix-identity"
    fi
    nm_sha256_file "$observe_canonical" > "$temp_dir/$observe_prefix-artifact"
}

call_model() {
    call_tag=$1
    call_prefix=$2
    call_request="$temp_dir/$call_prefix-request.json"
    call_response="$temp_dir/$call_prefix-response.json"
    call_output="$temp_dir/$call_prefix-output"

    observe_show "$call_tag" "$call_prefix-before"
    jq -Rs --arg model "$call_tag" \
        '{model:$model,prompt:.,stream:false,options:{temperature:0,num_predict:96}}' \
        "$prompt_file" > "$call_request"
    if curl --silent --show-error --fail --max-time 180 \
        -H 'Content-Type: application/json' --data-binary @"$call_request" \
        "$ollama_url/api/generate" > "$call_response" &&
        jq -er '.response | strings | gsub("[\\r\\n\\t]+"; " ")' \
            "$call_response" > "$call_output"
    then
        jq -r '((.total_duration // 0) / 1000000 | round)' \
            "$call_response" > "$temp_dir/$call_prefix-latency"
        printf '0\n' > "$temp_dir/$call_prefix-error"
    else
        : > "$call_output"
        printf '0\n' > "$temp_dir/$call_prefix-latency"
        printf '1\n' > "$temp_dir/$call_prefix-error"
    fi
    observe_show "$call_tag" "$call_prefix-after"
    nm_sha256_file "$call_output" > "$temp_dir/$call_prefix-output-sha"
}

: > "$form_result"
while IFS= read -r case_json
do
    source_text=$(printf '%s' "$case_json" | jq -r '.task')
    answer_text=$(printf '%s' "$case_json" | jq -r '.answer')
    printf 'Translate from English to Brazilian Portuguese. Return only the translation: %s\n' \
        "$source_text" > "$prompt_file"
    printf '%s\n' "$answer_text" > "$expected_file"
    call_model "$candidate_tag" candidate
    call_model "$incumbent_tag" incumbent
    {
        printf '%s\n' "$day" "$task_lane" "$candidate_id"
        cat "$temp_dir/candidate-before-artifact" "$temp_dir/candidate-after-artifact"
        cat "$temp_dir/candidate-before-identity" "$temp_dir/candidate-after-identity"
        cat "$temp_dir/candidate-latency" "$temp_dir/candidate-error"
        sed -n '1p' "$temp_dir/candidate-output"
        printf '%s\n' "$incumbent_id"
        cat "$temp_dir/incumbent-before-artifact" "$temp_dir/incumbent-after-artifact"
        cat "$temp_dir/incumbent-before-identity" "$temp_dir/incumbent-after-identity"
        cat "$temp_dir/incumbent-latency" "$temp_dir/incumbent-error"
        sed -n '1p' "$temp_dir/incumbent-output" "$prompt_file" "$expected_file"
    } | "$NM_FKWU" --src form/form-stdlib/native-model-eval-cli.fk >> "$form_result"
done < "$case_stream"

sed '/^$/d; /^0$/d; /^fkwu: warning:/d' "$form_result" > "$summary"
if ! grep -q '^paired=1$' "$summary"; then
    printf 'Form could not pair the local diagnostic\n' >&2
    cat "$summary" >&2
    exit 1
fi
observations=$(awk -F= '$1 == "paired" && $2 == 1 { n += 1 } END { print n + 0 }' "$summary")
if [ "$observations" -ne "$eval_items" ]; then
    printf 'Form could not pair every heldout observation: expected=%s observed=%s\n' \
        "$eval_items" "$observations" >&2
    exit 1
fi
candidate_total=$(awk -F= '$1 == "candidate_score_ppm" { s += $2 } END { print s + 0 }' "$summary")
incumbent_total=$(awk -F= '$1 == "incumbent_score_ppm" { s += $2 } END { print s + 0 }' "$summary")
candidate_mean=$((candidate_total / observations))
incumbent_mean=$((incumbent_total / observations))
if [ "$candidate_mean" -gt "$incumbent_mean" ]; then quality_decision=observed-better
elif [ "$candidate_mean" -lt "$incumbent_mean" ]; then quality_decision=observed-not-better
else quality_decision=observed-tie
fi

input_sha=$(nm_sha256_file "$prompt_file")
epoch_ms=$(( $(date +%s) * 1000 ))

append_eval_event() {
    event_prefix=$1
    event_model=$2
    event_result="$temp_dir/$event_prefix-event-result"
    event_error=$(sed -n '1p' "$temp_dir/$event_prefix-error")
    event_observed_success=$((1 - event_error))
    event_artifact_before=$(sed -n '1p' "$temp_dir/$event_prefix-before-artifact")
    event_artifact_after=$(sed -n '1p' "$temp_dir/$event_prefix-after-artifact")
    event_identity_before=$(sed -n '1p' "$temp_dir/$event_prefix-before-identity")
    event_identity_after=$(sed -n '1p' "$temp_dir/$event_prefix-after-identity")
    event_output=$(sed -n '1p' "$temp_dir/$event_prefix-output-sha")
    event_latency=$(sed -n '1p' "$temp_dir/$event_prefix-latency")
    event_units=$(wc -c < "$temp_dir/$event_prefix-output" | tr -d ' ')
    {
        printf '%s\n' "$ledger" "$day" "$epoch_ms" "$event_model" evaluation
        printf '%s\n' "$event_observed_success" "$event_artifact_before"
        printf '%s\n' "$event_artifact_after" "$event_identity_before" "$event_identity_after"
        printf '%s\n' "$input_sha" "$event_output" "$event_latency" "$event_units"
        printf '\n\n'
    } | "$NM_FKWU" --src form/form-stdlib/native-model-event-cli.fk \
        > "$event_result"
    if ! grep -q '^append_ok=1$' "$event_result"; then
        printf 'Form rejected %s evaluation occurrence\n' "$event_model" >&2
        cat "$event_result" >&2
        exit 1
    fi
    awk -F= '$1 == "event_sha256" { print $2; exit }' "$event_result"
}

candidate_event=$(append_eval_event candidate "$candidate_id")
incumbent_event=$(append_eval_event incumbent "$incumbent_id")
{
    cat "$summary"
    printf 'diagnostic_scope=bounded-heldout-paired-quality-check\n'
    printf 'diagnostic_observations=%s\n' "$observations"
    printf 'candidate_mean_score_ppm=%s\n' "$candidate_mean"
    printf 'incumbent_mean_score_ppm=%s\n' "$incumbent_mean"
    printf 'candidate_quality_decision=%s\n' "$quality_decision"
    printf 'candidate_event_sha256=%s\n' "$candidate_event"
    printf 'incumbent_event_sha256=%s\n' "$incumbent_event"
    printf 'raw_text_persisted=0\n'
} > "$temp_dir/durable-summary"

epoch=$(date +%s)
durable="$NM_STATE_DIR/eval-${day}-${epoch}.txt"
cp "$temp_dir/durable-summary" "$durable"
chmod 600 "$durable"
digest=$(nm_sha256_file "$durable")

cat "$durable"
printf 'eval_sha256=%s\n' "$digest"
printf 'eval_path=%s\n' "$durable"
