#!/bin/sh
# Train/evaluate an order-2 next-action world model on the complete frozen real
# Codex session benchmark. Raw prompts, reasoning, commands, and tool results
# never enter the durable episode set. The shell transports structural rows;
# Form owns classification, fixed-shape count learning, heldout scoring,
# semantic hashes, baseline comparison, and day-over-day delta.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

nm_require_command jq
nm_require_command shasum

temp_dir=$(nm_new_temp_dir)
before="$temp_dir/artifacts-before"
after="$temp_dir/artifacts-after"
sessions="$temp_dir/sessions"
recent_sessions="$temp_dir/recent-sessions"
recent_root_sessions="$temp_dir/recent-root-sessions"
train_sessions="$temp_dir/train-sessions"
heldout_candidates="$temp_dir/heldout-candidates"
heldout_sessions="$temp_dir/heldout-sessions"
train_actions="$temp_dir/train-actions.tsv"
heldout_actions="$temp_dir/heldout-actions.tsv"
train_episodes_all="$temp_dir/train-episodes-all.tsv"
heldout_episodes_all="$temp_dir/heldout-episodes-all.tsv"
episodes="$temp_dir/episodes.tsv"
form_input="$temp_dir/form-input"
band_source="$temp_dir/session-world-band.fk"
cli_source="$temp_dir/session-world-cli.fk"
raw_report="$temp_dir/report-raw"
report="$temp_dir/report"
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

cd "$NM_REPO_ROOT"
nm_discover_all_sessions() {
    destination=$1
    : > "$destination"
    for session_root in \
        "$NM_CODEX_HOME/sessions" "$NM_CODEX_HOME/archived_sessions"
    do
        if [ -d "$session_root" ]; then
            find "$session_root" -type f -name 'rollout-*.jsonl' -print
        fi
    done | sort -u > "$destination"
}

# The benchmark dates are frozen, so their discovery must not inherit the
# shared recent-session helper's -mtime window. Recent counts remain useful
# observation metadata but cannot make historical evidence disappear.
nm_discover_all_sessions "$sessions"
nm_discover_recent_sessions "$recent_sessions"
nm_root_project_sessions "$recent_sessions" "$recent_root_sessions"
recent_session_count=$(wc -l < "$recent_sessions" | tr -d ' ')
recent_root_session_count=$(wc -l < "$recent_root_sessions" | tr -d ' ')
if [ ! -s "$sessions" ]; then
    printf 'no real Codex sessions found in active or archived roots\n' >&2
    exit 1
fi

# Freeze a lineage-safe historical benchmark: July 2-4 roots/descendants train;
# July 8/10/11/14 user-root sessions are future heldout. July 5-7 is an embargo,
# and the still-changing July 15 sessions remain shadow-only.
awk -F/ '
    $NF ~ /^rollout-2026-07-02/ ||
    $NF ~ /^rollout-2026-07-03/ ||
    $NF ~ /^rollout-2026-07-04/ { print }
' "$sessions" > "$train_sessions"
awk -F/ '
    $NF ~ /^rollout-2026-07-08/ ||
    $NF ~ /^rollout-2026-07-10/ ||
    $NF ~ /^rollout-2026-07-11/ ||
    $NF ~ /^rollout-2026-07-14/ { print }
' "$sessions" > "$heldout_candidates"
nm_root_project_sessions "$heldout_candidates" "$heldout_sessions"

salt=$(nm_session_hash_salt)
nm_extract_session_actions() {
    source_sessions=$1
    destination=$2
    : > "$destination"
    chmod 600 "$destination"
    while IFS= read -r session_file
    do
        session_id=$(sed -n '1p' "$session_file" | \
            jq -r '.payload.id // .payload.session_id // ""')
        session_digest=$(printf '%s:%s' "$salt" "$session_id" | \
            shasum -a 256 | awk '{print substr($1,1,16)}')
        jq -r --arg sid "$session_digest" '
            def known_nested:
                ["exec_command","apply_patch","write_stdin","web__run",
                 "update_plan","create_goal","get_goal","update_goal",
                 "codex_app__automation_update",
                 "codex_app__read_thread_terminal",
                 "codex_app__load_workspace_dependencies",
                 "mcp__node_repl__js","view_image"];
            select(.type == "response_item" and
                   (.payload.type == "function_call" or
                    .payload.type == "custom_tool_call")) |
            (.timestamp // "") as $ts |
            (.payload.call_id // .payload.id // "") as $call |
            if (.payload.type == "custom_tool_call" and .payload.name == "exec")
            then
                ([(((.payload.input // .payload.arguments // "")) |
                    scan("tools\\.([A-Za-z0-9_]+)") | .[0]) |
                    select(. as $n | known_nested | index($n))] |
                 if length == 0 then ["exec"] else . end)[] as $name |
                [$ts,$sid,$call,$name] | @tsv
            else
                [$ts,$sid,$call,(.payload.name // "other")] | @tsv
            end
        ' "$session_file" >> "$destination"
    done < "$source_sessions"
}

nm_actions_to_episodes() {
    source_actions=$1
    destination=$2
    sort -u "$source_actions" | sort -k2,2 -k1,1 -k3,3 -k4,4 | \
        awk -F '\t' '
            BEGIN { OFS="\t" }
            $2 != session {
                session=$2; n=0; previous2=""; previous1=""
            }
            {
                n += 1
                if (n >= 3) print $1,$2,previous2,previous1,$4
                previous2=previous1
                previous1=$4
            }
        ' | sort -k1,1 > "$destination"
}

nm_extract_session_actions "$train_sessions" "$train_actions"
nm_extract_session_actions "$heldout_sessions" "$heldout_actions"
nm_actions_to_episodes "$train_actions" "$train_episodes_all"
nm_actions_to_episodes "$heldout_actions" "$heldout_episodes_all"

source_train_count=$(wc -l < "$train_episodes_all" | tr -d ' ')
source_heldout_count=$(wc -l < "$heldout_episodes_all" | tr -d ' ')
expected_train_count=31827
expected_heldout_count=1364
if [ "$source_train_count" -ne "$expected_train_count" ] || \
   [ "$source_heldout_count" -ne "$expected_heldout_count" ]; then
    printf 'frozen session-world pool drift: expected train=%s heldout=%s; observed train=%s heldout=%s\n' \
        "$expected_train_count" "$expected_heldout_count" \
        "$source_train_count" "$source_heldout_count" >&2
    exit 1
fi
cat "$train_episodes_all" "$heldout_episodes_all" > "$episodes"
episode_sha=$(nm_sha256_file "$episodes")

nm_build_session_world_source() {
    destination=$1
    entry=$2
    : > "$destination"
    for source_part in \
        form/form-stdlib/core.fk \
        form/form-stdlib/sha256.fk \
        form/form-stdlib/native-model-evidence.fk \
        form/form-stdlib/native-model-session-world.fk \
        "$entry"
    do
        sed '/^; preludes:/d' "$source_part" >> "$destination"
        printf '\n' >> "$destination"
    done
}

# Hash the exact generated source that fkwu will execute, including the core,
# evidence, model, and CLI entry closure. Daily deltas are comparable only
# when both the frozen carrier and this evaluation contract are identical.
nm_build_session_world_source "$cli_source" \
    form/form-stdlib/native-model-session-world-cli.fk
evaluation_contract_sha256=$(nm_sha256_file "$cli_source")

train_count=$source_train_count
heldout_count=$source_heldout_count
episode_count=$(wc -l < "$episodes" | tr -d ' ')
if [ "$train_count" -lt 32 ] || [ "$heldout_count" -lt 8 ]; then
    printf 'insufficient lineage-safe episodes: train=%s heldout=%s\n' \
        "$train_count" "$heldout_count" >&2
    exit 1
fi
actions_observed=$(( $(sort -u "$train_actions" | wc -l) + \
                      $(sort -u "$heldout_actions" | wc -l) ))

previous_accuracy=-1
previous_report=$(
    find "$NM_STATE_DIR" -maxdepth 1 -type f \
        -name 'session-world-*.txt' -print | sort -r | \
    while IFS= read -r candidate
    do
        if grep -q '^schema=native-model-session-world-report-v2$' \
                "$candidate" && \
           grep -q '^full_pool_evaluated=1$' "$candidate" && \
           grep -q '^train_episodes=31827$' "$candidate" && \
           grep -q '^heldout_episodes=1364$' "$candidate" && \
           grep -q "^episode_carrier_sha256=${episode_sha}$" \
                "$candidate" && \
           grep -q "^evaluation_contract_sha256=${evaluation_contract_sha256}$" \
                "$candidate" && \
           grep -q '^split=frozen-lineage-safe-20260702-04-train-20260708-14-heldout$' \
                "$candidate"
        then
            printf '%s\n' "$candidate"
            break
        fi
    done
)
if [ -n "$previous_report" ]; then
    previous_accuracy=$(awk -F= \
        '$1 == "model_accuracy_ppm" { print $2; exit }' "$previous_report")
    previous_accuracy=${previous_accuracy:--1}
fi

{
    printf '%s\n' "$previous_accuracy" "$recent_session_count" \
        "$recent_root_session_count"
    printf '%s\n' "$actions_observed" "$source_train_count" \
        "$source_heldout_count" 1 "$train_count" "$heldout_count"
    awk -F '\t' '{print $1; print $2; print $3; print $4; print $5}' "$episodes"
} > "$form_input"
chmod 600 "$form_input"

nm_build_session_world_source "$band_source" \
    form/form-stdlib/tests/native-model-session-world-band.fk

band=$($NM_FKWU --src "$band_source")
if [ "$band" != 4095 ]; then
    printf 'session world-model band failed: expected 4095, observed %s\n' \
        "$band" >&2
    exit 1
fi

$NM_FKWU --src "$cli_source" < "$form_input" > "$raw_report"
sed '/^$/d; /^0$/d; /^fkwu: warning:/d' "$raw_report" > "$report"
if ! grep -q '^world_model_valid=1$' "$report"; then
    printf 'real session world-model report failed validation\n' >&2
    cat "$report" >&2
    exit 1
fi
if ! grep -q '^full_pool_evaluated=1$' "$report" || \
   ! grep -q "^train_episodes=${expected_train_count}$" "$report" || \
   ! grep -q "^heldout_episodes=${expected_heldout_count}$" "$report"; then
    printf 'real session world-model did not evaluate the complete frozen pool\n' >&2
    cat "$report" >&2
    exit 1
fi

episode_path="$NM_STATE_DIR/session-world-episodes-${episode_sha}.tsv"
if [ ! -f "$episode_path" ]; then
    cp "$episodes" "$episode_path"
    chmod 600 "$episode_path"
fi

day=$(date -u +%Y%m%d)
epoch=$(date +%s)
durable="$NM_STATE_DIR/session-world-${day}-${epoch}.txt"
{
    cat "$report"
    printf 'world_model_band=%s\n' "$band"
    printf 'episode_carrier_sha256=%s\n' "$episode_sha"
    printf 'evaluation_contract_sha256=%s\n' \
        "$evaluation_contract_sha256"
    printf 'episode_path=%s\n' "$episode_path"
} > "$durable"
chmod 600 "$durable"
report_sha=$(nm_sha256_file "$durable")

cat "$durable"
printf 'world_model_report_sha256=%s\n' "$report_sha"
printf 'world_model_report_path=%s\n' "$durable"
