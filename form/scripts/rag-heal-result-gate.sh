#!/usr/bin/env bash
# Classify the native healer result while REF+CTOR resolution is unavailable.
# Exactly 73 is the expected explicit refusal. Empty, zero, and every other
# result are carrier failures, never successful no-ops.

rag_heal_result_gate() {
    local out="${1-}"
    if [[ "$out" == "73" ]]; then
        echo "[rag] refused native heal: nodeid-rag-v2 requires verified REF+CTOR+persisted-source resolution; use the consumer verified index carrier" >&2
        return 73
    fi
    echo "[rag] refused native heal: unexpected native gate result '${out:-<empty>}'" >&2
    return 75
}
