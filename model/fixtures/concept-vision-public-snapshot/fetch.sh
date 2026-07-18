#!/bin/sh
set -eu

fixture_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
mode=${1:-fetch}

node -e '
  const fs = require("fs");
  const s = JSON.parse(fs.readFileSync(process.argv[1], "utf8"));
  for (const x of s.selected) process.stdout.write([x.local,x.sha256,x.derivativeUrl].join("\t")+"\n");
' "$fixture_dir/SOURCE-SNAPSHOT.json" | while IFS="$(printf '\t')" read -r local expected url; do
  target="$fixture_dir/$local"
  if [ "$mode" != "--verify-only" ]; then
    /usr/bin/curl -LfsS --retry 3 --retry-delay 2 \
      -A 'coherence-kernel/1.0 (github.com/seeker71; public visual fixture)' \
      "$url" -o "$target"
  fi
  observed=$(/usr/bin/shasum -a 256 "$target" | /usr/bin/awk '{print $1}')
  if [ "$observed" != "$expected" ]; then
    printf 'checksum mismatch for %s: expected %s observed %s\n' "$local" "$expected" "$observed" >&2
    exit 1
  fi
done

printf '24 Wikimedia Commons public-snapshot photographs verified\n'
