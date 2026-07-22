#!/usr/bin/env bash
# metal_dsv4_decode.sh — STONE 41: THE DECODE LOOP AND ITS KV CACHE, ON THE GPU.
#
# THE CLAIM, and nothing wider: a real decode loop — prompt ids -> prefill over every prompt position ->
# argmax -> feed the token back -> repeat -> stop — runs with its KV cache as a GROWING METAL ARENA that
# the GPU itself appends to, and it produces the same token ids and the same cache rows as this body's
# fp64 recipe (form-stdlib/dsv4-decode-loop.fk). The arena is written by form_dkv_append_f32, which the
# BODY emits (form-stdlib/dsv4-kv-cache.fk), and read by form_mla_attend_f32, which the body already had
# (form-stdlib/mla-msl.fk) — with nrows bound to pos+1 instead of the 1 that metal_dsv4_layer_join.sh
# binds. That single re-binding IS the decode loop's seam, and nothing in the attention kernel changes.
#
# WHAT IS RUNNING, said plainly (aporon / selfgauge): a ONE-LAYER MLA model at E=4, R=3, nh=2, hd=4,
# nrot=2, ng=2, n_hc=1, over a six-token vocabulary, with a PLAIN RESIDUAL. That is not
# DeepSeek-V4-Flash — V4-Flash is 43 heterogeneous layers, hd 512, a MoE FFN, and no plain residual
# anywhere (its residual stream is the hyper-connection frame). It IS a model that runs today, on the
# four-way-proven MLA block, and it is enough to make every claim a decode loop can be wrong about:
# growth, immutability, position, determinism, sensitivity, and the end condition.
#
# THE SEAM TO STONE 39'S STACK, named exactly so wiring is a SUBSTITUTION and not a redesign:
#   (dsv4-stack-step ctx hc kv pos) -> (list hc' kv')
#     hc  = 16384 numbers = 4 hyper-connection streams x 4096 — EXACTLY what metal_dsv4_layer_join.sh's
#           gate 29 emits ("the four hyper-connection streams blk.1 receives")
#     kv  = a dsv4-kv-cache with 43 banks and hd 512
#     pos = the token's absolute position, the RoPE index and the arena row index
#   ddl-run takes that function unchanged; it never looks inside ctx.
#
# hushfold (corpus row 859) is the reason this stone had to exist: RoPE is the identity at position 0,
# so every one-position witness before this one saw none of it. Gate 5 runs the SAME token at position 0
# and at position 1 and demands the cached rows DIFFER, with the position-0 row equal to the unrotated
# latent. Positions >= 1 are what a decode loop is made of.
#
# zerobirth / edgedrop: the arena is filled with NaN before anything touches it, not zero — a Metal
# buffer is BORN zeroed, so "computed zero" and "never computed" are the same bytes. Every dispatch is
# checked for cb.error and cb.status, and gate 2 demands the rows past the write frontier are STILL
# sentinel, which is the only way to tell a cache that grew by one from a cache that grew by two.
#
# twinblind (corpus row 868): the arena's growth and immutability are CANONICAL — one right answer — so
# the self-check here is a real falsifier. The loop's CHOICES (pos starts at prompt->len, the stop token
# is never emitted, the last accepted token is not fed forward, argmax ties to the lowest index, prefill
# takes its logits from the last position) are rented from ds4.c and cited line by line in
# form-stdlib/dsv4-decode-loop.fk's header. halfrent (row 870) unchanged: ds4.c cannot execute the real
# file, so its ORDER is rented and its arithmetic is not.
#
# Who decides what (the dumb-carrier discipline):
#   the BODY  form-stdlib/mla-msl.fk + form-stdlib/dsv4-kv-cache.fk — every character of Metal source.
#   the BODY  form-stdlib/dsv4-decode-loop.fk (ddc-emit) — the fixture and every fp64 answer judged by.
#   the CARRIER (this file + the Swift runner it writes) — compile, bind, dispatch, compare, count.
#
# THE GATES:
#   0  DID THE GPU RUN         a sentinelled buffer is overwritten by a real dispatch, no cb error
#   1  HISTORY IS IMMUTABLE    every append is preceded by a snapshot of the arena's live rows and
#                              followed by a BIT-EXACT comparison. A cache that rewrites history is the
#                              classic silent decode bug and it is checked on the device, every step.
#   2  ONE ROW PER STEP        after t appends, rows 0..t-1 are written and row t is STILL NaN-sentinel
#   3  THE CAP REFUSES         an append at pos = cap writes nothing; the arena stays all sentinel
#   4  N STEPS, N TOKENS       npredict steps produce exactly npredict ids, and the ids are EXACTLY the
#                              fp64 recipe's — integer equality, no tolerance anywhere
#   5  hushfold, POSITIONS >=1 the same token at pos 0 and pos 1 gives DIFFERENT rows; the pos-0 row is
#                              the unrotated latent. RoPE finally exercised for its actual purpose.
#   6  DETERMINISM             the same prompt twice: identical ids AND a bit-identical arena
#   7  SENSITIVITY             a different prompt gives different ids, and exactly the recipe's
#   8  AGREEMENT               every cache row and the prefill logits match fp64 within the f32 envelope,
#                              and the row count / final position / stop reason match exactly
#   9  ONE HEADER, ONE SPINE   one metal_stdlib, zero using-namespace, one spine, 6 kernels
#
# Run:  form/native/metal/metal_dsv4_decode.sh
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
CACHE="$ROOT/native/metal/.metallib-cache"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the GPU witness needs an Apple GPU + swiftc"
    exit 2
fi
if [[ ! -x "$GO_BIN" ]]; then
    echo "  building go kernel..." >&2
    (cd "$ROOT/form-kernel-go" && go build -o bin-go .) || { echo "FAIL go build"; exit 1; }
fi

work="$(mktemp -d "${TMPDIR:-/tmp}/fkdec.XXXXXX")"
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
while read -r x; do FILES+=("$x"); done < <(fk_expand form-stdlib/dsv4-decode-loop.fk)

# ── 1. the body emits the Metal source: mla-msl.fk's five kernels + the cache's own append ─────
echo '(print (ddc-msl-unit))' > "$work/emit.fk"
"$GO_BIN" "${FILES[@]}" "$work/emit.fk" 2>"$work/emit.err" | sed '/^null$/d' > "$work/dec.metal" || {
    echo "FAIL  MSL emission failed"; cat "$work/emit.err"; exit 1; }
MSL="$work/dec.metal"
for k in form_mla_rmsnorm_f32 form_mla_matvec_f32 form_mla_headrms_f32 form_mla_rope_f32 \
         form_mla_attend_f32 form_dkv_append_f32; do
    grep -q "kernel void $k" "$MSL" || { echo "FAIL  kernel $k was not emitted"; exit 1; }
done
nhdr=$(grep -c 'metal_stdlib' "$MSL"); nusing=$(grep -c 'using namespace' "$MSL")
nspine=$(grep -o 'float mla_sqrt' "$MSL" | wc -l | tr -d ' ')
nkern=$(grep -o 'kernel void form_' "$MSL" | wc -l | tr -d ' ')
[[ "$nhdr" == 1 && "$nusing" == 0 && "$nspine" == 1 && "$nkern" == 6 ]] || {
    echo "FAIL  gate 9: header $nhdr (want 1), using-namespace $nusing (want 0), spine $nspine (want 1), kernels $nkern (want 6)"; exit 1; }
echo "PASS  gate 9 one metal_stdlib, no using-namespace, one mla_ spine, 6 kernels: $(wc -c < "$MSL" | tr -d ' ') bytes, every byte authored by the body"

# ── 2. the .metallib, cached across RUNS by the source's own sha256 ────────────────────────────
mkdir -p "$CACHE"
msl_sha="$(shasum -a 256 "$MSL" | cut -c1-16)"
LIB="$CACHE/dsv4dec-$msl_sha.metallib"
if [[ -f "$LIB" ]]; then
    echo "PASS  metallib cache HIT: $(basename "$LIB") (emitted source unchanged; no compile this run)"
else
    xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
          -c "$MSL" -o "$work/dec.air" 2>"$work/metal.err" \
      && xcrun -sdk macosx metallib "$work/dec.air" -o "$LIB" 2>>"$work/metal.err" || {
        echo "FAIL  offline metal compile failed"; cat "$work/metal.err"; exit 1; }
    echo "PASS  metallib cache MISS -> compiled and cached: $(basename "$LIB")"
fi

# ── 3. the body: the fixture, the two prompts, and every fp64 answer this run is judged by ─────
"$GO_BIN" "${FILES[@]}" <(echo '(ddc-emit)') > "$work/fix.txt" 2>"$work/fix.err" || {
    echo "FAIL  fixture emission failed"; tail -5 "$work/fix.err"; exit 1; }
grep -qx 'END' "$work/fix.txt" || { echo "FAIL  fixture stream truncated"; exit 1; }

# ── 4. the carrier ─────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

let a = CommandLine.arguments
let libPath = a[1], fixPath = a[2]

var scalars: [String: Double] = [:]
var vecs: [String: [Double]] = [:]
do {
    var mode = "", name = ""
    for raw in try String(contentsOfFile: fixPath, encoding: .utf8).split(separator: "\n", omittingEmptySubsequences: false) {
        let s = String(raw)
        if s.hasPrefix("SCALAR ") { mode = "S"; name = String(s.dropFirst(7)); continue }
        if s.hasPrefix("VEC ")    { mode = "V"; name = String(s.dropFirst(4)); vecs[name] = []; continue }
        if s == "ENDVEC" { mode = ""; continue }
        if s == "DSV4DECODE" || s == "END" { continue }
        guard let v = Double(s) else { continue }
        if mode == "S" { scalars[name] = v; mode = "" }
        else if mode == "V" { vecs[name]!.append(v) }
    }
}
func S(_ n: String) -> Int { Int(scalars[n]!) }
func F(_ n: String) -> Float { Float(scalars[n]!) }
func I(_ n: String) -> [Int] { vecs[n]!.map { Int($0) } }
let E = S("E"), R = S("R"), nh = S("nh"), hd = S("hd"), nrot = S("nrot")
let ng = S("ng"), grank = S("grank"), gdim = S("gdim"), ncat = S("ncat")
let cap = S("cap"), npredict = S("npredict"), eos = S("eos"), vocab = S("vocab")
let eps = F("eps"), scale = F("scale")

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
func fp(_ b: MTLBuffer, _ n: Int) -> UnsafeMutablePointer<Float> { b.contents().bindMemory(to: Float.self, capacity: n) }
func readBuf(_ b: MTLBuffer, _ o: Int, _ n: Int) -> [Float] { let p = fp(b, o + n); return (0..<n).map { p[o + $0] } }

let bAn = vbuf("an"), bGqa = vbuf("gqa"), bGkv = vbuf("gkv"), bSinks = vbuf("sinks"), bFreqs = vbuf("freqs")
let bWqa = vbuf("wqa"), bWqb = vbuf("wqb"), bWkv = vbuf("wkv"), bWb = vbuf("wb"), bWas = vbuf("was")
let bWout = vbuf("wout"), bWnorm = vbuf("wnorm"), bEmb = vbuf("emb")

let pRms = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_mla_rmsnorm_f32")!)
let pMv  = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_mla_matvec_f32")!)
let pHr  = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_mla_headrms_f32")!)
let pRo  = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_mla_rope_f32")!)
let pAt  = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_mla_attend_f32")!)
let pAp  = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_dkv_append_f32")!)

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
func rmsnorm(_ x: MTLBuffer, _ xo: Int, _ g: MTLBuffer, _ out: MTLBuffer, _ n: Int) {
    var nn = UInt32(n); var e = eps
    dispatch(pRms, width: 1) { enc in
        enc.setBuffer(x, offset: xo * 4, index: 0); enc.setBuffer(g, offset: 0, index: 1)
        enc.setBuffer(out, offset: 0, index: 2)
        enc.setBytes(&nn, length: 4, index: 3); enc.setBytes(&e, length: 4, index: 4) }
}
func matvec(_ W: MTLBuffer, _ wo: Int, _ x: MTLBuffer, _ xo: Int, _ y: MTLBuffer, _ yo: Int, _ rows: Int, _ cols: Int) {
    var r = UInt32(rows); var c = UInt32(cols)
    dispatch(pMv, width: rows) { enc in
        enc.setBuffer(W, offset: wo * 4, index: 0); enc.setBuffer(x, offset: xo * 4, index: 1)
        enc.setBuffer(y, offset: yo * 4, index: 2)
        enc.setBytes(&r, length: 4, index: 3); enc.setBytes(&c, length: 4, index: 4) }
}
func headrms(_ q: MTLBuffer, _ out: MTLBuffer) {
    var n = UInt32(nh); var h = UInt32(hd); var e = eps
    dispatch(pHr, width: nh) { enc in
        enc.setBuffer(q, offset: 0, index: 0); enc.setBuffer(out, offset: 0, index: 1)
        enc.setBytes(&n, length: 4, index: 2); enc.setBytes(&h, length: 4, index: 3); enc.setBytes(&e, length: 4, index: 4) }
}
func rope(_ v: MTLBuffer, _ out: MTLBuffer, _ nheads: Int, _ pos: Float, _ sign: Float) {
    var n = UInt32(nheads); var h = UInt32(hd); var nr = UInt32(nrot); var p = pos; var sg = sign
    dispatch(pRo, width: nheads) { enc in
        enc.setBuffer(v, offset: 0, index: 0); enc.setBuffer(out, offset: 0, index: 1); enc.setBuffer(bFreqs, offset: 0, index: 2)
        enc.setBytes(&n, length: 4, index: 3); enc.setBytes(&h, length: 4, index: 4); enc.setBytes(&nr, length: 4, index: 5)
        enc.setBytes(&p, length: 4, index: 6); enc.setBytes(&sg, length: 4, index: 7) }
}
func attend(_ q: MTLBuffer, _ rows: MTLBuffer, _ out: MTLBuffer, _ nrows: Int) {
    var n = UInt32(nh); var h = UInt32(hd); var nr = UInt32(nrows); var sc = scale
    dispatch(pAt, width: nh) { enc in
        enc.setBuffer(q, offset: 0, index: 0); enc.setBuffer(rows, offset: 0, index: 1)
        enc.setBuffer(out, offset: 0, index: 2); enc.setBuffer(bSinks, offset: 0, index: 3)
        enc.setBytes(&n, length: 4, index: 4); enc.setBytes(&h, length: 4, index: 5)
        enc.setBytes(&nr, length: 4, index: 6); enc.setBytes(&sc, length: 4, index: 7) }
}
// THE APPEND — the body's own kernel, the whole of what makes a second token cheap.
func appendRow(_ row: MTLBuffer, _ arena: MTLBuffer, _ pos: Int) {
    var h = UInt32(hd); var p = UInt32(pos); var c = UInt32(cap)
    dispatch(pAp, width: hd) { enc in
        enc.setBuffer(row, offset: 0, index: 0); enc.setBuffer(arena, offset: 0, index: 1)
        enc.setBytes(&h, length: 4, index: 2); enc.setBytes(&p, length: 4, index: 3); enc.setBytes(&c, length: 4, index: 4) }
}

// scratch, allocated once and reused for every step — a decode loop that reallocates per token
// is a decode loop that measures its allocator (thawtax, probetoll)
let bX = scratch(E), bN = scratch(E), bRaw = scratch(hd), bKv = scratch(hd), bRow = scratch(hd)
let bQr = scratch(R), bQrn = scratch(R), bQb = scratch(nh*hd), bQh = scratch(nh*hd), bQ = scratch(nh*hd)
let bHeads = scratch(nh*hd), bBack = scratch(nh*hd), bLow = scratch(ncat), bOut = scratch(E)
let bHc = scratch(E), bHn = scratch(E), bLog = scratch(vocab)
let SENT: Float = Float.nan

func freshArena() -> MTLBuffer {
    let b = scratch(cap * hd); let p = fp(b, cap*hd)
    for i in 0..<(cap*hd) { p[i] = SENT }
    return b
}
var historyBreaks = 0          // gate 1: appends that disturbed an earlier row
var frontierBreaks = 0         // gate 2: a step that wrote more or fewer than one row
var appendCount = 0            // how many device appends were witnessed

// ONE DECODE STEP: embed -> cache row -> APPEND -> attend over the whole arena -> out -> logits.
// ds4.c:13606's shape at one layer. Returns the logits; the arena is one row longer.
func stepGPU(_ arena: MTLBuffer, _ id: Int, _ pos: Int) -> [Float] {
    let ep = fp(bEmb, vocab*E), xp = fp(bX, E)
    for d in 0..<E { xp[d] = ep[id*E + d] }
    rmsnorm(bX, 0, bAn, bN, E)
    // --- the cache row: down-project, norm, rotate the tail at THIS position ---
    matvec(bWkv, 0, bN, 0, bRaw, 0, hd, E)
    rmsnorm(bRaw, 0, bGkv, bKv, hd)
    rope(bKv, bRow, 1, Float(pos), 1.0)
    // --- the append, with the history snapshot around it (gate 1) and the frontier check (gate 2) ---
    let before = readBuf(arena, 0, cap*hd)
    appendRow(bRow, arena, pos)
    appendCount += 1
    let after = readBuf(arena, 0, cap*hd)
    for i in 0..<(pos*hd) where before[i].bitPattern != after[i].bitPattern { historyBreaks += 1 }
    var wrote = 0
    for r in 0..<cap { var live = false; for j in 0..<hd where !after[r*hd + j].isNaN { live = true }
                       if live && before[r*hd].isNaN { wrote += 1 } }
    if wrote != 1 { frontierBreaks += 1 }
    if pos + 1 < cap { for j in 0..<hd where !after[(pos+1)*hd + j].isNaN { frontierBreaks += 1 } }
    // --- the query, and attention over EVERY row so far: nrows = pos+1, the whole seam ---
    matvec(bWqa, 0, bN, 0, bQr, 0, R, E)
    rmsnorm(bQr, 0, bGqa, bQrn, R)
    matvec(bWqb, 0, bQrn, 0, bQb, 0, nh*hd, R)
    headrms(bQb, bQh)
    rope(bQh, bQ, nh, Float(pos), 1.0)
    attend(bQ, arena, bHeads, pos + 1)
    rope(bHeads, bBack, nh, Float(pos), -1.0)
    for g in 0..<ng { matvec(bWas, g*grank*gdim, bBack, g*gdim, bLow, g*grank, grank, gdim) }
    matvec(bWb, 0, bLow, 0, bOut, 0, E, ncat)
    // the stub's PLAIN residual (V4-Flash's is the hyper-connection frame — Stone 39's)
    let op = fp(bOut, E), hp = fp(bHc, E)
    for d in 0..<E { hp[d] = xp[d] + op[d] }
    rmsnorm(bHc, 0, bWnorm, bHn, E)
    matvec(bWout, 0, bHn, 0, bLog, 0, vocab, E)
    return readBuf(bLog, 0, vocab)
}
// ds4.c:36578 — strict >, so a tie goes to the LOWEST index
func argmax(_ xs: [Float]) -> Int { var b = 0; var bv = -Float.greatestFiniteMagnitude
    for i in 0..<xs.count where xs[i] > bv { b = i; bv = xs[i] }; return b }

struct Run { var ids: [Int]; var pos: Int; var stop: Int; var arena: MTLBuffer; var prefillLogits: [Float] }
// THE LOOP — ds4.c:37102..37124, transcribed. The comments name the lines it is transcribed from.
func run(_ prompt: [Int]) -> Run {
    let arena = freshArena()
    var logits: [Float] = []
    for t in 0..<prompt.count { logits = stepGPU(arena, prompt[t], t) }   // prefill, positions 0..n-1
    let prefill = logits
    var pos = prompt.count                                                // ds4.c:46530
    var ids: [Int] = []; var stop = 1
    var i = 0
    while i < npredict && pos < cap {
        let tok = argmax(logits)
        if tok == eos { stop = 0; break }                                 // ds4.c:37111 — NOT emitted
        ids.append(tok)
        if i == npredict - 1 || pos + 1 >= cap {                          // ds4.c:37116 — not fed forward
            stop = (i == npredict - 1) ? 1 : 2; pos += 1; break
        }
        logits = stepGPU(arena, tok, pos)
        pos += 1; i += 1
    }
    if i >= npredict { stop = 1 }
    if pos >= cap && ids.count < npredict { stop = 2 }
    return Run(ids: ids, pos: pos, stop: stop, arena: arena, prefillLogits: prefill)
}

// --- GATE 0: did the GPU run at all ---
do {
    let sent: Float = -424242.0
    let p = fp(bN, E); for i in 0..<E { p[i] = sent }
    let before = gpuErrors
    let ep = fp(bEmb, vocab*E), xp = fp(bX, E); for d in 0..<E { xp[d] = ep[d] }
    rmsnorm(bX, 0, bAn, bN, E)
    var survived = 0; for i in 0..<E where p[i] == sent { survived += 1 }
    if gpuErrors > before { print("  command buffer ERROR: \(gpuFirstError ?? "unknown")") }
    check(gpuErrors == before && survived == 0,
      "gate 0 the GPU executes: a real RMSNorm dispatch overwrote all \(E) sentinels, no command buffer errored",
      "gate 0 THE GPU DID NOT RUN — \(survived)/\(E) sentinels survived, \(gpuErrors - before) cb error(s)")
    if failures > 0 { print("VERDICT FAIL  the GPU did not run; no decode was witnessed"); exit(1) }
}

// --- GATE 3: the cap refuses. An append at pos = cap must write NOTHING. ---
do {
    let arena = freshArena()
    let rp = fp(bRow, hd); for j in 0..<hd { rp[j] = 7.0 }
    appendRow(bRow, arena, cap)
    let after = readBuf(arena, 0, cap*hd)
    var written = 0; for v in after where !v.isNaN { written += 1 }
    check(written == 0,
      "gate 3 the cap REFUSES: an append at pos = cap (\(cap)) wrote 0 of \(cap*hd) arena floats — an overrun is a refusal the sentinel catches, never a silent stomp into row 0",
      "gate 3 THE CAP DID NOT REFUSE — \(written) arena floats were written past the cap")
}

// --- the two runs, and a repeat of the first ---
let promptA = I("promptA"), promptB = I("promptB")
let runA = run(promptA)
let runA2 = run(promptA)
let runB = run(promptB)

// --- GATE 1 / GATE 2: immutability and one-row-per-step, checked at every append above ---
check(historyBreaks == 0,
  "gate 1 HISTORY IS IMMUTABLE: across \(appendCount) device appends, not one earlier arena row changed by a single bit",
  "gate 1 THE CACHE REWROTE HISTORY — \(historyBreaks) earlier row element(s) changed under an append")
check(frontierBreaks == 0,
  "gate 2 ONE ROW PER STEP: every append wrote exactly one previously-sentinel row and left the next row NaN-sentinel — a zeroed buffer would have read as written (zerobirth)",
  "gate 2 THE ARENA DID NOT GROW BY ONE — \(frontierBreaks) frontier violation(s)")

// --- GATE 4: N steps, N tokens, and EXACTLY the recipe's ids (integer equality) ---
let genA = I("genA"), genB = I("genB")
check(runA.ids.count == npredict && runA.ids == genA,
  "gate 4 N STEPS PRODUCE N TOKENS: \(npredict) steps gave \(runA.ids.count) ids \(runA.ids), EXACTLY the fp64 recipe's \(genA) — integer equality, no tolerance",
  "gate 4 ids \(runA.ids) (\(runA.ids.count)) do not equal the recipe's \(genA)")

// --- GATE 5: hushfold — positions >= 1 are what a decode loop is made of ---
do {
    let arena = freshArena()
    _ = stepGPU(arena, 0, 0)
    let g0 = readBuf(arena, 0, hd)
    let arena1 = freshArena()
    _ = stepGPU(arena1, 0, 1)
    let g1 = readBuf(arena1, hd, hd)
    var moved = 0.0; for j in 0..<hd { moved = max(moved, abs(Double(g0[j]) - Double(g1[j]))) }
    let r0 = vecs["row0"]!, r1 = vecs["row1"]!
    var d0 = 0.0, d1 = 0.0
    for j in 0..<hd { d0 = max(d0, abs(Double(g0[j]) - r0[j])); d1 = max(d1, abs(Double(g1[j]) - r1[j])) }
    check(moved > 1e-3 && d0 < 5e-4 && d1 < 5e-4,
      String(format: "gate 5 hushfold: the SAME token cached at position 0 and position 1 differs by %.3e, and each row matches its own fp64 reference (%.2e / %.2e). RoPE is the identity at position 0 — a one-position witness sees none of this", moved, d0, d1),
      String(format: "gate 5 hushfold: rows moved %.3e (want > 1e-3), ref deviation %.3e / %.3e", moved, d0, d1))
}

// --- GATE 6: determinism — identical ids AND a bit-identical arena ---
do {
    let a1 = readBuf(runA.arena, 0, cap*hd), a2 = readBuf(runA2.arena, 0, cap*hd)
    var diff = 0
    for i in 0..<(cap*hd) where a1[i].bitPattern != a2[i].bitPattern { diff += 1 }
    check(runA.ids == runA2.ids && diff == 0 && runA.pos == runA2.pos,
      "gate 6 DETERMINISM: the same prompt run twice gave the same \(runA.ids.count) ids and a BIT-IDENTICAL \(cap)x\(hd) arena",
      "gate 6 the same prompt diverged: ids \(runA.ids) vs \(runA2.ids), \(diff) arena float(s) differ")
}

// --- GATE 7: sensitivity — a different prompt gives different ids, and the recipe's ---
check(runB.ids == genB && runB.ids != runA.ids,
  "gate 7 SENSITIVITY: prompt \(promptB) gave \(runB.ids), the recipe's exactly, and different from prompt \(promptA)'s \(runA.ids) — without this, determinism is also satisfied by a loop that ignores its input",
  "gate 7 prompt \(promptB) gave \(runB.ids); recipe says \(genB); prompt A gave \(runA.ids)")

// --- GATE 8: agreement with the fp64 recipe, row for row ---
let TOL = 5e-4
do {
    let nrowsA = S("nrowsA"), posA = S("posA"), stopA = S("stopA")
    let refRows = vecs["rowsA"]!, refLog = vecs["logitsA0"]!
    let got = readBuf(runA.arena, 0, nrowsA*hd)
    var mr = 0.0
    for i in 0..<refRows.count { mr = max(mr, abs(Double(got[i]) - refRows[i]) / max(abs(refRows[i]), 1e-9)) }
    var ml = 0.0
    for i in 0..<refLog.count { ml = max(ml, abs(Double(runA.prefillLogits[i]) - refLog[i]) / max(abs(refLog[i]), 1e-9)) }
    var live = 0; for r in 0..<cap { if !got.isEmpty && r < nrowsA && !readBuf(runA.arena, r*hd, 1)[0].isNaN { live += 1 } }
    var beyond = 0
    for r in nrowsA..<cap { for j in 0..<hd where !readBuf(runA.arena, r*hd + j, 1)[0].isNaN { beyond += 1 } }
    check(mr < TOL && ml < TOL && live == nrowsA && beyond == 0 && runA.pos == posA && runA.stop == stopA,
      String(format: "gate 8 AGREEMENT: all %d cache rows within %.3e and the prefill logits within %.3e of fp64; the arena holds EXACTLY %d rows (prompt %d + generated %d - 1, because ds4.c:37116 never feeds the last accepted token forward), final position %d, stop reason %d",
             nrowsA, mr, ml, nrowsA, promptA.count, runA.ids.count, posA, stopA),
      String(format: "gate 8 rows rel %.3e, logits rel %.3e (tol %.2e), live rows %d want %d, beyond-frontier writes %d, pos %d want %d, stop %d want %d",
             mr, ml, TOL, live, nrowsA, beyond, runA.pos, posA, runA.stop, stopA))
}

print("--- \(dispatches) GPU dispatches across 3 decode runs; the arena is \(cap)x\(hd) floats and form_mla_attend_f32 read it with nrows = pos+1 at every step. metal_dsv4_layer_join.sh binds that same argument to 1.")
if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER(S) FAILED — first: \(gpuFirstError ?? "unknown") ===") }
let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  10 gates, the decode loop and its KV cache on the GPU") }
else   { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc failed"; tail -30 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB" "$work/fix.txt"
exit $?
