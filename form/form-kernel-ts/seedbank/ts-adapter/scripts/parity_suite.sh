#!/usr/bin/env bash
# parity_suite.sh — three-way parity gate for the TS adapter.
#
# Every shipped demo runs through:
#   1. tsc transpile + node — the canonical TypeScript runtime
#   2. ts-eval — our captured-recipe TS walker (no .fk, no kernel binary)
#   3. ts-run  — emit .fk, execute on fkwu, the canonical kernel
# All three must agree on the printed value of the final bare expression.
#
# Add new files to PARITY_FILES as they're ripened. Run from
# form/form-kernel-ts/.

set -euo pipefail

PARITY_FILES=(
    "seedbank/ts-adapter/examples/ts_arith_demo.ts"
    "seedbank/ts-adapter/examples/ts_recursion_demo.ts"
    "seedbank/ts-adapter/examples/ts_arrow_demo.ts"
    "seedbank/ts-adapter/examples/ts_accumulator_demo.ts"
    "seedbank/ts-adapter/examples/ts_composition_demo.ts"
)

ROOT="$(cd "$(dirname "$0")/../../../../.." && pwd)"
FKWU="$ROOT/fkwu"
HAS_FKWU=0
if [[ -x "$FKWU" ]]; then
    HAS_FKWU=1
else
    echo "note: fkwu not built at $FKWU" >&2
    echo "      ts-run column will be skipped. Build it from the checkout root:" >&2
    echo "      cc -O2 -o fkwu runtime/fkwu-uni.c" >&2
    echo ""
fi

PASS=0
FAIL=0

# node-eval wraps the file's last bare expression in console.log() and
# runs it through node. The file is valid JS (the adapter supports a TS
# subset that is also a JS subset), so direct `node --input-type=module`
# works without a TS transpile step. This is the canonical runtime —
# the seam our captured-recipe walker and our .fk emitter both have to
# agree with.
node_eval() {
    local f="$1"
    node --input-type=module -e "$(node -e "
        const fs = require('fs');
        const src = fs.readFileSync('$f', 'utf8');
        const lines = src.split('\n');
        let lastIdx = -1;
        for (let i = lines.length - 1; i >= 0; i--) {
            const t = lines[i].trim();
            if (t === '' || t.startsWith('//') || t === '}' || t.endsWith(';')) continue;
            lastIdx = i;
            break;
        }
        if (lastIdx >= 0) {
            lines[lastIdx] = 'console.log(' + lines[lastIdx] + ')';
        }
        process.stdout.write(lines.join('\n'));
    ")" 2>&1 | tail -1
}

for f in "${PARITY_FILES[@]}"; do
    if [[ ! -f "$f" ]]; then
        echo "  SKIP $f (file missing)"
        continue
    fi

    node_result=$(node_eval "$f")

    ts_result=$(npx tsx seedbank/ts-adapter/src/main.ts ts-eval "$f" 2>&1 | tail -1)

    if [[ "$HAS_FKWU" -eq 1 ]]; then
        fk_path="${f%.ts}.fk"
        npx tsx seedbank/ts-adapter/src/main.ts ts-compile "$f" "$fk_path" >/dev/null 2>&1
        # fkwu exits nonzero when the compile carried errors; the printed
        # value is then a fold over nothing, so the exit code joins the gate.
        fkwu_rc=0
        fkwu_result=$("$FKWU" "$fk_path" </dev/null 2>&1 | tail -1) || fkwu_rc=$?
        if [[ "$fkwu_rc" -eq 0 && "$node_result" == "$ts_result" && "$node_result" == "$fkwu_result" ]]; then
            echo "  OK   $f  → $node_result"
            PASS=$((PASS + 1))
        else
            echo "  FAIL $f"
            echo "       node: $node_result"
            echo "       ts:   $ts_result"
            echo "       fkwu: $fkwu_result (rc=$fkwu_rc)"
            FAIL=$((FAIL + 1))
        fi
    else
        if [[ "$node_result" == "$ts_result" ]]; then
            echo "  OK   $f  → $node_result  (2-way, fkwu skipped)"
            PASS=$((PASS + 1))
        else
            echo "  FAIL $f"
            echo "       node: $node_result"
            echo "       ts:   $ts_result"
            FAIL=$((FAIL + 1))
        fi
    fi
done

echo ""
echo "parity_suite: $PASS passing, $FAIL failing"
exit $FAIL
