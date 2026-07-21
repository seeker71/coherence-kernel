#!/usr/bin/env bash
# metal_batched_prefill.sh — STONE 7. The prompt stops being P forward passes and becomes ONE, and the
# end-to-end rate is re-measured with the prefill inside it where it belongs.
#
# WHAT WAS MISSING BEFORE THIS. Stones 4 and 5 made DECODE fast — 2.591 -> 12.225 tok/s decode-only —
# and never touched the prompt. metal_first_token.sh's prefill loop is literally
#     for id in ids { cur = forward(id, pos); pos += 1 }
# so a P-token prompt runs the full 28-layer forward P times: all 2.0 GB of weights loaded P times,
# each weight decoded and used ONCE. Measured on the lane path immediately before this stone: prefill
# 0.461 s for 6 tokens = 76.8 ms/token, against decode's 81.9 ms/token. The same cost per token — which
# is exactly what token-at-a-time looks like from outside, and why prefill sat 49.3x behind the
# external oracle while decode sat 12.9x behind it.
#
# WHAT THIS DOES. qk-matmul-batch.fk emits ONE new kernel pair: one SIMD group per (row, tile of 8
# tokens), the weight decoded once and spent on all eight. Everything else in the forward pass is the
# EXISTING kernel, unchanged, dispatched differently:
#   * RMSNorm / RoPE / attention are independent across tokens, so the DECODE kernels are dispatched
#     once per token into one CONCURRENT encoder with no barrier between them. Same binary, so
#     bit-exact by construction rather than by argument, and the P dispatches overlap.
#   * SwiGLU and the residual add are elementwise, so the existing kernels run ONCE over P*n
#     contiguous elements. No new kernel, no change at all.
#   * The k/v projections write STRAIGHT INTO the pooled KV cache: the batched output layout is
#     y[t*rows + r] and the cache's per-position stride IS nkv*head_dim which IS `rows` for those two
#     tensors. Zero copies, and stated in qk-matmul-batch.fk's radius because the carrier leans on it.
#   * output_norm and the 128256-row unembedding run for the LAST prompt token only. Under
#     token-at-a-time they ran for every prompt token and 5.85 ms of every one of them was thrown away.
#
# THE GATES (all must pass before any rate is believed):
#   B1  THE KERNELS ARE THE BODY'S      both batch kernels present in the emitted unit, and the unit
#                                       still carries no `using namespace metal;`.
#   B2  NO EPSILON, AND IT IS MEASURED  the batched matmul is BIT-EXACT against P sequential lane
#                                       matvecs — zero differing floats out of P*rows — on real
#                                       llama3.2:3b tensors of both quants at four batch sizes.
#                                       Batching reassociates NOTHING; this is the claim that lets
#                                       Stone 5's derived bound stand unchanged.
#   B3  THE SAME TOKENS                 the batched-prefill path generates the SAME ids as the
#                                       token-at-a-time lane path, same prompt, same steps.
#   B4  A SLOPE, NOT A POINT            prefill measured at FOUR prompt lengths with a slope
#                                       (corpus row 827, unispan) — batching's benefit is
#                                       length-dependent, so one P would be a line pretending to be
#                                       a point's worth of evidence.
#   B5  THE WIDTH IS READ, NOT ASSUMED  threadExecutionWidth must be 32 or the harness SKIPs.
#
# Run:  form/native/metal/metal_batched_prefill.sh [nsteps] ["prompt"]        (defaults 12)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every other Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
NSTEPS="${1:-12}"
PROMPT="${2:-The capital of France is}"
BLOB="${FORM_GGUF_BLOB:-$HOME/.ollama/models/blobs/sha256-dde5aa3fc5ffc17176b5e8bdc82f587b24b2678c6c66101bf7da77af9f7ccdff}"
REFID="${FORM_REF_ID:-791}"
REFROW="${FORM_REF_ROW:-0}"
MAXPOS="${FORM_MAXPOS:-640}"
# the prefill CHUNK, and it is a MEASURED default, not a guess. Swept end to end at four prompt
# lengths (the B4 table below), best of each run, prefill tok/s at P = 32 / 128 / 512:
#   PCHUNK= 32    82.04   67.96   75.98
#   PCHUNK= 64    98.99   75.83   69.61
#   PCHUNK=128   102.45  114.76   98.85
# 128 wins at every length that can fill it and ties at the ones that cannot. Larger chunks were not
# taken further because the attention term (one thread per head, O(position) per token) is what bends
# the curve past here, not the chunk — named as an open gap in the receipt, not guessed at.
PCHUNK="${FORM_PCHUNK:-128}"
CACHE="$ROOT/native/metal/.first-token-cache"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the GPU witness needs an Apple GPU + swiftc"
    exit 2
fi
if [[ ! -f "$BLOB" ]]; then
    echo "SKIP  the llama3.2:3b GGUF blob is not on this host: $BLOB"
    exit 2
fi
if [[ ! -x "$GO_BIN" ]]; then
    echo "  building go kernel..." >&2
    (cd "$ROOT/form-kernel-go" && go build -o bin-go .)
fi

work="$(mktemp -d "${TMPDIR:-/tmp}/fkbatchpre.XXXXXX")"
trap 'rm -rf "$work"' EXIT

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

cd "$ROOT"
FILES=()
while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/first-token.fk)

# The body's answers about a FIXED file through FIXED cells are the same every run. Cached by the
# sha256 of (the cells + the blob's identity) — a changed recipe or a changed model is a cache MISS by
# construction. Shared with metal_first_token.sh: the SAME key, so neither re-emits the other's work.
mkdir -p "$CACHE"
key="$( { shasum -a 256 "${FILES[@]}" | awk '{print $1}'; printf '%s %s\n' "$BLOB" "$(stat -f %z "$BLOB")"; } | shasum -a 256 | cut -c1-16 )"
CFG="$CACHE/cfg-$key.txt"; TBL="$CACHE/tbl-$key.txt"; VOC="$CACHE/voc-$key.txt"; MSL="$CACHE/msl-$key.metal"

emit() {
    local out="$1" label="$2" expr="$3"
    if [[ -s "$out" ]] && grep -qx 'END' "$out"; then echo "  body cache HIT  $label"; return 0; fi
    local t0 t1; t0=$(date +%s)
    printf '%s\n' "$expr" > "$work/e.fk"
    "$GO_BIN" "${FILES[@]}" "$work/e.fk" > "$out.tmp" 2>"$work/e.err" || {
        echo "FAIL  $label emission failed"; tail -5 "$work/e.err"; exit 1; }
    grep -qx 'END' "$out.tmp" || { echo "FAIL  $label stream truncated"; exit 1; }
    mv "$out.tmp" "$out"; t1=$(date +%s)
    echo "  body cache MISS $label — emitted in $((t1-t0)) s"
}

echo "=== the body speaks (resolver-driven, ${#FILES[@]} cells) ==="
emit "$CFG" "config"  "(ft-emit-config \"$BLOB\")"
emit "$TBL" "table"   "(do (wtr-emit-table \"$BLOB\") (wtr-line \"END\"))"
emit "$VOC" "vocab"   "(ft-emit-vocab \"$BLOB\")"
if [[ -s "$MSL" ]]; then
    echo "  body cache HIT  msl"
else
    printf '(ft-emit-msl)\n' > "$work/e.fk"
    "$GO_BIN" "${FILES[@]}" "$work/e.fk" > "$work/msl.out" 2>"$work/e.err" || {
        echo "FAIL  MSL emission failed"; cat "$work/e.err"; exit 1; }
    awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/msl.out" > "$MSL"
    echo "  body cache MISS msl — $(wc -c < "$MSL" | tr -d ' ') bytes"
fi
# GATE B1 — the kernels this stone adds are the BODY's, and the unit's one standing invariant holds.
for k in form_q6k_matvec_lane_f32 form_q4k_matvec_lane_f32 \
         form_q6k_matmul_batch_f32 form_q4k_matmul_batch_f32 \
         form_rmsnorm_f32 form_rope_f32 form_gqa_decode_f32 form_swiglu_f32 form_add_f32 form_argmax_f32; do
    grep -q "kernel void $k" "$MSL" || { echo "FAIL  gate B1: kernel $k was not emitted"; exit 1; }
done
head -c 200 "$MSL" | grep -q '#include <metal_stdlib>' || { echo "FAIL  gate B1: metal_stdlib header is not at the top"; exit 1; }
grep -q 'using namespace metal' "$MSL" && { echo "FAIL  gate B1: the unit emitted 'using namespace metal;'"; exit 1; }
echo "PASS  gate B1 both batch kernels emitted by the body; the unit still carries no 'using namespace metal;'"

msl_sha="$(shasum -a 256 "$MSL" | cut -c1-16)"
LIB="$CACHE/ft-$msl_sha.metallib"
if [[ -f "$LIB" ]]; then
    echo "  metallib cache HIT: $(basename "$LIB")"
else
    xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
          -c "$MSL" -o "$work/ft.air" 2>"$work/metal.err" \
      && xcrun -sdk macosx metallib "$work/ft.air" -o "$LIB" 2>>"$work/metal.err" || {
        echo "FAIL  offline metal compile failed"; cat "$work/metal.err"; exit 1; }
    echo "  metallib cache MISS -> compiled and cached: $(basename "$LIB")"
fi

# ── the carrier ───────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
// STONE 7's carrier. CARRIER ONLY: it maps, binds, dispatches and times. Every kernel it runs was
// emitted by a .fk cell; the batched matmul is qk-matmul-batch.fk's.
import Metal
import Foundation

let a = CommandLine.arguments
let libPath = a[1], blobPath = a[2], tablePath = a[3], cfgPath = a[4], vocPath = a[5]
let prompt = a[6]
let nsteps = Int(a[7])!, maxpos = Int(a[8])!, PCHUNK = Int(a[9])!

var failures = 0
func check(_ ok: Bool, _ pass: String, _ fail: String) {
    if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 }
}

var cfg: [String: Double] = [:]
do {
    let lines = try String(contentsOfFile: cfgPath, encoding: .utf8).split(separator: "\n", omittingEmptySubsequences: false)
    var pendingKey: String? = nil
    for l in lines {
        let s = String(l)
        if s == "END" { continue }
        if s.hasPrefix("CONFIG ") {
            let parts = s.dropFirst(7).split(separator: " ")
            if parts.count == 2 { cfg[String(parts[0])] = Double(parts[1])!; pendingKey = nil }
            else { pendingKey = String(parts[0]) }
        } else if let k = pendingKey, let v = Double(s) { cfg[k] = v; pendingKey = nil }
    }
}
func ci(_ k: String) -> Int { Int(cfg[k]!) }
let nLayer = ci("llama.block_count"), dModel = ci("llama.embedding_length")
let dFF = ci("llama.feed_forward_length")
let nHead = ci("llama.attention.head_count"), nKV = ci("llama.attention.head_count_kv")
let headDim = ci("llama.rope.dimension_count"), vocabN = ci("llama.vocab_size")
let bosId = ci("tokenizer.ggml.bos_token_id"), eosId = ci("tokenizer.ggml.eos_token_id")
let ropeBase = Float(cfg["llama.rope.freq_base"]!)
let rmsEps = Float(cfg["llama.attention.layer_norm_rms_epsilon"]!)
let kvd = nKV * headDim
let scale = Float(1.0 / Double(headDim).squareRoot())

struct TInfo { let type: Int; let d0: Int; let d1: Int; let off: Int; let len: Int }
var table: [String: TInfo] = [:]
for l in try String(contentsOfFile: tablePath, encoding: .utf8).split(separator: "\n") {
    let p = l.split(separator: " ")
    guard p.count == 8, p[0] == "T" else { continue }
    table[String(p[1])] = TInfo(type: Int(p[2])!, d0: Int(p[4])!, d1: Int(p[5])!, off: Int(p[6])!, len: Int(p[7])!)
}
func T(_ n: String) -> TInfo {
    guard let t = table[n] else { print("FAIL  tensor \(n) is not in the body's table"); exit(1) }
    return t
}

var pieceOf: [Int: [UInt8]] = [:], idOfPiece: [String: Int] = [:]
for l in try String(contentsOfFile: vocPath, encoding: .utf8).split(separator: "\n") {
    let p = l.split(separator: " ", maxSplits: 1, omittingEmptySubsequences: false)
    guard p.count == 2, let id = Int(p[0]) else { continue }
    let hex = p[1]; var bytes: [UInt8] = []; var it = hex.startIndex
    while it < hex.endIndex { let nx = hex.index(it, offsetBy: 2)
        bytes.append(UInt8(hex[it..<nx], radix: 16)!); it = nx }
    pieceOf[id] = bytes
    idOfPiece[String(decoding: bytes, as: UTF8.self)] = id
}
var byteToUni = [Character](repeating: " ", count: 256)
var uniToByte: [Character: UInt8] = [:]
do {
    var bs: [Int] = Array(33...126) + Array(161...172) + Array(174...255)
    var cs = bs; var n = 0
    for b in 0...255 where !bs.contains(b) { bs.append(b); cs.append(256 + n); n += 1 }
    for (b, c) in zip(bs, cs) { let ch = Character(UnicodeScalar(UInt32(c))!)
        byteToUni[b] = ch; uniToByte[ch] = UInt8(b) }
}
func encodePiecesString(_ s: String) -> String { String(Array(s.utf8).map { byteToUni[Int($0)] }) }
func decodeIds(_ ids: [Int]) -> String {
    var out: [UInt8] = []
    for id in ids { guard let p = pieceOf[id] else { continue }
        for ch in String(decoding: p, as: UTF8.self) { if let b = uniToByte[ch] { out.append(b) } } }
    return String(decoding: out, as: UTF8.self)
}
func encode(_ s: String) -> [Int] {
    let chars = Array(encodePiecesString(s)); var ids = [bosId]; var i = 0
    while i < chars.count {
        var take = 0, took = -1
        var j = min(chars.count, i + 32)
        while j > i { if let id = idOfPiece[String(chars[i..<j])] { take = j - i; took = id; break }; j -= 1 }
        if took < 0 { take = 1; took = idOfPiece[String(chars[i])] ?? 0 }
        ids.append(took); i += take
    }
    return ids
}

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
let queue = dev.makeCommandQueue()!
let fd = open(blobPath, O_RDONLY)
guard fd >= 0 else { print("FAIL cannot open blob"); exit(1) }
var st = stat(); fstat(fd, &st)
let fileLen = Int(st.st_size), page = Int(getpagesize())
let mapLen = (fileLen + page - 1) / page * page
guard let mapped = mmap(nil, mapLen, PROT_READ, MAP_PRIVATE, fd, 0), mapped != MAP_FAILED,
      let modelBuf = dev.makeBuffer(bytesNoCopy: mapped, length: mapLen, options: .storageModeShared, deallocator: nil)
else { print("FAIL  mmap/bytesNoCopy over the model failed"); exit(1) }
print("resident: the whole \(fileLen)-byte blob in ONE MTLBuffer on \(dev.name), zero copies")

func pipe(_ n: String) throws -> MTLComputePipelineState {
    try dev.makeComputePipelineState(function: lib.makeFunction(name: n)!)
}
let pQ6D = try pipe("form_q6k_dequant_f32"), pQ4D = try pipe("form_q4k_dequant_f32")
let pRms = try pipe("form_rmsnorm_f32"), pRope = try pipe("form_rope_f32")
let pAttn = try pipe("form_gqa_decode_f32"), pSwi = try pipe("form_swiglu_f32")
let pAdd = try pipe("form_add_f32"), pArg = try pipe("form_argmax_f32")
let pQ6L = try pipe("form_q6k_matvec_lane_f32"), pQ4L = try pipe("form_q4k_matvec_lane_f32")
let pQ6B = try pipe("form_q6k_matmul_batch_f32"), pQ4B = try pipe("form_q4k_matmul_batch_f32")

// GATE B5 — the lane and batch kernels assume a SIMD width of exactly 32. On a device where that is
// false they are WRONG, not slow, so the width is READ from the pipeline and refused, never assumed.
let SIMDW = pQ6B.threadExecutionWidth
if SIMDW != 32 { print("SKIP  this GPU's threadExecutionWidth is \(SIMDW), not 32 — these kernels do not speak for it"); exit(2) }
print("PASS  gate B5 threadExecutionWidth read from the pipeline and it is 32")

func buf(_ n: Int) -> MTLBuffer { dev.makeBuffer(length: max(n, 16) * 4, options: .storageModeShared)! }
// ---- THE POOL. Allocated ONCE, sized for a whole prefill CHUNK, never reallocated. --------------
let bX = buf(PCHUNK * dModel), bXb = buf(PCHUNK * dModel), bQ = buf(PCHUNK * dModel)
let bAttn = buf(PCHUNK * dModel), bProj = buf(PCHUNK * dModel), bFfn = buf(PCHUNK * dModel)
let bGate = buf(PCHUNK * dFF), bUp = buf(PCHUNK * dFF), bAct = buf(PCHUNK * dFF)
let bLogits = buf(vocabN)
let bCacheK = buf(nLayer * maxpos * kvd), bCacheV = buf(nLayer * maxpos * kvd)
let sstride = maxpos
let bScratch = buf(PCHUNK * 2 * nHead * sstride)
let bOutI = dev.makeBuffer(length: 16, options: .storageModeShared)!
let bOutV = buf(4)
let poolBytes = (PCHUNK * (dModel * 6 + dFF * 3 + 2 * nHead * sstride) + vocabN
                 + 2 * nLayer * maxpos * kvd) * 4
print(String(format: "pooled: %.1f MB of activation + KV state for a chunk of %d tokens, allocated ONCE",
             Double(poolBytes) / 1048576.0, PCHUNK))

final class Step {
    let cb: MTLCommandBuffer, enc: MTLComputeCommandEncoder
    init() { cb = queue.makeCommandBuffer()!; enc = cb.makeComputeCommandEncoder(dispatchType: .concurrent)! }
    func go(_ p: MTLComputePipelineState, _ width: Int, barrier: Bool = true, tgMul32: Bool = false,
            _ bind: (MTLComputeCommandEncoder) -> Void) {
        enc.setComputePipelineState(p); bind(enc)
        var tg = min(p.maxTotalThreadsPerThreadgroup, 256)
        if tgMul32 { tg = max(32, (tg / 32) * 32) }
        enc.dispatchThreads(MTLSize(width: width, height: 1, depth: 1),
                            threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
        if barrier { enc.memoryBarrier(scope: .buffers) }
    }
    func done() { enc.endEncoding(); cb.commit(); cb.waitUntilCompleted() }
}

// STONE 5's lane matvec — ONE token. The path this stone must not diverge from.
func matvecLane(_ s: Step, _ t: TInfo, _ x: MTLBuffer, xOff: Int = 0, _ y: MTLBuffer, yOff: Int = 0,
                barrier: Bool = true) {
    var rows = UInt32(t.d1), cols = UInt32(t.d0)
    s.go(t.type == 14 ? pQ6L : pQ4L, t.d1 * 32, barrier: barrier, tgMul32: true) { e in
        e.setBuffer(modelBuf, offset: t.off, index: 0)
        e.setBuffer(x, offset: xOff, index: 1)
        e.setBuffer(y, offset: yOff, index: 2)
        e.setBytes(&rows, length: 4, index: 3); e.setBytes(&cols, length: 4, index: 4)
    }
}
// STONE 7 — THE BATCHED MATMUL. One SIMD group per (row, tile of 8 tokens); the grid is
// rows * ceil(ntok/8) * 32. x is token-major (t*cols + j) and y is token-major (t*rows + r), which is
// exactly the KV cache's per-position layout for the k and v projections.
func matmulBatch(_ s: Step, _ t: TInfo, _ x: MTLBuffer, xOff: Int = 0, _ y: MTLBuffer, yOff: Int = 0,
                 ntok: Int, barrier: Bool = true) {
    var rows = UInt32(t.d1), cols = UInt32(t.d0), nt = UInt32(ntok)
    let ntile = (ntok + 7) / 8
    s.go(t.type == 14 ? pQ6B : pQ4B, t.d1 * ntile * 32, barrier: barrier, tgMul32: true) { e in
        e.setBuffer(modelBuf, offset: t.off, index: 0)
        e.setBuffer(x, offset: xOff, index: 1)
        e.setBuffer(y, offset: yOff, index: 2)
        e.setBytes(&rows, length: 4, index: 3); e.setBytes(&cols, length: 4, index: 4)
        e.setBytes(&nt, length: 4, index: 5)
    }
}
func dequantRow(_ s: Step, _ t: TInfo, off: Int, n: Int, _ y: MTLBuffer, yOff: Int, barrier: Bool = true) {
    var o = UInt32(off), nn = UInt32(n)
    s.go(t.type == 14 ? pQ6D : pQ4D, n, barrier: barrier) { e in
        e.setBuffer(modelBuf, offset: t.off, index: 0)
        e.setBuffer(y, offset: yOff, index: 1)
        e.setBytes(&o, length: 4, index: 2); e.setBytes(&nn, length: 4, index: 3)
    }
}
func rmsnorm(_ s: Step, _ x: MTLBuffer, xOff: Int, _ gain: TInfo, _ y: MTLBuffer, yOff: Int, barrier: Bool = true) {
    var n = UInt32(dModel), eps = rmsEps
    s.go(pRms, 1, barrier: barrier) { e in
        e.setBuffer(x, offset: xOff, index: 0)
        e.setBuffer(modelBuf, offset: gain.off, index: 1)
        e.setBuffer(y, offset: yOff, index: 2)
        e.setBytes(&n, length: 4, index: 3); e.setBytes(&eps, length: 4, index: 4)
    }
}
let ropeT = T("rope_freqs.weight")
func rope(_ s: Step, _ v: MTLBuffer, off: Int, heads: Int, pos: Int, barrier: Bool = true) {
    var nh = UInt32(heads), hd = UInt32(headDim), p = UInt32(pos), b = ropeBase
    s.go(pRope, heads, barrier: barrier) { e in
        e.setBuffer(v, offset: off, index: 0)
        e.setBuffer(modelBuf, offset: ropeT.off, index: 1)
        e.setBytes(&nh, length: 4, index: 2); e.setBytes(&hd, length: 4, index: 3)
        e.setBytes(&p, length: 4, index: 4); e.setBytes(&b, length: 4, index: 5)
    }
}
func elem(_ s: Step, _ p: MTLComputePipelineState, _ x: MTLBuffer, _ y: MTLBuffer, _ z: MTLBuffer, _ n: Int) {
    var nn = UInt32(n)
    s.go(p, n) { e in
        e.setBuffer(x, offset: 0, index: 0); e.setBuffer(y, offset: 0, index: 1)
        e.setBuffer(z, offset: 0, index: 2); e.setBytes(&nn, length: 4, index: 3)
    }
}
func fvals(_ b: MTLBuffer, _ n: Int, _ off: Int = 0) -> [Float] {
    let p = b.contents().advanced(by: off * 4).bindMemory(to: Float.self, capacity: n)
    return (0..<n).map { p[$0] }
}

let embT = T("token_embd.weight"), onormT = T("output_norm.weight")
struct Layer { let an, q, k, v, o, fn, fg, fu, fd: TInfo }
let L: [Layer] = (0..<nLayer).map { l in
    Layer(an: T("blk.\(l).attn_norm.weight"), q: T("blk.\(l).attn_q.weight"),
          k: T("blk.\(l).attn_k.weight"), v: T("blk.\(l).attn_v.weight"),
          o: T("blk.\(l).attn_output.weight"), fn: T("blk.\(l).ffn_norm.weight"),
          fg: T("blk.\(l).ffn_gate.weight"), fu: T("blk.\(l).ffn_up.weight"),
          fd: T("blk.\(l).ffn_down.weight"))
}
// attention for ONE query position — the DECODE kernel, unchanged, given this token's offsets.
func attn1(_ s: Step, _ l: Int, _ t: Int, _ pos: Int, barrier: Bool = true) {
    var npos = UInt32(pos + 1), nq = UInt32(nHead), nkv = UInt32(nKV)
    var hd = UInt32(headDim), sc = scale, ss = UInt32(sstride)
    s.go(pAttn, nHead, barrier: barrier) { e in
        e.setBuffer(bQ, offset: t * dModel * 4, index: 0)
        e.setBuffer(bCacheK, offset: l * maxpos * kvd * 4, index: 1)
        e.setBuffer(bCacheV, offset: l * maxpos * kvd * 4, index: 2)
        e.setBuffer(bAttn, offset: t * dModel * 4, index: 3)
        e.setBuffer(bScratch, offset: t * 2 * nHead * sstride * 4, index: 4)
        e.setBytes(&npos, length: 4, index: 5); e.setBytes(&nq, length: 4, index: 6)
        e.setBytes(&nkv, length: 4, index: 7); e.setBytes(&hd, length: 4, index: 8)
        e.setBytes(&sc, length: 4, index: 9); e.setBytes(&ss, length: 4, index: 10)
    }
}

// ---- THE TOKEN-AT-A-TIME PATH (Stone 5's), kept verbatim as the thing this stone answers to -------
func forward1(_ id: Int, _ pos: Int) -> Int {
    let s = Step()
    dequantRow(s, embT, off: id * dModel, n: dModel, bX, yOff: 0)
    for l in 0..<nLayer {
        let kOff = (l * maxpos + pos) * kvd * 4
        rmsnorm(s, bX, xOff: 0, L[l].an, bXb, yOff: 0)
        matvecLane(s, L[l].q, bXb, xOff: 0, bQ, yOff: 0, barrier: false)
        matvecLane(s, L[l].k, bXb, xOff: 0, bCacheK, yOff: kOff, barrier: false)
        matvecLane(s, L[l].v, bXb, xOff: 0, bCacheV, yOff: kOff)
        rope(s, bQ, off: 0, heads: nHead, pos: pos, barrier: false)
        rope(s, bCacheK, off: kOff, heads: nKV, pos: pos)
        attn1(s, l, 0, pos)
        matvecLane(s, L[l].o, bAttn, xOff: 0, bProj, yOff: 0)
        elem(s, pAdd, bX, bProj, bX, dModel)
        rmsnorm(s, bX, xOff: 0, L[l].fn, bXb, yOff: 0)
        matvecLane(s, L[l].fg, bXb, xOff: 0, bGate, yOff: 0, barrier: false)
        matvecLane(s, L[l].fu, bXb, xOff: 0, bUp, yOff: 0)
        elem(s, pSwi, bGate, bUp, bAct, dFF)
        matvecLane(s, L[l].fd, bAct, xOff: 0, bFfn, yOff: 0)
        elem(s, pAdd, bX, bFfn, bX, dModel)
    }
    rmsnorm(s, bX, xOff: 0, onormT, bXb, yOff: 0)
    matvecLane(s, embT, bXb, xOff: 0, bLogits, yOff: 0)
    var nn = UInt32(vocabN)
    s.go(pArg, 1) { e in
        e.setBuffer(bLogits, offset: 0, index: 0); e.setBuffer(bOutI, offset: 0, index: 1)
        e.setBuffer(bOutV, offset: 0, index: 2); e.setBytes(&nn, length: 4, index: 3)
    }
    s.done()
    return Int(bOutI.contents().bindMemory(to: UInt32.self, capacity: 1)[0])
}

// ---- STONE 7: ONE forward pass for a WHOLE CHUNK of prompt tokens --------------------------------
// `last` says whether this chunk ends the prompt; only then are output_norm and the 128256-row
// unembedding run, and only for the chunk's LAST token. Under token-at-a-time they ran for every
// prompt token and every one of them but the last was thrown away.
func forwardChunk(_ ids: [Int], _ pos0: Int, last: Bool) -> Int {
    let P = ids.count
    let s = Step()
    // the embedding gather: P rows of token_embd, disjoint outputs, so P concurrent dispatches of the
    // SAME decode kernel — no batched twin needed and none written.
    for (t, id) in ids.enumerated() {
        dequantRow(s, embT, off: id * dModel, n: dModel, bX, yOff: t * dModel * 4, barrier: t == P - 1)
    }
    for l in 0..<nLayer {
        let kBase = (l * maxpos + pos0) * kvd * 4
        for t in 0..<P { rmsnorm(s, bX, xOff: t * dModel * 4, L[l].an, bXb, yOff: t * dModel * 4, barrier: t == P - 1) }
        // q, k and v read the same normed block and write disjoint buffers: no barrier until all three
        // are in. k and v land STRAIGHT in the cache at positions pos0 .. pos0+P-1.
        matmulBatch(s, L[l].q, bXb, bQ, ntok: P, barrier: false)
        matmulBatch(s, L[l].k, bXb, bCacheK, yOff: kBase, ntok: P, barrier: false)
        matmulBatch(s, L[l].v, bXb, bCacheV, yOff: kBase, ntok: P)
        for t in 0..<P {
            rope(s, bQ, off: t * dModel * 4, heads: nHead, pos: pos0 + t, barrier: false)
            rope(s, bCacheK, off: kBase + t * kvd * 4, heads: nKV, pos: pos0 + t,
                 barrier: t == P - 1)
        }
        // attention: one dispatch per query position, each over its OWN causal prefix
        // (npos = pos0 + t + 1). The whole chunk's k/v are already in the cache, so a token attends to
        // its predecessors inside this chunk exactly as it would have one at a time.
        for t in 0..<P { attn1(s, l, t, pos0 + t, barrier: t == P - 1) }
        matmulBatch(s, L[l].o, bAttn, bProj, ntok: P)
        elem(s, pAdd, bX, bProj, bX, P * dModel)
        for t in 0..<P { rmsnorm(s, bX, xOff: t * dModel * 4, L[l].fn, bXb, yOff: t * dModel * 4, barrier: t == P - 1) }
        matmulBatch(s, L[l].fg, bXb, bGate, ntok: P, barrier: false)
        matmulBatch(s, L[l].fu, bXb, bUp, ntok: P)
        elem(s, pSwi, bGate, bUp, bAct, P * dFF)
        matmulBatch(s, L[l].fd, bAct, bFfn, ntok: P)
        elem(s, pAdd, bX, bFfn, bX, P * dModel)
    }
    if !last { s.done(); return -1 }
    let tl = P - 1
    rmsnorm(s, bX, xOff: tl * dModel * 4, onormT, bXb, yOff: 0)
    matvecLane(s, embT, bXb, xOff: 0, bLogits, yOff: 0)
    var nn = UInt32(vocabN)
    s.go(pArg, 1) { e in
        e.setBuffer(bLogits, offset: 0, index: 0); e.setBuffer(bOutI, offset: 0, index: 1)
        e.setBuffer(bOutV, offset: 0, index: 2); e.setBytes(&nn, length: 4, index: 3)
    }
    s.done()
    return Int(bOutI.contents().bindMemory(to: UInt32.self, capacity: 1)[0])
}

func zeroPool() {
    memset(bCacheK.contents(), 0, bCacheK.length); memset(bCacheV.contents(), 0, bCacheV.length)
    memset(bX.contents(), 0, bX.length)
}

// ---- GATE B2: no epsilon, and it is MEASURED ------------------------------------------------------
// The batched matmul must be BIT-EXACT against P sequential lane matvecs — not inside a bound, equal.
// Within a lane the fold for token t is `pr + acc` over the same columns in the same k-DOWN order, and
// the cross-lane reduction is the same metal::simd_sum over the same 32 partials; the tile index picks
// an ACCUMULATOR, never an order. That is what lets qk-matvec-split.fk's derived bound stand unchanged
// with no new derivation. Checked on real tensors of BOTH quants at four batch sizes, because a tail
// tile (P not a multiple of 8) is where a batched kernel goes wrong.
func gateB2() {
    var worstBad = 0, tested = 0
    for name in ["blk.0.attn_q.weight", "blk.0.ffn_down.weight"] {
        let t = T(name)
        let rows = t.d1, cols = t.d0
        let quant = t.type == 14 ? "Q6_K" : "Q4_K"
        for P in [1, 6, 32, PCHUNK] {
            guard P <= PCHUNK, cols * P <= bAct.length / 4, rows * P <= bGate.length / 4 else { continue }
            // a real, varied activation block: P distinct rows of token_embd, dequantized by the body.
            let sd = Step()
            for t2 in 0..<P { dequantRow(sd, embT, off: (100 + t2 * 7) * dModel, n: dModel, bAct,
                                         yOff: t2 * cols * 4, barrier: t2 == P - 1) }
            sd.done()
            // widen each 3072-wide row to `cols` by repeating it, so an 8192-column tensor gets real
            // numbers everywhere rather than a zero tail that would make every association agree.
            if cols > dModel {
                let p = bAct.contents().bindMemory(to: Float.self, capacity: P * cols)
                for t2 in 0..<P { for j in dModel..<cols { p[t2 * cols + j] = p[t2 * cols + (j % dModel)] } }
            }
            let s1 = Step()
            for t2 in 0..<P { matvecLane(s1, t, bAct, xOff: t2 * cols * 4, bGate, yOff: t2 * rows * 4,
                                         barrier: t2 == P - 1) }
            s1.done()
            let ref = fvals(bGate, P * rows)
            let s2 = Step(); matmulBatch(s2, t, bAct, bUp, ntok: P); s2.done()
            let got = fvals(bUp, P * rows)
            var bad = 0
            for i in 0..<(P * rows) where ref[i] != got[i] { bad += 1 }
            worstBad += bad; tested += 1
            print("    \(quant) \(rows)x\(cols)  P=\(P): \(bad == 0 ? "BIT-EXACT" : "DIFFERS on \(bad)") of \(P * rows) floats vs \(P) sequential lane matvecs")
        }
    }
    check(worstBad == 0,
      "gate B2 the batched matmul is BIT-EXACT against P sequential lane matvecs, both quants, \(tested) batch sizes — batching reassociates NOTHING and needs no new epsilon",
      "gate B2 the batched matmul differs from the lane matvecs on \(worstBad) floats — it reassociated something")
}

struct Run { var out: [Int] = []; var prefill = 0.0; var decode = 0.0; var forwards = 0; var np = 0 }
// the token-at-a-time path: Stone 5's, unchanged, the thing this stone answers to
func generateSerial(_ ids: [Int], _ steps: Int) -> Run {
    zeroPool()
    var r = Run(); var pos = 0; var cur = 0
    r.np = ids.count
    let t0 = Date()
    for id in ids { cur = forward1(id, pos); pos += 1 }
    let t1 = Date()
    for _ in 0..<steps { r.out.append(cur)
        if cur == eosId || cur == 128001 { break }
        cur = forward1(cur, pos); pos += 1; r.forwards += 1 }
    let t2 = Date()
    r.prefill = t1.timeIntervalSince(t0); r.decode = t2.timeIntervalSince(t1)
    return r
}
// STONE 7's path: the prompt in chunks of PCHUNK, then decode one at a time exactly as before.
func generateBatched(_ ids: [Int], _ steps: Int) -> Run {
    zeroPool()
    var r = Run(); var cur = 0
    r.np = ids.count
    let t0 = Date()
    var at = 0
    while at < ids.count {
        let n = min(PCHUNK, ids.count - at)
        let isLast = (at + n == ids.count)
        let got = forwardChunk(Array(ids[at..<(at + n)]), at, last: isLast)
        if isLast { cur = got }
        at += n
    }
    let t1 = Date()
    var pos = ids.count
    for _ in 0..<steps { r.out.append(cur)
        if cur == eosId || cur == 128001 { break }
        cur = forward1(cur, pos); pos += 1; r.forwards += 1 }
    let t2 = Date()
    r.prefill = t1.timeIntervalSince(t0); r.decode = t2.timeIntervalSince(t1)
    return r
}

// ---- AN EXTERNAL DENOMINATOR (corpus row 834, selfgauge) -----------------------------------------
// An ollama/llama.cpp oracle measured on THIS machine, THIS model and THIS 2.0 GB blob over a
// 150-token sample. A MEASUREMENT MADE ELSEWHERE, quoted, never re-run here and never mixed into a
// gate — carried so that no ratio below can be read without its absolute cost.
let ollamaDecode = 157.83, ollamaPrefill = 640.94

let promptIds = encode(prompt)
print("=== the prompt ===")
print("  \"\(prompt)\"")
print("  ids: \(promptIds)")
if promptIds.count + nsteps >= maxpos { print("FAIL  prompt+steps exceeds the KV pool"); exit(1) }

print("=== gate B2: the batched matmul answers to the lane matvec, bit for bit ===")
gateB2()
if failures > 0 { print("=== gate(s) failed BEFORE any token — refusing to report a rate ==="); exit(1) }

print("=== gate B3: the batched-prefill path generates the attestant's tokens ===")
let sRun = generateSerial(promptIds, nsteps)
let bRun = generateBatched(promptIds, nsteps)
func report(_ label: String, _ r: Run) {
    print(String(format: "  %-18@ prefill %.3f s for %d prompt tokens (%.2f tok/s)  |  decode %.3f s for %d forwards (%.2f tok/s)",
                 label as NSString, r.prefill, r.np, Double(r.np) / r.prefill,
                 r.decode, r.forwards, Double(max(1, r.forwards)) / r.decode))
    print(String(format: "    END-TO-END %.3f tok/s over %d generated tokens (prefill+decode %.3f s)",
                 Double(r.out.count) / (r.prefill + r.decode), r.out.count, r.prefill + r.decode))
    print("    text : \"\(decodeIds(r.out))\"")
}
report("token-at-a-time", sRun)
report("batched prefill", bRun)
check(bRun.out == sRun.out,
  "gate B3 the batched-prefill path generates the SAME \(sRun.out.count) token ids as the token-at-a-time path: \(bRun.out)",
  "gate B3 the batched path diverged: \(bRun.out) vs \(sRun.out)")
print(String(format: "  prefill %.2fx (%.3f s -> %.3f s)   |   END-TO-END %.2fx (%.3f -> %.3f tok/s)",
             sRun.prefill / bRun.prefill, sRun.prefill, bRun.prefill,
             (sRun.prefill + sRun.decode) / (bRun.prefill + bRun.decode),
             Double(sRun.out.count) / (sRun.prefill + sRun.decode),
             Double(bRun.out.count) / (bRun.prefill + bRun.decode)))
print(String(format: "  vs the world (ollama on this machine, quoted): prefill %.2f of %.2f tok/s (%.1fx behind)  |  decode %.2f of %.2f tok/s (%.1fx behind)",
             Double(bRun.np) / bRun.prefill, ollamaPrefill, ollamaPrefill / (Double(bRun.np) / bRun.prefill),
             Double(max(1, bRun.forwards)) / bRun.decode, ollamaDecode,
             ollamaDecode / (Double(max(1, bRun.forwards)) / bRun.decode)))

// ---- GATE B4: FOUR prompt lengths and a slope (corpus row 827, unispan) ---------------------------
// Batching's whole benefit is length-dependent, so a rate quoted at one prompt length would be a point
// pretending to be a line. A long natural text is encoded once and SLICED, so every length is a real
// prefix of real ids and not a synthetic repeat of one token.
let longText = String(repeating: "The capital of France is Paris and the capital of Italy is Rome. " +
                                "Rivers run to the sea, and the sea does not fill. ", count: 24)
let longIds = encode(longText)
print("=== gate B4: prefill at four prompt lengths, and a slope ===")
print("  (a real \(longIds.count)-token encoding, sliced — every length is a genuine prefix)")
var lens: [Int] = [6, 32, 128, 512].filter { $0 <= longIds.count && $0 + 4 < maxpos }
var serialT: [Double] = [], batchT: [Double] = []
print("  P      token-at-a-time        batched          x        prefill tok/s   of ollama 640.94")
for P in lens {
    let ids = Array(longIds[0..<P])
    zeroPool(); let t0 = Date()
    for (i, id) in ids.enumerated() { _ = forward1(id, i) }
    let sT = Date().timeIntervalSince(t0)
    zeroPool(); let t1 = Date()
    var at = 0
    while at < P { let n = min(PCHUNK, P - at)
        _ = forwardChunk(Array(ids[at..<(at + n)]), at, last: at + n == P); at += n }
    let bT = Date().timeIntervalSince(t1)
    serialT.append(sT); batchT.append(bT)
    print(String(format: "  %4d   %8.3f s            %8.3f s      %5.2fx    %8.2f        %5.1fx behind",
                 P, sT, bT, sT / bT, Double(P) / bT, ollamaPrefill / (Double(P) / bT)))
}
check(lens.count >= 3, "gate B4 prefill measured at \(lens.count) prompt lengths, not one",
                       "gate B4 fewer than three prompt lengths were measured")
if lens.count >= 2 {
    let i0 = 0, i1 = lens.count - 1
    let sSlope = (serialT[i1] - serialT[i0]) / Double(lens[i1] - lens[i0])
    let bSlope = (batchT[i1] - batchT[i0]) / Double(lens[i1] - lens[i0])
    print(String(format: "  SLOPE, s per additional prompt token: token-at-a-time %.5f (%.1f tok/s marginal), batched %.5f (%.1f tok/s marginal) — %.2fx",
                 sSlope, 1.0 / max(1e-9, sSlope), bSlope, 1.0 / max(1e-9, bSlope), sSlope / max(1e-9, bSlope)))
}

print(failures == 0 ? "=== VERDICT PASS — 5 gates ===" : "=== VERDICT FAIL — \(failures) gate(s) ===")
exit(failures == 0 ? 0 : 1)
SWIFT

echo "=== compiling the carrier ==="
swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc failed"; tail -40 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB" "$BLOB" "$TBL" "$CFG" "$VOC" "$PROMPT" "$NSTEPS" "$MAXPOS" "$PCHUNK"
exit $?
