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
# llama3 RoPE convention) swept in batch and summarized as a max error over N points.
set -euo pipefail
cd "$(dirname "$0")/.."   # form/

GO_BIN="form-kernel-go/bin-go"
if [[ ! -x "$GO_BIN" ]] || find form-kernel-go -name '*.go' -newer "$GO_BIN" -print -quit | grep -q .; then
    echo "  building go kernel..." >&2
    (cd form-kernel-go && go build -o bin-go .)
fi

WORK="$(mktemp -d "${TMPDIR:-/tmp}/form-asm-trig-witness.XXXXXX")"
trap 'rm -rf "$WORK"' EXIT

# ── 1. emit the .o byte images for _fam_sin / _fam_cos (form-macho wraps fam-sin/fam-cos's own
# bytes — the SAME bytes tests/form-asm-trig-band.fk already proved four-way byte-identical) ──
cat > "$WORK/emit.fk" <<'FK'
(do
    (list
        (mo-object-sym (fam-sin) (list 95 102 97 109 95 115 105 110))
        (mo-object-sym (fam-cos) (list 95 102 97 109 95 99 111 115))))
FK

"$GO_BIN" \
    form-stdlib/core.fk form-stdlib/format-arith.fk form-stdlib/f64-bytes.fk \
    form-stdlib/form-asm.fk form-stdlib/form-asm-matvec.fk form-stdlib/form-macho.fk \
    form-stdlib/form-asm-trig.fk "$WORK/emit.fk" > "$WORK/objs.txt"

python3 - "$WORK/objs.txt" "$WORK/fam_sin.o" "$WORK/fam_cos.o" <<'PY'
import ast, sys
objs_path, sin_path, cos_path = sys.argv[1:4]
sin_o, cos_o = ast.literal_eval(open(objs_path).read().strip())
open(sin_path, "wb").write(bytes(sin_o))
open(cos_path, "wb").write(bytes(cos_o))
print(f"emitted _fam_sin.o ({len(sin_o)} bytes), _fam_cos.o ({len(cos_o)} bytes)")
PY

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
NAMED_PTS=$(python3 -c "
import math
pts = [0, math.pi/6, math.pi/4, math.pi/2, math.pi, 3*math.pi/2, 2*math.pi]
print(' '.join(repr(p) for p in pts))
")
"$WORK/loader" "$WORK/libfam_sin.dylib" "$WORK/libfam_cos.dylib" $NAMED_PTS

# ── 5. sweep RoPE's real domain in batch (position 0..8192, inv_freq = 1/base^(2i/d), d=128,
# base=500000) — the actual angles a RoPE rotation would feed this lane ──
echo
echo "── RoPE domain sweep (position 0..8192 step 7, d=128, base=500000) ──"
python3 -c "
base = 500000.0
d = 128
for pos in range(0, 8193, 7):
    for i in range(0, d // 2):
        inv_freq = 1.0 / (base ** (2 * i / d))
        print(pos * inv_freq)
" > "$WORK/rope_domain.txt"
"$WORK/loader" "$WORK/libfam_sin.dylib" "$WORK/libfam_cos.dylib" < "$WORK/rope_domain.txt" | tail -1
