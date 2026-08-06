#!/bin/sh
# Execute the bounded, actual in-memory training whose weights and metrics are
# computed entirely in Form.  This is not a persisted native LLM checkpoint.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

temp_dir=$(nm_new_temp_dir)
before="$temp_dir/artifacts-before"
after="$temp_dir/artifacts-after"
report="$temp_dir/training-report"
raw_report="$temp_dir/training-report-raw"
band_source="$temp_dir/training-band.fk"
report_source="$temp_dir/training-report.fk"
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

# Stage one combined Form source for each entry. Compiling dependency files as
# isolated artifacts reports their cross-file calls as recovered unresolved
# calls even though the final prelude-linked program executes correctly. The
# combined source makes the same dependency closure explicit and error-clean.
nm_build_training_source() {
    bundle_destination=$1
    bundle_entry=$2
    : > "$bundle_destination"
    for bundle_part in \
        form/form-stdlib/core.fk \
        form/form-stdlib/transformer-numerics.fk \
        form/form-stdlib/transformer-block.fk \
        form/form-stdlib/transformer-backprop.fk \
        form/form-stdlib/transformer-corpus-train.fk \
        form/form-stdlib/neural-lm.fk \
        form/form-stdlib/native-model-live-training.fk \
        "$bundle_entry"
    do
        sed '/^; preludes:/d' "$bundle_part" >> "$bundle_destination"
        printf '\n' >> "$bundle_destination"
    done
}

nm_build_training_source "$band_source" \
    form/form-stdlib/tests/native-model-live-training-band.fk
nm_build_training_source "$report_source" \
    form/form-stdlib/native-model-live-training-main.fk

band=$($NM_FKWU --src "$band_source")
if [ "$band" != "255" ]; then
    printf 'Form training band failed: expected 255, observed %s\n' "$band" >&2
    exit 1
fi

$NM_FKWU --src "$report_source" > "$raw_report"
# The direct-source runner prints the top-level value returned by print_str.
# Keep only the Form report itself, not that carrier-level trailing zero.
sed '/^$/d; /^0$/d; /^fkwu: warning:/d' "$raw_report" > "$report"
valid_count=$(awk -F= '$1 == "training_valid" && $2 == "1" { n += 1 } END { print n + 0 }' "$report")
if [ "$valid_count" -ne 2 ]; then
    printf 'Form training report did not carry both valid verdicts\n' >&2
    cat "$report" >&2
    exit 1
fi

day=$(date -u +%Y%m%d)
epoch=$(date +%s)
durable="$NM_STATE_DIR/training-${day}-${epoch}.txt"
cp "$report" "$durable"
chmod 600 "$durable"
digest=$(nm_sha256_file "$durable")

cat "$report"
printf 'training_band=%s\n' "$band"
printf 'report_sha256=%s\n' "$digest"
printf 'report_path=%s\n' "$durable"
"$NM_SCRIPT_DIR/native_model_checkpoint.sh"
