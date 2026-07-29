#!/usr/bin/env bash
# metal_mla_gpu.sh — Multi-head Latent Attention on the GPU, made a witness.
#
# THE CLAIM, and nothing wider: the MLA attention block DeepSeek-V4-Flash uses — the low-rank Q/KV
# projections, the embed-space and rank-space RMSNorms, the per-head unweighted RMSNorm, the trailing-nrot
# RoPE, the single-latent-row attention with the per-head SINK logit, the inverse-RoPE, and the grouped
# low-rank output projection — runs as Metal kernels the BODY emitted (form-stdlib/mla-msl.fk), and its
# f32 output agrees with the fp64 CPU recipe (form-stdlib/mla-attn.fk) on a THREE-position example
# (positions 0, 1, 2) that exercises RoPE, stage by stage AND end to end, within the f32 working-precision
# envelope. RoPE is the identity at position 0 (hushfold, corpus row 865), so positions 1 and 2 are what
# witness it; position 2 is the distant case checked at every intermediate stage (snugcause).
#
# THE EVIDENCE CLASS, named before the gates and not after (aporon / selfgauge):
#   * NOT bit-exact. The kernels transcribe this body's OWN deterministic numerics (tn-exp's 14-term
#     Taylor with argument halving, tn-sqrt's 50-iter Newton, fsin/fcos's round-reduced 10-term Taylor)
#     and match every reduction's fold direction (tb-dot is a RIGHT fold; the sum-of-squares and the
#     weighted accumulate are LEFT folds). So the ONLY difference from the fp64 recipe is the WORKING
#     PRECISION: f32 vs fp64 over an identical operation graph — not a different approximation, not a
#     different summation order. The residual is f32 rounding, not the decomposition. The gate is a
#     relative envelope (TOL below); the OBSERVED max is printed, and it is far tighter than the gate.
#   * NOT the real dims. Demo is E=4, R=3, nh=2, hd=4, nrot=2, ng=2. V4-Flash is nh=64, hd=512, nrot=64.
#     The kernels are dim-generic (all dims are runtime uniforms) but no gate here has seen the real dims.
#   * NOT YaRN, NOT the E4M3 cache encoding, NOT a resident real weight tensor, NOT the residual/MoE half
#     of the layer. Same gaps mla-attn.fk's radius names. This is the attention block, on the GPU, once.
#
# Who decides what (the dumb-carrier discipline):
#   the BODY  form-stdlib/mla-msl.fk   — the Metal source. Not one character here.
#   the BODY  form-stdlib/mla-demo.fk  — the toy fixture and every fp64 reference this carrier judges by.
#   the CARRIER (this file + the Swift runner it writes) — compile, bind, dispatch, compare.
#
# THE SLOT (asktoll): form_mla_attend_f32 is one thread per head — sinks[h] and the softmax denominator
# are a lane constant, computed once per head, reused across the head's hd outputs.
#
# THE GATES:
#   0  DID THE GPU RUN            a sentinelled buffer is overwritten by a real dispatch, no cb error —
#                                 an unrun kernel reads as a computed zero (edgedrop/zerobirth)
#   1  RMSNORM STAGE             embed-space RMSNorm of x2 == recipe's n2 within TOL
#   2  Q PROJECTION STAGE        matvec/rmsnorm/matvec/head-rms == recipe's qh2 within TOL
#   3  FORWARD ROPE STAGE        the query rope (pos 2, sign +1) == recipe's q2 within TOL
#   4  SINK ATTENTION STAGE      the single-latent-row sink softmax == recipe's heads2 within TOL
#   5  INVERSE ROPE STAGE        the output un-rotation (pos 2, sign -1) == recipe's back2 within TOL
#   6  CACHE ROWS                the three fp64 cache rows r0,r1,r2 reproduced within TOL (unispan)
#   7  BLOCK OUTPUTS             the three fp64 block outputs o0,o1,o2 reproduced within TOL (unispan;
#                                o0 at pos 0 where rope is identity, o2 at pos 2 where it is not)
#   8  RESIDENCY                 ITERS re-dispatches of the block, output checksum unchanged, no re-upload
#   9  ONE HEADER, ONE SPINE     exactly one metal_stdlib, zero using-namespace, one spine, 5 kernels
#
# Run:  form/native/metal/metal_mla_gpu.sh [iters]     (default 200)
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

work="$(mktemp -d "${TMPDIR:-/tmp}/fkmla.XXXXXX")"
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
while read -r x; do FILES+=("$x"); done < <(fk_expand form-stdlib/mla-demo.fk)

# ── 1. the body emits the Metal source (five kernels, one header, one spine) ───────────────────
echo '(print (mla-msl-unit))' > "$work/emit.fk"
"$GO_BIN" "${FILES[@]}" "$work/emit.fk" 2>"$work/emit.err" | sed '/^null$/d' > "$work/mla.metal" || {
    echo "FAIL  MSL emission failed"; cat "$work/emit.err"; exit 1; }
MSL="$work/mla.metal"
for k in form_mla_rmsnorm_f32 form_mla_matvec_f32 form_mla_headrms_f32 form_mla_rope_f32 form_mla_attend_f32; do
    grep -q "kernel void $k" "$MSL" || { echo "FAIL  kernel $k was not emitted"; exit 1; }
done
# gate 9: ONE header, no `using namespace metal;` (this body's `round` goes ambiguous otherwise), one spine
head -c 200 "$MSL" | grep -q '#include <metal_stdlib>' || { echo "FAIL  gate 9: metal_stdlib is not at the top"; exit 1; }
nhdr=$(grep -c 'metal_stdlib' "$MSL"); nusing=$(grep -c 'using namespace' "$MSL")
nspine=$(grep -o 'float mla_sqrt' "$MSL" | wc -l | tr -d ' ')
nkern=$(grep -o 'kernel void form_mla_' "$MSL" | wc -l | tr -d ' ')
[[ "$nhdr" == 1 && "$nusing" == 0 && "$nspine" == 1 && "$nkern" == 5 ]] || {
    echo "FAIL  gate 9: header $nhdr (want 1), using-namespace $nusing (want 0), spine $nspine (want 1), kernels $nkern (want 5)"; exit 1; }
echo "PASS  gate 9 one metal_stdlib, no using-namespace, one mla_ spine, 5 kernels: $(wc -c < "$MSL" | tr -d ' ') bytes, every byte authored by mla-msl.fk"

# ── 2. the .metallib, cached across RUNS by the source's own sha256 ────────────────────────────
mkdir -p "$CACHE"
msl_sha="$(shasum -a 256 "$MSL" | cut -c1-16)"
LIB="$CACHE/mla-$msl_sha.metallib"
if [[ -f "$LIB" ]]; then
    echo "PASS  metallib cache HIT: $(basename "$LIB") (emitted source unchanged; no compile this run)"
else
    xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
          -c "$MSL" -o "$work/mla.air" 2>"$work/metal.err" \
      && xcrun -sdk macosx metallib "$work/mla.air" -o "$LIB" 2>>"$work/metal.err" || {
        echo "FAIL  offline metal compile failed"; cat "$work/metal.err"; exit 1; }
    echo "PASS  metallib cache MISS -> compiled and cached: $(basename "$LIB")"
fi

# ── 3. the body: the toy fixture, its fp64 references, the pair frequencies, the config ─────────
"$GO_BIN" "${FILES[@]}" <(echo '(mld-emit-all)') > "$work/demo.txt" 2>"$work/demo.err" || {
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
        if s == "MLADEMO" || s == "END" { continue }
        guard let v = Double(s) else { continue }
        if mode == "S" { scalars[name] = v; mode = "" }
        else if mode == "V" { vecs[name]!.append(v) }
    }
}
func S(_ n: String) -> Int { Int(scalars[n]!) }
func F(_ n: String) -> Float { Float(scalars[n]!) }
let E = S("E"), R = S("R"), nh = S("nh"), hd = S("hd"), nrot = S("nrot")
let ng = S("ng"), grank = S("grank"), gdim = S("gdim"), ncat = S("ncat")
let eps = F("eps"), base = F("base"), scale = F("scale")
let npos = 3

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
func scratch(_ n: Int) -> MTLBuffer { dev.makeBuffer(length: n * 4, options: .storageModeShared)! }

// weights and inputs, resident
let bAn = vbuf("an"), bGqa = vbuf("gqa"), bGkv = vbuf("gkv"), bSinks = vbuf("sinks"), bFreqs = vbuf("freqs")
let bWqa = vbuf("wqa"), bWqb = vbuf("wqb"), bWkv = vbuf("wkv"), bWb = vbuf("wb"), bWas = vbuf("was")
let bX = [vbuf("x0"), vbuf("x1"), vbuf("x2")]

let pRms = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_mla_rmsnorm_f32")!)
let pMv  = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_mla_matvec_f32")!)
let pHr  = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_mla_headrms_f32")!)
let pRo  = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_mla_rope_f32")!)
let pAt  = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_mla_attend_f32")!)

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
func rmsnorm(_ x: MTLBuffer, _ xo: Int, _ g: MTLBuffer, _ out: MTLBuffer, _ oo: Int, _ n: Int) {
    var nn = UInt32(n); var e = eps
    dispatch(pRms, width: 1) { enc in
        enc.setBuffer(x, offset: xo * 4, index: 0); enc.setBuffer(g, offset: 0, index: 1)
        enc.setBuffer(out, offset: oo * 4, index: 2)
        enc.setBytes(&nn, length: 4, index: 3); enc.setBytes(&e, length: 4, index: 4)
    }
}
func matvec(_ W: MTLBuffer, _ wo: Int, _ x: MTLBuffer, _ xo: Int, _ y: MTLBuffer, _ yo: Int, _ rows: Int, _ cols: Int) {
    var r = UInt32(rows); var c = UInt32(cols)
    dispatch(pMv, width: rows) { enc in
        enc.setBuffer(W, offset: wo * 4, index: 0); enc.setBuffer(x, offset: xo * 4, index: 1)
        enc.setBuffer(y, offset: yo * 4, index: 2)
        enc.setBytes(&r, length: 4, index: 3); enc.setBytes(&c, length: 4, index: 4)
    }
}
func headrms(_ q: MTLBuffer, _ out: MTLBuffer) {
    var n = UInt32(nh); var h = UInt32(hd); var e = eps
    dispatch(pHr, width: nh) { enc in
        enc.setBuffer(q, offset: 0, index: 0); enc.setBuffer(out, offset: 0, index: 1)
        enc.setBytes(&n, length: 4, index: 2); enc.setBytes(&h, length: 4, index: 3); enc.setBytes(&e, length: 4, index: 4)
    }
}
func rope(_ v: MTLBuffer, _ out: MTLBuffer, _ nheads: Int, _ pos: Float, _ sign: Float) {
    var n = UInt32(nheads); var h = UInt32(hd); var nr = UInt32(nrot); var p = pos; var sg = sign
    dispatch(pRo, width: nheads) { enc in
        enc.setBuffer(v, offset: 0, index: 0); enc.setBuffer(out, offset: 0, index: 1); enc.setBuffer(bFreqs, offset: 0, index: 2)
        enc.setBytes(&n, length: 4, index: 3); enc.setBytes(&h, length: 4, index: 4); enc.setBytes(&nr, length: 4, index: 5)
        enc.setBytes(&p, length: 4, index: 6); enc.setBytes(&sg, length: 4, index: 7)
    }
}
func attend(_ q: MTLBuffer, _ rows: MTLBuffer, _ out: MTLBuffer, _ nrows: Int) {
    var n = UInt32(nh); var h = UInt32(hd); var nr = UInt32(nrows); var sc = scale
    dispatch(pAt, width: nh) { enc in
        enc.setBuffer(q, offset: 0, index: 0); enc.setBuffer(rows, offset: 0, index: 1)
        enc.setBuffer(out, offset: 0, index: 2); enc.setBuffer(bSinks, offset: 0, index: 3)
        enc.setBytes(&n, length: 4, index: 4); enc.setBytes(&h, length: 4, index: 5)
        enc.setBytes(&nr, length: 4, index: 6); enc.setBytes(&sc, length: 4, index: 7)
    }
}

// scratch
let bRow1 = scratch(hd)
let bN = scratch(E), bRaw = scratch(hd), bKv = scratch(hd)
let bQr = scratch(R), bQrn = scratch(R), bQb = scratch(nh*hd), bQh = scratch(nh*hd)
let bQ = scratch(nh*hd), bHeads = scratch(nh*hd), bBack = scratch(nh*hd)
let bLow = scratch(ncat), bOut = scratch(E)
let bRows = scratch(npos * hd)
let qMv = nh * hd

func readBuf(_ b: MTLBuffer, _ o: Int, _ n: Int) -> [Float] {
    let p = b.contents().bindMemory(to: Float.self, capacity: o + n); return (0..<n).map { p[o + $0] }
}
func cmp(_ got: [Float], _ ref: [Double]) -> (Double, Double) {
    var ma = 0.0, mr = 0.0
    for i in 0..<ref.count { let d = abs(Double(got[i]) - ref[i]); ma = max(ma, d); mr = max(mr, d / max(abs(ref[i]), 1e-9)) }
    return (ma, mr)
}
let TOL = 5e-4   // the f32 working-precision envelope; observed is far tighter and printed
var worstRel = 0.0

// cache row for input at position p -> writes into bRows at row p
func cacheRow(_ x: MTLBuffer, _ pos: Int) {
    rmsnorm(x, 0, bAn, bN, 0, E)
    matvec(bWkv, 0, bN, 0, bRaw, 0, hd, E)
    rmsnorm(bRaw, 0, bGkv, bKv, 0, hd)
    rope(bKv, bRow1, 1, Float(pos), 1.0)         // nh=1: the whole hd latent, tail = last nrot
    // copy bRow1 -> bRows[pos]
    let src = bRow1.contents().bindMemory(to: Float.self, capacity: hd)
    let dst = bRows.contents().bindMemory(to: Float.self, capacity: npos * hd)
    for d in 0..<hd { dst[pos*hd + d] = src[d] }
}

// block for input x at position pos, attending over the first nrows cache rows in bRows
func block(_ x: MTLBuffer, _ pos: Int, _ nrows: Int) {
    rmsnorm(x, 0, bAn, bN, 0, E)
    matvec(bWqa, 0, bN, 0, bQr, 0, R, E)
    rmsnorm(bQr, 0, bGqa, bQrn, 0, R)
    matvec(bWqb, 0, bQrn, 0, bQb, 0, qMv, R)
    headrms(bQb, bQh)
    rope(bQh, bQ, nh, Float(pos), 1.0)
    attend(bQ, bRows, bHeads, nrows)
    rope(bHeads, bBack, nh, Float(pos), -1.0)
    // grouped low-rank out: og = was[g] . back[g*gdim ..]  -> low[g*grank ..]
    for g in 0..<ng { matvec(bWas, g*grank*gdim, bBack, g*gdim, bLow, g*grank, grank, gdim) }
    matvec(bWb, 0, bLow, 0, bOut, 0, E, ncat)
}

// --- GATE 0: did the GPU run. Sentinel bN, run one real dispatch, demand it overwrote, no cb error. ---
do {
    let sent: Float = -424242.0
    let p = bN.contents().bindMemory(to: Float.self, capacity: E)
    for i in 0..<E { p[i] = sent }
    let before = gpuErrors
    rmsnorm(bX[2], 0, bAn, bN, 0, E)
    var survived = 0; for i in 0..<E where p[i] == sent { survived += 1 }
    if gpuErrors > before { print("  command buffer ERROR: \(gpuFirstError ?? "unknown")") }
    check(gpuErrors == before && survived == 0,
      "gate 0 the GPU executes: a real RMSNorm dispatch overwrote all \(E) sentinels, no command buffer errored",
      "gate 0 THE GPU DID NOT RUN — \(survived)/\(E) sentinels survived, \(gpuErrors - before) cb error(s)")
    if failures > 0 { print("VERDICT FAIL  the GPU did not run; no MLA arithmetic was witnessed"); exit(1) }
}

// build the three cache rows first (the block attends over them)
for p in 0..<npos { cacheRow(bX[p], p) }

// --- gate 6: the three cache rows (unispan) ---
do {
    var wr = 0.0
    for (p, nm) in [(0,"r0"),(1,"r1"),(2,"r2")] {
        let (_, mr) = cmp(readBuf(bRows, p*hd, hd), vecs[nm]!); wr = max(wr, mr)
    }
    worstRel = max(worstRel, wr)
    check(wr < TOL, String(format: "gate 6 cache rows r0,r1,r2 (all 3 positions) reproduced within %.2e of fp64 (worst rel %.3e)", TOL, wr),
                    String(format: "gate 6 cache rows: worst relative deviation %.3e exceeds %.2e", wr, TOL))
}

// --- run the block at pos 2 (x2) and check every STAGE against the recipe (snugcause: the distant case) ---
block(bX[2], 2, npos)
func stage(_ gate: Int, _ label: String, _ b: MTLBuffer, _ n: Int, _ ref: String) {
    let (ma, mr) = cmp(readBuf(b, 0, n), vecs[ref]!); worstRel = max(worstRel, mr)
    check(mr < TOL, String(format: "gate %d %@ == recipe's %@ (rel %.3e, abs %.3e)", gate, label, ref, mr, ma),
                    String(format: "gate %d %@: rel %.3e exceeds %.2e", gate, label, mr, TOL))
}
stage(1, "RMSNorm(x2)", bN, E, "n2")
stage(2, "Q projection (matvec/rmsnorm/matvec/head-rms)", bQh, nh*hd, "qh2")
stage(3, "forward RoPE (pos 2, sign +1)", bQ, nh*hd, "q2")
stage(4, "sink attention over the latent rows", bHeads, nh*hd, "heads2")
stage(5, "inverse RoPE (pos 2, sign -1)", bBack, nh*hd, "back2")

// --- gate 7: the three block outputs end to end (unispan; o0 at pos 0 = rope identity, o2 at pos 2 not) ---
do {
    var wr = 0.0
    for (p, nm, nr) in [(0,"o0",1),(1,"o1",2),(2,"o2",3)] {
        block(bX[p], p, nr)
        let (_, mr) = cmp(readBuf(bOut, 0, E), vecs[nm]!); wr = max(wr, mr)
    }
    worstRel = max(worstRel, wr)
    check(wr < TOL, String(format: "gate 7 block outputs o0,o1,o2 (all 3 positions) reproduced within %.2e of fp64 (worst rel %.3e)", TOL, wr),
                    String(format: "gate 7 block outputs: worst relative deviation %.3e exceeds %.2e", wr, TOL))
}

// --- gate 8: residency — re-dispatch the block ITERS times, checksum unchanged, no re-upload ---
do {
    block(bX[2], 2, npos)
    var first = 0.0; let o = readBuf(bOut, 0, E); for v in o { first += Double(v) }
    let before = dispatches
    for _ in 0..<iters { block(bX[2], 2, npos) }
    var after = 0.0; let o2 = readBuf(bOut, 0, E); for v in o2 { after += Double(v) }
    check(after == first,
      "gate 8 residency: \(iters) block re-dispatches (\(dispatches - before) GPU dispatches), output checksum unchanged, weights never re-uploaded",
      "gate 8: the output checksum changed across re-dispatches")
}

print(String(format: "--- worst relative deviation across every stage and output: %.3e (gate %.2e). f32 vs fp64 over an identical operation graph; the residual is working precision, not the decomposition.", worstRel, TOL))
if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER(S) FAILED — first: \(gpuFirstError ?? "unknown") ==="); }
let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  10 gates, MLA on the GPU") } else { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc failed"; tail -30 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB" "$work/demo.txt" "$ITERS"
exit $?
