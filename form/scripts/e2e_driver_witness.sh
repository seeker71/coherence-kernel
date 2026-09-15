#!/usr/bin/env bash
# e2e_driver_witness.sh — run the end-to-end chat driver's REAL lane on fkwu and write the honest
# per-stage receipt. Two fkwu runs: (1) the real community-page prompt through template +
# tokenize (stops honestly at tokenize — the llama3 rank-table carrier is pending), (2) the driver's
# forward+sample binding over REAL llama3.2:3b Q6_K bytes at today's real width (dim=4, 1 layer).
# The body's clock times them and the body writes the receipt (form/scripts/e2e-driver-receipt.bml):
# form/docs/receipts/e2e_native_lane_status.json. Never fabricates: every id in the receipt
# came out of a Form recipe walking real weight bytes; the response text slot stays a named refusal
# until tokenize/detokenize bindings land.
set -euo pipefail
cd "$(dirname "$0")/../.."   # repo root

if [[ ! -x ./fkwu || runtime/fkwu-uni.c -nt ./fkwu ]]; then
    echo "building runtime fkwu..." >&2
    cc -O2 -o ./fkwu.new runtime/fkwu-uni.c && mv ./fkwu.new ./fkwu
fi

BLOB="$HOME/.ollama/models/blobs/sha256-dde5aa3fc5ffc17176b5e8bdc82f587b24b2678c6c66101bf7da77af9f7ccdff"
[[ -f "$BLOB" ]] || { echo "llama3.2:3b blob absent at $BLOB — the forward stage cannot run on real weights" >&2; exit 1; }

now_ms() { ./fkwu observe/clock-ms-run.bml 2>/dev/null; }

t0=$(now_ms)
OUT1=$(./fkwu form/scripts/e2e-run-prompt.fk 2>/dev/null | tail -1)
t1=$(now_ms)
OUT2=$(./fkwu form/scripts/e2e-run-forward-real.fk 2>/dev/null | tail -1)
t2=$(now_ms)

[[ -n "$OUT1" && -n "$OUT2" ]] || { echo "EMPTY fkwu output — treat as a silent crash, run without stderr suppression" >&2; exit 1; }

echo "run 1 (template+tokenize, real prompt):   $OUT1  ($((t1-t0)) ms)"
echo "run 2 (forward+sample, real 3b weights):  $OUT2  ($((t2-t1)) ms)"

printf '%s\n%s\n%s\n%s\n%s\n%s\n' "$OUT1" "$OUT2" "$((t1-t0))" "$((t2-t1))" "$BLOB" "$(date -u +%Y-%m-%dT%H:%M:%SZ)" \
    | ./fkwu form/scripts/e2e-driver-receipt.bml 2>/dev/null
