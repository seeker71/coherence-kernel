#!/bin/sh
# Persist/reload exact learned f64 images through Form. The shell chooses
# private same-directory paths and invokes fkwu; Form owns bytes, admission,
# atomic rename, equivalence, and keep/revert.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

temp_dir=$(nm_new_temp_dir)
before="$temp_dir/artifacts-before"
after="$temp_dir/artifacts-after"
band_source="$temp_dir/checkpoint-band.fk"
cli_source="$temp_dir/checkpoint-cli.fk"
raw_report="$temp_dir/checkpoint-report-raw"
report="$temp_dir/checkpoint-report"
nm_snapshot_generated "$before"

candidate_temporary=""
cleanup() {
    nm_remove_new_generated "$before" "$after"
    if [ -n "$candidate_temporary" ]; then
        rm -f "$candidate_temporary"
    fi
    rm -rf "$temp_dir"
}
trap cleanup EXIT HUP INT TERM

if [ ! -x "$NM_FKWU" ]; then
    printf 'missing executable kernel: %s\n' "$NM_FKWU" >&2
    exit 1
fi

cd "$NM_REPO_ROOT"

nm_build_checkpoint_source() {
    checkpoint_destination=$1
    checkpoint_entry=$2
    : > "$checkpoint_destination"
    for checkpoint_part in \
        form/form-stdlib/core.fk \
        form/form-stdlib/sha256.fk \
        form/form-stdlib/format-arith.fk \
        form/form-stdlib/f64-bytes.fk \
        form/form-stdlib/transformer-numerics.fk \
        form/form-stdlib/transformer-block.fk \
        form/form-stdlib/transformer-backprop.fk \
        form/form-stdlib/transformer-corpus-train.fk \
        form/form-stdlib/neural-lm.fk \
        form/form-stdlib/native-model-evidence.fk \
        form/form-stdlib/native-model-live-training.fk \
        form/form-stdlib/native-model-checkpoint.fk \
        "$checkpoint_entry"
    do
        sed '/^; preludes:/d' "$checkpoint_part" >> "$checkpoint_destination"
        printf '\n' >> "$checkpoint_destination"
    done
}

nm_build_checkpoint_source "$band_source" \
    form/form-stdlib/tests/native-model-checkpoint-band.fk
nm_build_checkpoint_source "$cli_source" \
    form/form-stdlib/native-model-checkpoint-cli.fk

band=$($NM_FKWU --src "$band_source")
if [ "$band" != 4095 ]; then
    printf 'Form checkpoint band failed: expected 4095, observed %s\n' "$band" >&2
    exit 1
fi

previous=$(find "$NM_STATE_DIR" -maxdepth 1 -type f -name 'checkpoint-*.nmck' -print | sort | tail -n 1)
day=$(date -u +%Y%m%d)
epoch=$(date +%s)
destination="$NM_STATE_DIR/checkpoint-${day}-${epoch}.nmck"
candidate_temporary="$NM_STATE_DIR/.checkpoint-${day}-${epoch}-$$.tmp"

printf '%s\n%s\n%s\n' "$previous" "$candidate_temporary" "$destination" | \
    "$NM_FKWU" --src "$cli_source" > "$raw_report"
sed '/^$/d; /^0$/d; /^fkwu: warning:/d' "$raw_report" > "$report"

if ! grep -q '^checkpoint_valid=1$' "$report"; then
    printf 'Form checkpoint persistence/reload failed\n' >&2
    cat "$report" >&2
    exit 1
fi

active_path=$(awk -F= '$1 == "checkpoint_path" { print substr($0, index($0, "=") + 1); exit }' "$report")
if [ ! -f "$active_path" ]; then
    printf 'Form reported a missing active checkpoint: %s\n' "$active_path" >&2
    exit 1
fi
chmod 600 "$active_path"

cat "$report"
printf 'checkpoint_band=%s\n' "$band"
