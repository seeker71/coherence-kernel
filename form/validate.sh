#!/usr/bin/env bash
# validate.sh — sibling kernels run every Form source file;
# outputs must be identical. The kernels are siblings; they keep each
# other honest. Any divergence is a bug in one of them or a spec corner
# nobody documented — worth knowing.
#
# Run from form/.
#   ./validate.sh            # validate all samples
#   ./validate.sh path.fk    # validate one file
#   ./validate.sh prelude.fk test.fk  # validate one workload
#   ./validate.sh --binary  # compile every workload, execute artifacts
#   ./validate.sh --binary prelude.fk test.fk  # compile once, execute artifact
#   ./validate.sh --bench    # sibling bench suites, side-by-side

set -euo pipefail
cd "$(dirname "$0")"

# --- THE SEAL: a verdict belongs to the tree it was read from ---------------
# Laid 2026-08-17, from a run that was still going while the files under it
# changed three times. It would have printed a number, and the number would have
# been about a tree that no longer existed — green or red equally meaningless.
# Nothing caught that; it was noticed by hand, from a timestamp, and only
# because someone happened to wonder. Once is luck.
#
# This body already knew the shape: scripts/fourth-arm.sh seals its table to a
# generation and voids it when the sources move underneath ("table index
# generation changed"). That guard sat one level down from the run that needed
# it. This is the same guard at the top.
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
    local rc=$? now
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

# Keep package-manager advisory text out of sibling-kernel output comparison.
# The TypeScript arm may invoke npm/npx when tsx is not locally installed; an
# update notice on stdout makes identical kernel results look divergent.
export NO_UPDATE_NOTIFIER=1
export NPM_CONFIG_UPDATE_NOTIFIER=false
export npm_config_update_notifier=false

# Resolve a working Python 3: prefer `py -3` on Windows (a bare `python3` there
# resolves to the App-Execution-Alias stub that prints "Python was not found"),
# and verify the interpreter actually runs before using it.
BP_PY=""
if command -v py >/dev/null 2>&1 && py -3 --version >/dev/null 2>&1; then
    BP_PY="py -3"
elif command -v python3 >/dev/null 2>&1 && python3 --version >/dev/null 2>&1; then
    BP_PY="python3"
fi
# Phase 0 fkwu native surface gate (spec: fkwu-only-kernel-collapse.md).
if [[ -n "$BP_PY" && -f scripts/validate_fkwu_native_surface.py ]]; then
    $BP_PY scripts/validate_fkwu_native_surface.py
fi
if [[ -n "$BP_PY" && -f scripts/gen_flt_ops_from_manifest.py ]]; then
    $BP_PY scripts/gen_flt_ops_from_manifest.py
fi
if [[ -n "$BP_PY" && -f scripts/sync_native_op_manifest.py ]]; then
    $BP_PY scripts/sync_native_op_manifest.py
fi
if [[ -n "$BP_PY" && -f scripts/verify_category_contract.py ]]; then
    $BP_PY scripts/verify_category_contract.py
fi
# Registry drift gate: every Go native carries a registry row and the band's
# pinned counts match — the primitive-registry-band's declared verdict (63)
# is only honest while these numbers agree. Before this line the gate was a
# bell nobody rang: the band answered 42 three-way from birth (#231) and the
# agreement-only sibling comparison printed a green check over it.
if [[ -n "$BP_PY" && -f scripts/validate_primitive_registry.py ]]; then
    $BP_PY scripts/validate_primitive_registry.py --quiet
fi

GO_DIR="form-kernel-go"
RS_DIR="form-kernel-rust"
TS_DIR="form-kernel-ts"
GO_BIN="$GO_DIR/bin-go"
RS_BIN="$RS_DIR/target/release/form-kernel-rust"

form_hash16() {
    if command -v shasum >/dev/null 2>&1 && printf test | shasum >/dev/null 2>&1; then
        cat "$@" 2>/dev/null | shasum | cut -c1-16
    elif command -v sha1sum >/dev/null 2>&1 && printf test | sha1sum >/dev/null 2>&1; then
        cat "$@" 2>/dev/null | sha1sum | cut -c1-16
    elif command -v sha256sum >/dev/null 2>&1 && printf test | sha256sum >/dev/null 2>&1; then
        cat "$@" 2>/dev/null | sha256sum | cut -c1-16
    elif command -v cksum >/dev/null 2>&1 && printf test | cksum >/dev/null 2>&1; then
        cat "$@" 2>/dev/null | cksum | cut -c1-16
    else
        echo "validate.sh: need shasum, sha1sum, sha256sum, or cksum for cache keys" >&2
        return 1
    fi
}

# --- build compiled sibling kernels if stale -----------------------------
build_go() {
    if [[ ! -x "$GO_BIN" ]] || find "$GO_DIR" -name '*.go' -newer "$GO_BIN" -print -quit | grep -q .; then
        echo "  building go kernel..." >&2
        (cd "$GO_DIR" && go build -o bin-go .)
    fi
}
build_rs() {
    if [[ ! -x "$RS_BIN" || "$RS_DIR/src/main.rs" -nt "$RS_BIN" || "$RS_DIR/src/bp_table.rs" -nt "$RS_BIN" ]]; then
        echo "  building rust kernel..." >&2
        (cd "$RS_DIR" && cargo build --release --quiet)
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

build_go &
build_rs &
build_ts &
wait

# The runtime walker (repo-root fkwu, runtime/fkwu-uni.c) carries the
# resolver-driven the source door door that fkwu-only proof-level bands run on.
# Distinct from the emitted fourth-arm walker (bootstrap uni.c): that one
# walks pre-flattened tables; this one resolves `; preludes:` directives.
FKWU_SRC=""
build_fkwu_src() {
    local src="../runtime/fkwu-uni.c" bin="../fkwu"
    [[ -f "$src" ]] || return 0
    if [[ ! -x "$bin" || "$src" -nt "$bin" ]]; then
        command -v cc >/dev/null 2>&1 || return 0
        echo "  building runtime fkwu (repo root, door)..." >&2
        cc -O2 -o "$bin" "$src" 2>/dev/null || return 0
    fi
    [[ -x "$bin" ]] && FKWU_SRC="$bin"
}
build_fkwu_src

# ── FORM BALANCE, and the response to it ────────────────────────────────────
# Cells whose forms do not close were found four times in one week, each by
# accident, each only when something refused to run — and one of them had been
# that way since its only commit while its header claimed a verdict. A class
# found only by accident is a class mostly not found, so it is counted here
# every run. The count is not the deliverable: `observe/tree-heal.fk` repairs
# them, and it is safe to run unattended because it never trusts its own edit —
# a candidate closer is kept only when the kernel stops objecting, and reverted
# byte-for-byte otherwise.
#
#   echo '(do (write_file "/tmp/heal.txt" (th-report)) 1)' > /tmp/heal.fk   # see tree-heal.fk USE
#
fk_form_balance() {
    [[ -n "$FKWU_SRC" ]] || return 0
    local drv="${TMPDIR:-/tmp}/fk-balance-$$.fk"
    printf '; preludes: form-stdlib/core.fk observe/tree-balance.fk\n(do (int_to_str (tb-unbalanced-n)))\n' > "$drv"
    local n
    n="$( (cd .. && "$FKWU_SRC" "${drv}") 2>/dev/null | tail -1 )"
    rm -f "$drv" "${drv%.fk}.fkb" "${drv%.fk}.sym"
    if [[ "$n" == "0" ]]; then
        echo "  form balance: every cell closes"
    else
        echo "  form balance: ${n} cell(s) do not close — observe/tree-heal.fk repairs them (gated)"
    fi
}

# A band may declare its proof level in its comment head:
#   ; PROOF LEVEL: FOURTH-ARM ONLY ...   → runs on the runtime fkwu (the source door),
#     compared against the first "Verdict <n>" its head declares. Loud
#     pass/fail — a wrong home-arm answer is a real failure, never skipped.
#   ; PROOF LEVEL: FKWU-STAGED ...       → needs a host carrier staging bytes
#     into input_byte; the carrier did not travel in the CN→CK consolidation,
#     so the band is reported ⧗ pending — visible every run, never green.
fk_band_proof_level() {
    sed -n 's/^; PROOF LEVEL: \([A-Z-]*\).*/\1/p' "$1" 2>/dev/null | head -1
}
fk_band_declared_verdict() {
    awk '/^;/ { if (match($0, /Verdict [0-9]+/)) { print substr($0, RSTART + 8, RLENGTH - 8); exit } } !/^;/ && NF { exit }' "$1" 2>/dev/null
}

# The fourth sibling — the universal walker binary emitted from Form
# recipes — joins covered bands as a fourth leg. Built AFTER the Go kernel
# (its C source is emitted by running the Go walker); everything degrades
# honestly when clang or the manifest is absent. See scripts/fourth-arm.sh.
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
build_fourth

# AXIOM-4: "passage not through the offered interface is breach, and breach is
# observable." An absent fourth arm is a breach of the proof interface, and until
# 2026-07-22 it was NOT observable: build_fourth printed one line to stderr,
# returned with FKWU unset, and validate.sh went on to stamp ✓ on every band and
# to OMIT the "fourth arm: N four-way" summary entirely. A whole leg of the proof
# could vanish and the run still read as green.
#
# Witnessed on claude/deepseek-v4-flash-gguf-54a96c at 9f8a116e8, pristine
# checkout: both committed fourth-arm artifacts were stale against their own
# sources (binary stamp 695a9a0f39c157e6 vs wanted 52d0ef7b7c8a74cf; uni.c stamp
# 6670bf9df67a1e28 vs wanted 2c1d416f79add09a), so the arm never ran — 1284 ok,
# 41 divergent, zero four-way lines, and a checkmark on all of it. Regenerating
# the bootstrap turned the arm back on and seven bands immediately disagreed.
# They had not regressed; they had never been asked.
#
# fourth-arm.sh's own header already stated the law — "every declared fourth-arm
# workload is mandatory: preparation, execution, and agreement failures fail
# validation instead of silently reducing the proof to three siblings" — and the
# code did the opposite of the sentence written above it. This is that sentence,
# executed. FORM_ALLOW_THREE_ARM=1 is the one door out, for a host that genuinely
# cannot build fkwu (no clang); it must be asked for out loud, never assumed.
if ! fourth_available; then
    if [[ "${FORM_ALLOW_THREE_ARM:-0}" == 1 ]]; then
        echo "  fourth arm ABSENT — proceeding three-arm by explicit FORM_ALLOW_THREE_ARM=1" >&2
        echo "  every ✓ below speaks for three kernels, not four" >&2
    else
        echo "validate.sh: the fourth arm is ABSENT — refusing to report a three-arm run as green." >&2
        echo "  A ✓ here would mean 'three kernels agreed', not 'four kernels agreed', and nothing" >&2
        echo "  in the output would say which. See the reason build_fourth printed above." >&2
        echo "  Heal it (scripts/regen_fkwu_bootstrap.sh), or say so out loud: FORM_ALLOW_THREE_ARM=1" >&2
        exit 1
    fi
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

source_compile_dir="$(mktemp -d "${TMPDIR:-/tmp}/form-source.XXXXXX")"
mkdir -p form-stdlib/.cache
artifact=""
cleanup() {
    rm -rf "$source_compile_dir"
    if [[ -n "$artifact" ]]; then
        rm -f "$artifact"
    fi
}
trap cleanup EXIT

fk_declared_deps() {
    local file="$1"
    awk '
        function emit(tok) {
            gsub(/^[ \t,;"]+|[ \t,;"]+$/, "", tok)
            if (tok ~ /\.fk$/) print tok
        }
        /^;[ \t]*import([ \t:]|")/ {
            s = $0
            sub(/^;[ \t]*import[ \t:]*/, "", s)
            if (match(s, /"[^"]+\.fk"/)) {
                emit(substr(s, RSTART + 1, RLENGTH - 2))
            } else {
                n = split(s, a, /[ \t,;]+/)
                if (n >= 1) emit(a[1])
            }
        }
        /^[ \t]*import([ \t:]|")/ {
            s = $0
            sub(/^[ \t]*import[ \t:]*/, "", s)
            if (match(s, /"[^"]+\.fk"/)) {
                emit(substr(s, RSTART + 1, RLENGTH - 2))
            } else {
                n = split(s, a, /[ \t,;]+/)
                if (n >= 1) emit(a[1])
            }
        }
        /^;[ \t]*preludes:/ {
            s = $0
            sub(/^;[ \t]*preludes:[ \t]*/, "", s)
            gsub(/,/, " ", s)
            n = split(s, a, /[ \t]+/)
            for (i = 1; i <= n; i++) {
                low = tolower(a[i])
                if (a[i] == "\\" || low == "none" || low == "(none)") continue
                emit(a[i])
            }
        }
    ' "$file" 2>/dev/null || true
}

fk_resolve_dep_path() {
    local owner="$1"
    local token="$2"
    local dir cand
    case "$token" in
        /*|[A-Za-z]:*) printf "%s\n" "$token"; return ;;
    esac
    dir="$(dirname "$owner")"
    cand="$dir/$token"
    if [[ -f "$cand" ]]; then
        printf "%s\n" "$cand"
    elif [[ -f "$token" ]]; then
        printf "%s\n" "$token"
    elif [[ "$token" == form/* && -f "${token#form/}" ]]; then
        printf "%s\n" "${token#form/}"
    elif [[ -f "../$token" ]]; then
        # repo-root-anchored preludes (learn/…, observe/…) — the same door
        # the runtime resolver learned in #270; validate runs with cwd=form/.
        # Tried only after every form/-shaped rescue has failed, so no
        # currently-resolving token changes meaning. (Twice-found the same
        # night by independent lineages — the wound was that real.)
        printf "%s\n" "../$token"
    else
        printf "%s\n" "$cand"
    fi
}

fk_expand_seen=()
fk_expand_added=()
fk_import_expanded=()

fk_seen_contains() {
    local needle="$1" x
    [[ ${#fk_expand_seen[@]} -eq 0 ]] && return 1
    for x in "${fk_expand_seen[@]}"; do
        [[ "$x" == "$needle" ]] && return 0
    done
    return 1
}

fk_added_contains() {
    local needle="$1" x
    [[ ${#fk_expand_added[@]} -eq 0 ]] && return 1
    for x in "${fk_expand_added[@]}"; do
        [[ "$x" == "$needle" ]] && return 0
    done
    return 1
}

fk_add_expanded_dep() {
    local dep="$1"
    if ! fk_added_contains "$dep"; then
        fk_import_expanded+=("$dep")
        fk_expand_added+=("$dep")
    fi
}

fk_expand_file_deps() {
    local file="$1" token dep
    fk_seen_contains "$file" && return
    fk_expand_seen+=("$file")
    if [[ ! -f "$file" ]]; then
        echo "validate.sh: declared Form dependency not found: $file" >&2
        return 1
    fi
    while IFS= read -r token; do
        [[ -n "$token" ]] || continue
        dep="$(fk_resolve_dep_path "$file" "$token")"
        fk_expand_file_deps "$dep"
        fk_add_expanded_dep "$dep"
    done < <(fk_declared_deps "$file")
}

fk_expand_declared_deps() {
    fk_expand_seen=()
    fk_expand_added=()
    fk_import_expanded=()
    fk_expand_file_deps "$1"
}

# Source-compiled preludes are cached by CONTENT (file + compiler chain): the
# same unchanged core.fk compiles once, not once per band. Without this cache
# every validate invocation re-ran the full BML source-compiler (~12s) on
# identical input — 455 bands paid ~90 serial minutes for the same artifact.
SOURCE_CACHE_DIR="form-stdlib/.cache/source-compiled"
mkdir -p "$SOURCE_CACHE_DIR"
compiler_stamp=""
# The go lane's explicit closure. The walkers do not read `; preludes:` lines,
# so this list exists for them alone; the fkwu lane below needs no list at all.
# engine-constants / compiler-objects / form-ontology-bp joined 2026-08-18 when
# their births in Form broke this hand-held mirror silently for a day.
compiler_chain=("form-stdlib/engine-constants.fk" "form-stdlib/compiler-objects.fk" "form-stdlib/form-ontology-bp.fk" "form-stdlib/form-ontology-loader.fk" "form-stdlib/line-grammar.fk" "form-stdlib/bmf-core.fk" "form-stdlib/bmf-grammar.fk" "form-stdlib/bml.fk" "form-stdlib/bml-source.fk" "form-stdlib/source-compiler.fk" "form-stdlib/grammars/form-bml.fk" "form-stdlib/form-bml-lower.fk")
compiler_stamp="$(form_hash16 "${compiler_chain[@]}" "${FKWU_SRC:-}" "$GO_BIN")"

prepared_args=()
prepare_sources() {
    prepared_args=()
    local src out safe driver key cached
    for src in "$@"; do
        if grep -Eq '^[[:space:]]*section \[' "$src"; then
            key="$(form_hash16 "$src")-$compiler_stamp"
            cached="$SOURCE_CACHE_DIR/$key.fk"
            if [[ ! -s "$cached" ]]; then
                safe="${src//\//__}"
                # fkwu first: the driver names one cell and fkwu's resolver
                # walks the body's own preludes graph -- no chain list to drift.
                if [[ -n "${FKWU_SRC:-}" && -x "${FKWU_SRC:-}" ]]; then
                    out="$(mktemp "$SOURCE_CACHE_DIR/.${key}.XXXXXX")"
                    driver="$(mktemp "$source_compile_dir/compile-${safe}.XXXXXX.fk")"
                    {
                        printf '; generated lens driver -- the closure is the preludes graph.\n'
                        printf '; preludes: form-stdlib/source-compiler-text-lens.fk\n'
                        printf '(do (form-source-compile-file "%s" "%s") 0)\n' "$src" "$out"
                    } > "$driver"
                    if "$FKWU_SRC" "$driver" >/dev/null 2>&1 && [[ -s "$out" ]]; then
                        mv -f "$out" "$cached"
                    fi
                    rm -f "$out" "$driver" "${driver%.fk}.fkb" "${driver%.fk}.sym" 2>/dev/null
                fi
                if [[ ! -s "$cached" ]]; then
                    out="$(mktemp "$SOURCE_CACHE_DIR/.${key}.XXXXXX")"
                    driver="$(mktemp "$source_compile_dir/compile-${safe}.XXXXXX")"
                    printf '(do (form-source-compile-file "%s" "%s"))\n' "$src" "$out" > "$driver"
                    if "$GO_BIN" "${compiler_chain[@]}" "$driver" >/dev/null && [[ -s "$out" ]]; then
                        mv -f "$out" "$cached"
                    fi
                    rm -f "$out" "$driver"
                fi
            fi
            if [[ -s "$cached" ]]; then
                prepared_args+=("$cached")
            else
                # No silent raw fallback: a raw section-bearing source cannot
                # agree on any arm, so handing it forward only moves the failure
                # somewhere quieter. Refusing HERE names the real seam.
                echo "validate.sh: source lens failed for $src on both fkwu and go lanes" >&2
                echo "  a section-bearing source cannot run raw; fix the lens, not the band" >&2
                exit 1
            fi
        else
            prepared_args+=("$src")
        fi
    done
}

# --- bench mode: run sibling bench suites side-by-side -------------------
if [[ "${1:-}" == "--bench" ]]; then
    echo "=== Go ==="
    "$GO_BIN" --bench
    echo ""
    echo "=== Rust ==="
    "$RS_BIN" --bench
    echo ""
    echo "=== TypeScript ==="
    run_ts --bench
    exit 0
fi

binary_mode=0
if [[ "${1:-}" == "--binary" ]]; then
    binary_mode=1
    shift
fi

# --- run_siblings: feed one Form workload through all kernels, compare ---
# A "workload" can be multiple .fk files loaded sequentially (e.g. stdlib
# prelude + test file). Every kernel receives the same file list.
run_siblings() {
    local label="$1"; shift
    local go_out rs_out ts_out legs
    prepare_sources "$@"
    # Fourth leg: when the workload's band is in the fourth-arm manifest,
    # its pre-flattened table runs on the emitted universal walker (fkwu)
    # alongside the three walkers. Native execution answers in milliseconds,
    # so max(legs) — the band's wall time — does not move.
    local fourth_tbl="" fk_out=""
    if fourth_available; then
        # AXIOM-4 again, per band. A band DECLARED in fourth-arm-bands.txt whose
        # table fails to prepare used to leave $fourth_tbl empty, and the band
        # then ran three-arm and was stamped ✓ like any other — the manifest
        # said four, the run gave three, and the output said nothing. Declared
        # means mandatory: if the manifest names this band, its table must exist.
        local fourth_stem
        fourth_stem="$(fourth_band_stem "${*: -1}" || true)"
        fourth_tbl="$(fourth_table_for_band "${*: -1}" || true)"
        if [[ -n "$fourth_stem" && -z "$fourth_tbl" ]]; then
            echo "validate.sh: $fourth_stem is declared in $FOURTH_MANIFEST but its fourth-arm table" >&2
            echo "  did not prepare. Refusing to run it three-arm under a four-arm declaration." >&2
            exit 1
        fi
    fi
    # The three kernels run CONCURRENTLY: a band's wall time is max(leg), not
    # sum — on compiler-heavy bands the Go+Rust legs ride inside the TS leg's
    # shadow for free. Outputs stay byte-compared exactly as before.
    #
    # Each leg gets its OWN TMPDIR under the legs dir: bands reach scratch
    # space through the `temp_dir` native, so concurrent sibling legs (and
    # concurrent validate runs) never share a scratch path. The legs dir is
    # removed after comparison, so band scratch leaves no sediment.
    legs="$(mktemp -d "${TMPDIR:-/tmp}/form-legs.XXXXXX")"
    prepare_leg_args() {
        local leg="$1"
        local root="$legs/tmp-$leg"
        local outdir="$legs/src-$leg"
        local src out
        mkdir -p "$root" "$outdir"
        leg_args=()
        for src in "${prepared_args[@]}"; do
            if grep -q '"/tmp/' "$src"; then
                out="$outdir/$(basename "$src")"
                sed "s#\"/tmp/#\"$root/#g" "$src" > "$out"
                leg_args+=("$out")
            else
                leg_args+=("$src")
            fi
        done
    }
    prepare_leg_args go
    go_args=("${leg_args[@]}")
    prepare_leg_args rs
    rs_args=("${leg_args[@]}")
    prepare_leg_args ts
    ts_args=("${leg_args[@]}")
    ( TMPDIR="$legs/tmp-go" "$GO_BIN" "${go_args[@]}" > "$legs/go" 2>&1 || true ) &
    ( TMPDIR="$legs/tmp-rs" "$RS_BIN" "${rs_args[@]}" > "$legs/rs" 2>&1 || true ) &
    ( TMPDIR="$legs/tmp-ts" run_ts "${ts_args[@]}" > "$legs/ts" 2>&1 || true ) &
    if [[ -n "$fourth_tbl" ]]; then
        # Consume the complete arm-profile stream while retaining the verdict.
        # `head -1` closed the pipe early and could SIGPIPE the emitted worker
        # thread during process teardown, leaving macOS in an unkillable UE wait.
        ( TMPDIR="$legs/tmp-fk" "$FKWU" "$fourth_tbl" 0 2>/dev/null | sed -n '1p' > "$legs/fk" || true ) &
    fi
    wait
    go_out=$(cat "$legs/go"); rs_out=$(cat "$legs/rs"); ts_out=$(cat "$legs/ts")
    if [[ -n "$fourth_tbl" ]]; then fk_out=$(cat "$legs/fk" 2>/dev/null || true); fi
    rm -rf "$legs" 2>/dev/null || true
    if [[ "$go_out" == "$rs_out" && "$go_out" == "$ts_out" ]] \
        && { [[ -z "$fourth_tbl" ]] || [[ "$fk_out" == "$go_out" ]]; }; then
        # REGISTERED-VERDICT GATE (2026-08-17). fourth-arm-bands.txt column 3 is
        # the band's registered verdict, and until today NOTHING read it — every
        # consumer parsed `stem kind _`, so the column was write-only and 114
        # bands drifted from it silently, both directions, over months. This
        # suite already proves the four arms AGREE; agreement was the only thing
        # it proved, so a band that grew six checks kept passing while the
        # registry still described the old six. Now an agreed verdict that
        # differs from the registered one is a failure with its own word, so a
        # band cannot change what it certifies without the change being seen.
        # The verdict compared is the LAST line of the agreed output — fks bands
        # answer one scalar, fkc bands may print above it — and only when the
        # registered column is numeric (the one teach-sema-code row is not a
        # band row and never reaches here).
        local reg_stem reg_want reg_have
        reg_stem="$(fourth_band_stem "${*: -1}" || true)"
        reg_want=""
        if [[ -n "$reg_stem" ]]; then
            reg_want="$(awk -v b="$reg_stem" '!/^#/ && $1==b{print $3; exit}' "$FOURTH_MANIFEST")"
        fi
        if [[ "$reg_want" =~ ^[0-9]+$ ]]; then
            reg_have="${go_out##*$'\n'}"
            if [[ "$reg_have" != "$reg_want" ]]; then
                printf "  ✗  %-30s  → %s agreed on every arm, but the manifest registers %s — REGISTERED-VERDICT DRIFT\n" \
                    "$label" "$reg_have" "$reg_want"
                fail=$((fail + 1))
                if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
                return
            fi
        fi
        printf "  ✓  %-30s  → %s\n" "$label" "$go_out"
        ok=$((ok + 1))
        if [[ -n "$fourth_tbl" ]]; then fourth_ok=$((fourth_ok + 1)); fi
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then
            if [[ -n "$fourth_tbl" ]]; then echo "ok fourth" > "$SUITE_STATUS_FILE"; else echo "ok" > "$SUITE_STATUS_FILE"; fi
        fi
    elif [[ -n "$fourth_tbl" ]]; then
        printf "  ✗  %-30s\n      go         = %s\n      rust       = %s\n      typescript = %s\n      fourth     = %s\n" \
            "$label" "$go_out" "$rs_out" "$ts_out" "$fk_out"
        fail=$((fail + 1))
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
    else
        printf "  ✗  %-30s\n      go         = %s\n      rust       = %s\n      typescript = %s\n" \
            "$label" "$go_out" "$rs_out" "$ts_out"
        fail=$((fail + 1))
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
    fi
}

run_siblings_binary() {
    local label="$1"; shift
    local artifact="$1"; shift
    local go_out rs_out ts_out
    go_out=$("$GO_BIN" --binary "$artifact" 2>&1 || true)
    rs_out=$("$RS_BIN" --binary "$artifact" 2>&1 || true)
    ts_out=$(run_ts --binary "$artifact" 2>&1 || true)
    if [[ "$go_out" == "$rs_out" && "$go_out" == "$ts_out" ]]; then
        printf "  ✓  %-30s  → %s\n" "$label" "$go_out"
        ok=$((ok + 1))
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "ok" > "$SUITE_STATUS_FILE"; fi
    else
        printf "  ✗  %-30s\n      go         = %s\n      rust       = %s\n      typescript = %s\n" \
            "$label" "$go_out" "$rs_out" "$ts_out"
        fail=$((fail + 1))
        if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "fail" > "$SUITE_STATUS_FILE"; fi
    fi
}

run_workload() {
    local label="$1"; shift
    local bin_artifact
    if [[ $binary_mode -eq 0 ]]; then
        local band="${*: -1}" level declared answered
        level="$(fk_band_proof_level "$band")"
        if [[ "$level" == "FOURTH-ARM" ]]; then
            declared="$(fk_band_declared_verdict "$band")"
            if [[ -z "$FKWU_SRC" ]]; then
                printf "  ⧗  %-30s  fkwu-only lane — runtime fkwu unavailable; not witnessed this run\n" "$label"
                if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "staged" > "$SUITE_STATUS_FILE"; fi
                staged=$((staged + 1))
                return
            fi
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
            answered="$( (cd .. && ./fkwu "form/$band") 2>"$lane_out" | tail -1 || true)"
            lane_diags="$(grep -c "unresolved-call\|error:" "$lane_out" 2>/dev/null || true)"
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
            printf "  ⧗  %-30s  staged fkwu lane — carrier absent in this checkout; pending, not witnessed\n" "$label"
            if [[ -n "${SUITE_STATUS_FILE:-}" ]]; then echo "staged" > "$SUITE_STATUS_FILE"; fi
            staged=$((staged + 1))
            return
        fi
    fi
    if [[ $binary_mode -eq 1 ]]; then
        bin_artifact="$(mktemp "${TMPDIR:-/tmp}/form-kernel.XXXXXX")"
        prepare_sources "$@"
        "$GO_BIN" --emit-binary "$bin_artifact" "${prepared_args[@]}"
        run_siblings_binary "binary/$label" "$bin_artifact"
        rm -f "$bin_artifact"
    else
        run_siblings "$label" "$@"
    fi
}

ok=0
fail=0
fourth_ok=0
fkwu_only=0
staged=0

# --- explicit mode: validate one file list as one workload --------------
if [[ $# -gt 0 ]]; then
    explicit_args=("$@")
    # Single band file: honor declared imports like the full stdlib/tests sweep.
    if [[ $# -eq 1 ]]; then
        f="$1"
        fk_expand_declared_deps "$f"
        if [[ ${#fk_import_expanded[@]} -gt 0 ]]; then
            explicit_args=(form-stdlib/core.fk "${fk_import_expanded[@]}" "$f")
        fi
    fi
    # A missing input file is not a kernel divergence. Without this guard the
    # three walkers each open the absent path and emit a DIFFERENT file-not-found
    # string while fkwu emits nothing, so the verdict reads "kernels disagree —
    # investigate which is correct" — a phantom divergence that has cost real
    # diagnostic effort (e.g. running `gelu-erf-band.fk` when the band is named
    # `transformer-gelu-erf-band.fk`). Name the absent path plainly instead.
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
    # Pre-flatten every covered band's table in one Go run before the
    # suite fans out — cold cache pays ~20s once; warm runs skip it.
    fourth_prepare_all
    # Pre-compile the one prelude every band shares so the pool's first
    # wave doesn't race N copies of the same compile (atomic mv converges
    # them, but each lost race re-pays the full source-compiler walk).
    prepare_sources form-stdlib/core.fk

    # The suite fans out ACROSS bands: each workload is one job in a pool
    # (VALIDATE_JOBS wide, default 8), writing an ordered result block plus
    # a status file; the aggregation prints blocks in collection order and
    # counts from the status files. A band's legs were already concurrent;
    # this makes the bands themselves concurrent — the suite's wall time is
    # sum(bands)/jobs instead of sum(bands). Caches stay safe under the
    # fan-out: source-compile and fourth-table writes are content-keyed and
    # atomic (mv), every leg owns a private TMPDIR.
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
    # --- form-stdlib/tests/*.{fk,form}: prepend stdlib preludes --------
    # Convention: core.fk is always prepended. If the test name matches
    # an additional module (e.g. tests/parser.fk → parser.fk), that
    # module is loaded between core.fk and the test.
    if [[ -d form-stdlib/tests ]]; then
        for f in form-stdlib/tests/*.fk form-stdlib/tests/*.form; do
            if [[ ! -e "$f" ]]; then
                continue
            fi
            base="$(basename "$f")"
            base="${base%.*}"
            module="form-stdlib/${base}.fk"
            # A test file may declare extra imports via header lines:
            #   ; import "form-stdlib/engine.fk"
            # Legacy `; preludes:` headers are still expanded by the same path.
            # When present, those modules load between core.fk and the test
            # (in the order declared). The same-name convention still works
            # — modules referenced by the header replace the auto-prepend.
            fk_expand_declared_deps "$f"
            if [[ ${#fk_import_expanded[@]} -gt 0 ]]; then
                add_workload "stdlib/$(basename "$f")" "form-stdlib/core.fk" "${fk_import_expanded[@]}" "$f"
            elif [[ -f "$module" && "$module" != "$f" ]]; then
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
            ok)             ok=$((ok + 1)) ;;
            staged)         staged=$((staged + 1)) ;;
            *)              fail=$((fail + 1)) ;;
        esac
        i=$((i + 1))
    done
    rm -rf "$suite_dir"
fi

echo ""
if [[ $fourth_ok -gt 0 ]]; then
    echo "  fourth arm: $fourth_ok band(s) four-way (fkwu + pre-flattened tables)"
elif [[ $ok -gt 0 ]]; then
    # The SECOND way a zero happens, and the one the fourth_available refusal
    # above cannot see. That gate asks "can the arm be built at all" and exits 1
    # when it cannot. This asks the different question: the arm built fine, and
    # then fired for NOT ONE band in the run — because coverage is per-band
    # (fourth-arm-bands.txt), so a workload naming only unregistered bands gets
    # a full set of ✓ marks with three kernels behind every one of them.
    #
    # Until now that printed nothing whatsoever: the summary line was inside
    # `if fourth_ok > 0`, so zero was reported by silence. Witnessed 2026-07-25,
    # from the other side of the same wound the block above documents — a band
    # written that day claimed four-way in its own header while
    # scripts/fourth-arm-gate.sh answered NO-FOURTH, and nothing in a plain
    # validate.sh run would have said so. Say the zero out loud instead.
    echo "  fourth arm: 0 band(s) — built, but no band in this run is covered;"
    echo "              every ✓ above speaks for three kernels. Register the band in"
    echo "              fourth-arm-bands.txt, or check bootstrap/fkwu-uni.stamp against"
    echo "              scripts/fourth-arm.sh fourth_emit_chain_stamp."
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
    else
        echo "  $ok ok, 0 divergent — kernels agree on every sample."
    fi
    exit 0
else
    echo "  $ok ok, $fail divergent — kernels disagree. Investigate which is correct."
    exit 1
fi
