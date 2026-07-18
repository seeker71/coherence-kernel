#!/bin/sh
# Daily Form-native witness. Shell observes compiler/process/time boundaries;
# fkwu executes the proofs, training, gates, and occurrence semantics in Form.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

nm_require_command cc
nm_require_command shasum

temp_dir=$(nm_new_temp_dir)
before="$temp_dir/artifacts-before"
after="$temp_dir/artifacts-after"
native_way="$temp_dir/native-vs-rented.fk"
training="$temp_dir/training"
rag="$temp_dir/rag"
diagnostic_output="$temp_dir/diagnostic-output"
real_flows="$temp_dir/real-flows"
tally="$temp_dir/tally"
summary="$temp_dir/daily-summary"
nm_snapshot_generated "$before"

cleanup() {
    nm_remove_new_generated "$before" "$after"
    rm -rf "$temp_dir"
}
trap cleanup EXIT HUP INT TERM

cd "$NM_REPO_ROOT"
cc -O2 -o "$NM_FKWU" runtime/fkwu-uni.c

ground=$($NM_FKWU --src bootstrap/ground.fk)
recursive=$($NM_FKWU --src bootstrap/ground-recursive.fk 10)
freshness=$($NM_FKWU --src form/form-stdlib/tests/binary-freshness-band.fk)
{
    cat observe/native-vs-rented.fk
    printf '%s\n' '(native-vs-rented-check)'
} > "$native_way"
native=$($NM_FKWU --src "$native_way")
live_loop=$($NM_FKWU --src form/form-stdlib/tests/native-model-live-loop-band.fk)
sha256_band=$($NM_FKWU --src form/form-stdlib/tests/sha256-band.fk)
replacement_band=$($NM_FKWU --src form/form-stdlib/tests/native-model-form-replacement-band.fk)

"$NM_SCRIPT_DIR/native_model_rag.sh" > "$rag"
rag_band=$(awk -F= '$1 == "rag_band" { print $2; exit }' "$rag")

"$NM_SCRIPT_DIR/native_model_train.sh" > "$training"
training_band=$(awk -F= '$1 == "training_band" { print $2; exit }' "$training")
checkpoint_band=$(awk -F= '$1 == "checkpoint_band" { print $2; exit }' "$training")

if [ "$ground" != 42 ] || [ "$recursive" != 55 ] ||
   [ "$freshness" != 15 ] || [ "$native" != 11111 ] ||
   [ "$live_loop" != 4095 ] || [ "$training_band" != 255 ] ||
   [ "$checkpoint_band" != 4095 ] || [ "$sha256_band" != 2 ] ||
   [ "$replacement_band" != 262143 ] || [ "$rag_band" != 7 ]; then
    printf 'integrity failure: ground=%s recursive=%s freshness=%s native=%s live_loop=%s training=%s checkpoint=%s sha256=%s replacement=%s rag=%s\n' \
        "$ground" "$recursive" "$freshness" "$native" "$live_loop" \
        "$training_band" "$checkpoint_band" "$sha256_band" "$replacement_band" "$rag_band" >&2
    exit 1
fi

diagnostic=not-requested
if [ "${NATIVE_MODEL_OLLAMA_DIAGNOSTIC:-1}" = 1 ]; then
    if "$NM_SCRIPT_DIR/native_model_eval.sh" > "$diagnostic_output"
    then
        diagnostic=paired-form-scored-and-logged
    else
        diagnostic=failed-and-logged-if-observable
    fi
fi
"$NM_SCRIPT_DIR/native_model_real_flows.sh" > "$real_flows"
"$NM_SCRIPT_DIR/native_model_tally.sh" > "$tally"

day=$(date -u +%Y%m%d)
epoch=$(date +%s)
{
    printf 'schema=native-model-form-daily-v1\n'
    printf 'day=%s\n' "$day"
    printf 'ground=%s\n' "$ground"
    printf 'recursive=%s\n' "$recursive"
    printf 'binary_freshness=%s\n' "$freshness"
    printf 'native_vs_rented=%s\n' "$native"
    printf 'live_loop_band=%s\n' "$live_loop"
    printf 'training_band=%s\n' "$training_band"
    printf 'checkpoint_band=%s\n' "$checkpoint_band"
    printf 'sha256_band=%s\n' "$sha256_band"
    printf 'replacement_band=%s\n' "$replacement_band"
    cat "$rag"
    printf 'ollama_diagnostic=%s\n' "$diagnostic"
    cat "$training"
    if [ -s "$diagnostic_output" ]; then
        cat "$diagnostic_output"
    fi
    cat "$real_flows"
    cat "$tally"
} > "$summary"

durable="$NM_STATE_DIR/daily-${day}-${epoch}.txt"
cp "$summary" "$durable"
chmod 600 "$durable"
digest=$(nm_sha256_file "$durable")

cat "$summary"
printf 'daily_sha256=%s\n' "$digest"
printf 'daily_path=%s\n' "$durable"
