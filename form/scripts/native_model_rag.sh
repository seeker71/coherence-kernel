#!/bin/sh
# Prove direct, non-staged RAG through the shipped Form-native CLI artifact.
# The deterministic fixture gates the full ask route; the user's live index is
# an additional world-model observation and may honestly be absent.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

temp_dir=$(nm_new_temp_dir)
fixture_root="$temp_dir/run"
fixture="$fixture_root/.coherence-network/rag-index/index.jsonl"
fixture_report="$temp_dir/fixture-report"
missing_report="$temp_dir/missing-report"
live_report="$temp_dir/live-report"

cleanup() {
    rm -rf "$temp_dir"
}
trap cleanup EXIT HUP INT TERM

form_cli=${NATIVE_MODEL_FORM_CLI:-"$NM_REPO_ROOT/form/form-cli"}
if [ ! -x "$form_cli" ]; then
    printf 'missing native Form CLI: %s\n' "$form_cli" >&2
    exit 1
fi

fixture_source="$fixture_root/docs/grounding.form"
mkdir -p "$(dirname "$fixture")" "$(dirname "$fixture_source")"
printf '%s' 'grounding source fixture v2' > "$fixture_source"
fixture_source_sha256=$(nm_sha256_file "$fixture_source")
fixture_answer_sha256=$(printf '%s' 'A' | shasum -a 256 | awk '{print $1}')
printf '%s\n' \
    "{\"id\":\"@1.1.9.41\",\"node_id\":\"@1.1.9.41\",\"content_node_id\":\"@8.4.4.12\",\"source_path\":\"docs/grounding.form\",\"kind\":\"cell\",\"key\":\"$fixture_source_sha256\",\"persisted_source_sha256\":\"$fixture_source_sha256\",\"schema\":\"nodeid-rag-v2\",\"embedding_kind\":\"form-semantic-v2\",\"snippet\":\"grounding\",\"answer_key\":\"$fixture_answer_sha256\",\"answer_hex\":\"41\",\"vec\":[753942,1030707]}" \
    > "$fixture"

(
    cd "$fixture_root"
    printf 'grounded grounding\nquit\n' | "$form_cli"
) > "$fixture_report"
(
    cd "$fixture_root"
    printf 'grounded native-model-rag-definite-miss-7e4b\nquit\n' | "$form_cli"
) > "$missing_report"

fixture_result=$(sed -n '1p' "$fixture_report")
fixture_lane=$(grep -Fx 'local-lane:fkwu-rag-grounded' "$fixture_report" | head -n 1 || true)
fixture_synthesis=$(grep -Fx 'synthesis-lane:fkwu-rag-grounded' "$fixture_report" | head -n 1 || true)
missing_result=$(sed -n '1p' "$missing_report")
expected_match=0
missing_match=0
nonempty=0
if [ "$fixture_result" = 'grounded:@1.1.9.41' ] &&
   [ "$fixture_lane" = 'local-lane:fkwu-rag-grounded' ] &&
   [ "$fixture_synthesis" = 'synthesis-lane:fkwu-rag-grounded' ]; then
    expected_match=1
fi
if [ "$missing_result" = 'grounded:miss' ]; then
    missing_match=1
fi
if [ -n "$fixture_result" ]; then
    nonempty=1
fi
fixture_band=$(( expected_match + (missing_match * 2) + (nonempty * 4) ))
if [ "$fixture_band" -ne 7 ]; then
    printf 'Form-native RAG artifact fixture failed: expected band 7, observed %s\n' "$fixture_band" >&2
    cat "$fixture_report" >&2
    exit 1
fi

live_index="$HOME/.coherence-network/rag-index/index.jsonl"
live_present=0
live_hit=0
live_result=not-observed
if [ -s "$live_index" ]; then
    live_present=1
    (
        cd "$HOME"
        printf 'ask active-inference.fk\nquit\n' | "$form_cli"
    ) > "$live_report"
    live_result=$(sed -n '1p' "$live_report")
    if [ "$live_result" != 'grounded:miss' ] && [ -n "$live_result" ]; then
        live_hit=1
    fi
fi

printf 'result=%s\n' "$fixture_result"
printf 'missing_result=%s\n' "$missing_result"
printf 'expected_match=%s\n' "$expected_match"
printf 'missing_match=%s\n' "$missing_match"
printf 'nonempty=%s\n' "$nonempty"
printf 'rag_band=%s\n' "$fixture_band"
printf 'live_index_present=%s\n' "$live_present"
printf 'live_result=%s\n' "$live_result"
printf 'live_hit=%s\n' "$live_hit"

exit 0
