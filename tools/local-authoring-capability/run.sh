#!/usr/bin/env bash
# run.sh — STONE 44's instrument. Ask a LOCAL model, offline, one of four tasks that
# actually occurred in this program, and record the raw answer and the wall clock.
#
# WHAT THIS MEASURES, AND NOTHING WIDER (aporon). These four prompts, on the models
# named on the command line, once each at temperature 0. It does NOT measure "can local
# models code". It does not measure any model not run here. It grades nothing — grading
# is `grade.sh` and, for the one generative task with a machine answer, `fkwu` itself.
#
# OFFLINE. Every model tag passed here must be resident in `ollama list` WITHOUT a
# `:cloud` suffix; the harness refuses a `:cloud` tag outright, because the whole
# question is what this machine can do with no network.
#
# thawtax (848): a model is warmed with a one-token throwaway request BEFORE the timed
# request, so the number is inference, not mmap. The warm request is timed too and
# reported separately, so the tax is visible rather than hidden.
#
# gapghost (871): one clock. curl times its own request (`%{time_total}`), so the number
# is the single call and nothing around it, and the machine load at that instant is
# recorded beside it — five sibling lineages are live on this host and a wall-clock
# number that does not carry the load is a number pretending to be alone.
#
# The body carries the meaning on both sides of each call (ask.bml beside this file):
# it writes the request bodies and reads the answer into <stem>.txt and <stem>.json.
#
# usage:  tools/local-authoring-capability/run.sh <model-tag> <out-dir>

set -u
MODEL="${1:?usage: run.sh <model-tag> <out-dir>}"
OUT="${2:?usage: run.sh <model-tag> <out-dir>}"
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
ASK="tools/local-authoring-capability/ask.bml"

case "$MODEL" in
  *:cloud) echo "REFUSED: '$MODEL' is a cloud tag; this stone measures LOCAL only." >&2; exit 2;;
esac

if ! ollama list | awk '{print $1}' | grep -qx -- "$MODEL"; then
  echo "REFUSED: '$MODEL' is not resident in ollama list." >&2; exit 2
fi
[ -x "$ROOT/fkwu" ] || { echo "REFUSED: the body's kernel is missing: $ROOT/fkwu" >&2; exit 2; }

SLUG="$(printf '%s' "$MODEL" | tr '/:.' '___')"
mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"

generate () {   # generate <body-file> <answer-file> — prints curl's own seconds; exit is curl's
  curl -sS --max-time 3600 -o "$2" -w '%{time_total}' \
    http://127.0.0.1:11434/api/generate -H "Content-Type: application/json" -d @"$1"
}

ask () {   # ask <task-name> <prompt-file> <num_predict>
  local name="$1" pf="$2" npred="$3" stem="$OUT/${SLUG}__${name}"
  printf 'body\n%s\n%s\n%s\n%s\n' "$MODEL" "$pf" "$npred" "$stem" | (cd "$ROOT" && ./fkwu "$ASK") >/dev/null 2>&1 \
    || { echo "  ${SLUG}__${name}: the body could not write the request" >&2; return 1; }
  # thawtax: the warm request. Timed, reported, and thrown away.
  local warm dt rc load
  warm="$(generate "$stem.warm.json" /dev/null)" || warm=0
  dt="$(generate "$stem.request.json" "$stem.response.json")"; rc=$?
  set -- $(sysctl -n vm.loadavg); load="${2:-0}"
  printf 'record\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n' "$MODEL" "$pf" "$stem" "$rc" "${warm:-0}" "${dt:-0}" "$load" \
    | (cd "$ROOT" && ./fkwu "$ASK") 2>/dev/null
  rm -f "$stem.warm.json" "$stem.request.json" "$stem.response.json"
}

echo "=== $MODEL ==="
ask t1 "$HERE/tasks/t1-prelude.txt"  64
ask t2 "$HERE/tasks/t2-recipe.txt"   512
ask t3 "$HERE/tasks/t3-read.txt"     256
ask t4 "$HERE/tasks/t4-mla.txt"      1400

# snugcause (t2b): the variant of t2 that pattern-echo of the example cannot produce —
# a filter AND a cube, neither shown. Run only for models that PASSED t2.
if [ "${SNUGCAUSE:-0}" = "1" ]; then
  ask t2b "$HERE/tasks/t2b-snugcause.txt" 512
fi
