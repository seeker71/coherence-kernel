#!/usr/bin/env bash
# ask_ds4.sh — the ASK door for the form-native DeepSeek-V4-Flash lane. One question in words, one
# answer in words, every arithmetic operation executed by kernels this body emits.
#
#   form/native/metal/ask_ds4.sh "The three primary colors are"
#   form/native/metal/ask_ds4.sh -n 60 "Water boils at a temperature of"
#
# WHAT THIS CARRIER IS. A stopwatch and a subprocess. It contributes no arithmetic and no recipe: the
# tokenizer is form-stdlib/dsv4-tokenizer-cli.fk, the forward is metal_dsv4_stack.sh's 43 stacked
# layers, and the detokenizer is the same tokenizer cell read the other way. This file chooses the
# number of steps and prints what comes back. Same discipline as metal_ask.sh, which is this door's
# sibling for llama3.2:3b.
#
# WHY IT IS SEPARATE FROM metal_ask.sh. That one drives a different model through
# metal_first_token.sh, a single-token harness. This drives the sequence lane, which carries a KV
# cache, the second (compressed) cache, and the router across steps. Different model, different
# harness, same verb — so they are siblings rather than one file with a switch.
#
# THE RADIUS, said before the answer is read (aporon):
#   * GREEDY only. temp 0, argmax every step. No sampling, no top-p, no seed. A second run of the
#     same question returns the same words.
#   * NO chat template, NO system prompt, NO stop token. This is a BASE-model continuation: the
#     answer continues the sentence you typed. "The capital of France is" works; "What is the capital
#     of France?" will be continued as a question, not answered as one.
#   * The step count is a hard stop, not an end of thought. The last sentence will usually be cut.
#   * It is NOT ds4's stream. On the prompt this lane was verified against, 24 of 24 tokens match
#     ds4 exactly; on others the two agree for a dozen tokens and then paraphrase differently, because
#     at a near tie ds4's own two backends disagree with each other too
#     (receipts/2026-08-01-the-model-is-useful.md). The facts agree; the wording may not.
#
# Proven by: the receipts above, and metal_dsv4_stack.sh's own 106 gates, which this door does not
# re-run — pass FORM_DS4_GATES=1 to see them.

set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form

STEPS=40
while [[ $# -gt 0 ]]; do
    case "$1" in
        -n) STEPS="$2"; shift 2 ;;
        -h|--help) sed -n '2,8p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) break ;;
    esac
done
PROMPT="${*:-}"
[[ -n "$PROMPT" ]] || { echo "usage: ask_ds4.sh [-n steps] \"your prompt\""; exit 2; }

# the cache must outrun the walk: the sequence lane refuses when steps reach the cap, because a
# silently sliding window is the same class of bug as a rewritten history (dsv4-kv-cache.fk's aporon).
export FORM_DS4_KV_STEPS="$STEPS"
export FORM_DS4_KV_CAP=$(( STEPS + 8 ))
export FORM_DS4_KV_SEQUENCE=1
export FORM_DS4_MATCH_ORDER=1
export FORM_DS4_GATES="${FORM_DS4_GATES:-0}"
export FORM_DS4_PROMPT="$PROMPT"

t0=$(date +%s)
out="$("$ROOT/native/metal/metal_dsv4_stack.sh" 2>&1)"; rc=$?
t1=$(date +%s)

if [[ $rc -ne 0 ]]; then
    echo "$out" | grep -E "^(FAIL|SKIP)" | head -3
    echo "ask_ds4: the lane did not complete (rc=$rc)"; exit $rc
fi

ids="$(printf '%s\n' "$out" | grep -oE 'FEEDBACK \([^)]*\): \[[0-9, ]*\]' | tail -1 | grep -oE '\[[0-9, ]*\]')"
answer="$(printf '%s\n' "$out" | grep -A 1 'NATIVE DECODED CONTINUATION' | tail -1)"
speed="$(printf '%s\n' "$out" | grep -oE 'generation [0-9.]+ t/s' | tail -1)"

blob="$(printf '%s\n' "$out" | grep -oE '^ds4 blob: [^ ]+' | head -1 | sed 's/^ds4 blob: //')"

# THE STREAM IS READ BEFORE IT IS QUOTED. form-stdlib/ds4-stream-sanity.fk's three readings —
# reserved-block emission, single-id multiplicity, alternating-run length — proven in
# form-stdlib/tests/ds4-stream-sanity-band.fk (verdict 255, five mutations refuted). This is the
# reading whose absence let " the name, the<|place_holder_mm_span_0155|> detection device detection
# device" be quoted for three days under 107 green gates. It is a floor, not an oracle: it refutes a
# class, and a fluent confident wrong sentence passes it. ~/models/ds4-engine/ds4 answers truth.
verdict=""
if [[ -n "$ids" ]]; then
    _fk="$ROOT/../fkwu"
    if [[ -x "$_fk" ]]; then
        pdir="$(mktemp -d -t ds4sanity)"; probe="$pdir/probe.fk"; trap 'rm -rf "$pdir"' EXIT
        printf '; preludes: form-stdlib/ds4-stream-sanity.fk\n(dss-trip-count (list %s))\n' \
               "$(printf '%s' "$ids" | tr -d '[],')" > "$probe"
        trips="$(cd "$ROOT" && "$_fk" --src "$probe" 2>/dev/null | tail -1)"
        case "$trips" in
            0) verdict="stream sane (0 of 3 readings tripped)" ;;
            1|2|3) verdict="STREAM DEGENERATE — $trips of 3 readings tripped (repetition, cycling, or a reserved token)" ;;
        esac
    fi
fi

printf '\n%s%s\n' "$PROMPT" "$answer"
printf '\n  [form-native · 43 layers · %s · %ss wall · greedy, base-model continuation]\n' \
       "${speed:-speed unmeasured}" "$((t1 - t0))"
printf '  [weights: %s]\n' "${blob:-unnamed}"
[[ -n "$verdict" ]] && printf '  [%s]\n' "$verdict"
printf '  [token ids: %s]\n' "${ids:-none}"
