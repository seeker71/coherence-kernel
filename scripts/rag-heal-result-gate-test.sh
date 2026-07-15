#!/usr/bin/env bash
set -euo pipefail

FORM="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=scripts/rag-heal-result-gate.sh
source "$FORM/scripts/rag-heal-result-gate.sh"

expect_gate_status() {
    local value="$1"
    local expected="$2"
    local actual
    set +e
    rag_heal_result_gate "$value" >/dev/null 2>&1
    actual=$?
    set -e
    [[ "$actual" == "$expected" ]] || {
        echo "rag-heal result '$value': expected $expected, got $actual" >&2
        return 1
    }
}

expect_gate_status "73" "73"
expect_gate_status "" "75"
expect_gate_status "0" "75"
expect_gate_status "unexpected" "75"
echo "rag-heal result gate: explicit refusal and unexpected-output failures verified"
