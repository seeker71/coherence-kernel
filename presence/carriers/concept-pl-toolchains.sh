#!/usr/bin/env bash
# Fetch pinned, repository-external Java/Kotlin carriers into /tmp.
# Usage: eval "$(presence/carriers/concept-pl-toolchains.sh)"
set -euo pipefail

toolchain_root=${CONCEPT_PL_TOOLCHAIN_ROOT:-/tmp/coherence-cp10-toolchains}
jdk_archive="$toolchain_root/OpenJDK25U-jdk_aarch64_mac_hotspot_25.0.3_9.tar.gz"
kotlin_archive="$toolchain_root/kotlin-compiler-2.3.21.zip"
jdk_url='https://github.com/adoptium/temurin25-binaries/releases/download/jdk-25.0.3%2B9/OpenJDK25U-jdk_aarch64_mac_hotspot_25.0.3_9.tar.gz'
kotlin_url='https://github.com/JetBrains/kotlin/releases/download/v2.3.21/kotlin-compiler-2.3.21.zip'
jdk_sha=7baab4d69a15554e119b86ff78d40e3fdc28819b5b322955c913cebfe3f6a37c
kotlin_sha=a8cfc1d62cd4d0de4d04f42575e40135bd620588c17d568a20eb9c7c259af14f

mkdir -p "$toolchain_root" "$toolchain_root/jdk" "$toolchain_root/kotlin"

fetch_verified() {
  local url=$1 archive=$2 expected=$3
  if [[ ! -f "$archive" ]]; then
    /usr/bin/curl -fL --retry 3 --progress-bar "$url" -o "$archive"
  fi
  local actual
  actual=$(shasum -a 256 "$archive" | awk '{print $1}')
  if [[ "$actual" != "$expected" ]]; then
    echo "checksum mismatch for $archive: expected $expected, got $actual" >&2
    exit 1
  fi
}

if [[ ! -x "$toolchain_root/jdk/Contents/Home/bin/javac" ]]; then
  fetch_verified "$jdk_url" "$jdk_archive" "$jdk_sha"
  tar -xzf "$jdk_archive" -C "$toolchain_root/jdk" --strip-components=1
fi

if [[ ! -x "$toolchain_root/kotlin/kotlinc/bin/kotlinc" ]]; then
  fetch_verified "$kotlin_url" "$kotlin_archive" "$kotlin_sha"
  unzip -q -o "$kotlin_archive" -d "$toolchain_root/kotlin"
fi

printf "export PATH='%s:%s':\"$PATH\"\n" \
  "$toolchain_root/jdk/Contents/Home/bin" "$toolchain_root/kotlin/kotlinc/bin"
