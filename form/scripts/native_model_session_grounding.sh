#!/bin/sh
# Replay real root-session queries against the live local body.  jq selects
# event fields and Base64 carries transient bytes.  The shell hashes private
# carriers; Form validates identities and owns embedding, ranking, scoring,
# coverage, and daily delta.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

nm_require_command jq
nm_require_command shasum
nm_require_command base64
nm_require_command grep

index=${NATIVE_MODEL_RAG_INDEX:-$HOME/.coherence-network/rag-index/index.jsonl}
if [ ! -f "$index" ]; then
    printf 'missing local RAG index: %s\n' "$index" >&2
    exit 1
fi
if [ ! -x "$NM_FKWU" ]; then
    printf 'missing executable kernel: %s\n' "$NM_FKWU" >&2
    exit 1
fi

temp_dir=$(nm_new_temp_dir)
before="$temp_dir/artifacts-before"
after="$temp_dir/artifacts-after"
candidates="$temp_dir/candidates.tsv"
candidate_ids="$temp_dir/candidate-ids"
label_ids="$temp_dir/label-ids"
label_candidates="$temp_dir/label-candidates.tsv"
distractor_candidates="$temp_dir/distractor-candidates.tsv"
admitted_candidates="$temp_dir/admitted-candidates.tsv"
admitted_ids="$temp_dir/admitted-ids"
missing_labels="$temp_dir/missing-labels"
sessions_all="$temp_dir/sessions-all.tsv"
sessions="$temp_dir/sessions"
pairs_all="$temp_dir/pairs-all.tsv"
pairs_raw="$temp_dir/pairs-raw.tsv"
pairs="$temp_dir/pairs.tsv"
form_input="$temp_dir/form-input"
contract_source="$temp_dir/session-grounding-contract.fk"
raw_report="$temp_dir/report-raw"
report="$temp_dir/report"
timeout_marker="$temp_dir/cli-timeout"
cli_pid=
watchdog_pid=
nm_snapshot_generated "$before"

stop_children() {
    if [ -n "$watchdog_pid" ]; then
        kill "$watchdog_pid" 2>/dev/null || true
        wait "$watchdog_pid" 2>/dev/null || true
        watchdog_pid=
    fi
    if [ -n "$cli_pid" ]; then
        kill -TERM "$cli_pid" 2>/dev/null || true
        wait "$cli_pid" 2>/dev/null || true
        cli_pid=
    fi
}

cleanup() {
    stop_children
    if [ -d "$temp_dir" ]; then
        if [ -f "$before" ]; then
            nm_remove_new_generated "$before" "$after" || true
        fi
        rm -rf "$temp_dir"
    fi
}
handle_signal() {
    signal_status=$1
    stop_children
    exit "$signal_status"
}
trap cleanup EXIT
trap 'handle_signal 129' HUP
trap 'handle_signal 130' INT
trap 'handle_signal 143' TERM
umask 077

cli_budget=${NATIVE_MODEL_SESSION_GROUNDING_BUDGET_SECONDS:-3600}
case "$cli_budget" in
    ''|*[!0-9]*) printf 'invalid production CLI budget: %s\n' "$cli_budget" >&2; exit 1 ;;
esac
if [ "$cli_budget" -lt 1 ]; then
    printf 'production CLI budget must be at least 1 second: %s\n' "$cli_budget" >&2
    exit 1
fi
detailed_rows=${NATIVE_MODEL_SESSION_GROUNDING_DETAILED_ROWS:-0}
case "$detailed_rows" in
    0|1) ;;
    *) printf 'detailed rows flag must be 0 or 1: %s\n' "$detailed_rows" >&2; exit 1 ;;
esac

cd "$NM_REPO_ROOT"
: > "$candidates"
jq -r '[.source_path // "", (((.source_path // "") + " " + ((.snippet // "")[0:384])) | @base64)] | @tsv' "$index" |
while IFS="$(printf '\t')" read -r id text64
do
    case "$id" in ''|/*|*../*|../*) continue ;; esac
    if [ -f "$NM_REPO_ROOT/$id" ]; then
        printf '%s\t%s\n' "$id" "$text64"
    fi
done | sort -t "$(printf '\t')" -k1,1 -u > "$candidates"
cut -f1 "$candidates" > "$candidate_ids"
source_index_count=$(wc -l < "$candidates" | tr -d ' ')
if [ "$source_index_count" -lt 2 ]; then
    printf 'insufficient live candidate paths: %s\n' "$source_index_count" >&2
    exit 1
fi

: > "$sessions_all"
for root in "$NM_CODEX_HOME/sessions" "$NM_CODEX_HOME/archived_sessions"
do
    [ -d "$root" ] || continue
    find "$root" -type f -name 'rollout-*.jsonl' -mtime -120 -print
done | sort -u | while IFS= read -r session_file
do
    meta=$(sed -n '1p' "$session_file")
    session_cwd=$(printf '%s\n' "$meta" | jq -r '.payload.cwd // ""' 2>/dev/null || printf '')
    parent=$(printf '%s\n' "$meta" | jq -r '.payload.parent_thread_id // ""' 2>/dev/null || printf '')
    started=$(printf '%s\n' "$meta" | jq -r '.payload.timestamp // .timestamp // ""' 2>/dev/null || printf '')
    case "$session_cwd" in */coherence-kernel) ;; *) continue ;; esac
    [ -z "$parent" ] || continue
    [ -n "$started" ] || continue
    printf '%s\t%s\n' "$started" "$session_file"
done > "$sessions_all"
source_session_files=$(wc -l < "$sessions_all" | tr -d ' ')
file_limit=${NATIVE_MODEL_SESSION_GROUNDING_FILES:-16}
sort -k1,1 "$sessions_all" | tail -n "$file_limit" | cut -f2- > "$sessions"
root_session_files=$(wc -l < "$sessions" | tr -d ' ')

: > "$pairs_all"
completed_pairs_scanned=0
while IFS= read -r session_file
do
    while IFS="$(printf '\t')" read -r query_when query64 completion64
    do
        completed_pairs_scanned=$((completed_pairs_scanned + 1))
        labels=$(printf '%s' "$completion64" | base64 -D |
            grep -F -o -f "$candidate_ids" | sort -u || true)
        [ -n "$labels" ] || continue
        label_csv=$(printf '%s\n' "$labels" | paste -sd, -)
        printf '%s\t%s\t%s\n' "$query_when" "$query64" "$label_csv" >> "$pairs_all"
    done <<EOF
$(jq -nr '
  foreach inputs as $e (
    {query:null,query_when:"",emit:null};
    .emit=null |
    if ($e.type=="event_msg" and $e.payload.type=="user_message" and ($e.payload.message|type)=="string")
    then .query=$e.payload.message | .query_when=($e.timestamp // "")
    elif ($e.type=="event_msg" and $e.payload.type=="task_complete" and .query!=null and (($e.payload.last_agent_message // "")|length)>0)
    then .emit=[.query_when,(.query|@base64),(($e.payload.last_agent_message // "")|@base64)] | .query=null
    else . end;
    .emit // empty | @tsv
  )' "$session_file")
EOF
done < "$sessions"
labeled_pairs_observed=$(wc -l < "$pairs_all" | tr -d ' ')
# The native walker replays this privacy-minimized diagnostic without a
# promotion path.  Keep the default corpus deliberately small enough for a
# daily witness: one recent labeled query against four admitted paths.
# Operators may raise either bound explicitly for a separately reviewed run.
query_limit=${NATIVE_MODEL_SESSION_GROUNDING_QUERIES:-1}
sort -k1,1 -u "$pairs_all" | tail -n "$query_limit" > "$pairs_raw"
episode_count=$(wc -l < "$pairs_raw" | tr -d ' ')
if [ "$episode_count" -lt 1 ]; then
    printf 'no completed real queries cited a live indexed path\n' >&2
    exit 1
fi

salt_file="$NM_STATE_DIR/session-grounding-salt"
if [ ! -s "$salt_file" ]; then
    salt_tmp="$temp_dir/salt"
    dd if=/dev/urandom bs=32 count=1 2>/dev/null | shasum -a 256 | awk '{print $1}' > "$salt_tmp"
    chmod 600 "$salt_tmp"
    mv "$salt_tmp" "$salt_file"
fi
chmod 600 "$salt_file"
salt=$(sed -n '1p' "$salt_file")
: > "$pairs"
while IFS="$(printf '\t')" read -r query_when query64 label_csv
do
    query_digest=$( { printf '%s\n' "$salt"; printf '%s' "$query64"; } |
        shasum -a 256 | awk '{print $1}')
    printf '%s\t%s\t%s\t%s\n' "$query_when" "$query64" "$query_digest" "$label_csv" >> "$pairs"
done < "$pairs_raw"

# Daily replay must be small enough to run on the native walker.  Admit every
# observed label, then fill a deterministic corpus from the sorted live index.
# A tie therefore cannot gain a preferred position merely by being a label.
awk -F '\t' '{n=split($4,a,","); for (i=1; i<=n; i++) if (a[i] != "") print a[i]}' \
    "$pairs" | sort -u > "$label_ids"
distinct_label_count=$(wc -l < "$label_ids" | tr -d ' ')
admitted_limit=${NATIVE_MODEL_SESSION_GROUNDING_CANDIDATES:-4}
case "$admitted_limit" in
    ''|*[!0-9]*) printf 'invalid admitted candidate limit: %s\n' "$admitted_limit" >&2; exit 1 ;;
esac
if [ "$admitted_limit" -lt 2 ]; then
    printf 'admitted candidate limit must be at least 2: %s\n' "$admitted_limit" >&2
    exit 1
fi
awk -F '\t' 'NR==FNR {wanted[$1]=1; next} ($1 in wanted)' \
    "$label_ids" "$candidates" > "$label_candidates"
if [ "$(wc -l < "$label_candidates" | tr -d ' ')" -ne "$distinct_label_count" ]; then
    printf 'one or more selected labels are absent from the live source index\n' >&2
    exit 1
fi
if [ "$distinct_label_count" -lt "$admitted_limit" ]; then
    distractor_limit=$((admitted_limit - distinct_label_count))
else
    distractor_limit=0
fi
awk -F '\t' 'NR==FNR {wanted[$1]=1; next} !($1 in wanted)' \
    "$label_ids" "$candidates" | sed -n "1,${distractor_limit}p" > "$distractor_candidates"
sort -u "$label_candidates" "$distractor_candidates" > "$admitted_candidates"
cut -f1 "$admitted_candidates" | sort -u > "$admitted_ids"
comm -23 "$label_ids" "$admitted_ids" > "$missing_labels"
if [ -s "$missing_labels" ]; then
    printf 'admission lost selected labels:\n' >&2
    sed 's/^/  /' "$missing_labels" >&2
    exit 1
fi
admitted_count=$(wc -l < "$admitted_candidates" | tr -d ' ')

append_source() {
    destination=$1
    part=$2
    sed '/^; preludes:/d' "$part" >> "$destination"
    printf '\n' >> "$destination"
}

build_source() {
    destination=$1
    entry=$2
    include_wire=$3
    : > "$destination"
    append_source "$destination" form/form-stdlib/core.fk
    if [ "$include_wire" -eq 1 ]; then
        append_source "$destination" form/form-stdlib/str-byte-at.fk
        append_source "$destination" form/form-stdlib/base64.fk
    fi
    append_source "$destination" form/form-stdlib/record-src-shim.fk
    for part in \
        form/form-stdlib/rag-embed.fk \
        form/form-stdlib/rag-retrieve.fk \
        form/form-stdlib/native-model-session-grounding.fk \
        "$entry"
    do
        append_source "$destination" "$part"
    done
}

# The source runner resolves this canonical entry's one-line prelude closure
# through fresh .fkb dependencies.  Executing the old concatenated 66 KiB
# bundle recompiled every dependency as one giant source unit and exhausted the
# replay budget before a single Form evaluation could begin.  Retain that exact
# closure only as a contract identity; never make it the runtime unit.
build_source "$contract_source" form/form-stdlib/native-model-session-grounding-cli.fk 1
evaluation_contract_sha256=$(nm_sha256_file "$contract_source")
pool_digest=$( { printf '%s\n' "$salt"; cat "$admitted_candidates"; } |
    shasum -a 256 | awk '{print $1}')
dataset_digest=$( { printf '%s\n' "$salt"; cat "$pairs"; } |
    shasum -a 256 | awk '{print $1}')

previous_hit5=-1
previous_comparable=0
previous_report=$(find "$NM_STATE_DIR" -maxdepth 1 -type f \
    -name 'session-grounding-[0-9]*-[0-9]*.txt' -print | sort | tail -n 1)
if [ -n "$previous_report" ]; then
    previous_candidate=$(awk -F= '$1 == "candidate_set_sha256" {print $2; exit}' "$previous_report")
    previous_dataset=$(awk -F= '$1 == "dataset_sha256" {print $2; exit}' "$previous_report")
    previous_contract=$(awk -F= '$1 == "evaluation_contract_sha256" {print $2; exit}' "$previous_report")
    if [ "$previous_candidate" = "$pool_digest" ] && \
       [ "$previous_dataset" = "$dataset_digest" ] && \
       [ "$previous_contract" = "$evaluation_contract_sha256" ]; then
        comparable_hit5=$(awk -F= '$1 == "top5_ppm" {print $2; exit}' "$previous_report")
        case "$comparable_hit5" in
            ''|*[!0-9]*) ;;
            *) previous_hit5=$comparable_hit5; previous_comparable=1 ;;
        esac
    fi
fi

{
    printf '%s\n' "$pool_digest" "$dataset_digest" "$evaluation_contract_sha256"
    printf '%s\n' "$previous_hit5" "$previous_comparable"
    printf '%s\n' "$detailed_rows"
    printf '%s\n' "$source_session_files" "$root_session_files"
    printf '%s\n' "$completed_pairs_scanned" "$labeled_pairs_observed" "$source_index_count" "$admitted_count"
    awk -F '\t' '{print $1; print $2}' "$admitted_candidates"
    printf '%s\n' "$episode_count"
    while IFS="$(printf '\t')" read -r query_when query64 query_digest label_csv
    do
        printf '%s\n' "$query_when" "$query64" "$query_digest"
        label_count=$(printf '%s\n' "$label_csv" | tr ',' '\n' | wc -l | tr -d ' ')
        printf '%s\n' "$label_count"
        printf '%s\n' "$label_csv" | tr ',' '\n'
    done < "$pairs"
} > "$form_input"
chmod 600 "$form_input"

# Parse the wire shape without decoding or printing any private carrier.  This
# catches line-framing drift before the native walker sees the invocation.
framing_preflight=$(awk \
    -v admitted="$admitted_count" \
    -v expected="$episode_count" '
    BEGIN { first_episode = 13 + (2 * admitted); state = 0; seen = 0; min_labels = -1; ok = 1 }
    NR == 12 {
        if ($0 != admitted) { print "framing preflight: admitted count mismatch" > "/dev/stderr"; ok = 0 }
        next
    }
    NR < first_episode { next }
    NR == first_episode {
        if ($0 != expected) { print "framing preflight: episode count mismatch" > "/dev/stderr"; ok = 0 }
        state = 1
        next
    }
    state == 1 { state = 2; next }
    state == 2 { state = 3; next }
    state == 3 { state = 4; next }
    state == 4 {
        if ($0 !~ /^[0-9]+$/ || $0 < 1) {
            print "framing preflight: episode has no path label" > "/dev/stderr"
            ok = 0
            remaining = 0
            state = 1
        } else {
            remaining = $0 + 0
            if (min_labels < 0 || remaining < min_labels) min_labels = remaining
            state = 5
        }
        next
    }
    state == 5 {
        remaining--
        if (remaining == 0) { seen++; state = 1 }
        next
    }
    END {
        if (seen != expected || state != 1) {
            print "framing preflight: incomplete episode payload" > "/dev/stderr"
            ok = 0
        }
        if (!ok) exit 1
        printf "%d\t%d\n", seen, min_labels
    }' "$form_input") || {
        printf 'session grounding framing preflight failed\n' >&2
        exit 1
    }
framing_preflight_episodes=${framing_preflight%%"$(printf '\t')"*}
framing_preflight_min_labels=${framing_preflight#*"$(printf '\t')"}
if [ "${NATIVE_MODEL_SESSION_GROUNDING_PREFLIGHT_ONLY:-0}" -eq 1 ]; then
    printf 'source_index_candidates=%s\n' "$source_index_count"
    printf 'admitted_candidates=%s\n' "$admitted_count"
    printf 'framing_preflight_episodes=%s\n' "$framing_preflight_episodes"
    printf 'framing_preflight_min_labels=%s\n' "$framing_preflight_min_labels"
    printf 'evaluation_contract_sha256=%s\n' "$evaluation_contract_sha256"
    printf 'production_cli_budget_seconds=%s\n' "$cli_budget"
    printf 'detail_rows=%s\n' "$detailed_rows"
    exit 0
fi

band=$($NM_FKWU --src form/form-stdlib/tests/native-model-session-grounding-band.fk)
if [ "$band" != 4095 ]; then
    printf 'session grounding band failed: expected 4095, observed %s\n' "$band" >&2
    exit 1
fi

replay_started=$(date +%s)
$NM_FKWU --src form/form-stdlib/native-model-session-grounding-cli.fk \
    < "$form_input" > "$raw_report" &
cli_pid=$!
(
    sleep "$cli_budget"
    if kill -0 "$cli_pid" 2>/dev/null; then
        : > "$timeout_marker"
        kill -TERM "$cli_pid" 2>/dev/null || true
        sleep 5
        kill -KILL "$cli_pid" 2>/dev/null || true
    fi
) &
watchdog_pid=$!
if wait "$cli_pid"; then
    cli_status=0
else
    cli_status=$?
fi
cli_pid=
kill "$watchdog_pid" 2>/dev/null || true
wait "$watchdog_pid" 2>/dev/null || true
watchdog_pid=
replay_elapsed_seconds=$(( $(date +%s) - replay_started ))

day=$(date -u +%Y%m%d)
epoch=$(date +%s)
if [ -f "$timeout_marker" ]; then
    timeout_report="$NM_STATE_DIR/session-grounding-timeout-${day}-${epoch}.txt"
    {
        printf 'schema=native-model-session-grounding-timeout-v1\n'
        printf 'source=real-root-codex-user-message-to-task-complete\n'
        printf 'source_index_candidates=%s\n' "$source_index_count"
        printf 'admitted_candidates=%s\n' "$admitted_count"
        printf 'replayed_queries=%s\n' "$episode_count"
        printf 'candidate_set_sha256=%s\n' "$pool_digest"
        printf 'dataset_sha256=%s\n' "$dataset_digest"
        printf 'evaluation_contract_sha256=%s\n' "$evaluation_contract_sha256"
        printf 'framing_preflight_episodes=%s\n' "$framing_preflight_episodes"
        printf 'framing_preflight_min_labels=%s\n' "$framing_preflight_min_labels"
        printf 'production_cli_budget_seconds=%s\n' "$cli_budget"
        printf 'replay_elapsed_seconds=%s\n' "$replay_elapsed_seconds"
        printf 'detail_rows=%s\n' "$detailed_rows"
        printf 'operational_timeout=1\nquality_metric_observed=0\nraw_query_persisted=0\n'
        printf 'cli_exit_code=%s\n' "$cli_status"
    } > "$timeout_report"
    chmod 600 "$timeout_report"
    timeout_digest=$(nm_sha256_file "$timeout_report")
    cat "$timeout_report"
    printf 'grounding_timeout_report_sha256=%s\n' "$timeout_digest"
    printf 'grounding_timeout_report_path=%s\n' "$timeout_report"
    exit 124
fi
if [ "$cli_status" -ne 0 ]; then
    printf 'session grounding production CLI failed: exit %s after %s seconds\n' \
        "$cli_status" "$replay_elapsed_seconds" >&2
    exit "$cli_status"
fi

sed '/^$/d; /^0$/d; /^fkwu: warning:/d' "$raw_report" > "$report"
if ! grep -q '^replay_valid=1$' "$report" || ! grep -q '^raw_query_persisted=0$' "$report"; then
    printf 'Form rejected real session grounding replay\n' >&2
    cat "$report" >&2
    exit 1
fi

durable="$NM_STATE_DIR/session-grounding-${day}-${epoch}.txt"
{
    cat "$report"
    printf 'grounding_band=%s\n' "$band"
    printf 'framing_preflight_episodes=%s\n' "$framing_preflight_episodes"
    printf 'framing_preflight_min_labels=%s\n' "$framing_preflight_min_labels"
    printf 'production_cli_budget_seconds=%s\n' "$cli_budget"
    printf 'replay_elapsed_seconds=%s\n' "$replay_elapsed_seconds"
    printf 'operational_timeout=0\nquality_metric_observed=1\n'
} > "$durable"
chmod 600 "$durable"
digest=$(nm_sha256_file "$durable")
cat "$durable"
printf 'grounding_report_sha256=%s\n' "$digest"
printf 'grounding_report_path=%s\n' "$durable"
