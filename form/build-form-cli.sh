#!/usr/bin/env zsh
# build-form-cli.sh — produce the standalone native form-cli binary.
#
# Build-time honest floor (2026-06-24):
#   STANDARD — copy the committed platform binary when the source stamp matches
#              (no Go, no clang, no shell in the receipt path).
#   REGEN    — maintainer-only bootstrap artifacts refresh the table/C/platform
#              binaries. Runtime remains the standalone fkwu binary.
# Runtime: the resulting form-cli runs toolchain-free.
#
#   ./build-form-cli.sh            # -> form/form-cli
#   echo ping | ./form-cli        # -> pong   (no toolchain present)
#   ./form-cli                     # interactive REPL on a real tty
set -euo pipefail
cd "$(dirname "$0")"

S=form-stdlib
OUT="${1:-form-cli}"
CC_BIN="${CC:-clang}"
CLI_BOOTSTRAP_C="$S/bootstrap/form-cli-emitted.c"
CLI_BOOTSTRAP_STAMP="$S/bootstrap/form-cli.stamp"
CLI_BOOTSTRAP_SOURCE_SHA256="$S/bootstrap/form-cli.source.sha256"
CLI_BOOTSTRAP_ATTESTATION="$S/bootstrap/form-cli.generation.attestation"
FORM_CLI_FORCE_LINK="${FORM_CLI_FORCE_LINK:-0}"
FORM_CLI_CALLER_EXTRA_SRC="${FORM_CLI_EXTRA_SRC:-}"
FORM_CLI_CALLER_EXTRA_LDFLAGS="${FORM_CLI_EXTRA_LDFLAGS:-}"
FORM_CLI_EXTRA_SRC="$FORM_CLI_CALLER_EXTRA_SRC"
FORM_CLI_EXTRA_LDFLAGS="$FORM_CLI_CALLER_EXTRA_LDFLAGS"

# Canonical bootstrap/platform publication is not an extension point.  The
# normal build command may still accept an explicit local carrier addition,
# but a publisher has to produce the exact source-defined carrier rather than
# inherit arbitrary objects or linker flags from its environment.  The host's
# built-in Metal carrier is appended below after this check.
if [[ "${FORM_CLI_CANONICAL_PUBLISH:-0}" == 1 ]] && \
        [[ -n "$FORM_CLI_CALLER_EXTRA_SRC" || -n "$FORM_CLI_CALLER_EXTRA_LDFLAGS" ]]; then
    printf '%s\n' 'form-cli canonical publish refuses FORM_CLI_EXTRA_SRC/FORM_CLI_EXTRA_LDFLAGS' >&2
    exit 1
fi

# The accelerator this host HAS is not a question for the caller. The kernel already settles the
# Windows side with no flag at all — fkwu-uni.c LoadLibraryA's nvcuda.dll at runtime, JITs the
# Form-emitted PTX, and reports absence if no driver answers. Darwin gets the same treatment here:
# Metal.framework ships on every Mac, so there is nothing to opt into, and the carrier itself
# returns SKIP when MTLCreateSystemDefaultDevice() finds no device. A build that asked
# FORM_CLI_EXTRA_SRC for this was asking the caller a question the host already answers, which is
# how a body ends up not knowing what it is capable of.
if [[ "$(uname -s 2>/dev/null)" == "Darwin" && -f "native/metal/fk-metal-carrier.m" ]]; then
    FORM_CLI_EXTRA_SRC="native/metal/fk-metal-carrier.m${FORM_CLI_EXTRA_SRC:+ $FORM_CLI_EXTRA_SRC}"
    FORM_CLI_EXTRA_LDFLAGS="-framework Metal -framework Foundation -fobjc-arc${FORM_CLI_EXTRA_LDFLAGS:+ $FORM_CLI_EXTRA_LDFLAGS}"
fi

# The same law, one organ later. `fkwu` has linked the MLX carrier whenever
# libmlxc is on the host since 2026-08-20 (AGENTS.md carries that recipe), but
# form-cli did not, so the binary a session actually speaks through answered
# mlx_linked=false on a machine whose MLX was live — the body not knowing what
# it is capable of, in the exact shape the paragraph above names. Unlike Metal,
# MLX is not shipped with the OS, so presence is the question, not the caller.
FK_MLX_LIB="${FK_MLX_LIB:-/opt/homebrew/lib/libmlxc.dylib}"
FK_MLX_PREFIX="${FK_MLX_LIB%/lib/libmlxc.dylib}"
if [[ "$(uname -s 2>/dev/null)" == "Darwin" && -f "native/mlx/fk-mlx-carrier.c" && -f "$FK_MLX_LIB" ]]; then
    FORM_CLI_EXTRA_SRC="native/mlx/fk-mlx-carrier.c${FORM_CLI_EXTRA_SRC:+ $FORM_CLI_EXTRA_SRC}"
    FORM_CLI_EXTRA_LDFLAGS="-I$FK_MLX_PREFIX/include -L$FK_MLX_PREFIX/lib -lmlxc -Wl,-rpath,$FK_MLX_PREFIX/lib${FORM_CLI_EXTRA_LDFLAGS:+ $FORM_CLI_EXTRA_LDFLAGS}"
fi

is_windows_host() {
    [[ "${OS:-}" == "Windows_NT" || "${OSTYPE:-}" == msys* || "${OSTYPE:-}" == cygwin* ]]
}

patch_windows_emitted_c() {
    local c_file="$1"
    sed -i '1i #define _CRT_SECURE_NO_WARNINGS 1' "$c_file"
    sed -i 's|extern unsigned int arc4random(void);|extern int rand(void); static unsigned int arc4random(void) { return (unsigned int)rand(); }|' "$c_file"
    sed -i 's|extern long long read(int, void \*, unsigned long);|extern int read(int, void *, unsigned int);|' "$c_file"
    sed -i 's|extern long long write(long long, const void \*, unsigned long);|extern int write(int, const void *, unsigned int);|' "$c_file"
    sed -i 's|mkdir(d, 0777)|mkdir(d)|g; s|mkdir(p, 0777)|mkdir(p)|g' "$c_file"
    sed -i 's|extern int sprintf(char \*, const char \*, ...);|typedef __builtin_va_list fk_va_list; extern int vsnprintf(char *, unsigned long long, const char *, fk_va_list); static int sprintf(char *b, const char *fmt, ...) { fk_va_list ap; __builtin_va_start(ap, fmt); int n = vsnprintf(b, 4096ULL, fmt, ap); __builtin_va_end(ap); return n; }|' "$c_file"
    sed -i 's|struct timeval { long tv_sec; int tv_usec; }; extern int gettimeofday(struct timeval \*, void \*);|struct timeval { long tv_sec; int tv_usec; }; struct fk_filetime { unsigned int dwLowDateTime; unsigned int dwHighDateTime; }; __declspec(dllimport) void __stdcall GetSystemTimeAsFileTime(struct fk_filetime *); static int gettimeofday(struct timeval *tv, void *tz) { (void)tz; struct fk_filetime ft; unsigned long long ticks; unsigned long long us; GetSystemTimeAsFileTime(\&ft); ticks = ((unsigned long long)ft.dwHighDateTime * 4294967296ULL) + (unsigned long long)ft.dwLowDateTime; us = (ticks / 10ULL) - 11644473600000000ULL; tv->tv_sec = (long)(us / 1000000ULL); tv->tv_usec = (int)(us % 1000000ULL); return 0; }|' "$c_file"
    sed -i 's|extern void \*dlopen(const char \*, int); extern void \*dlsym(void \*, const char \*);|static void *dlopen(const char *p, int f) { (void)p; (void)f; return 0; } static void *dlsym(void *h, const char *s) { (void)h; (void)s; return 0; }|' "$c_file"
}

if [[ "${FORM_STANDARD_LANE:-0}" != 1 ]]; then
    command -v "$CC_BIN" >/dev/null || { echo "${CC_BIN} is required at BUILD time (not at run time)"; exit 1; }
fi

W="$(mktemp -d)"
trap 'rm -rf "$W"' EXIT
BOOTSTRAP_SNAPSHOT_DIR="$W/bootstrap-snapshot"
SNAPSHOT_TABLE="$BOOTSTRAP_SNAPSHOT_DIR/form-cli-table.txt"
SNAPSHOT_EMITTED_C="$BOOTSTRAP_SNAPSHOT_DIR/form-cli-emitted.c"
SNAPSHOT_STAMP="$BOOTSTRAP_SNAPSHOT_DIR/form-cli.stamp"
SNAPSHOT_SOURCE_SHA256="$BOOTSTRAP_SNAPSHOT_DIR/form-cli.source.sha256"
SNAPSHOT_ATTESTATION="$BOOTSTRAP_SNAPSHOT_DIR/form-cli.generation.attestation"
SNAPSHOT_PLATFORM_BIN="$BOOTSTRAP_SNAPSHOT_DIR/form-cli-platform"
SNAPSHOT_PLATFORM_STAMP="$BOOTSTRAP_SNAPSHOT_DIR/form-cli-platform.stamp"
SNAPSHOT_PLATFORM_ATTESTATION="$BOOTSTRAP_SNAPSHOT_DIR/form-cli-platform.generation.attestation"

# EMIT_CHAIN, FLAT_CHAIN, MODS and BAND stood here and were assigned, never read.
# Four of the "SIX lists, one program" below, kept in step by hand for nothing: a
# cell added to them reached no build and a cell missing from them broke none.
# form_cli_source_list.sh is the one identity now, so they are gone rather than
# reconciled — two lines had each edited MODS in the same hour, which is the cost
# of mirroring a list that no reader ever consults. Found by reading every
# capitalised assignment in this file for a matching read: 3 of 31 had none, and
# EMIT_CHAIN's only reader was FLAT_CHAIN, itself dead.

# Prefer fkwu self-host flatten (no Go) when T_flat + cached fkwu are warm.
# This list authors the stamp the build compares against the one
# scripts/regen_form_cli_bootstrap.sh wrote, so it mirrors that script's
# FORM_CLI_SRCS in CONTENT AND ORDER. A cell added there and not here stops the
# build with "source stamp stale (have=... want=...)" — two hashes and no
# filename — which reads like a stale tree and is a mirroring gap. See the
# "SIX lists, one program" note in that script.
# One source identity is shared by the build, bootstrap regeneration, and
# platform-carrier regeneration.  The retired host turn carrier is neither
# part of this identity nor a Form flatten input.
# shellcheck source=scripts/form_cli_source_list.sh
source scripts/form_cli_source_list.sh
form_cli_load_sources
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
# shellcheck source=scripts/form_cli_bootstrap_proof.sh
source scripts/form_cli_bootstrap_proof.sh
slug="$(fourth_platform_slug)"
CLI_BOOTSTRAP_BIN="$S/bootstrap/form-cli-${slug}"
CLI_BOOTSTRAP_BIN_STAMP="$S/bootstrap/form-cli-${slug}.stamp"
CLI_BOOTSTRAP_BIN_ATTESTATION="$S/bootstrap/form-cli-${slug}.generation.attestation"
stamp="$(fourth_fkwu_cache_stamp)"
cached_fkwu="$FOURTH_DIR/fkwu-$stamp"
[[ -x "$cached_fkwu" ]] && FKWU="$cached_fkwu"
if [[ -z "${FKWU:-}" ]]; then
    if [[ "${FORM_STANDARD_LANE:-0}" == 1 ]]; then
        build_fourth
    else
        build_fourth >/dev/null 2>&1 || true
    fi
fi

want_cli_stamp="$(fourth_hash16 "${FORM_CLI_SRCS[@]}")"
want_source_sha256="$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")"

snapshot_regular_file() {
    local source="$1" destination="$2"
    [[ -f "$source" && ! -L "$source" ]] || {
        printf 'form-cli bootstrap: missing regular input for snapshot: %s\n' "$source" >&2
        return 1
    }
    cp "$source" "$destination"
    [[ -f "$destination" && ! -L "$destination" ]]
}

verify_staged_bootstrap_carrier() {
    form_cli_verify_bootstrap \
        "$SNAPSHOT_TABLE" "$SNAPSHOT_EMITTED_C" "$SNAPSHOT_STAMP" "$want_cli_stamp" \
        && form_cli_verify_source_digest "$SNAPSHOT_SOURCE_SHA256" "$want_source_sha256" \
        && form_cli_verify_generation_attestation \
            "$SNAPSHOT_ATTESTATION" "$want_source_sha256" "$want_cli_stamp" \
            "$SNAPSHOT_TABLE" "$SNAPSHOT_EMITTED_C" "not-applicable"
}

snapshot_bootstrap_carrier() {
    mkdir "$BOOTSTRAP_SNAPSHOT_DIR"
    snapshot_regular_file "$S/bootstrap/form-cli-table.txt" "$SNAPSHOT_TABLE" \
        && snapshot_regular_file "$CLI_BOOTSTRAP_C" "$SNAPSHOT_EMITTED_C" \
        && snapshot_regular_file "$CLI_BOOTSTRAP_STAMP" "$SNAPSHOT_STAMP" \
        && snapshot_regular_file "$CLI_BOOTSTRAP_SOURCE_SHA256" "$SNAPSHOT_SOURCE_SHA256" \
        && snapshot_regular_file "$CLI_BOOTSTRAP_ATTESTATION" "$SNAPSHOT_ATTESTATION" \
        && verify_staged_bootstrap_carrier
}

snapshot_platform_carrier() {
    snapshot_regular_file "$CLI_BOOTSTRAP_BIN" "$SNAPSHOT_PLATFORM_BIN" \
        && snapshot_regular_file "$CLI_BOOTSTRAP_BIN_STAMP" "$SNAPSHOT_PLATFORM_STAMP" \
        && snapshot_regular_file "$CLI_BOOTSTRAP_BIN_ATTESTATION" "$SNAPSHOT_PLATFORM_ATTESTATION" \
        && [[ -x "$SNAPSHOT_PLATFORM_BIN" ]] \
        && [[ "$(cat "$SNAPSHOT_PLATFORM_STAMP")" == "$want_cli_stamp" ]] \
        && form_cli_verify_generation_attestation \
            "$SNAPSHOT_PLATFORM_ATTESTATION" "$want_source_sha256" "$want_cli_stamp" \
            "$SNAPSHOT_TABLE" "$SNAPSHOT_EMITTED_C" "$slug" "$SNAPSHOT_PLATFORM_BIN" \
            "$SNAPSHOT_ATTESTATION"
}

bootstrap_carrier_fresh=0
if snapshot_bootstrap_carrier; then
    bootstrap_carrier_fresh=1
else
    printf '%s\n' 'form-cli bootstrap: staged generation attestation missing or mixed; regeneration required' >&2
fi
platform_carrier_fresh=0
if [[ "$bootstrap_carrier_fresh" == 1 ]] && snapshot_platform_carrier; then
    platform_carrier_fresh=1
fi

# Standard lane: copy a verified private snapshot of the platform carrier (no
# clang).  Its attestation is checked against the snapshot bootstrap hash, so
# a publisher cannot replace table/C between a live check and this copy.
if [[ "${FORM_STANDARD_LANE:-0}" == 1 ]]; then
    if [[ "$bootstrap_carrier_fresh" == 1 && "$platform_carrier_fresh" == 1 ]]; then
        cp "$SNAPSHOT_PLATFORM_BIN" "$OUT"
        chmod +x "$OUT"
        form_cli_verify_binary_identity "$OUT" "$want_source_sha256"
        echo "standard lane: $OUT from bootstrap/${slug} (no clang)" >&2
        exit 0
    fi
    echo "standard lane: bootstrap form-cli-${slug} missing or stale" >&2
    exit 1
fi

# Warm path: same snapshot rule before invoking clang when available.
if [[ "$bootstrap_carrier_fresh" == 1 && "$platform_carrier_fresh" == 1 && "$FORM_CLI_FORCE_LINK" != 1 && -z "$FORM_CLI_EXTRA_SRC" ]]; then
    if form_cli_verify_generation_attestation \
            "$SNAPSHOT_PLATFORM_ATTESTATION" "$want_source_sha256" "$want_cli_stamp" \
            "$SNAPSHOT_TABLE" "$SNAPSHOT_EMITTED_C" "$slug" "$SNAPSHOT_PLATFORM_BIN" \
            "$SNAPSHOT_ATTESTATION"; then
        cp "$SNAPSHOT_PLATFORM_BIN" "$OUT"
        chmod +x "$OUT"
        form_cli_verify_binary_identity "$OUT" "$want_source_sha256"
        echo "  link: bootstrap form-cli-${slug} (no clang)" >&2
        exit 0
    fi
    printf '%s\n' 'form-cli bootstrap: staged platform carrier attestation missing or mixed; relinking from staged C' >&2
fi

# 1. flatten form-cli-repl into its program table (string pool rides behind it).
# The self-host fallback list is the executable no-Go program order, with the
# shim dropped because fourth_band_request prepends it.  It is deliberately
# separate from the source-identity list: shell carrier files are stamped but
# never parsed as Form.  BML-dialect sources (http-client.fk) ride raw here;
# the compiled lowering lives in scripts/regen_form_cli_bootstrap.sh.
FORM_CLI_SELFHOST_SRCS=(
    "$S/core.fk" "$S/grammars/sanskrit-roots.fk" "$S/resource-port.fk" "$S/bml-native-interface-package-import.fk"
    "$S/hati-os-targets.fk" "$S/form-native-resource-interfaces.fk" "$S/form-fs.fk"
    "$S/storage-port.fk" "$S/host-kernel-carrier.fk" "$S/fnri-standin.fk" "$S/fnri-receipt.fk"
    "$S/http-client.fk" "$S/line-grammar.fk" "$S/str-byte-at.fk" "$S/sha256.fk"
    "$S/hmac-sha256.fk" "$S/hex.fk" "$S/format-arith.fk" "$S/f16-decode.fk"
    "$S/q6k-dequant.fk" "$S/equireach.fk" "$S/equireach-gguf.fk" "$S/gguf-meta.fk" "$S/model-discovery.fk" "$S/q4k-dequant.fk" "$S/weight-load.fk" "$S/voice-traits.fk"
    "$S/nearest-shape.fk" "$S/co-learning.fk" "$S/co-learning-stream.fk" "$S/mesh-dispatch.fk"
    "$S/surprise-salience.fk" "$S/host-sense-organ.fk" "$S/speech-organ.fk"
    "$S/native-host-instance.fk" "$S/text-tokenize.fk" "$S/rag-embed.fk" "$S/rag-index-codec.fk"
    "$S/rag-retrieve.fk" "$S/rag-ask.fk" "$S/ask-cost-receipt.fk" "$S/ask-native-lane.fk" "$S/form-cli-ask.fk" "$S/form-cli-router.fk"
    "$S/form-cli-judge.fk" "$S/confidence-weighted-vote.fk" "$S/lineage-discounted-vote.fk"
    "$S/form-cli-oracle-loop.fk" "$S/form-cli-sufficiency.fk" "$S/form-freq-check.fk"
    "$S/trust-row.fk" "$S/form-cli-ask-gate.fk" "$S/form-cli-staged-trace.fk"
    "$S/form-cli-request.fk" "$S/form-cli-carrier.fk" "$S/form-cli-ask-plus.fk" "$S/form-cli-surface-inquiry.fk"
    "$S/current-branch-landing.fk" "$S/form-cli-inquiry-edge-ledger.fk" "$S/form-cli-inquiry.fk" "$S/ds4-query-channel.fk" "$S/form-cli.fk" "$S/form-cli-gguf-cell.fk"
    "$S/relational-inquiry-metabolism.fk"
    "$S/native-model-native-hierarchy.fk"
    "$S/native-model-control-plane.fk"
    "$S/ask-lane-router.fk"
    "$S/dsv4-tokenizer.fk"
    "$S/qwen35-tokenizer.fk"
    "$S/qwen35-tokfast-v2.fk"
    "$S/kernel-http.fk"
    "$S/form-asm.fk"
    "$S/metal-door.fk"
    "native/metal/sha256-arm64-jit.fk"
    "$S/qwen35-artifact-fetch.fk"
    "$S/q6k-msl.fk"
    "$S/q8-0-msl.fk"
    "$S/q3k-msl.fk"
    "$S/q4k-msl.fk"
    "$S/gated-deltanet-msl.fk"
    "$S/transformer-numerics.fk"
    "$S/trig.fk"
    "$S/tensor-ir.fk"
    "$S/jit-tensor-emit.fk"
    "$S/llama-decode-msl.fk"
    "$S/mla-msl.fk"
    "$S/moe-route-wide-msl.fk"
    "$S/gguf-tensor-index.fk"
    "$S/q3k-dequant.fk"
    "$S/q3k-equireach.fk"
    "$S/kat-coder-embed.fk"
    "native/metal/kat-token-handle.fk"
    "native/metal/qwen35-linear-span-layout-contract.fk"
    "native/metal/qwen35-dense-token-handle.fk"
    "native/metal/qwen35-crystal.fk"
    "native/metal/model-bandwidth.fk"
    "$S/form-cli-qwen-teach-layer.fk"
    "$S/form-cli-model-generate.fk"
    "$S/form-cli-repl.fk"
)
if [[ "$bootstrap_carrier_fresh" == 1 ]]; then
    verify_staged_bootstrap_carrier || {
        printf '%s\n' 'form-cli bootstrap: staged table/C identity changed before flatten; refusing link' >&2
        exit 1
    }
    cp "$SNAPSHOT_TABLE" "$W/table.txt"
    echo "  flatten: bootstrap table (no Go)" >&2
elif fourth_selfhost && fourth_flatten_sources form-cli-build fks "$W/table.txt" "${FORM_CLI_SELFHOST_SRCS[@]}"; then
    echo "  flatten: fkwu self-host (no Go)" >&2
else
    echo "  flatten: unavailable — need bootstrap/form-cli-table.txt or T_flat self-host (maintainer: scripts/regen_form_cli_bootstrap.sh)" >&2
    exit 1
fi
[[ -s "$W/table.txt" ]] || { echo "flatten produced no table"; exit 1; }

# 2. emit the combined walker with the table baked in (fk_prog).
if [[ "$bootstrap_carrier_fresh" == 1 ]]; then
    verify_staged_bootstrap_carrier || {
        printf '%s\n' 'form-cli bootstrap: staged table/C identity changed before emit; refusing link' >&2
        exit 1
    }
    cp "$SNAPSHOT_EMITTED_C" "$W/form-cli.c"
    echo "  emit: bootstrap (no Go)" >&2
else
    echo "  emit: unavailable — need bootstrap/form-cli-emitted.c (maintainer: scripts/regen_form_cli_bootstrap.sh)" >&2
    exit 1
fi
grep -q fk_prog "$W/form-cli.c" || { echo "emit missing baked program"; exit 1; }

# 3. Bake GENESIS from this same canonical identity list. It is both the
#    source digest's input and the file-marked source carried by the binary, so
#    a new active compiler/carrier input cannot be stamped without being
#    re-observable through the source verb.
{
  while IFS= read -r source; do
    printf ';;;; ==== FILE: %s ====\n' "$source"
    cat "$source"
  done < <(form_cli_source_list)
} > "$W/genesis.txt"
GEN_LEN=$(wc -c < "$W/genesis.txt" | tr -d ' ')
# The source identity is not a promise made at the start of a long link.  It
# must still name the bytes just carried into genesis; otherwise leave the
# prior carrier in place and ask for a fresh regeneration/build.
current_cli_stamp="$(fourth_hash16 "${FORM_CLI_SRCS[@]}")"
current_source_sha256="$(form_cli_source_sha256 "${FORM_CLI_SRCS[@]}")"
[[ "$current_cli_stamp" == "$want_cli_stamp" && "$current_source_sha256" == "$want_source_sha256" ]] || {
  printf '%s\n' 'form-cli build: canonical source identity changed during genesis; refusing link' >&2
  exit 1
}
{
  printf '\nconst unsigned char fk_genesis[] = {'
  od -An -v -tu1 "$W/genesis.txt" | tr -s ' \n' ',' | sed 's/^,//; s/,$//'
  printf '};\nconst long long fk_genesis_len = %s;\n' "$GEN_LEN"
} >> "$W/form-cli.c"

# 4. compile once -> the standalone native binary (program + own source baked in).
if [[ "${FORM_STANDARD_LANE:-0}" == 1 ]]; then
    echo "standard lane: skip clang link (use warmed $OUT)" >&2
    exit 0
fi
out_dir="$(dirname "$OUT")"
[[ "$out_dir" == "." ]] || mkdir -p "$out_dir"
clang_args=(
  -O2
  -Wno-error=implicit-function-declaration
  -Wno-implicit-function-declaration
  -Wno-incompatible-library-redeclaration
  -o "$OUT" "$W/form-cli.c"
)
if [[ -n "$FORM_CLI_EXTRA_SRC" ]]; then
  extra_srcs=(${=FORM_CLI_EXTRA_SRC})
  clang_args+=("${extra_srcs[@]}")
fi
if [[ -n "$FORM_CLI_EXTRA_LDFLAGS" ]]; then
  extra_ldflags=(${=FORM_CLI_EXTRA_LDFLAGS})
  clang_args+=("${extra_ldflags[@]}")
fi
if is_windows_host; then
  patch_windows_emitted_c "$W/form-cli.c"
  # The emitted Form walker carries recursive recipe frames on the native
  # thread stack. PE/COFF's 1 MiB default is below the observed production
  # floor for a 600-byte semantic-v2 source excerpt and exits with
  # STATUS_STACK_OVERFLOW (0xC00000FD). Keep the evidence width identical on
  # every host and give the Windows carrier the same practical headroom used
  # by the other proof siblings instead of truncating the grounded source.
  compiler_target="$("$CC_BIN" -dumpmachine 2>/dev/null || true)"
  if [[ "$compiler_target" == *-windows-msvc ]]; then
    clang_args+=(-Wl,/STACK:16777216)
  else
    clang_args+=(-Wl,--stack=16777216)
  fi
  clang_args+=(-lws2_32 -llegacy_stdio_definitions)
fi
"$CC_BIN" "${clang_args[@]}"
form_cli_verify_binary_identity "$OUT" "$want_source_sha256"
echo "built $OUT  ($(wc -c < "$OUT") bytes, self-contained — runs with no Go/clang/table; carries ${GEN_LEN}B of its own source)"
