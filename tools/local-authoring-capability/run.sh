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
# gapghost (871): one clock. `date +%s%N` is not portable on darwin; the timing is taken
# by python3's time.monotonic() around the single curl, in one process, and the machine
# load at that instant is recorded beside it — five sibling lineages are live on this
# host and a wall-clock number that does not carry the load is a number pretending to
# be alone.
#
# usage:  tools/local-authoring-capability/run.sh <model-tag> <out-dir>

set -u
MODEL="${1:?usage: run.sh <model-tag> <out-dir>}"
OUT="${2:?usage: run.sh <model-tag> <out-dir>}"
HERE="$(cd "$(dirname "$0")" && pwd)"

case "$MODEL" in
  *:cloud) echo "REFUSED: '$MODEL' is a cloud tag; this stone measures LOCAL only." >&2; exit 2;;
esac

if ! ollama list | awk '{print $1}' | grep -qx -- "$MODEL"; then
  echo "REFUSED: '$MODEL' is not resident in ollama list." >&2; exit 2
fi

SLUG="$(printf '%s' "$MODEL" | tr '/:.' '___')"
mkdir -p "$OUT"

ask () {   # ask <task-name> <prompt-file> <num_predict>
  local name="$1" pf="$2" npred="$3"
  python3 - "$MODEL" "$pf" "$npred" "$OUT/${SLUG}__${name}" <<'PY'
import json, subprocess, sys, time, os
model, pf, npred, stem = sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4]
prompt = open(pf).read()

def call(p, n):
    body = json.dumps({
        "model": model, "prompt": p, "stream": False,
        "options": {"temperature": 0, "seed": 44, "num_predict": n, "num_ctx": 8192},
    })
    t0 = time.monotonic()
    r = subprocess.run(["curl", "-sS", "--max-time", "3600",
                        "http://127.0.0.1:11434/api/generate",
                        "-H", "Content-Type: application/json", "-d", body],
                       capture_output=True, text=True)
    dt = time.monotonic() - t0
    return dt, r.stdout, r.returncode

# thawtax: the warm request. Timed, reported, and thrown away.
warm_dt, _, _ = call("hi", 1)

dt, out, rc = call(prompt, npred)
load = os.getloadavg()[0]

txt, meta = "", {}
try:
    j = json.loads(out)
    txt = j.get("response", "")
    meta = {k: j.get(k) for k in ("eval_count", "prompt_eval_count", "eval_duration",
                                  "prompt_eval_duration", "done_reason")}
except Exception as e:
    txt, meta = "", {"parse_error": str(e), "raw_head": out[:400]}

open(stem + ".txt", "w").write(txt)
open(stem + ".json", "w").write(json.dumps({
    "model": model, "task": os.path.basename(pf), "curl_rc": rc,
    "warm_seconds": round(warm_dt, 3), "seconds": round(dt, 3),
    "loadavg_1m": round(load, 2), "chars": len(txt),
    # edgedrop / zerobirth: an empty answer and a one-token loop are FAILED RUNS, not
    # wrong answers, and the grader must be able to tell them apart from a real attempt.
    "empty": len(txt.strip()) == 0,
    "degenerate": len(set(txt.split())) <= 2 and len(txt.split()) > 8,
    **meta}, indent=2))
print(f"  {os.path.basename(stem)}: {dt:.1f}s (warm {warm_dt:.1f}s) load {load:.1f} "
      f"chars {len(txt)} eval {meta.get('eval_count')}")
PY
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
