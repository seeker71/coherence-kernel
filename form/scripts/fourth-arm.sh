#!/usr/bin/env zsh
# fourth-arm.sh — the native fkwu source/JIT door as a validate.sh leg.
#
# Sourced by validate.sh (cwd = form/). Validation's fourth sibling is the
# repo-root fkwu: it walks the workload root validate.sh writes, the same
# closure the three siblings read as plain Form, and lets the same binary's
# Form-native CPU/Metal/MLX JIT doors crystallize only what execution asks
# for. fkwu is a leg of every workload: execution, diagnostics, and agreement
# are checked, never reduced to three siblings. A manifest row adds the
# verdict its band is registered to certify.

FOURTH_MANIFEST="fourth-arm-bands.txt"
FOURTH_SOURCE_FKWU="${FORM_FOURTH_SOURCE_FKWU:-}"

fourth_available() {
    [[ -n "$FOURTH_SOURCE_FKWU" && -x "$FOURTH_SOURCE_FKWU" ]]
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

# fourth_band_stem — manifest stem for a band file path, or empty. A registered
# verdict only applies to a CANONICAL band — the workload's last file living under
# form-stdlib/tests/. A same-named sample elsewhere (e.g. a cross-modal demo
# whose basename collides with a manifest stem) never resolves, because the
# registered band is form-stdlib/tests/<stem>-band.fk and the sample's own
# output would be compared against it — a false divergence. Anchoring to the
# tests/ path keeps the stem the contract for the real band only.
fourth_band_stem() {
    local band="$1" stem hit
    if [[ "$band" != form-stdlib/tests/* && "$band" != */form-stdlib/tests/* ]]; then
        return 0
    fi
    stem="$(basename "$band")"
    stem="${stem%.fk}"
    stem="${stem%.bml}"
    [[ -f "$FOURTH_MANIFEST" ]] || return 0
    # Exact name first, then the name without -band: a row may name the file
    # whole (tests/<stem>.fk) or the band it proves (tests/<stem>-band.fk).
    hit="$(awk -v b="$stem" '$1==b{print $1; exit}' "$FOURTH_MANIFEST")"
    if [[ -n "$hit" ]]; then printf '%s\n' "$hit"; return 0; fi
    stem="${stem%-band}"
    awk -v b="$stem" '$1==b{print $1; exit}' "$FOURTH_MANIFEST"
}
