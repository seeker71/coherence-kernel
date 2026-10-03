#!/usr/bin/env bash
# validate.sh — every Form band answers on fkwu, the one runtime. A band runs by
# its own path through the repo-root fkwu and is judged by its exit, its stderr,
# the first Verdict its head declares and its manifest row (band-verdicts.txt).
# Nothing else is built or started: the C seed and the optional Metal carrier are
# the only compilers this script may call, and only when they are stale.
#
# Run from form/.
#   ./validate.sh            # validate every band the body keeps (the sweep)
#   ./validate.sh --list     # name the sweep's workloads, run nothing
#   VALIDATE_MATCH='glass|landing' ./validate.sh  # the sweep's workloads whose label matches
#   ./validate.sh path.fk    # validate one file
#   ./validate.sh prelude.fk test.fk  # validate one workload
#   ./validate.sh --bench    # native Form JIT witness

set -euo pipefail
cd "$(dirname "$0")"
# A verdict never depends on what a terminal types: every kernel this script starts reads end of
# input. binary-freshness-band's EOF canary reads stdin, and an open tty or socket never ends.
exec < /dev/null

if [[ "${1:-}" == "--bench" ]]; then
    cd ..
    exec ./fkwu observe/native-jit-witness-run.fk
fi

BAND_MANIFEST="band-verdicts.txt"

# --- the sweep's workloads ---------------------------------------------------
# Every band the body keeps, enumerated in one place (`./validate.sh --list`
# prints each workload's label and files, and runs nothing):
#   form-samples/*.fk — each a self-contained file;
#   form-stdlib/tests/*.fk and *-band.bml — core.fk, then the band. fkwu follows
#     each band's own preludes and imports; a band named after a module
#     (tests/line-grammar.fk → line-grammar.fk) loads that module between. A
#     -child, -fixture or -run file is a door its band spawns or reads (the
#     string-op-stops children exit 1 by design): its band carries it, and it is
#     never swept alone;
#   every tracked or new *-band.fk / *-band.bml in another home's tests/
#     (observe, cognition, control, gate, grammars, learn, model, plugin,
#     form/native/metal) — run alone, by its own path, as its head says.
wl_labels=()
wl_args=()
# VALIDATE_MATCH=<extended regex> narrows the sweep (and --list) to the workloads whose label
# matches: one family through the same pool, lanes and deadlines as the whole sweep.
add_workload() {
    local label="$1"; shift
    if [[ -n "${VALIDATE_MATCH:-}" && ! "$label" =~ $VALIDATE_MATCH ]]; then return 0; fi
    wl_labels+=("$label")
    local joined="" a
    for a in "$@"; do joined="$joined$a"$'\x1f'; done
    wl_args+=("$joined")
}
suite_enumerate() {
    local f base module
    for f in form-samples/*.fk; do
        [[ -e "$f" ]] || continue
        add_workload "$(basename "$f")" "$f"
    done
    for f in form-stdlib/tests/*.fk form-stdlib/tests/*-band.bml; do
        [[ -e "$f" ]] || continue
        base="$(basename "$f")"
        base="${base%.*}"
        case "$base" in
            *-child|*-fixture|*-run) continue ;;
        esac
        module="form-stdlib/${base}.fk"
        if [[ -f "$module" && "$module" != "$f" ]]; then
            add_workload "stdlib/$(basename "$f")" "form-stdlib/core.fk" "$module" "$f"
        else
            add_workload "stdlib/$(basename "$f")" "form-stdlib/core.fk" "$f"
        fi
    done
    # a tracked band deleted in the working tree is skipped, never a stop: the enumeration
    # answers 0 whatever its last path was
    while IFS= read -r f; do
        [[ -f "../$f" ]] || continue
        add_workload "$f" "../$f"
    done < <(cd .. && git ls-files --cached --others --exclude-standard -- '*/tests/*-band.fk' '*/tests/*-band.bml' 2>/dev/null \
                 | grep -v '^form/form-stdlib/tests/' | grep -v '^\.' | sort -u)
    return 0
}

# A band may declare a staged carrier in its comment head:
#   ; PROOF LEVEL: FKWU-STAGED ...       → needs a host carrier. A band that
#     names it (`; STAGED CARRIER: <path from the repo root>`) runs whenever that
#     carrier stands; otherwise it is reported ⧗ pending — visible every run,
#     never green. Every other band runs.
fk_band_proof_level() {
    sed -n 's/^; PROOF LEVEL: \([A-Z-]*\).*/\1/p' "$1" 2>/dev/null | head -1
}
fk_band_staged_carrier() {
    sed -n 's/^; STAGED CARRIER: \([^ ]*\).*/\1/p' "$1" 2>/dev/null | head -1
}
# the door that builds a staged band's carrier (`; STAGED CARRIER DOOR: <command>`), named
# in the pending line so the next reader knows how to make the carrier present
fk_band_staged_door() {
    sed -n 's/^; STAGED CARRIER DOOR: \(.*\)$/\1/p' "$1" 2>/dev/null | head -1
}
# The head pin: the first `Verdict <n>` on a `; ` line of the band's comment head, which ends at
# its first code line. One spelling, so a head reads one way: `Verdict: 131 (P1) + 12*42 (P2) =
# 635` (form-eval-full-band) is prose about a sum, and a reader that took the colon spelling too
# would pin it 131 against its registered 635.
fk_band_declared_verdict() {
    awk '/^;/ { if (match($0, /Verdict [0-9]+/)) { print substr($0, RSTART + 8, RLENGTH - 8); exit } } !/^;/ && NF { exit }' "$1" 2>/dev/null
}

# band_stem — manifest stem for a band file path, or empty. A registered verdict only applies to a
# CANONICAL band — the workload's last file living under form-stdlib/tests/. A same-named sample
# elsewhere (e.g. a cross-modal demo whose basename collides with a manifest stem) never resolves,
# because the registered band is form-stdlib/tests/<stem>-band.fk and the sample's own output
# would be compared against it. Anchoring to the tests/ path keeps the stem the contract for the
# real band only.
band_stem() {
    local band="$1" stem hit
    if [[ "$band" != form-stdlib/tests/* && "$band" != */form-stdlib/tests/* ]]; then
        return 0
    fi
    stem="$(basename "$band")"
    stem="${stem%.fk}"
    stem="${stem%.bml}"
    [[ -f "$BAND_MANIFEST" ]] || return 0
    # Exact name first, then the name without -band: a row may name the file
    # whole (tests/<stem>.fk) or the band it proves (tests/<stem>-band.fk).
    hit="$(awk -v b="$stem" '$1==b{print $1; exit}' "$BAND_MANIFEST")"
    if [[ -n "$hit" ]]; then printf '%s\n' "$hit"; return 0; fi
    stem="${stem%-band}"
    awk -v b="$stem" '$1==b{print $1; exit}' "$BAND_MANIFEST"
}

# `./validate.sh --list` names each workload, whether its head stages a carrier, its head pin and
# its manifest row. A band with neither pin is swept but judged by its exit alone.
if [[ "${1:-}" == "--list" ]]; then
    suite_enumerate
    i=0
    while [[ $i -lt ${#wl_labels[@]} ]]; do
        list_files="$(printf '%s' "${wl_args[$i]}" | tr '\037' ' ')"
        list_files="${list_files% }"
        list_band="${list_files##* }"
        list_level="$(fk_band_proof_level "$list_band")"
        list_pin="$(fk_band_declared_verdict "$list_band")"
        list_stem="$(band_stem "$list_band" || true)"
        list_row=""
        if [[ -n "$list_stem" ]]; then list_row="$(awk -v b="$list_stem" '!/^#/ && $1==b{print $3; exit}' "$BAND_MANIFEST")"; fi
        printf '%s\t%s\tstaged=%s\tpin=%s\trow=%s\n' "${wl_labels[$i]}" "$list_files" \
            "$([[ "$list_level" == "FKWU-STAGED" ]] && echo yes || echo no)" "${list_pin:--}" "${list_row:--}"
        i=$((i + 1))
    done
    exit 0
fi

# --- THE SEAL: a verdict belongs to the tree it was read from ---------------
# A run that is still going while the files under it change would print a
# number about a tree that no longer exists — green or red equally meaningless.
# The run stamps the tree (HEAD plus the digest of its working status) when it
# starts and again when it ends, and a stamp that moved voids the reading.
#
# A sweep here takes over an hour, so the window is wide and an edit inside it
# is ordinary, not careless. What must never be ordinary is READING the result
# afterward as though it said something. Exit 3 is a void reading — not a pass
# and not a failure, an answer about nothing. Re-run on the settled tree.
#
# RADIUS, stated rather than implied: this compares the tree at the start against
# the tree at the end. An edit that is made and undone entirely inside the window
# leaves both stamps equal and passes unseen. It catches the drift that persists,
# which is the drift that misleads a reader afterward. A per-file mtime watch
# would close the rest and is the next stone if this one is ever the reason
# something got through.
_validate_tree_generation() {
    printf '%s:%s\n' \
        "$(git rev-parse HEAD 2>/dev/null || echo no-git)" \
        "$(git status --porcelain=v1 2>/dev/null | shasum -a 256 | cut -d' ' -f1)"
}
VALIDATE_TREE_AT_START="$(_validate_tree_generation)"
_validate_seal() {
    local rc="${1:-$?}" now
    now="$(_validate_tree_generation)"
    if [[ "$now" != "$VALIDATE_TREE_AT_START" ]]; then
        echo "validate.sh: VOID READING — the tree moved while this run was in flight." >&2
        echo "              start $VALIDATE_TREE_AT_START" >&2
        echo "              end   $now" >&2
        echo "              This run's verdict describes a tree that is no longer here." >&2
        echo "              It is neither a pass nor a failure. Re-run on the settled tree." >&2
        exit 3
    fi
    exit $rc
}
trap _validate_seal EXIT

# Phase 0: the source tree cannot claim a shell/Python-free Form-owned body
# while an unclassified script sits outside every declared auxiliary boundary.
# fkwu's current failure values are typed Form results, not process status, so
# this existing validation carrier owns the one transparent conversion: native
# structural status 1 continues; 0 becomes this validator's nonzero refusal.
if STRUCTURAL_GATE_RESULT="$(cd .. && ./fkwu gate/structural-gate-run.fk)"; then
    STRUCTURAL_GATE_FKWU_RC=0
else
    STRUCTURAL_GATE_FKWU_RC=$?
fi
printf '%s\n' "$STRUCTURAL_GATE_RESULT"
STRUCTURAL_GATE_STATUS="${STRUCTURAL_GATE_RESULT##*$'\n'}"
if [[ "$STRUCTURAL_GATE_FKWU_RC" -ne 0 || "$STRUCTURAL_GATE_STATUS" != "1" ]]; then
    printf '%s\n' 'validate.sh: structural gate refused an unclassified .sh or .py file.' >&2
    exit 1
fi

# Form owns the phase-zero check list and its verdict. This carrier maps the
# observed native value and process status into the existing validation flow.
if native_result="$(cd .. && ./fkwu gate/validation-start-run.fk)"; then
    native_rc=0
else
    native_rc=$?
fi
printf '%s\n' "$native_result"
if [[ "$native_rc" -ne 0 || "${native_result##*$'\n'}" != "1" ]]; then
    printf '%s\n' 'validate.sh: native validation-start needs attention.' >&2
    exit 1
fi

# The cache key of the text on stdin.
form_hash16() {
    if command -v shasum >/dev/null 2>&1 && printf test | shasum >/dev/null 2>&1; then
        shasum | cut -c1-16
    elif command -v sha1sum >/dev/null 2>&1 && printf test | sha1sum >/dev/null 2>&1; then
        sha1sum | cut -c1-16
    elif command -v sha256sum >/dev/null 2>&1 && printf test | sha256sum >/dev/null 2>&1; then
        sha256sum | cut -c1-16
    elif command -v cksum >/dev/null 2>&1 && printf test | cksum >/dev/null 2>&1; then
        cksum | cut -c1-16
    else
        echo "validate.sh: need shasum, sha1sum, sha256sum, or cksum for cache keys" >&2
        return 1
    fi
}

# The runtime (repo-root fkwu, runtime/fkwu-uni.c) is the one reader of every workload.
FKWU_SRC=""
build_fkwu_src() {
    local src="../runtime/fkwu-uni.c" bin="../fkwu"
    local carrier_src="native/metal/fk-metal-carrier.m"
    local carrier_dylib="native/metal/fk-metal-carrier.dylib"
    [[ -f "$src" ]] || return 0
    if [[ ! -x "$bin" || "$src" -nt "$bin" ]]; then
        command -v cc >/dev/null 2>&1 || return 0
        echo "  building runtime fkwu (repo root, plain seed)..." >&2
        cc -O2 -o "$bin" "$src" || return 1
    fi
    if [[ "$(uname -s)" == "Darwin" && -f "$carrier_src" &&
          ( ! -f "$carrier_dylib" || "$carrier_src" -nt "$carrier_dylib" ) ]]; then
        command -v cc >/dev/null 2>&1 || return 0
        echo "  building dynamic Metal carrier..." >&2
        cc -O2 -dynamiclib -o "$carrier_dylib" "$carrier_src" \
            -framework Metal -framework Foundation -fobjc-arc || return 1
    fi
    [[ -x "$bin" ]] && FKWU_SRC="$bin"
}
build_fkwu_src || exit 1

# A diagnostic is an error line on fkwu's stderr, or its word that the cached image it ran was
# compiled with errors: a warm cache answers the verdict, and must not answer it clean.
fk_diag_count() {
    grep -c 'unresolved-call\|error:\|compiled with errors' "$1" 2>/dev/null || true
}

# The runtime, held absolute: it runs from the repo root.
FORM_SOURCE_FKWU="${FORM_SOURCE_FKWU:-$FKWU_SRC}"
case "$FORM_SOURCE_FKWU" in
    ""|/*|[A-Za-z]:*) ;;
    *) FORM_SOURCE_FKWU="$PWD/$FORM_SOURCE_FKWU" ;;
esac

# AXIOM-4: "passage not through the offered interface is breach, and breach is
# observable." Without the runtime no band has an input: the run refuses here
# rather than fail every band one by one.
if [[ -z "$FORM_SOURCE_FKWU" || ! -x "$FORM_SOURCE_FKWU" ]]; then
    echo "validate.sh: the runtime fkwu is ABSENT — it walks every workload, so no band can run." >&2
    echo "  Build it: cc -O2 -o fkwu runtime/fkwu-uni.c (from the repo root)." >&2
    exit 1
fi

WORKLOAD_DIR="form-stdlib/.cache/workloads"
# A workload's legs dir (its stream, exit, wall time and scratch) lives under the host's temp root
# while it runs, outside the checkout: a tree walk of the checkout (the structural gate's source
# inventory, a recursive grep) never meets a dir that vanishes under it or a fifo a leg left open.
# A passing workload removes its legs; a failing one moves them to the repo root's .hearth, and
# that path is the evidence it prints.
HEARTH="${PWD%/*}/.hearth"
LEGS_ROOT="${TMPDIR:-/tmp}"
LEGS_ROOT="${LEGS_ROOT%/}"

# Deadline. Every leg ends: a leg that outlives FORM_VALIDATE_LEG_CEILING_S (default 1800 s) is
# ended with its whole process tree, its band fails naming the band, its streams are kept, and
# the sweep moves on.
LEG_CEILING_S="${FORM_VALIDATE_LEG_CEILING_S:-1800}"

# fk_legs_new KIND — a fresh legs dir under the host's temp root.
fk_legs_new() {
    mktemp -d "$LEGS_ROOT/validation-$1.XXXXXX"
}

# fk_legs_keep LEGS — a failing workload's legs move to .hearth; prints where they now stand.
fk_legs_keep() {
    local dest
    mkdir -p "$HEARTH"
    dest="$HEARTH/$(basename "$1")"
    if mv "$1" "$dest" 2>/dev/null; then printf '%s\n' "$dest"; else printf '%s\n' "$1"; fi
}

# fk_unit FILE — one workload file spelled from the repo root, where fkwu runs.
fk_unit() {
    local f="${1#./}"
    case "$f" in
        /*|[A-Za-z]:*) printf '%s\n' "$f" ;;
        ../*) printf '%s\n' "${f#../}" ;;
        *) printf 'form/%s\n' "$f" ;;
    esac
}

# fk_workload_root FILE... — sets workload_unit to what fkwu runs, spelled from the repo root.
# One file runs as itself, by the path its own head names (`./fkwu <band>`). Several files run
# through a root: one `; preludes:` line naming them in order, each spelled from the repo root, so
# fkwu meets every unit under the one spelling its .lowfk memos carry. The root is keyed by its
# text and written once, so fkwu's .fkb identity check beside it reuses the image across runs and
# rebuilds when a closure file moves.
fk_workload_root() {
    local text="; preludes:" f stem key tmp workload_root
    if [[ $# -eq 1 ]]; then
        workload_unit="$(fk_unit "$1")"
        return 0
    fi
    for f in "$@"; do
        text="$text $(fk_unit "$f")"
    done
    key="$(printf '%s\n' "$text" | form_hash16)" || return 1
    stem="$(basename "${*: -1}")"
    workload_root="$WORKLOAD_DIR/$key-${stem%.*}.fk"
    if [[ ! -s "$workload_root" ]]; then
        mkdir -p "$WORKLOAD_DIR"
        tmp="$(mktemp "$WORKLOAD_DIR/.$key.XXXXXX")"
        printf '%s\n' "$text" > "$tmp"
        mv -f "$tmp" "$workload_root"
    fi
    workload_unit="form/$workload_root"
}

# fk_release_fifos DIR [RECORD] — a leg's scratch is the band's to the end of the leg, and no
# longer. A band may leave a reader waiting in open() on a fifo there, for a writer it deferred
# (the cell-channel band's bell: `cat b.bell &`, then a ring through tools/ftimeout). Once the
# band has ended and its scratch is gone, that writer can no longer reach the fifo and the reader
# waits forever, an orphan of the run. So when a leg ends, every fifo in its scratch is opened
# read-write for an instant (that open never blocks, and a waiting reader reads end of file) and
# then removed, so nothing that walks the kept evidence later blocks in open() on it. RECORD, when
# given, names each removed fifo, so the evidence still says the band made it.
fk_release_fifos() (
    while IFS= read -r p; do
        { exec 9<>"$p"; } 2>/dev/null && exec 9>&-
        rm -f "$p"
        if [[ -n "${2:-}" ]]; then printf '%s\n' "$p" >> "$2"; fi
    done < <(find "$1" -type p 2>/dev/null)
    exit 0
)

# fk_run_leg LEGS DIR COMMAND... — the band's leg: COMMAND runs from DIR with its own TMPDIR, the
# scratch bands reach through the `temp_dir` native, a short private dir under the host's temp
# root (a band that opens a Unix socket there, a glass frame socket, needs a path inside the
# host's 104-byte socket limit), so concurrent legs never share a path. Its streams land in
# LEGS/fk and LEGS/fk.err, the pid the deadline ends in LEGS/fk.pid, its wall seconds in
# LEGS/fk.wall. When it ends its fifos are released and removed (LEGS/fk.fifos names them), its
# scratch moves into LEGS as tmp-fk, and its exit is written last, to LEGS/fk.rc: the watch reads
# that file as "this leg is over". It is always started with `&` (its brace body then runs in
# that job's own process); bash's word that a killed child died is kept off the streams, since
# the deadline file already says it.
fk_run_leg() {
    set +e
    local legs="$1" dir="$2" tmp began pid rc
    shift 2
    tmp="$(mktemp -d "$LEGS_ROOT/validation-fk.XXXXXX")" || tmp="$legs/tmp-fk"
    mkdir -p "$tmp"
    began=$SECONDS
    ( cd "$dir" && export TMPDIR="$tmp" && "$@" ) > "$legs/fk" 2> "$legs/fk.err" &
    pid=$!
    printf '%s\n' "$pid" > "$legs/fk.pid"
    wait "$pid" 2>/dev/null
    rc=$?
    printf '%s\n' "$((SECONDS - began))" > "$legs/fk.wall"
    fk_release_fifos "$tmp" "$legs/fk.fifos"
    if [[ "$tmp" != "$legs/tmp-fk" ]]; then mv "$tmp" "$legs/tmp-fk" 2>/dev/null || rm -rf "$tmp"; fi
    printf '%s\n' "$rc" > "$legs/fk.rc"
}

# fk_kill_tree PID — ends PID and every process under it. Each is stopped before its children
# are read, so none can fork past the reading, then killed.
fk_kill_tree() {
    # the suite's workers hold IFS at the unit separator; the pids pgrep prints split on newlines
    local IFS=$' \t\n' pid="$1" child
    kill -STOP "$pid" 2>/dev/null || return 0
    for child in $(pgrep -P "$pid" 2>/dev/null); do fk_kill_tree "$child"; done
    kill -KILL "$pid" 2>/dev/null || true
}

# fk_watch_leg LEGS — the deadline of the band's leg, run beside it. A leg past the ceiling gets
# LEGS/fk.deadline (what it outlived) and its process tree is ended; the leg then writes its exit
# as any leg does. The watch ends when the leg has written its exit, or its legs dir is gone. It
# is always started with `&`, and its body is a brace group, not a subshell: the pid `$!` names
# is then the watch itself, so fk_end_watch ends the watch and not a parent that would leave the
# loop running as an orphan.
fk_watch_leg() {
    set +e
    local legs="$1" began=$SECONDS nap=0.1 pid age
    while :; do
        [[ -d "$legs" ]] || exit 0
        [[ -f "$legs/fk.rc" ]] && exit 0
        age=$((SECONDS - began))
        if [[ $age -ge $LEG_CEILING_S && ! -f "$legs/fk.deadline" ]]; then
            pid="$(cat "$legs/fk.pid" 2>/dev/null || true)"
            if [[ -n "$pid" ]]; then
                printf 'ended after %ss: deadline %ss (the ceiling)\n' "$age" "$LEG_CEILING_S" > "$legs/fk.deadline"
                fk_kill_tree "$pid"
            fi
        fi
        if [[ $age -ge 3 ]]; then nap=1; fi
        sleep "$nap"
    done
}

# fk_end_watch PID — the leg has written its exit; the watch beside it ends.
fk_end_watch() {
    kill "$1" 2>/dev/null || true
    wait "$1" 2>/dev/null || true
}

# --- run_band: fkwu walks the workload, and the band's last line answers its pins ----------------
# The band's last line answers the Verdict its head declares and its band-verdicts.txt row,
# whichever it carries. A band with neither runs clean or fails; its answer is shown, not judged.
# A nonzero exit or a diagnostic on stderr fails the band even when the last line matches its pin
# (an answer printed before a stop is no verdict), and a failure keeps its streams. The leg is
# bounded by the ceiling (fk_watch_leg): a band that has not answered by then is ended and fails
# with its streams kept.
run_band() {
    local label="$1"; shift
    local band="${*: -1}" legs rc diags answered head_pin reg_pin stem why="" over fk_pid watch_pid
    fk_workload_root "$@"
    legs="$(fk_legs_new legs)"
    fk_run_leg "$legs" .. "$FORM_SOURCE_FKWU" "$workload_unit" &
    fk_pid=$!
    fk_watch_leg "$legs" &
    watch_pid=$!
    wait "$fk_pid" || true
    fk_end_watch "$watch_pid"
    over=""
    [[ -f "$legs/fk.deadline" ]] && over="fkwu"
    rc="$(cat "$legs/fk.rc" 2>/dev/null || echo 1)"
    diags="$(fk_diag_count "$legs/fk.err")"
    answered="$(cat "$legs/fk" 2>/dev/null || true)"
    answered="${answered##*$'\n'}"
    head_pin="$(fk_band_declared_verdict "$band")"
    stem="$(band_stem "$band" || true)"
    reg_pin=""
    if [[ -n "$stem" ]]; then reg_pin="$(awk -v b="$stem" '!/^#/ && $1==b{print $3; exit}' "$BAND_MANIFEST")"; fi
    [[ "$reg_pin" =~ ^[0-9]+$ ]] || reg_pin=""
    if [[ -n "$over" ]]; then why="ended at the deadline: $(cat "$legs/fk.deadline" 2>/dev/null)"
    elif [[ "$rc" != 0 ]]; then why="exit $rc"
    elif [[ "${diags:-0}" -gt 0 ]]; then why="$diags diagnostic line(s) on stderr"
    elif [[ -n "$head_pin" && "$answered" != "$head_pin" ]]; then why="answered ${answered:-<nothing>}, its head pins $head_pin"
    elif [[ -n "$reg_pin" && "$answered" != "$reg_pin" ]]; then why="answered ${answered:-<nothing>}, the manifest registers $reg_pin"
    fi
    if [[ -n "$why" ]]; then
        legs="$(fk_legs_keep "$legs")"
        printf "  ✗  %-30s  fkwu: %s\n      evidence=%s\n" "$label" "$why" "$legs"
        if [[ -n "$over" ]]; then
            deadlines="${deadlines:+$deadlines }$label"
            if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail deadline $label" > "$SUITE_STATUS_FILE"; fi
        elif [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
        fail=$((fail + 1))
        return
    fi
    rm -rf "$legs"
    if [[ -n "$head_pin" || -n "$reg_pin" ]]; then
        printf "  ✓  %-30s  → %s (its pin)\n" "$label" "$answered"
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "ok pinned" > "$SUITE_STATUS_FILE"; fi
        ok=$((ok + 1)); pinned=$((pinned + 1))
    else
        printf "  ·  %-30s  → %s (ran clean; no pin to answer)\n" "$label" "${answered:-<nothing>}"
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "ok unpinned" > "$SUITE_STATUS_FILE"; fi
        ok=$((ok + 1)); unpinned=$((unpinned + 1))
    fi
}

run_workload() {
    local label="$1"; shift
    local band="${*: -1}" level
    level="$(fk_band_proof_level "$band")"
    if [[ "$level" == "FKWU-STAGED" ]]; then
        local carrier door
        carrier="$(fk_band_staged_carrier "$band")"
        if [[ -z "$carrier" || ! -x "../$carrier" ]]; then
            door="$(fk_band_staged_door "$band")"
            if [[ -n "$door" ]]; then
                printf "  ⧗  %-30s  staged — carrier absent in this checkout (%s builds it); pending, not witnessed\n" "$label" "$door"
            else
                printf "  ⧗  %-30s  staged — carrier absent in this checkout; pending, not witnessed\n" "$label"
            fi
            if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "staged" > "$SUITE_STATUS_FILE"; fi
            staged=$((staged + 1))
            return
        fi
    fi
    run_band "$label" "$@"
}

ok=0
fail=0
pinned=0
unpinned=0
staged=0
deadlines=""

# --- explicit mode: validate one file list as one workload --------------
if [[ $# -gt 0 ]]; then
    # The root names exactly the files typed; fkwu resolves each one's closure.
    explicit_args=("$@")
    # A missing input file is not a failing band: name the absent path plainly (running a band
    # by a shortened name when its file carries a longer one is the usual cause).
    missing=()
    for f in "${explicit_args[@]}"; do
        [[ -f "$f" ]] || missing+=("$f")
    done
    if [[ ${#missing[@]} -gt 0 ]]; then
        printf "  ✗  input file(s) not found — this is a missing file, not a failing band.\n" >&2
        printf "      input paths resolve relative to the form/ directory (e.g. form-stdlib/core.fk):\n" >&2
        for f in "${missing[@]}"; do printf "        %s\n" "$f" >&2; done
        exit 2
    fi
    label=""
    for f in "${explicit_args[@]}"; do
        base="$(basename "$f")"
        if [[ -z "$label" ]]; then
            label="$base"
        else
            label="$label+$base"
        fi
    done
    run_workload "$label" "${explicit_args[@]}"
else
    # The suite fans out ACROSS bands: each workload is one job in a pool
    # (VALIDATE_JOBS wide, default 8), writing an ordered result block plus
    # a status file; the aggregation prints blocks in collection order and
    # counts from the status files. The suite's wall time is sum(bands)/jobs
    # instead of sum(bands). The fan-out shares nothing it writes: workload
    # roots are content-keyed and land by mv, every band owns its legs dir
    # and every leg a private TMPDIR.
    SUITE_PAR="${VALIDATE_JOBS:-8}"
    suite_dir="$(mktemp -d "${TMPDIR:-/tmp}/form-suite.XXXXXX")"
    suite_enumerate
    run_one_indexed() {
        local idx="$1"
        local IFS=$'\x1f'
        # shellcheck disable=SC2206
        local files=(${wl_args[$idx]})
        SUITE_STATUS_FILE="$suite_dir/$idx.status" \
            run_workload "${wl_labels[$idx]}" "${files[@]}" > "$suite_dir/$idx.out" 2>&1 || true
    }
    i=0
    total=${#wl_labels[@]}
    while [[ $i -lt $total ]]; do
        run_one_indexed "$i" &
        i=$((i + 1))
        while [[ "$(jobs -r | wc -l)" -ge "$SUITE_PAR" ]]; do sleep 0.2; done
    done
    wait
    i=0
    while [[ $i -lt $total ]]; do
        cat "$suite_dir/$i.out" 2>/dev/null || true
        case "$(cat "$suite_dir/$i.status" 2>/dev/null || echo fail)" in
            "ok pinned")    ok=$((ok + 1)); pinned=$((pinned + 1)) ;;
            "ok unpinned")  ok=$((ok + 1)); unpinned=$((unpinned + 1)) ;;
            staged)         staged=$((staged + 1)) ;;
            "fail deadline "*)
                fail=$((fail + 1))
                deadline_status="$(cat "$suite_dir/$i.status")"
                deadlines="${deadlines:+$deadlines }${deadline_status#fail deadline }" ;;
            *)              fail=$((fail + 1)) ;;
        esac
        i=$((i + 1))
    done
    rm -rf "$suite_dir"
fi

echo ""
echo "  fkwu: $pinned band(s) answered their pins; $unpinned ran clean with no pin to answer"
if [[ $staged -gt 0 ]]; then
    echo "  staged lanes pending: $staged band(s) need an absent host carrier — not witnessed"
fi
if [[ -n "$deadlines" ]]; then
    echo "  deadlines met: $deadlines — each leg named was ended with its process tree; its streams are kept"
fi
if [[ $fail -eq 0 ]]; then
    echo "  $ok ok, 0 failed — fkwu answered every pin it was asked."
    exit 0
else
    echo "  $ok ok, $fail failed — inspect exit statuses and retained streams."
    exit 1
fi
