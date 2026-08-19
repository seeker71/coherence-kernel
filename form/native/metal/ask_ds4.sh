#!/usr/bin/env bash
# ask_ds4.sh — the doorknob on the form-shell ASK door for the DeepSeek-V4-Flash lane.
#
#   form/native/metal/ask_ds4.sh "The three primary colors are"
#   form/native/metal/ask_ds4.sh -n 60 "Water boils at a temperature of"
#   FORM_DS4_SITUATION="backup restore ran out of its window" \
#   FORM_DS4_TOKEN_HOOK_DOOR=form-stdlib/dsv4-hook-door-control.fk \
#     form/native/metal/ask_ds4.sh "the run status is"
#
# WHAT THIS FILE IS NOW. Two acts: stage the request as keyed stdin lines, and exec
# the native runner on form-stdlib/fsh-ask-ds4.fk. Every decision this file used to
# make — the contract read, the path membrane, the env wiring, the output parse, the
# stream-sanity reading, the footer — lives in that Form cell, held to by
# form-stdlib/tests/fsh-ask-ds4-band.fk. Bash contributes no parse and no verdict;
# the transmutation receipt is receipts/2026-08-19-the-hook-crosses-the-loop.md.
#
# The RADIUS (greedy, base-model continuation, hard step stop, not ds4's stream) is
# stated by the ask contract itself: FORM_DS4_CONTRACT_ONLY=1 prints it and stops.

set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
FKWU="$ROOT/../fkwu"

STEPS=40
while [[ $# -gt 0 ]]; do
    case "$1" in
        -n) STEPS="$2"; shift 2 ;;
        -h|--help) sed -n '2,10p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) break ;;
    esac
done
PROMPT="${*:-}"
[[ -x "$FKWU" ]] || { echo "ask_ds4: native fkwu is missing: $FKWU"; exit 1; }
if [[ "${FORM_DS4_CONTRACT_ONLY:-0}" != "1" && -z "$PROMPT" ]]; then
    echo "usage: ask_ds4.sh [-n steps] \"your prompt\""; exit 2
fi

cd "$ROOT"
{
    printf 'steps %s\n' "$STEPS"
    [[ "${FORM_DS4_CONTRACT_ONLY:-0}" == "1" ]] && printf 'contract_only 1\n'
    [[ -n "${FORM_DS4_GATES:-}" ]] && printf 'gates %s\n' "$FORM_DS4_GATES"
    [[ -n "${FORM_DS4_TOKEN_HOOK_DOOR:-}" ]] && printf 'hook_door %s\n' "$FORM_DS4_TOKEN_HOOK_DOOR"
    [[ -n "${FORM_DS4_SITUATION:-}" ]] && printf 'situation %s\n' "$FORM_DS4_SITUATION"
    [[ -n "$PROMPT" ]] && printf 'prompt %s\n' "$PROMPT"
    printf 'end\n'
} | exec "$FKWU" form-stdlib/fsh-ask-ds4-main.fk
