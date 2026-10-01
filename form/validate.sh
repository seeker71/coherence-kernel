#!/usr/bin/env bash
# validate.sh — every Form band answers on fkwu, the runtime. When a kernel
# source moved since origin/main (gate/kernel-change.bml), the sibling kernels
# (Go, Rust, TypeScript) run it too and every output must be identical: they
# validate the kernel change, they are not a runtime, and their speed is no
# goal. Otherwise each band answers its pin on fkwu alone and no sibling is
# built or started; FORM_VALIDATE_SIBLINGS=1 asks them anyway. Any divergence
# is a bug in one of them or a spec corner nobody documented — worth knowing.
#
# One input for every arm: a workload is a root naming its files, fkwu walks
# that root, and `fkwu --closure` writes the same closure as one plain-Form file
# the siblings read. fkwu alone resolves and lowers; the siblings only walk.
#
# Run from form/.
#   ./validate.sh            # validate all samples
#   ./validate.sh path.fk    # validate one file
#   ./validate.sh prelude.fk test.fk  # validate one workload
#   ./validate.sh --binary  # compile every workload, execute artifacts
#   ./validate.sh --binary prelude.fk test.fk  # compile once, execute artifact
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

# Keep package-manager advisory text out of sibling-kernel output comparison.
# The TypeScript arm may invoke npm/npx when tsx is not locally installed; an
# update notice on stdout makes identical kernel results look divergent.
export NO_UPDATE_NOTIFIER=1
export NPM_CONFIG_UPDATE_NOTIFIER=false
export npm_config_update_notifier=false

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

GO_DIR="form-kernel-go"
RS_DIR="form-kernel-rust"
TS_DIR="form-kernel-ts"
GO_BIN="$GO_DIR/bin-go"
RS_BIN="$RS_DIR/target/release/form-kernel-rust"

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

# --- build compiled sibling kernels if stale -----------------------------
build_go() {
    if [[ ! -x "$GO_BIN" ]] || find "$GO_DIR" -name '*.go' -newer "$GO_BIN" -print -quit | grep -q .; then
        echo "  building go kernel..." >&2
        (cd "$GO_DIR" && GOPROXY=off go build -o bin-go .)
    fi
}
build_rs() {
    if [[ ! -x "$RS_BIN" ]] || find "$RS_DIR/src" -name '*.rs' -newer "$RS_BIN" -print -quit | grep -q .; then
        echo "  building rust kernel..." >&2
        (cd "$RS_DIR" && cargo build --release --offline --quiet)
    fi
}

build_ts() {
    # Bundle the TS kernel once (esbuild, cached by source mtimes) so each band
    # runs via plain `node` (~60ms) instead of npx tsx (~1.5s). With 455 bands
    # that is the difference between seconds and 11+ minutes of startup tax.
    local bundle="$TS_DIR/dist/main.mjs"
    local stale=0
    if [[ ! -f "$bundle" ]]; then stale=1; else
        local f
        for f in "$TS_DIR"/src/*.ts; do
            [[ "$f" -nt "$bundle" ]] && { stale=1; break; }
        done
    fi
    if [[ "$stale" == "1" ]]; then
        if [[ -x "$TS_DIR/node_modules/.bin/esbuild" ]]; then
            echo "  bundling ts kernel..." >&2
            "$TS_DIR/node_modules/.bin/esbuild" "$TS_DIR/src/main.ts" --bundle --platform=node \
                --format=esm --outfile="$bundle" --log-level=warning >&2
        fi
    fi
}

# The sibling kernels validate a kernel change. gate/kernel-change.bml answers
# whether a kernel source moved since origin/main; when none did, no sibling is
# built or started and every band runs on fkwu against its pin (run_fkwu_lane).
# FORM_VALIDATE_SIBLINGS=1 asks the siblings anyway; --binary always does.
SIBLINGS=1
if [[ "${FORM_VALIDATE_SIBLINGS:-0}" != 1 && "${1:-}" != "--binary" ]]; then
    kernel_change="$(cd .. && ./fkwu gate/kernel-change-run.fk 2>/dev/null)" || kernel_change=""
    if [[ -n "$kernel_change" ]]; then printf '%s\n' "$kernel_change" | sed -e '$d' -e '/^$/d' -e 's/^/  /'; fi
    if [[ "${kernel_change##*$'\n'}" == "0" ]]; then SIBLINGS=0; fi
fi
if [[ $SIBLINGS -eq 1 ]]; then
    build_go &
    build_rs &
    build_ts &
    wait
fi

# The runtime (repo-root fkwu, runtime/fkwu-uni.c) is the one reader of every
# workload: it resolves and lowers the closure, walks it as the fourth arm and
# the fkwu lanes, and writes it as plain Form for the siblings.
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

# A band may declare its proof level in its comment head:
#   ; PROOF LEVEL: FOURTH-ARM ONLY ...   → runs on the runtime fkwu (the source door),
#     compared against the first "Verdict <n>" its head declares. Loud
#     pass/fail — a wrong home-arm answer is a real failure, never skipped.
#   ; PROOF LEVEL: FKWU-STAGED ...       → needs a host carrier. A band that
#     names it (`; STAGED CARRIER: <path from the repo root>`) runs on the
#     fkwu-only lane, verdict and diagnostics held as above, whenever that
#     carrier stands; otherwise it is reported ⧗ pending — visible every run,
#     never green.
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
fk_band_declared_verdict() {
    awk '/^;/ { if (match($0, /Verdict [0-9]+/)) { print substr($0, RSTART + 8, RLENGTH - 8); exit } } !/^;/ && NF { exit }' "$1" 2>/dev/null
}
# A diagnostic is an error line on fkwu's stderr, or its word that the cached image it ran was
# compiled with errors: a warm cache answers the verdict, and must not answer it clean.
fk_diag_count() {
    grep -c 'unresolved-call\|error:\|compiled with errors' "$1" 2>/dev/null || true
}
# A voiced organ-health row carries its speaker's identity: its id and the id of the
# reading it cares for (care_of), its flow (the pid), its clock stamps and a care supply's
# duration, and any pid in its evidence. The kernels agree on what an organ said, never on
# which process said it, so the legs compare the reading and not the speaker.
organ_steady() {
    sed -E '/^form-organ health \{/{s/"id":"[^"]*",//;s/"flow":"[^"]*",//;s/,"observed_at_ms":[0-9]+//;s/,"at_ms":[0-9]+//;s/,"care_of":"[^"]*"//g;s/,"supply_elapsed_ms":[0-9]+//g;s/"pid":[0-9]+,?//g;}' "$1" 2>/dev/null || true
}

# The fourth sibling is the repo-root fkwu source/JIT door. It walks the
# workload root directly; hot CPU/Metal/MLX recipes may crystallize on demand.
# Held absolute: it runs from the repo root.
FORM_FOURTH_SOURCE_FKWU="${FORM_FOURTH_SOURCE_FKWU:-$FKWU_SRC}"
case "$FORM_FOURTH_SOURCE_FKWU" in
    ""|/*|[A-Za-z]:*) ;;
    *) FORM_FOURTH_SOURCE_FKWU="$PWD/$FORM_FOURTH_SOURCE_FKWU" ;;
esac
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
build_fourth

# AXIOM-4: "passage not through the offered interface is breach, and breach is
# observable." fkwu writes the closure every sibling reads, so without it no arm
# has an input: the run refuses here rather than fail every band one by one.
if ! fourth_available; then
    echo "validate.sh: the runtime fkwu is ABSENT — it walks every workload and writes the" >&2
    echo "  closure the siblings read, so no arm can run. See the reason build_fourth printed above." >&2
    exit 1
fi

# The TS kernel carries its deep Form recursion on a worker thread whose V8
# limit MATCHES its real stack (main.ts deep-stack door; FORM_KERNEL_STACK_MB
# passes through the environment, same name as the emitted C walker's door).
# Never re-add a node --stack_size flag here: it cannot grow the fixed OS
# main-thread stack, it only lifts V8's overflow check past it — turning deep
# recursion into a SILENT SIGSEGV with zero output (the aphonia family).
run_ts() {
    local bundle="$TS_DIR/dist/main.mjs"
    local loader="$PWD/$TS_DIR/node_modules/tsx/dist/loader.mjs"
    local current=1 f
    if [[ ! -f "$bundle" ]]; then
        current=0
    else
        for f in "$TS_DIR"/src/*.ts; do
            [[ "$f" -nt "$bundle" ]] && { current=0; break; }
        done
    fi
    if [[ "$current" == "1" ]]; then
        node "$bundle" "$@"
    elif node --experimental-strip-types --version >/dev/null 2>&1; then
        node --experimental-strip-types "$TS_DIR/src/main.ts" "$@"
    elif [[ -x "$TS_DIR/node_modules/.bin/tsx" ]]; then
        node --import "$loader" "$TS_DIR/src/main.ts" "$@"
    else
        echo "validate.sh: TypeScript arm needs Node strip-types or a local tsx install" >&2
        return 1
    fi
}

WORKLOAD_DIR="form-stdlib/.cache/workloads"
# Legs dirs live at the repo root's .hearth; a passing band removes its own.
HEARTH="${PWD%/*}/.hearth"

# fk_workload_root FILE... — sets workload_root to the workload's root: one `; preludes:` line
# naming its files in order, each spelled from the repo root, so fkwu meets every unit under
# the one spelling its .lowfk memos carry. Keyed by its text and written once, so fkwu's .fkb
# identity check beside it reuses the image across runs and rebuilds when a closure file moves.
fk_workload_root() {
    local text="; preludes:" f stem key tmp
    for f in "$@"; do
        f="${f#./}"
        case "$f" in
            /*|[A-Za-z]:*) ;;
            ../*) f="${f#../}" ;;
            *) f="form/$f" ;;
        esac
        text="$text $f"
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
}

# fk_workload_closure LEGS LABEL — fkwu writes the root's closure to LEGS/closure.fk from the
# repo root. A refusal prints the door's own words and the band fails: no sibling reads anything
# fkwu did not hand over.
fk_workload_closure() {
    local legs="$1" label="$2" rc=0 line
    ( cd .. && "$FOURTH_SOURCE_FKWU" --closure "form/$workload_root" "$legs/closure.fk" ) \
        > "$legs/closure.out" 2> "$legs/closure.err" || rc=$?
    [[ $rc -eq 0 && -s "$legs/closure.fk" ]] && return 0
    printf "  ✗  %-30s  fkwu --closure refused form/%s (exit %s)\n" "$label" "$workload_root" "$rc"
    while IFS= read -r line; do printf '      %s\n' "$line"; done < <(head -n 20 "$legs/closure.err")
    printf '      evidence=%s\n' "$legs"
    fail=$((fail + 1))
    if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
    return 1
}

# fk_leg_closure LEGS LEG — a per-leg copy of a closure that names "/tmp/, rooted in the leg's
# own TMPDIR. Only the lines that name it are rewritten: bash 3.2 expands a whole multi-MB text
# in quadratic time.
fk_leg_closure() {
    local src="$1/closure.fk" out="$1/closure-$2.fk" to="\"$1/tmp-$2/" hit n line prev=0
    : > "$out"
    while IFS= read -r hit; do
        n="${hit%%:*}"
        line="${hit#*:}"
        if [[ $n -gt $((prev + 1)) ]]; then
            head -n "$((n - 1))" "$src" | tail -n "+$((prev + 1))" >> "$out"
        fi
        printf '%s\n' "${line//\"\/tmp\//$to}" >> "$out"
        prev="$n"
    done < <(grep -n -F '"/tmp/' "$src")
    tail -n "+$((prev + 1))" "$src" >> "$out"
}

# fk_run_leg LEGS LEG COMMAND... — one sibling over the closure, its streams and exit in LEGS.
# Each leg owns its TMPDIR: bands reach scratch space through the `temp_dir` native and by
# literal "/tmp/ paths, so concurrent legs and concurrent runs never share a scratch path.
fk_run_leg() (
    set +e
    legs="$1" leg="$2" input="$1/closure.fk"
    shift 2
    mkdir -p "$legs/tmp-$leg"
    if grep -q -F '"/tmp/' "$input"; then
        fk_leg_closure "$legs" "$leg"
        input="$legs/closure-$leg.fk"
    fi
    TMPDIR="$legs/tmp-$leg" "$@" "$input" > "$legs/$leg" 2> "$legs/$leg.err"
    printf '%s\n' "$?" > "$legs/$leg.rc"
)

binary_mode=0
if [[ "${1:-}" == "--binary" ]]; then
    binary_mode=1
    shift
fi

# --- run_siblings: feed one Form workload through all kernels, compare ---
# A workload is one or more files (e.g. core.fk then a band). Every sibling
# reads the one closure fkwu hands over; a manifest-covered band also runs its
# root on fkwu, the fourth leg.
run_siblings() {
    local label="$1"; shift
    local go_out rs_out ts_out go_rc rs_rc ts_rc legs
    # A nonzero exit or source diagnostic is a failed fourth witness even when
    # the last printed scalar happens to match (verdict-parity numbness).
    local fourth_stem fk_out="" fk_rc=0 fk_diags=0 reg_want="" reg_have
    fourth_stem="$(fourth_band_stem "${*: -1}" || true)"
    fk_workload_root "$@"
    mkdir -p "$HEARTH"
    legs="$(mktemp -d "$HEARTH/validation-legs.XXXXXX")"
    fk_workload_closure "$legs" "$label" || return 0
    # The legs run CONCURRENTLY: a band's wall time is max(leg), not sum. Compare
    # result stdout. Stderr is a distinct diagnostic channel: timing and resource
    # receipts belong to each physical carrier.
    fk_run_leg "$legs" go "$GO_BIN" &
    fk_run_leg "$legs" rs "$RS_BIN" &
    fk_run_leg "$legs" ts run_ts &
    if [[ -n "$fourth_stem" ]]; then
        (
            set +e
            mkdir -p "$legs/tmp-fk"
            cd .. && TMPDIR="$legs/tmp-fk" "$FOURTH_SOURCE_FKWU" "form/$workload_root" > "$legs/fk" 2> "$legs/fk.err"
            printf '%s\n' "$?" > "$legs/fk.rc"
        ) &
    fi
    wait
    go_out=$(organ_steady "$legs/go"); rs_out=$(organ_steady "$legs/rs"); ts_out=$(organ_steady "$legs/ts")
    go_rc=$(cat "$legs/go.rc" 2>/dev/null || echo 1)
    rs_rc=$(cat "$legs/rs.rc" 2>/dev/null || echo 1)
    ts_rc=$(cat "$legs/ts.rc" 2>/dev/null || echo 1)
    if [[ -n "$fourth_stem" ]]; then
        fk_out=$(organ_steady "$legs/fk")
        fk_rc=$(cat "$legs/fk.rc" 2>/dev/null || echo 1)
        fk_diags=$(fk_diag_count "$legs/fk.err")
        # REGISTERED-VERDICT GATE. fourth-arm-bands.txt column 3 is the band's
        # registered verdict. The four arms agreeing proves agreement and
        # nothing more, so an agreed verdict that differs from the registered
        # one is a failure with its own word: a band cannot change what it
        # certifies without the change being seen. The verdict compared is the
        # LAST line of the agreed output, and only when the column is numeric.
        reg_want="$(awk -v b="$fourth_stem" '!/^#/ && $1==b{print $3; exit}' "$FOURTH_MANIFEST")"
    fi
    if [[ "$go_rc" == 0 && "$rs_rc" == 0 && "$ts_rc" == 0 && "$go_out" == "$rs_out" && "$go_out" == "$ts_out" ]] \
        && { [[ -z "$fourth_stem" ]] || { [[ "$fk_rc" == 0 && "$fk_diags" == 0 && "$fk_out" == "$go_out" ]]; }; }; then
        reg_have="${go_out##*$'\n'}"
        if [[ "$reg_want" =~ ^[0-9]+$ && "$reg_have" != "$reg_want" ]]; then
            printf "  ✗  %-30s  → %s agreed on every arm, but the manifest registers %s — REGISTERED-VERDICT DRIFT\n      evidence=%s\n" \
                "$label" "$reg_have" "$reg_want" "$legs"
            fail=$((fail + 1))
            if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
            return 0
        fi
        local head_pin
        head_pin="$(fk_band_declared_verdict "${*: -1}")"
        if [[ -n "$head_pin" && "$reg_have" != "$head_pin" ]]; then
            printf "  ✗  %-30s  → %s agreed on every arm, but its head pins Verdict %s — DECLARED-VERDICT DRIFT\n      evidence=%s\n" \
                "$label" "$reg_have" "$head_pin" "$legs"
            fail=$((fail + 1))
            if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
            return 0
        fi
        rm -rf "$legs"
        printf "  ✓  %-30s  → %s\n" "$label" "$go_out"
        ok=$((ok + 1))
        if [[ -n "$fourth_stem" ]]; then fourth_ok=$((fourth_ok + 1)); fi
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then
            if [[ -n "$fourth_stem" ]]; then echo "ok fourth" > "$SUITE_STATUS_FILE"; else echo "ok" > "$SUITE_STATUS_FILE"; fi
        fi
        return 0
    fi
    printf '  evidence=%s exits go=%s rust=%s typescript=%s\n' "$legs" "$go_rc" "$rs_rc" "$ts_rc"
    if [[ -n "$fourth_stem" ]]; then
        printf "  ✗  %-30s\n      go         = %s\n      rust       = %s\n      typescript = %s\n      fourth-src = %s\n      fourth-rc  = %s  diagnostics=%s\n" \
            "$label" "$go_out" "$rs_out" "$ts_out" "$fk_out" "$fk_rc" "$fk_diags"
    else
        printf "  ✗  %-30s\n      go         = %s\n      rust       = %s\n      typescript = %s\n" \
            "$label" "$go_out" "$rs_out" "$ts_out"
    fi
    fail=$((fail + 1))
    if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
}

# --- run_siblings_binary: Go emits the closure as one binary artifact, and
# all three siblings execute that artifact.
run_siblings_binary() {
    local label="$1"; shift
    local go_out rs_out ts_out go_rc rs_rc ts_rc legs
    fk_workload_root "$@"
    mkdir -p "$HEARTH"
    legs="$(mktemp -d "$HEARTH/validation-binary.XXXXXX")"
    fk_workload_closure "$legs" "$label" || return 0
    if ! "$GO_BIN" --emit-binary "$legs/artifact" "$legs/closure.fk" > "$legs/emit" 2> "$legs/emit.err"; then
        printf "  ✗  %-30s  go --emit-binary refused the closure\n      evidence=%s\n" "$label" "$legs"
        fail=$((fail + 1))
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
        return 0
    fi
    ( set +e; "$GO_BIN" --binary "$legs/artifact" > "$legs/go" 2> "$legs/go.err"; printf '%s\n' "$?" > "$legs/go.rc" ) &
    ( set +e; "$RS_BIN" --binary "$legs/artifact" > "$legs/rs" 2> "$legs/rs.err"; printf '%s\n' "$?" > "$legs/rs.rc" ) &
    ( set +e; run_ts --binary "$legs/artifact" > "$legs/ts" 2> "$legs/ts.err"; printf '%s\n' "$?" > "$legs/ts.rc" ) &
    wait
    go_out=$(organ_steady "$legs/go"); rs_out=$(organ_steady "$legs/rs"); ts_out=$(organ_steady "$legs/ts")
    go_rc=$(cat "$legs/go.rc" 2>/dev/null || echo 1)
    rs_rc=$(cat "$legs/rs.rc" 2>/dev/null || echo 1)
    ts_rc=$(cat "$legs/ts.rc" 2>/dev/null || echo 1)
    if [[ "$go_rc" == 0 && "$rs_rc" == 0 && "$ts_rc" == 0 && "$go_out" == "$rs_out" && "$go_out" == "$ts_out" ]]; then
        rm -rf "$legs"
        printf "  ✓  %-30s  → %s\n" "$label" "$go_out"
        ok=$((ok + 1))
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "ok" > "$SUITE_STATUS_FILE"; fi
    else
        printf '  evidence=%s exits go=%s rust=%s typescript=%s\n' "$legs" "$go_rc" "$rs_rc" "$ts_rc"
        printf "  ✗  %-30s\n      go         = %s\n      rust       = %s\n      typescript = %s\n" \
            "$label" "$go_out" "$rs_out" "$ts_out"
        fail=$((fail + 1))
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
    fi
}

# --- run_fkwu_lane: no kernel source moved, so no sibling is asked ---------
# fkwu walks the workload root, and the band's last line answers its pins: the
# Verdict its head declares and its fourth-arm-bands.txt row, whichever it
# carries. A band with neither runs clean or fails; its answer is shown, not
# judged. A nonzero exit or a diagnostic on stderr fails the band as it fails
# the fourth leg, and a failure keeps its streams.
run_fkwu_lane() {
    local label="$1"; shift
    local band="${*: -1}" legs rc diags answered head_pin reg_pin stem why=""
    fk_workload_root "$@"
    mkdir -p "$HEARTH"
    legs="$(mktemp -d "$HEARTH/validation-legs.XXXXXX")"
    ( set +e; cd .. && TMPDIR="$legs" "$FOURTH_SOURCE_FKWU" "form/$workload_root" > "$legs/fk" 2> "$legs/fk.err"; printf '%s\n' "$?" > "$legs/fk.rc" )
    rc="$(cat "$legs/fk.rc" 2>/dev/null || echo 1)"
    diags="$(fk_diag_count "$legs/fk.err")"
    answered="$(organ_steady "$legs/fk")"
    answered="${answered##*$'\n'}"
    head_pin="$(fk_band_declared_verdict "$band")"
    stem="$(fourth_band_stem "$band" || true)"
    reg_pin=""
    if [[ -n "$stem" ]]; then reg_pin="$(awk -v b="$stem" '!/^#/ && $1==b{print $3; exit}' "$FOURTH_MANIFEST")"; fi
    [[ "$reg_pin" =~ ^[0-9]+$ ]] || reg_pin=""
    if [[ "$rc" != 0 ]]; then why="exit $rc"
    elif [[ "${diags:-0}" -gt 0 ]]; then why="$diags diagnostic line(s) on stderr"
    elif [[ -n "$head_pin" && "$answered" != "$head_pin" ]]; then why="answered ${answered:-<nothing>}, its head pins $head_pin"
    elif [[ -n "$reg_pin" && "$answered" != "$reg_pin" ]]; then why="answered ${answered:-<nothing>}, the manifest registers $reg_pin"
    fi
    if [[ -n "$why" ]]; then
        printf "  ✗  %-30s  fkwu lane: %s\n      evidence=%s\n" "$label" "$why" "$legs"
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
        fail=$((fail + 1))
        return
    fi
    rm -rf "$legs"
    if [[ -n "$head_pin" || -n "$reg_pin" ]]; then
        printf "  ✓  %-30s  → %s (fkwu, its pin)\n" "$label" "$answered"
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "ok fkwu-lane" > "$SUITE_STATUS_FILE"; fi
        ok=$((ok + 1)); fkwu_lane=$((fkwu_lane + 1))
    else
        printf "  ·  %-30s  → %s (fkwu, ran clean; no pin to answer)\n" "$label" "${answered:-<nothing>}"
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "ok unpinned" > "$SUITE_STATUS_FILE"; fi
        ok=$((ok + 1)); unpinned=$((unpinned + 1))
    fi
}

run_workload() {
    local label="$1"; shift
    if [[ $binary_mode -eq 0 ]]; then
        local band="${*: -1}" level declared answered
        level="$(fk_band_proof_level "$band")"
        if [[ "$level" == "FKWU-STAGED" ]]; then
            local carrier
            carrier="$(fk_band_staged_carrier "$band")"
            if [[ -n "$carrier" && -x "../$carrier" ]]; then
                level="FOURTH-ARM"
            fi
        fi
        if [[ "$level" == "FOURTH-ARM" ]]; then
            declared="$(fk_band_declared_verdict "$band")"
            if [[ -z "$declared" ]]; then
                printf "  ✗  %-30s  declares FOURTH-ARM ONLY but pins no Verdict in its head\n" "$label"
                if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
                fail=$((fail + 1))
                return
            fi
            # Verdict equality alone cannot witness: an image with numb
            # unresolved calls can answer the right number (verdict-parity
            # numbness — nothing==nothing stays green). Demand the verdict
            # AND zero axiom-5 diagnostics on stderr.
            local lane_out lane_diags
            lane_out="$(mktemp "${TMPDIR:-/tmp}/form-fkwu-lane.XXXXXX")"
            answered="$( (cd .. && "$FOURTH_SOURCE_FKWU" "form/$band") 2>"$lane_out" | tail -1 || true)"
            lane_diags="$(fk_diag_count "$lane_out")"
            rm -f "$lane_out"
            if [[ "${lane_diags:-0}" -gt 0 ]]; then
                printf "  ✗  %-30s  fkwu-only lane: %s diagnostic line(s) on stderr — verdict %s untrusted\n" \
                    "$label" "$lane_diags" "${answered:-<nothing>}"
                if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
                fail=$((fail + 1))
                return
            fi
            if [[ "$answered" == "$declared" ]]; then
                printf "  ✓  %-30s  → %s (fkwu-only lane)\n" "$label" "$answered"
                if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "ok fkwu-only" > "$SUITE_STATUS_FILE"; fi
                ok=$((ok + 1)); fkwu_only=$((fkwu_only + 1))
            else
                printf "  ✗  %-30s  fkwu-only lane: declared Verdict %s, fkwu answered %s\n" \
                    "$label" "$declared" "${answered:-<nothing>}"
                if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
                fail=$((fail + 1))
            fi
            return
        elif [[ "$level" == "FKWU-STAGED" ]]; then
            local door
            door="$(fk_band_staged_door "$band")"
            if [[ -n "$door" ]]; then
                printf "  ⧗  %-30s  staged fkwu lane — carrier absent in this checkout (%s builds it); pending, not witnessed\n" "$label" "$door"
            else
                printf "  ⧗  %-30s  staged fkwu lane — carrier absent in this checkout; pending, not witnessed\n" "$label"
            fi
            if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "staged" > "$SUITE_STATUS_FILE"; fi
            staged=$((staged + 1))
            return
        fi
        if [[ $SIBLINGS -eq 0 ]]; then
            run_fkwu_lane "$label" "$@"
            return
        fi
    fi
    if [[ $binary_mode -eq 1 ]]; then
        run_siblings_binary "binary/$label" "$@"
    else
        run_siblings "$label" "$@"
    fi
}

ok=0
fail=0
fourth_ok=0
fkwu_only=0
fkwu_lane=0
unpinned=0
staged=0

# --- explicit mode: validate one file list as one workload --------------
if [[ $# -gt 0 ]]; then
    # The root names exactly the files typed; fkwu resolves each one's closure.
    explicit_args=("$@")
    # A missing input file is not a kernel divergence: name the absent path
    # plainly (running a band by a shortened name when its file carries a
    # longer one is the usual cause) instead of failing the band.
    missing=()
    for f in "${explicit_args[@]}"; do
        [[ -f "$f" ]] || missing+=("$f")
    done
    if [[ ${#missing[@]} -gt 0 ]]; then
        printf "  ✗  input file(s) not found — this is a missing file, not a kernel divergence.\n" >&2
        printf "      kernel input paths resolve relative to the form/ directory (e.g. form-stdlib/core.fk):\n" >&2
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
    # counts from the status files. A band's legs were already concurrent;
    # this makes the bands themselves concurrent — the suite's wall time is
    # sum(bands)/jobs instead of sum(bands). The fan-out shares nothing it
    # writes: workload roots are content-keyed and land by mv, every band owns
    # its legs dir and every leg a private TMPDIR.
    SUITE_PAR="${VALIDATE_JOBS:-8}"
    suite_dir="$(mktemp -d "${TMPDIR:-/tmp}/form-suite.XXXXXX")"
    wl_labels=()
    wl_args=()
    add_workload() {
        local label="$1"; shift
        wl_labels+=("$label")
        local joined="" a
        for a in "$@"; do joined="$joined$a"$'\x1f'; done
        wl_args+=("$joined")
    }
    # --- form-samples/*.fk: self-contained files ------------------------
    for f in form-samples/*.fk; do
        add_workload "$(basename "$f")" "$f"
    done
    # --- form-stdlib/tests/*.{fk,form}: core.fk, then the band -----------
    # fkwu follows each band's own preludes and imports. A band named after a
    # module (tests/line-grammar.fk → line-grammar.fk) loads that module
    # between core.fk and the test.
    if [[ -d form-stdlib/tests ]]; then
        for f in form-stdlib/tests/*.fk form-stdlib/tests/*.form; do
            if [[ ! -e "$f" ]]; then
                continue
            fi
            base="$(basename "$f")"
            base="${base%.*}"
            module="form-stdlib/${base}.fk"
            if [[ -f "$module" && "$module" != "$f" ]]; then
                add_workload "stdlib/$(basename "$f")" "form-stdlib/core.fk" "$module" "$f"
            else
                add_workload "stdlib/$(basename "$f")" "form-stdlib/core.fk" "$f"
            fi
        done
    fi
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
            "ok fourth")    ok=$((ok + 1)); fourth_ok=$((fourth_ok + 1)) ;;
            "ok fkwu-only") ok=$((ok + 1)); fkwu_only=$((fkwu_only + 1)) ;;
            "ok fkwu-lane") ok=$((ok + 1)); fkwu_lane=$((fkwu_lane + 1)) ;;
            "ok unpinned")  ok=$((ok + 1)); unpinned=$((unpinned + 1)) ;;
            ok)             ok=$((ok + 1)) ;;
            staged)         staged=$((staged + 1)) ;;
            *)              fail=$((fail + 1)) ;;
        esac
        i=$((i + 1))
    done
    rm -rf "$suite_dir"
fi

echo ""
if [[ $SIBLINGS -eq 0 ]]; then
    echo "  sibling kernels: not asked — no kernel source moved (FORM_VALIDATE_SIBLINGS=1 asks them)"
    echo "  fkwu lane: $fkwu_lane band(s) answered their pins; $unpinned ran clean with no pin to answer"
elif [[ $binary_mode -eq 1 ]]; then
    echo "  binary mode: three siblings over Go's artifact of each closure; fkwu walks no root here"
elif [[ $fourth_ok -gt 0 ]]; then
    echo "  fourth arm: $fourth_ok band(s) four-way (runtime fkwu source/JIT)"
elif [[ $((ok - fkwu_only)) -gt 0 ]]; then
    # The zero the fourth_available refusal above cannot see. That gate asks
    # "is the arm present at all" and exits 1 when it is not. This asks the
    # different question: the arm is present, and
    # fired for NOT ONE band in the run — because coverage is per-band
    # (fourth-arm-bands.txt), so a workload naming only unregistered bands gets
    # a full set of ✓ marks with three kernels behind every one of them. The
    # zero is said out loud, never left to silence.
    echo "  fourth arm: 0 band(s) — present, but no band in this run is covered;"
    echo "              every ✓ above speaks for three kernels. Register the band in"
    echo "              fourth-arm-bands.txt. The source/JIT door itself is present."
fi
if [[ $fkwu_only -gt 0 ]]; then
    echo "  fkwu-only lanes: $fkwu_only band(s) at declared proof level (runtime fkwu)"
fi
if [[ $staged -gt 0 ]]; then
    echo "  staged lanes pending: $staged band(s) need an absent host carrier — not witnessed"
fi
if [[ $fail -eq 0 ]]; then
    if [[ $binary_mode -eq 1 ]]; then
        echo "  $ok ok, 0 divergent — kernels agree on every binary artifact."
    elif [[ $SIBLINGS -eq 0 ]]; then
        echo "  $ok ok, 0 failed — fkwu answered every pin it was asked."
    else
        echo "  $ok ok, 0 divergent — kernels agree on every sample."
    fi
    exit 0
else
    echo "  $ok ok, $fail failed or divergent — inspect exit statuses and retained streams."
    exit 1
fi
