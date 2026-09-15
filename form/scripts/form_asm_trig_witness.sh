#!/usr/bin/env bash
# form_asm_trig_witness.sh — execute fam-sin/fam-cos's Form->asm BYTES on the M4, not just the
# recipe. Bytes -> form-macho .o (mo-object-sym) -> `ld -dylib` (ad-hoc signed, zero clang in the
# compute) -> a tiny dlopen+dlsym+call C loader (double f(double), no math of its own — the FFI
# harness only, the exact float generalization of jit_inram_darwin_arm64.go's form_dylib_call) ->
# compared against libm sin/cos. The four-way band (tests/form-asm-trig-band.fk) proves every
# kernel emits IDENTICAL bytes; this script proves those bytes, once emitted, compute the right
# answer on real hardware — the two proofs are complementary, neither substitutes for the other.
#
# Probes: the named accuracy-bar points (0, pi/6, pi/4, pi/2, pi, 3pi/2, 2pi) printed individually,
# then RoPE's real domain (position 0..8192, inv_freq = 1/base^(2i/d), d=128, base=500000 — the
# llama3 RoPE convention) swept in batch and summarized as a max error over N points. The body
# makes the objects and both point sets (form/scripts/form-asm-trig-witness.bml); this carrier
# links, loads and compares.
set -euo pipefail
cd "$(dirname "$0")/.."   # form/

[[ -x ../fkwu ]] || { echo "the body's kernel is missing: ../fkwu (cc -O2 -o fkwu runtime/fkwu-uni.c)" >&2; exit 1; }

WORK="$(mktemp -d "${TMPDIR:-/tmp}/form-asm-trig-witness.XXXXXX")"
trap 'rm -rf "$WORK"' EXIT

# ── 1. the body writes the .o byte images for _fam_sin / _fam_cos (form-macho wraps fam-sin/fam-cos's
# own bytes — the SAME bytes tests/form-asm-trig-band.fk already proved four-way byte-identical), the
# named points and the RoPE domain ──
printf '%s\n' "$WORK" > "$WORK/in"
(cd .. && ./fkwu form/scripts/form-asm-trig-witness.bml < "$WORK/in" 2>/dev/null)

# ── 2. link into durable recipe dylibs (ld -dylib, ad-hoc signed — the recipe-dylib carrier
# form-macho.fk's own header names; NO clang in the compute, only the system linker) ──
SDK="$(xcrun --sdk macosx --show-sdk-path)"
ld -dylib -arch arm64 -platform_version macos 11.0 11.0 \
    -o "$WORK/libfam_sin.dylib" "$WORK/fam_sin.o" -lSystem -L "$SDK/usr/lib" -syslibroot "$SDK"
ld -dylib -arch arm64 -platform_version macos 11.0 11.0 \
    -o "$WORK/libfam_cos.dylib" "$WORK/fam_cos.o" -lSystem -L "$SDK/usr/lib" -syslibroot "$SDK"

# ── 3. the dlopen+dlsym+call loader — an FFI harness ONLY, no compute. libm here is the ORACLE
# for correctness (a real, independent sin/cos), never a stand-in for the emitted bytes' own math. ──
cat > "$WORK/loader.c" <<'C'
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <math.h>

int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "usage: loader <sin.dylib> <cos.dylib> [x0 x1 ...]\n"); return 2; }
    void *hs = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    void *hc = dlopen(argv[2], RTLD_NOW | RTLD_LOCAL);
    if (!hs || !hc) { fprintf(stderr, "dlopen failed: %s\n", dlerror()); return 1; }
    double (*fam_sin)(double) = (double (*)(double))dlsym(hs, "fam_sin");
    double (*fam_cos)(double) = (double (*)(double))dlsym(hc, "fam_cos");
    if (!fam_sin || !fam_cos) { fprintf(stderr, "dlsym failed: %s\n", dlerror()); return 1; }

    double max_sin_err = 0.0, max_cos_err = 0.0;
    long n = 0;
    if (argc > 3) {
        for (int i = 3; i < argc; i++) {
            double x = atof(argv[i]);
            double asm_s = fam_sin(x), asm_c = fam_cos(x);
            double lib_s = sin(x), lib_c = cos(x);
            double es = fabs(asm_s - lib_s), ec = fabs(asm_c - lib_c);
            if (es > max_sin_err) max_sin_err = es;
            if (ec > max_cos_err) max_cos_err = ec;
            printf("x=%-22.17g asm_sin=%-22.17g libm_sin=%-22.17g err=%-12.3e | asm_cos=%-22.17g libm_cos=%-22.17g err=%-12.3e\n",
                   x, asm_s, lib_s, es, asm_c, lib_c, ec);
            n++;
        }
    } else {
        char line[128];
        while (fgets(line, sizeof(line), stdin)) {
            double x = atof(line);
            double es = fabs(fam_sin(x) - sin(x));
            double ec = fabs(fam_cos(x) - cos(x));
            if (es > max_sin_err) max_sin_err = es;
            if (ec > max_cos_err) max_cos_err = ec;
            n++;
        }
    }
    printf("N=%ld MAX_SIN_ERR=%.6e MAX_COS_ERR=%.6e\n", n, max_sin_err, max_cos_err);
    dlclose(hs); dlclose(hc);
    return 0;
}
C
cc -O2 -o "$WORK/loader" "$WORK/loader.c"

# ── 4. run the named accuracy-bar points individually ──
echo "── named points (0, pi/6, pi/4, pi/2, pi, 3pi/2, 2pi) ──"
NAMED_PTS=$(cat "$WORK/named_pts.txt")
"$WORK/loader" "$WORK/libfam_sin.dylib" "$WORK/libfam_cos.dylib" $NAMED_PTS

# ── 5. sweep RoPE's real domain in batch (position 0..8192, inv_freq = 1/base^(2i/d), d=128,
# base=500000) — the actual angles a RoPE rotation would feed this lane ──
echo
echo "── RoPE domain sweep (position 0..8192 step 7, d=128, base=500000) ──"
"$WORK/loader" "$WORK/libfam_sin.dylib" "$WORK/libfam_cos.dylib" < "$WORK/rope_domain.txt" | tail -1
