#!/usr/bin/env bash
# fourth-arm-gate.sh — four-way gate for manifest rows: each named band runs
# through validate.sh (its declared preludes, suite-mode shape) and must
# answer "0 divergent" WITH the fourth leg firing. Usage:
#   scripts/fourth-arm-gate.sh stem [stem ...]
set -u
cd "$(dirname "$0")/.."

# fourth-arm.sh owns the honest preludes reader (fourth_band_prelude_mods_raw):
# multi-line "; preludes:" headers and "; " continuation lines, core.fk kept in
# declared position. A first-line-only grep truncates multi-line headers and the
# reference kernels crash unbound — a false DIVERGENT (witnessed 2026-07-17).
# shellcheck source=fourth-arm.sh
. scripts/fourth-arm.sh

gate_one() {
    local stem="$1" band="form-stdlib/tests/$stem-band.fk" pres out
    pres="$(fourth_band_prelude_mods_raw "$band")"
    if [[ -z "$pres" ]]; then
        [[ -f "form-stdlib/$stem.fk" ]] && pres="form-stdlib/$stem.fk"
    fi
    # The three reference kernels need core.fk even when the band's header omits
    # it (only the fourth arm's shim mirrors core). Prepend it only when absent
    # so a band that declares core.fk mid-list keeps its declared order.
    if ! grep -qE '(^|/)core\.fk$' <<<"$pres"; then
        pres="form-stdlib/core.fk${pres:+$'\n'$pres}"
    fi
    # shellcheck disable=SC2086
    out="$(./validate.sh $pres "$band" 2>&1)"
    if echo "$out" | grep -q "0 divergent" && echo "$out" | grep -q "fourth arm: 1"; then
        echo "PASS-4WAY  $stem"
    elif echo "$out" | grep -q "0 divergent"; then
        echo "NO-FOURTH  $stem"
    else
        echo "DIVERGENT  $stem"
        echo "$out" | grep -A5 '✗' | head -8
    fi
}

for stem in "$@"; do
    gate_one "$stem" &
    # bash 3.2 (macOS) has no `wait -n`; a short poll throttles the fan-out
    while [[ "$(jobs -r | wc -l)" -ge 4 ]]; do sleep 1; done
done
wait
