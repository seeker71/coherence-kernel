#!/usr/bin/env bash
# metal_moe_gpu.sh — the DeepSeek-V4-Flash routed Mixture-of-Experts FFN on the GPU, made a witness.
#
# THE CLAIM, and nothing wider: the routed-MoE + shared FFN DeepSeek-V4-Flash uses — the sqrt(softplus)
# router with its biased selection and UNbiased weighting, the top-k gather, the clamped-SwiGLU expert fold,
# the weighted accumulation of the chosen experts, and the shared expert added alongside — runs as Metal
# kernels the BODY emitted (form-stdlib/dsv4-router-msl.fk + form-stdlib/dsv4-moe-msl.fk), and its f32 output
# agrees with the fp64 CPU recipe (form-stdlib/dsv4-forward.fk, band 63) at toy dims, STAGE BY STAGE and end
# to end, within the f32 working-precision envelope. Two clamp configs (snugcause): lim 0.9 (the clamp bites)
# and lim 0.0 (disabled), and the two asymmetries the stone rests on are witnessed as load-bearing on the GPU
# itself: the router weights by the UNbiased prob not the biased score (gate 9), and the clamp changes the
# fold (gate 8).
#
# THE EVIDENCE CLASS (aporon / selfgauge), named before the gates:
#   * NOT bit-exact. The kernels transcribe THIS body's own deterministic numerics (tn-exp's 14-term Taylor,
#     fln's atanh series over the reduced mantissa, tn-sqrt's 50-iter Newton) and fold every reduction in the
#     recipe's direction (the matvec walks columns DESCENDING == tb-dot's right fold; the router sum and the
#     expert accumulate are left folds). So the ONLY difference from the fp64 recipe is the WORKING PRECISION,
#     f32 vs fp64 over an identical operation graph — not a different approximation, not a different order. The
#     gate is a relative envelope (TOL); the observed max is printed and is far tighter.
#   * NOT the real dims. Demo is E=8, ne=4 experts top-2, ff=6. V4-Flash is E=4096, 256 experts top-8, wider
#     ff. The kernels are dim-generic (all runtime uniforms) but no gate here has seen the real dims; the
#     router's fixed arrays cap ne<=256, nused<=8 and REFUSE above that.
#   * NOT the quantised expert weights (MXFP4/IQ2_XXS). This fold consumes f32 experts; a real ffn_down_exps
#     is IQ2_XXS (type 16) and NO GPU IQ2 dequant exists yet — named as the dependency a real-weight block
#     needs, not smuggled. NOT whole-model residency. This is the MoE arithmetic, on the GPU, once, at toy width.
#
# Who decides what (the dumb-carrier discipline):
#   the BODY  form-stdlib/dsv4-router-msl.fk + dsv4-moe-msl.fk  — the Metal source. Not one character here.
#   the BODY  form-stdlib/dsv4-moe-demo.fk                       — the toy fixture and every fp64 reference.
#   the CARRIER (this file + the Swift runner it writes)         — compile, bind, dispatch, gather, compare.
#
# THE SLOT (asktoll): form_dsv4_router_f32 is ONE thread (a serial max-free argmax over ne experts, nused
# times); the swiglu/matvec kernels are one-thread-per-output. The expert GATHER is host-side: the router
# returns ids, the carrier binds expert e's weights at e*ff*E / e*E*ff — the `t.off + e*nb02` decode gather.
#
# THE GATES:
#   0  DID THE GPU RUN      a sentinelled logits buffer is overwritten by a real matvec, no cb error
#                           (an unrun kernel reads as a computed zero — edgedrop/zerobirth)
#   1  ROUTER LOGITS        matvec(gate_inp, norm) == recipe's logits within TOL
#   2  ROUTER DECISION      ids == recipe's sel (exact), wts == recipe's ews within TOL, wts NONZERO
#   3  EXPERT GATE/UP       the chosen expert's gate & up projections == recipe's gate0/up0 within TOL
#   4  CLAMPED SWIGLU       silu(clamp gate)*clamp(up)*w == recipe's mid0 within TOL
#   5  EXPERT OUT           down . mid == recipe's eout0 within TOL
#   6  ROUTED MoE           the weighted sum of the chosen experts == recipe's moeOn within TOL
#   7  MoE + SHARED         routed + shared expert == recipe's totalOn within TOL
#   8  CLAMP LOAD-BEARING   lim 0.0 output == recipe's totalOff AND differs from lim 0.9 by > 1e-3 on the GPU
#   9  BIAS ASYMMETRY       the kernel's wts == unbiased weighting and DIFFER from biased weighting by > 1e-3
#  10  RESIDENCY            ITERS re-dispatches, output checksum unchanged, weights never re-uploaded
#  11  ONE HEADER, ONE SPINE  exactly one metal_stdlib, zero using-namespace, one dm_ spine, 5 kernels
#
# Run:  form/native/metal/metal_moe_gpu.sh [iters]     (default 200)
# Off-Mac (or no swiftc) it SKIPs with exit 2, like every Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
ITERS="${1:-200}"
CACHE="$ROOT/native/metal/.metallib-cache"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the GPU witness needs an Apple GPU + swiftc"
    exit 2
fi
if [[ ! -x "$GO_BIN" ]]; then
    echo "  building go kernel..." >&2
    (cd "$ROOT/form-kernel-go" && go build -o bin-go .)
fi

work="$(mktemp -d "${TMPDIR:-/tmp}/fkmoe.XXXXXX")"
trap 'rm -rf "$work"' EXIT

# ── the `; preludes:` directives are LIVE recursive load instructions; walked, never hand-catted ──
FK_SEEN=""
fk_deps() {
    awk '/^;[ \t]*preludes:/ { s=$0; sub(/^;[ \t]*preludes:[ \t]*/,"",s); gsub(/,/," ",s);
        n=split(s,a,/[ \t]+/); for(i=1;i<=n;i++){ low=tolower(a[i]);
        if(a[i]=="\\"||low=="none"||low=="(none)"||a[i]=="")continue; if(a[i]~/\.fk$/)print a[i] } }' "$1" 2>/dev/null
}
fk_path() {
    local dir; dir="$(dirname "$1")"
    if   [[ -f "$ROOT/$2" ]]; then printf '%s\n' "$ROOT/$2"
    elif [[ -f "$dir/$2" ]]; then printf '%s\n' "$dir/$2"
    elif [[ "$2" == form/* && -f "${2#form/}" ]]; then printf '%s\n' "${2#form/}"
    else printf '%s\n' "$ROOT/$2"; fi
}
fk_expand() {
    local f="$1" d p
    case " $FK_SEEN " in *" $f "*) return ;; esac
    FK_SEEN="$FK_SEEN $f"
    while read -r d; do [[ -z "$d" ]] && continue; p="$(fk_path "$f" "$d")"; fk_expand "$p"; done < <(fk_deps "$f")
    printf '%s\n' "$f"
}

cd "$ROOT"
FILES=()
while read -r x; do FILES+=("$x"); done < <(fk_expand form-stdlib/dsv4-moe-demo.fk)

# ── 1. the body emits the Metal source (5 kernels, one header, one spine) ───────────────────────
echo '(print (dsv4-moe-msl-unit))' > "$work/emit.fk"
"$GO_BIN" "${FILES[@]}" "$work/emit.fk" 2>"$work/emit.err" | sed '/^null$/d' > "$work/moe.metal" || {
    echo "FAIL  MSL emission failed"; cat "$work/emit.err"; exit 1; }
MSL="$work/moe.metal"
for k in form_dsv4_matvec_f32 form_dsv4_swiglu_f32 form_dsv4_scale_f32 form_dsv4_axpy_f32 form_dsv4_router_f32; do
    grep -q "kernel void $k" "$MSL" || { echo "FAIL  kernel $k was not emitted"; exit 1; }
done
# gate 11: ONE header, no using-namespace, one spine, 5 kernels
head -c 200 "$MSL" | grep -q '#include <metal_stdlib>' || { echo "FAIL  gate 11: metal_stdlib is not at the top"; exit 1; }
nhdr=$(grep -c 'metal_stdlib' "$MSL"); nusing=$(grep -c 'using namespace' "$MSL")
nspine=$(grep -o 'float dm_ln' "$MSL" | wc -l | tr -d ' ')
nkern=$(grep -o 'kernel void form_dsv4_' "$MSL" | wc -l | tr -d ' ')
[[ "$nhdr" == 1 && "$nusing" == 0 && "$nspine" == 1 && "$nkern" == 5 ]] || {
    echo "FAIL  gate 11: header $nhdr (want 1), using-namespace $nusing (want 0), spine $nspine (want 1), kernels $nkern (want 5)"; exit 1; }
echo "PASS  gate 11 one metal_stdlib, no using-namespace, one dm_ spine, 5 kernels: $(wc -c < "$MSL" | tr -d ' ') bytes, every byte authored by the body"

# ── 2. the .metallib, cached across RUNS by the source's own sha256 ─────────────────────────────
mkdir -p "$CACHE"
msl_sha="$(shasum -a 256 "$MSL" | cut -c1-16)"
LIB="$CACHE/moe-$msl_sha.metallib"
if [[ -f "$LIB" ]]; then
    echo "PASS  metallib cache HIT: $(basename "$LIB") (emitted source unchanged; no compile this run)"
else
    xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
          -c "$MSL" -o "$work/moe.air" 2>"$work/metal.err" \
      && xcrun -sdk macosx metallib "$work/moe.air" -o "$LIB" 2>>"$work/metal.err" || {
        echo "FAIL  offline metal compile failed"; cat "$work/metal.err"; exit 1; }
    echo "PASS  metallib cache MISS -> compiled and cached: $(basename "$LIB")"
fi

# ── 3. the body: the toy fixture, its fp64 references ───────────────────────────────────────────
"$GO_BIN" "${FILES[@]}" <(echo '(dmd-emit-all)') > "$work/demo.txt" 2>"$work/demo.err" || {
    echo "FAIL  demo emission failed"; tail -5 "$work/demo.err"; exit 1; }
grep -qx 'END' "$work/demo.txt" || { echo "FAIL  demo stream truncated"; exit 1; }

# ── 4. the carrier ──────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

let a = CommandLine.arguments
let libPath = a[1], demoPath = a[2]
let iters = Int(a[3])!

// --- parse: SCALAR name / value ; VEC name / floats.. / ENDVEC ---
var scalars: [String: Double] = [:]
var vecs: [String: [Double]] = [:]
do {
    var mode = "", name = ""
    for raw in try String(contentsOfFile: demoPath, encoding: .utf8).split(separator: "\n", omittingEmptySubsequences: false) {
        let s = String(raw)
        if s.hasPrefix("SCALAR ") { mode = "S"; name = String(s.dropFirst(7)); continue }
        if s.hasPrefix("VEC ")    { mode = "V"; name = String(s.dropFirst(4)); vecs[name] = []; continue }
        if s == "ENDVEC" { mode = ""; continue }
        if s == "MOEDEMO" || s == "END" { continue }
        guard let v = Double(s) else { continue }
        if mode == "S" { scalars[name] = v; mode = "" }
        else if mode == "V" { vecs[name]!.append(v) }
    }
}
func S(_ n: String) -> Int { Int(scalars[n]!) }
func F(_ n: String) -> Float { Float(scalars[n]!) }
let E = S("E"), ne = S("ne"), nused = S("nused"), ff = S("ff")
let wscale = F("wscale"), limOn = F("limOn"), limOff = F("limOff")

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
let queue = dev.makeCommandQueue()!
var failures = 0, dispatches = 0
var gpuErrors = 0; var gpuFirstError: String? = nil
func check(_ ok: Bool, _ pass: String, _ fail: String) {
    if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 }
}
func vbuf(_ name: String) -> MTLBuffer {
    let d = vecs[name]!.map { Float($0) }
    return dev.makeBuffer(bytes: d, length: max(4, d.count * 4), options: .storageModeShared)!
}
func scratch(_ n: Int) -> MTLBuffer { dev.makeBuffer(length: max(4, n * 4), options: .storageModeShared)! }

// resident weights + input
let bNorm = vbuf("norm"), bGateInp = vbuf("gateInp"), bBias = vbuf("bias")
let bExpGate = vbuf("expertsGate"), bExpUp = vbuf("expertsUp"), bExpDown = vbuf("expertsDown")
let bShGate = vbuf("shGate"), bShUp = vbuf("shUp"), bShDown = vbuf("shDown")

let pMv = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_dsv4_matvec_f32")!)
let pRt = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_dsv4_router_f32")!)
let pSw = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_dsv4_swiglu_f32")!)
let pSc = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_dsv4_scale_f32")!)
let pAx = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_dsv4_axpy_f32")!)

func dispatch(_ p: MTLComputePipelineState, width: Int, _ bind: (MTLComputeCommandEncoder) -> Void) {
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(p); bind(enc)
    let tg = min(p.maxTotalThreadsPerThreadgroup, 256)
    enc.dispatchThreads(MTLSize(width: max(1, width), height: 1, depth: 1),
                        threadsPerThreadgroup: MTLSize(width: min(tg, max(1, width)), height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let e = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(e)" } }
    if cb.status != .completed { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "status \(cb.status.rawValue)" } }
    dispatches += 1
}
// W bound at woElems*4; y at yoElems*4
func matvec(_ W: MTLBuffer, _ wo: Int, _ x: MTLBuffer, _ xo: Int, _ y: MTLBuffer, _ yo: Int, _ rows: Int, _ cols: Int) {
    var r = UInt32(rows); var c = UInt32(cols)
    dispatch(pMv, width: rows) { enc in
        enc.setBuffer(W, offset: wo * 4, index: 0); enc.setBuffer(x, offset: xo * 4, index: 1)
        enc.setBuffer(y, offset: yo * 4, index: 2)
        enc.setBytes(&r, length: 4, index: 3); enc.setBytes(&c, length: 4, index: 4)
    }
}
func router(_ logits: MTLBuffer, _ ids: MTLBuffer, _ wts: MTLBuffer) {
    var n = UInt32(ne); var k = UInt32(nused); var ws = wscale
    dispatch(pRt, width: 1) { enc in
        enc.setBuffer(logits, offset: 0, index: 0); enc.setBuffer(bBias, offset: 0, index: 1)
        enc.setBuffer(ids, offset: 0, index: 2); enc.setBuffer(wts, offset: 0, index: 3)
        enc.setBytes(&n, length: 4, index: 4); enc.setBytes(&k, length: 4, index: 5); enc.setBytes(&ws, length: 4, index: 6)
    }
}
func swiglu(_ gate: MTLBuffer, _ up: MTLBuffer, _ out: MTLBuffer, _ n: Int, _ w: Float, _ lim: Float) {
    var nn = UInt32(n); var ww = w; var ll = lim
    dispatch(pSw, width: n) { enc in
        enc.setBuffer(gate, offset: 0, index: 0); enc.setBuffer(up, offset: 0, index: 1); enc.setBuffer(out, offset: 0, index: 2)
        enc.setBytes(&nn, length: 4, index: 3); enc.setBytes(&ww, length: 4, index: 4); enc.setBytes(&ll, length: 4, index: 5)
    }
}
func scaleK(_ x: MTLBuffer, _ y: MTLBuffer, _ aa: Float, _ n: Int) {
    var a = aa; var nn = UInt32(n)
    dispatch(pSc, width: n) { enc in
        enc.setBuffer(x, offset: 0, index: 0); enc.setBuffer(y, offset: 0, index: 1)
        enc.setBytes(&a, length: 4, index: 2); enc.setBytes(&nn, length: 4, index: 3)
    }
}
func axpyK(_ x: MTLBuffer, _ y: MTLBuffer, _ aa: Float, _ n: Int) {
    var a = aa; var nn = UInt32(n)
    dispatch(pAx, width: n) { enc in
        enc.setBuffer(x, offset: 0, index: 0); enc.setBuffer(y, offset: 0, index: 1)
        enc.setBytes(&a, length: 4, index: 2); enc.setBytes(&nn, length: 4, index: 3)
    }
}

// scratch
let bLogits = scratch(ne), bIds = dev.makeBuffer(length: max(4, nused*4), options: .storageModeShared)!, bWts = scratch(nused)
let bGate = scratch(ff), bUp = scratch(ff), bMid = scratch(ff), bEout = scratch(E), bAcc = scratch(E)

func readF(_ b: MTLBuffer, _ o: Int, _ n: Int) -> [Float] {
    let p = b.contents().bindMemory(to: Float.self, capacity: o + n); return (0..<n).map { p[o + $0] }
}
func readU(_ b: MTLBuffer, _ n: Int) -> [UInt32] {
    let p = b.contents().bindMemory(to: UInt32.self, capacity: n); return (0..<n).map { p[$0] }
}
func cmp(_ got: [Float], _ ref: [Double]) -> (Double, Double) {
    var ma = 0.0, mr = 0.0
    for i in 0..<ref.count { let d = abs(Double(got[i]) - ref[i]); ma = max(ma, d); mr = max(mr, d / max(abs(ref[i]), 1e-9)) }
    return (ma, mr)
}
func vmax(_ x: [Float], _ y: [Float]) -> Double { var m = 0.0; for i in 0..<x.count { m = max(m, abs(Double(x[i]) - Double(y[i]))) }; return m }
let TOL = 5e-4
var worstRel = 0.0
func stage(_ label: String, _ got: [Float], _ ref: String) -> Bool {
    let (ma, mr) = cmp(got, vecs[ref]!); worstRel = max(worstRel, mr)
    check(mr < TOL, String(format: "%@ == recipe's %@ (rel %.3e, abs %.3e)", label, ref, mr, ma),
                    String(format: "%@: rel %.3e exceeds %.2e", label, mr, TOL))
    return mr < TOL
}

// the whole MoE for a given clamp; leaves the routed sum in bAcc after the expert loop, the total after shared.
// returns (ids, wts, moeOnly, total)
func computeMoE(_ lim: Float) -> ([UInt32], [Float], [Float], [Float]) {
    matvec(bGateInp, 0, bNorm, 0, bLogits, 0, ne, E)
    router(bLogits, bIds, bWts)
    let ids = readU(bIds, nused); let wts = readF(bWts, 0, nused)
    for j in 0..<nused {
        let e = Int(ids[j]); let w = wts[j]
        matvec(bExpGate, e*ff*E, bNorm, 0, bGate, 0, ff, E)
        matvec(bExpUp,   e*ff*E, bNorm, 0, bUp,   0, ff, E)
        swiglu(bGate, bUp, bMid, ff, w, lim)
        matvec(bExpDown, e*E*ff, bMid, 0, bEout, 0, E, ff)
        if j == 0 { scaleK(bEout, bAcc, 1.0, E) } else { axpyK(bEout, bAcc, 1.0, E) }
    }
    let moeOnly = readF(bAcc, 0, E)
    // shared expert, router weight 1.0, added onto the accumulator
    matvec(bShGate, 0, bNorm, 0, bGate, 0, ff, E)
    matvec(bShUp,   0, bNorm, 0, bUp,   0, ff, E)
    swiglu(bGate, bUp, bMid, ff, 1.0, lim)
    matvec(bShDown, 0, bMid, 0, bEout, 0, E, ff)
    axpyK(bEout, bAcc, 1.0, E)
    let total = readF(bAcc, 0, E)
    return (ids, wts, moeOnly, total)
}

// --- GATE 0: did the GPU run. Sentinel logits, one real matvec, demand overwrite, no cb error. ---
do {
    let sent: Float = -424242.0
    let p = bLogits.contents().bindMemory(to: Float.self, capacity: ne)
    for i in 0..<ne { p[i] = sent }
    let before = gpuErrors
    matvec(bGateInp, 0, bNorm, 0, bLogits, 0, ne, E)
    var survived = 0; for i in 0..<ne where p[i] == sent { survived += 1 }
    if gpuErrors > before { print("  command buffer ERROR: \(gpuFirstError ?? "unknown")") }
    check(gpuErrors == before && survived == 0,
      "gate 0 the GPU executes: a real matvec overwrote all \(ne) logit sentinels, no command buffer errored",
      "gate 0 THE GPU DID NOT RUN — \(survived)/\(ne) sentinels survived, \(gpuErrors - before) cb error(s)")
    if failures > 0 { print("VERDICT FAIL  the GPU did not run; no MoE arithmetic was witnessed"); exit(1) }
}

// --- run the clamp-active MoE and check every stage ---
let (ids, wts, moeOnly, totalOn) = computeMoE(limOn)

// gate 1: logits
_ = stage("gate 1 router logits matvec(gate_inp,norm)", readF(bLogits, 0, ne), "logits")

// gate 2: router decision — ids exact, wts within TOL, wts nonzero
do {
    let selRef = vecs["sel"]!.map { UInt32($0.rounded()) }
    var idsOk = ids.count == selRef.count
    for i in 0..<min(ids.count, selRef.count) where ids[i] != selRef[i] { idsOk = false }
    let (_, mr) = cmp(wts, vecs["ews"]!); worstRel = max(worstRel, mr)
    let wsum = wts.reduce(0) { $0 + $1 }
    let nonzero = wts.allSatisfy { $0 > 1e-3 } && wsum > 1e-3
    check(idsOk && mr < TOL && nonzero,
      String(format: "gate 2 router: ids %@ == recipe sel, wts == recipe ews (rel %.3e), weights nonzero (sum %.4f)", "\(ids)", mr, wsum),
      String(format: "gate 2 router: idsOk=%@ wtsRel=%.3e nonzero=%@ (ids %@ sel %@)", "\(idsOk)", mr, "\(nonzero)", "\(ids)", "\(selRef)"))
}

// gates 3-5: the first-chosen expert stage by stage (clamp on)
do {
    let e0 = Int(ids[0]); let w0 = wts[0]
    matvec(bExpGate, e0*ff*E, bNorm, 0, bGate, 0, ff, E)
    matvec(bExpUp,   e0*ff*E, bNorm, 0, bUp,   0, ff, E)
    let g3 = stage("gate 3 expert gate proj", readF(bGate, 0, ff), "gate0")
    let g3b = stage("gate 3 expert up proj",  readF(bUp,   0, ff), "up0")
    _ = (g3 && g3b)
    swiglu(bGate, bUp, bMid, ff, w0, limOn)
    _ = stage("gate 4 clamped SwiGLU mid", readF(bMid, 0, ff), "mid0")
    matvec(bExpDown, e0*E*ff, bMid, 0, bEout, 0, E, ff)
    _ = stage("gate 5 expert out (down . mid)", readF(bEout, 0, E), "eout0")
}

// gate 6: routed MoE (both experts accumulated); gate 7: + shared
_ = stage("gate 6 routed MoE (weighted expert sum)", moeOnly, "moeOn")
_ = stage("gate 7 MoE + shared expert", totalOn, "totalOn")

// gate 8: clamp load-bearing — lim 0.0 output matches totalOff AND differs from lim 0.9 on the GPU
do {
    let (_, _, _, totalOff) = computeMoE(limOff)
    let (_, mr) = cmp(totalOff, vecs["totalOff"]!); worstRel = max(worstRel, mr)
    let clampGap = vmax(totalOn, totalOff)
    check(mr < TOL && clampGap > 1e-3,
      String(format: "gate 8 clamp load-bearing: lim0 output == recipe totalOff (rel %.3e) and differs from lim0.9 by %.3e (>1e-3) ON THE GPU", mr, clampGap),
      String(format: "gate 8 clamp: totalOffRel=%.3e clampGap=%.3e", mr, clampGap))
}

// gate 9: bias asymmetry — the kernel's wts are the UNbiased weighting and differ from the biased one
do {
    // biased alternative: (probs[id]+bias[id]) normalized to wscale
    let probs = vecs["probs"]!, bias = vecs["bias"]!
    var biased = [Double](); for id in ids { biased.append(probs[Int(id)] + bias[Int(id)]) }
    let bs = biased.reduce(0, +)
    let biasedW = biased.map { Float($0 * Double(wscale) / bs) }
    let gap = vmax(wts, biasedW)
    // and confirm wts DO match the unbiased recipe ews (already gate 2, restated as the contrast)
    let (_, mrU) = cmp(wts, vecs["ews"]!)
    check(gap > 1e-3 && mrU < TOL,
      String(format: "gate 9 bias asymmetry: kernel wts == UNbiased ews (rel %.3e), differ from biased weighting by %.3e (>1e-3) — bias steers selection, prob sets weight", mrU, gap),
      String(format: "gate 9 bias asymmetry: unbiasedRel=%.3e biasedGap=%.3e", mrU, gap))
}

// gate 10: residency — re-dispatch the whole MoE, checksum unchanged, no re-upload
do {
    let (_, _, _, t0) = computeMoE(limOn)
    var first = 0.0; for v in t0 { first += Double(v) }
    let before = dispatches
    var lastSum = first
    for _ in 0..<iters { let (_, _, _, t) = computeMoE(limOn); lastSum = 0.0; for v in t { lastSum += Double(v) } }
    check(lastSum == first,
      "gate 10 residency: \(iters) MoE re-dispatches (\(dispatches - before) GPU dispatches), output checksum unchanged, weights never re-uploaded",
      "gate 10: the output checksum changed across re-dispatches")
}

print(String(format: "--- worst relative deviation across every stage and output: %.3e (gate %.2e). f32 vs fp64 over an identical operation graph; the residual is working precision, not the decomposition.", worstRel, TOL))
if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER(S) FAILED — first: \(gpuFirstError ?? "unknown") ==="); }
let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  12 gates, DeepSeek-V4 routed MoE on the GPU") } else { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc failed"; tail -30 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB" "$work/demo.txt" "$ITERS"
exit $?
