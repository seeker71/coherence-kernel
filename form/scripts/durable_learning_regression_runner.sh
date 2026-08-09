#!/bin/sh
set -eu

repo_root=$(CDPATH= cd "$(dirname "$0")/../.." && pwd)
cd "$repo_root"

passes=0

run_exact() {
    source_path=$1
    expected=$2
    actual=$(./fkwu "$source_path")
    if [ "$actual" != "$expected" ]; then
        printf 'FAIL command=./fkwu %s expected=%s actual=%s\n' \
            "$source_path" "$expected" "$actual" >&2
        exit 1
    fi
    passes=$((passes + 1))
    printf 'PASS source=%s verdict=%s\n' "$source_path" "$actual"
}

require_hash() {
    label=$1
    actual=$2
    expected=$3
    if [ "$actual" != "$expected" ]; then
        printf 'FAIL carrier=%s expected_sha256=%s actual_sha256=%s\n' \
            "$label" "$expected" "$actual" >&2
        exit 1
    fi
}

run_exact form/form-stdlib/tests/offer-ack-core-band.fk 1023
run_exact form/form-stdlib/tests/offer-ack-alternative-band.fk 32767
run_exact form/form-stdlib/tests/cell-coordinate-stream-band.fk 32767
run_exact form/form-stdlib/tests/cell-coordinate-dag-stream-band.fk 32767
run_exact form/form-stdlib/tests/living-world-organ-band.fk 65535
run_exact form/form-stdlib/tests/living-world-lens-channel-band.fk 32767
run_exact form/form-stdlib/tests/living-world-recipe-execution-band.fk 32767
run_exact form/form-stdlib/tests/living-world-action-selection-band.fk 32767
run_exact form/form-stdlib/tests/living-world-action-execution-band.fk 32767
run_exact form/form-stdlib/tests/living-world-action-learning-band.fk 32767
run_exact form/form-stdlib/tests/living-world-continuity-band.fk 32767
run_exact form/form-stdlib/tests/living-world-continuity-durable-band.fk 32767

run_exact form/form-stdlib/tests/living-world-durable-learning-clean.fk 63
run_exact form/form-stdlib/tests/living-world-durable-learning-write.fk 31
run_exact form/form-stdlib/tests/living-world-durable-learning-learn.fk 65535
run_exact form/form-stdlib/tests/living-world-durable-learning-adopt.fk 65535
run_exact form/form-stdlib/tests/living-world-durable-learning-reload.fk 65535

before_hash=$(shasum -a 256 \
    /tmp/coherence-living-world-durable-learning-before-v1.ccs \
    | awk '{print $1}')
after_hash=$(shasum -a 256 \
    /tmp/coherence-living-world-durable-learning-after-v1.ccs \
    | awk '{print $1}')
final_hash=$(shasum -a 256 \
    /tmp/coherence-living-world-durable-learning-final-v1.ccs \
    | awk '{print $1}')

require_hash before "$before_hash" \
    5d5b2301d3be7dd07068adcfe5bbc5a8816ecf06a3cbc78efc0b5e8776e6ec00
require_hash after "$after_hash" \
    88c49066e41a3154e943b55767409a654fc88f208d5f4d920c5c3502d86fcee6
require_hash final "$final_hash" \
    f3990aaa308055dc0ff2a8f264d39530b98549cc8101864f8c539a5311376048

printf 'PASS carrier=before sha256=%s\n' "$before_hash"
printf 'PASS carrier=after sha256=%s\n' "$after_hash"
printf 'PASS carrier=final sha256=%s\n' "$final_hash"

run_exact form/form-stdlib/tests/living-world-durable-learning-clean.fk 63
printf 'focused_regression_passes=%s\n' "$passes"
