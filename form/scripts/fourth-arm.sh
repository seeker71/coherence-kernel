#!/usr/bin/env zsh
# fourth-arm.sh — the native fkwu source/JIT door as a validate.sh leg.
#
# Sourced by validate.sh (cwd = form/). Validation's fourth sibling is the
# repo-root fkwu source runner: it resolves the band's own `; preludes:` graph,
# walks source directly, and lets the same binary's Form-native CPU/Metal/MLX
# JIT doors crystallize only what execution asks for. No band table, flattened
# artifact or index is an admission condition for the proof. Every manifest
# workload is mandatory: source execution, diagnostics, and agreement are
# checked, never silently reduced to three siblings.

# Workload lists are zero-based. Zsh normally indexes arrays from 1, which
# would silently turn ${srcs[0]} into an empty read_file row and shift the
# band source. KSH_ARRAYS gives this source-compatible carrier the same
# zero-based indexing without making Bash a condition of Form publication.
if [[ -n "${ZSH_VERSION:-}" ]]; then
    setopt KSH_ARRAYS
fi

# Anchored to this script's own form/ so a caller whose cwd is the repo root
# never grows a phantom form-stdlib/.cache/ there.
FOURTH_HOME="$(cd "$(dirname "${BASH_SOURCE[0]:-$0}")/.." && pwd)"
FOURTH_DIR="$FOURTH_HOME/form-stdlib/.cache/fourth"
FOURTH_MANIFEST="fourth-arm-bands.txt"
# Direct validation copies each band into a content+carrier-keyed source cache.
# This keeps stale/error-bearing .fkb/.sym files beside the authored test from
# deciding a new run, while allowing fkwu's own source artifact/JIT cache to be
# reused on the next run.
FOURTH_SOURCE_RUN_DIR="$FOURTH_DIR/source-run"
FOURTH_SOURCE_FKWU="${FORM_FOURTH_SOURCE_FKWU:-}"

fourth_available() {
    [[ -n "$FOURTH_SOURCE_FKWU" && -x "$FOURTH_SOURCE_FKWU" ]]
}

# Portable content hash for cache keys. macOS ships `shasum`; Linux and Git
# Bash ship `sha256sum`; they don't overlap. The value is only ever a per-host
# cache key, so the algorithm is free — only availability matters.
_fourth_hash_stdin() {
    # Strip CR so cache keys match across LF checkouts (mac/linux) and CRLF
    # working trees (Windows Git Bash autocrlf).
    if command -v shasum >/dev/null 2>&1 && printf test | shasum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | shasum | cut -c1-16
    elif command -v sha1sum >/dev/null 2>&1 && printf test | sha1sum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | sha1sum | cut -c1-16
    elif command -v sha256sum >/dev/null 2>&1 && printf test | sha256sum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | sha256sum | cut -c1-16
    elif command -v cksum >/dev/null 2>&1 && printf test | cksum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | cksum | cut -c1-16
    else
        echo "fourth-arm.sh: need shasum, sha1sum, sha256sum, or cksum for cache keys" >&2
        return 1
    fi
}

# The bytes a cache key folds: each file present, and a line naming each one absent, so a
# key over an artifact a fresh checkout has not built yet stays a key under pipefail.
_fourth_cat_present() {
    local f
    for f in "$@"; do
        if [[ -f "$f" ]]; then cat "$f"; else printf 'absent:%s\n' "$f"; fi
    done
}

fourth_hash16() {
    _fourth_cat_present "$@" | _fourth_hash_stdin
}

# Executables are byte artifacts, so their cache identity preserves every byte
# (including 0x0d).  Text keys above intentionally normalize checkout CRLF.
fourth_raw_hash16() {
    if command -v shasum >/dev/null 2>&1 && printf test | shasum >/dev/null 2>&1; then
        _fourth_cat_present "$@" |shasum | cut -c1-16
    elif command -v sha1sum >/dev/null 2>&1 && printf test | sha1sum >/dev/null 2>&1; then
        _fourth_cat_present "$@" |sha1sum | cut -c1-16
    elif command -v sha256sum >/dev/null 2>&1 && printf test | sha256sum >/dev/null 2>&1; then
        _fourth_cat_present "$@" |sha256sum | cut -c1-16
    elif command -v cksum >/dev/null 2>&1 && printf test | cksum >/dev/null 2>&1; then
        _fourth_cat_present "$@" |cksum | cut -c1-16
    else
        echo "fourth-arm.sh: need shasum, sha1sum, sha256sum, or cksum for cache keys" >&2
        return 1
    fi
}

# build_fourth — name the fourth kernel validate.sh was handed: the runtime fkwu
# source/JIT door. It is built once by validate.sh (plain seed); nothing is
# compiled or flattened here.
build_fourth() {
    [[ -f "$FOURTH_MANIFEST" ]] || return 0
    if fourth_available; then
        echo "  fourth kernel: runtime fkwu source/JIT door (no flatten tables)" >&2
    else
        echo "  fourth kernel: runtime fkwu source/JIT door unavailable" >&2
    fi
}

# fourth_in_string_awk — the string-literal tracker the workload composer below
# reads: an escaped character stays inside a literal, and a ";" outside one starts
# a comment. A line that opens inside a literal is carried as text.
fourth_in_string_awk='
    function fk_scan(line,   i, c, n) {
        n = length(line)
        for (i = 1; i <= n; i++) {
            c = substr(line, i, 1)
            if (fk_in) {
                if (c == "\\") { i++; continue }
                if (c == "\"") fk_in = 0
            } else {
                if (c == ";") break
                if (c == "\"") fk_in = 1
            }
        }
    }
'

# fourth_prepare_source_workload — compose validate.sh's already-prepared
# sibling workload into one direct-source root. Section-bearing BML arguments
# have already crossed the Form source-text lens; every non-root source becomes
# an absolute prelude and the prepared root remains executable source. This is
# the source analogue of passing N files to Go/Rust/TS. The original root's
# dependency directives are removed from the cached copy because their prepared
# absolute equivalents are authoritative for this run.
fourth_prepare_source_workload() {
    local outdir="$1"; shift
    local srcs=("$@") count last band out tmp key carrier_key abs f
    count="${#srcs[@]}"
    [[ "$count" -ge 1 ]] || return 1
    last=$((count - 1))
    band="${srcs[$last]}"
    [[ -f "$band" ]] || return 1
    mkdir -p "$outdir"
    carrier_key="$(fourth_raw_hash16 "$FOURTH_SOURCE_FKWU")"
    key="$(fourth_hash16 "${srcs[@]}")-$carrier_key"
    out="$outdir/w-v2-$key-$(basename "$band")"
    if [[ ! -s "$out" ]]; then
        tmp="$(mktemp "$outdir/.w-v2-$key.XXXXXX")"
        {
            printf '; generated direct-source workload; prepared source closure follows.\n'
            if [[ "$last" -gt 0 ]]; then
                printf '; preludes:'
                local i
                for ((i = 0; i < last; i++)); do
                    f="${srcs[$i]}"
                    [[ -f "$f" ]] || { rm -f "$tmp"; return 1; }
                    case "$f" in
                        /*|[A-Za-z]:*) abs="$f" ;;
                        *) abs="$PWD/$f" ;;
                    esac
                    printf ' %s' "$abs"
                done
                printf '\n'
            else
                printf '; preludes: form-stdlib/core.fk\n'
            fi
            # Dependency declarations have already been expanded into the
            # absolute prelude list above. Keeping the relative directive
            # would load a second, sometimes BML-bearing, copy of the module.
            # A line that opens inside a string literal is text, never a
            # directive (validate.sh's fk_in_string_awk reads it the same way).
            awk "$fourth_in_string_awk"'
                fk_in { print; fk_scan($0); next }
                /^;[[:space:]]*preludes:/ { next }
                /^[[:space:]]*import[[:space:]]+[A-Za-z_][A-Za-z0-9_?!-]*[[:space:]]*;/ { print; fk_scan($0); next }
                /^[[:space:]]*import([[:space:]:]|\")/ { next }
                /^;[[:space:]]*import([[:space:]:]|\")/ { next }
                { print; fk_scan($0) }
            ' "$band"
        } > "$tmp"
        mv -f "$tmp" "$out"
    fi
    case "$out" in
        /*|[A-Za-z]:*) abs="$out" ;;
        *) abs="$PWD/$out" ;;
    esac
    printf '%s\n' "$abs"
}

# fourth_band_stem — manifest stem for a band file path, or empty. The fourth
# arm only applies to a CANONICAL band — the workload's last file living under
# form-stdlib/tests/. A same-named sample elsewhere (e.g. a cross-modal demo
# whose basename collides with a manifest stem) never resolves, because the
# registered band is form-stdlib/tests/<stem>-band.fk and the sample's own
# output would be compared against it — a false divergence. Anchoring to the
# tests/ path keeps the stem the contract for the real band only.
fourth_band_stem() {
    local band="$1" stem hit home="tests"
    if [[ "$band" == form-stdlib/seedbank/tests/* || "$band" == */form-stdlib/seedbank/tests/* ]]; then
        home="seedbank"
    elif [[ "$band" != form-stdlib/tests/* && "$band" != */form-stdlib/tests/* ]]; then
        return 0
    fi
    stem="$(basename "$band")"
    stem="${stem%.fk}"
    stem="${stem%.bml}"
    [[ -f "$FOURTH_MANIFEST" ]] || return 0
    # A seedbank test (form-stdlib/seedbank/tests/<stem>.fk) is a band home too, under its
    # exact name and only when no form-stdlib/tests band claims the stem: tests/ resolves
    # first, so one stem names one file from either side.
    if [[ "$home" == seedbank ]]; then
        if [[ -f "form-stdlib/tests/${stem}-band.fk" || -f "form-stdlib/tests/${stem}.fk" ]]; then
            return 0
        fi
        awk -v b="$stem" '$1==b{print $1; exit}' "$FOURTH_MANIFEST"
        return 0
    fi
    # Exact name FIRST, stripped second. Rows whose registered name KEEPS the
    # -band suffix (form-cli-band, form-cli-repl-control-band) are reached by
    # their exact file name; every other row is reached by the name without it.
    hit="$(awk -v b="$stem" '$1==b{print $1; exit}' "$FOURTH_MANIFEST")"
    if [[ -n "$hit" ]]; then printf '%s\n' "$hit"; return 0; fi
    stem="${stem%-band}"
    awk -v b="$stem" '$1==b{print $1; exit}' "$FOURTH_MANIFEST"
}
