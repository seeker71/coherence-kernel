#!/bin/sh
# Observe the real Hati base -> adapter -> fused -> Q4 -> served lineage.
#
# Shell and jq only cross host/file/API boundaries. Form validates and hashes
# every node and edge. Historical session text and model bytes remain private;
# only byte counts, SHA-256 values, dates, and success bits cross into Form.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

nm_require_command curl
nm_require_command jq
nm_require_command shasum

temp_dir=$(nm_new_temp_dir)
before="$temp_dir/artifacts-before"
after="$temp_dir/artifacts-after"
form_input="$temp_dir/form-input"
band_source="$temp_dir/lineage-band.fk"
cli_source="$temp_dir/lineage-cli.fk"
raw_report="$temp_dir/report-raw"
report="$temp_dir/report"
show_request="$temp_dir/show-request.json"
show_raw="$temp_dir/show.json"
show_canonical="$temp_dir/show-canonical.json"
show_details="$temp_dir/show-details.json"
show_model_info="$temp_dir/show-model-info.json"
show_parameters="$temp_dir/show-parameters.txt"
show_system="$temp_dir/show-system.txt"
show_template="$temp_dir/show-template.txt"
nm_snapshot_generated "$before"

cleanup() {
    nm_remove_new_generated "$before" "$after"
    rm -rf "$temp_dir"
}
trap cleanup EXIT HUP INT TERM

if [ ! -x "$NM_FKWU" ]; then
    printf 'missing executable kernel: %s\n' "$NM_FKWU" >&2
    exit 1
fi

base_snapshot=${NATIVE_MODEL_HATI_BASE_SNAPSHOT:-"$HOME/.cache/huggingface/hub/models--mlx-community--Llama-3.2-3B-Instruct-4bit/snapshots/7f0dc925e0d0afb0322d96f9255cfddf2ba5636e"}
train_root=${NATIVE_MODEL_HATI_TRAIN_ROOT:-"$HOME/.coherence-network/form-train-runs/translation-corpus"}
adapter_root="$train_root/mt-ptbr-adapter"
fused_root="$train_root/hati-translator-fused"
ollama_root=${OLLAMA_MODELS:-"$HOME/.ollama/models"}
q4_manifest=${NATIVE_MODEL_HATI_Q4_MANIFEST:-"$ollama_root/manifests/registry.ollama.ai/library/hati-translator-q4/latest"}
f16_manifest=${NATIVE_MODEL_HATI_F16_MANIFEST:-"$ollama_root/manifests/registry.ollama.ai/library/hati-translator/latest"}
q4_stage=${NATIVE_MODEL_HATI_Q4_STAGE:-"$HOME/.coherence-network/android-models/hati-translator-q4.gguf"}
events=${NATIVE_MODEL_EVENT_LEDGER:-"$NM_STATE_DIR/events.jsonl"}
ollama_url=${OLLAMA_URL:-http://127.0.0.1:11434}

# Historical session coordinates are private state, not public source. Each
# value may instead be supplied explicitly through its named environment
# variable. Partial overrides fall back to the mode-0600 locator.
private_locator=${NATIVE_MODEL_HATI_PRIVATE_LOCATOR:-"$NM_STATE_DIR/hati-lineage-private.json"}
need_private_locator=0
for private_override in \
    "${NATIVE_MODEL_HATI_SESSION:-}" \
    "${NATIVE_MODEL_HATI_TRAIN_TOOL_ID:-}" \
    "${NATIVE_MODEL_HATI_FUSE_TOOL_ID:-}" \
    "${NATIVE_MODEL_HATI_QUANTIZE_TOOL_ID:-}" \
    "${NATIVE_MODEL_HATI_QUANTIZE_MODELFILE_TOOL_ID:-}"
do
    if [ -z "$private_override" ]; then
        need_private_locator=1
    fi
done
if [ "$need_private_locator" -eq 1 ]; then
    case "$private_locator" in
        "$NM_STATE_DIR"/*) ;;
        *)
            printf 'private Hati locator must remain under %s: %s\n' \
                "$NM_STATE_DIR" "$private_locator" >&2
            exit 1 ;;
    esac
    if [ ! -f "$private_locator" ]; then
        printf 'missing private Hati locator: %s\n' "$private_locator" >&2
        exit 1
    fi
    case $(uname -s) in
        Darwin|*BSD) private_mode=$(stat -f '%Lp' "$private_locator") ;;
        *) private_mode=$(stat -c '%a' "$private_locator") ;;
    esac
    if [ "$private_mode" != 600 ]; then
        printf 'private Hati locator must have mode 0600, observed %s: %s\n' \
            "$private_mode" "$private_locator" >&2
        exit 1
    fi
    if ! jq -e '
        type == "object" and
        ([.historical_session_path, .training_tool_id, .fuse_tool_id,
          .quantize_tool_id, .quantize_modelfile_tool_id] |
         all(type == "string" and length > 0))
        ' "$private_locator" >/dev/null
    then
        printf 'private Hati locator is missing required string fields: %s\n' \
            "$private_locator" >&2
        exit 1
    fi
fi
nm_private_value() {
    override=$1
    key=$2
    if [ -n "$override" ]; then
        printf '%s\n' "$override"
    else
        jq -er --arg key "$key" '.[$key]' "$private_locator"
    fi
}
historical_session=$(nm_private_value "${NATIVE_MODEL_HATI_SESSION:-}" \
    historical_session_path)
training_tool_id=$(nm_private_value "${NATIVE_MODEL_HATI_TRAIN_TOOL_ID:-}" \
    training_tool_id)
fuse_tool_id=$(nm_private_value "${NATIVE_MODEL_HATI_FUSE_TOOL_ID:-}" \
    fuse_tool_id)
quantize_tool_id=$(nm_private_value "${NATIVE_MODEL_HATI_QUANTIZE_TOOL_ID:-}" \
    quantize_tool_id)
quantize_modelfile_tool_id=$(nm_private_value \
    "${NATIVE_MODEL_HATI_QUANTIZE_MODELFILE_TOOL_ID:-}" \
    quantize_modelfile_tool_id)
for private_tool_id in "$training_tool_id" "$fuse_tool_id" \
    "$quantize_tool_id" "$quantize_modelfile_tool_id"
do
    case "$private_tool_id" in
        toolu_*) ;;
        *)
            printf 'private Hati tool-use identifier has invalid shape\n' >&2
            exit 1 ;;
    esac
done

required_files="
$base_snapshot/config.json
$base_snapshot/model.safetensors
$base_snapshot/model.safetensors.index.json
$base_snapshot/special_tokens_map.json
$base_snapshot/tokenizer.json
$base_snapshot/tokenizer_config.json
$adapter_root/adapter_config.json
$adapter_root/adapters.safetensors
$fused_root/chat_template.jinja
$fused_root/config.json
$fused_root/model.safetensors.index.json
$fused_root/model-00001-of-00002.safetensors
$fused_root/model-00002-of-00002.safetensors
$fused_root/tokenizer.json
$fused_root/tokenizer_config.json
$train_root/mlx-data/train.jsonl
$train_root/mlx-data/valid.jsonl
$train_root/heldout.jsonl
$train_root/lora-train.log
$train_root/fuse-gguf.log
$train_root/Modelfile.hati
$q4_manifest
$f16_manifest
$q4_stage
$events
$historical_session
$HOME/.coherence-network/offline-train-venv/lib/python3.14/site-packages/mlx_lm/lora.py
$HOME/.coherence-network/offline-train-venv/lib/python3.14/site-packages/mlx_lm/fuse.py
"
printf '%s\n' "$required_files" | while IFS= read -r required
do
    if [ -n "$required" ] && [ ! -f "$required" ]; then
        printf 'missing real lineage artifact: %s\n' "$required" >&2
        exit 1
    fi
done

q4_model_ref=$(jq -er '.layers[] | select(.mediaType == "application/vnd.ollama.image.model") | .digest' "$q4_manifest")
q4_model_sha=${q4_model_ref#sha256:}
q4_model_size=$(jq -er '.layers[] | select(.mediaType == "application/vnd.ollama.image.model") | .size' "$q4_manifest")
q4_blob="$ollama_root/blobs/sha256-$q4_model_sha"
if [ ! -f "$q4_blob" ]; then
    printf 'missing Q4 content-addressed blob: %s\n' "$q4_blob" >&2
    exit 1
fi

manifest_config_ref=$(jq -er '.config.digest' "$q4_manifest")
manifest_config_sha=${manifest_config_ref#sha256:}
manifest_config_size=$(jq -er '.config.size' "$q4_manifest")
params_ref=$(jq -er '.layers[] | select(.mediaType == "application/vnd.ollama.image.params") | .digest' "$q4_manifest")
params_sha=${params_ref#sha256:}
params_size=$(jq -er '.layers[] | select(.mediaType == "application/vnd.ollama.image.params") | .size' "$q4_manifest")
system_ref=$(jq -er '.layers[] | select(.mediaType == "application/vnd.ollama.image.system") | .digest' "$q4_manifest")
system_sha=${system_ref#sha256:}
system_size=$(jq -er '.layers[] | select(.mediaType == "application/vnd.ollama.image.system") | .size' "$q4_manifest")

jq -nc '{model:"hati-translator-q4:latest",verbose:false}' > "$show_request"
curl --silent --show-error --fail --max-time 30 \
    -H 'Content-Type: application/json' --data-binary @"$show_request" \
    "$ollama_url/api/show" > "$show_raw"
jq -cS . "$show_raw" > "$show_canonical"
jq -cjS '.details // {}' "$show_raw" > "$show_details"
jq -cjS '.model_info // {}' "$show_raw" > "$show_model_info"
jq -j '.parameters // ""' "$show_raw" > "$show_parameters"
jq -j '.system // ""' "$show_raw" > "$show_system"
jq -j '.template // ""' "$show_raw" > "$show_template"

# Recover only exact command bytes and success metadata from the private
# historical session. No raw conversation or tool output is persisted.
nm_session_command_sha() {
    tool_id=$1
    jq -je --arg id "$tool_id" \
        '.message.content[]? | select(.type == "tool_use" and .id == $id) | .input.command' \
        "$historical_session" | shasum -a 256 | awk '{print $1}'
}
train_invocation_sha=$(nm_session_command_sha "$training_tool_id")
fuse_invocation_sha=$(nm_session_command_sha "$fuse_tool_id")
quantize_invocation_sha=$(nm_session_command_sha "$quantize_tool_id")
quantize_modelfile_sha=$(
    jq -r --arg id "$quantize_modelfile_tool_id" \
        '.message.content[]? | select(.type == "tool_use" and .id == $id) | .input.command' \
        "$historical_session" | \
    awk '/cat > "\$DEST\/Modelfile.hati" <<EOF/{inside=1;next}
         inside && /^EOF$/{exit}
         inside{print}' | \
    shasum -a 256 | awk '{print $1}'
)
if ! jq -e --arg id "$quantize_tool_id" '
        .message.content[]? |
        select(.type == "tool_result" and .tool_use_id == $id and
               ((.is_error // false) == false) and
               ((.content // "") | contains("success")))
    ' "$historical_session" >/dev/null
then
    printf 'historical Q4 creation success receipt is absent\n' >&2
    exit 1
fi
if ! grep -q 'Saved final weights' "$train_root/lora-train.log"; then
    printf 'historical adapter completion receipt is absent\n' >&2
    exit 1
fi
direct_gguf_export_failed=0
if grep -q 'can only serialize row-major arrays' "$train_root/fuse-gguf.log"; then
    direct_gguf_export_failed=1
fi

event_line=$(jq -c '
    select(.model == "translation.hati-lora-q4" and
           .kind == "evaluation" and .success == 1)
    ' "$events" | tail -n 1)
if [ -z "$event_line" ]; then
    printf 'no successful real Hati inference event is available\n' >&2
    exit 1
fi
event_sha=$(printf '%s\n' "$event_line" | jq -er '.event_sha256')
event_artifact=$(printf '%s\n' "$event_line" | jq -er '.artifact_sha256')
event_day=$(printf '%s\n' "$event_line" | jq -er '.day')
event_success=$(printf '%s\n' "$event_line" | jq -er '.success')
eval_report=$(grep -l "^candidate_event_sha256=$event_sha\$" \
    "$NM_STATE_DIR"/eval-*.txt 2>/dev/null | sort | tail -n 1)
if [ -z "$eval_report" ]; then
    printf 'paired Form evaluation report for event %s is absent\n' "$event_sha" >&2
    exit 1
fi
evaluator_sha=$(awk -F= '$1 == "evaluator_sha256" { print $2; exit }' "$eval_report")
evaluator_tool_sha=$(nm_sha256_file "$NM_SCRIPT_DIR/native_model_eval.sh")
current_day=$(date -u +%Y%m%d)

nm_emit_file_component() {
    component_file=$1
    wc -c < "$component_file" | tr -d ' '
    nm_sha256_file "$component_file"
}
nm_emit_digest_component() {
    printf '%s\n%s\n' "$1" "$2"
}

previous_report=$(find "$NM_STATE_DIR" -maxdepth 1 -type f \
    -name 'lineage-*.txt' -print | sort | tail -n 1)
{
    for key in base_node_sha256 adapter_node_sha256 fused_node_sha256 \
        quantized_node_sha256 served_node_sha256
    do
        if [ -n "$previous_report" ]; then
            awk -F= -v key="$key" '$1 == key { print $2; found=1; exit }
                END { if (!found) print "" }' "$previous_report"
        else
            printf '\n'
        fi
    done

    nm_emit_file_component "$base_snapshot/config.json"
    nm_emit_file_component "$base_snapshot/model.safetensors"
    nm_emit_file_component "$base_snapshot/model.safetensors.index.json"
    nm_emit_file_component "$base_snapshot/special_tokens_map.json"
    nm_emit_file_component "$base_snapshot/tokenizer.json"
    nm_emit_file_component "$base_snapshot/tokenizer_config.json"

    nm_emit_file_component "$adapter_root/adapter_config.json"
    nm_emit_file_component "$adapter_root/adapters.safetensors"

    nm_emit_file_component "$fused_root/chat_template.jinja"
    nm_emit_file_component "$fused_root/config.json"
    nm_emit_file_component "$fused_root/model.safetensors.index.json"
    nm_emit_file_component "$fused_root/model-00001-of-00002.safetensors"
    nm_emit_file_component "$fused_root/model-00002-of-00002.safetensors"
    nm_emit_file_component "$fused_root/tokenizer.json"
    nm_emit_file_component "$fused_root/tokenizer_config.json"

    nm_emit_file_component "$q4_blob"

    nm_emit_file_component "$q4_manifest"
    nm_emit_digest_component "$q4_model_size" "$q4_model_sha"
    nm_emit_digest_component "$manifest_config_size" "$manifest_config_sha"
    nm_emit_digest_component "$params_size" "$params_sha"
    nm_emit_file_component "$show_canonical"
    nm_emit_file_component "$show_details"
    nm_emit_file_component "$show_model_info"
    nm_emit_file_component "$show_parameters"
    nm_emit_file_component "$show_system"
    nm_emit_file_component "$show_template"
    nm_emit_digest_component "$system_size" "$system_sha"

    nm_sha256_file "$historical_session"
    nm_sha256_file "$train_root/mlx-data/train.jsonl"
    nm_sha256_file "$train_root/mlx-data/valid.jsonl"
    nm_sha256_file "$train_root/heldout.jsonl"
    nm_sha256_file "$train_root/lora-train.log"
    nm_sha256_file "$HOME/.coherence-network/offline-train-venv/lib/python3.14/site-packages/mlx_lm/lora.py"
    printf '%s\n' "$train_invocation_sha"
    nm_sha256_file "$train_root/fuse-gguf.log"
    nm_sha256_file "$HOME/.coherence-network/offline-train-venv/lib/python3.14/site-packages/mlx_lm/fuse.py"
    printf '%s\n' "$fuse_invocation_sha" "$quantize_invocation_sha"
    printf '%s\n' "$quantize_modelfile_sha"
    printf '\n' # historical Ollama executable identity was not captured
    nm_sha256_file "$q4_stage"
    jq -er '.layers[] | select(.mediaType == "application/vnd.ollama.image.model") | .digest' \
        "$f16_manifest" | sed 's/^sha256://'
    nm_sha256_file "$train_root/Modelfile.hati"
    printf '%s\n' "$direct_gguf_export_failed" "$current_day"
    printf '%s\n' "$event_sha" "$event_artifact" "$event_day" "$event_success"
    printf '%s\n' "$evaluator_tool_sha" "$evaluator_sha"
} > "$form_input"
chmod 600 "$form_input"

nm_build_lineage_source() {
    destination=$1
    entry=$2
    : > "$destination"
    for source_part in \
        form/form-stdlib/core.fk \
        form/form-stdlib/sha256.fk \
        form/form-stdlib/native-model-evidence.fk \
        form/form-stdlib/native-model-lineage-form.fk \
        "$entry"
    do
        sed '/^; preludes:/d' "$source_part" >> "$destination"
        printf '\n' >> "$destination"
    done
}

cd "$NM_REPO_ROOT"
nm_build_lineage_source "$band_source" \
    form/form-stdlib/tests/native-model-lineage-band.fk
nm_build_lineage_source "$cli_source" \
    form/form-stdlib/native-model-lineage-cli.fk

band=$($NM_FKWU --src "$band_source")
if [ "$band" != 33554431 ]; then
    printf 'lineage band failed: expected 33554431, observed %s\n' "$band" >&2
    exit 1
fi

$NM_FKWU --src "$cli_source" < "$form_input" > "$raw_report"
sed '/^$/d; /^0$/d; /^fkwu: warning:/d' "$raw_report" > "$report"
for required_verdict in \
    lineage_valid=1 q4_copy_equality=1 served_event_bound=1 \
    edges_valid=4 edges_reviewed=0 edges_authorized=0 edges_successful=4 \
    current_f16_to_q4_claimed=0 authority_ready=0
do
    if ! grep -q "^$required_verdict\$" "$report"; then
        printf 'Form lineage verdict missing: %s\n' "$required_verdict" >&2
        cat "$report" >&2
        exit 1
    fi
done

day=$(date -u +%Y%m%d)
epoch=$(date +%s)
durable="$NM_STATE_DIR/lineage-${day}-${epoch}.txt"
{
    cat "$report"
    printf 'lineage_band=%s\n' "$band"
    printf 'base_artifact_path=%s\n' "$base_snapshot"
    printf 'adapter_artifact_path=%s\n' "$adapter_root/adapters.safetensors"
    printf 'fused_artifact_path=%s\n' "$fused_root"
    printf 'quantized_artifact_path=%s\n' "$q4_blob"
    printf 'staged_artifact_path=%s\n' "$q4_stage"
    printf 'served_manifest_path=%s\n' "$q4_manifest"
    printf 'raw_model_bytes_persisted=0\n'
    printf 'raw_session_text_persisted=0\n'
} > "$durable"
chmod 600 "$durable"
report_sha=$(nm_sha256_file "$durable")

cat "$durable"
printf 'lineage_report_sha256=%s\n' "$report_sha"
printf 'lineage_report_path=%s\n' "$durable"
