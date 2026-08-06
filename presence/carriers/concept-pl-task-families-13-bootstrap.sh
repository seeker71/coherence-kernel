#!/usr/bin/env bash
# Prepare and verify every external carrier used by the task-family live gate.
# This script invokes no Python command.
set -euo pipefail

carrier_dir=$(cd "$(dirname "$0")" && pwd)
eval "$("$carrier_dir/concept-pl-toolchains.sh")"

for command_name in node npx clang clang++ dotnet go rustc ruby php swiftc; do
  if ! command -v "$command_name" >/dev/null 2>&1; then
    printf 'missing required carrier: %s\n' "$command_name" >&2
    exit 1
  fi
done

toolchain_root=/tmp/coherence-cp10-toolchains
for executable in \
  "$toolchain_root/jdk/Contents/Home/bin/javac" \
  "$toolchain_root/jdk/Contents/Home/bin/java" \
  "$toolchain_root/kotlin/kotlinc/bin/kotlinc"; do
  if [[ ! -x "$executable" ]]; then
    printf 'missing bootstrapped carrier: %s\n' "$executable" >&2
    exit 1
  fi
done

npx --yes --package typescript@5.9.3 tsc --version
npx --yes --package tsx@4.20.6 tsx --version
printf 'task-family carriers ready: 12; Python held by policy\n'
