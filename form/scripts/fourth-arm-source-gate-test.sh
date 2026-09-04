#!/usr/bin/env bash
# Focused regression for the validation architecture: the fourth proof leg must
# stay available from runtime fkwu source when every flattened-table artifact is
# absent. This test neither reads nor writes the real fourth-arm cache.

set -euo pipefail
cd "$(dirname "$0")/.."

FORM_FOURTH_EXECUTION_MODE=source
FORM_FOURTH_SOURCE_FKWU=../fkwu
GO_BIN=form-kernel-go/bin-go

# shellcheck source=fourth-arm.sh
source scripts/fourth-arm.sh

tmp="$(mktemp -d "${TMPDIR:-/tmp}/form-fourth-source-gate.XXXXXX")"
trap 'rm -rf "$tmp"' EXIT

FOURTH_DIR="$tmp/cache"
FOURTH_INDEX="$tmp/absent-table-index.tsv"
FOURTH_FLATTEN_TABLE="$tmp/absent-T-flat.txt"
FOURTH_SOURCE_RUN_DIR="$tmp/source-run"
FOURTH_SOURCE_TEXT_DIR="$tmp/lowered"

build_fourth
fourth_available
fourth_prepare_all
[[ ! -e "$FOURTH_INDEX" ]]

# A normal manifest band executes its own declared source closure.
normal_src="$(fourth_prepare_source_workload "$FOURTH_SOURCE_RUN_DIR" \
    form-stdlib/core.fk form-stdlib/learning-trend.fk \
    form-stdlib/tests/learning-trend-band.fk)"
normal="$("$FORM_FOURTH_SOURCE_FKWU" "$normal_src" 2>"$tmp/normal.err")"
[[ "$normal" == 127 ]]
! grep -Eq 'unresolved-call|error:' "$tmp/normal.err"

# A legacy headerless band receives only an ephemeral core prelude; no table.
legacy="$(fourth_prepare_source_band form-stdlib/tests/int-literal-width-band.fk "$FOURTH_SOURCE_RUN_DIR")"
legacy_out="$("$FORM_FOURTH_SOURCE_FKWU" "$legacy" 2>"$tmp/legacy.err")"
[[ "$legacy_out" == 9 ]]
! grep -Eq 'unresolved-call|error:' "$tmp/legacy.err"

# The source path carries the actual auto-JIT decision law, not merely a shell
# switch: hot+pure crystallizes, impure refuses, cooled melts, and hysteresis
# prevents boundary thrash. Choice/cut/nothing likewise remain Form semantics
# in the streamed allowance organ rather than becoming validation control flow.
jit_src="$(fourth_prepare_source_band form-stdlib/tests/jit-decision-band.fk "$FOURTH_SOURCE_RUN_DIR")"
jit_out="$("$FORM_FOURTH_SOURCE_FKWU" "$jit_src" 2>"$tmp/jit.err")"
[[ "$jit_out" == 11111 ]]
! grep -Eq 'unresolved-call|error:' "$tmp/jit.err"

allowance_src="$(fourth_prepare_source_band form-stdlib/tests/form-cli-allowance-band.fk "$FOURTH_SOURCE_RUN_DIR")"
allowance_out="$("$FORM_FOURTH_SOURCE_FKWU" "$allowance_src" 2>"$tmp/allowance.err")"
[[ "$allowance_out" == 2047 ]]
! grep -Eq 'unresolved-call|error:' "$tmp/allowance.err"

# BML crosses the Form source-text lens on its declared host-I/O sibling lane,
# then the lowered module (including top-level constants) executes on fkwu.
# The fourth result therefore witnesses BML meaning, not a raw section parse,
# binary-loader native, flattened table, or copied expected scalar.
bml_ledger="$(fourth_prepare_source_text form-stdlib/bml-capability-ledger.bml)"
bml_band="$(fourth_prepare_source_text form-stdlib/tests/bml-capability-ledger-band.bml)"
[[ -s "$bml_ledger" && -s "$bml_band" ]]
bml_src="$(fourth_prepare_source_workload "$FOURTH_SOURCE_RUN_DIR" \
    form-stdlib/core.fk form-stdlib/core.fk "$bml_ledger" "$bml_band")"
bml_out="$("$FORM_FOURTH_SOURCE_FKWU" "$bml_src" 2>"$tmp/bml.err")"
[[ "$bml_out" == 255 ]]
! grep -Eq 'unresolved-call|error:' "$tmp/bml.err"

echo "fourth-arm source gate: PASS (declared + legacy + BML-lowered source + JIT/control laws, no table/index)"
