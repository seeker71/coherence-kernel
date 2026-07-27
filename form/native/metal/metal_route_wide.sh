#!/usr/bin/env bash
# metal_route_wide.sh — the wide softmax router's emitted MSL is COMPILED, not just grepped.
#
# form/form-stdlib/tests/moe-route-wide-msl-band.fk gates the emitted text by substring: the
# kernel name, the 256-wide arrays, the refusals, the write order, the absence of a steering
# bias. Every one of those checks can pass over text that no compiler would accept, because a
# substring is not a syntax. This harness closes that gap by handing the body's own emission to
# Apple's Metal compiler and asking it.
#
# It does NOT run the kernel. There is no device dispatch here, no comparison against the Form
# recipe's numbers, and therefore no claim that the router computes the right thing on a GPU.
# What is claimed is exactly: the body emitted Metal, and Metal accepts it. The numeric agreement
# gate belongs with a dispatch harness (metal_moe_gpu.sh's shape) and has not been run.
#
# The spine is supplied HERE, not by the cell: moe-route-wide-msl.fk emits an APPENDIX and no
# spine, so a standalone compile has to stand `fexp` up itself. That is the cell's contract being
# honoured, not worked around — and gate 5 proves the compile gate can fail, so gates 3/4 are
# known not to be vacuous.
#
# Usage:  form/native/metal/metal_route_wide.sh
set -u

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT" || exit 1
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

pass=0
fail=0
gate() { # gate <name> <ok?>
    if [ "$2" -eq 1 ]; then pass=$((pass + 1)); echo "  ok   $1"
    else fail=$((fail + 1)); echo "  FAIL $1"; fi
}

if [ ! -x ./fkwu ]; then
    echo "metal_route_wide: no ./fkwu — build it first (cc -O2 -o fkwu runtime/fkwu-uni.c)"
    exit 1
fi
if ! xcrun -sdk macosx metal --version >/dev/null 2>&1; then
    echo "metal_route_wide: no Metal compiler on this host — SKIPPED, not passed"
    exit 2
fi

# ── emit ────────────────────────────────────────────────────────────────────
cat > "$WORK/emit.fk" <<'EOF'
; preludes: form-stdlib/core.fk form-stdlib/moe-route-wide-msl.fk
(print_str (mrw-msl-appendix))
EOF
# fkwu prints the top-level result after print_str's output. Drop it BY POSITION with `sed '$d'`,
# never by content: filtering `^0$` would also delete any emitted line that is legitimately "0".
./fkwu --src "$WORK/emit.fk" 2>/dev/null | sed '$d' > "$WORK/body.msl"
bytes=$(wc -c < "$WORK/body.msl" | tr -d ' ')
gate "1 the body emits a non-empty appendix ($bytes bytes)" "$([ "$bytes" -gt 200 ] && echo 1 || echo 0)"

# ── the read-back band still agrees ─────────────────────────────────────────
verdict=$(./fkwu --src form/form-stdlib/tests/moe-route-wide-msl-band.fk 2>/dev/null | tail -1)
gate "2 read-back band = 255 (got $verdict)" "$([ "$verdict" = "255" ] && echo 1 || echo 0)"

spine() { printf '#include <metal_stdlib>\ninline float fexp(float x) { return metal::exp(x); }\n'; }

# ── compile ─────────────────────────────────────────────────────────────────
{ spine; cat "$WORK/body.msl"; } > "$WORK/router.metal"
if xcrun -sdk macosx metal -ffp-contract=off -c "$WORK/router.metal" -o "$WORK/router.air" 2> "$WORK/err.txt"; then
    gate "3 Apple's Metal compiler accepts the emitted kernel" 1
else
    gate "3 Apple's Metal compiler accepts the emitted kernel" 0
    sed 's/^/      /' "$WORK/err.txt" | head -8
fi
airb=$(wc -c < "$WORK/router.air" 2>/dev/null | tr -d ' ' || echo 0)
gate "4 a non-empty .air object came out ($airb bytes)" "$([ "${airb:-0}" -gt 0 ] && echo 1 || echo 0)"

# ── the compile gate is not vacuous ─────────────────────────────────────────
# Break one token in the emission and require the compiler to REFUSE it. Without this, gates 3
# and 4 would pass just as loudly against a compiler that accepts anything handed to it.
{ spine; sed 's/float p\[256\];/float p\[256\/\/;/' "$WORK/body.msl"; } > "$WORK/broken.metal"
if xcrun -sdk macosx metal -ffp-contract=off -c "$WORK/broken.metal" -o "$WORK/broken.air" >/dev/null 2>&1; then
    gate "5 a deliberately broken emission is REFUSED (control)" 0
else
    gate "5 a deliberately broken emission is REFUSED (control)" 1
fi

echo
if [ "$fail" -eq 0 ]; then
    echo "metal_route_wide: VERDICT PASS, $pass gates"
    echo "  claimed: the body emits Metal and Metal accepts it."
    echo "  NOT claimed: that the kernel was dispatched, or that it agrees numerically"
    echo "               with mm-route-weights. No device ran here."
    exit 0
fi
echo "metal_route_wide: VERDICT FAIL, $fail of $((pass + fail)) gates"
exit 1
