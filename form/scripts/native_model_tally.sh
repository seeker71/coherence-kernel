#!/bin/sh
# Render the Form-owned occurrence/final-work tally. No event classification or
# share arithmetic is performed in this carrier.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

nm_require_command jq

ledger="$NM_STATE_DIR/events.jsonl"
if [ ! -f "$ledger" ]; then
    : > "$ledger"
    chmod 600 "$ledger"
fi

temp_dir=$(nm_new_temp_dir)
before="$temp_dir/artifacts-before"
after="$temp_dir/artifacts-after"
result="$temp_dir/result"
form_input="$temp_dir/form-input"
nm_snapshot_generated "$before"
cleanup() {
    nm_remove_new_generated "$before" "$after"
    rm -rf "$temp_dir"
}
trap cleanup EXIT HUP INT TERM

observed_lines=$(awk 'length($0) > 0 { n += 1 } END { print n + 0 }' "$ledger")

# JSON parsing is transport only. Form receives the untouched original line
# plus every scalar, then recomputes the event digest and canonical JSON before
# it may count the row. Unsafe control characters are conservatively invalid.
if jq -e -s '
    def safe_string: type == "string" and (explode | all(. >= 32));
    all(.[];
        type == "object" and
        ([.schema,.class,.model,.lane,.kind,.purpose,.outcome,
          .artifact_sha256,.input_sha256,.output_sha256,
          .training_pair_sha256,.consent_sha256,.license_sha256,
          .lineage_sha256,.authorization_sha256,.seal_sha256,.event_sha256]
          | all(.[]; safe_string)) and
        ([.day,.epoch_ms,.success,.latency_ms,.units,.correction]
          | all(.[]; type == "number")))
    ' "$ledger" >/dev/null 2>&1
then
    transported_events=$(jq -s 'length' "$ledger")
    transport_invalid=0
    {
        printf '%s\n' "$observed_lines" "$transported_events" "$transport_invalid"
        jq -Rr '
            select(length > 0) |
            . as $line | (fromjson) as $o |
            $line,
            ($o.day|tostring), ($o.epoch_ms|tostring),
            $o.class, $o.model, $o.lane, $o.kind, $o.purpose, $o.outcome,
            ($o.success|tostring),
            $o.artifact_sha256, $o.input_sha256, $o.output_sha256,
            ($o.latency_ms|tostring), ($o.units|tostring),
            ($o.correction|tostring),
            $o.training_pair_sha256, $o.consent_sha256, $o.license_sha256,
            $o.lineage_sha256, $o.authorization_sha256, $o.seal_sha256,
            $o.event_sha256
        ' "$ledger"
    } > "$form_input"
else
    printf '%s\n' "$observed_lines" 0 "$observed_lines" > "$form_input"
fi

"$NM_FKWU" --src form/form-stdlib/native-model-tally-cli.fk \
    < "$form_input" > "$result"
sed '/^$/d; /^0$/d; /^fkwu: warning:/d' "$result"
if ! grep -q '^invalid_rows=0$' "$result"; then
    printf 'accepted_final_share=unmeasured-invalid-ledger\n'
elif grep -q '^accepted_final_total=0$' "$result"; then
    printf 'accepted_final_share=unmeasured\n'
else
    printf 'accepted_final_share=measured\n'
fi
