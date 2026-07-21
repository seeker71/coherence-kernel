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

# --- QUIESCENCE: the measurement states what else was running, or it does not count ----------------
# 2026-07-21 23:19 this oracle's author measured ollama at 56-88 tok/s while metal_first_token.sh and
# fkwu saturated this host, published "the denominator is 2.8x too high", and was wrong by 2.4x. Idle
# truth that hour was 139.62. Three runs agreed with each other and all three were wrong together —
# a loaded host biases in ONE direction and reproducibly, which is what made it convincing. So the
# host is asked how busy it is BEFORE the number is taken, and the answer rides in the env file.
# (corpus: selfload)
CORES="$(sysctl -n hw.ncpu 2>/dev/null || echo 8)"
LOAD1="$(uptime | sed 's/.*averages*: *//' | awk '{print $1}' | tr -d ,)"
BUSY="$(python3 -c "print(1 if float('${LOAD1:-0}') > ${CORES} * 0.35 else 0)" 2>/dev/null || echo 0)"
SIBS="$(pgrep -fl 'fkwu|metal_first_token|metal_batched|swiftc' 2>/dev/null | grep -vc ollama_oracle || echo 0)"
if [ "$BUSY" = "1" ] || [ "${SIBS:-0}" -gt 0 ]; then
    echo "ollama_oracle: ⚠ HOST IS NOT QUIET — load1 ${LOAD1} on ${CORES} cores, ${SIBS} kernel/harness process(es) running." >&2
    echo "ollama_oracle:   Numbers taken now are SELFLOADED and biased LOW. Recorded as such; re-run on an idle host." >&2
    QUIET=0
else
    QUIET=1
fi

echo "ollama_oracle: measuring $MODEL, $RUNS runs, warming first ..." >&2
ollama run "$MODEL" "hi" >/dev/null 2>&1   # warm: load duration must not land in the sample

decodes=(); prefills=(); pcounts=()
for i in $(seq 1 "$RUNS"); do
    raw="$(ollama run "$MODEL" --verbose "$PROMPT" 2>&1 | tr -d '\033' | sed 's/\[?25[lh]//g')"
    d="$(printf '%s\n' "$raw" | awk '/^eval rate:/ {print $3}')"
    n="$(printf '%s\n' "$raw" | awk '/^eval count:/ {print $3}')"
    [ -n "${d:-}" ] && { decodes+=("$d"); printf '  decode run %s: %s tok/s over %s tokens\n' "$i" "$d" "${n:-?}" >&2; }
done

# --- PREFILL: a rate needs a BATCH SIZE beside it -------------------------------------------------
# The first version of this oracle read prompt-eval off the same ~10-token decode prompt and reported
# 4685 tok/s. That figure is meaningless and was published as untrustworthy in its own receipt: at ten
# tokens the per-token rate is dominated by fixed cost, and the runs scattered 1187 / 4432 / 4501 / 5071
# on identical input. Prefill is a BATCHED matmul — its throughput is a function of how many tokens are
# in the batch, so a prefill number without its prompt length is not a slow or fast number, it is not a
# number. Measured here over a long prompt, with the token count carried beside the rate.
LONGSEED="The ocean covers seventy-one percent of the planet surface and holds ninety-seven percent of its water. Currents move heat from the equator toward the poles, and the deep water formed at high latitudes returns along the sea floor over centuries. Plankton in the sunlit layer fix carbon that sinks as particles into the dark below. "
LONGPROMPT=""; for _ in $(seq 1 24); do LONGPROMPT="$LONGPROMPT$LONGSEED"; done
LONGPROMPT="$LONGPROMPT Summarize the paragraph above in exactly one word."

# THE PROMPT CACHE. First long-prompt run of this oracle gave 1032 / 64042 / 52930 tok/s on the SAME
# 1645-token prompt. 64 000 tok/s is not a speed — at ~3.2 GMAC/token it would be 205 TMAC/s, far past
# this machine. Runs 2 and 3 never prefilled anything: ollama matched the cached prompt prefix and
# reported a near-zero eval duration. Repetition, the usual defence against a bad measurement, was
# manufacturing the bad measurement. Each run therefore gets a UNIQUE prefix — at the FRONT, because
# prefix caching keys on the head, so a changed tail would still hit.
for i in $(seq 1 "$RUNS"); do
    UNIQ="Note $i-$$-$(od -An -N3 -tu4 /dev/urandom | tr -d ' '): read the following carefully. "
    raw="$(ollama run "$MODEL" --verbose "$UNIQ$LONGPROMPT" 2>&1 | tr -d '\033' | sed 's/\[?25[lh]//g')"
    p="$(printf '%s\n' "$raw" | awk '/^prompt eval rate:/ {print $4}')"
    pc="$(printf '%s\n' "$raw" | awk '/^prompt eval count:/ {print $4}')"
    [ -n "${p:-}" ] && { prefills+=("$p"); pcounts+=("${pc:-0}"); printf '  prefill run %s: %s tok/s over %s prompt tokens\n' "$i" "$p" "${pc:-?}" >&2; }
done

[ "${#decodes[@]}" -gt 0 ] || { echo "ollama_oracle: every decode run failed — denominator NOT measured" >&2; exit 3; }
[ "${#prefills[@]}" -gt 0 ] && PSET="${prefills[*]}" || PSET=""
[ "${#pcounts[@]}"  -gt 0 ] && PCSET="${pcounts[*]}" || PCSET=""

read -r DMED DMIN DMAX PMED PMIN PMAX PTOK SAMPLE <<EOF
$(D="${decodes[*]}" P="$PSET" PC="$PCSET" python3 -c '
import os, statistics as s
d=[float(x) for x in os.environ["D"].split()]
p=[float(x) for x in os.environ["P"].split()] or [0.0]
pc=[float(x) for x in os.environ["PC"].split()] or [0.0]
print(f"{s.median(d):.2f} {min(d):.2f} {max(d):.2f} "
      f"{s.median(p):.2f} {min(p):.2f} {max(p):.2f} {int(s.median(pc))} {len(d)}")
')
EOF

cat > "$OUT" <<EOF
# MEASURED, not quoted. Regenerate with: form/native/metal/ollama_oracle.sh
OLLAMA_DECODE=$DMED
OLLAMA_DECODE_MIN=$DMIN
OLLAMA_DECODE_MAX=$DMAX
OLLAMA_PREFILL=$PMED
OLLAMA_PREFILL_MIN=$PMIN
OLLAMA_PREFILL_MAX=$PMAX
# A prefill rate is meaningless without the batch it was measured over. Carried, always.
OLLAMA_PREFILL_TOKENS=$PTOK
OLLAMA_MODEL=$MODEL
OLLAMA_HOST="$(uname -m) $(sysctl -n machdep.cpu.brand_string 2>/dev/null || uname -s)"
OLLAMA_WHEN="$(date '+%Y-%m-%d %H:%M %Z')"
OLLAMA_RUNS=$SAMPLE
OLLAMA_SAMPLE_TOKENS="~245"
# 1 when the host was quiet at measurement time, 0 when a sibling harness was running (numbers biased LOW)
OLLAMA_QUIET=$QUIET
OLLAMA_LOAD1=$LOAD1
OLLAMA_CORES=$CORES
EOF

echo "ollama_oracle: decode median $DMED tok/s (spread $DMIN-$DMAX); prefill median $PMED tok/s over $PTOK prompt tokens (spread $PMIN-$PMAX) -> $OUT" >&2
cat "$OUT"
