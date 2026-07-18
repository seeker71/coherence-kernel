#!/usr/bin/env bash
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
CLI_BOOTSTRAP_TABLE="$S/bootstrap/form-cli-table.txt"
CLI_BOOTSTRAP_STAMP="$S/bootstrap/form-cli.stamp"
FORM_CLI_FORCE_LINK="${FORM_CLI_FORCE_LINK:-0}"
FORM_CLI_REGEN="${FORM_CLI_REGEN:-0}"
FORM_CLI_FKWU="${FORM_CLI_FKWU:-../fkwu}"
FORM_CLI_TABLE_FKWU="${FORM_CLI_TABLE_FKWU:-}"
FORM_CLI_EXTRA_SRC="${FORM_CLI_EXTRA_SRC:-}"
FORM_CLI_EXTRA_LDFLAGS="${FORM_CLI_EXTRA_LDFLAGS:-}"

if [[ "${FORM_STANDARD_LANE:-0}" == 1 && -x "$OUT" ]]; then
    echo "standard lane: $OUT present (no build)" >&2
    exit 0
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

# the emit chain (plain Form) + the flatten chain.
EMIT_CHAIN="$S/minimal-surface.fk $S/hati-os-kernel.fk $S/host-io-fs-fkwu-emit.fk $S/fkc-table-serialize.fk $S/hati-os-kernel-emit.fk"
FLAT_CHAIN="$EMIT_CHAIN $S/core.fk $S/form-parse.fk $S/bmf-core.fk $S/bmf-grammar.fk $S/host-effect-grammar.fk $S/form-flatten.fk"
DIRECT_FLAT_CHAIN="$S/core.fk $S/str-byte-at.fk $S/form-fs.fk $S/file-byte-window.fk $S/bmf-byte-cursor.fk $EMIT_CHAIN $S/form-parse.fk $S/bmf-core.fk $S/bmf-grammar.fk $S/host-effect-grammar.fk $S/form-flatten.fk"
# Keep the ask support modules before the dispatcher; default ask receipts stay
# local through fkwu RAG while http-client remains available to legacy carriers.
MODS="(list (read_file \"$S/fourth-shim.fk\") (read_file \"$S/core.fk\") (read_file \"$S/resource-port.fk\") (read_file \"$S/bml-native-interface-package-import.fk\") (read_file \"$S/hati-os-targets.fk\") (read_file \"$S/form-native-resource-interfaces.fk\") (read_file \"$S/form-fs.fk\") (read_file \"$S/storage-port.fk\") (read_file \"$S/host-kernel-carrier.fk\") (read_file \"$S/fnri-standin.fk\") (read_file \"$S/fnri-receipt.fk\") (read_file \"$S/http-client.fk\") (read_file \"$S/line-grammar.fk\") (read_file \"$S/str-byte-at.fk\") (read_file \"$S/sha256.fk\") (read_file \"$S/hex.fk\") (read_file \"$S/format-arith.fk\") (read_file \"$S/f16-decode.fk\") (read_file \"$S/q6k-dequant.fk\") (read_file \"$S/q4k-dequant.fk\") (read_file \"$S/weight-load.fk\") (read_file \"$S/voice-traits.fk\") (read_file \"$S/nearest-shape.fk\") (read_file \"$S/co-learning.fk\") (read_file \"$S/co-learning-stream.fk\") (read_file \"$S/mesh-dispatch.fk\") (read_file \"$S/surprise-salience.fk\") (read_file \"$S/host-sense-organ.fk\") (read_file \"$S/speech-organ.fk\") (read_file \"$S/native-host-instance.fk\") (read_file \"$S/text-tokenize.fk\") (read_file \"$S/rag-embed.fk\") (read_file \"$S/rag-index-codec.fk\") (read_file \"$S/rag-retrieve.fk\") (read_file \"$S/rag-ask.fk\") (read_file \"$S/form-cli-ask.fk\") (read_file \"$S/current-branch-landing.fk\") (read_file \"$S/form-cli.fk\") (read_file \"$S/form-cli-gguf-cell.fk\"))"
BAND="(read_file \"$S/form-cli-repl.fk\")"

# Cached artifacts serve ordinary builds; explicit regeneration walks the
# current Form flattener sources directly through the resident fkwu.
FORM_CLI_SRCS=(
    "$S/core.fk" "$S/line-grammar.fk"
    "$S/str-byte-at.fk" "$S/sha256.fk" "$S/hex.fk"
    "$S/resource-port.fk" "$S/bml-native-interface-package-import.fk" "$S/hati-os-targets.fk"
    "$S/form-native-resource-interfaces.fk" "$S/form-fs.fk" "$S/storage-port.fk"
    "$S/host-kernel-carrier.fk" "$S/fnri-standin.fk" "$S/fnri-receipt.fk"
    "$S/http-client.fk" "$S/format-arith.fk" "$S/f16-decode.fk"
    "$S/q6k-dequant.fk" "$S/q4k-dequant.fk" "$S/weight-load.fk"
    "$S/voice-traits.fk"
    "$S/nearest-shape.fk" "$S/co-learning.fk" "$S/co-learning-stream.fk"
    "$S/mesh-dispatch.fk" "$S/surprise-salience.fk" "$S/host-sense-organ.fk"
    "$S/speech-organ.fk" "$S/native-host-instance.fk"
    "$S/text-tokenize.fk" "$S/rag-embed.fk" "$S/rag-index-codec.fk" "$S/rag-retrieve.fk" "$S/rag-ask.fk"
    "$S/form-cli-ask.fk"
    "$S/current-branch-landing.fk"
    "$S/form-cli.fk"
    "$S/form-cli-gguf-cell.fk"
    "$S/form-cli-repl.fk"
)
# A warmed platform binary is only current when both the Form program and the
# Form-owned compiler path that flattened/emitted it are current.  Without
# these inputs a walker-semantics repair can leave a stale binary looking fresh.
FORM_CLI_BUILD_INPUTS=(
    "${FORM_CLI_SRCS[@]}"
    "$S/fourth-shim.fk"
    "$S/minimal-surface.fk" "$S/hati-os-kernel.fk"
    "$S/host-io-fs-fkwu-emit.fk" "$S/fkc-table-serialize.fk" "$S/hati-os-kernel-emit.fk"
    "$S/form-parse.fk" "$S/bmf-core.fk" "$S/bmf-grammar.fk"
    "$S/host-effect-grammar.fk" "$S/form-flatten.fk"
    "$S/file-byte-window.fk" "$S/bmf-byte-cursor.fk"
    "build-form-cli.sh"
)
# shellcheck source=scripts/fourth-arm.sh
source scripts/fourth-arm.sh
slug="$(fourth_platform_slug)"
FORM_CLI_TABLE_FKWU="${FORM_CLI_TABLE_FKWU:-$S/bootstrap/fkwu-${slug}}"
CLI_BOOTSTRAP_BIN="$S/bootstrap/form-cli-${slug}"
CLI_BOOTSTRAP_BIN_STAMP="$S/bootstrap/form-cli-${slug}.stamp"
stamp="$(fourth_fkwu_cache_stamp)"
cached_fkwu="$FOURTH_DIR/fkwu-$stamp"
if [[ "$FORM_CLI_REGEN" == 1 && -x "$FORM_CLI_TABLE_FKWU" ]]; then
    FKWU="$FORM_CLI_TABLE_FKWU"
elif [[ -x "$cached_fkwu" ]]; then
    FKWU="$cached_fkwu"
fi
if [[ -z "${FKWU:-}" ]]; then
    if [[ "${FORM_STANDARD_LANE:-0}" == 1 ]]; then
        build_fourth
    else
        build_fourth >/dev/null 2>&1 || true
    fi
fi

want_cli_stamp="$(fourth_hash16 "${FORM_CLI_BUILD_INPUTS[@]}")"

# Maintainer regeneration is Form + shell only: the resident fkwu executes the
# current Form flattener and emitter sources to produce the combined C carrier.
# The generated C is an artifact; walker meaning remains in
# fkc-table-serialize.fk and the other Form cells above.
if [[ "$FORM_CLI_REGEN" == 1 ]]; then
    [[ "${FORM_STANDARD_LANE:-0}" != 1 ]] || {
        echo "FORM_CLI_REGEN=1 cannot use the standard no-build lane" >&2
        exit 1
    }
    [[ -x "$FORM_CLI_FKWU" ]] || {
        echo "FORM_CLI_REGEN=1 requires a resident direct-source fkwu" >&2
        exit 1
    }
    : > "$W/universal-emitter.fk"
    sed '/^; preludes:/d' "$S/core.fk" >> "$W/universal-emitter.fk"
    # bp is referenced only inside unused structural helpers while this cell
    # emits C text; keep it parseable without importing the ontology table.
    printf '\n(defn bp (name) (intern_trivial_string name))\n' >> "$W/universal-emitter.fk"
    for emitter_source in $EMIT_CHAIN; do
        sed '/^; preludes:/d' "$emitter_source" >> "$W/universal-emitter.fk"
        printf '\n' >> "$W/universal-emitter.fk"
    done
    printf '(print_str (fkc-emit-universal))\n' >> "$W/universal-emitter.fk"
    "$FORM_CLI_FKWU" --src "$W/universal-emitter.fk" > "$W/fkwu-table.with-root"
    sed '$d' "$W/fkwu-table.with-root" > "$W/fkwu-table.c"
    grep -q 'slot109' "$W/fkwu-table.c" || {
        echo "Form universal emitter did not carry rooted let semantics ($(wc -c < "$W/fkwu-table.c") bytes)" >&2
        exit 1
    }
    : > "$W/direct-flatten.fk"
    for flatten_source in $DIRECT_FLAT_CHAIN; do
        sed '/^; preludes:/d' "$flatten_source" >> "$W/direct-flatten.fk"
        printf '\n' >> "$W/direct-flatten.fk"
    done
    printf '(defn bp (name) (intern_trivial_string name))\n' >> "$W/direct-flatten.fk"
    printf '(print_str (fks-table-file (flt-band-sources-fns %s %s) (flt-band-sources-pool %s %s)))\n' \
        "$MODS" "$BAND" "$MODS" "$BAND" >> "$W/direct-flatten.fk"
    "$FORM_CLI_FKWU" --src "$W/direct-flatten.fk" > "$W/form-cli-table.with-root"
    sed '$d' "$W/form-cli-table.with-root" > "$W/form-cli-table.txt"
    [[ -s "$W/form-cli-table.txt" ]] || {
        echo "Form self-host flatten produced no form-cli table" >&2
        exit 1
    }
    tr -d '\r\n' < "$W/form-cli-table.txt" > "$W/form-cli-table-one-line.txt"
    : > "$W/form-cli-emitter.fk"
    sed '/^; preludes:/d' "$S/core.fk" >> "$W/form-cli-emitter.fk"
    printf '\n(defn bp (name) (intern_trivial_string name))\n' >> "$W/form-cli-emitter.fk"
    for emitter_source in $EMIT_CHAIN; do
        sed '/^; preludes:/d' "$emitter_source" >> "$W/form-cli-emitter.fk"
        printf '\n' >> "$W/form-cli-emitter.fk"
    done
    printf '(print_str (fkc-emit-combined-repl (read_file "%s")))\n' "$W/form-cli-table-one-line.txt" >> "$W/form-cli-emitter.fk"
    "$FORM_CLI_FKWU" --src "$W/form-cli-emitter.fk" > "$W/form-cli-emitted.with-root"
    sed '$d' "$W/form-cli-emitted.with-root" > "$W/form-cli-emitted.c"
    grep -q 'static const char fk_prog' "$W/form-cli-emitted.c" || {
        echo "Form emitter produced no baked form-cli program" >&2
        exit 1
    }
    grep -q 'slot109' "$W/form-cli-emitted.c" || {
        echo "Form combined emitter did not carry rooted let semantics" >&2
        exit 1
    }
    cp "$W/form-cli-table.txt" "$CLI_BOOTSTRAP_TABLE.tmp"
    mv -f "$CLI_BOOTSTRAP_TABLE.tmp" "$CLI_BOOTSTRAP_TABLE"
    cp "$W/form-cli-emitted.c" "$CLI_BOOTSTRAP_C.tmp"
    mv -f "$CLI_BOOTSTRAP_C.tmp" "$CLI_BOOTSTRAP_C"
    printf '%s\n' "$want_cli_stamp" > "$CLI_BOOTSTRAP_STAMP.tmp"
    mv -f "$CLI_BOOTSTRAP_STAMP.tmp" "$CLI_BOOTSTRAP_STAMP"
    echo "  regen: Form-native table and emitted C" >&2
fi

# Standard lane: copy committed platform binary when stamp matches (no clang).
if [[ "${FORM_STANDARD_LANE:-0}" == 1 ]]; then
    got_cli_boot="$(cat "$CLI_BOOTSTRAP_BIN_STAMP" 2>/dev/null || true)"
    if [[ -x "$CLI_BOOTSTRAP_BIN" && "$got_cli_boot" == "$want_cli_stamp" ]]; then
        cp "$CLI_BOOTSTRAP_BIN" "$OUT"
        chmod +x "$OUT"
        echo "standard lane: $OUT from bootstrap/${slug} (no clang)" >&2
        exit 0
    fi
    echo "standard lane: bootstrap form-cli-${slug} missing or stale" >&2
    exit 1
fi

# Warm path: copy platform binary before invoking clang when available.
got_cli_boot="$(cat "$CLI_BOOTSTRAP_BIN_STAMP" 2>/dev/null || true)"
if [[ "$FORM_CLI_FORCE_LINK" != 1 && -z "$FORM_CLI_EXTRA_SRC" && -x "$CLI_BOOTSTRAP_BIN" && "$got_cli_boot" == "$want_cli_stamp" ]]; then
    cp "$CLI_BOOTSTRAP_BIN" "$OUT"
    chmod +x "$OUT"
    echo "  link: bootstrap form-cli-${slug} (no clang)" >&2
    exit 0
fi

# 1. flatten form-cli-repl into its program table (string pool rides behind it).
if [[ -s "$S/bootstrap/form-cli-table.txt" && "$(cat "$CLI_BOOTSTRAP_STAMP" 2>/dev/null)" == "$want_cli_stamp" ]]; then
    cp "$S/bootstrap/form-cli-table.txt" "$W/table.txt"
    echo "  flatten: bootstrap table (no Go)" >&2
elif fourth_selfhost && fourth_flatten_sources form-cli-build fks "$W/table.txt" "${FORM_CLI_SRCS[@]}"; then
    echo "  flatten: fkwu self-host (no Go)" >&2
else
    echo "  flatten: unavailable — need bootstrap/form-cli-table.txt (maintainer: FORM_CLI_REGEN=1 ./build-form-cli.sh)" >&2
    exit 1
fi
[[ -s "$W/table.txt" ]] || { echo "flatten produced no table"; exit 1; }

# 2. emit the combined walker with the table baked in (fk_prog).
if [[ -s "$CLI_BOOTSTRAP_C" && "$(cat "$CLI_BOOTSTRAP_STAMP" 2>/dev/null)" == "$want_cli_stamp" ]]; then
    cp "$CLI_BOOTSTRAP_C" "$W/form-cli.c"
    echo "  emit: bootstrap (no Go)" >&2
else
    echo "  emit: unavailable — need bootstrap/form-cli-emitted.c (maintainer: FORM_CLI_REGEN=1 ./build-form-cli.sh)" >&2
    exit 1
fi
grep -q fk_prog "$W/form-cli.c" || { echo "emit missing baked program"; exit 1; }

# 3. bake the GENESIS — this binary's own Form source — so 'form-cli source' can
#    print it and you can rebuild from the binary alone. It's the file-marked
#    concatenation of every recipe the build reads plus this script, appended as a
#    byte array (escape-free) and read at runtime by self_source (walker tag 117).
SOURCES="minimal-surface hati-os-kernel fkc-table-serialize hati-os-kernel-emit core form-parse bmf-core bmf-grammar host-effect-grammar form-flatten file-byte-window bmf-byte-cursor fourth-shim resource-port bml-native-interface-package-import hati-os-targets form-native-resource-interfaces form-fs storage-port host-kernel-carrier fnri-standin fnri-receipt line-grammar str-byte-at sha256 hex format-arith f16-decode q6k-dequant q4k-dequant weight-load voice-traits nearest-shape co-learning co-learning-stream mesh-dispatch surprise-salience host-sense-organ speech-organ native-host-instance text-tokenize rag-embed rag-index-codec rag-retrieve rag-ask form-cli-ask current-branch-landing form-cli form-cli-gguf-cell form-cli-main form-cli-repl"
{
  for f in $SOURCES; do printf ';;;; ==== FILE: %s/%s.fk ====\n' "$S" "$f"; cat "$S/$f.fk"; done
  printf ';;;; ==== FILE: build-form-cli.sh ====\n'; cat "$(basename "$0")"
} > "$W/genesis.txt"
GEN_LEN=$(wc -c < "$W/genesis.txt" | tr -d ' ')
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
  # shellcheck disable=SC2206
  extra_srcs=($FORM_CLI_EXTRA_SRC)
  clang_args+=("${extra_srcs[@]}")
fi
if [[ -n "$FORM_CLI_EXTRA_LDFLAGS" ]]; then
  # shellcheck disable=SC2206
  extra_ldflags=($FORM_CLI_EXTRA_LDFLAGS)
  clang_args+=("${extra_ldflags[@]}")
fi
if is_windows_host; then
  patch_windows_emitted_c "$W/form-cli.c"
  clang_args+=(-lws2_32 -llegacy_stdio_definitions)
fi
"$CC_BIN" "${clang_args[@]}"
echo "built $OUT  ($(wc -c < "$OUT") bytes, self-contained — runs with no Go/clang/table; carries ${GEN_LEN}B of its own source)"

if [[ "$FORM_CLI_REGEN" == 1 && -z "$FORM_CLI_EXTRA_SRC" && -z "$FORM_CLI_EXTRA_LDFLAGS" ]]; then
    gate_dir="$W/rag-direct-gate"
    mkdir -p "$gate_dir/.coherence-network/rag-index"
    printf '%s\n' '{"id":"fixture/native-rag-hit","snippet":"rsi-fixture-needle","vec":[]}' \
        > "$gate_dir/.coherence-network/rag-index/index.jsonl"
    printf '%s\n' \
        'grounded:fixture/native-rag-hit' \
        'local-lane:fkwu-rag-grounded' \
        'synthesis-lane:fkwu-rag-grounded' > "$W/rag-direct-expected"
    out_abs="$(cd "$(dirname "$OUT")" && pwd)/$(basename "$OUT")"
    (
        cd "$gate_dir"
        printf 'ask rsi-fixture-needle\nquit\n' | "$out_abs"
    ) > "$W/rag-direct-observed"
    if ! cmp -s "$W/rag-direct-expected" "$W/rag-direct-observed"; then
        echo "regenerated form-cli failed the direct native RAG gate" >&2
        cat "$W/rag-direct-observed" >&2
        exit 1
    fi
    echo "  gate: direct native RAG hit" >&2
    cp "$OUT" "$CLI_BOOTSTRAP_BIN.tmp"
    chmod +x "$CLI_BOOTSTRAP_BIN.tmp"
    mv -f "$CLI_BOOTSTRAP_BIN.tmp" "$CLI_BOOTSTRAP_BIN"
    printf '%s\n' "$want_cli_stamp" > "$CLI_BOOTSTRAP_BIN_STAMP.tmp"
    mv -f "$CLI_BOOTSTRAP_BIN_STAMP.tmp" "$CLI_BOOTSTRAP_BIN_STAMP"
    echo "  regen: bootstrap form-cli-${slug}" >&2
fi
