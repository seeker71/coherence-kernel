#!/usr/bin/env bash
# ollama_oracle.sh — MEASURE the external denominator instead of quoting it.
#
# Every ratio the inference program reports against "the world" has been divided by two constants
# (157.83 decode, 640.94 prefill) carried from a measurement made elsewhere. Stone 5 labelled them
# honestly as quoted-and-not-re-run. They then travelled into two harnesses and two recipes and became
# the denominator of every gap claim in the program. Re-measured 2026-07-21 23:19 WITA on this machine,
# model and blob, warm, three ~245-token samples: 88.32 / 56.95 / 55.93 tok/s. The quoted decode figure
# was ~2.8x high, so the body has been reporting itself further behind than it is.
#
# The repair is not a better constant. It is EXPIRY: a denominator carries the date, the host and the
# harness that produced it, and a reader that cannot re-derive it says so out loud rather than dividing
# by a number whose currency nobody can check. (corpus: stalequote)
#
# Writes form/native/metal/.ollama-oracle.env — sourced by the harnesses:
#   OLLAMA_DECODE / OLLAMA_PREFILL   tok/s, MEDIAN of the runs
#   OLLAMA_DECODE_MIN / _MAX         the spread, because a single figure hides it
#   OLLAMA_MODEL / OLLAMA_HOST / OLLAMA_WHEN / OLLAMA_RUNS / OLLAMA_SAMPLE_TOKENS
#
# usage: ollama_oracle.sh [model] [runs]     default: llama3.2:3b 3
set -uo pipefail

MODEL="${1:-llama3.2:3b}"
RUNS="${2:-3}"
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT="$HERE/.ollama-oracle.env"
PROMPT="Write a 200-word essay about the ocean."

command -v ollama >/dev/null 2>&1 || { echo "ollama_oracle: no ollama on PATH — denominator NOT measured" >&2; exit 2; }

echo "ollama_oracle: measuring $MODEL, $RUNS runs, warming first ..." >&2
ollama run "$MODEL" "hi" >/dev/null 2>&1   # warm: load duration must not land in the sample

decodes=(); prefills=()
for i in $(seq 1 "$RUNS"); do
    raw="$(ollama run "$MODEL" --verbose "$PROMPT" 2>&1 | tr -d '\033' | sed 's/\[?25[lh]//g')"
    d="$(printf '%s\n' "$raw" | awk '/^eval rate:/ {print $3}')"
    p="$(printf '%s\n' "$raw" | awk '/^prompt eval rate:/ {print $4}')"
    n="$(printf '%s\n' "$raw" | awk '/^eval count:/ {print $3}')"
    [ -n "${d:-}" ] && { decodes+=("$d"); prefills+=("${p:-0}"); printf '  run %s: decode %s tok/s, prefill %s tok/s, %s tokens\n' "$i" "$d" "${p:-?}" "${n:-?}" >&2; }
done

[ "${#decodes[@]}" -gt 0 ] || { echo "ollama_oracle: every run failed — denominator NOT measured" >&2; exit 3; }

read -r DMED DMIN DMAX PMED SAMPLE <<EOF
$(D="${decodes[*]}" P="${prefills[*]}" python3 -c '
import os, statistics as s
d=[float(x) for x in os.environ["D"].split()]
p=[float(x) for x in os.environ["P"].split()]
print(f"{s.median(d):.2f} {min(d):.2f} {max(d):.2f} {s.median(p):.2f} {len(d)}")
')
EOF

cat > "$OUT" <<EOF
# MEASURED, not quoted. Regenerate with: form/native/metal/ollama_oracle.sh
OLLAMA_DECODE=$DMED
OLLAMA_DECODE_MIN=$DMIN
OLLAMA_DECODE_MAX=$DMAX
OLLAMA_PREFILL=$PMED
OLLAMA_MODEL=$MODEL
OLLAMA_HOST="$(uname -m) $(sysctl -n machdep.cpu.brand_string 2>/dev/null || uname -s)"
OLLAMA_WHEN="$(date '+%Y-%m-%d %H:%M %Z')"
OLLAMA_RUNS=$SAMPLE
OLLAMA_SAMPLE_TOKENS="~245"
EOF

echo "ollama_oracle: decode median $DMED tok/s (spread $DMIN-$DMAX), prefill median $PMED tok/s -> $OUT" >&2
cat "$OUT"
