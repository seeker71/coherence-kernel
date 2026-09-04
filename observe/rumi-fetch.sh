#!/bin/sh
# rumi-fetch.sh — the one host crossing: Ganjoor's public API, Mowlana's Masnavi (cat 103)
# and Divan-e Shams (cat 98), public-domain Persian originals, one poem per file of verse
# lines under form-stdlib/locale-rows/rumi/<cat>/<poem>.txt, a title line first ("# ...").
# Polite pacing; a done marker ends it. Run detached: nohup sh observe/rumi-fetch.sh &
cd "$(dirname "$0")/.." || exit 1
OUT=form/form-stdlib/locale-rows/rumi; API=https://api.ganjoor.net/api/ganjoor
Q=$OUT/.queue; LOG=$OUT/fetch.log; : > $Q; printf '103\n98\n' >> $Q; n=0
while [ -s $Q ]; do
  cat=$(head -1 $Q); sed -i '' '1d' $Q
  json=$(curl -s --max-time 30 "$API/cat/$cat?poems=true") || continue
  echo "$json" | sed -n 's/.*"children":\[\(.*\)\],"poems".*/\1/p' | tr '{' '\n' | grep -o '"id":[0-9]*' | cut -d: -f2 >> $Q
  mkdir -p $OUT/$cat
  for p in $(echo "$json" | sed -n 's/.*"poems":\[\(.*\)\].*/\1/p' | tr '{' '\n' | grep -o '"id":[0-9]*' | cut -d: -f2); do
    [ -s $OUT/$cat/$p.txt ] && continue
    pj=$(curl -s --max-time 30 "$API/poem/$p") || continue
    { echo "$pj" | grep -o '"title":"[^"]*"' | head -1 | sed 's/"title":"/# /; s/"$//'; echo "$pj" | tr '{' '\n' | grep -o '"text":"[^"]*"' | sed 's/"text":"//; s/"$//'; } > $OUT/$cat/$p.txt
    n=$((n+1)); [ $((n % 100)) -eq 0 ] && echo "$(date +%H:%M) poems=$n cat=$cat" >> $LOG
    sleep 0.25
  done
done
echo "$(date +%H:%M) done poems=$n" >> $LOG; date > $OUT/done
