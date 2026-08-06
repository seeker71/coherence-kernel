#!/usr/bin/env bash
# fourth-arm.sh — the emitted fourth kernel as a validate.sh leg.
#
# Sourced by validate.sh (cwd = form/). The fourth sibling is the universal
# walker binary (fkwu) whose C source is emitted entirely by Form recipes
# (fourth-walker-emit.fk, fkc-emit-universal). Bands listed in
# fourth-arm-bands.txt — each one already gated four-way by
# scripts/hati_os_kernel_audit.sh — run on it as a fourth leg: the band's
# UNMODIFIED source is flattened once into a node-table file (the
# pre-compiled artifact), cached by content, and the binary answers in
# milliseconds. A band's wall time stays max(legs); the fourth witness is
# effectively free.
#
# An unavailable optional toolchain can leave the fourth sibling absent. Once
# the committed bootstrap and manifest are present, every declared fourth-arm
# workload is mandatory: preparation, execution, and agreement failures fail
# validation instead of silently reducing the proof to three siblings.

# Bash-only: the array loops below index from 0. Sourced into zsh (arrays
# 1-based) they SILENTLY malform every flatten expr — ${srcs[0]} reads empty
# so a (read_file "") row rides the module list, and ${srcs[last]} grabs the
# wrong band file. Die loudly instead of authoring a malformed carrier.
if [[ -n "${ZSH_VERSION:-}" ]]; then
    echo "fourth-arm.sh: bash-only (zsh arrays are 1-indexed; flatten exprs would be silently malformed) — source this under bash" >&2
    return 1 2>/dev/null || exit 1
fi

FOURTH_DIR="form-stdlib/.cache/fourth"
FOURTH_MANIFEST="fourth-arm-bands.txt"
FOURTH_INDEX="$FOURTH_DIR/table-index-v2.tsv"
# The emitter chain: every file whose content shapes either the fkwu binary
# or a flattened table. Cache keys hash these so a flattener or walker
# change rebuilds exactly what it touches.
FOURTH_CHAIN=(
    form-stdlib/minimal-surface.fk
    form-stdlib/hati-os-kernel.fk
    form-stdlib/host-io-fs-fkwu-emit.fk
    form-stdlib/fkc-table-serialize.fk
    form-stdlib/hati-os-kernel-emit.fk
    form-stdlib/core.fk
    form-stdlib/form-parse.fk
    form-stdlib/bmf-core.fk
    form-stdlib/bmf-grammar.fk
    form-stdlib/host-effect-grammar.fk
    form-stdlib/form-flatten.fk
    form-stdlib/fourth-shim.fk
)
# Lighter chain for bootstrapping T_flat only: the flattener + table serializers,
# without hati-os-kernel-emit.fk's C-emission literals. fkwu self-host flatten
# reads these module paths via read_file; parsing the full emit file bus-errors
# fkwu's fixed arena. Rebuild T_flat with this chain (bin-go once, then fkwu).
FOURTH_FLATTEN_CHAIN=(
    form-stdlib/minimal-surface.fk
    form-stdlib/hati-os-kernel.fk
    form-stdlib/fkc-table-serialize.fk
    form-stdlib/core.fk
    form-stdlib/form-parse.fk
    form-stdlib/bmf-core.fk
    form-stdlib/bmf-grammar.fk
    form-stdlib/host-effect-grammar.fk
    form-stdlib/form-flatten.fk
    form-stdlib/fourth-shim.fk
)
# the shim rides every flatten as the FIRST source: core vocabulary and the
# string stones resolve as ordinary function rows; band defns shadow it
FOURTH_SHIM="form-stdlib/fourth-shim.fk"
FKWU=""
# Emitter chain (walker C emission) — lighter than FOURTH_CHAIN; committed as bootstrap/fkwu-uni.c
FOURTH_EMIT_CHAIN=(
    form-stdlib/minimal-surface.fk
    form-stdlib/hati-os-kernel.fk
    form-stdlib/host-io-fs-fkwu-emit.fk
    form-stdlib/fkc-table-serialize.fk
    form-stdlib/hati-os-kernel-emit.fk
)
FOURTH_BOOTSTRAP_UNI_C="form-stdlib/bootstrap/fkwu-uni.c"
FOURTH_BOOTSTRAP_UNI_STAMP="form-stdlib/bootstrap/fkwu-uni.stamp"

# T_flat — the flattener (form-flatten.fk) flattened once over FOURTH_FLATTEN_CHAIN
# into a committed bootstrap table. fkwu walks it to flatten every band, so bin-go
# leaves the per-band flatten path. The driver (fourth-flatten-driver.fk) reads a batch
# request on stdin and prints marker-framed tables — the same ==T-<stem>== /
# ==T-END== framing the Go path produced, so fourth_run_chunk splits the stream
# unchanged. Rebuilt when form-flatten.fk or fkc-table-serialize.fk changes via
# scripts/regen_t_flat.sh (bin-go bootstrap, fks table — not fkc; thereafter fkwu
# self-host). The trailing fn-0 value + arm profile fkwu prints after
# ==T-END== falls outside every per-band marker range, so the split ignores it.
FOURTH_FLATTEN_TABLE="form-stdlib/fourth-flatten-table.txt"
# T_flat is a compiler workload: large dependency closures recurse much more
# deeply than an already-flattened runtime table. The emitted walker owns an
# explicit stack-size door, so give this phase the measured stack that carries
# the largest manifest band instead of relying on the 256 MiB runtime default.
FOURTH_FLATTEN_STACK_MB="${FOURTH_FLATTEN_STACK_MB:-2048}"

# BML reaches the fourth-arm flattener through executable Form text.  The
# primary source compiler emits a durable .fkb image + loader; the final module
# in this dedicated chain is a late text lens for consumers that flatten source.
FOURTH_SOURCE_TEXT_DIR="form-stdlib/.cache/fourth-source-text"
FOURTH_SOURCE_COMPILER_CHAIN=(
    form-stdlib/form-ontology-loader.fk
    form-stdlib/line-grammar.fk
    form-stdlib/bmf-core.fk
    form-stdlib/bmf-grammar.fk
    form-stdlib/bml.fk
    form-stdlib/bml-source.fk
    form-stdlib/source-compiler.fk
    form-stdlib/grammars/form-bml.fk
    form-stdlib/form-bml-lower.fk
    form-stdlib/source-compiler-text-lens.fk
)

fourth_available() { [[ -n "$FKWU" && -x "$FKWU" ]]; }

# fourth_selfhost — true when the committed flattener table is present, so the
# fourth arm flattens its own band tables on fkwu.  The Windows build patch puts
# stdin/stdout and every read-only file door in binary mode, so the same T_flat
# and source bytes cross macOS, Linux, and Windows without CRLF translation.
fourth_selfhost() {
    [[ -s "$FOURTH_FLATTEN_TABLE" && -n "$FKWU" && -x "$FKWU" ]]
}

# fourth_band_request — emit one band's request block for the flatten driver:
#   stem \n kind \n nmod \n <mod path>*nmod \n <band path>
# Modules are the shim (always first) then every non-band source in order; the
# last source is the band — matching fourth_flatten_expr's module/band split.
fourth_band_request() {
    local stem="$1" kind="$2"; shift 2
    local srcs=("$@") count last band i
    count="${#srcs[@]}"
    last=$((count - 1))
    band="${srcs[$last]}"
    local mods=("$FOURTH_SHIM")
    for ((i = 0; i < last; i++)); do mods+=("${srcs[$i]}"); done
    printf '%s\n%s\n%s\n' "$stem" "$kind" "${#mods[@]}"
    printf '%s\n' "${mods[@]}"
    printf '%s\n' "$band"
}

# Portable content hash for cache stamps. macOS ships `shasum`; Linux and Git
# Bash ship `sha256sum`; they don't overlap. The value is only ever a per-host
# cache key, so the algorithm is free — only availability matters.
_fourth_hash_stdin() {
    # Strip CR so cache stamps match across LF checkouts (mac/linux) and CRLF
    # working trees (Windows Git Bash autocrlf).
    if command -v shasum >/dev/null 2>&1 && printf test | shasum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | shasum | cut -c1-16
    elif command -v sha1sum >/dev/null 2>&1 && printf test | sha1sum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | sha1sum | cut -c1-16
    elif command -v sha256sum >/dev/null 2>&1 && printf test | sha256sum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | sha256sum | cut -c1-16
    elif command -v cksum >/dev/null 2>&1 && printf test | cksum >/dev/null 2>&1; then
        LC_ALL=C tr -d '\r' | cksum | cut -c1-16
    else
        echo "fourth-arm.sh: need shasum, sha1sum, sha256sum, or cksum for cache keys" >&2
        return 1
    fi
}

fourth_hash16() {
    cat "$@" 2>/dev/null | _fourth_hash_stdin
}

# The flattener generation is common to every band. Hash it once per prepare
# pass, then mix that digest with only the band's sources. The old key reread
# the entire compiler chain for every manifest row.
fourth_flatten_generation() {
    fourth_hash16 "${FOURTH_FLATTEN_CHAIN[@]}" "$FOURTH_FLATTEN_TABLE"
}

fourth_table_key() {
    local generation="${FOURTH_FLATTEN_GENERATION:-}"
    [[ -n "$generation" ]] || generation="$(fourth_flatten_generation)"
    {
        printf 'fourth-table-v2\n%s\n' "$generation"
        cat "$@" 2>/dev/null
    } | _fourth_hash_stdin
}

# Executables are byte artifacts, so their cache identity preserves every byte
# (including 0x0d).  Text stamps above intentionally normalize checkout CRLF.
fourth_raw_hash16() {
    if command -v shasum >/dev/null 2>&1 && printf test | shasum >/dev/null 2>&1; then
        cat "$@" 2>/dev/null | shasum | cut -c1-16
    elif command -v sha1sum >/dev/null 2>&1 && printf test | sha1sum >/dev/null 2>&1; then
        cat "$@" 2>/dev/null | sha1sum | cut -c1-16
    elif command -v sha256sum >/dev/null 2>&1 && printf test | sha256sum >/dev/null 2>&1; then
        cat "$@" 2>/dev/null | sha256sum | cut -c1-16
    elif command -v cksum >/dev/null 2>&1 && printf test | cksum >/dev/null 2>&1; then
        cat "$@" 2>/dev/null | cksum | cut -c1-16
    else
        echo "fourth-arm.sh: need shasum, sha1sum, sha256sum, or cksum for cache keys" >&2
        return 1
    fi
}

# One generation seal covers every source that can shape a fourth-arm table,
# the self-hosting flattener image, this preparation logic, and the exact Go
# proof sibling used by the BML source-text lens. Shared source bytes are read
# once per validation rather than once per manifest row.
fourth_validation_generation() {
    {
        local f
        for f in scripts/fourth-arm.sh "$FOURTH_MANIFEST" "$FOURTH_FLATTEN_TABLE" "${GO_BIN:-}"; do
            [[ -n "$f" && -f "$f" ]] || continue
            printf 'file:%s\n' "$f"
            cat "$f"
        done
        find form-stdlib -path "$FOURTH_DIR" -prune -o -type f \
            \( -name '*.fk' -o -name '*.bml' -o -name '*.form' -o -name '*.grammar' \) -print \
            | LC_ALL=C sort \
            | while IFS= read -r f; do printf 'file:%s\n' "$f"; cat "$f"; done
    } | _fourth_hash_stdin
}

fourth_index_valid() {
    if [[ ! -s "$FOURTH_INDEX" ]]; then
        echo "fourth arm: table index missing" >&2
        return 1
    fi
    local label sealed actual expected rows unique stem out digest name
    IFS=$'\t' read -r label sealed < "$FOURTH_INDEX"
    if [[ "$label" != generation || -z "$sealed" ]]; then
        echo "fourth arm: malformed table index header" >&2
        return 1
    fi
    actual="$(fourth_validation_generation)"
    if [[ "$actual" != "$sealed" ]]; then
        echo "fourth arm: table index generation changed ($sealed -> $actual)" >&2
        return 1
    fi
    expected="$(awk '!/^#/ && NF {n++} END {print n + 0}' "$FOURTH_MANIFEST")"
    rows="$(awk 'NR > 1 && NF {n++} END {print n + 0}' "$FOURTH_INDEX")"
    unique="$(awk -F '\t' 'NR > 1 && NF {print $1}' "$FOURTH_INDEX" | LC_ALL=C sort -u | wc -l | tr -d ' ')"
    if [[ "$rows" != "$expected" || "$unique" != "$expected" ]]; then
        echo "fourth arm: table index coverage mismatch (expected=$expected rows=$rows unique=$unique)" >&2
        return 1
    fi
    while IFS=$'\t' read -r stem out digest; do
        name="$(basename "$out")"
        if [[ -z "$stem" || ! "$name" =~ ^t-${stem}-[0-9a-f]{16}\.txt$ || ! -s "$out" ]]; then
            echo "fourth arm: invalid indexed table path for $stem: $out" >&2
            return 1
        fi
        if [[ "$(fourth_raw_hash16 "$out")" != "$digest" ]]; then
            echo "fourth arm: indexed table digest changed for $stem" >&2
            return 1
        fi
    done < <(sed -n '2,$p' "$FOURTH_INDEX")
}

fourth_publish_index() {
    local plan="$1" generation tmp stem out
    generation="$(fourth_validation_generation)"
    tmp="$FOURTH_INDEX.tmp"
    printf 'generation\t%s\n' "$generation" > "$tmp"
    while IFS=$'\t' read -r stem out; do
        if [[ -z "$stem" || ! -s "$out" ]]; then
            echo "fourth arm: cannot seal missing table for $stem" >&2
            rm -f "$tmp"
            return 1
        fi
        printf '%s\t%s\t%s\n' "$stem" "$out" "$(fourth_raw_hash16 "$out")" >> "$tmp"
    done < "$plan"
    mv -f "$tmp" "$FOURTH_INDEX"
    fourth_index_valid
}

# Recovery is intentionally explicit: it seals only a complete generation in
# which every manifest stem has exactly one table newer than T_flat. It is used
# after an interrupted cold build; ordinary validation never guesses this way.
fourth_recover_fresh_index() {
    local plan
    plan="$(mktemp "${TMPDIR:-/tmp}/form-fourth-index.XXXXXX")"
    if ! awk '
        NR == FNR {
            if ($0 !~ /^#/ && NF) { order[++n] = $1; wanted[$1] = 1 }
            next
        }
        {
            path = $0
            name = path
            sub(/^.*\//, "", name)
            sub(/\.txt$/, "", name)
            if (substr(name, 1, 2) != "t-" || length(name) < 20) next
            key = substr(name, length(name) - 15)
            stem = substr(name, 3, length(name) - 19)
            if (key !~ /^[0-9a-f]+$/ || length(key) != 16 || !wanted[stem]) next
            count[stem]++
            table[stem] = path
        }
        END {
            bad = 0
            for (i = 1; i <= n; i++) {
                stem = order[i]
                if (count[stem] != 1) {
                    print "fourth arm: expected one fresh table for " stem ", found " (count[stem] + 0) > "/dev/stderr"
                    bad = 1
                } else {
                    print stem "\t" table[stem]
                }
            }
            exit bad
        }
    ' "$FOURTH_MANIFEST" <(
        find "$FOURTH_DIR" -maxdepth 1 -name 't-*.txt' -newer "$FOURTH_FLATTEN_TABLE" -print
    ) > "$plan"; then
        rm -f "$plan"
        return 1
    fi
    fourth_publish_index "$plan"
    local rc=$?
    rm -f "$plan"
    return "$rc"
}

# fourth_prepare_source_text — compile one BML-bearing source through the
# explicit source-text lens.  Content + compiler + proof-sibling bytes key the
# cache, so a compiler or kernel change cannot reuse an older lowering.
fourth_prepare_source_text() {
    local src="$1" key cached out driver
    [[ -n "${GO_BIN:-}" && -x "${GO_BIN:-}" ]] || return 0
    mkdir -p "$FOURTH_SOURCE_TEXT_DIR"
    key="$(fourth_hash16 "$src" "${FOURTH_SOURCE_COMPILER_CHAIN[@]}")-$(fourth_raw_hash16 "$GO_BIN")"
    cached="$FOURTH_SOURCE_TEXT_DIR/$key.fk"
    if [[ ! -s "$cached" ]]; then
        out="$(mktemp "$FOURTH_SOURCE_TEXT_DIR/.${key}.out.XXXXXX")"
        driver="$(mktemp "$FOURTH_SOURCE_TEXT_DIR/.${key}.driver.XXXXXX")"
        printf '(do (form-source-compile-file "%s" "%s"))\n' "$src" "$out" > "$driver"
        if "$GO_BIN" "${FOURTH_SOURCE_COMPILER_CHAIN[@]}" "$driver" >/dev/null 2>&1 \
            && [[ -s "$out" ]]; then
            mv -f "$out" "$cached"
        else
            rm -f "$out" "$driver"
            return 0
        fi
        rm -f "$out" "$driver"
    fi
    [[ -s "$cached" ]] && printf '%s\n' "$cached"
}

# fourth_fkwu_cache_stamp — cache key for the standing fkwu binary (emitter chain + committed uni.c).
fourth_fkwu_cache_stamp() {
    fourth_hash16 "${FOURTH_CHAIN[@]}" "$FOURTH_BOOTSTRAP_UNI_C"
}

# fourth_emit_chain_stamp — hash of emitter sources; must match bootstrap/fkwu-uni.stamp.
fourth_emit_chain_stamp() {
    fourth_hash16 "${FOURTH_EMIT_CHAIN[@]}"
}

# fourth_platform_slug — darwin-arm64, linux-amd64, … for committed bootstrap binaries.
fourth_platform_slug() {
    local os arch
    os="$(uname -s 2>/dev/null | tr '[:upper:]' '[:lower:]')"
    arch="$(uname -m 2>/dev/null)"
    case "$arch" in
        x86_64|amd64) arch="amd64" ;;
        aarch64|arm64) arch="arm64" ;;
    esac
    case "$os" in
        darwin) printf 'darwin-%s' "$arch" ;;
        linux) printf 'linux-%s' "$arch" ;;
        mingw*|msys*|cygwin*) printf 'windows-%s' "$arch" ;;
        *) printf '%s-%s' "$os" "$arch" ;;
    esac
}

# fourth_patch_windows_emitted_c — Windows host patches for emitted walker C.
fourth_patch_windows_emitted_c() {
    local c_file="$1"
    sed -i '1i #define _CRT_SECURE_NO_WARNINGS 1' "$c_file"
    sed -i 's|extern unsigned int arc4random(void);|extern int rand(void); static unsigned int arc4random(void) { return (unsigned int)rand(); }|' "$c_file"
    sed -i 's|extern long long read(int, void \*, unsigned long);|extern int read(int, void *, unsigned int);|' "$c_file"
    sed -i 's|extern long long write(long long, const void \*, unsigned long);|extern int write(int, const void *, unsigned int);|' "$c_file"
    sed -i 's|mkdir(d, 0777)|mkdir(d)|g; s|mkdir(p, 0777)|mkdir(p)|g' "$c_file"
    sed -i 's|extern int sprintf(char \*, const char \*, ...);|typedef __builtin_va_list fk_va_list; extern int vsnprintf(char *, unsigned long long, const char *, fk_va_list); static int sprintf(char *b, const char *fmt, ...) { fk_va_list ap; __builtin_va_start(ap, fmt); int n = vsnprintf(b, 4096ULL, fmt, ap); __builtin_va_end(ap); return n; }|' "$c_file"
    sed -i 's|struct timeval { long tv_sec; int tv_usec; }; extern int gettimeofday(struct timeval \*, void \*);|struct timeval { long tv_sec; int tv_usec; }; struct fk_filetime { unsigned int dwLowDateTime; unsigned int dwHighDateTime; }; __declspec(dllimport) void __stdcall GetSystemTimeAsFileTime(struct fk_filetime *); static int gettimeofday(struct timeval *tv, void *tz) { (void)tz; struct fk_filetime ft; unsigned long long ticks; unsigned long long us; GetSystemTimeAsFileTime(\&ft); ticks = ((unsigned long long)ft.dwHighDateTime * 4294967296ULL) + (unsigned long long)ft.dwLowDateTime; us = (ticks / 10ULL) - 11644473600000000ULL; tv->tv_sec = (long)(us / 1000000ULL); tv->tv_usec = (int)(us % 1000000ULL); return 0; }|' "$c_file"
    sed -i 's|extern void \*dlopen(const char \*, int); extern void \*dlsym(void \*, const char \*);|static void *dlopen(const char *p, int f) { (void)p; (void)f; return 0; } static void *dlsym(void *h, const char *s) { (void)h; (void)s; return 0; }|' "$c_file"
    sed -i 's|getaddrinfo(host, port, \&hints, \&res)|fk_sock_getaddrinfo(host, port, \&hints, \&res)|g; s|fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)|fd = fk_sock_socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)|g; s|connect(fd, rp->ai_addr, rp->ai_addrlen)|fk_sock_connect(fd, rp->ai_addr, rp->ai_addrlen)|g' "$c_file"
    sed -i 's|read(fd, resp + total, 65535 - total)|fk_sock_read(fd, resp + total, 65535 - total)|g; s|write(fd, req + wr, rn - wr)|fk_sock_write(fd, req + wr, rn - wr)|g; s|write(fd, rptr + wr, rlen - wr)|fk_sock_write(fd, rptr + wr, rlen - wr)|g; s|close(fd);|fk_sock_close(fd);|g' "$c_file"
    # UCRT defaults descriptors and open(path, 0) to text mode.  T_flat and
    # source files are byte contracts: preserve CR/LF and 0x1a exactly.
    sed -E -i 's/open\(([^,()]+),[[:space:]]*0\)/open(\1, 0x8000)/g' "$c_file"
    sed -i 's|static int fk_run(int argc, char \*\*argv) {|extern int _setmode(int, int); static int fk_run(int argc, char **argv) { _setmode(0, 0x8000); _setmode(1, 0x8000); _setmode(2, 0x8000);|' "$c_file"
}

# build_fourth — the standing fkwu binary, cached by emitter content.
build_fourth() {
    [[ -f "$FOURTH_MANIFEST" ]] || return 0
    mkdir -p "$FOURTH_DIR"
    local stamp out tmp d is_windows uni_c want got slug boot got_boot
    local -a clang_args
    is_windows=0
    if [[ "${OS:-}" == "Windows_NT" || "${OSTYPE:-}" == msys* || "${OSTYPE:-}" == cygwin* ]]; then
        is_windows=1
    fi
    stamp="$(fourth_fkwu_cache_stamp)"
    # Revision suffix invalidates Windows binaries built before the binary-I/O
    # patch without disturbing proven platform bootstrap stamps elsewhere.
    [[ "$is_windows" -eq 1 ]] && stamp="${stamp}-winbin1"
    out="$FOURTH_DIR/fkwu-$stamp"
    [[ -x "$out" ]] && FKWU="$out" && return 0
    if [[ "${FORM_STANDARD_LANE:-0}" == 1 ]]; then
        slug="$(fourth_platform_slug)"
        boot="form-stdlib/bootstrap/fkwu-${slug}"
        got_boot="$(cat "${boot}.stamp" 2>/dev/null || true)"
        if [[ -x "$boot" && "$got_boot" == "$stamp" ]]; then
            cp "$boot" "$out"
            chmod +x "$out"
            FKWU="$out"
            echo "  standard lane: fkwu from bootstrap/${slug} (no compile)" >&2
            return 0
        fi
        echo "  standard lane: fkwu cache miss at $out (no compile)" >&2
        return 0
    fi
    command -v clang >/dev/null 2>&1 || { echo "  fourth kernel: clang absent — bands run three-kernel only" >&2; return 0; }
    slug="$(fourth_platform_slug)"
    boot="form-stdlib/bootstrap/fkwu-${slug}"
    got_boot="$(cat "${boot}.stamp" 2>/dev/null || true)"
    if [[ -x "$boot" && "$got_boot" == "$stamp" ]]; then
        cp "$boot" "$out"
        chmod +x "$out"
        FKWU="$out"
        echo "  fourth kernel: bootstrap binary ${slug} (no clang)" >&2
        find "$FOURTH_DIR" -maxdepth 1 -name 'fkwu-*' ! -name "$(basename "$out")" -delete 2>/dev/null || true
        return 0
    fi
    uni_c="$FOURTH_BOOTSTRAP_UNI_C"
    want="$(fourth_emit_chain_stamp)"
    got="$(cat "$FOURTH_BOOTSTRAP_UNI_STAMP" 2>/dev/null || true)"
    if [[ ! -s "$uni_c" || "$want" != "$got" ]]; then
        if [[ "${FORM_ALLOW_BOOTSTRAP_EMIT:-0}" == 1 && -n "${GO_BIN:-}" && -x "$GO_BIN" ]]; then
            echo "  emitting bootstrap uni.c via bin-go (maintainer regen)..." >&2
            d="$(mktemp -d "${TMPDIR:-/tmp}/form-fourth-emit.XXXXXX")"
            echo '(fkc-emit-universal)' > "$d/emit.fk"
            "$GO_BIN" "${FOURTH_EMIT_CHAIN[@]}" "$d/emit.fk" > "$uni_c.tmp" 2>"$d/uni.err" || true
            [[ -s "$uni_c.tmp" ]] && mv -f "$uni_c.tmp" "$uni_c"
            printf '%s\n' "$want" > "$FOURTH_BOOTSTRAP_UNI_STAMP"
            rm -rf "$d"
        else
            echo "  bootstrap uni.c missing or stale — run scripts/regen_fkwu_bootstrap.sh" >&2
            return 0
        fi
    fi
    echo "  building fourth kernel (fkwu) from bootstrap uni.c (no Go)..." >&2
    d="$(mktemp -d "${TMPDIR:-/tmp}/form-fourth.XXXXXX")"
    cp "$uni_c" "$d/uni.c"
    if [[ "$is_windows" == "1" ]]; then
        fourth_patch_windows_emitted_c "$d/uni.c"
    fi
    tmp="$(mktemp "$FOURTH_DIR/.fkwu-$stamp.XXXXXX")"
    clang_args=(
        -O2
        -Wno-error=implicit-function-declaration
        -Wno-implicit-function-declaration
        -Wno-incompatible-library-redeclaration
        -o "$tmp" "$d/uni.c"
    )
    if [[ "$is_windows" == "1" ]]; then
        clang_args+=(-lws2_32 -llegacy_stdio_definitions)
        clang_args+=(-Xlinker /STACK:67108864)
    else
        clang_args+=(-pthread)
    fi
    if [[ -s "$d/uni.c" ]] && clang "${clang_args[@]}" 2>"$d/clang.err"; then
        mv -f "$tmp" "$out"
    elif [[ -s "$d/uni.c" && "$is_windows" == "1" ]] \
        && command -v gcc >/dev/null 2>&1 \
        && gcc -O2 -Wno-implicit-function-declaration -Wno-builtin-declaration-mismatch -o "$tmp" "$d/uni.c" -lws2_32 2>"$d/gcc.err"; then
        mv -f "$tmp" "$out"
    else
        rm -f "$tmp"
        if [[ -s "$d/clang.err" ]]; then sed -n '1,12p' "$d/clang.err" >&2; fi
        echo "  fourth kernel build did not land — bands run three-kernel only" >&2
    fi
    rm -rf "$d"
    [[ -x "$out" ]] && FKWU="$out"
    find "$FOURTH_DIR" -maxdepth 1 -name 'fkwu-*' ! -name "$(basename "$out")" -delete 2>/dev/null || true
}

# fourth_band_stem — manifest stem for a band file path, or empty. The fourth
# arm only applies to a CANONICAL band — the workload's last file living under
# form-stdlib/tests/. A same-named sample elsewhere (e.g. a cross-modal demo
# whose basename collides with a manifest stem) never resolves, because its
# table would still be built from form-stdlib/tests/<stem>-band.fk and compared
# against the sample's own three-kernel output — a false divergence. Anchoring
# to the tests/ path keeps the stem the contract for the real band only.
fourth_band_stem() {
    local band="$1" stem
    [[ "$band" == form-stdlib/tests/* || "$band" == */form-stdlib/tests/* ]] || return 0
    stem="$(basename "$band")"
    stem="${stem%.fk}"
    stem="${stem%-band}"
    [[ -f "$FOURTH_MANIFEST" ]] || return 0
    awk -v b="$stem" '$1==b{print $1; exit}' "$FOURTH_MANIFEST"
}

# fourth_band_srcs — the band's source list: every non-core prelude in
# declared order, then the band file itself (same-name convention as the
# fallback when no prelude is declared).
# fourth_band_prelude_mods_raw — every module path from a band's ; preludes: header,
# including continuation lines that start with "; " (multi-line prelude blocks),
# in declared order with core.fk KEPT (the three reference kernels need it; only
# the fourth arm's shim mirrors core). Callers that feed the fourth leg use
# fourth_band_prelude_mods below, which drops core.fk from this list.
# Emits ONLY source-path tokens (ending in .fk/.bml/.form/.grammar); a continuation
# comment line that yields no path token (e.g. "; Verdict 11111: taught skills ...")
# STOPS the block instead of being slurped as bogus prelude paths — otherwise the
# first non-file token makes fourth_prep_srcs bail and silently drop the band file,
# flattening a table with no top-level (check) call → fkwu fn-0 = 0 (a false divergence).
fourth_band_prelude_mods_raw() {
    local band="$1"
    awk '
        function emit_paths(line, strict,   i, n, a, got) {
            n = split(line, a, /[[:space:]]+/)
            got = 0
            if (strict)
                for (i = 1; i <= n; i++)
                    if (a[i] != "" && a[i] != "\\" && a[i] !~ /\.(fk|bml|form|grammar)$/) return 0
            for (i = 1; i <= n; i++)
                if (a[i] ~ /\.(fk|bml|form|grammar)$/) { print a[i]; got = 1 }
            return got
        }
        /^; preludes:/ {
            s = $0; sub(/^; preludes:[[:space:]]*/, "", s)
            emit_paths(s, 0); cont = 1; next
        }
        cont && /^;[[:space:]]/ {
            s = $0; sub(/^;[[:space:]]*/, "", s)
            if (emit_paths(s, 1) == 0) cont = 0
            next
        }
        { cont = 0 }
    ' "$band" 2>/dev/null | grep . || true
}

fourth_band_prelude_mods() {
    fourth_band_prelude_mods_raw "$1" | grep -vE '(^|/)core\.fk$' | grep . || true
}

fourth_band_srcs() {
    local stem="$1" mods band
    band="form-stdlib/tests/${stem}-band.fk"
    # A manifest stem maps to tests/<stem>-band.fk OR the plain tests/<stem>.fk —
    # fourth_band_stem strips -band when reading, so both are the same band.
    # Read the preludes header from whichever file exists, else a stem registered
    # under the plain name silently builds an empty table and runs three-kernel only.
    [[ -f "$band" ]] || band="form-stdlib/tests/${stem}.fk"
    # Drop ONLY the exact core.fk prelude (the shim mirrors it). Anchor the match
    # to a path boundary so sibling-named modules — substrate-core.fk, bmf-core.fk
    # — keep their place in the source list instead of vanishing as substrings.
    mods="$(fourth_band_prelude_mods "$band")"
    [[ -z "$mods" && -f "form-stdlib/${stem}.fk" ]] && mods="form-stdlib/${stem}.fk"
    printf '%s\n' $mods "$band"
}

# fourth_prep_srcs — prepared source paths for a stem, one per line.  A
# BML-dialect file rides the source compiler's explicit text lens so the
# flattener reads executable Form rather than the primary .fkb load driver.
# Empty output means the band degrades honestly to the three proof siblings.
fourth_prep_srcs() {
    local stem="$1" f prepared band_srcs prepared_srcs=()
    # Bash 3.2 retains process-substitution descriptors until the surrounding
    # function returns.  This function is called once for every manifest row,
    # so capture and consume the short path list synchronously instead of
    # spending one descriptor per row on macOS's 256-descriptor default.
    band_srcs="$(fourth_band_srcs "$stem")" || return 1
    while IFS= read -r f; do
        [[ -n "$f" ]] || continue
        if [[ ! -f "$f" && "$f" == form/* && -f "${f#form/}" ]]; then
            f="${f#form/}"
        fi
        [[ -f "$f" ]] || return 1
        if grep -Eq '^[[:space:]]*section \[' "$f"; then
            prepared="$(fourth_prepare_source_text "$f")"
            [[ -n "$prepared" && -s "$prepared" ]] || return 1
            prepared_srcs+=("$prepared")
        else
            prepared_srcs+=("$f")
        fi
    done <<< "$band_srcs"
    [[ "${#prepared_srcs[@]}" -ge 1 ]] || return 1
    printf '%s\n' "${prepared_srcs[@]}"
}

# fourth_flatten_expr — the driver line that flattens a source list through
# the multi-source door (fks carries the string pool; fkc is pool-free).
fourth_flatten_expr() {
    local kind="$1"; shift
    local srcs=("$@") count last band mods=" (read_file \"$FOURTH_SHIM\")" band_read f i
    count="${#srcs[@]}"
    [[ "$count" -gt 0 ]] || return 1
    last=$((count - 1))
    band="${srcs[$last]}"
    for ((i = 0; i < last; i++)); do
        f="${srcs[$i]}"
        mods="$mods (read_file \"$f\")"
    done
    band_read="(read_file \"$band\")"
    if [[ "$kind" == "fks" ]]; then
        printf '(print (fks-table-file (flt-band-sources-fns (list%s) %s) (flt-band-sources-pool (list%s) %s)))\n' "$mods" "$band_read" "$mods" "$band_read"
    else
        printf '(print (fkc-table-file (flt-band-sources-fns (list%s) %s)))\n' "$mods" "$band_read"
    fi
}

# fourth_table — cached flattened node-table for one band (path on stdout).
# Flattens only through fkwu walking the committed T_flat. The old Go fallback
# concatenated the whole fourth chain plus the workload into a giant driver; that
# is the wrong arm now that the actual path is layered image/fkb loading.
# Empty output means the band runs three-kernel only this time.
fourth_table() {
    local stem="$1" kind key out d f prepared_srcs srcs=()
    kind="$(awk -v b="$stem" '$1==b{print $2; exit}' "$FOURTH_MANIFEST")"
    [[ -n "$kind" ]] || return 0
    prepared_srcs="$(fourth_prep_srcs "$stem")" || prepared_srcs=""
    while IFS= read -r f; do [[ -n "$f" ]] && srcs+=("$f"); done <<< "$prepared_srcs"
    if [[ "${#srcs[@]}" -lt 1 ]]; then
        echo "fourth arm: source preparation failed for $stem" >&2
        return 1
    fi
    key="$(fourth_table_key "${srcs[@]}")"
    out="$FOURTH_DIR/t-$stem-$key.txt"
    if [[ ! -s "$out" ]]; then
        if fourth_selfhost; then
            # one-band request → fkwu walks T_flat → marker-framed table; the
            # trailing fn-0 value + arm profile sit past ==T-END==, outside the range.
            { printf '1\n'; fourth_band_request "$stem" "$kind" "${srcs[@]}"; } \
                | FORM_KERNEL_STACK_MB="$FOURTH_FLATTEN_STACK_MB" "$FKWU" "$FOURTH_FLATTEN_TABLE" 0 \
                | sed -n "/^==T-${stem}==\$/,/^==T-END==\$/p" | sed -e '1d' -e '$d' > "$out.tmp"
            local statuses=("${PIPESTATUS[@]}")
            if [[ "${statuses[1]}" -ne 0 || ! -s "$out.tmp" ]]; then
                echo "fourth arm: fkwu failed to produce a table for $stem" >&2
                rm -f "$out.tmp"
                return 1
            fi
            mv -f "$out.tmp" "$out"
        fi
    fi
    [[ -s "$out" ]] && printf '%s\n' "$out"
    return 0
}

# fourth_flatten_sources — flatten an ad-hoc prelude list + band (outside the manifest).
# Only fkwu walking T_flat is allowed here. The former bin-go monolithic fallback
# built giant source drivers and belongs to the retired witness path.
fourth_flatten_sources() {
    local stem="$1" kind="$2" out="$3"
    shift 3
    local srcs=("$@")
    [[ "${#srcs[@]}" -ge 1 ]] || return 1
    if fourth_selfhost; then
        { printf '1\n'; fourth_band_request "$stem" "$kind" "${srcs[@]}"; } \
            | FORM_KERNEL_STACK_MB="$FOURTH_FLATTEN_STACK_MB" "$FKWU" "$FOURTH_FLATTEN_TABLE" 0 \
            | sed -n "/^==T-${stem}==\$/,/^==T-END==\$/p" | sed -e '1d' -e '$d' > "$out.tmp"
        local statuses=("${PIPESTATUS[@]}")
        if [[ "${statuses[1]}" -ne 0 || ! -s "$out.tmp" ]]; then
            echo "fourth arm: fkwu failed to produce ad-hoc table $stem" >&2
            rm -f "$out.tmp"
            return 1
        fi
        mv -f "$out.tmp" "$out"
    fi
    [[ -s "$out" ]]
}

# fourth_table_for_band — the cached table for a band FILE PATH (the last
# workload argument), or empty when the band is outside the manifest.
fourth_table_for_band() {
    local stem
    stem="$(fourth_band_stem "$1")"
    [[ -n "$stem" ]] || return 0
    fourth_table "$stem"
}

# fourth_run_chunk — flatten ONE chunk in a single pass, then split the
# marker-delimited output into per-band tables.
#   driver: self-host → a batch request (nbands + per-band blocks) fkwu walks
#           through T_flat; Go path → FOURTH_CHAIN + (==T-stem==, flatten expr)*
#           + ==T-END==
#   plan:   one "stem<TAB>outpath" line per band, in driver order
# Both paths emit the same ==T-<stem>== / ==T-END== framing, so the split is
# identical (the self-host trailing fn-0 value + arm profile sit past ==T-END==).
# Self-contained (reads only its two files), so it runs safely as a background
# job: every table publishes atomically (mv -f "$cur.tmp" "$cur"), so parallel
# chunks never collide. Every manifest-covered band is part of the four-kernel
# contract: a failed walker or missing table is a hard validation failure.
fourth_run_chunk() {
    local driver="$1" plan="$2" out_all="$1.out"
    if fourth_selfhost; then
        if ! FORM_KERNEL_STACK_MB="$FOURTH_FLATTEN_STACK_MB" \
            "$FKWU" "$FOURTH_FLATTEN_TABLE" 0 < "$driver" > "$out_all"; then
            echo "fourth arm: fkwu failed while flattening:" >&2
            cut -f1 "$plan" | sed 's/^/  - /' >&2
            rm -f "$out_all"
            return 1
        fi
    else
        if ! "$GO_BIN" "$driver" > "$out_all"; then
            echo "fourth arm: Go walker failed while flattening:" >&2
            cut -f1 "$plan" | sed 's/^/  - /' >&2
            rm -f "$out_all"
            return 1
        fi
    fi
    local stems=() outs=() s o
    while IFS=$'\t' read -r s o; do stems+=("$s"); outs+=("$o"); done < "$plan"
    local n="${#stems[@]}" p cur nextmark
    for ((p = 0; p < n; p++)); do
        cur="${outs[$p]}"
        if ((p + 1 < n)); then nextmark="==T-${stems[$((p + 1))]}=="; else nextmark="==T-END=="; fi
        sed -n "/^==T-${stems[$p]}==\$/,/^${nextmark}\$/p" "$out_all" | sed -e '1d' -e '$d' > "$cur.tmp"
        if [[ -s "$cur.tmp" ]]; then
            mv -f "$cur.tmp" "$cur"
        else
            echo "fourth arm: fkwu emitted no table for ${stems[$p]}" >&2
            rm -f "$cur.tmp" "$out_all"
            return 1
        fi
    done
    rm -f "$out_all"
}

# fourth_prepare_all — emit every MISSING manifest table before the suite fans
# out. Each Go walker run re-parses the whole FOURTH_CHAIN, so the cost that
# matters is the NUMBER of walker runs, not the band count. We group the missing
# bands into CHUNKS of $batch_max and flatten each chunk in ONE walker run
# (chain parsed once per chunk) — turning N separate runs (one per band on a
# cold cache) into ceil(N/batch_max). Chunks fan out across cores in waves of
# $jobs with a plain `wait` barrier: no busy-poll, no `wait -n`, holds in bash
# 3.2 (macOS default). Warm runs (missing=0) return before any walker starts.
# fourth_seal_chunk — close a chunk so fourth_run_chunk emits ==T-END==.
# Self-host: prepend the band count so the driver's stdin loop knows the batch
# size. Go path: append the literal end marker.
fourth_seal_chunk() {
    local driver="$1" n="$2" selfhost="$3"
    if [[ "$selfhost" -eq 1 ]]; then
        { printf '%s\n' "$n"; cat "$driver"; } > "$driver.req" && mv -f "$driver.req" "$driver"
    else
        printf '(print "==T-END==")\n' >> "$driver"
    fi
}

fourth_prepare_all() {
    fourth_available || return 0
    [[ -f "$FOURTH_MANIFEST" ]] || return 0
    local workdir stem kind key out missing=0 f prepared_srcs srcs driver plan cidx=0 ccount=0
    local batch_max="${FOURTH_PREPARE_ALL_BATCH_MAX:-1}"
    local selfhost=0; fourth_selfhost && selfhost=1
    if [[ "$selfhost" -ne 1 ]]; then
        echo "  fourth arm: T_flat self-host unavailable — skipping monolithic Go table fallback" >&2
        return 0
    fi
    if fourth_index_valid; then
        return 0
    fi
    FOURTH_FLATTEN_GENERATION="$(fourth_flatten_generation)"
    workdir="$(mktemp -d "${TMPDIR:-/tmp}/form-fourth-all.XXXXXX")"
    local index_plan="$workdir/index.tsv"
    : > "$index_plan"
    while read -r stem kind _; do
        [[ -z "$stem" || "$stem" == \#* ]] && continue
        srcs=()
        prepared_srcs="$(fourth_prep_srcs "$stem")" || prepared_srcs=""
        while IFS= read -r f; do [[ -n "$f" ]] && srcs+=("$f"); done <<< "$prepared_srcs"
        if [[ "${#srcs[@]}" -lt 1 ]]; then
            echo "fourth arm: source preparation failed for $stem" >&2
            rm -rf "$workdir"
            return 1
        fi
        key="$(fourth_table_key "${srcs[@]}")"
        out="$FOURTH_DIR/t-$stem-$key.txt"
        printf '%s\t%s\n' "$stem" "$out" >> "$index_plan"
        [[ -s "$out" ]] && continue
        missing=$((missing + 1))
        if [[ "$ccount" -eq 0 ]]; then        # open a fresh chunk driver + plan
            driver="$workdir/driver-$cidx.fk"; plan="$workdir/plan-$cidx.tsv"
            : > "$driver"
            : > "$plan"
        fi
        fourth_band_request "$stem" "$kind" "${srcs[@]}" >> "$driver"
        printf '%s\t%s\n' "$stem" "$out" >> "$plan"
        ccount=$((ccount + 1))
        if [[ "$ccount" -ge "$batch_max" ]]; then   # seal a full chunk
            fourth_seal_chunk "$driver" "$ccount" "$selfhost"
            cidx=$((cidx + 1)); ccount=0
        fi
    done < "$FOURTH_MANIFEST"
    if [[ "$ccount" -gt 0 ]]; then               # seal the trailing partial chunk
        fourth_seal_chunk "$driver" "$ccount" "$selfhost"
        cidx=$((cidx + 1))
    fi
    if [[ "$missing" -eq 0 ]]; then
        if ! fourth_publish_index "$index_plan"; then
            rm -rf "$workdir"
            return 1
        fi
        rm -rf "$workdir"
        return 0
    fi
    local jobs="${FOURTH_PREPARE_ALL_JOBS:-8}"
    [[ "$jobs" =~ ^[0-9]+$ && "$jobs" -ge 1 ]] || jobs=4
    echo "  flattening $missing band tables for the fourth arm in $cidx walker run(s) across $jobs cores (cold cache; chunk $batch_max)..." >&2
    local k inflight=0 failed=0 pids=()
    for ((k = 0; k < cidx; k++)); do
        fourth_run_chunk "$workdir/driver-$k.fk" "$workdir/plan-$k.tsv" &
        pids+=("$!")
        inflight=$((inflight + 1))
        if [[ "$inflight" -ge "$jobs" ]]; then
            local pid
            for pid in "${pids[@]}"; do wait "$pid" || failed=1; done
            pids=(); inflight=0
            [[ "$failed" -eq 0 ]] || break
        fi
    done
    if [[ "$failed" -eq 0 && "${#pids[@]}" -gt 0 ]]; then
        # ${#pids[@]} guard: when the chunk count is an exact multiple of
        # $jobs, the last wave's reset leaves pids EMPTY here, and bash 3.2
        # under `set -u` treats expanding an empty array as an unbound
        # variable — the suite died at the finish line of an 856-table cold
        # flatten (856 % 8 == 0) before this guard existed.
        local pid
        for pid in "${pids[@]}"; do wait "$pid" || failed=1; done
    fi
    if [[ "$failed" -ne 0 ]]; then
        echo "  fourth arm: parallel wave failed; retrying only missing tables in isolated walkers..." >&2
        failed=0
        local missing_out retry_needed
        for ((k = 0; k < cidx; k++)); do
            retry_needed=0
            while IFS=$'\t' read -r _ missing_out; do
                [[ -s "$missing_out" ]] || retry_needed=1
            done < "$workdir/plan-$k.tsv"
            if [[ "$retry_needed" -eq 1 ]]; then
                fourth_run_chunk "$workdir/driver-$k.fk" "$workdir/plan-$k.tsv" || failed=1
            fi
        done
    fi
    if [[ "$failed" -ne 0 ]] || ! fourth_publish_index "$index_plan"; then
        rm -rf "$workdir"
        return 1
    fi
    rm -rf "$workdir"
    # Compost stale tables from earlier source generations.
    find "$FOURTH_DIR" -maxdepth 1 -name 't-*' -mtime +14 -delete 2>/dev/null || true
}
