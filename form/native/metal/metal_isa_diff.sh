#!/usr/bin/env bash
# metal_isa_diff.sh — STONE 10. The Q6_K matvec the BODY emits, and llama.cpp's Q6_K matvec, on the
# SAME device, over the SAME real llama3.2:3b weights, in the SAME process — plus the two controlled
# variants that say WHERE the gap lives.
#
# WHY THIS EXISTS. Stone 7 measured D = 6L and concluded that the remaining decode gap is "almost
# entirely decode arithmetic". Two mechanism hypotheses were then formed at altitude and BOTH were
# refuted by measurement (Q6_K's 210-byte stride; llama.cpp's nr0 register blocking). This file
# exists so that no third hypothesis has to be formed at altitude: it compiles both kernels and runs
# them side by side, and any claim about the gap can be falsified in one command.
#
# WHAT THE INSTRUMENT FOUND (M4 Max, 2026-07-21; receipts/2026-07-21-isa-diff-against-the-floor.md):
#
#   variant                                          arithmetic   thread map    x ggml (3 shapes)
#   form_q6k_matvec_lane_f32   (the body's, today)   div/rem      flat index    7.30 / 7.59 / 9.58
#   v1  bit ops, our map                             bit          flat index    3.15 / 3.20 / 3.93
#   v3  our arithmetic, ggml's map                   div/rem      4-wide slot   1.11 / 1.26 / 1.37
#   v2  bit ops, ggml's map                          bit          4-wide slot   1.26 / 1.36 / 1.46
#
# Read rows 2 and 3 together. Healing only the ARITHMETIC bought 2.4x. Healing only the THREAD MAP
# bought 7.0x — and once the map was right, the arithmetic form was worth NOTHING (v3, which keeps
# every division and remainder, is not slower than v2, which has none). The divisions were never the
# cost; a FLAT INDEX makes the field selector g a per-weight runtime value, so q6k_pow4(g) becomes a
# runtime divisor — a real integer divide, per weight, on a GPU that has no integer divide unit —
# and a three-way switch lands in the innermost loop. Corpus row 846, asktoll.
#
# THE FLOOR THIS INSTRUMENT SITS ON, and it is a real one. Apple ships NO AGX assembly printer:
# `applegpu-nt -S` answers "Plugin interface not implemented: AIRNTEmitAssembly", and metal-objdump
# says "no instruction printer for target agx3---macho" on the native slice of a serialized
# MTLBinaryArchive. So there is no native instruction stream to diff on this machine. What IS
# available is AIR — post-`-O2` LLVM IR, one level above the ISA — and wall time. This file measures
# the second and receipts/2026-07-21-isa-diff-against-the-floor.md counts the first.
#
# THE RADIUS. Q6_K matvec, decode shape (one activation vector), llama3.2:3b's tensors, this GPU.
# It does not speak for Q4_K, for attention, for prefill, or for any batched shape.
#
# THE ATTESTANT IS UNTOUCHED. This file compiles and times; it changes no emitted kernel and is on
# no inference path. The variants live in THIS file, in C, precisely because they are not yet the
# body's — nothing here is admissible into the body until a .fk cell authors it and a band proves it.
#
# ggml/llama.cpp's kernel below is MIT-licensed and reproduced verbatim from the MSL recovered out of
# the ollama binary on this host; only its surrounding type declarations were re-assembled, each with
# the line it came from noted. Copyright (c) 2023-2024 The ggml authors.
#
# Run:  form/native/metal/metal_isa_diff.sh
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
BLOB="${FORM_GGUF_BLOB:-$HOME/.ollama/models/blobs/sha256-dde5aa3fc5ffc17176b5e8bdc82f587b24b2678c6c66101bf7da77af9f7ccdff}"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host"; exit 2
fi
if [[ ! -f "$BLOB" ]]; then
    echo "SKIP  the llama3.2:3b GGUF blob is not on this host: $BLOB"; exit 2
fi
if [[ ! -x "$GO_BIN" ]]; then (cd "$ROOT/form-kernel-go" && go build -o bin-go .); fi

work="$(mktemp -d "${TMPDIR:-/tmp}/fkisadiff.XXXXXX")"
trap 'rm -rf "$work"' EXIT
cd "$ROOT"

# ── the `; preludes:` directives are LIVE recursive load instructions; walked, never hand-catted ──
FK_SEEN=""
fk_deps() {
    awk '
        /^;[ \t]*preludes:/ {
            s = $0; sub(/^;[ \t]*preludes:[ \t]*/, "", s); gsub(/,/, " ", s)
            n = split(s, a, /[ \t]+/)
            for (i = 1; i <= n; i++) {
                low = tolower(a[i])
                if (a[i] == "\\" || low == "none" || low == "(none)" || a[i] == "") continue
                if (a[i] ~ /\.fk$/) print a[i]
            }
        }' "$1" 2>/dev/null
}
fk_path() {
    local dir; dir="$(dirname "$1")"
    if   [[ -f "$dir/$2" ]]; then printf '%s\n' "$dir/$2"
    elif [[ -f "$2" ]];      then printf '%s\n' "$2"
    elif [[ "$2" == form/* && -f "${2#form/}" ]]; then printf '%s\n' "${2#form/}"
    else printf '%s\n' "$dir/$2"; fi
}
fk_expand() {
    local f="$1" d p
    case " $FK_SEEN " in *" $f "*) return ;; esac
    FK_SEEN="$FK_SEEN $f"
    while read -r d; do
        [[ -z "$d" ]] && continue
        p="$(fk_path "$f" "$d")"
        fk_expand "$p"
    done < <(fk_deps "$f")
    printf '%s\n' "$f"
}
FILES=()
while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/first-token.fk)

echo "=== the body speaks (resolver-driven, ${#FILES[@]} cells) ==="
printf '(ft-emit-msl)\n' > "$work/e.fk"
"$GO_BIN" "${FILES[@]}" "$work/e.fk" > "$work/msl.out" 2>"$work/e.err" || {
    echo "FAIL  MSL emission failed"; cat "$work/e.err"; exit 1; }
awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/msl.out" > "$work/ours.metal"
grep -q 'kernel void form_q6k_matvec_lane_f32' "$work/ours.metal" || {
    echo "FAIL  the body did not emit form_q6k_matvec_lane_f32"; exit 1; }
echo "  $(wc -c < "$work/ours.metal" | tr -d ' ') bytes, every character authored by a .fk cell"

# ── ggml's kernel. Verbatim; the declarations around it carry their provenance line by line. ──
cat > "$work/theirs.metal" <<'THEIRS'
#include <metal_stdlib>
using namespace metal;
#define QK_K 256
#define N_R0_Q6_K 2
#define FC_MUL_MV 600
#define FOR_UNROLL(x) _Pragma("clang loop unroll(full)") for (x)
typedef half ggml_half;
typedef struct {                 // ggml/llama.cpp, MIT. block_q6_K
    uint8_t ql[QK_K/2];
    uint8_t qh[QK_K/4];
    int8_t  scales[QK_K/16];
    ggml_half d;
} block_q6_K;
typedef struct {                 // ggml_metal_kargs_mul_mv
    int32_t  ne00; int32_t  ne01; int32_t  ne02;
    uint64_t nb00; uint64_t nb01; uint64_t nb02; uint64_t nb03;
    int32_t  ne10; int32_t  ne11; int32_t  ne12;
    uint64_t nb10; uint64_t nb11; uint64_t nb12; uint64_t nb13;
    int32_t  ne0;  int32_t  ne1;  int32_t  nr0;
    int16_t  r2;   int16_t  r3;
} ggml_metal_kargs_mul_mv;
constant short FC_mul_mv_nsg [[function_constant(FC_MUL_MV + 0)]];
template<int nr0, typename args_t>
void kernel_mul_mv_q6_K_f32_impl(
        args_t args, device const char * src0, device const char * src1,
        device char * dst, threadgroup char * shmem,
        uint3 tgpig, ushort tiisg, ushort sgitg) {
    const short NSG = FC_mul_mv_nsg;
    constexpr uint8_t kmask1 = 0x03; constexpr uint8_t kmask2 = 0x0C;
    constexpr uint8_t kmask3 = 0x30; constexpr uint8_t kmask4 = 0xC0;
    const int nb = args.ne00/QK_K;
    const int r0 = tgpig.x; const int r1 = tgpig.y; const int im = tgpig.z;
    const int first_row = (r0 * NSG + sgitg) * nr0;
    const uint i12 = im%args.ne12; const uint i13 = im/args.ne12;
    const uint64_t offset0 = first_row*args.nb01 + (i12/args.r2)*args.nb02 + (i13/args.r3)*args.nb03;
    const uint64_t offset1 =        r1*args.nb11 + (i12        )*args.nb12 + (i13        )*args.nb13;
    device const block_q6_K * x = (device const block_q6_K *) (src0 + offset0);
    device const float     * yy = (device const float      *) (src1 + offset1);
    float sumf[nr0] = { 0.f };
    float yl[16];
    const short tid = tiisg/2; const short ix = tiisg%2;
    const short ip = tid/8; const short il = tid%8;
    const short l0 = 4*il; const short is = 8*ip + l0/16;
    const short y_offset   = 128*ip + l0;
    const short q_offset_l =  64*ip + l0;
    const short q_offset_h =  32*ip + l0;
    for (int i = ix; i < nb; i += 2) {
        device const uint8_t * q1 = x[i].ql + q_offset_l;
        device const uint8_t * q2 = q1 + 32;
        device const uint8_t * qh = x[i].qh + q_offset_h;
        device const int8_t  * sc = x[i].scales + is;
        device const half    * dh = &x[i].d;
        device const float * y = yy + i * QK_K + y_offset;
        for (short l = 0; l < 4; ++l) {
            yl[4*l + 0] = y[l +  0]; yl[4*l + 1] = y[l + 32];
            yl[4*l + 2] = y[l + 64]; yl[4*l + 3] = y[l + 96];
        }
        for (short row = 0; row < nr0; ++row) {
            float4 sums = {0.f, 0.f, 0.f, 0.f};
            FOR_UNROLL (short l = 0; l < 4; ++l) {
                sums[0] += yl[4*l + 0] * ((int8_t)((q1[l] & 0xF) | ((qh[l] & kmask1) << 4)) - 32);
                sums[1] += yl[4*l + 1] * ((int8_t)((q2[l] & 0xF) | ((qh[l] & kmask2) << 2)) - 32);
                sums[2] += yl[4*l + 2] * ((int8_t)((q1[l]  >> 4) | ((qh[l] & kmask3) << 0)) - 32);
                sums[3] += yl[4*l + 3] * ((int8_t)((q2[l]  >> 4) | ((qh[l] & kmask4) >> 2)) - 32);
            }
            sumf[row] += dh[0] * (sums[0] * sc[0] + sums[1] * sc[2] + sums[2] * sc[4] + sums[3] * sc[6]);
            q1 += args.nb01; q2 += args.nb01; qh += args.nb01;
            sc += args.nb01; dh += args.nb01/2;
        }
    }
    device float * dst_f32 = (device float *) dst + (uint64_t)im*args.ne0*args.ne1 + (uint64_t)r1*args.ne0;
    for (int row = 0; row < nr0 && first_row + row < args.ne0; ++row) {
        float sum_all = simd_sum(sumf[row]);
        if (tiisg == 0) { dst_f32[first_row + row] = sum_all; }
    }
}
[[host_name("kernel_mul_mv_q6_K_f32")]]
kernel void kernel_mul_mv_q6_K_f32(
        constant ggml_metal_kargs_mul_mv & args,
        device const char * src0, device const char * src1, device char * dst,
        uint3  tgpig[[threadgroup_position_in_grid]],
        ushort tiisg[[thread_index_in_simdgroup]],
        ushort sgitg[[simdgroup_index_in_threadgroup]]) {
    kernel_mul_mv_q6_K_f32_impl<N_R0_Q6_K, constant ggml_metal_kargs_mul_mv &>(args, src0, src1, dst, nullptr, tgpig, tiisg, sgitg);
}
THEIRS

# ── the two controlled variants. NOT the body's — see the header. Each removes exactly one thing. ──
cat > "$work/tail.metal" <<'VARIANTS'

// V1 — the body's lane kernel with ONLY the integer identities replaced by bit operations.
// The thread map is unchanged. Isolates the cost of the arithmetic FORM.
static inline float q6k_wi_bit(device const uchar* qb, uint b, int i, float d) {
    int h = i >> 7; int wi = i & 127; int l = wi & 31; int g = wi >> 5; int is = l >> 4;
    int qlb = int(qb[b + uint(h * 64 + l + (g & 1) * 32)]);
    int nib = (g < 2) ? (qlb & 15) : (qlb >> 4);
    int qhb = int(qb[b + 128u + uint(h * 32 + l)]);
    int hi = (qhb >> (2 * g)) & 3;
    int q = nib + hi * 16 - 32;
    int sc = int((char)(qb[b + 192u + uint(h * 8 + is + 2 * g)]));
    return (d * float(sc)) * float(q);
}
kernel void isa_q6k_v1_f32 (device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], uint gid [[thread_position_in_grid]], uint lane [[thread_index_in_simdgroup]]) {
    uint r = gid / 32u; if (r >= rows) return;
    float acc = 0.0f; uint curblk = 4294967295u; uint b = 0u; float d = 0.0f;
    uint nk = (cols > lane) ? ((cols - lane + 31u) / 32u) : 0u;
    uint k = nk;
    while (k > 0) { k -= 1; uint j = lane + k * 32u; uint idx = r * cols + j;
        uint blk = idx / 256u;
        if (blk != curblk) { curblk = blk; b = blk * 210u; d = q6k_f16(int(qb[b + 208u]) + 256 * int(qb[b + 209u])); }
        acc = q6k_wi_bit(qb, b, int(idx - blk * 256u), d) * float(x[j]) + acc; }
    float s = metal::simd_sum(acc);
    if (lane == 0) y[r] = s;
}

// V2 — bit ops PLUS ggml's thread map: each lane owns a fixed 4-wide slot in every superblock, so
// one ql byte feeds 2 weights, one qh byte feeds 4, one scale byte feeds 16, and four products land
// in four INDEPENDENT accumulators. The body's own f16 decode and simd_sum are kept.
// Assumes cols % 256 == 0 (true of every llama3.2:3b Q6_K tensor); a general form needs a tail.
kernel void isa_q6k_v2_f32 (device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], uint gid [[thread_position_in_grid]], uint lane [[thread_index_in_simdgroup]]) {
    uint r = gid / 32u; if (r >= rows) return;
    uint nb = cols / 256u; uint rowbase = r * nb * 210u;
    int tid = int(lane) / 2, ix = int(lane) % 2;
    int ip = tid / 8, il = tid % 8, l0 = 4 * il;
    int is = 8 * ip + l0 / 16;
    int y_offset = 128 * ip + l0, q_offset_l = 64 * ip + l0, q_offset_h = 32 * ip + l0;
    float sumf = 0.0f;
    for (uint i = uint(ix); i < nb; i += 2) {
        uint b = rowbase + i * 210u;
        device const uchar* q1 = qb + b + uint(q_offset_l);
        device const uchar* q2 = q1 + 32;
        device const uchar* qh = qb + b + 128u + uint(q_offset_h);
        device const uchar* sc = qb + b + 192u + uint(is);
        float d = q6k_f16(int(qb[b + 208u]) + 256 * int(qb[b + 209u]));
        device const float* yv = x + i * 256u + uint(y_offset);
        float4 sums = float4(0.0f);
        for (int l = 0; l < 4; ++l) {
            sums[0] += yv[l +  0] * float(int((q1[l] & 15) | ((qh[l] &   3) << 4)) - 32);
            sums[1] += yv[l + 32] * float(int((q2[l] & 15) | ((qh[l] &  12) << 2)) - 32);
            sums[2] += yv[l + 64] * float(int((q1[l] >> 4) | ((qh[l] &  48)     )) - 32);
            sums[3] += yv[l + 96] * float(int((q2[l] >> 4) | ((qh[l] & 192) >> 2)) - 32);
        }
        sumf += d * (sums[0] * float(int((char)sc[0])) + sums[1] * float(int((char)sc[2]))
                   + sums[2] * float(int((char)sc[4])) + sums[3] * float(int((char)sc[6])));
    }
    float s = metal::simd_sum(sumf);
    if (lane == 0) y[r] = s;
}

// V3 — ggml's thread map with the BODY'S arithmetic put back: q6k_mod and division, no bit operator
// anywhere in the decode. This is the variant that decides the question, and it is the one that
// refuses the arithmetic hypothesis: it is not slower than V2.
kernel void isa_q6k_v3_f32 (device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], uint gid [[thread_position_in_grid]], uint lane [[thread_index_in_simdgroup]]) {
    uint r = gid / 32u; if (r >= rows) return;
    uint nb = cols / 256u; uint rowbase = r * nb * 210u;
    int tid = int(lane) / 2, ix = q6k_mod(int(lane), 2);
    int ip = tid / 8, il = q6k_mod(tid, 8), l0 = 4 * il;
    int is = 8 * ip + l0 / 16;
    int y_offset = 128 * ip + l0, q_offset_l = 64 * ip + l0, q_offset_h = 32 * ip + l0;
    float sumf = 0.0f;
    for (uint i = uint(ix); i < nb; i += 2) {
        uint b = rowbase + i * 210u;
        device const uchar* q1 = qb + b + uint(q_offset_l);
        device const uchar* q2 = q1 + 32;
        device const uchar* qh = qb + b + 128u + uint(q_offset_h);
        device const uchar* sc = qb + b + 192u + uint(is);
        float d = q6k_f16(int(qb[b + 208u]) + 256 * int(qb[b + 209u]));
        device const float* yv = x + i * 256u + uint(y_offset);
        float4 sums = float4(0.0f);
        for (int l = 0; l < 4; ++l) {
            int a1 = int(q1[l]), a2 = int(q2[l]), ah = int(qh[l]);
            sums[0] += yv[l +  0] * float(q6k_mod(a1, 16) + q6k_mod(ah, 4) * 16 - 32);
            sums[1] += yv[l + 32] * float(q6k_mod(a2, 16) + q6k_mod(ah / 4, 4) * 16 - 32);
            sums[2] += yv[l + 64] * float(a1 / 16 + q6k_mod(ah / 16, 4) * 16 - 32);
            sums[3] += yv[l + 96] * float(a2 / 16 + (ah / 64) * 16 - 32);
        }
        sumf += d * (sums[0] * float(q6k_s8(int(sc[0]))) + sums[1] * float(q6k_s8(int(sc[2])))
                   + sums[2] * float(q6k_s8(int(sc[4]))) + sums[3] * float(q6k_s8(int(sc[6]))));
    }
    float s = metal::simd_sum(sumf);
    if (lane == 0) y[r] = s;
}
VARIANTS
cat "$work/ours.metal" "$work/tail.metal" > "$work/variants.metal"

# ── the AIR both sides are counted in. There is no native slice to count: see the header. ──
for f in variants theirs; do
    xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
        -c "$work/$f.metal" -o "$work/$f.air" 2>"$work/$f.err" || {
        echo "FAIL  $f did not compile"; cat "$work/$f.err"; exit 1; }
    xcrun metal-opt -S "$work/$f.air" -o "$work/$f.ll" 2>/dev/null
done
echo "  AIR emitted: $(wc -l < "$work/variants.ll" | tr -d ' ') lines ours+variants, $(wc -l < "$work/theirs.ll" | tr -d ' ') lines ggml"

# ── the carrier: it binds, dispatches, times and compares. It judges nothing. ──
cat > "$work/bench.swift" <<'SWIFT'
import Metal
import Foundation
let a = CommandLine.arguments
let blobPath = a[1], oursPath = a[2], theirsPath = a[3]
let off = Int(a[4])!, cols = Int(a[5])!, rows = Int(a[6])!, iters = Int(a[7])!, label = a[8]
let dev = MTLCreateSystemDefaultDevice()!
let q = dev.makeCommandQueue()!
let nb01 = (cols / 256) * 210
let nbytes = nb01 * rows
let fh = FileHandle(forReadingAtPath: blobPath)!
fh.seek(toFileOffset: UInt64(off))
let data = fh.readData(ofLength: nbytes)
precondition(data.count == nbytes, "short read")
let bQ = data.withUnsafeBytes { dev.makeBuffer(bytes: $0.baseAddress!, length: nbytes, options: .storageModeShared)! }
var x = [Float](repeating: 0, count: cols)
for i in 0..<cols { x[i] = Float((i &* 2654435761) % 1000) / 1000.0 - 0.5 }
let bX = dev.makeBuffer(bytes: &x, length: cols * 4, options: .storageModeShared)!
let bY1 = dev.makeBuffer(length: rows * 4, options: .storageModeShared)!
let bY2 = dev.makeBuffer(length: rows * 4, options: .storageModeShared)!
func libFrom(_ p: String) throws -> MTLLibrary {
    let o = MTLCompileOptions(); o.languageVersion = .version3_0; o.mathMode = .safe
    return try dev.makeLibrary(source: String(contentsOfFile: p, encoding: .utf8), options: o)
}
let libOurs = try libFrom(oursPath), libTheirs = try libFrom(theirsPath)
// every dispatch of a run is encoded into ONE command buffer, so the per-dispatch command-buffer
// round trip (~0.2 ms on this host, larger than several of these kernels) is not being timed.
var pOurs = try dev.makeComputePipelineState(function: libOurs.makeFunction(name: "form_q6k_matvec_lane_f32")!)
func runOurs(_ n: Int) -> Double {
    var r = UInt32(rows), c = UInt32(cols)
    let t0 = Date()
    let cb = q.makeCommandBuffer()!, e = cb.makeComputeCommandEncoder()!
    e.setComputePipelineState(pOurs)
    e.setBuffer(bQ, offset: 0, index: 0); e.setBuffer(bX, offset: 0, index: 1)
    e.setBuffer(bY1, offset: 0, index: 2)
    e.setBytes(&r, length: 4, index: 3); e.setBytes(&c, length: 4, index: 4)
    let tg = min(256, pOurs.maxTotalThreadsPerThreadgroup / 32 * 32)
    for _ in 0..<n {
        e.dispatchThreads(MTLSize(width: rows * 32, height: 1, depth: 1),
                          threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
    }
    e.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    return Date().timeIntervalSince(t0) / Double(n) * 1000.0
}
struct Kargs {
    var ne00: Int32 = 0, ne01: Int32 = 0, ne02: Int32 = 0
    var nb00: UInt64 = 0, nb01: UInt64 = 0, nb02: UInt64 = 0, nb03: UInt64 = 0
    var ne10: Int32 = 0, ne11: Int32 = 0, ne12: Int32 = 0
    var nb10: UInt64 = 0, nb11: UInt64 = 0, nb12: UInt64 = 0, nb13: UInt64 = 0
    var ne0: Int32 = 0, ne1: Int32 = 0, nr0: Int32 = 0
    var r2: Int16 = 0, r3: Int16 = 0
}
var ka = Kargs()
ka.ne00 = Int32(cols); ka.ne01 = Int32(rows); ka.ne02 = 1
ka.nb00 = 1; ka.nb01 = UInt64(nb01); ka.nb02 = UInt64(nb01 * rows); ka.nb03 = ka.nb02
ka.ne10 = Int32(cols); ka.ne11 = 1; ka.ne12 = 1
ka.nb10 = 4; ka.nb11 = UInt64(cols * 4); ka.nb12 = ka.nb11; ka.nb13 = ka.nb11
ka.ne0 = Int32(rows); ka.ne1 = 1; ka.nr0 = 2; ka.r2 = 1; ka.r3 = 1
let NR0 = 2
func pipeTheirs(_ nsg: Int) throws -> MTLComputePipelineState {
    let cv = MTLFunctionConstantValues(); var s = Int16(nsg)
    cv.setConstantValue(&s, type: .short, index: 600)
    return try dev.makeComputePipelineState(function: try libTheirs.makeFunction(name: "kernel_mul_mv_q6_K_f32", constantValues: cv))
}
func runTheirs(_ p: MTLComputePipelineState, _ nsg: Int, _ n: Int) -> Double {
    let t0 = Date()
    let cb = q.makeCommandBuffer()!, e = cb.makeComputeCommandEncoder()!
    e.setComputePipelineState(p)
    e.setBytes(&ka, length: MemoryLayout<Kargs>.stride, index: 0)
    e.setBuffer(bQ, offset: 0, index: 1); e.setBuffer(bX, offset: 0, index: 2)
    e.setBuffer(bY2, offset: 0, index: 3)
    let ntg = (rows + NR0 * nsg - 1) / (NR0 * nsg)
    for _ in 0..<n {
        e.dispatchThreadgroups(MTLSize(width: ntg, height: 1, depth: 1),
                               threadsPerThreadgroup: MTLSize(width: 32, height: nsg, depth: 1))
    }
    e.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    return Date().timeIntervalSince(t0) / Double(n) * 1000.0
}
print("SHAPE \(label) rows=\(rows) cols=\(cols) MACs=\(rows*cols) qbytes=\(nbytes)")
var results = [(String, Double)]()
// the body's kernel FIRST and the fastest variant LAST, so the agreement check below compares the
// variant that is being proposed against ggml, not whichever one happened to run
for nm in ["form_q6k_matvec_lane_f32", "isa_q6k_v1_f32", "isa_q6k_v3_f32", "isa_q6k_v2_f32"] {
    pOurs = try dev.makeComputePipelineState(function: libOurs.makeFunction(name: nm)!)
    _ = runOurs(1)
    var t = [Double]()
    for _ in 0..<3 { t.append(runOurs(iters)) }        // three runs, the minimum reported
    let mv = t.min()!
    results.append((nm, mv))
    print("  ours  \(nm.padding(toLength: 26, withPad: " ", startingAt: 0))  \(String(format: "%.4f", mv)) ms   \(String(format: "%.2f", Double(rows*cols)/mv/1e6)) GMAC/s")
}
var best = Double.infinity, bestNsg = 0
for nsg in [1, 2, 4, 8] {
    let p = try pipeTheirs(nsg)
    if 32 * nsg > p.maxTotalThreadsPerThreadgroup { continue }
    _ = runTheirs(p, nsg, 1)
    var ts = [Double]()
    for _ in 0..<3 { ts.append(runTheirs(p, nsg, iters)) }
    let mv = ts.min()!
    print("  ggml  kernel_mul_mv_q6_K_f32 nsg=\(nsg)   \(String(format: "%.4f", mv)) ms   \(String(format: "%.2f", Double(rows*cols)/mv/1e6)) GMAC/s")
    if mv < best { best = mv; bestNsg = nsg }
}
let y1 = bY1.contents().bindMemory(to: Float.self, capacity: rows)
let y2 = bY2.contents().bindMemory(to: Float.self, capacity: rows)
var maxAbs = 0.0, maxRel = 0.0
for i in 0..<rows {
    let d = abs(Double(y1[i] - y2[i]))
    let m = max(abs(Double(y1[i])), abs(Double(y2[i])))
    if d > maxAbs { maxAbs = d }
    if m > 1e-6 { maxRel = max(maxRel, d / m) }
}
// V2 and ggml sum the same terms in the same association, so this is an EQUALITY claim, not an
// epsilon: any nonzero difference here means the transcription of ggml's kernel is not faithful.
print("  AGREE v2 vs ggml over all \(rows) rows: max|Δ|=\(String(format: "%.3e", maxAbs))  max rel=\(String(format: "%.3e", maxRel))")
if maxAbs != 0.0 { print("FAIL  v2 and ggml do not agree exactly — the transcription is suspect") }
for (nm, mv) in results {
    print("  RATIO \(nm.padding(toLength: 26, withPad: " ", startingAt: 0)) / ggml(nsg=\(bestNsg)) = \(String(format: "%.2f", mv / best))x")
}
SWIFT
xcrun swiftc -O "$work/bench.swift" -o "$work/bench" 2>"$work/sw.err" || {
    echo "FAIL  carrier did not build"; cat "$work/sw.err"; exit 1; }

# ── three real shapes, because no rate here is one point pretending to be a line (row 827) ──
echo
"$work/bench" "$BLOB" "$work/variants.metal" "$work/theirs.metal" 331055328 8192   3072 50 blk.0.ffn_down
echo
"$work/bench" "$BLOB" "$work/variants.metal" "$work/theirs.metal" 392409312 3072   1024 50 blk.0.attn_v
echo
"$work/bench" "$BLOB" "$work/variants.metal" "$work/theirs.metal"   7837920 3072 128256 10 token_embd.output
