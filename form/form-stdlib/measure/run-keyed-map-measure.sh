#!/usr/bin/env bash
# run-keyed-map-measure.sh — the wall-clock half of Stone 40, on the Go kernel.
#
# This runs a MEASUREMENT, not a band. It lives in form-stdlib/measure/ and not in
# form-stdlib/tests/ on purpose: validate.sh globs every .fk under tests/ and runs it on every
# arm, and a program that reads the 85 GB model's header and walks the merge array a dozen times
# has no business inside a correctness suite. The correctness proof is
# form-stdlib/tests/keyed-map-band.fk (verdict 4095, Go kernel and fkwu, ~33 s, no model file).
#
# `; preludes:` comment lines are LIVE recursive load directives; this resolves them the same way
# tests/run-dsv4-tokenizer-band.sh does and runs bin-go over the resolved file list.
#
# Takes several minutes and its numbers are only as still as the machine. Record `uptime` before
# and after (gapghost) and read the two interleaved passes A and B against each other.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
BAND="form-stdlib/measure/keyed-map-measure.fk"

if [[ ! -x "$GO_BIN" ]]; then
    echo "  building go kernel..." >&2
    (cd "$ROOT/form-kernel-go" && go build -o bin-go .)
fi

FK_SEEN=""
fk_deps() {
    awk '/^;[ \t]*preludes:/{s=$0;sub(/^;[ \t]*preludes:[ \t]*/,"",s);gsub(/,/," ",s);
         n=split(s,a,/[ \t]+/);for(i=1;i<=n;i++){low=tolower(a[i]);
         if(a[i]=="\\"||low=="none"||low=="(none)"||a[i]=="")continue;
         if(a[i]~/\.fk$/)print a[i]}}' "$1" 2>/dev/null
}
fk_path() {
    local dir; dir="$(dirname "$1")"
    if   [[ -f "$dir/$2" ]]; then printf '%s\n' "$dir/$2"
    elif [[ -f "$2" ]];      then printf '%s\n' "$2"
    elif [[ "$2" == form/* && -f "${2#form/}" ]]; then printf '%s\n' "${2#form/}"
    else printf '%s\n' "$dir/$2"; fi
}
fk_expand() {
    local f="$1" d p
    case " $FK_SEEN " in *" $f "*) return ;; esac
    FK_SEEN="$FK_SEEN $f"
    while read -r d; do
        [[ -z "$d" ]] && continue
        p="$(fk_path "$f" "$d")"
        fk_expand "$p"
    done < <(fk_deps "$f")
    printf '%s\n' "$f"
}

cd "$ROOT"
FILES=(); while read -r x; do FILES+=("$x"); done < <(fk_expand "$BAND")
exec "$GO_BIN" "${FILES[@]}"
