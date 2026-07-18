#!/bin/sh
# Shared host-only membrane for the native-model Form loop.

set -eu

NM_SCRIPT_DIR=$(CDPATH= cd "$(dirname "$0")" && pwd)
NM_REPO_ROOT=$(CDPATH= cd "$NM_SCRIPT_DIR/../.." && pwd)
NM_FKWU=${NATIVE_MODEL_FKWU:-"$NM_REPO_ROOT/fkwu"}
NM_STATE_DIR=${NATIVE_MODEL_STATE_DIR:-"$HOME/.coherence-network/native-model-form-shell"}
NM_CODEX_HOME=${CODEX_HOME:-"$HOME/.codex"}

umask 077
mkdir -p "$NM_STATE_DIR"
chmod 700 "$NM_STATE_DIR"

nm_new_temp_dir() {
    mktemp -d "${TMPDIR:-/tmp}/native-model-form.XXXXXX"
}

nm_snapshot_generated() {
    destination=$1
    (
        cd "$NM_REPO_ROOT"
        find bootstrap form/form-stdlib -type f \( -name '*.fkb' -o -name '*.sym' \) -print | sort
    ) > "$destination"
}

# Direct-source fkwu emits checkout witness artifacts.  Delete only artifacts
# absent from the pre-run snapshot; never touch a pre-existing user artifact.
nm_remove_new_generated() {
    before=$1
    after=$2
    nm_snapshot_generated "$after"
    comm -13 "$before" "$after" | while IFS= read -r generated
    do
        if [ -n "$generated" ]; then
            rm -f "$NM_REPO_ROOT/$generated"
        fi
    done
}

nm_sha256_file() {
    shasum -a 256 "$1" | awk '{print $1}'
}

nm_require_command() {
    if ! command -v "$1" >/dev/null 2>&1; then
        printf 'missing required host carrier: %s\n' "$1" >&2
        return 1
    fi
}

# Discover recent real Codex sessions. Only path and session metadata cross
# this host boundary; prompts, reasoning, commands, and results are not written
# by the discovery pass.
nm_discover_recent_sessions() {
    destination=$1
    : > "$destination"
    for session_root in "$NM_CODEX_HOME/sessions" "$NM_CODEX_HOME/archived_sessions"
    do
        if [ ! -d "$session_root" ]; then
            continue
        fi
        find "$session_root" -type f -name 'rollout-*.jsonl' -mtime -45 -print
    done | sort -u > "$destination"
}

nm_discover_project_sessions() {
    destination=$1
    recent=$(nm_new_temp_dir)
    all_sessions="$recent/sessions"
    nm_discover_recent_sessions "$all_sessions"
    : > "$destination"
    while IFS= read -r session_file
    do
        session_cwd=$(sed -n '1p' "$session_file" | jq -r \
            '.payload.cwd // ""' 2>/dev/null | \
            tr '[:upper:]' '[:lower:]' || printf '')
        case "$session_cwd" in
            *coherence-kernel*|*coherence-network*)
                printf '%s\n' "$session_file" ;;
        esac
    done > "$destination"
    rm -rf "$recent"
}

nm_root_project_sessions() {
    source_list=$1
    destination=$2
    : > "$destination"
    while IFS= read -r session_file
    do
        if sed -n '1p' "$session_file" | \
            jq -e '.payload.thread_source == "user"' >/dev/null 2>&1
        then
            printf '%s\n' "$session_file" >> "$destination"
        fi
    done < "$source_list"
}

nm_session_hash_salt() {
    salt_path="$NM_STATE_DIR/session-id-salt"
    if [ ! -f "$salt_path" ]; then
        nm_require_command openssl
        openssl rand -hex 32 > "$salt_path"
        chmod 600 "$salt_path"
    fi
    sed -n '1p' "$salt_path"
}
