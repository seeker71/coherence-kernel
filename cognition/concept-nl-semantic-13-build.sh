#!/usr/bin/env bash
# Build the compact, concept-ID-aligned 10k × 13 semantic surface without Python.
# Usage: concept-nl-semantic-13-build.sh /path/to/omw-data /path/to/wn30/dict /path/to/wn31/dict
set -euo pipefail

source_repo=${1:?usage: concept-nl-semantic-13-build.sh /path/to/omw-data /path/to/wn30/dict /path/to/wn31/dict}
wn30_dict=${2:?missing WordNet 3.0 dict directory}
wn31_dict=${3:?missing WordNet 3.1 dict directory}
pinned_commit=406bf83b3c507a3d1f26e88252d5d66893fd36bf
actual_commit=$(git -C "$source_repo" rev-parse HEAD)
if [[ "$actual_commit" != "$pinned_commit" ]]; then
  echo "OMW commit mismatch: expected $pinned_commit, got $actual_commit" >&2
  exit 1
fi

repo_root=$(git rev-parse --show-toplevel)
output_base="$repo_root/cognition/concept-nl-semantic-13"
machine_tsv="$output_base-machine.tsv"
semantic_index="$repo_root/model/concept-semantics-10000-index.dat"
ranked_data="$repo_root/model/concept-10000-ranked.dat"
wn30_sense="$wn30_dict/index.sense"
wn31_sense="$wn31_dict/index.sense"
build_tmp=$(mktemp -d /tmp/concept-nl-semantic-13.XXXXXX)
trap 'rm -rf "$build_tmp"' EXIT

[[ $(wc -c < "$semantic_index" | tr -d ' ') == 350000 ]] || {
  echo "semantic index is not 10,000 fixed 35-byte records" >&2; exit 1;
}
[[ $(wc -c < "$ranked_data" | tr -d ' ') == 300000 ]] || {
  echo "ranked concept data is not 10,000 fixed 30-byte records" >&2; exit 1;
}
[[ -f "$wn30_sense" && -f "$wn31_sense" ]] || {
  echo "WordNet index.sense input missing" >&2; exit 1;
}
[[ $(wc -l < "$machine_tsv" | tr -d ' ') == 10001 ]] || {
  echo "machine translation cache must contain a header plus 10,000 rows" >&2; exit 1;
}
machine_bad_cells=$(LC_ALL=C awk -F $'\t' '
  NR == 1 { if (NF != 13) bad++; next }
  { if (NF != 13) bad++; for (i=2; i<=13; i++) if ($i == "") bad++ }
  END { print bad+0 }
' "$machine_tsv")
[[ "$machine_bad_cells" == 0 ]] || {
  echo "machine translation cache contains $machine_bad_cells missing/malformed cells" >&2; exit 1;
}

expected_wn30_sense_hash=68b3a468cddfd8e92134b9b0624339a02a1b837159243c297c5f138a3d618392
expected_wn31_sense_hash=a09db263da96dbb3273064c60546530dee5927a2e5a39c90fb576cdbebbb1a22
actual_wn30_sense_hash=$(shasum -a 256 "$wn30_sense" | awk '{print $1}')
actual_wn31_sense_hash=$(shasum -a 256 "$wn31_sense" | awk '{print $1}')
[[ "$actual_wn30_sense_hash" == "$expected_wn30_sense_hash" ]] || {
  echo "WordNet 3.0 index.sense hash mismatch" >&2; exit 1;
}
[[ "$actual_wn31_sense_hash" == "$expected_wn31_sense_hash" ]] || {
  echo "WordNet 3.1 index.sense hash mismatch" >&2; exit 1;
}

codes=(eng ind spa fra por swa deu rus cmn jpn arb hin tur)
lenses=(en id es fr pt-br sw de ru zh ja ar hi tr)

# Source order is evidence order: the already-landed Wiktionary projection,
# then a language-specific OMW wordnet when one exists, then the CLDR wordnet.
# First hit wins, so this only fills witnessed absences; it never silently
# replaces an existing label with a different automatic source.
set_language_sources() {
  local code=$1
  language_files=("$source_repo/wns/wikt/wn-wikt-$code.tab")
  case "$code" in
    eng) language_files+=("$source_repo/wns/eng/wn-data-eng.tab") ;;
    ind) language_files+=("$source_repo/wns/msa/wn-data-ind.tab") ;;
    spa) language_files+=("$source_repo/wns/mcr/wn-data-spa.tab") ;;
    fra) language_files+=("$source_repo/wns/fra/wn-data-fra.tab") ;;
    por) language_files+=("$source_repo/wns/por/wn-data-por.tab") ;;
    cmn) language_files+=("$source_repo/wns/cow/wn-data-cmn.tab") ;;
    jpn) language_files+=("$source_repo/wns/jpn/wn-data-jpn.tab") ;;
    arb) language_files+=("$source_repo/wns/arb/wn-data-arb.tab") ;;
  esac
  local cldr="$source_repo/wns/cldr/wn-cldr-$code.tab"
  [[ -f "$cldr" ]] && language_files+=("$cldr")
}

# The semantic model is PWN 3.1 aligned while OMW is PWN 3.0 aligned. Join
# identical sense keys between the two official releases, then retain the
# frequency-ranked English label as the canonical concept identity. The row
# number remains the concept ID; duplicates are deliberately preserved.
LC_ALL=C awk \
  -v wn30_file="$wn30_sense" -v wn31_file="$wn31_sense" \
  -v ranked_file="$ranked_data" -v semantic_file="$semantic_index" '
  function pos(key, parts, kind) {
    split(key, parts, "%")
    kind=substr(parts[2], 1, 1)
    return (kind == 1 ? "n" : (kind == 2 ? "v" : ((kind == 3 || kind == 5) ? "a" : "r")))
  }
  FILENAME == wn30_file { pwn30[$1]=$2 "-" pos($1); next }
  FILENAME == wn31_file {
    if ($1 in pwn30) pwn31_to_30[pos($1) $2]=pwn30[$1]
    next
  }
  FILENAME == ranked_file {
    raw=$0
    for (id=0; id<10000; id++) {
      label=substr(raw, id * 30 + 1, 20)
      sub(/[ ]+$/, "", label)
      ranked_label[id]=label
    }
    next
  }
  FILENAME == semantic_file {
    raw=$0
    for (id=0; id<10000; id++) {
      base=id * 35
      method=substr(raw, base + 1, 1)
      synset=""
      if (method != "0") {
        synset_pos=substr(raw, base + 15, 1)
        if (synset_pos == "s") synset_pos="a"
        pwn31=synset_pos substr(raw, base + 16, 8)
        if (pwn31 in pwn31_to_30) synset=pwn31_to_30[pwn31]
      }
      print id "\t" synset "\t" ranked_label[id]
    }
  }
' "$wn30_sense" "$wn31_sense" "$ranked_data" "$semantic_index" > "$build_tmp/anchors"

if [[ $(wc -l < "$build_tmp/anchors" | tr -d ' ') != 10000 ]]; then
  echo "anchor extraction did not produce exactly 10000 concept rows" >&2
  exit 1
fi

mapped_anchor_count=$(LC_ALL=C awk -F $'\t' '$2 != "" { n++ } END { print n+0 }' "$build_tmp/anchors")
[[ "$mapped_anchor_count" == 7368 ]] || {
  echo "expected 7368 PWN3.0-aligned concepts, got $mapped_anchor_count" >&2; exit 1;
}

for code in "${codes[@]:1}"; do
  set_language_sources "$code"
  LC_ALL=C awk -F $'\t' -v wanted_code="$code" '
    NR == FNR { if ($2 != "") wanted[$2] = 1; next }
    $2 == wanted_code ":lemma" && $3 != "" && wanted[$1] && !seen[$1]++ {
      label=$3
      gsub(/\r/, "", label)
      source=(FILENAME ~ /\/wikt\// ? "W" : (FILENAME ~ /\/cldr\// ? "C" : "D"))
      print $1 "\t" label "\t" source
    }
  ' "$build_tmp/anchors" "${language_files[@]}" \
    > "$build_tmp/$code"
done

LC_ALL=C awk -F $'\t' \
  -v codes="${codes[*]}" -v lenses="${lenses[*]}" \
  -v machine_file="$machine_tsv" \
  -v source_out="$output_base-sources.dat" '
  NR == FNR {
    ids[++anchor_count]=$1
    anchors[anchor_count]=$2
    english[anchor_count]=$3
    next
  }
  {
    if (FILENAME == machine_file) {
      if (FNR > 1) for (j=2; j<=13; j++) machine[$1 SUBSEP j]=$j
      next
    }
    file=FILENAME
    sub(/^.*\//, "", file)
    labels[file SUBSEP $1]=$2
    sources[file SUBSEP $1]=$3
  }
  END {
    code_count=split(codes, code_order, " ")
    lens_count=split(lenses, lens_order, " ")
    printf "synset"
    for (j=1; j<=lens_count; j++) printf "\t%s", lens_order[j]
    printf "\n"
    for (i=1; i<=anchor_count; i++) {
      anchor=anchors[i]
      printf "%s", anchor
      for (j=1; j<=code_count; j++) {
        if (j == 1) {
          label=english[i]
          source="F"
        } else {
          label=(anchor == "" ? "" : labels[code_order[j] SUBSEP anchor])
          source=(anchor == "" ? "" : sources[code_order[j] SUBSEP anchor])
          if (label == "") {
            label=machine[ids[i] SUBSEP j]
            if (label != "") source="G"
          }
        }
        printf "\t%s", label
        printf "%s", (source == "" ? "0" : source) > source_out
      }
      printf "\n"
    }
  }
' "$build_tmp/anchors" \
  "$build_tmp/ind" "$build_tmp/spa" "$build_tmp/fra" \
  "$build_tmp/por" "$build_tmp/swa" "$build_tmp/deu" "$build_tmp/rus" \
  "$build_tmp/cmn" "$build_tmp/jpn" "$build_tmp/arb" "$build_tmp/hin" \
  "$build_tmp/tur" "$machine_tsv" > "$output_base-omw.tsv"

[[ $(wc -c < "$output_base-sources.dat" | tr -d ' ') == 130000 ]] || {
  echo "source matrix is not exactly 130,000 cells" >&2; exit 1;
}

{
  echo '; concept-nl-semantic-13-offsets.fk — generated byte offsets for 10k OMW rows.'
  echo '; witnessed: 2026-07-18 -> 8191 live fkwu'
  echo '; preludes: form/form-stdlib/core.fk'
  echo '(do'
  echo '  (defn cnl13-offsets () (list'
  LC_ALL=C awk 'NR == 1 { offset=length($0)+1; next }
    { print "    " offset; offset += length($0)+1 }
    END { print "    " offset }
  ' "$output_base-omw.tsv"
  echo '  ))'
  echo ')'
} > "$output_base-offsets.fk"

cp "$source_repo/wns/wikt/LICENSE" "$output_base-omw-license.txt"

license_dir="$output_base-licenses"
mkdir -p "$license_dir"
normalize_license() {
  LC_ALL=C awk '{ gsub(/\t/, "    "); sub(/[ \r]+$/, ""); lines[NR]=$0; if ($0 != "") last=NR }
    END { for (i=1; i<=last; i++) print lines[i] }' "$1" > "$2"
}
normalize_license "$source_repo/wns/wikt/LICENSE" "$license_dir/wikt.txt"
normalize_license "$source_repo/wns/msa/LICENSE" "$license_dir/msa-ind.txt"
normalize_license "$source_repo/wns/mcr/LICENSE" "$license_dir/mcr-spa.txt"
normalize_license "$source_repo/wns/fra/LICENSE" "$license_dir/fra.txt"
normalize_license "$source_repo/wns/por/LICENSE" "$license_dir/por.txt"
normalize_license "$source_repo/wns/cow/LICENSE" "$license_dir/cow-cmn.txt"
normalize_license "$source_repo/wns/jpn/LICENSE" "$license_dir/jpn.txt"
normalize_license "$source_repo/wns/arb/LICENSE" "$license_dir/arb.txt"
normalize_license "$source_repo/wns/cldr/LICENSE" "$license_dir/cldr.txt"

selected_hash=$(shasum -a 256 "$output_base-omw.tsv" | awk '{print $1}')
source_code_hash=$(shasum -a 256 "$output_base-sources.dat" | awk '{print $1}')
machine_hash=$(shasum -a 256 "$machine_tsv" | awk '{print $1}')
{
  echo '; concept-nl-semantic-13-metadata.fk — generated OMW provenance and exact coverage.'
  echo '; witnessed: 2026-07-18 -> 1023 four-way; 8191 live fkwu'
  echo '; preludes: form/form-stdlib/core.fk'
  echo '(do'
  echo '  (defn cnl13-source-meta ()'
  echo '    (list "https://github.com/omwn/omw-data"'
  echo "          \"$pinned_commit\""
  echo '          "per-source-licenses-in-concept-nl-semantic-13-licenses"'
  echo "          \"$selected_hash\""
  echo '          10000'
  echo '          "concept-10000-id-via-pwn31-sense-key-to-pwn30"'
  echo '          "wikt-then-dedicated-wordnet-then-cldr-then-machine-first-hit"'
  echo '          "automatic-mapped-unreviewed"'
  echo "          \"$source_code_hash\"))"
  echo '  (defn cnl13-machine-source-meta ()'
  echo '    (list "https://translate.googleapis.com/translate_a/single"'
  echo "          \"$machine_hash\" 120000 \"machine-translated-unreviewed\"))"
  echo '  (defn cnl13-locale-meta-rows () (list'
  for locale_index in "${!codes[@]}"; do
    code=${codes[$locale_index]}
    lens=${lenses[$locale_index]}
    column=$((locale_index + 2))
    present=$(LC_ALL=C awk -F $'\t' -v col="$column" 'NR > 1 && $col != "" { n++ } END { print n+0 }' "$output_base-omw.tsv")
    absent=$((10000 - present))
    if [[ "$code" == eng ]]; then
      source_hash=$(shasum -a 256 "$ranked_data" | awk '{print $1}')
    else
      set_language_sources "$code"
      source_hash=$(for source_file in "${language_files[@]}"; do
        shasum -a 256 "$source_file"
      done | shasum -a 256 | awk '{print $1}')
    fi
    echo "    (list \"$lens\" \"$code\" $present $absent \"$source_hash\" \"mapped-unreviewed\")"
  done
  echo '  ))'
  echo ')'
} > "$output_base-metadata.fk"

{
  echo "source=https://github.com/omwn/omw-data"
  echo "commit=$pinned_commit"
  echo "machine-source=https://translate.googleapis.com/translate_a/single"
  echo "machine-state=automatic-machine-translated-unreviewed"
  echo "selection=concept-10000-id-via-pwn31-sense-key-to-pwn30"
  echo "label=wikt-then-dedicated-wordnet-then-cldr-then-google-machine-first-hit"
  echo "license=per-source; see concept-nl-semantic-13-licenses/"
  echo "wordnet-license=model/concept-semantics-10000-WORDNET-LICENSE.txt"
  echo "$actual_wn30_sense_hash  WordNet-3.0/dict/index.sense"
  echo "$actual_wn31_sense_hash  WordNet-3.1/dict/index.sense"
  echo "$(shasum -a 256 "$semantic_index" | awk '{print $1}')  model/concept-semantics-10000-index.dat"
  echo "$(shasum -a 256 "$ranked_data" | awk '{print $1}')  model/concept-10000-ranked.dat"
  echo "$machine_hash  cognition/concept-nl-semantic-13-machine.tsv"
  echo "$(shasum -a 256 "$repo_root/cognition/concept-nl-semantic-13-machine-build.mjs" | awk '{print $1}')  cognition/concept-nl-semantic-13-machine-build.mjs"
  for code in "${codes[@]:1}"; do
    set_language_sources "$code"
    for source_file in "${language_files[@]}"; do
      source_hash=$(shasum -a 256 "$source_file" | awk '{print $1}')
      echo "$source_hash  ${source_file#"$source_repo/"}"
    done
  done
  echo "$selected_hash  concept-nl-semantic-13-omw.tsv"
  echo "$source_code_hash  concept-nl-semantic-13-sources.dat"
  for license_file in "$license_dir"/*.txt; do
    license_hash=$(shasum -a 256 "$license_file" | awk '{print $1}')
    echo "$license_hash  concept-nl-semantic-13-licenses/${license_file##*/}"
  done
} > "$output_base-source-manifest.txt"

echo "built $output_base-omw.tsv"
wc -l "$output_base-omw.tsv"
