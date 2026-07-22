#!/usr/bin/env bash
# metal_dsv4_layer.sh — Stone 35, Stage 1: ENTER the DeepSeek-V4-Flash MLA attention block at REAL DIMS
# over the windowed-resident 85 GiB file. Stone 33 proved the two ends (metal_dsv4_token.sh: EMBED in,
# MXFP8 vocab out). Stone 34 proved the MoE-FFN middle. This harness proves the ATTENTION block's ENTRY:
# the input RMSNorm, the low-rank Q and KV down-projections (attn_q_a 4096->1024, attn_kv 4096->512, both
# type-41 MXFP8), and the two rank-space RMSNorms (attn_q_a_norm 1024, attn_kv_a_norm 512) — each dispatch
# on the file's OWN blk.0 weights through the overlapping views, each checked against an INDEPENDENT CPU
# carve at the tensor's absolute mmap offset. The kernels are mla-msl.fk's (MLA_MAX_HD=512 = the real
# head_dim) and mxfp8-msl.fk's (the vocab-projection kernel, unchanged), authored by the body.
#
# THE RADIUS (aporon). No external oracle can run this file — ds4/llama.cpp/ollama REFUSE types 40/41, so
# any activation is UNFALSIFIABLE against a reference (selfgauge). Each dispatch stands on the internal
# falsifier: GPU-through-the-view == independent CPU carve of the same bytes, to a stated f32 bound (the
# MXFP8/F32 weight decode is exact; a matvec and a Newton-sqrt reassociate — assocwall, row 866).
#
# THE HONEST BOUND on the INPUT (knownsolved). The true MLA input at layer 0 is the HC-pre of the residual
# stream, not yet wired. So the chain is fed the token's real EMBEDDING as a PROBE vector — exactly Stone
# 33/34's mechanism-witness class: it proves the projections and rank-norms BIND and COMPUTE at real dims
# through the views, NOT that the numbers are the real layer-0 activations. Tensors/dims/offsets ARE real.
#
# THE OFFERED-INTERFACE GUARD (edgedrop/zerobirth). Every output buffer is SENTINELLED (NaN) before its
# dispatch and cb.error/cb.status checked after; every result is required NON-DEGENERATE. A dead read cannot pass.
#
# Run:  form/native/metal/metal_dsv4_layer.sh   (optional: FORM_DS4_PROMPT_TOKEN=<id>)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
BLOB="${FORM_DS4_BLOB:-$HOME/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf}"
CACHE="$ROOT/native/metal/.metallib-cache"
TOKEN="${FORM_DS4_PROMPT_TOKEN:-671}"   # "The capital of France is" -> 671 ...

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the GPU witness needs an Apple GPU + swiftc"; exit 2
fi
if [[ ! -f "$BLOB" ]]; then
    echo "SKIP  the ds4 GGUF is not on this host: $BLOB   (set FORM_DS4_BLOB)"; exit 2
fi
if [[ ! -x "$GO_BIN" ]]; then
    echo "  building the Go kernel..."; (cd "$ROOT/form-kernel-go" && go build -o bin-go .) || { echo "FAIL go build"; exit 1; }
fi
FSIZE=$(stat -f%z "$BLOB")
work="$(mktemp -d)"; trap 'rm -rf "$work"' EXIT
echo "ds4 blob: $FSIZE bytes at $(date '+%H:%M:%S')   MLA-entry probe(token=$TOKEN)"

# ── the `; preludes:` directives are LIVE recursive load instructions; walked, never hand-catted ──
fk_deps(){ awk 'BEGIN{IGNORECASE=1} /^;[ \t]*preludes:/{ s=$0; sub(/^;[ \t]*preludes:[ \t]*/,"",s); n=split(s,a,/[ \t]+/); for(i=1;i<=n;i++){ if(a[i]=="\\"||tolower(a[i])=="none"||tolower(a[i])=="(none)"||a[i]=="")continue; if(a[i]~/\.fk$/)print a[i] } }' "$1" 2>/dev/null; }
fk_path(){ local dir; dir="$(dirname "$1")"; if [[ -f "$dir/$2" ]]; then printf '%s\n' "$dir/$2"; elif [[ -f "$2" ]]; then printf '%s\n' "$2"; elif [[ "$2" == form/* && -f "${2#form/}" ]]; then printf '%s\n' "${2#form/}"; else printf '%s\n' "$dir/$2"; fi; }
fk_expand(){ local f="$1" d p; case " $FK_SEEN " in *" $f "*) return ;; esac; FK_SEEN="$FK_SEEN $f"; while read -r d; do [[ -z "$d" ]] && continue; p="$(fk_path "$f" "$d")"; fk_expand "$p"; done < <(fk_deps "$f"); printf '%s\n' "$f"; }
cd "$ROOT"
FK_SEEN=""; FILES=(); while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/dsv4-mla-real.fk)

# ── 1. measure the device ─────────────────────────────────────────────────────────────────────────
cat > "$work/probe.swift" <<'SWIFT'
import Metal
import Foundation
guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
print("\(dev.maxBufferLength) \(getpagesize()) \(dev.name)")
SWIFT
swiftc -O -o "$work/probe" "$work/probe.swift" 2>"$work/probe.err" || { echo "FAIL swiftc probe"; tail "$work/probe.err"; exit 1; }
PROBE="$("$work/probe")"; prc=$?
if [[ $prc -eq 2 ]]; then echo "$PROBE"; exit 2; fi
MAXBUF="$(echo "$PROBE" | awk '{print $1}')"; PAGE="$(echo "$PROBE" | awk '{print $2}')"; DEVNAME="$(echo "$PROBE" | cut -d' ' -f3-)"
echo "device: $DEVNAME  maxBufferLength=$MAXBUF  page=$PAGE"

# ── 2. the residency plan (view/inner/holds per tensor) + the manifest (types & dims) over the file ──
echo "walking the file header for the residency plan and the manifest..."
printf '(wre-emit "%s" %s %s %s)\n' "$BLOB" "$FSIZE" "$MAXBUF" "$PAGE" > "$work/plan.fk"
"$GO_BIN" "${FILES[@]}" "$work/plan.fk" > "$work/plan.out" 2>"$work/plan.err" || { echo "FAIL plan emission"; tail -5 "$work/plan.err"; exit 1; }
grep -qx 'END' "$work/plan.out" || { echo "FAIL plan stream truncated"; exit 1; }
WR=($(awk '$1=="WR"{print; exit}' "$work/plan.out"))
STEP=${WR[7]}; VIEWLIMIT=${WR[5]}; NVIEWS=${WR[9]}
printf '(gm-emit-manifest "%s")\n' "$BLOB" > "$work/man.fk"
"$GO_BIN" "${FILES[@]}" "$work/man.fk" > "$work/man.out" 2>"$work/man.err" || { echo "FAIL manifest emission"; tail -5 "$work/man.err"; exit 1; }
echo "  plan: view_limit=$VIEWLIMIT step=$STEP nviews=$NVIEWS"

tv()  { awk -v n="$1" -v f="$2" '$1=="TV" && $2==n {print $(f); exit}' "$work/plan.out"; }  # 3=abs 4=bytes 5=idx 6=inner 7=holds
trow(){ awk -v n="$1" -v f="$2" '$1=="T"  && $2==n {print $(f); exit}' "$work/man.out"; }    # T name type ndim d0 d1 d2 abs nelslice slices bytes

N_EMBD=4096
# token_embd — F16 [n_embd, vocab]; the probe vector source.
EMB_ABS=$(tv token_embd.weight 3); EMB_IDX=$(tv token_embd.weight 5); EMB_INNER=$(tv token_embd.weight 6); EMB_HOLDS=$(tv token_embd.weight 7)
ROW_OFF=$(( TOKEN * N_EMBD * 2 ))
# attn_norm — F32 [4096]; the input RMSNorm weight.
NORM_ABS=$(tv blk.0.attn_norm.weight 3); NORM_IDX=$(tv blk.0.attn_norm.weight 5); NORM_INNER=$(tv blk.0.attn_norm.weight 6); NORM_HOLDS=$(tv blk.0.attn_norm.weight 7)
# attn_q_a — MXFP8 [in=4096, out=1024]; Q down-projection. rows=out=d1, cols=in=d0, nel=rows*cols.
QA_ABS=$(tv blk.0.attn_q_a.weight 3); QA_IDX=$(tv blk.0.attn_q_a.weight 5); QA_INNER=$(tv blk.0.attn_q_a.weight 6); QA_HOLDS=$(tv blk.0.attn_q_a.weight 7)
QA_IN=$(trow blk.0.attn_q_a.weight 5); QA_OUT=$(trow blk.0.attn_q_a.weight 6)
# attn_q_a_norm — F32 [1024].
QAN_ABS=$(tv blk.0.attn_q_a_norm.weight 3); QAN_IDX=$(tv blk.0.attn_q_a_norm.weight 5); QAN_INNER=$(tv blk.0.attn_q_a_norm.weight 6); QAN_HOLDS=$(tv blk.0.attn_q_a_norm.weight 7)
# attn_q_b — MXFP8 [in=1024, out=32768=n_head*head_dim]; Q up-projection.
QB_ABS=$(tv blk.0.attn_q_b.weight 3); QB_IDX=$(tv blk.0.attn_q_b.weight 5); QB_INNER=$(tv blk.0.attn_q_b.weight 6); QB_HOLDS=$(tv blk.0.attn_q_b.weight 7)
QB_IN=$(trow blk.0.attn_q_b.weight 5); QB_OUT=$(trow blk.0.attn_q_b.weight 6)
# attn_kv — MXFP8 [in=4096, out=512]; KV down-projection (the single latent).
KV_ABS=$(tv blk.0.attn_kv.weight 3); KV_IDX=$(tv blk.0.attn_kv.weight 5); KV_INNER=$(tv blk.0.attn_kv.weight 6); KV_HOLDS=$(tv blk.0.attn_kv.weight 7)
KV_IN=$(trow blk.0.attn_kv.weight 5); KV_OUT=$(trow blk.0.attn_kv.weight 6)
# attn_kv_a_norm — F32 [512].
KVAN_ABS=$(tv blk.0.attn_kv_a_norm.weight 3); KVAN_IDX=$(tv blk.0.attn_kv_a_norm.weight 5); KVAN_INNER=$(tv blk.0.attn_kv_a_norm.weight 6); KVAN_HOLDS=$(tv blk.0.attn_kv_a_norm.weight 7)
RMS_EPS=0.0000009999999975
echo "  attn_norm: abs=$NORM_ABS view=$NORM_IDX inner=$NORM_INNER holds=$NORM_HOLDS"
echo "  attn_q_a (MXFP8 $QA_IN->$QA_OUT): abs=$QA_ABS view=$QA_IDX inner=$QA_INNER holds=$QA_HOLDS"
echo "  attn_kv  (MXFP8 $KV_IN->$KV_OUT): abs=$KV_ABS view=$KV_IDX inner=$KV_INNER holds=$KV_HOLDS"

# ── 3. compile the three translation units (embed, MLA norms, MXFP8 matvec), cached by sha ────────────
compile_unit() { # $1 emit-form  $2 grep-token  $3 cache-prefix -> echoes LIB path
    local form="$1" tok="$2" pre="$3" out lib sha
    echo "($form)" > "$work/$pre.fk"
    "$GO_BIN" "${FILES[@]}" "$work/$pre.fk" > "$work/$pre.out" 2>"$work/$pre.err" || { echo "FAIL $pre MSL emission" >&2; cat "$work/$pre.err" >&2; return 1; }
    awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/$pre.out" > "$work/$pre.metal"
    grep -q "$tok" "$work/$pre.metal" || { echo "FAIL $pre kernel $tok not emitted" >&2; return 1; }
    sha="$(shasum -a 256 "$work/$pre.metal" | cut -c1-16)"; lib="$CACHE/$pre-$sha.metallib"
    if [[ ! -f "$lib" ]]; then
        xcrun -sdk macosx metal -O2 -std=metal3.0 -c "$work/$pre.metal" -o "$work/$pre.air" 2>"$work/$pre.merr" \
          && xcrun -sdk macosx metallib "$work/$pre.air" -o "$lib" 2>>"$work/$pre.merr" || { echo "FAIL $pre metal compile" >&2; cat "$work/$pre.merr" >&2; return 1; }
        echo "PASS  $pre metallib compiled: $(basename "$lib")" >&2
    else
        echo "PASS  $pre metallib cache HIT: $(basename "$lib")" >&2
    fi
    printf '%s\n' "$lib"
}
mkdir -p "$CACHE"
LIB_EMB="$(compile_unit dsv4-embed-msl form_dsv4_embed_f16 dsv4emb)" || exit 1
LIB_MLA="$(compile_unit dsv4-mla-unit form_mla_rmsnorm_f32 dsv4mla)" || exit 1
LIB8="$(compile_unit dsv4-mx8-matvec-msl form_dsv4_mx8_matvec dsv4mx8)" || exit 1

# ── 4. the carrier ────────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

let a = CommandLine.arguments
var ai = 1
func S() -> String { let v = a[ai]; ai += 1; return v }
func I() -> Int { let v = Int(a[ai])!; ai += 1; return v }
func F() -> Float { let v = Float(a[ai])!; ai += 1; return v }
let libEmb = S(), libMla = S(), lib8 = S(), blobPath = S()
let step = I(), viewLimit = I(), nviews = I()
let embAbs = I(), rowOff = I(), nEmbd = I(), embIdx = I(), embInner = I(), embHolds = I(), token = I()
let normAbs = I(), normIdx = I(), normInner = I(), normHolds = I()
let qaAbs = I(), qaIdx = I(), qaInner = I(), qaHolds = I(), qaRows = I(), qaCols = I()
let qanAbs = I(), qanIdx = I(), qanInner = I(), qanHolds = I()
let qbAbs = I(), qbIdx = I(), qbInner = I(), qbHolds = I(), qbRows = I(), qbCols = I()
let kvAbs = I(), kvIdx = I(), kvInner = I(), kvHolds = I(), kvRows = I(), kvCols = I()
let kvanAbs = I(), kvanIdx = I(), kvanInner = I(), kvanHolds = I()
let eps = F()

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let lEmb = try dev.makeLibrary(URL: URL(fileURLWithPath: libEmb))
let lMla = try dev.makeLibrary(URL: URL(fileURLWithPath: libMla))
let l8   = try dev.makeLibrary(URL: URL(fileURLWithPath: lib8))
let queue = dev.makeCommandQueue()!
var failures = 0, gpuErrors = 0
var gpuFirstError: String? = nil
func check(_ ok: Bool, _ pass: String, _ fail: String) { if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 } }

func f16_to_f32(_ h: UInt16) -> Float {
    let sign = UInt32(h & 0x8000) << 16
    let exp  = Int((h >> 10) & 0x1F)
    let mant = UInt32(h & 0x3FF)
    if exp == 0 {
        if mant == 0 { return Float(bitPattern: sign) }
        var m = mant, e = -1
        repeat { m <<= 1; e += 1 } while (m & 0x400) == 0
        m &= 0x3FF
        return Float(bitPattern: sign | UInt32(127 - 15 - e) << 23 | (m << 13))
    } else if exp == 0x1F {
        return Float(bitPattern: sign | 0x7F800000 | (mant << 13))
    }
    return Float(bitPattern: sign | UInt32(exp - 15 + 127) << 23 | (mant << 13))
}
// MXFP8 (E4M3 payload, E8M0 scale) — the carrier's independent decode, textually mxfp8-msl.fk's.
func mxm_pow2(_ e: Int) -> Float {
    var aa: Float = 1.0, k = e
    while k >= 8 { aa *= 256.0; k -= 8 }; while k > 0 { aa *= 2.0; k -= 1 }
    while k <= -8 { aa *= 0.00390625; k += 8 }; while k < 0 { aa *= 0.5; k += 1 }
    return aa
}
func mxm_e8m0(_ e: Int) -> Float { return mxm_pow2(e - 127) }
func mx8_val(_ b: Int) -> Float {
    let mant = b % 8, ex = (b / 8) % 16, sgn = b / 128
    let frac = Float(mant) / 8.0
    let mag = (ex == 0) ? (mxm_pow2(-6) * frac) : (mxm_pow2(ex - 7) * (1.0 + frac))
    return (sgn == 1) ? -mag : mag
}

let fd = open(blobPath, O_RDONLY)
guard fd >= 0 else { print("FAIL cannot open blob"); exit(1) }
var st = stat(); fstat(fd, &st)
let fileLen = Int(st.st_size); let page = Int(getpagesize())
let mapLen = (fileLen + page - 1) / page * page
guard let mapped0 = mmap(nil, mapLen, PROT_READ, MAP_PRIVATE, fd, 0), mapped0 != MAP_FAILED else { print("FAIL mmap failed"); exit(1) }
let base = mapped0.assumingMemoryBound(to: UInt8.self)

// endian/alignment-safe F32 tensor read from the mmap (the RMSNorm weight g).
func readF32(_ off: Int, _ cnt: Int) -> [Float] {
    var out = [Float](repeating: 0, count: cnt)
    for i in 0..<cnt {
        let p = off + i*4
        let u = UInt32(base[p]) | (UInt32(base[p+1])<<8) | (UInt32(base[p+2])<<16) | (UInt32(base[p+3])<<24)
        out[i] = Float(bitPattern: u)
    }
    return out
}
// the CPU reference RMSNorm (ln-rmsnorm): sumsq LEFT fold, out = (x*1/rms)*g.
func rmsnorm(_ x: [Float], _ g: [Float]) -> [Float] {
    var ss: Float = 0; for v in x { ss += v*v }
    let inv = 1.0 / sqrtf(ss/Float(x.count) + eps)
    var o = [Float](repeating: 0, count: x.count)
    for i in 0..<x.count { o[i] = (x[i]*inv)*g[i] }
    return o
}
// the CPU reference MXFP8 fused matvec at a tensor's ABSOLUTE offset; rows=out, cols=in, x is cols-wide.
func mx8matvec(_ absOff: Int, _ x: [Float], _ rows: Int, _ cols: Int) -> [Float] {
    let qb = base.advanced(by: absOff); let nel = rows*cols
    var y = [Float](repeating: 0, count: rows)
    for r in 0..<rows {
        var acc: Float = 0; let rowPay = r*cols; let g0 = rowPay/32; var g = 0
        while g < cols/32 {
            let s = mxm_e8m0(Int(qb[nel + g0 + g])); var a2: Float = 0; let pbase = rowPay + g*32
            for m in 0..<32 { a2 += x[g*32 + m] * mx8_val(Int(qb[pbase + m])) }
            acc += s*a2; g += 1
        }
        y[r] = acc
    }
    return y
}
// compare a GPU buffer to a CPU reference: (ok, maxAbs, maxRel, nan, distinct, min, max).
// The weight decode (MXFP8 E4M3 * E8M0, or F32 g) is bit-exact; a matvec / Newton-sqrt reassociates, so
// the honest bound is float precision: an ABSOLUTE bound everywhere (catches every element), and a
// RELATIVE bound taken only over entries with |cpu| > 1e-3 (below that, an 8e-8 abs diff reads as a large
// relative purely because the denominator is ~0 — a near-zero-denominator artifact, not an error).
func cmp(_ gpu: UnsafeMutablePointer<Float>, _ cpu: [Float]) -> (Bool, Float, Float, Int, Int, Float, Float) {
    var maxAbs: Float = 0, maxRel: Float = 0, nan = 0
    var seen = Set<UInt32>(); var vmin = Float.greatestFiniteMagnitude, vmax = -Float.greatestFiniteMagnitude
    for i in 0..<cpu.count {
        let g = gpu[i]
        if g.isNaN || !g.isFinite { nan += 1; continue }
        let d = abs(g - cpu[i])
        if d > maxAbs { maxAbs = d }
        if abs(cpu[i]) > 1e-3 { let r = d/abs(cpu[i]); if r > maxRel { maxRel = r } }
        seen.insert(g.bitPattern); vmin = min(vmin, g); vmax = max(vmax, g)
    }
    let nonDegen = seen.count > 8 && vmax > vmin
    return (nan == 0 && maxRel < 1e-3 && maxAbs < 1e-4 && nonDegen, maxAbs, maxRel, nan, seen.count, vmin, vmax)
}

// ── GATE 0: the views map. One buffer over the whole file FAILs (onelean); build the overlapping set.
var views: [MTLBuffer] = []
for i in 0..<nviews {
    let vs = i*step; let vlen = min(viewLimit, mapLen - vs)
    guard vs % page == 0 else { print("FAIL view \(i) start not page-aligned"); exit(1) }
    guard let buf = dev.makeBuffer(bytesNoCopy: mapped0.advanced(by: vs), length: vlen, options: .storageModeShared, deallocator: nil) else {
        print("FAIL view \(i) makeBuffer failed"); failures += 1; break
    }
    views.append(buf)
}
check(views.count == nviews,
  "gate 0 the views map: all \(nviews) overlapping page-aligned bytesNoCopy views of the \(fileLen) B file wrap on \(dev.name) (one buffer over the whole file FAILs; \(nviews) views do not)",
  "gate 0 only \(views.count)/\(nviews) views mapped")
if failures > 0 { print("VERDICT FAIL the views did not map"); exit(1) }

// ── GATE 1: the five MLA tensors are each resident in a single view (holds==1).
let resident = embHolds==1 && normHolds==1 && qaHolds==1 && qanHolds==1 && qbHolds==1 && kvHolds==1 && kvanHolds==1
  && embIdx<nviews && normIdx<nviews && qaIdx<nviews && qanIdx<nviews && qbIdx<nviews && kvIdx<nviews && kvanIdx<nviews
check(resident,
  "gate 1 residency: token_embd(v\(embIdx)), attn_norm(v\(normIdx)), attn_q_a(v\(qaIdx)), attn_q_a_norm(v\(qanIdx)), attn_q_b(v\(qbIdx)), attn_kv(v\(kvIdx)), attn_kv_a_norm(v\(kvanIdx)) each lie wholly inside one view",
  "gate 1 an MLA tensor spans views (holds: emb\(embHolds) norm\(normHolds) qa\(qaHolds) qan\(qanHolds) kv\(kvHolds) kvan\(kvanHolds))")
if failures > 0 { print("VERDICT FAIL"); exit(1) }

// helper: dispatch the one-thread RMSNorm kernel; x is a device buffer, g bound from a view at gOff.
let pRms = try dev.makeComputePipelineState(function: lMla.makeFunction(name: "form_mla_rmsnorm_f32")!)
func gpuRmsnorm(_ x: MTLBuffer, _ n: Int, _ gView: MTLBuffer, _ gOff: Int) -> MTLBuffer {
    let out = dev.makeBuffer(length: n*4, options: .storageModeShared)!
    let op = out.contents().bindMemory(to: Float.self, capacity: n)
    for i in 0..<n { op[i] = Float.nan }               // SENTINEL
    var n32 = UInt32(n), e = eps
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(pRms)
    enc.setBuffer(x, offset: 0, index: 0)
    enc.setBuffer(gView, offset: gOff, index: 1)
    enc.setBuffer(out, offset: 0, index: 2)
    enc.setBytes(&n32, length: 4, index: 3); enc.setBytes(&e, length: 4, index: 4)
    enc.dispatchThreads(MTLSize(width: 1, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: 1, height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let er = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(er)" } }
    if cb.status != .completed { gpuErrors += 1 }
    return out
}
// helper: dispatch the MXFP8 fused matvec; qb bound from a view at qbOff, x device buffer, -> y[rows].
let p8 = try dev.makeComputePipelineState(function: l8.makeFunction(name: "form_dsv4_mx8_matvec")!)
func gpuMx8(_ qbView: MTLBuffer, _ qbOff: Int, _ x: MTLBuffer, _ rows: Int, _ cols: Int) -> MTLBuffer {
    let out = dev.makeBuffer(length: rows*4, options: .storageModeShared)!
    let op = out.contents().bindMemory(to: Float.self, capacity: rows)
    for i in 0..<rows { op[i] = Float.nan }            // SENTINEL
    var r32 = UInt32(rows), c32 = UInt32(cols), nel32 = UInt32(rows*cols)
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(p8)
    enc.setBuffer(qbView, offset: qbOff, index: 0)
    enc.setBuffer(x, offset: 0, index: 1)
    enc.setBuffer(out, offset: 0, index: 2)
    enc.setBytes(&r32, length: 4, index: 3); enc.setBytes(&c32, length: 4, index: 4); enc.setBytes(&nel32, length: 4, index: 5)
    let tg = min(p8.maxTotalThreadsPerThreadgroup, 256)
    enc.dispatchThreads(MTLSize(width: rows*32, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let er = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(er)" } }
    if cb.status != .completed { gpuErrors += 1 }
    return out
}

// ── the probe vector x0: the token's real F16 embedding, GPU-decoded and CPU-carved (both independent).
let pEmb = try dev.makeComputePipelineState(function: lEmb.makeFunction(name: "form_dsv4_embed_f16")!)
let x0 = dev.makeBuffer(length: nEmbd*4, options: .storageModeShared)!
let x0p = x0.contents().bindMemory(to: Float.self, capacity: nEmbd)
for i in 0..<nEmbd { x0p[i] = Float(bitPattern: 0x7F7FFFFF) }
var b64 = UInt64(embInner + rowOff), c32 = UInt32(nEmbd)
do {
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(pEmb)
    enc.setBuffer(views[embIdx], offset: 0, index: 0); enc.setBuffer(x0, offset: 0, index: 1)
    enc.setBytes(&b64, length: 8, index: 2); enc.setBytes(&c32, length: 4, index: 3)
    let tg = min(pEmb.maxTotalThreadsPerThreadgroup, 256)
    enc.dispatchThreads(MTLSize(width: nEmbd, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let er = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(er)" } }
}
let rowAbs = embAbs + rowOff
var x0cpu = [Float](repeating: 0, count: nEmbd)
for i in 0..<nEmbd { x0cpu[i] = f16_to_f32(UInt16(base[rowAbs+i*2]) | (UInt16(base[rowAbs+i*2+1])<<8)) }

// ── GATE 2: the INPUT RMSNorm (attn_norm) at real dims through view \(normIdx).
let xnBuf = gpuRmsnorm(x0, nEmbd, views[normIdx], normInner)
let xnp = xnBuf.contents().bindMemory(to: Float.self, capacity: nEmbd)
let gNorm = readF32(normAbs, nEmbd)
let xnCpu = rmsnorm(x0cpu, gNorm)
let (ok2, ma2, mr2, nn2, ds2, mn2, mx2) = cmp(xnp, xnCpu)
check(ok2 && gpuErrors == 0,
  "gate 2 input RMSNorm at real dims: form_mla_rmsnorm_f32 over the \(nEmbd)-wide probe with attn_norm's F32 g read through view \(normIdx) agrees with the CPU carve (maxRel \(mr2), maxAbs \(ma2); \(ds2) distinct, range [\(mn2),\(mx2)]; \(nn2) NaN)",
  "gate 2 input RMSNorm: maxRel \(mr2) maxAbs \(ma2) nan \(nn2) distinct \(ds2) gpuErrors \(gpuErrors)")

// ── GATE 3: the Q down-projection (attn_q_a, MXFP8 4096->1024) fused decode+matvec through view \(qaIdx).
let qLatBuf = gpuMx8(views[qaIdx], qaInner, xnBuf, qaRows, qaCols)
let qLatp = qLatBuf.contents().bindMemory(to: Float.self, capacity: qaRows)
let qLatCpu = mx8matvec(qaAbs, xnCpu, qaRows, qaCols)
let (ok3, ma3, mr3, nn3, ds3, mn3, mx3) = cmp(qLatp, qLatCpu)
check(ok3 && gpuErrors == 0,
  "gate 3 Q down-projection at real dims: the type-41 attn_q_a (\(qaRows)x\(qaCols)) fused decode+matvec through view \(qaIdx) agrees with the CPU MXFP8 carve on all \(qaRows) latents to float precision (maxRel \(mr3), maxAbs \(ma3); \(ds3) distinct, range [\(mn3),\(mx3)]; \(nn3) NaN)",
  "gate 3 Q down: maxRel \(mr3) maxAbs \(ma3) nan \(nn3) distinct \(ds3) gpuErrors \(gpuErrors)")

// ── GATE 4: the Q rank-space RMSNorm (attn_q_a_norm, 1024) through view \(qanIdx).
let qLatNBuf = gpuRmsnorm(qLatBuf, qaRows, views[qanIdx], qanInner)
let qLatNp = qLatNBuf.contents().bindMemory(to: Float.self, capacity: qaRows)
let gQan = readF32(qanAbs, qaRows)
let qLatNCpu = rmsnorm(qLatCpu, gQan)
let (ok4, ma4, mr4, nn4, ds4, _, _) = cmp(qLatNp, qLatNCpu)
check(ok4 && gpuErrors == 0,
  "gate 4 Q rank-space RMSNorm at real dims: form_mla_rmsnorm_f32 over the \(qaRows)-wide Q latent with attn_q_a_norm's g through view \(qanIdx) agrees with the CPU carve (maxRel \(mr4), maxAbs \(ma4); \(ds4) distinct; \(nn4) NaN)",
  "gate 4 Q rank-norm: maxRel \(mr4) maxAbs \(ma4) nan \(nn4)")

// ── GATE 7: the Q up-projection (attn_q_b, MXFP8 1024->32768 = n_head*head_dim) through view \(qbIdx).
// This completes the MLA PROJECTION surface: q = q_b · q_a_norm(q_a·xn), the per-head query stack.
let qBuf = gpuMx8(views[qbIdx], qbInner, qLatNBuf, qbRows, qbCols)
let qp = qBuf.contents().bindMemory(to: Float.self, capacity: qbRows)
let qCpu = mx8matvec(qbAbs, qLatNCpu, qbRows, qbCols)
let (ok7, ma7, mr7, nn7, ds7, mn7, mx7) = cmp(qp, qCpu)
check(ok7 && gpuErrors == 0,
  "gate 7 Q up-projection at real dims: the type-41 attn_q_b (\(qbRows)x\(qbCols)) fused decode+matvec through view \(qbIdx) agrees with the CPU MXFP8 carve on all \(qbRows) query elements (= n_head 64 * head_dim 512) to float precision (maxRel \(mr7), maxAbs \(ma7); \(ds7) distinct, range [\(mn7),\(mx7)]; \(nn7) NaN)",
  "gate 7 Q up: maxRel \(mr7) maxAbs \(ma7) nan \(nn7) distinct \(ds7) gpuErrors \(gpuErrors)")

// ── GATE 5: the KV down-projection (attn_kv, MXFP8 4096->512) fused decode+matvec through view \(kvIdx).
let kvLatBuf = gpuMx8(views[kvIdx], kvInner, xnBuf, kvRows, kvCols)
let kvLatp = kvLatBuf.contents().bindMemory(to: Float.self, capacity: kvRows)
let kvLatCpu = mx8matvec(kvAbs, xnCpu, kvRows, kvCols)
let (ok5, ma5, mr5, nn5, ds5, mn5, mx5) = cmp(kvLatp, kvLatCpu)
check(ok5 && gpuErrors == 0,
  "gate 5 KV down-projection at real dims: the type-41 attn_kv (\(kvRows)x\(kvCols)) fused decode+matvec through view \(kvIdx) agrees with the CPU MXFP8 carve on all \(kvRows) latents (maxRel \(mr5), maxAbs \(ma5); \(ds5) distinct, range [\(mn5),\(mx5)]; \(nn5) NaN)",
  "gate 5 KV down: maxRel \(mr5) maxAbs \(ma5) nan \(nn5) distinct \(ds5) gpuErrors \(gpuErrors)")

// ── GATE 6: the KV rank-space RMSNorm (attn_kv_a_norm, 512) through view \(kvanIdx).
let kvLatNBuf = gpuRmsnorm(kvLatBuf, kvRows, views[kvanIdx], kvanInner)
let kvLatNp = kvLatNBuf.contents().bindMemory(to: Float.self, capacity: kvRows)
let gKvan = readF32(kvanAbs, kvRows)
let kvLatNCpu = rmsnorm(kvLatCpu, gKvan)
let (ok6, ma6, mr6, nn6, ds6, _, _) = cmp(kvLatNp, kvLatNCpu)
check(ok6 && gpuErrors == 0,
  "gate 6 KV rank-space RMSNorm at real dims: form_mla_rmsnorm_f32 over the \(kvRows)-wide KV latent with attn_kv_a_norm's g through view \(kvanIdx) agrees with the CPU carve (maxRel \(mr6), maxAbs \(ma6); \(ds6) distinct; \(nn6) NaN)",
  "gate 6 KV rank-norm: maxRel \(mr6) maxAbs \(ma6) nan \(nn6)")

if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER ERROR(S) — first: \(gpuFirstError ?? "unknown") ===") }
print(String(format: "      Q latent[0..3] = %.6f %.6f %.6f %.6f", qLatp[0], qLatp[1], qLatp[2], qLatp[3]))
print(String(format: "      KV latent[0..3] = %.6f %.6f %.6f %.6f", kvLatp[0], kvLatp[1], kvLatp[2], kvLatp[3]))
print(String(format: "      device.currentAllocatedSize = %ld B (%.2f GiB) — the model is mmapped and wrapped, not copied (onelean)", dev.currentAllocatedSize, Double(dev.currentAllocatedSize)/1073741824.0))

let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  8 gates — Stone 35 Stage 1: the WHOLE MLA projection surface at real dims (input RMSNorm + Q down/rank-norm/up + KV down/rank-norm), through the windowed views, each == an independent CPU carve") }
else { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT
swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || { echo "FAIL swiftc runner"; tail -30 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB_EMB" "$LIB_MLA" "$LIB8" "$BLOB" \
    "$STEP" "$VIEWLIMIT" "$NVIEWS" \
    "$EMB_ABS" "$ROW_OFF" "$N_EMBD" "$EMB_IDX" "$EMB_INNER" "$EMB_HOLDS" "$TOKEN" \
    "$NORM_ABS" "$NORM_IDX" "$NORM_INNER" "$NORM_HOLDS" \
    "$QA_ABS" "$QA_IDX" "$QA_INNER" "$QA_HOLDS" "$QA_OUT" "$QA_IN" \
    "$QAN_ABS" "$QAN_IDX" "$QAN_INNER" "$QAN_HOLDS" \
    "$QB_ABS" "$QB_IDX" "$QB_INNER" "$QB_HOLDS" "$QB_OUT" "$QB_IN" \
    "$KV_ABS" "$KV_IDX" "$KV_INNER" "$KV_HOLDS" "$KV_OUT" "$KV_IN" \
    "$KVAN_ABS" "$KVAN_IDX" "$KVAN_INNER" "$KVAN_HOLDS" \
    "$RMS_EPS"
rc=$?
exit $rc
