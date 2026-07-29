#!/usr/bin/env bash
# run-dsv4-tokenizer-band.sh — run the Stone 31 tokenizer proof on the Go kernel.
#
# The band reads the DS4 Flash GGUF header (eqr-of-file) and walks 127 741 merges per encoded piece,
# so it is a file-reading compute proof like native/metal/metal_first_token.sh, not an fkwu arithmetic
# band. This resolves the band's `; preludes:` the same way that script does (the comment lines are
# LIVE recursive load directives) and runs bin-go over the resolved file list.
#
# Prints "Verdict 8191" when every claim holds. Takes ~1-2 min (bands are correctness, not timing).
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
BAND="form-stdlib/tests/dsv4-tokenizer-band.fk"

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
