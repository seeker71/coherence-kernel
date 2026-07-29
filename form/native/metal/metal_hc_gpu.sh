#!/usr/bin/env bash
# metal_hc_gpu.sh — DeepSeek-V4-Flash hyper-connections on the GPU, made a witness.
#
# THE CLAIM, and nothing wider: the HC residual structure DeepSeek-V4-Flash runs its whole forward pass
# inside — the stream broadcast, hc_pre (no-weight RMSNorm, the fn matvec, the 20-iteration Sinkhorn split,
# the stream-reduce), hc_post (block-inject + the combine-mix with its load-bearing TRANSPOSE), and the
# output-head collapse — runs as Metal kernels the BODY emitted (form-stdlib/dsv4-hc-msl.fk), and its f32
# output agrees with the fp64 CPU recipe (form-stdlib/dsv4-hc.fk) on a coherent single HC-wrapped sublayer
# at n_hc=4 (V4-Flash's real stream count), n_embd=3, stage by stage AND end to end, within the f32
# working-precision envelope. The combine is asymmetric so the post transpose is witnessed (edgedrop): a
# kernel reading comb[src+dst*n_hc] instead of comb[dst+src*n_hc] fails the post stage.
#
# THE EVIDENCE CLASS, named before the gates and not after (aporon / selfgauge):
#   * NOT bit-exact. The kernels transcribe this body's OWN deterministic numerics (tn-exp's 14-term Taylor
#     with argument halving, tn-sqrt's 50-iter Newton, ln-sigmoid) and match every reduction's fold
#     direction (tb-dot is a RIGHT fold; the sum-of-squares and the stream/Sinkhorn sums are LEFT folds). So
#     the ONLY difference from the fp64 recipe is the WORKING PRECISION: f32 vs fp64 over an identical
#     operation graph. The 20 Sinkhorn iterations compound f32 rounding, so the gate is a relative envelope
#     (TOL); the OBSERVED max is printed and is far tighter than the gate.
#   * NOT the real dims. Demo is n_hc=4, n_embd=3, fn [24,12]. V4-Flash is n_hc=4, n_embd=4096, fn [16384,24],
#     43 layers x two HC pairs + an output head. The kernels are dim-generic (all dims are runtime uniforms)
#     but no gate here has seen the real dims. form_hc_split_f32's private combine array is capped at
#     HC_MAX_C (64 = 8*8); above n_hc=8 it is WRONG, not slow. V4-Flash's n_hc=4 (16 entries) is far inside.
#   * NOT the fp8/f16 stream encodings, NOT the attention/FFN/MoE the HC wraps, NOT a resident real weight
#     tensor, NOT a token. Same gaps dsv4-hc.fk's radius names. This is the HC wrapper, on the GPU, once.
#
# Who decides what (the dumb-carrier discipline):
#   the BODY  form-stdlib/dsv4-hc-msl.fk   — the Metal source. Not one character here.
#   the BODY  form-stdlib/dsv4-hc-demo.fk  — the toy fixture and every fp64 reference this carrier judges by.
#   the CARRIER (this file + the Swift runner it writes) — compile, bind, dispatch, compare.
#
# THE SLOT (asktoll): form_hc_split_f32 is one thread (the sequential 20-iteration Sinkhorn owns a lane);
# form_hc_post_f32 is one thread per dst stream (post[dst] is a lane constant, read once per stream).
#
# THE GATES:
#   0  DID THE GPU RUN            a sentinelled buffer is overwritten by a real dispatch, no cb error —
#                                 an unrun kernel reads as a computed zero (edgedrop/zerobirth)
#   1  BROADCAST                 the token broadcast to n_hc streams == recipe's bcast within TOL
#   2  NO-WEIGHT RMSNORM         rms(no weight) of the flat HC state == recipe's rmsflat within TOL
#   3  FN MATVEC                 fn . rmsflat == recipe's mix (2n_hc + n_hc^2 wide) within TOL
#   4  SINKHORN SPLIT            the pre/post/comb split (20 iterations) == recipe's split within TOL,
#                                and the GPU's combine is doubly stochastic (rows AND cols sum to 1)
#   5  STREAM-REDUCE (hc_pre)    sum_h stream[h]*pre[h] == recipe's preinp within TOL
#   6  POST (transpose combine)  block-inject + combine-mix reading comb[dst+src*n_hc] == recipe's post
#   7  OUTPUT-HEAD COLLAPSE      rms -> fn -> sigmoid weights -> stream-reduce == recipe's head within TOL
#   8  RESIDENCY                 ITERS re-dispatches of the whole sublayer, head checksum unchanged
#   9  ONE HEADER, ONE SPINE     exactly one metal_stdlib, zero using-namespace, one spine, 7 kernels
#
# Run:  form/native/metal/metal_hc_gpu.sh [iters]     (default 200)
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

work="$(mktemp -d "${TMPDIR:-/tmp}/fkhc.XXXXXX")"
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
while read -r x; do FILES+=("$x"); done < <(fk_expand form-stdlib/dsv4-hc-demo.fk)

# ── 1. the body emits the Metal source (seven kernels, one header, one spine) ───────────────────
# emit uses only dsv4-hc-msl.fk (its prelude is core.fk), a subset of the demo's deps
HC_SEEN=""; HC_FILES=()
FK_SEEN=""; while read -r x; do HC_FILES+=("$x"); done < <(fk_expand form-stdlib/dsv4-hc-msl.fk)
echo '(print (hc-msl-unit))' > "$work/emit.fk"
"$GO_BIN" "${HC_FILES[@]}" "$work/emit.fk" 2>"$work/emit.err" | sed '/^null$/d' > "$work/hc.metal" || {
    echo "FAIL  MSL emission failed"; cat "$work/emit.err"; exit 1; }
MSL="$work/hc.metal"
for k in form_hc_broadcast_f32 form_hc_rmsnorm_nw_f32 form_hc_matvec_f32 form_hc_split_f32 \
         form_hc_wsum_f32 form_hc_post_f32 form_hc_headw_f32; do
    grep -q "kernel void $k" "$MSL" || { echo "FAIL  kernel $k was not emitted"; exit 1; }
done
# gate 9: ONE header, no `using namespace metal;` (this body's `round` goes ambiguous otherwise), one spine
head -c 200 "$MSL" | grep -q '#include <metal_stdlib>' || { echo "FAIL  gate 9: metal_stdlib is not at the top"; exit 1; }
nhdr=$(grep -c 'metal_stdlib' "$MSL"); nusing=$(grep -c 'using namespace' "$MSL")
nspine=$(grep -o 'float hc_sqrt' "$MSL" | wc -l | tr -d ' ')
nkern=$(grep -o 'kernel void form_hc_' "$MSL" | wc -l | tr -d ' ')
[[ "$nhdr" == 1 && "$nusing" == 0 && "$nspine" == 1 && "$nkern" == 7 ]] || {
    echo "FAIL  gate 9: header $nhdr (want 1), using-namespace $nusing (want 0), spine $nspine (want 1), kernels $nkern (want 7)"; exit 1; }
echo "PASS  gate 9 one metal_stdlib, no using-namespace, one hc_ spine, 7 kernels: $(wc -c < "$MSL" | tr -d ' ') bytes, every byte authored by dsv4-hc-msl.fk"

# ── 2. the .metallib, cached across RUNS by the source's own sha256 ────────────────────────────
mkdir -p "$CACHE"
msl_sha="$(shasum -a 256 "$MSL" | cut -c1-16)"
LIB="$CACHE/hc-$msl_sha.metallib"
if [[ -f "$LIB" ]]; then
    echo "PASS  metallib cache HIT: $(basename "$LIB") (emitted source unchanged; no compile this run)"
else
    xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
          -c "$MSL" -o "$work/hc.air" 2>"$work/metal.err" \
      && xcrun -sdk macosx metallib "$work/hc.air" -o "$LIB" 2>>"$work/metal.err" || {
        echo "FAIL  offline metal compile failed"; cat "$work/metal.err"; exit 1; }
    echo "PASS  metallib cache MISS -> compiled and cached: $(basename "$LIB")"
fi

# ── 3. the body: the toy fixture, its fp64 references, the config ───────────────────────────────
"$GO_BIN" "${FILES[@]}" <(echo '(hcd-emit-all)') > "$work/demo.txt" 2>"$work/demo.err" || {
    echo "FAIL  demo emission failed"; tail -5 "$work/demo.err"; exit 1; }
grep -qx 'END' "$work/demo.txt" || { echo "FAIL  demo stream truncated"; exit 1; }

# ── 4. the carrier ─────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

let a = CommandLine.arguments
let libPath = a[1], demoPath = a[2]
let iters = Int(a[3])!

// --- parse the body's dump: SCALAR name / value; VEC name / floats.. / ENDVEC ---
var scalars: [String: Double] = [:]
var vecs: [String: [Double]] = [:]
do {
    var mode = "", name = ""
    for raw in try String(contentsOfFile: demoPath, encoding: .utf8).split(separator: "\n", omittingEmptySubsequences: false) {
        let s = String(raw)
        if s.hasPrefix("SCALAR ") { mode = "S"; name = String(s.dropFirst(7)); continue }
        if s.hasPrefix("VEC ")    { mode = "V"; name = String(s.dropFirst(4)); vecs[name] = []; continue }
        if s == "ENDVEC" { mode = ""; continue }
        if s == "HCDEMO" || s == "END" { continue }
        guard let v = Double(s) else { continue }
        if mode == "S" { scalars[name] = v; mode = "" }
        else if mode == "V" { vecs[name]!.append(v) }
    }
}
func S(_ n: String) -> Int { Int(scalars[n]!) }
func F(_ n: String) -> Float { Float(scalars[n]!) }
let n_hc = S("n_hc"), n_embd = S("n_embd"), fn_rows = S("fn_rows"), fn_cols = S("fn_cols")
let itersS = UInt32(S("iters"))
let eps = F("eps")

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
func scratch(_ n: Int) -> MTLBuffer { dev.makeBuffer(length: max(1, n) * 4, options: .storageModeShared)! }

// weights and inputs, resident
let bX = vbuf("x"), bResid = vbuf("resid"), bBlk = vbuf("blk")
let bScale = vbuf("scale"), bBase = vbuf("base"), bScaleh = vbuf("scaleh"), bBaseh = vbuf("baseh")
let bFn = vbuf("fn"), bFnh = vbuf("fnh")

let pBc = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_hc_broadcast_f32")!)
let pRm = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_hc_rmsnorm_nw_f32")!)
let pMv = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_hc_matvec_f32")!)
let pSp = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_hc_split_f32")!)
let pWs = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_hc_wsum_f32")!)
let pPo = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_hc_post_f32")!)
let pHw = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_hc_headw_f32")!)

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

// --- stage dispatchers ---
func broadcast(_ x: MTLBuffer, _ out: MTLBuffer) {
    var nh = UInt32(n_hc); var ne = UInt32(n_embd)
    dispatch(pBc, width: n_hc*n_embd) { enc in
        enc.setBuffer(x, offset: 0, index: 0); enc.setBuffer(out, offset: 0, index: 1)
        enc.setBytes(&nh, length: 4, index: 2); enc.setBytes(&ne, length: 4, index: 3)
    }
}
func rmsnorm(_ x: MTLBuffer, _ xo: Int, _ out: MTLBuffer, _ n: Int) {
    var nn = UInt32(n); var e = eps
    dispatch(pRm, width: 1) { enc in
        enc.setBuffer(x, offset: xo*4, index: 0); enc.setBuffer(out, offset: 0, index: 1)
        enc.setBytes(&nn, length: 4, index: 2); enc.setBytes(&e, length: 4, index: 3)
    }
}
func matvec(_ W: MTLBuffer, _ x: MTLBuffer, _ y: MTLBuffer, _ rows: Int, _ cols: Int) {
    var r = UInt32(rows); var c = UInt32(cols)
    dispatch(pMv, width: rows) { enc in
        enc.setBuffer(W, offset: 0, index: 0); enc.setBuffer(x, offset: 0, index: 1); enc.setBuffer(y, offset: 0, index: 2)
        enc.setBytes(&r, length: 4, index: 3); enc.setBytes(&c, length: 4, index: 4)
    }
}
func split(_ mix: MTLBuffer, _ out: MTLBuffer) {
    var nh = UInt32(n_hc); var it = itersS; var e = eps
    dispatch(pSp, width: 1) { enc in
        enc.setBuffer(mix, offset: 0, index: 0); enc.setBuffer(bScale, offset: 0, index: 1)
        enc.setBuffer(bBase, offset: 0, index: 2); enc.setBuffer(out, offset: 0, index: 3)
        enc.setBytes(&nh, length: 4, index: 4); enc.setBytes(&it, length: 4, index: 5); enc.setBytes(&e, length: 4, index: 6)
    }
}
// stream reduce: out[d] = sum_h streams[h*n_embd+d]*w[h].  w is a buffer with a float offset wo.
func wsum(_ streams: MTLBuffer, _ w: MTLBuffer, _ wo: Int, _ out: MTLBuffer) {
    var nh = UInt32(n_hc); var ne = UInt32(n_embd)
    dispatch(pWs, width: n_embd) { enc in
        enc.setBuffer(streams, offset: 0, index: 0); enc.setBuffer(w, offset: wo*4, index: 1); enc.setBuffer(out, offset: 0, index: 2)
        enc.setBytes(&nh, length: 4, index: 3); enc.setBytes(&ne, length: 4, index: 4)
    }
}
// post: block-inject + combine-mix. post weights at split offset po, comb at split offset co (floats).
func post(_ blk: MTLBuffer, _ resid: MTLBuffer, _ split: MTLBuffer, _ po: Int, _ co: Int, _ out: MTLBuffer) {
    var nh = UInt32(n_hc); var ne = UInt32(n_embd)
    dispatch(pPo, width: n_hc) { enc in
        enc.setBuffer(blk, offset: 0, index: 0); enc.setBuffer(resid, offset: 0, index: 1)
        enc.setBuffer(split, offset: po*4, index: 2); enc.setBuffer(split, offset: co*4, index: 3)
        enc.setBuffer(out, offset: 0, index: 4)
        enc.setBytes(&nh, length: 4, index: 5); enc.setBytes(&ne, length: 4, index: 6)
    }
}
func headw(_ pre: MTLBuffer, _ out: MTLBuffer) {
    var nh = UInt32(n_hc); var e = eps
    dispatch(pHw, width: n_hc) { enc in
        enc.setBuffer(pre, offset: 0, index: 0); enc.setBuffer(bScaleh, offset: 0, index: 1)
        enc.setBuffer(bBaseh, offset: 0, index: 2); enc.setBuffer(out, offset: 0, index: 3)
        enc.setBytes(&nh, length: 4, index: 4); enc.setBytes(&e, length: 4, index: 5)
    }
}

// scratch
let hcDim = n_hc*n_embd
let bBcast = scratch(hcDim), bRmsflat = scratch(hcDim), bMix = scratch(fn_rows), bSplit = scratch(fn_rows)
let bPreinp = scratch(n_embd), bPost = scratch(hcDim)
let bHrms = scratch(hcDim), bHpre = scratch(n_hc), bHw = scratch(n_hc), bHead = scratch(n_embd)

func readBuf(_ b: MTLBuffer, _ o: Int, _ n: Int) -> [Float] {
    let p = b.contents().bindMemory(to: Float.self, capacity: o + n); return (0..<n).map { p[o + $0] }
}
func cmp(_ got: [Float], _ ref: [Double]) -> (Double, Double) {
    var ma = 0.0, mr = 0.0
    for i in 0..<ref.count { let d = abs(Double(got[i]) - ref[i]); ma = max(ma, d); mr = max(mr, d / max(abs(ref[i]), 1e-9)) }
    return (ma, mr)
}
let TOL = 1e-4   // the f32 working-precision envelope (20 Sinkhorn iterations compound f32 rounding); observed printed, ~170x tighter
var worstRel = 0.0

// run the whole HC sublayer once; leaves every stage buffer populated
func sublayer() {
    broadcast(bX, bBcast)                                  // hc_from_plain_embedding
    rmsnorm(bResid, 0, bRmsflat, hcDim)                    // rms(no weight) of the flat HC state
    matvec(bFn, bRmsflat, bMix, fn_rows, fn_cols)          // fn . flat -> mix
    split(bMix, bSplit)                                    // the 20-iteration Sinkhorn split
    wsum(bResid, bSplit, 0, bPreinp)                       // stream-reduce by the pre weights (split[0..n_hc])
    post(bBlk, bResid, bSplit, n_hc, 2*n_hc, bPost)        // block-inject + transpose combine-mix
    // output-head collapse over the new streams
    rmsnorm(bPost, 0, bHrms, hcDim)
    matvec(bFnh, bHrms, bHpre, n_hc, fn_cols)
    headw(bHpre, bHw)
    wsum(bPost, bHw, 0, bHead)
}

// --- GATE 0: did the GPU run. Sentinel bBcast, run one real broadcast, demand it overwrote, no cb error. ---
do {
    let sent: Float = -424242.0
    let p = bBcast.contents().bindMemory(to: Float.self, capacity: hcDim)
    for i in 0..<hcDim { p[i] = sent }
    let before = gpuErrors
    broadcast(bX, bBcast)
    var survived = 0; for i in 0..<hcDim where p[i] == sent { survived += 1 }
    if gpuErrors > before { print("  command buffer ERROR: \(gpuFirstError ?? "unknown")") }
    check(gpuErrors == before && survived == 0,
      "gate 0 the GPU executes: a real broadcast dispatch overwrote all \(hcDim) sentinels, no command buffer errored",
      "gate 0 THE GPU DID NOT RUN — \(survived)/\(hcDim) sentinels survived, \(gpuErrors - before) cb error(s)")
    if failures > 0 { print("VERDICT FAIL  the GPU did not run; no HC arithmetic was witnessed"); exit(1) }
}

sublayer()

func stage(_ gate: Int, _ label: String, _ b: MTLBuffer, _ n: Int, _ ref: String) {
    let (ma, mr) = cmp(readBuf(b, 0, n), vecs[ref]!); worstRel = max(worstRel, mr)
    check(mr < TOL, String(format: "gate %d %@ == recipe's %@ (rel %.3e, abs %.3e)", gate, label, ref, mr, ma),
                    String(format: "gate %d %@: rel %.3e exceeds %.2e", gate, label, mr, TOL))
}
stage(1, "broadcast to n_hc streams", bBcast, hcDim, "bcast")
stage(2, "no-weight RMSNorm of the HC state", bRmsflat, hcDim, "rmsflat")
stage(3, "fn matvec", bMix, fn_rows, "mix")
stage(4, "the 20-iteration Sinkhorn split", bSplit, fn_rows, "split")

// gate 4b: the GPU's combine is doubly stochastic (rows AND columns sum to 1) — catches a broken Sinkhorn
do {
    let sp = readBuf(bSplit, 2*n_hc, n_hc*n_hc)   // c[src + dst*n_hc]
    var wrow = 0.0, wcol = 0.0
    for dst in 0..<n_hc { var s = 0.0; for src in 0..<n_hc { s += Double(sp[src + dst*n_hc]) }; wrow = max(wrow, abs(s - 1.0)) }
    for src in 0..<n_hc { var s = 0.0; for dst in 0..<n_hc { s += Double(sp[src + dst*n_hc]) }; wcol = max(wcol, abs(s - 1.0)) }
    check(wrow < 1e-3 && wcol < 1e-3,
      String(format: "gate 4b GPU combine is doubly stochastic after %d iterations (worst row dev %.2e, col dev %.2e)", Int(itersS), wrow, wcol),
      String(format: "gate 4b GPU combine NOT doubly stochastic: row dev %.2e, col dev %.2e", wrow, wcol))
}

stage(5, "stream-reduce (hc_pre input)", bPreinp, n_embd, "preinp")
stage(6, "post: block-inject + transpose combine-mix", bPost, hcDim, "post")
stage(7, "output-head collapse", bHead, n_embd, "head")

// --- gate 8: residency — re-dispatch the whole sublayer ITERS times, head checksum unchanged ---
do {
    sublayer()
    var first = 0.0; let o = readBuf(bHead, 0, n_embd); for v in o { first += Double(v) }
    let before = dispatches
    for _ in 0..<iters { sublayer() }
    var after = 0.0; let o2 = readBuf(bHead, 0, n_embd); for v in o2 { after += Double(v) }
    check(after == first,
      "gate 8 residency: \(iters) sublayer re-dispatches (\(dispatches - before) GPU dispatches), head checksum unchanged, weights never re-uploaded",
      "gate 8: the head checksum changed across re-dispatches")
}

print(String(format: "--- worst relative deviation across every stage and output: %.3e (gate %.2e). f32 vs fp64 over an identical operation graph; the residual is working precision, not the decomposition.", worstRel, TOL))
if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER(S) FAILED — first: \(gpuFirstError ?? "unknown") ==="); }
let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  10 gates, HC on the GPU") } else { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc failed"; tail -30 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB" "$work/demo.txt" "$ITERS"
exit $?
