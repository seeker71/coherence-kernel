#!/bin/sh
# Train/evaluate an order-2 next-action world model on the current, bounded
# project-session carrier. Raw prompts, reasoning, commands, and tool results
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
root_sessions="$temp_dir/root-sessions"
quiescent_sessions="$temp_dir/quiescent-sessions.tsv"
train_sessions="$temp_dir/train-sessions"
heldout_sessions="$temp_dir/heldout-sessions"
train_actions="$temp_dir/train-actions.tsv"
heldout_actions="$temp_dir/heldout-actions.tsv"
train_episodes="$temp_dir/train-episodes.tsv"
heldout_episodes="$temp_dir/heldout-episodes.tsv"
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
# The live carrier is limited to recent sessions whose declared cwd belongs to
# this project. It is intentionally not a fixed historical date set. Only
# user-root sessions that have been quiet for six hours may enter; quiescence
# is an observable boundary, not an assertion that a session is complete.
# Whole sessions—not individual rows—are split chronologically 80/20.
nm_discover_project_sessions "$sessions"
nm_root_project_sessions "$sessions" "$root_sessions"
recent_session_count=$(wc -l < "$sessions" | tr -d ' ')
recent_root_session_count=$(wc -l < "$root_sessions" | tr -d ' ')
quiescence_seconds=21600
nm_select_quiescent_sessions() {
    nmsw_source_sessions=$1
    nmsw_destination=$2
    nmsw_now=$(date +%s)
    while IFS= read -r nmsw_session_file
    do
        nmsw_modified=$(stat -f %m "$nmsw_session_file")
        if [ $((nmsw_now - nmsw_modified)) -ge "$quiescence_seconds" ]; then
            printf '%s\t%s\n' "$nmsw_modified" "$nmsw_session_file"
        fi
    done < "$nmsw_source_sessions" | sort -n -k1,1 -k2,2 > "$nmsw_destination"
}
nm_select_quiescent_sessions "$root_sessions" "$quiescent_sessions"
eligible_session_count=$(wc -l < "$quiescent_sessions" | tr -d ' ')
if [ "$eligible_session_count" -lt 3 ]; then
    printf 'insufficient quiescent user-root project sessions: eligible=%s quiescence_seconds=%s\n' \
        "$eligible_session_count" "$quiescence_seconds" >&2
    exit 1
fi
train_session_count=$((eligible_session_count * 4 / 5))
heldout_session_count=$((eligible_session_count - train_session_count))
if [ "$train_session_count" -lt 2 ] || [ "$heldout_session_count" -lt 1 ]; then
    printf 'invalid session-disjoint split: eligible=%s train=%s heldout=%s\n' \
        "$eligible_session_count" "$train_session_count" "$heldout_session_count" >&2
    exit 1
fi
awk -F '\t' -v count="$train_session_count" 'NR <= count { print $2 }' \
    "$quiescent_sessions" > "$train_sessions"
awk -F '\t' -v count="$train_session_count" 'NR > count { print $2 }' \
    "$quiescent_sessions" > "$heldout_sessions"
if sort "$train_sessions" "$heldout_sessions" | uniq -d | grep -q .; then
    printf 'session-disjoint split failed: a session entered both train and heldout\n' >&2
    exit 1
fi
train_latest=$(awk -F '\t' -v count="$train_session_count" 'NR == count { print $1 }' \
    "$quiescent_sessions")
heldout_earliest=$(awk -F '\t' -v count="$train_session_count" 'NR == count + 1 { print $1 }' \
    "$quiescent_sessions")
if [ "$train_latest" -gt "$heldout_earliest" ]; then
    printf 'session temporal split failed: train_latest=%s heldout_earliest=%s\n' \
        "$train_latest" "$heldout_earliest" >&2
    exit 1
fi
session_split_valid=1

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
nm_actions_to_episodes "$train_actions" "$train_episodes"
nm_actions_to_episodes "$heldout_actions" "$heldout_episodes"

source_train_count=$(wc -l < "$train_episodes" | tr -d ' ')
source_heldout_count=$(wc -l < "$heldout_episodes" | tr -d ' ')
if [ "$source_train_count" -lt 32 ] || [ "$source_heldout_count" -lt 8 ]; then
    printf 'insufficient closed user-root project episodes: train=%s heldout=%s\n' \
        "$source_train_count" "$source_heldout_count" >&2
    exit 1
fi
train_count=$source_train_count
heldout_count=$source_heldout_count
episode_count=$((train_count + heldout_count))
split_id=session-disjoint-recent-45d-user-root-quiescent-6h-temporal-80-20
cat "$train_episodes" "$heldout_episodes" > "$episodes"
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
# when both the live carrier and this evaluation contract are identical.
nm_build_session_world_source "$cli_source" \
    form/form-stdlib/native-model-session-world-cli.fk
evaluation_contract_sha256=$(nm_sha256_file "$cli_source")

actions_observed=$(( $(sort -u "$train_actions" | wc -l) + \
                      $(sort -u "$heldout_actions" | wc -l) ))

previous_accuracy=-1
previous_report=$(
    find "$NM_STATE_DIR" -maxdepth 1 -type f \
        -name 'session-world-*.txt' -print | sort -r | \
    while IFS= read -r candidate
    do
        if grep -q '^schema=native-model-session-world-report-v5$' \
                "$candidate" && \
           grep -q '^full_pool_evaluated=1$' "$candidate" && \
           grep -q "^episode_carrier_sha256=${episode_sha}$" \
                "$candidate" && \
           grep -q "^evaluation_contract_sha256=${evaluation_contract_sha256}$" \
                "$candidate" && \
           grep -q "^split=${split_id}$" \
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
    printf '%s\n' "$split_id"
    printf '%s\n' "$eligible_session_count" "$train_session_count" \
        "$heldout_session_count" "$quiescence_seconds"
    printf '%s\n' "$session_split_valid"
    awk -F '\t' '{print $1; print $2; print $3; print $4; print $5}' "$episodes"
} > "$form_input"
chmod 600 "$form_input"

nm_build_session_world_source "$band_source" \
    form/form-stdlib/tests/native-model-session-world-band.fk

band=$($NM_FKWU "$band_source")
if [ "$band" != 4095 ]; then
    printf 'session world-model band failed: expected 4095, observed %s\n' \
        "$band" >&2
    exit 1
fi

$NM_FKWU "$cli_source" < "$form_input" > "$raw_report"
sed '/^$/d; /^0$/d; /^fkwu: warning:/d' "$raw_report" > "$report"
if ! grep -q '^world_model_valid=1$' "$report"; then
    printf 'real session world-model report failed validation\n' >&2
    cat "$report" >&2
    exit 1
fi
if ! grep -q '^full_pool_evaluated=1$' "$report" || \
   ! grep -q "^train_episodes=${train_count}$" "$report" || \
   ! grep -q "^heldout_episodes=${heldout_count}$" "$report" || \
   ! grep -q "^split=${split_id}$" "$report"; then
    printf 'real session world-model did not evaluate the complete current pool\n' >&2
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
