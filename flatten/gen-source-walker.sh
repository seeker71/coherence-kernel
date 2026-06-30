#!/bin/sh
# gen-source-walker.sh — regenerate runtime/fkwu-optable.h from the ONE op table.
#
# The op-table (name arity tag) and the rewrite rules are DATA: flt-ops in
# flatten/form-flatten.fk (from form/form-stdlib/native-op-manifest.fk) + the
# rewrite rows in flatten/gen-source-walker-table.fk. This wrapper is the only
# non-Form glue — a trivial stdout capture, never a generator in Go. The Form
# recipe computes the entire header; the shell just splices flt-ops into the
# recipe's (do ...) and redirects the printed value to the header file.
#
# Usage: flatten/gen-source-walker.sh [path-to-fkwu]   (default /tmp/fkwu)
set -e
cd "$(dirname "$0")/.."
FKWU="${1:-/tmp/fkwu}"
[ -x "$FKWU" ] || { cc -O2 -o /tmp/fkwu runtime/fkwu-uni.c; FKWU=/tmp/fkwu; }
RUN="$(mktemp)"
{
  echo '(do'
  # the flt-ops defn (the manifest table) — span derived, not hardcoded, so adding
  # an op row (a (nothing)/value row) never silently truncates the table here.
  OPS_BEG="$(grep -n '^(defn flt-ops ()' flatten/form-flatten.fk | head -1 | cut -d: -f1)"
  OPS_END="$(awk "NR>$OPS_BEG && /^\(defn /{print NR-1; exit}" flatten/form-flatten.fk)"
  sed -n "${OPS_BEG},${OPS_END}p" flatten/form-flatten.fk
  sed '1s/^(do$//' flatten/gen-source-walker-table.fk
} > "$RUN"
"$FKWU" --src "$RUN" > runtime/fkwu-optable.h
rm -f "$RUN"
echo "wrote runtime/fkwu-optable.h ($(wc -l < runtime/fkwu-optable.h) lines)"
