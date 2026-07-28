#!/bin/sh
set -eu

fixture_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
repo_dir=$(CDPATH= cd -- "$fixture_dir/../../.." && pwd)
work_dir=${TMPDIR:-/tmp}/concept-vision-public-snapshot
classifier="$work_dir/classify"
output=${1:-"$fixture_dir/MODEL-OUTPUTS.tsv"}
mkdir -p "$work_dir"

swiftc -O "$repo_dir/presence/carriers/concept-video-vision-classify.swift" -o "$classifier"
printf 'ordinal\tvariant\trank\tconfidence-ppm\tlabel\n' > "$output"
for source in "$fixture_dir"/[0-9][0-9][0-9][0-9].jpg; do
  stem=${source##*/}
  stem=${stem%.jpg}
  band="$work_dir/$stem-band.jpg"
  crop="$work_dir/$stem-crop.jpg"
  ffmpeg -loglevel error -y -i "$source" \
    -vf 'drawbox=x=0:y=ih-80:w=iw:h=80:color=magenta:t=fill' -frames:v 1 "$band"
  ffmpeg -loglevel error -y -i "$source" \
    -vf 'crop=iw*0.8:ih*0.8:iw*0.1:ih*0.1' -frames:v 1 "$crop"
  for variant in original band crop; do
    case "$variant" in
      original) image="$source" ;;
      band) image="$band" ;;
      crop) image="$crop" ;;
    esac
    "$classifier" "$image" | /usr/bin/awk -F '\t' -v o="$stem" -v v="$variant" \
      '{print o "\t" v "\t" NR "\t" $1 "\t" $2}' >> "$output"
  done
done

printf 'captured 72 content-only Apple Vision streams in %s\n' "$output"
