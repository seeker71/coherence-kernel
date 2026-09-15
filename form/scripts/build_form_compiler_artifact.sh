#!/usr/bin/env bash
# Build .fkb artifacts used by the binary→native-python proof loop.
#
# Phase 1 surface: the Python-native emitter reads its category table
# out of a compiled .fkb instead of carrying it as a Form literal.
# This script produces that .fkb.
#
# Two output modes:
#
#   --out PATH         legacy: build the source-compiler .fkb (kept for
#                     downstream callers; falls back to a placeholder when
#                     --emit-binary support shifts in the kernel).
#
#   --categories PATH  Phase 1: form-stdlib/bml/ontology-emit.bml reads
#                     form-stdlib/form-ontology.json and emits a tiny Form
#                     file that binds pn-categories to the python.bmf
#                     (name, inst) list (door: form-stdlib/ontology-emit-run.fk),
#                     then this script source-compiles it to PATH via
#                     form-kernel-go --emit-binary.
#
# Usage examples:
#   ./form/scripts/build_form_compiler_artifact.sh --categories form/.cache/emit_native_python/python-bmf-categories.fkb
#   ./form/scripts/build_form_compiler_artifact.sh --out form/.cache/form-native-python/source.fkb

set -euo pipefail

usage() {
    cat >&2 <<EOF
Usage:
  $0 --categories OUT_FKB_PATH
  $0 --out OUT_FKB_PATH
EOF
    exit 2
}

MODE=""
OUT=""
while [[ $# -gt 0 ]]; do
    case "$1" in
        --categories)
            MODE="categories"
            shift
            OUT="${1:-}"
            ;;
        --out)
            MODE="legacy"
            shift
            OUT="${1:-}"
            ;;
        --help|-h)
            usage
            ;;
        *)
            echo "unknown arg: $1" >&2
            usage
            ;;
    esac
    shift || true
done

[[ -z "$MODE" || -z "$OUT" ]] && usage

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
FORM_DIR="$REPO_ROOT/form"
KERNEL_GO="$FORM_DIR/form-kernel-go"

mkdir -p "$(dirname "$OUT")"

if [[ ! -x "$KERNEL_GO/bin-go" ]]; then
    echo "Building form-kernel-go..." >&2
    (cd "$KERNEL_GO" && go build -o bin-go .) || {
        echo "form-kernel-go build failed" >&2
        exit 1
    }
fi

cd "$FORM_DIR"

if [[ "$MODE" == "categories" ]]; then
    # form-stdlib/bml/ontology-emit.bml reads the python.bmf categories out
    # of form-ontology.json and emits a tiny .fk that binds pn-categories.
    # The .fk is the *generated* source: its sole job is to carry the same
    # data the ontology JSON holds, in a shape the kernel can serialize. The
    # emitter never touches the .fk; only its .fkb projection.
    GEN_FK="$(dirname "$OUT")/python-bmf-categories.fk"
    case "$GEN_FK" in
        /*) GEN_ABS="$GEN_FK" ;;
        *) GEN_ABS="$PWD/$GEN_FK" ;;
    esac
    if [[ ! -x "$REPO_ROOT/fkwu" ]]; then
        echo "Building fkwu..." >&2
        (cd "$REPO_ROOT" && cc -O2 -o fkwu.new runtime/fkwu-uni.c && mv fkwu.new fkwu)
    fi
    (cd "$REPO_ROOT" && ./fkwu form/form-stdlib/ontology-emit-run.fk <<< "pn $GEN_ABS") >&2
    echo "Generated $GEN_FK ($(wc -l < "$GEN_FK") lines)" >&2

    echo "Compiling $(basename "$GEN_FK") → $(basename "$OUT") via form-kernel-go --emit-binary..." >&2
    "$KERNEL_GO/bin-go" --emit-binary "$OUT" "$GEN_FK"
    echo "ok — $OUT ($(stat -c%s "$OUT" 2>/dev/null || stat -f%z "$OUT") bytes)" >&2
    exit 0
fi

# --- legacy --out path (compiler artifact) -----------------------------
SOURCES=(
    "form-stdlib/core.fk"
    "form-stdlib/compiler.fk"
    "form-stdlib/source-compiler.fk"
    "form-stdlib/engine.fk"
    "form-stdlib/grammars/python-bmf.fk"
)

echo "Compiling Form sources via form-kernel-go --emit-binary..." >&2
"$KERNEL_GO/bin-go" --emit-binary "$OUT" "${SOURCES[@]}"

echo "ok — $OUT"
