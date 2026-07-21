#!/bin/sh
# Run the three real, daily-comparable north-star flows.  Each child carrier
# persists its own Form-approved report; this shell writes only a compact
# overview of those already-computed verdicts.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

temp_dir=$(nm_new_temp_dir)
world="$temp_dir/world"
grounding="$temp_dir/grounding"
lineage="$temp_dir/lineage"

cleanup() {
    rm -rf "$temp_dir"
}
trap cleanup EXIT HUP INT TERM

nm_report_value() {
    report=$1
    key=$2
    awk -F= -v key="$key" '$1 == key { print $2; exit }' "$report"
}

nm_require_report_value() {
    report=$1
    key=$2
    expected=$3
    observed=$(nm_report_value "$report" "$key")
    if [ "$observed" != "$expected" ]; then
        printf 'real-flow gate failed: %s expected %s, observed %s\n' \
            "$key" "$expected" "${observed:-missing}" >&2
        cat "$report" >&2
        exit 1
    fi
}

nm_bind_child_report() {
    report=$1
    sha_key=$2
    path_key=$3
    child_sha=$(nm_report_value "$report" "$sha_key")
    child_path=$(nm_report_value "$report" "$path_key")
    if [ -z "$child_sha" ] || [ -z "$child_path" ] || [ ! -f "$child_path" ]; then
        printf 'real-flow child binding missing: %s / %s\n' \
            "$sha_key" "$path_key" >&2
        cat "$report" >&2
        exit 1
    fi
    observed_sha=$(nm_sha256_file "$child_path")
    if [ "$observed_sha" != "$child_sha" ]; then
        printf 'real-flow child binding failed: %s expected %s, observed %s\n' \
            "$child_path" "$child_sha" "$observed_sha" >&2
        exit 1
    fi
}

nm_reuse_child_report() {
    destination=$1
    child_path=$2
    expected_sha=$3
    sha_key=$4
    path_key=$5
    if [ -z "$child_path" ] || [ -z "$expected_sha" ] || [ ! -f "$child_path" ]; then
        printf 'real-flow reused child is incomplete: %s\n' \
            "${child_path:-missing-path}" >&2
        exit 1
    fi
    observed_sha=$(nm_sha256_file "$child_path")
    if [ "$observed_sha" != "$expected_sha" ]; then
        printf 'real-flow reused child failed identity: %s expected %s, observed %s\n' \
            "$child_path" "$expected_sha" "$observed_sha" >&2
        exit 1
    fi
    cat "$child_path" > "$destination"
    printf '%s=%s\n%s=%s\n' \
        "$sha_key" "$expected_sha" "$path_key" "$child_path" >> "$destination"
}

reuse_world=${NATIVE_MODEL_REAL_FLOWS_WORLD_REPORT:-}
reuse_grounding=${NATIVE_MODEL_REAL_FLOWS_GROUNDING_REPORT:-}
reuse_lineage=${NATIVE_MODEL_REAL_FLOWS_LINEAGE_REPORT:-}
if [ -n "$reuse_world$reuse_grounding$reuse_lineage" ]; then
    if [ -z "$reuse_world" ] || [ -z "$reuse_grounding" ] || [ -z "$reuse_lineage" ]; then
        printf 'real-flow reuse requires all three child report paths\n' >&2
        exit 1
    fi
    execution_mode=verified-existing-child-reports
    nm_reuse_child_report "$world" "$reuse_world" \
        "${NATIVE_MODEL_REAL_FLOWS_WORLD_SHA256:-}" \
        world_model_report_sha256 world_model_report_path
    nm_reuse_child_report "$grounding" "$reuse_grounding" \
        "${NATIVE_MODEL_REAL_FLOWS_GROUNDING_SHA256:-}" \
        grounding_report_sha256 grounding_report_path
    nm_reuse_child_report "$lineage" "$reuse_lineage" \
        "${NATIVE_MODEL_REAL_FLOWS_LINEAGE_SHA256:-}" \
        lineage_report_sha256 lineage_report_path
else
    execution_mode=fresh-child-execution
    "$NM_SCRIPT_DIR/native_model_session_world.sh" > "$world"
    nm_require_report_value "$world" world_model_valid 1
    nm_require_report_value "$world" full_pool_evaluated 1
    nm_require_report_value "$world" world_model_band 4095
    nm_bind_child_report "$world" world_model_report_sha256 world_model_report_path
    "$NM_SCRIPT_DIR/native_model_session_grounding.sh" > "$grounding"
    nm_require_report_value "$grounding" replay_valid 1
    nm_require_report_value "$grounding" raw_query_persisted 0
    nm_require_report_value "$grounding" grounding_band 4095
    nm_bind_child_report "$grounding" grounding_report_sha256 grounding_report_path
    "$NM_SCRIPT_DIR/native_model_lineage.sh" > "$lineage"
    nm_require_report_value "$lineage" lineage_valid 1
    nm_require_report_value "$lineage" q4_copy_equality 1
    nm_require_report_value "$lineage" served_event_bound 1
    nm_require_report_value "$lineage" edges_valid 4
    nm_require_report_value "$lineage" edges_successful 4
    nm_require_report_value "$lineage" lineage_band 33554431
    nm_bind_child_report "$lineage" lineage_report_sha256 lineage_report_path
fi

nm_require_report_value "$world" world_model_valid 1
nm_require_report_value "$world" full_pool_evaluated 1
nm_require_report_value "$world" world_model_band 4095
nm_bind_child_report "$world" world_model_report_sha256 world_model_report_path

nm_require_report_value "$grounding" replay_valid 1
nm_require_report_value "$grounding" raw_query_persisted 0
nm_require_report_value "$grounding" grounding_band 4095
nm_bind_child_report "$grounding" grounding_report_sha256 grounding_report_path

nm_require_report_value "$lineage" lineage_valid 1
nm_require_report_value "$lineage" q4_copy_equality 1
nm_require_report_value "$lineage" served_event_bound 1
nm_require_report_value "$lineage" edges_valid 4
nm_require_report_value "$lineage" edges_successful 4
nm_require_report_value "$lineage" lineage_band 33554431
nm_bind_child_report "$lineage" lineage_report_sha256 lineage_report_path

day=$(date -u +%Y%m%d)
epoch=$(date +%s)
durable="$NM_STATE_DIR/real-flows-${day}-${epoch}.txt"
{
    printf 'schema=native-model-real-flows-v1\n'
    printf 'day=%s\n' "$day"
    printf 'execution_mode=%s\n' "$execution_mode"
    printf 'flows_expected=3\n'
    printf 'flows_valid=3\n'
    printf 'session_world_decision=%s\n' \
        "$(nm_report_value "$world" candidate_decision)"
    printf 'session_world_accuracy_ppm=%s\n' \
        "$(nm_report_value "$world" model_accuracy_ppm)"
    printf 'session_world_baseline_ppm=%s\n' \
        "$(nm_report_value "$world" majority_accuracy_ppm)"
    printf 'session_world_gain_ppm=%s\n' \
        "$(nm_report_value "$world" model_accuracy_gain_ppm)"
    printf 'session_world_delta_ppm=%s\n' \
        "$(nm_report_value "$world" model_accuracy_delta_ppm)"
    printf 'session_grounding_queries=%s\n' \
        "$(nm_report_value "$grounding" replayed_queries)"
    printf 'session_grounding_top1_ppm=%s\n' \
        "$(nm_report_value "$grounding" top1_ppm)"
    printf 'session_grounding_top3_ppm=%s\n' \
        "$(nm_report_value "$grounding" top3_ppm)"
    printf 'session_grounding_top5_ppm=%s\n' \
        "$(nm_report_value "$grounding" top5_ppm)"
    printf 'session_grounding_delta_ppm=%s\n' \
        "$(nm_report_value "$grounding" top5_delta_ppm)"
    printf 'lineage_dag_sha256=%s\n' \
        "$(nm_report_value "$lineage" dag_sha256)"
    printf 'lineage_artifact_drift_ppm=%s\n' \
        "$(nm_report_value "$lineage" artifact_drift_ppm)"
    printf 'lineage_edges_valid=%s\n' \
        "$(nm_report_value "$lineage" edges_valid)"
    printf 'lineage_edges_reviewed=%s\n' \
        "$(nm_report_value "$lineage" edges_reviewed)"
    printf 'lineage_edges_authorized=%s\n' \
        "$(nm_report_value "$lineage" edges_authorized)"
    printf 'lineage_edges_successful=%s\n' \
        "$(nm_report_value "$lineage" edges_successful)"
    printf 'lineage_authority_ready=%s\n' \
        "$(nm_report_value "$lineage" authority_ready)"
    printf 'world_report_sha256=%s\n' \
        "$(nm_report_value "$world" world_model_report_sha256)"
    printf 'grounding_report_sha256=%s\n' \
        "$(nm_report_value "$grounding" grounding_report_sha256)"
    printf 'lineage_report_sha256=%s\n' \
        "$(nm_report_value "$lineage" lineage_report_sha256)"
    printf 'world_report_path=%s\n' \
        "$(nm_report_value "$world" world_model_report_path)"
    printf 'grounding_report_path=%s\n' \
        "$(nm_report_value "$grounding" grounding_report_path)"
    printf 'lineage_report_path=%s\n' \
        "$(nm_report_value "$lineage" lineage_report_path)"
} > "$durable"
chmod 600 "$durable"
digest=$(nm_sha256_file "$durable")

cat "$durable"
printf 'real_flows_sha256=%s\n' "$digest"
printf 'real_flows_path=%s\n' "$durable"
