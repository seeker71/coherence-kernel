#!/usr/bin/env bash
# review-panel.sh — ask the installed reviewer CLIs to read an artifact and
# answer one question, in parallel, and collect their answers as files.
#
# The point is that outside review stops being an event and becomes a habit.
# Before this, "a review Grok and Gemini would find satisfactory" was a thing
# that had to be arranged; now it is one command.
#
#   tools/review-panel.sh <artifact-file> "<question>"
#   tools/review-panel.sh --panel grok,claude <artifact> "<question>"
#   tools/review-panel.sh --list
#
# Answers land in a timestamped directory under the scratch root and the paths
# are printed. Nothing is written into the repo by this script.
#
# ── THE DOORS, as measured 2026-07-28 on this host ────────────────────────
#   grok          grok -p "<prompt>"                          WORKS
#   claude        claude -p "<prompt>"                        WORKS
#   codex         codex exec --skip-git-repo-check "<prompt>" WORKS (needs >=0.145.0;
#                 v0.142.5 failed twice — a malformed ~/.codex/models_cache.json,
#                 then "model gpt-5.6-sol requires a newer Codex")
#   cursor        cursor-agent -p --trust "<prompt>"          WORKS (--trust, NOT
#                 --force/--yolo: the panel should read and opine, never execute)
#   gemini        gemini -p "<prompt>"                        DEAD — the CLI is
#                 refused with IneligibleTierError ("no longer supported for
#                 Gemini Code Assist for individuals"), it was replaced by
#                 Antigravity, and /Applications/Antigravity.app ships NO command
#                 line (no bin/, no agent binary anywhere in the bundle). There is
#                 no headless Gemini on this host until either GEMINI_API_KEY is
#                 set or Antigravity ships a CLI. Kept in the table as a door that
#                 is CLOSED rather than deleted, so its absence stays visible.
#
# ── ISOLATION ─────────────────────────────────────────────────────────────
# Each reviewer runs from an empty scratch directory holding only a copy of the
# artifact, never from the repo. The artifact's text is embedded in the prompt,
# so a reviewer needs no filesystem reach to do its job — and if one writes
# anyway, it writes into scratch. `--trust` is granted (so cursor does not block
# on a prompt) but `--force`/`--yolo` never is.

set -uo pipefail

SCRATCH_ROOT="${REVIEW_PANEL_ROOT:-${TMPDIR:-/tmp}/review-panel}"
DEFAULT_PANEL="grok,claude,codex,cursor"

usage() { sed -n '2,50p' "$0" | sed 's/^# \{0,1\}//'; exit "${1:-0}"; }

# --- door table: name -> how to invoke it with a prompt on argv -----------
run_door() {
    local name="$1" prompt="$2"
    case "$name" in
        grok)   grok -p "$prompt" ;;
        claude) claude -p "$prompt" ;;
        codex)  codex exec --skip-git-repo-check "$prompt" ;;
        cursor) cursor-agent -p --trust "$prompt" ;;
        gemini) echo "DOOR CLOSED: gemini CLI is tier-refused and Antigravity ships no CLI." >&2; return 78 ;;
        *)      echo "unknown door: $name" >&2; return 64 ;;
    esac
}

door_available() {
    case "$1" in
        grok)   command -v grok         >/dev/null 2>&1 ;;
        claude) command -v claude       >/dev/null 2>&1 ;;
        codex)  command -v codex        >/dev/null 2>&1 ;;
        cursor) command -v cursor-agent >/dev/null 2>&1 ;;
        gemini) return 1 ;;
        *)      return 1 ;;
    esac
}

if [[ "${1:-}" == "--list" ]]; then
    printf '%-8s %-10s %s\n' DOOR STATE INVOCATION
    printf '%-8s %-10s %s\n' grok   "$(door_available grok   && echo open || echo missing)" 'grok -p "<prompt>"'
    printf '%-8s %-10s %s\n' claude "$(door_available claude && echo open || echo missing)" 'claude -p "<prompt>"'
    printf '%-8s %-10s %s\n' codex  "$(door_available codex  && echo open || echo missing)" 'codex exec --skip-git-repo-check "<prompt>"'
    printf '%-8s %-10s %s\n' cursor "$(door_available cursor && echo open || echo missing)" 'cursor-agent -p --trust "<prompt>"'
    printf '%-8s %-10s %s\n' gemini closed 'no headless door on this host (see header)'
    exit 0
fi

PANEL="$DEFAULT_PANEL"
if [[ "${1:-}" == "--panel" ]]; then PANEL="$2"; shift 2; fi
[[ $# -ge 2 ]] || usage 64

ARTIFACT="$1"; QUESTION="$2"
[[ -f "$ARTIFACT" ]] || { echo "no such artifact: $ARTIFACT" >&2; exit 66; }

stamp="$(date +%Y%m%dT%H%M%S)"
outdir="$SCRATCH_ROOT/$stamp"
mkdir -p "$outdir/work"
cp "$ARTIFACT" "$outdir/work/$(basename "$ARTIFACT")"

# One prompt, identical for every door, so the answers are comparable.
prompt_file="$outdir/prompt.txt"
{
    echo "$QUESTION"
    echo
    echo "Answer from the artifact below. Be specific and name what is wrong or"
    echo "unsupported; agreement is less useful than a located objection. If a"
    echo "claim is unsupported by the text, say which claim and why."
    echo
    echo "----- BEGIN ARTIFACT: $(basename "$ARTIFACT") -----"
    cat "$ARTIFACT"
    echo "----- END ARTIFACT -----"
} > "$prompt_file"
prompt="$(cat "$prompt_file")"

echo "artifact : $ARTIFACT" >&2
echo "panel    : $PANEL" >&2
echo "answers  : $outdir" >&2

pids=()
IFS=',' read -r -a doors <<< "$PANEL"
for d in "${doors[@]}"; do
    if ! door_available "$d"; then
        echo "SKIP $d (door not open)" >&2
        printf 'DOOR NOT OPEN\n' > "$outdir/$d.out"; echo 78 > "$outdir/$d.rc"
        continue
    fi
    (
        cd "$outdir/work" || exit 70
        run_door "$d" "$prompt" > "$outdir/$d.out" 2> "$outdir/$d.err" </dev/null
        echo $? > "$outdir/$d.rc"
    ) &
    pids+=($!)
done
for p in "${pids[@]:-}"; do [[ -n "$p" ]] && wait "$p"; done

echo >&2
printf '%-8s %-4s %-7s %s\n' DOOR RC BYTES FIRST-LINE
for d in "${doors[@]}"; do
    rc="$(cat "$outdir/$d.rc" 2>/dev/null || echo '?')"
    sz="$(wc -c < "$outdir/$d.out" 2>/dev/null | tr -d ' ')"
    first="$(grep -m1 -v '^[[:space:]]*$' "$outdir/$d.out" 2>/dev/null | cut -c1-72)"
    printf '%-8s %-4s %-7s %s\n' "$d" "$rc" "${sz:-0}" "$first"
done
echo
echo "full answers: $outdir/<door>.out"
