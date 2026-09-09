#!/usr/bin/env bash
# fourth-arm-survey.sh — sense how far the fourth kernel's band coverage
# reaches TODAY: every form-stdlib/tests/*-band.fk outside the manifest is
# classified (BML-dialect / multi-prelude / shape) and, where the flattener
# can read it, flattened + run on fkwu against the Go walker's own verdict.
# Output: one line per band in $OUT/results.tsv —
#   stem<TAB>category<TAB>expected<TAB>fkw
# Categories: pass mismatch bml multi-prelude multi-line non-1line-int
#             flatten-empty go-timeout in-manifest
# Read the resulting vocabulary candidates with the native source lens:
# from the repository root: ./fkwu observe/fourth-arm-gap-analysis-run.fk </dev/null
# The lens does not execute bands or establish semantic parity.
set -u
cd "$(dirname "$0")/.."

GO_BIN="form-kernel-go/bin-go"
OUT="form-stdlib/.cache/fourth-survey"
mkdir -p "$OUT"
source scripts/fourth-arm.sh
FOURTH_EXECUTION_MODE=table
SURVEY_SOURCE_FKWU="$FOURTH_HOME/../fkwu"
SURVEY_SOURCE_DOOR="$FOURTH_HOME/../observe/native-source-prepare-run.fk"
SURVEY_WORK="$(mktemp -d "$OUT/.source-work.XXXXXX")" || exit 1
trap 'rm -rf "$SURVEY_WORK"' EXIT
[[ -x "$SURVEY_SOURCE_FKWU" ]] || { echo "native source compiler unavailable: $SURVEY_SOURCE_FKWU" >&2; exit 1; }
# Select the same content-qualified table witness in the parent and workers.
build_fourth
fourth_available || { echo "no fkwu — abort"; exit 1; }

EMPTY="$OUT/empty.fk"
: > "$EMPTY"

# portable timeout (macOS has no coreutils timeout; perl alarm carries it)
run_to() { local secs="$1"; shift; perl -e 'alarm shift; exec @ARGV' "$secs" "$@"; }

# Form reads current source on each request; fkwu owns its compiler image and
# transitive dependency freshness. Temporary BML proof inputs belong to this
# invocation and are released by the trap. No second persistent source cache.
prep() {
    local src="$1" out result
    out="$(mktemp "$SURVEY_WORK/input.XXXXXX")" || return 1
    if ! result="$(printf '%s\n' "$FOURTH_HOME/$src" "$FOURTH_HOME/$out" |
        (cd "$FOURTH_HOME/.." && "$SURVEY_SOURCE_FKWU" "$SURVEY_SOURCE_DOOR") 2>>"$OUT/source-${SURVEY_STEM:-prepare}.err")"; then
        printf 'native source preparation failed for %s; evidence=%s/source-%s.err\n' "$src" "$OUT" "${SURVEY_STEM:-prepare}" >&2
        return 1
    fi
    if [[ "${result##*$'\n'}" != 1 ]]; then
        printf 'source=%s\n%s\n' "$src" "$result" >>"$OUT/source-${SURVEY_STEM:-prepare}.err"
        printf 'native source preparation refused %s: %s\n' "$src" "$result" >&2
        return 1
    fi
    printf '%s\n' "${result%%$'\n'*}"
}

survey_one() {
    local band="$1" stem pres noncores cls="single" exp d tbl fkw f srcs=() rl pl prepared core phase_status
    stem="$(basename "$band")"; stem="${stem%-band.fk}"
    SURVEY_STEM="$stem"
    : > "$OUT/source-$stem.err"
    # in-manifest rows are already gated; skip
    if awk -v b="$stem" '$1==b{found=1} END{exit !found}' fourth-arm-bands.txt 2>/dev/null; then
        printf '%s\tin-manifest\t-\t-\n' "$stem"; return
    fi
    pres="$(grep -E '^; preludes:' "$band" 2>/dev/null | head -1 | sed 's/^; preludes://')"
    noncores="$(printf '%s\n' $pres | grep -v 'core\.fk' | grep . || true)"
    if grep -Eq '^[[:space:]]*section \[' "$band" $noncores 2>/dev/null; then
        cls="bml"
    elif [[ "$(printf '%s\n' "$noncores" | grep -c .)" -gt 1 ]]; then
        cls="multi"
    fi
    # io bands stay unexecuted: they reach the host filesystem/processes (a
    # cache-eviction band once removed form-stdlib/.cache out from under a
    # running survey), and the io family is a named wall — no table to gain
    if grep -hEq '\((write_file|write_form_binary|read_form_binary|file_mtime|file_size|temp_dir|scan_run|source_scan_file|shell_run|tcp_|http_)' "$band" $noncores 2>/dev/null; then
        printf '%s\t%s-io\t-\t-\n' "$stem" "$cls"; return
    fi
    # The band's source list (non-core preludes then the band), each prepared
    # through the native compiler when it carries BML.
    if [[ -z "$noncores" && -f "form-stdlib/$stem.fk" ]]; then
        noncores="form-stdlib/$stem.fk"
    fi
    for f in $noncores "$band"; do
        if ! prepared="$(prep "$f")"; then
            printf '%s\t%s-source-preparation-failed\t-\t-\n' "$stem" "$cls"
            return 1
        fi
        srcs+=("$prepared")
    done
    if ! core="$(prep form-stdlib/core.fk)"; then
        printf '%s\t%s-source-preparation-failed\t-\t-\n' "$stem" "$cls"
        return 1
    fi
    # the Go walker's own verdict (the siblings' shape; full output)
    run_to 90 "$GO_BIN" "$core" "${srcs[@]}" >"$OUT/go-$stem.out" 2>"$OUT/go-$stem.err"
    phase_status=$?
    printf '%s\n' "$phase_status" >"$OUT/go-$stem.exit"
    if [[ "$phase_status" != 0 ]]; then
        printf '%s\t%s-go-exit-%s\t-\t-\n' "$stem" "$cls" "$phase_status"
        return 1
    fi
    exp="$(cat "$OUT/go-$stem.out")"
    if [[ -z "$exp" ]]; then
        printf '%s\t%s-go-empty\t-\t-\n' "$stem" "$cls"; return
    fi
    if [[ "$(printf '%s\n' "$exp" | wc -l)" -gt 1 ]]; then
        printf '%s\t%s-multi-line\t%s\t-\n' "$stem" "$cls" "$(printf '%s' "$exp" | head -1)"; return
    fi
    if ! [[ "$exp" =~ ^-?[0-9]+$ ]]; then
        printf '%s\t%s-non-1line-int\t%s\t-\n' "$stem" "$cls" "$exp"; return
    fi
    # flatten every source through the multi-source string-pool door (the
    # shim rides first: core vocabulary + string stones as function rows)
    rl=" (read_file \"form-stdlib/fourth-shim.fk\")"
    for f in "${srcs[@]}"; do rl="$rl (read_file \"$f\")"; done
    d="$(mktemp -d "${TMPDIR:-/tmp}/fk-survey.XXXXXX")"
    cat form-stdlib/minimal-surface.fk form-stdlib/hati-os-kernel.fk \
        form-stdlib/hati-os-kernel-emit.fk form-stdlib/form-parse.fk \
        form-stdlib/form-flatten.fk > "$d/driver.fk"
    printf '(fks-table-file (flt-srcs-fns (list%s)) (flt-srcs-pool (list%s) (list)))\n' \
        "$rl" "$rl" >> "$d/driver.fk"
    tbl="$OUT/t-$stem.txt"
    run_to 120 "$GO_BIN" "$d/driver.fk" > "$tbl" 2>"$OUT/flatten-$stem.err"
    phase_status=$?
    printf '%s\n' "$phase_status" >"$OUT/flatten-$stem.exit"
    rm -rf "$d"
    if [[ "$phase_status" != 0 ]]; then
        printf '%s\t%s-flatten-exit-%s\t%s\t-\n' "$stem" "$cls" "$phase_status" "$exp"
        return 1
    fi
    if [[ ! -s "$tbl" ]]; then
        printf '%s\t%s-flatten-empty\t%s\t-\n' "$stem" "$cls" "$exp"; return
    fi
    run_to 60 "$FKWU" "$tbl" 0 >"$OUT/table-$stem.out" 2>"$OUT/table-$stem.err"
    phase_status=$?
    printf '%s\n' "$phase_status" >"$OUT/table-$stem.exit"
    if [[ "$phase_status" != 0 ]]; then
        printf '%s\t%s-table-exit-%s\t%s\t-\n' "$stem" "$cls" "$phase_status" "$exp"
        return 1
    fi
    fkw="$(head -1 "$OUT/table-$stem.out")"
    if [[ "$fkw" == "$exp" ]]; then
        printf '%s\t%s-pass\t%s\t%s\n' "$stem" "$cls" "$exp" "$fkw"
    else
        printf '%s\t%s-mismatch\t%s\t%s\n' "$stem" "$cls" "$exp" "${fkw:-∅}"
    fi
}

if [[ "${1:-}" == "--one" ]]; then
    survey_one "$2" > "$OUT/r-$(basename "${2%-band.fk}").tsv"
    exit $?
fi

rm -f "$OUT"/r-*.tsv
ls form-stdlib/tests/*-band.fk | xargs -P 8 -I{} bash "scripts/fourth-arm-survey.sh" --one {}
survey_status=$?
cat "$OUT"/r-*.tsv > "$OUT/results.tsv"

echo "── survey ──"
cut -f2 "$OUT/results.tsv" | sort | uniq -c | sort -rn
echo "results: $OUT/results.tsv"
exit "$survey_status"
