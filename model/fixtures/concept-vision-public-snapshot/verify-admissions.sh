#!/bin/sh
set -eu

fixture_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

# First bind all 24 local JPEG bytes to their pinned SOURCE-SNAPSHOT hashes.
/bin/sh "$fixture_dir/fetch.sh" --verify-only >/dev/null

# Then derive the ordered target-id contract from provenance, accepting a row
# only when its exact target label occurs in that image's committed ORIGINAL
# classifier stream at the explicit 100,000-ppm floor. MODEL-OUTPUTS.tsv and
# PROVENANCE.tsv are themselves hash-gated by the calling Form cell.
/usr/bin/awk -F '\t' '
  NR == FNR {
    if (FNR > 1 && $2 == "original" && ($4 + 0) >= 100000)
      observed[$1 SUBSEP $5] = 1
    next
  }
  FNR == 1 { next }
  {
    ordinal = sprintf("%04d", $1 + 0)
    if (!observed[ordinal SUBSEP $5]) {
      printf "missing original content admission for %s id=%s label=%s\n", ordinal, $4, $5 > "/dev/stderr"
      failed = 1
    }
    ids = ids (count == 0 ? "" : ",") $4
    count++
  }
  END {
    if (failed || count != 24) exit 1
    printf "%s", ids
  }
' "$fixture_dir/MODEL-OUTPUTS.tsv" "$fixture_dir/PROVENANCE.tsv"
