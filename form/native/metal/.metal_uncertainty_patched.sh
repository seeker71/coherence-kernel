#!/usr/bin/env bash
# metal_first_token.sh — THE FIRST REAL llama3.2:3b TOKEN GENERATED FORM-NATIVE, and an honest
# end-to-end tokens/second for it.
#
# The claim, and nothing wider: real llama3.2:3b weights, FULL width (d=3072, 28 layers, 24/8 GQA
# heads, dff=8192, vocab 128 256), real tokenizer ids in and real token ids out, every arithmetic op
# executed by a kernel the BODY emitted, off the ONE resident quantized buffer Stone 3 mapped. No f32
# copy of any tensor is ever materialized — not on the host, not on the device.
#
# WHAT WAS MISSING BEFORE THIS. Stone 1 measured 43.8 tok/s at dim=32 with ONE layer — the only
# end-to-end number this program had. Stone 3 made every COMPONENT fast and proven, and generated
# nothing. The distance between them was not arithmetic: it was that the ops BETWEEN the matvecs
# (RMSNorm, RoPE, GQA attention over the pooled cache, SwiGLU, the residual, the argmax) existed only
# inside single-threaded whole-block kernels that take f32 weight pointers. llama-decode-msl.fk splits
# them out; this carrier threads them.
#
# Who decides what (the dumb-carrier discipline the Metal lane keeps):
#   the BODY  native/metal/first-token.fk  — the config (from the file's own metadata), the tensor
#             table, the tokenizer, the emitted MSL, and Form's own fp64 answers at three points.
#   the BODY  form-stdlib/{q6k,q4k}-msl.fk + form-stdlib/llama-decode-msl.fk — every kernel.
#   the CARRIER (this file + the Swift runner it writes) — mmap, bind, dispatch, time, and the
#             byte-alphabet rendering of a token id back into text (named as a gap in the receipt:
#             the GPT-2 byte alphabet is the one piece of tokenizer knowledge still in the carrier).
#
# THE GATES (all must pass before any token is believed):
#   1  THE CONFIG IS THE FILE'S           28/3072/8192/24/8/128, rope base and rms eps read, not typed.
#   2  THE EMBEDDING IS THE BODY'S        the GPU gather of token_embd row `id` equals Form's dequant
#                                         BIT FOR BIT (Q6_K's one-rounding argument, q6k-msl.fk).
#   3  RMSNORM IS THE BODY'S              the GPU RMSNorm of that row against real blk.0.attn_norm
#                                         gains tracks Form's fp64 within the stated n*u bound.
#   4  A REAL Q4_K MATVEC IS THE BODY'S   GPU q[r] equals Form's fp64 dot of blk.0.attn_q row r with
#                                         that normed vector, within the DERIVED cols*u*SUM|term|.
#   5  THE CACHE IS A CACHE               the KV pool is allocated ONCE for the whole run; the k/v of
#                                         position p are written by the projection itself and never
#                                         recomputed. Gate: re-running the prompt reproduces the same
#                                         ids from a freshly zeroed pool.
#   6  A TOKEN                            greedy decode emits ids; they are legal vocab indices; they
#                                         are not constant across different prompts.
#   7  TWO SIZES AND A SLOPE              the rate is measured at two different generation lengths, so
#                                         no tok/s here is one point pretending to be a line
#                                         (corpus row 827, unispan).
#
# Run:  form/native/metal/metal_first_token.sh [nsteps] ["prompt"]        (defaults 12, a fixed prompt)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every other Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
NSTEPS="${1:-12}"
PROMPT="${2:-The capital of France is}"
BLOB="${FORM_GGUF_BLOB:-$HOME/.ollama/models/blobs/sha256-dde5aa3fc5ffc17176b5e8bdc82f587b24b2678c6c66101bf7da77af9f7ccdff}"
REFID="${FORM_REF_ID:-791}"          # the token id gates 2-4 are taken at
REFROW="${FORM_REF_ROW:-0}"          # the attn_q row gate 4 is taken at
MAXPOS="${FORM_MAXPOS:-256}"
CACHE="$ROOT/native/metal/.first-token-cache"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the GPU witness needs an Apple GPU + swiftc"
    exit 2
fi
if [[ ! -f "$BLOB" ]]; then
    echo "SKIP  the llama3.2:3b GGUF blob is not on this host: $BLOB"
    echo "      (ollama pull llama3.2:3b, or set FORM_GGUF_BLOB)"
    exit 2
fi
if [[ ! -x "$GO_BIN" ]]; then
    echo "  building go kernel..." >&2
    (cd "$ROOT/form-kernel-go" && go build -o bin-go .)
fi

work="$(mktemp -d "${TMPDIR:-/tmp}/fkfirsttok.XXXXXX")"
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

# The body's answers about a FIXED file through FIXED cells are the same every run, and they cost
# ~55 s of header walking. Cache them by the sha256 of (the cells + the blob's identity) — a changed
# recipe or a changed model is a cache MISS by construction, never a stale answer.
mkdir -p "$CACHE"
key="$( { shasum -a 256 "${FILES[@]}" | awk '{print $1}'; printf '%s %s\n' "$BLOB" "$(stat -f %z "$BLOB")"; } | shasum -a 256 | cut -c1-16 )"
CFG="$CACHE/cfg-$key.txt"; TBL="$CACHE/tbl-$key.txt"; VOC="$CACHE/voc-$key.txt"; MSL="$CACHE/msl-$key.metal"
REF="$CACHE/ref-$key-$REFID-$REFROW.txt"

emit() {   # emit <outfile> <label> <form-expr>
    local out="$1" label="$2" expr="$3"
    if [[ -s "$out" ]] && grep -qx 'END' "$out"; then
        echo "  body cache HIT  $label"
        return 0
    fi
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
emit "$REF" "refs"    "(ft-emit-ref \"$BLOB\" $REFID $REFROW)"
if [[ -s "$MSL" ]]; then
    echo "  body cache HIT  msl"
else
    printf '(ft-emit-msl)\n' > "$work/e.fk"
    "$GO_BIN" "${FILES[@]}" "$work/e.fk" > "$work/msl.out" 2>"$work/e.err" || {
        echo "FAIL  MSL emission failed"; cat "$work/e.err"; exit 1; }
    awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/msl.out" > "$MSL"
    echo "  body cache MISS msl — $(wc -c < "$MSL" | tr -d ' ') bytes"
fi
for k in form_q6k_dequant_f32 form_q6k_matvec_f32 form_q4k_dequant_f32 form_q4k_matvec_f32 \
         form_q6k_matvec_part_f32 form_q4k_matvec_part_f32 form_part_combine_f32 \
         form_q6k_matvec_hoist_f32 form_q4k_matvec_hoist_f32 \
         form_q6k_matvec_lane_f32 form_q4k_matvec_lane_f32 \
         form_rmsnorm_f32 form_rope_f32 form_gqa_decode_f32 form_swiglu_f32 form_add_f32 form_argmax_f32 \
         form_rmsnorm_tg_f32 form_rope_pair_f32 form_argmax_part_f32 form_argmax_comb_f32; do
    grep -q "kernel void $k" "$MSL" || { echo "FAIL  kernel $k was not emitted"; exit 1; }
done
# The header must LEAD the unit and must NOT drag `using namespace metal;` in with it — the body's own
# `round` is unqualified and has to keep resolving to the body's. Checked here rather than trusted,
# because the failure mode is a compile error a hundred lines away from its cause.
head -c 200 "$MSL" | grep -q '#include <metal_stdlib>' || { echo "FAIL  metal_stdlib header is not at the top of the unit"; exit 1; }
grep -q 'using namespace metal' "$MSL" && { echo "FAIL  the unit emitted 'using namespace metal;' — the body's round becomes ambiguous"; exit 1; }
echo "  21 kernels emitted, $(wc -c < "$MSL" | tr -d ' ') bytes, every character authored by a .fk cell"

# ── the .metallib, cached across RUNS by the emitted source's own sha256 ──────────────────────
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
// The first form-native llama3.2:3b token. CARRIER ONLY: it maps, binds, dispatches and times.
// Every number it judges came from the body; every kernel it runs was emitted by a .fk cell.
import Metal
import Foundation

let a = CommandLine.arguments
let libPath = a[1], blobPath = a[2], tablePath = a[3], cfgPath = a[4]
let vocPath = a[5], refPath = a[6], prompt = a[7]
let nsteps = Int(a[8])!, maxpos = Int(a[9])!, refId = Int(a[10])!, refRow = Int(a[11])!

var failures = 0
func check(_ ok: Bool, _ pass: String, _ fail: String) {
    if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 }
}

// ---- the body's config -------------------------------------------------------------------------
var cfg: [String: Double] = [:]
do {
    let lines = try String(contentsOfFile: cfgPath, encoding: .utf8).split(separator: "\n", omittingEmptySubsequences: false)
    var pendingKey: String? = nil
    for l in lines {
        let s = String(l)
        if s == "END" { continue }
        if s.hasPrefix("CONFIG ") {
            let parts = s.dropFirst(7).split(separator: " ")
            if parts.count == 2 { cfg[String(parts[0])] = Double(parts[1])! ; pendingKey = nil }
            else { pendingKey = String(parts[0]) }        // float values print on the NEXT line
        } else if let k = pendingKey, let v = Double(s) { cfg[k] = v; pendingKey = nil }
    }
}
func ci(_ k: String) -> Int { Int(cfg[k]!) }
let nLayer = ci("llama.block_count"), dModel = ci("llama.embedding_length")
let dFF = ci("llama.feed_forward_length")
let nHead = ci("llama.attention.head_count"), nKV = ci("llama.attention.head_count_kv")
let headDim = ci("llama.rope.dimension_count")
let vocabN = ci("llama.vocab_size")
let bosId = ci("tokenizer.ggml.bos_token_id"), eosId = ci("tokenizer.ggml.eos_token_id")
let ropeBase = Float(cfg["llama.rope.freq_base"]!)
let rmsEps = Float(cfg["llama.attention.layer_norm_rms_epsilon"]!)
let tied = ci("tied_embeddings") == 1
let kvd = nKV * headDim
let scale = Float(1.0 / Double(headDim).squareRoot())

print("=== gate 1: the config is the FILE's ===")
print("  layers \(nLayer)  d \(dModel)  dff \(dFF)  heads \(nHead)/\(nKV)  head_dim \(headDim)  vocab \(vocabN)")
print("  rope_base \(ropeBase)  rms_eps \(rmsEps)  bos \(bosId)  eos \(eosId)  tied \(tied)")
check(nLayer > 0 && dModel > 0 && nHead % nKV == 0 && headDim * nHead == dModel && tied,
      "gate 1 config read from the blob's own metadata KVs, self-consistent",
      "gate 1 config is not self-consistent")

// ---- the body's tensor table ---------------------------------------------------------------------
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

// ---- the body's tokenizer ------------------------------------------------------------------------
// A piece is BYTES (the body streamed hex, precisely so nothing re-encodes it). The GPT-2 byte
// alphabet that maps those bytes back to real bytes is the one piece of tokenizer knowledge still
// living in the carrier — named as an open row in the receipt, not pretended away.
var pieceOf: [Int: [UInt8]] = [:]
var idOfPiece: [String: Int] = [:]
do {
    for l in try String(contentsOfFile: vocPath, encoding: .utf8).split(separator: "\n") {
        let p = l.split(separator: " ", maxSplits: 1, omittingEmptySubsequences: false)
        guard p.count == 2, let id = Int(p[0]) else { continue }
        let hex = p[1]; var bytes: [UInt8] = []; var it = hex.startIndex
        while it < hex.endIndex {
            let nx = hex.index(it, offsetBy: 2)
            bytes.append(UInt8(hex[it..<nx], radix: 16)!); it = nx
        }
        pieceOf[id] = bytes
        idOfPiece[String(decoding: bytes, as: UTF8.self)] = id
    }
}
var byteToUni = [Character](repeating: " ", count: 256)
var uniToByte: [Character: UInt8] = [:]
do {
    var bs: [Int] = Array(33...126) + Array(161...172) + Array(174...255)
    var cs = bs; var n = 0
    for b in 0...255 where !bs.contains(b) { bs.append(b); cs.append(256 + n); n += 1 }
    for (b, c) in zip(bs, cs) {
        let ch = Character(UnicodeScalar(UInt32(c))!)
        byteToUni[b] = ch; uniToByte[ch] = UInt8(b)
    }
}
func encodePiecesString(_ s: String) -> String { String(Array(s.utf8).map { byteToUni[Int($0)] }) }
func decodeIds(_ ids: [Int]) -> String {
    var out: [UInt8] = []
    for id in ids {
        guard let p = pieceOf[id] else { continue }
        for ch in String(decoding: p, as: UTF8.self) { if let b = uniToByte[ch] { out.append(b) } }
    }
    return String(decoding: out, as: UTF8.self)
}
// greedy LONGEST-MATCH over the vocabulary. Not BPE's merge order — stated, not hidden. For prompts
// whose words are whole vocabulary entries the two agree; the receipt prints the pieces so a reader
// can see exactly what went in.
func encode(_ s: String) -> [Int] {
    let chars = Array(encodePiecesString(s)); var ids = [bosId]; var i = 0
    while i < chars.count {
        var take = 0, took = -1
        var j = min(chars.count, i + 32)
        while j > i {
            if let id = idOfPiece[String(chars[i..<j])] { take = j - i; took = id; break }
            j -= 1
        }
        if took < 0 { take = 1; took = idOfPiece[String(chars[i])] ?? 0 }
        ids.append(took); i += take
    }
    return ids
}

// ---- the body's fp64 references ------------------------------------------------------------------
var refEmb: [Double] = [], refRms: [Double] = [], refQ: Double = 0
do {
    var sec = ""
    for l in try String(contentsOfFile: refPath, encoding: .utf8).split(separator: "\n") {
        let s = String(l)
        if s.hasPrefix("REFEMB") { sec = "E"; continue }
        if s.hasPrefix("REFRMS") { sec = "R"; continue }
        if s.hasPrefix("REFQ")   { sec = "Q"; continue }
        if s == "END" { sec = ""; continue }   // the kernel prints the expression's own value after END
        guard let v = Double(s) else { continue }
        if sec == "E" { refEmb.append(v) } else if sec == "R" { refRms.append(v) } else if sec == "Q" { refQ = v; sec = "" }
    }
}

// ---- the device, and THE WHOLE MODEL, RESIDENT ---------------------------------------------------
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
let pQ6D = try pipe("form_q6k_dequant_f32"), pQ6M = try pipe("form_q6k_matvec_f32")
let pQ4D = try pipe("form_q4k_dequant_f32"), pQ4M = try pipe("form_q4k_matvec_f32")
let pRms = try pipe("form_rmsnorm_f32"), pRope = try pipe("form_rope_f32")
let pAttn = try pipe("form_gqa_decode_f32"), pSwi = try pipe("form_swiglu_f32")
let pAdd = try pipe("form_add_f32"), pArg = try pipe("form_argmax_f32")
let pQ6P = try pipe("form_q6k_matvec_part_f32"), pQ4P = try pipe("form_q4k_matvec_part_f32")
let pComb = try pipe("form_part_combine_f32")
// STONE 5: the hoisted split (bit-exact, no epsilon) and the SIMD-group lane kernel (reassociates,
// answers to the named epsilon). qk-matvec-lane.fk.
let pQ6H = try pipe("form_q6k_matvec_hoist_f32"), pQ4H = try pipe("form_q4k_matvec_hoist_f32")
let pQ6L = try pipe("form_q6k_matvec_lane_f32"), pQ4L = try pipe("form_q4k_matvec_lane_f32")
// STONE 13: the SLOT kernels. Same signature and same dispatch shape as the lane kernels — one SIMD
// group per row, rows*32 threads — so they drop into the same carrier slot. The ONLY difference is
// the thread map, which is the whole measured win. qk-matvec-slot.fk.
let pQ6S = try pipe("form_q6k_matvec_slot_f32"), pQ4S = try pipe("form_q4k_matvec_slot_f32")
// STONE 16: the four FAST TWINS of the ops that were dispatched one thread at a time. Each is bit for
// bit its attestant (llama-decode-msl.fk states why for each), and each attestant stays right here
// and stays on the attestant's path — the twins are what the lane and slot paths run, and gate 11
// makes them produce the attestant's own token ids or fall.
let pRmsT = try pipe("form_rmsnorm_tg_f32"), pRopeP = try pipe("form_rope_pair_f32")
let pArgP = try pipe("form_argmax_part_f32"), pArgC = try pipe("form_argmax_comb_f32")
// THE RADII, ASKED OF THIS MODEL RATHER THAN ASSUMED OF EVERY MODEL. The cooperative RMSNorm holds a
// COMPILE-TIME 4096-float threadgroup scratch and reduces across ONE threadgroup; the pair-grain RoPE
// has no trailing-odd arm where the per-head kernel silently drops one; the argmax combine holds 4096
// partials. Outside any of these the twin is WRONG, not slow. Refused here, not discovered later.
let ARGP = 1024                                   // argmax partitions; also bounded by the combine's scratch
let rmsTG = min(pRmsT.maxTotalThreadsPerThreadgroup, 1024)
let argTG = min(pArgC.maxTotalThreadsPerThreadgroup, 1024)
if dModel > 4096 || headDim % 2 != 0 || ARGP > 4096 {
    print("SKIP  this model leaves a Stone 16 twin's stated radius (d=\(dModel) hd=\(headDim) parts=\(ARGP))")
    exit(2)
}
// THE LANE KERNEL ASSUMES A SIMD WIDTH OF EXACTLY 32 (qk-matvec-lane.fk's stated radius). On a device
// where that is false the kernel is WRONG, not slow, so it is READ from the pipeline and refused —
// never assumed. A hardware constant a kernel silently depends on is the aporon error (corpus 811).
let SIMDW = pQ6L.threadExecutionWidth
if SIMDW != 32 {
    print("SKIP  this GPU's threadExecutionWidth is \(SIMDW), not 32 — the lane kernel does not speak for it")
    exit(2)
}
// PARTS = 1 is the attestant itself (one chunk, same direction, added to nothing). Anything above 1
// reassociates the row sum and must answer to the named epsilon below.
let PARTS = Int(ProcessInfo.processInfo.environment["FORM_PARTS"] ?? "32")!

func buf(_ n: Int) -> MTLBuffer { dev.makeBuffer(length: max(n, 16) * 4, options: .storageModeShared)! }
// ---- THE POOL. Allocated ONCE, before any token, and never reallocated. --------------------------
let bX = buf(dModel), bXb = buf(dModel), bQ = buf(dModel)
let bAttn = buf(dModel), bProj = buf(dModel), bFfn = buf(dModel)
let bGate = buf(dFF), bUp = buf(dFF), bAct = buf(dFF)
let bLogits = buf(vocabN)
let bCacheK = buf(nLayer * maxpos * kvd), bCacheV = buf(nLayer * maxpos * kvd)
let bScratch = buf(2 * nHead * maxpos)
let bPart = buf(vocabN * PARTS)          // the split fold's partials, pooled once like everything else
let bXbT = buf(dModel)                   // gate 3's second answer: the cooperative RMSNorm twin's
let bAlt = buf(vocabN)                   // the attestant's answer, for the every-run agreement gate
let bSlot = buf(vocabN)                  // gate 9b's third answer: the SLOT kernel's, held alongside
                                         // the split's and the lane's so all three are compared
                                         // against ONE attestant read, in one pass (Stone 13)
let bArgPI = dev.makeBuffer(length: max(ARGP,16) * 4, options: .storageModeShared)!  // stage-one indices
let bArgPV = buf(ARGP)                   // stage-one values — pooled once, like everything else
let bOutI = dev.makeBuffer(length: 16, options: .storageModeShared)!
let bOutV = buf(4)
let poolBytes = (dModel * 6 + dFF * 3 + vocabN + 2 * nLayer * maxpos * kvd + 2 * nHead * maxpos) * 4
print(String(format: "pooled: %.1f MB of activation + KV state, allocated ONCE for the whole run",
             Double(poolBytes) / 1048576.0))

// ---- encoding: one command buffer per token, one encoder, explicit barriers between ops ----------
// THE ENCODER IS CONCURRENT, AND THAT IS THE MEASUREMENT'S DOING. A .serial compute encoder runs
// dispatches one after another whether or not they depend on each other, and the per-shape profile
// showed why that is expensive here: a matvec's wall time tracks its COLUMN count almost exactly and
// barely notices its ROW count (3072 cols ~2.6 ms at 1024, 3072 AND 8192 rows; 8192 cols ~6.7 ms).
// Rows are parallelism the machine already has spare capacity for; the dispatch is latency-bound with
// the GPU mostly idle. So the three q/k/v projections — which all read the same normed vector and
// write disjoint buffers — have no reason to wait for each other, nor do gate and up, nor the two
// RoPEs. `barrier: false` marks exactly those, and NOTHING about the arithmetic changes: each row is
// still one thread folding right-to-left, bit for bit what the attestant folds.
// DISPATCHES PER TOKEN is the causal number this stone is measured by, so it is COUNTED rather than
// derived from the source by hand — a hand count of a loop body is exactly the kind of number that
// stays right until someone edits the loop.
var nDispatch = 0
// the acknowledgement axiom 5 offers, counted run-wide rather than per call site
var gpuErrors = 0
var gpuFirstError: String? = nil
final class Step {
    let cb: MTLCommandBuffer, enc: MTLComputeCommandEncoder
    init() { cb = queue.makeCommandBuffer()!; enc = cb.makeComputeCommandEncoder(dispatchType: .concurrent)! }
    func go(_ p: MTLComputePipelineState, _ width: Int, barrier: Bool = true, tgMul32: Bool = false,
            _ bind: (MTLComputeCommandEncoder) -> Void) {
        nDispatch += 1
        enc.setComputePipelineState(p); bind(enc)
        var tg = min(p.maxTotalThreadsPerThreadgroup, 256)
        // the lane kernel folds one row per SIMD GROUP, so a threadgroup that is not a whole number of
        // simdgroups would split a row across two of them and lose part of its sum. Rounded down here,
        // stated rather than assumed to be true of 256.
        if tgMul32 { tg = max(32, (tg / 32) * 32) }
        enc.dispatchThreads(MTLSize(width: width, height: 1, depth: 1),
                            threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
        if barrier { enc.memoryBarrier(scope: .buffers) }
    }
    // ONE threadgroup, exactly. The cooperative kernels reduce across a THREADGROUP, not across the
    // grid, so dispatchThreads (which the driver may split into as many threadgroups as it likes)
    // would silently give each group its own partial answer. dispatchThreadgroups(1, ...) is the
    // shape those kernels' stated radius asks for, and it is asked for here rather than assumed.
    func go1(_ p: MTLComputePipelineState, _ tg: Int, barrier: Bool = true,
             _ bind: (MTLComputeCommandEncoder) -> Void) {
        nDispatch += 1
        enc.setComputePipelineState(p); bind(enc)
        enc.dispatchThreadgroups(MTLSize(width: 1, height: 1, depth: 1),
                                 threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
        if barrier { enc.memoryBarrier(scope: .buffers) }
    }
    // AXIOM 4 (boundary) AND AXIOM 5 (offer), READ AND APPLIED. A command buffer's OFFERED
    // interface is `status` and `error`. `contents()` is not that interface — it is the memory
    // behind it, and reading it without consulting the offer is passage not through the offered
    // interface, which axiom 4 says is breach and promises is OBSERVABLE. It is observable here.
    //
    // And axiom 5 is why the breach is not merely impolite. It acknowledges with exactly one of
    // `nothing, 0, 1, node` — `nothing` is a STATE, distinct from `0`. Every readback buffer in
    // this carrier is freshly allocated and therefore zeroed, so on the wire `nothing` and `0`
    // are byte-identical, and a raw read COLLAPSES two states the axiom holds apart. That
    // collapse is what cost Stone 14 its MoE token: the GPU acknowledged `nothing`, the carrier
    // read `0`, and five gates reported an arithmetic disagreement for one residency fact.
    //
    // So the offer is consulted, run-wide, and nothing read back is trustworthy if any commit
    // failed. "The GPU is wrong" and "the GPU did not run" are not the same sentence.
    func done() {
        enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
        if let e = cb.error {
            gpuErrors += 1
            if gpuFirstError == nil { gpuFirstError = "\(e)" }
        }
        if cb.status != .completed {
            gpuErrors += 1
            if gpuFirstError == nil { gpuFirstError = "command buffer status \(cb.status.rawValue), not .completed" }
        }
    }
}
// a quantized matvec straight off the resident buffer: rows = d1, cols = d0 (GGUF's dim0 is the
// fastest-varying axis, so the matrix is d1 rows of d0 columns). The KERNEL is chosen by the
// tensor's own ggml type from the body's table — llama3.2:3b mixes Q6_K and Q4_K per layer.
// THE ATTESTANT: one thread per row, serial right-fold. Never deleted, never replaced — the split
// path answers to this, every run.
func matvecSerial(_ s: Step, _ t: TInfo, _ x: MTLBuffer, _ y: MTLBuffer, yOff: Int = 0, barrier: Bool = true) {
    var rows = UInt32(t.d1), cols = UInt32(t.d0)
    s.go(t.type == 14 ? pQ6M : pQ4M, t.d1, barrier: barrier) { e in
        e.setBuffer(modelBuf, offset: t.off, index: 0)
        e.setBuffer(x, offset: 0, index: 1)
        e.setBuffer(y, offset: yOff, index: 2)
        e.setBytes(&rows, length: 4, index: 3); e.setBytes(&cols, length: 4, index: 4)
    }
}
// THE SPLIT TWIN: rows*parts threads fold contiguous column chunks, then one thread per row folds the
// partials with p counting DOWN. At parts = 1 this is the attestant, bit for bit.
func matvecSplit(_ s: Step, _ t: TInfo, _ x: MTLBuffer, _ y: MTLBuffer, yOff: Int = 0,
                 barrier: Bool = true, parts: Int) {
    var rows = UInt32(t.d1), cols = UInt32(t.d0), np = UInt32(parts)
    s.go(t.type == 14 ? pQ6P : pQ4P, t.d1 * parts) { e in
        e.setBuffer(modelBuf, offset: t.off, index: 0)
        e.setBuffer(x, offset: 0, index: 1)
        e.setBuffer(bPart, offset: 0, index: 2)
        e.setBytes(&rows, length: 4, index: 3); e.setBytes(&cols, length: 4, index: 4)
        e.setBytes(&np, length: 4, index: 5)
    }
    s.go(pComb, t.d1, barrier: barrier) { e in
        e.setBuffer(bPart, offset: 0, index: 0)
        e.setBuffer(y, offset: yOff, index: 1)
        e.setBytes(&rows, length: 4, index: 2); e.setBytes(&np, length: 4, index: 3)
    }
}
// STONE 5, PIECE 1 — THE HOISTED SPLIT. Byte-for-byte the same partition, the same `part` buffer and
// the same combine as matvecSplit; the ONLY difference is that the superblock-invariant f16 decode is
// computed once per superblock crossing instead of once per weight. Same f32 value, same association,
// so at parts = 1 this is STILL the attestant bit for bit — which is exactly what gate 8b checks.
func matvecHoist(_ s: Step, _ t: TInfo, _ x: MTLBuffer, _ y: MTLBuffer, yOff: Int = 0,
                 barrier: Bool = true, parts: Int) {
    var rows = UInt32(t.d1), cols = UInt32(t.d0), np = UInt32(parts)
    s.go(t.type == 14 ? pQ6H : pQ4H, t.d1 * parts) { e in
        e.setBuffer(modelBuf, offset: t.off, index: 0)
        e.setBuffer(x, offset: 0, index: 1)
        e.setBuffer(bPart, offset: 0, index: 2)
        e.setBytes(&rows, length: 4, index: 3); e.setBytes(&cols, length: 4, index: 4)
        e.setBytes(&np, length: 4, index: 5)
    }
    s.go(pComb, t.d1, barrier: barrier) { e in
        e.setBuffer(bPart, offset: 0, index: 0)
        e.setBuffer(y, offset: yOff, index: 1)
        e.setBytes(&rows, length: 4, index: 2); e.setBytes(&np, length: 4, index: 3)
    }
}
// STONE 5, PIECE 2 — THE LANE KERNEL. One SIMD group per row, hoisted, reduced by simd_sum. ONE
// dispatch, and — the part that is not just about speed — NO SHARED SCRATCH. matvecSplit and
// matvecHoist both write the pooled `bPart`, so two of them can never overlap and every projection
// pays a barrier it does not need. The lane kernel writes only its own output, so the caller's
// `barrier:` flag is honoured again and q/k/v (and gate/up) go back to being concurrent.
func matvecLane(_ s: Step, _ t: TInfo, _ x: MTLBuffer, _ y: MTLBuffer, yOff: Int = 0, barrier: Bool = true) {
    var rows = UInt32(t.d1), cols = UInt32(t.d0)
    s.go(t.type == 14 ? pQ6L : pQ4L, t.d1 * 32, barrier: barrier, tgMul32: true) { e in
        e.setBuffer(modelBuf, offset: t.off, index: 0)
        e.setBuffer(x, offset: 0, index: 1)
        e.setBuffer(y, offset: yOff, index: 2)
        e.setBytes(&rows, length: 4, index: 3); e.setBytes(&cols, length: 4, index: 4)
    }
}
// STONE 13, THE SLOT KERNEL. Identical binding and identical grid to matvecLane — one SIMD group per
// row, no shared scratch, so the caller's `barrier:` flag is honoured exactly as there. What differs
// is inside: each lane owns a fixed 4-wide slot of every superblock instead of a stride-32 run of flat
// weight indices, so the field selectors are lane constants and one ql byte feeds 2 weights, one qh
// byte 4 and one scale byte 16.
//
// THE RADIUS IS ASKED, NOT ASSUMED. qk-matvec-slot.fk states that the map has NO tail arm: at a width
// that is not a whole multiple of 256 it is WRONG, not slow. So the width is CHECKED here and a
// tensor that fails it falls back to the lane kernel, which has a tail. Every llama3.2:3b width
// passes; the check exists so that a model whose widths do not will be right rather than plausible.
func matvecSlot(_ s: Step, _ t: TInfo, _ x: MTLBuffer, _ y: MTLBuffer, yOff: Int = 0, barrier: Bool = true) {
    guard t.d0 % 256 == 0 else {
        matvecLane(s, t, x, y, yOff: yOff, barrier: barrier); return
    }
    var rows = UInt32(t.d1), cols = UInt32(t.d0)
    s.go(t.type == 14 ? pQ6S : pQ4S, t.d1 * 32, barrier: barrier, tgMul32: true) { e in
        e.setBuffer(modelBuf, offset: t.off, index: 0)
        e.setBuffer(x, offset: 0, index: 1)
        e.setBuffer(y, offset: yOff, index: 2)
        e.setBytes(&rows, length: 4, index: 3); e.setBytes(&cols, length: 4, index: 4)
    }
}
enum MVPath: String { case serial, split, hoist, lane, slot }
var usePartsNow = 1
var mvNow: MVPath = .serial
func matvec(_ s: Step, _ t: TInfo, _ x: MTLBuffer, _ y: MTLBuffer, yOff: Int = 0, barrier: Bool = true) {
    switch mvNow {
    case .slot:
        matvecSlot(s, t, x, y, yOff: yOff, barrier: barrier)
    case .lane:
        matvecLane(s, t, x, y, yOff: yOff, barrier: barrier)
    case .hoist:
        // shares the pooled partials, so the barrier is not optional; the carrier says so, not races.
        matvecHoist(s, t, x, y, yOff: yOff, barrier: true, parts: usePartsNow)
    case .split:
        matvecSplit(s, t, x, y, yOff: yOff, barrier: true, parts: usePartsNow)
    case .serial:
        matvecSerial(s, t, x, y, yOff: yOff, barrier: barrier)
    }
}
func dequant(_ s: Step, _ t: TInfo, off: Int, n: Int, _ y: MTLBuffer) {
    var o = UInt32(off), nn = UInt32(n)
    s.go(t.type == 14 ? pQ6D : pQ4D, n) { e in
        e.setBuffer(modelBuf, offset: t.off, index: 0)
        e.setBuffer(y, offset: 0, index: 1)
        e.setBytes(&o, length: 4, index: 2); e.setBytes(&nn, length: 4, index: 3)
    }
}
// STONE 16. `fastOps` chooses the THREAD MAP and nothing else: the same buffers, the same constants,
// the same expressions. False is the attestant — one thread, 2n device round trips. True is the
// cooperative twin. The attestant path never sets it, so the every-run agreement gates compare two
// really different maps and not one map with itself.
var fastOps = false
func rmsnorm(_ s: Step, _ x: MTLBuffer, _ gain: TInfo, _ y: MTLBuffer) {
    var n = UInt32(dModel), eps = rmsEps
    let bind: (MTLComputeCommandEncoder) -> Void = { e in
        e.setBuffer(x, offset: 0, index: 0)
        e.setBuffer(modelBuf, offset: gain.off, index: 1)   // F32 gains, read in place
        e.setBuffer(y, offset: 0, index: 2)
        e.setBytes(&n, length: 4, index: 3); e.setBytes(&eps, length: 4, index: 4)
    }
    if fastOps { s.go1(pRmsT, rmsTG, bind) } else { s.go(pRms, 1, bind) }
}
let ropeT = T("rope_freqs.weight")
func rope(_ s: Step, _ v: MTLBuffer, off: Int, heads: Int, pos: Int, barrier: Bool = true) {
    var nh = UInt32(heads), hd = UInt32(headDim), p = UInt32(pos), b = ropeBase
    let bind: (MTLComputeCommandEncoder) -> Void = { e in
        e.setBuffer(v, offset: off, index: 0)
        e.setBuffer(modelBuf, offset: ropeT.off, index: 1)   // llama3.2's OWN frequency factors
        e.setBytes(&nh, length: 4, index: 2); e.setBytes(&hd, length: 4, index: 3)
        e.setBytes(&p, length: 4, index: 4); e.setBytes(&b, length: 4, index: 5)
    }
    // one thread per head, or one per (head, pair) — the SAME rotation either way
    if fastOps { s.go(pRopeP, heads * (headDim / 2), barrier: barrier, bind) }
    else       { s.go(pRope,  heads,                 barrier: barrier, bind) }
}
func elem(_ s: Step, _ p: MTLComputePipelineState, _ x: MTLBuffer, _ y: MTLBuffer, _ z: MTLBuffer, _ n: Int) {
    var nn = UInt32(n)
    s.go(p, n) { e in
        e.setBuffer(x, offset: 0, index: 0); e.setBuffer(y, offset: 0, index: 1)
        e.setBuffer(z, offset: 0, index: 2); e.setBytes(&nn, length: 4, index: 3)
    }
}
func fvals(_ b: MTLBuffer, _ n: Int) -> [Float] {
    let p = b.contents().bindMemory(to: Float.self, capacity: n)
    return (0..<n).map { p[$0] }
}

// ---- ONE forward pass at position `pos` for token `id` -> logits, then argmax --------------------
// The whole token is ONE command buffer: ~395 dispatches encoded back to back with an explicit
// buffer barrier between each, committed once. The weights are never touched by the host.
let embT = T("token_embd.weight"), onormT = T("output_norm.weight")
struct Layer { let an, q, k, v, o, fn, fg, fu, fd: TInfo }
let L: [Layer] = (0..<nLayer).map { l in
    Layer(an: T("blk.\(l).attn_norm.weight"), q: T("blk.\(l).attn_q.weight"),
          k: T("blk.\(l).attn_k.weight"), v: T("blk.\(l).attn_v.weight"),
          o: T("blk.\(l).attn_output.weight"), fn: T("blk.\(l).ffn_norm.weight"),
          fg: T("blk.\(l).ffn_gate.weight"), fu: T("blk.\(l).ffn_up.weight"),
          fd: T("blk.\(l).ffn_down.weight"))
}
func attn(_ s: Step, _ l: Int, _ pos: Int) {
    var npos = UInt32(pos + 1), nq = UInt32(nHead), nkv = UInt32(nKV)
    var hd = UInt32(headDim), sc = scale, sstride = UInt32(maxpos)
    s.go(pAttn, nHead) { e in
        e.setBuffer(bQ, offset: 0, index: 0)
        e.setBuffer(bCacheK, offset: l * maxpos * kvd * 4, index: 1)
        e.setBuffer(bCacheV, offset: l * maxpos * kvd * 4, index: 2)
        e.setBuffer(bAttn, offset: 0, index: 3); e.setBuffer(bScratch, offset: 0, index: 4)
        e.setBytes(&npos, length: 4, index: 5); e.setBytes(&nq, length: 4, index: 6)
        e.setBytes(&nkv, length: 4, index: 7); e.setBytes(&hd, length: 4, index: 8)
        e.setBytes(&sc, length: 4, index: 9); e.setBytes(&sstride, length: 4, index: 10)
    }
}
// PROFILE MODE (FORM_PROFILE=1) cuts a SEAM after each op: the command buffer is closed, waited on,
// and its wall time attributed to that op class. It is the SAME body — a separate profiled copy of
// forward() would be the one that silently drifts. The seams cost real time (one command buffer per
// op instead of one per token), and the harness prints the profiled total next to the unprofiled one
// so the overhead is visible rather than folded into a conclusion.
var profAcc: [String: Double] = [:], profN: [String: Int] = [:]
let profile = ProcessInfo.processInfo.environment["FORM_PROFILE"] == "1"
// ---- FORM_GEN_ONLY: the ANSWERING mode, and why it lives INSIDE the witness ----------------------
// The full suite generates the same prompt EIGHT times — once short and once long on each of the
// attestant, split, lane and slot paths — because its subject is that they AGREE. An ask's subject
// is the answer, and paying 8x (the attestant alone is 18x slower per token) to re-prove agreement
// on every question a person types is what makes a local model unusable rather than merely slow.
// FORM_GEN_ONLY=1 runs the slot path ONCE and reports it.
// It is a mode of THIS file and not a second runner, and that is the whole point: metal_ask.sh's own
// header names the risk — "a lean generation-only runner ... will have to prove it emits the same ids
// as this one before it is allowed to answer anything." A separate runner would have to prove that
// forever, against drift nobody watches. A flag around the CALLS to the SAME `generate`, over the
// same kernels, the same buffers and the same pool, has nothing left to drift: the agreement gates
// are what is skipped, never the code they were agreeing about. The verdict says so out loud, and
// metal_ask.sh stamps GATES with `-genonly` so no receipt can quote a lean run as the full suite.
let genOnly = ProcessInfo.processInfo.environment["FORM_GEN_ONLY"] == "1"
// ---- FORM_ABLATE: the seam-free instrument ------------------------------------------------------
// FORM_PROFILE cuts a command buffer after each op so it can attribute one, and the cut COSTS more
// than several of the ops it is measuring — which is how this stone was briefed with a mechanism that
// did not exist. This instrument never cuts. It re-dispatches ONE op class `ablDup` extra times,
// in stream, inside the same single command buffer, and the whole-token time difference divided by
// the extra count IS that op's real cost between its neighbours. The extra copies write where nothing
// reads them (or are idempotent), so the token ids are unchanged and the gates still judge the run.
let ablate = ProcessInfo.processInfo.environment["FORM_ABLATE"] ?? ""
let ablDup = Int(ProcessInfo.processInfo.environment["FORM_ABLATE_N"] ?? "4")!

// ==== STONE 43 (metal_uncertainty.sh, read-only extension): the logit vector's own decisiveness ====
// Spliced in by metal_uncertainty.sh. metal_first_token.sh itself is never modified. Everything below
// reads `bLogits` AFTER the token's single command buffer has completed, so the bytes are final.
let uncOn = ProcessInfo.processInfo.environment["FORM_UNC"] == "1"
var uncRecording = false
let uncK = 32                       // the truncation width for the cheap entropy
struct UncStat {
    let id: Int, ties: Int
    let l1: Double, l2: Double, margin: Double, p1: Double, ent: Double, entK: Double, sd: Double
    let alive: Bool
}
var uncStats: [UncStat] = []
var uncSeconds = 0.0
// PRICED SEPARATELY, because the stone has to CHOOSE one signal and "cheap matters: this runs per
// token" is a claim about seconds. tScan is the max/second-max/sd pass every signal needs. tExp is
// the full-vocabulary logsumexp pass that p1 and the full entropy need (128 256 exp() calls). tTopK
// is the selection pass plus 32 exps. A signal's real price is tScan plus its own column.
var uncTScan = 0.0, uncTExp = 0.0, uncTTopK = 0.0
func uncMeasure() {
    let t0 = Date()
    let p = bLogits.contents().bindMemory(to: Float.self, capacity: vocabN)
    var m = -Double.infinity, m2 = -Double.infinity
    var iMax = -1, ties = 0, nonFinite = 0
    var sum = 0.0, sumsq = 0.0
    for i in 0..<vocabN {
        let v = Double(p[i])
        if !v.isFinite { nonFinite += 1; continue }
        sum += v; sumsq += v * v
        if v > m { m2 = m; m = v; iMax = i; ties = 1 }
        else if v == m { ties += 1 }
        else if v > m2 { m2 = v }
    }
    let n = Double(vocabN)
    let mean = sum / n
    let sd = max(0.0, sumsq / n - mean * mean).squareRoot()
    let tA = Date(); uncTScan += tA.timeIntervalSince(t0)
    // one more pass: the softmax denominator and E_p[l], both shifted by the max exactly as
    // form-stdlib/transformer-numerics.fk's tn-softmax shifts it.
    var Z = 0.0, wsum = 0.0
    for i in 0..<vocabN {
        let v = Double(p[i]); if !v.isFinite { continue }
        let e = exp(v - m); Z += e; wsum += e * v
    }
    let lse = m + log(Z)
    let p1 = exp(m - lse)
    let ent = lse - wsum / Z                     // H = logsumexp - E_p[l], in nats
    let tB = Date(); uncTExp += tB.timeIntervalSince(tA)
    // the top-k truncation, renormalized over the k kept
    var top = [Double](repeating: -Double.infinity, count: uncK)
    for i in 0..<vocabN {
        let v = Double(p[i]); if !v.isFinite { continue }
        if v > top[uncK - 1] {
            var j = uncK - 1
            while j > 0 && top[j - 1] < v { top[j] = top[j - 1]; j -= 1 }
            top[j] = v
        }
    }
    var Zk = 0.0, wk = 0.0
    for v in top { let e = exp(v - m); Zk += e; wk += e * v }
    let entK = (m + log(Zk)) - wk / Zk
    uncTTopK += Date().timeIntervalSince(tB)
    let chosen = Int(bOutI.contents().bindMemory(to: UInt32.self, capacity: 1)[0])
    // THE DEAD-FORWARD GUARD. A zeroed pool, an unrun dispatch and a NaN blowout all produce a vector
    // an unguarded margin/probability would happily score. None of them may read as decided.
    let alive = (nonFinite == 0) && (ties == 1) && (sd > 1e-6) && (m > m2) && (chosen == iMax)
    uncStats.append(UncStat(id: chosen, ties: ties, l1: m, l2: m2, margin: m - m2,
                            p1: p1, ent: ent, entK: entK, sd: sd, alive: alive))
    uncSeconds += Date().timeIntervalSince(t0)
}
func uncReport(_ nPrompt: Int) {
    print("=== UNCERTAINTY — the logit vector, per forward (STONE 43) ===")
    print("  UNCPROMPT \(nPrompt) prompt forwards; step < \(nPrompt) is prefill, and step \(nPrompt - 1) is the FIRST CONTENT TOKEN's decision")
    print("  UNCVOCAB \(vocabN) ln(V)=\(String(format: "%.6f", log(Double(vocabN)))) topk=\(uncK)")
    print("  UNCHEAD step id margin p1 entropy entropy_topk top1 top2 sd ties alive text")
    for (i, s) in uncStats.enumerated() {
        print(String(format: "  UNC %d %d %.6f %.6f %.6f %.6f %.6f %.6f %.6f %d %d",
                     i, s.id, s.margin, s.p1, s.ent, s.entK, s.l1, s.l2, s.sd, s.ties, s.alive ? 1 : 0)
              + " [" + decodeIds([s.id]) + "]")
    }
    let n = max(1, uncStats.count)
    print(String(format: "  UNCCOST %.6f s of host CPU across %d measurements = %.3f ms each — the instrument's own price, charged (row 858)",
                 uncSeconds, uncStats.count, 1000.0 * uncSeconds / Double(n)))
    print(String(format: "  UNCCOSTSPLIT per step, ms: scan(max,2nd,sd) %.3f | full-vocab exp pass (p1, entropy) %.3f | top-%d select+exp (entropy_topk) %.3f",
                 1000.0 * uncTScan / Double(n), 1000.0 * uncTExp / Double(n), uncK,
                 1000.0 * uncTTopK / Double(n)))
    print(String(format: "  UNCPRICE per step, ms: margin %.3f | p1 %.3f | entropy %.3f | entropy_topk %.3f  (each = scan + its own pass)",
                 1000.0 * uncTScan / Double(n), 1000.0 * (uncTScan + uncTExp) / Double(n),
                 1000.0 * (uncTScan + uncTExp) / Double(n),
                 1000.0 * (uncTScan + uncTTopK) / Double(n)))
}
// ==== end STONE 43 extension ====

func forward(_ id: Int, _ pos: Int) -> Int {
    var s = Step()
    var t0 = Date()
    func seam(_ rawTag: String) {
        guard profile else { return }
        // TAGGED BY OP SET. Since Stone 16 the same op name means two different thread maps, and one
        // bucket holding both would report their average and call it a measurement.
        let tag = (fastOps ? "T " : "A ") + rawTag
        s.done()
        profAcc[tag, default: 0] += Date().timeIntervalSince(t0); profN[tag, default: 0] += 1
        s = Step(); t0 = Date()
    }
    func mv(_ t: TInfo, _ x: MTLBuffer, _ y: MTLBuffer, _ off: Int = 0, _ barrier: Bool = true) {
        matvec(s, t, x, y, yOff: off, barrier: barrier)
        seam((t.type == 14 ? "mv Q6_K " : "mv Q4_K ") + "\(t.d1)x\(t.d0)")
    }
    func dup(_ tag: String, _ f: () -> Void) { if ablate == tag { for _ in 0..<ablDup { f() } } }
    // FORM_ABLATE=seam — the falsifier this stone owes itself (row `snugcause`). Every other ablation
    // prices an op's WORK plus its seam together. This one inserts dispatches with essentially no
    // work at all — a 1-element add, barriered exactly like every real op — so the delta over N is
    // the seam ALONE, measured in the real harness rather than in a synthetic loop. If the seam were
    // the 113 us the stone was briefed with, this one knob would dominate every other ablation.
    dequant(s, embT, off: id * dModel, n: dModel, bX); seam("embed gather")
    for l in 0..<nLayer {
        let kOff = (l * maxpos + pos) * kvd * 4
        rmsnorm(s, bX, L[l].an, bXb); seam("rmsnorm")
        dup("rmsnorm") { rmsnorm(s, bX, L[l].an, bXbT) }      // idempotent shape, scratch destination
        mv(L[l].q, bXb, bQ, 0, false)                  // q, k and v read the same vector and
        mv(L[l].k, bXb, bCacheK, kOff, false)          // write disjoint buffers: no dependency,
        mv(L[l].v, bXb, bCacheV, kOff)                 // so no barrier until all three are in
        rope(s, bQ, off: 0, heads: nHead, pos: pos, barrier: false)
        rope(s, bCacheK, off: kOff, heads: nKV, pos: pos); seam("rope")
        dup("rope") { rope(s, bAlt, off: 0, heads: nHead, pos: pos) }   // same shape, scratch vector
        attn(s, l, pos); seam("attention")
        dup("attention") { attn(s, l, pos) }                  // idempotent: same inputs, same output
        mv(L[l].o, bAttn, bProj)
        elem(s, pAdd, bX, bProj, bX, dModel); seam("residual add")
        dup("residual add") { elem(s, pAdd, bX, bProj, bAlt, dModel) }  // scratch destination
        rmsnorm(s, bX, L[l].fn, bXb); seam("rmsnorm")
        mv(L[l].fg, bXb, bGate, 0, false)              // gate and up, likewise independent
        mv(L[l].fu, bXb, bUp)
        dup("ffnup") { mv(L[l].fu, bXb, bAlt) }  // re-READS the ~17 MB up weight; scratch destination
        elem(s, pSwi, bGate, bUp, bAct, dFF); seam("swiglu")
        dup("swiglu") { elem(s, pSwi, bGate, bUp, bAlt, dFF) }         // scratch destination
        dup("seam") { elem(s, pAdd, bX, bProj, bAlt, 1) }              // ~no work, one full seam
        mv(L[l].fd, bAct, bFfn)
        elem(s, pAdd, bX, bFfn, bX, dModel); seam("residual add")
    }
    rmsnorm(s, bX, onormT, bXb); seam("rmsnorm")
    mv(embT, bXb, bLogits)                 // tied: the unembedding IS token_embd
    var nn = UInt32(vocabN), np = UInt32(ARGP)
    let nArg = 1 + (ablate == "argmax" ? ablDup : 0)   // idempotent: same logits, same answer
    for _ in 0..<nArg {
    if fastOps {
        // stage one: ARGP contiguous chunks, each scanned with the attestant's strict `>`
        s.go(pArgP, ARGP) { e in
            e.setBuffer(bLogits, offset: 0, index: 0); e.setBuffer(bArgPI, offset: 0, index: 1)
            e.setBuffer(bArgPV, offset: 0, index: 2)
            e.setBytes(&nn, length: 4, index: 3); e.setBytes(&np, length: 4, index: 4)
        }
        // stage two: the partials folded in ASCENDING part order, same strict `>`, one threadgroup
        s.go1(pArgC, argTG) { e in
            e.setBuffer(bArgPI, offset: 0, index: 0); e.setBuffer(bArgPV, offset: 0, index: 1)
            e.setBuffer(bOutI, offset: 0, index: 2); e.setBuffer(bOutV, offset: 0, index: 3)
            e.setBytes(&np, length: 4, index: 4)
        }
    } else {
        s.go(pArg, 1) { e in
            e.setBuffer(bLogits, offset: 0, index: 0); e.setBuffer(bOutI, offset: 0, index: 1)
            e.setBuffer(bOutV, offset: 0, index: 2); e.setBytes(&nn, length: 4, index: 3)
        }
    }
    }
    s.done()
    if profile { let tg = (fastOps ? "T " : "A ") + "argmax"
                 profAcc[tg, default: 0] += Date().timeIntervalSince(t0); profN[tg, default: 0] += 1 }
    if uncRecording { uncMeasure() }
    return Int(bOutI.contents().bindMemory(to: UInt32.self, capacity: 1)[0])
}

// ---- gates 2-4: three points on the token's OWN path, judged by the body -------------------------
let u: Double = 5.960464477539063e-08      // 2^-24, the f32 unit roundoff
var gate2 = false, gate3 = false, gate4 = false
var twinRms: [Float] = [], attestRms: [Float] = []
// The gate pass re-walks the FIRST THREE OPS of a forward pass with a barrier after each, so each op
// can be read back and judged on its own. It is deliberately NOT `forward` with hooks: a timing path
// and a gating path that share a body drift, and the one that drifts is always the one nobody ran.
func gatePass() {
    let s1 = Step(); dequant(s1, embT, off: refId * dModel, n: dModel, bX); s1.done(); gateEmb()
    let s2 = Step(); rmsnorm(s2, bX, L[0].an, bXb); s2.done()
    // STONE 16: the SAME point, re-walked by the cooperative twin, so gate 3 judges the attestant
    // against Form AND the twin against the attestant in one place. A twin proven only by the token
    // ids it happens to produce is proven by a 12-sample; this is proven on all 3072 outputs.
    let attRms = fvals(bXb, dModel)
    fastOps = true
    let s2b = Step(); rmsnorm(s2b, bX, L[0].an, bXbT); s2b.done()
    fastOps = false
    twinRms = fvals(bXbT, dModel); attestRms = attRms
    gateRms()
    let s3 = Step(); matvec(s3, L[0].q, bXb, bQ); s3.done(); gateQ()
}
func gateEmb() {
    let got = fvals(bX, dModel)
    var bad = 0, worst = 0.0
    for j in 0..<dModel where Double(got[j]) != Double(Float(refEmb[j])) {
        bad += 1; worst = max(worst, abs(Double(got[j]) - refEmb[j]))
    }
    gate2 = (bad == 0)
    check(gate2,
      "gate 2 embedding gather: all \(dModel) weights of token_embd row \(refId) BIT-EXACT vs Form (Q6_K, one rounding each side)",
      "gate 2 embedding gather: \(bad)/\(dModel) weights differ from Form, worst |d| = \(worst)")
}
func gateRms() {
    let got = fvals(bXb, dModel)
    var sumsq = 0.0
    for v in refEmb { sumsq += v * v }
    // the stated bound: the f32 sum-of-squares over n terms carries at most n*u of relative error,
    // and the Newton-50 sqrt and the two multiplies add O(u). Report the measured FRACTION of it.
    let bound = Double(dModel) * u
    var worstRel = 0.0
    for j in 0..<dModel {
        let r = refRms[j]
        if abs(r) > 1e-30 { worstRel = max(worstRel, abs(Double(got[j]) - r) / abs(r)) }
    }
    var twinDiff = 0
    for j in 0..<dModel where twinRms[j].bitPattern != attestRms[j].bitPattern { twinDiff += 1 }
    gate3 = worstRel <= bound && twinDiff == 0
    print(String(format: "  rmsnorm worst relative deviation %.3e vs derived bound n*u = %.3e (%.1f%% of it)",
                 worstRel, bound, 100 * worstRel / bound))
    print("  cooperative twin vs the one-thread attestant: \(dModel - twinDiff)/\(dModel) outputs BIT-IDENTICAL")
    check(gate3, "gate 3 RMSNorm on the GPU tracks Form's fp64 inside the derived n*u bound, AND the cooperative twin is the one-thread attestant BIT FOR BIT on all \(dModel) outputs",
                 twinDiff == 0 ? "gate 3 RMSNorm exceeds the derived n*u bound"
                               : "gate 3 the cooperative RMSNorm twin differs from the attestant on \(twinDiff)/\(dModel) outputs")
}
func gateQ() {
    let got = Double(fvals(bQ, dModel)[refRow])
    // the DERIVED bound, the same shape Stone 3 used: an f32 right-fold of `cols` products carries at
    // most cols*u*SUM|term|. SUM|term| is computed here from the GPU's own dequant of that row and
    // the body's own normed vector, so nothing about the bound is guessed.
    let s = Step(); dequant(s, T("blk.0.attn_q.weight"), off: refRow * dModel, n: dModel, bGate); s.done()
    let w = fvals(bGate, dModel)
    var sumAbs = 0.0
    for j in 0..<dModel { sumAbs += abs(Double(w[j]) * refRms[j]) }
    let bound = Double(dModel) * u * sumAbs
    let d = abs(got - refQ)
    gate4 = d <= bound
    print(String(format: "  q[%d] GPU %.9g  Form(fp64) %.9g  |d| %.3e  bound cols*u*SUM|term| %.3e (%.1f%% of it)",
                 refRow, got, refQ, d, bound, 100 * d / bound))
    check(gate4, "gate 4 a real Q4_K fused matvec at width \(dModel) is the body's answer",
                 "gate 4 the Q4_K fused matvec left the derived bound")
}

// ---- gates 8-9: the split twin answers to the attestant --------------------------------------------
// Gate 8 is structural and needs no epsilon at all: at parts = 1 the split kernels must reproduce the
// attestant BIT FOR BIT on every row, because one chunk folded downward and added to nothing IS the
// attestant. Gate 9 is the named epsilon: at the parts the run actually uses, every row must stay
// inside (cols + ceil(cols/parts) + parts) * u * SUM|term|, derived in qk-matvec-split.fk.
var gate8 = false, gate9 = false, gate8b = false, gate9b = false
func splitGates() {
    let t = L[0].fd                       // a real Q6_K 3072 x 8192 — the deepest row in the model
    let rows = t.d1, cols = t.d0
    // x is real activation state, not a synthetic vector: whatever bXb holds after the gate pass.
    let s0 = Step(); matvecSerial(s0, t, bAct, bAlt); s0.done()
    let ref = fvals(bAlt, rows)
    let s1 = Step(); matvecSplit(s1, t, bAct, bLogits, parts: 1); s1.done()
    let one = fvals(bLogits, rows)
    var bad1 = 0
    for r in 0..<rows where one[r] != ref[r] { bad1 += 1 }
    gate8 = (bad1 == 0)
    check(gate8, "gate 8 the split kernel at parts=1 IS the attestant, bit for bit, on all \(rows) rows of \(t.d1)x\(t.d0)",
                 "gate 8 the split kernel at parts=1 differs from the attestant on \(bad1)/\(rows) rows")
    // GATE 8b — STONE 5. The HOIST claims to be free: computing the superblock's f16 super-scale once
    // per crossing instead of once per weight yields the identical f32 and touches no association.
    // "Claims" is the operative word, so it is checked the same structural way and with no epsilon:
    // at parts = 1 the hoisted kernel must ALSO be the attestant, bit for bit, on every row.
    let s1h = Step(); matvecHoist(s1h, t, bAct, bLogits, parts: 1); s1h.done()
    let oneH = fvals(bLogits, rows)
    var bad1h = 0
    for r in 0..<rows where oneH[r] != ref[r] { bad1h += 1 }
    gate8b = (bad1h == 0)
    check(gate8b, "gate 8b the HOISTED kernel at parts=1 is ALSO the attestant, bit for bit — the hoist costs no accuracy at all",
                  "gate 8b the hoisted kernel at parts=1 differs from the attestant on \(bad1h)/\(rows) rows")
    guard PARTS > 1 else { gate9 = true; gate9b = true; print("  (parts = 1: no reassociation, no epsilon to name)"); return }
    let s2 = Step(); matvecSplit(s2, t, bAct, bLogits, parts: PARTS); s2.done()
    let many = fvals(bLogits, rows)
    let s2l = Step(); matvecLane(s2l, t, bAct, bAlt); s2l.done()
    let lane = fvals(bAlt, rows)
    // STONE 13. The SLOT kernel answers to the SAME derived bound, and it is folded into gate 9b
    // rather than given a gate of its own: 9b's claim is "the fast twins stay inside the epsilon the
    // body derived", and there are now two of them. One gate, two kernels, and it falls if EITHER
    // leaves the bound — a separate gate would let one pass while the other quietly did not.
    let s2s = Step(); matvecSlot(s2s, t, bAct, bSlot); s2s.done()
    let slot = fvals(bSlot, rows)
    // SUM|term| per row, from the GPU's own dequant of that row and the actual x.
    let xs = fvals(bAct, cols)
    var worstFrac = 0.0, worstAbs = 0.0, out = 0
    var lWorstFrac = 0.0, lWorstAbs = 0.0, lOut = 0
    var sWorstFrac = 0.0, sWorstAbs = 0.0, sOut = 0
    let chunk = (cols + PARTS - 1) / PARTS
    let coeff = Double(cols + chunk + PARTS)
    // THE LANE KERNEL'S COEFFICIENT, and it is the SAME formula with parts = 32 — no new derivation.
    // Its tree is a lane chain of depth ceil(cols/32) followed by whatever tree metal::simd_sum uses
    // over 32 values. Metal does not specify that tree and does not have to: ANY association of 32
    // terms has depth at most 31 < 32, so parts = 32 bounds every tree simd_sum could be using.
    let lChunk = (cols + 32 - 1) / 32
    let lCoeff = Double(cols + lChunk + 32)
    let probe = min(rows, 64)             // 64 rows dequantized and bounded exactly; the rest by the
                                          // same coefficient against the max SUM|term| seen (stated)
    var maxSumAbs = 0.0
    for r in 0..<probe {
        let sd = Step(); dequant(sd, t, off: r * cols, n: cols, bGate); sd.done()
        let w = fvals(bGate, cols)
        var sa = 0.0
        for j in 0..<cols { sa += abs(Double(w[j]) * Double(xs[j])) }
        maxSumAbs = max(maxSumAbs, sa)
        let bound = coeff * u * sa
        let d = abs(Double(many[r]) - Double(ref[r]))
        if bound > 0 { worstFrac = max(worstFrac, d / bound) }
        worstAbs = max(worstAbs, d)
        if d > bound { out += 1 }
        let lBound = lCoeff * u * sa
        let ld = abs(Double(lane[r]) - Double(ref[r]))
        if lBound > 0 { lWorstFrac = max(lWorstFrac, ld / lBound) }
        lWorstAbs = max(lWorstAbs, ld)
        if ld > lBound { lOut += 1 }
        // the slot kernel is bounded by the SAME coefficient — see qk-matvec-slot.fk's epsilon block:
        // its tree is shallower than the lane's and its extra roundings cost less than the per-term u
        // the depth argument already charges.
        let sd2 = abs(Double(slot[r]) - Double(ref[r]))
        if lBound > 0 { sWorstFrac = max(sWorstFrac, sd2 / lBound) }
        sWorstAbs = max(sWorstAbs, sd2)
        if sd2 > lBound { sOut += 1 }
    }
    let looseBound = coeff * u * maxSumAbs
    for r in probe..<rows where abs(Double(many[r]) - Double(ref[r])) > looseBound { out += 1 }
    let lLooseBound = lCoeff * u * maxSumAbs
    for r in probe..<rows where abs(Double(lane[r]) - Double(ref[r])) > lLooseBound { lOut += 1 }
    for r in probe..<rows where abs(Double(slot[r]) - Double(ref[r])) > lLooseBound { sOut += 1 }
    gate9 = (out == 0)
    print(String(format: "  named epsilon: |split - attestant| <= (cols + ceil(cols/parts) + parts)*u*SUM|term|"))
    print(String(format: "    cols %d  parts %d  chunk %d  coeff %.0f  worst |d| %.3e  worst fraction of bound %.4f",
                 cols, PARTS, chunk, coeff, worstAbs, worstFrac))
    check(gate9, "gate 9 at parts=\(PARTS) every row stays inside the DERIVED bound (worst \(String(format: "%.2f", 100*worstFrac))% of it)",
                 "gate 9 \(out) rows left the derived bound")
    gate9b = (lOut == 0 && sOut == 0)
    print(String(format: "    LANE (simd_sum, 32 lanes): chunk %d  coeff %.0f  worst |d| %.3e  worst fraction of bound %.4f",
                 lChunk, lCoeff, lWorstAbs, lWorstFrac))
    print(String(format: "    SLOT (simd_sum, 4-wide slot): same coeff %.0f  worst |d| %.3e  worst fraction of bound %.4f",
                 lCoeff, sWorstAbs, sWorstFrac))
    check(gate9b, "gate 9b the LANE and SLOT kernels BOTH stay inside the SAME derived bound at parts=32 (worst \(String(format: "%.2f", 100*max(lWorstFrac, sWorstFrac)))% of it)",
                  "gate 9b \(lOut) row(s) left the bound on the lane kernel and \(sOut) on the slot kernel")
}

// ---- the run -------------------------------------------------------------------------------------
func zeroPool() {
    memset(bCacheK.contents(), 0, bCacheK.length); memset(bCacheV.contents(), 0, bCacheV.length)
    memset(bX.contents(), 0, bX.length)
}
// ENCODE IS TIMED because an ASK's latency starts at the person's sentence, not at the first
// matvec. It is microseconds here and that is exactly why it is worth printing: a latency budget
// with an untimed term in it invites the reader to imagine the term is large.
let encT0 = Date()
let promptIds = encode(prompt)
let encodeS = Date().timeIntervalSince(encT0)
print("=== the prompt ===")
print("  \"\(prompt)\"")
print("  ids: \(promptIds)")
print(String(format: "  encode %.6f s for %d prompt tokens", encodeS, promptIds.count))
print("  pieces: " + promptIds.map { "[" + decodeIds([$0]) + "]" }.joined())
if promptIds.count + nsteps >= maxpos { print("FAIL  prompt+steps exceeds the KV pool"); exit(1) }

// ---- gate 0: DID THE GPU RUN. Asked FIRST, because every gate below reads a buffer back, and a
// buffer that was never written reads as zeros — which is a NUMBER, and numbers get compared and
// reported as disagreement (axiom 5: `nothing` is not `0`). The CPU writes a sentinel no kernel
// would ever produce and a kernel must overwrite it. If it survives, the gates below are REFUSED
// rather than graded against unwritten memory.
print("=== gate 0: did the GPU run at all ===")
do {
    let sentinel: Float = -424242.0
    let sp = bXb.contents().bindMemory(to: Float.self, capacity: 4)
    for i in 0..<4 { sp[i] = sentinel }
    let before = gpuErrors
    let s = Step()
    dequant(s, embT, off: 0, n: 4, bXb)      // the cheapest real kernel in the unit
    s.done()
    let after = fvals(bXb, 4)
    let untouched = after.allSatisfy { $0 == sentinel }
    if gpuErrors > before { print("  command buffer ERROR: \(gpuFirstError ?? "unknown")") }
    print("  sentinel \(sentinel) -> \(after)")
    check(gpuErrors == before && !untouched,
          "gate 0 the GPU executes: a kernel overwrote the CPU's sentinel, and no command buffer errored",
          "gate 0 THE GPU DID NOT RUN — the sentinel survived and/or a command buffer failed. Every gate below would grade an unwritten buffer as an arithmetic disagreement; they are not run.")
    if failures > 0 {
        print("")
        print("  This is a RESIDENCY condition, not an arithmetic one. This carrier asks Metal to make")
        print("  \(String(format: "%.2f", Double(dev.currentAllocatedSize)/1073741824.0)) GiB resident; whether that succeeds depends on what else holds memory on this")
        print("  machine RIGHT NOW — it is not a property of the code or of the model.")
        print("=== VERDICT FAIL — refusing to grade unwritten memory ===")
        exit(1)
    }
}

print("=== gates 2-4: three points on the token's own path ===")
zeroPool()
gatePass()
print("=== gates 8-9: the split twin answers to the attestant ===")
splitGates()
zeroPool()

struct Run { var out: [Int] = []; var prefill = 0.0; var decode = 0.0; var forwards = 0; var disp = 0 }
func generate(_ ids: [Int], _ steps: Int) -> Run {
    zeroPool()
    var r = Run(); var pos = 0; var cur = 0
    let t0 = Date()
    for id in ids { cur = forward(id, pos); pos += 1 }     // prefill: the last logit IS token 1
    let t1 = Date()
    let d0 = nDispatch
    for _ in 0..<steps {
        r.out.append(cur)
        if cur == eosId || cur == 128001 { break }
        cur = forward(cur, pos); pos += 1; r.forwards += 1
    }
    let t2 = Date()
    r.disp = (nDispatch - d0) / max(1, r.forwards)
    r.prefill = t1.timeIntervalSince(t0); r.decode = t2.timeIntervalSince(t1)
    return r
}

if failures > 0 { print("=== \(failures) gate(s) failed BEFORE any token — refusing to report a rate ==="); exit(1) }
if gpuErrors > 0 { print("=== \(gpuErrors) command buffer(s) failed (\(gpuFirstError ?? "?")) — nothing read back is trustworthy, refusing to report a rate ==="); exit(1) }

// ---- FORM_GEN_ONLY: generate once, on the slot path, and stop ------------------------------------
// The state is set to EXACTLY what the full suite has in hand when it reaches its slot section:
// usePartsNow = 1, fastOps = true, mvNow = .slot. Anything else here would be a different run
// wearing the slot path's name.
if genOnly {
    print("=== FORM_GEN_ONLY: the slot path, once. The cross-path agreement gates were NOT run. ===")
    usePartsNow = 1; fastOps = true; mvNow = .slot
    uncRecording = uncOn
    var g = generate(promptIds, nsteps)
    // THE ANSWER ENDED, OR THE HARNESS DID, and those are different sentences. `generate` appends the
    // token and THEN breaks on it, so an end-of-turn id is the last element when the model stopped —
    // and it is a control token, not prose, so it is trimmed off the text a person reads and reported
    // separately. When no end token is present the answer did not finish; the receipt says `cap` and
    // does not dress a truncation up as a completion.
    let last = g.out.last ?? -1
    let hitEos = (last == eosId || last == 128001 || last == 128009)
    if hitEos { g.out.removeLast() }
    report("slot-long ", g)
    // PROSE DOES NOT FIT ON A LINE. `report` prints the text inside quotes on one line, which is
    // right for a twelve-token fragment and wrong for an answer — the first newline the model emits
    // makes the line unparseable, and the carrier that reads it reports "the harness's format
    // moved" rather than an answer. A delimited block carries whatever the model wrote.
    print("ANSWER-TEXT-BEGIN")
    print(decodeIds(g.out))
    print("ANSWER-TEXT-END")
    print("  STOP reason=\(hitEos ? "eos" : "cap") stop_id=\(hitEos ? last : -1) eos_id=\(eosId) generated=\(g.out.count) cap=\(nsteps)")
    print(String(format: "  LATENCY encode %.6f s + prefill %.3f s (%d prompt tokens) + decode %.3f s (%d forwards) = %.3f s in-process",
                 encodeS, g.prefill, promptIds.count, g.decode, g.forwards, encodeS + g.prefill + g.decode))
    if uncOn { uncReport(promptIds.count) }
    if gpuErrors > 0 {
        print("=== VERDICT FAIL — \(gpuErrors) command buffer(s) failed (\(gpuFirstError ?? "?")) ==="); exit(1)
    }
    // The count is what was ACTUALLY asked in this mode, counted from the `check` calls that ran:
    // gate 1 (config), gate 0 (the GPU ran), gates 2/3/4 (embedding, RMSNorm, matvec against the
    // body's own fp64) and gates 8/8b/9/9b (the split, hoist, lane and slot kernels against the
    // attestant, bit for bit or inside the derived bound) = 9. What is NOT asked is gates 5, 6, 7,
    // 10 and 11 — reproducibility, input-dependence, the slope, and cross-path token agreement —
    // because every one of them costs a second full generation of the prompt.
    print(failures == 0 ? "=== VERDICT PASS — 9 gates (FORM_GEN_ONLY; gates 5, 6, 7, 10, 11 not run) ==="
                        : "=== VERDICT FAIL — \(failures) gate(s) ===")
    exit(failures == 0 ? 0 : 1)
}

print("=== gate 6: a token ===")
let short = max(2, nsteps / 3)
usePartsNow = 1; mvNow = .serial
let rShort = generate(promptIds, short)
let rLong  = generate(promptIds, nsteps)

func report(_ label: String, _ r: Run) {
    print("  \(label): prefill \(String(format: "%.3f", r.prefill)) s for \(promptIds.count) prompt tokens; decode \(String(format: "%.3f", r.decode)) s for \(r.forwards) further forwards")
    print("    ids  : \(r.out)")
    print("    text : \"\(decodeIds(r.out))\"")
    // END-TO-END means what it says: generated tokens divided by ALL the wall clock it took to have
    // them, prefill included. The decode-only number is reported next to it and labelled, never
    // instead of it.
    print(String(format: "    END-TO-END %.3f tok/s over %d generated tokens (prefill+decode %.3f s)  |  decode-only %.3f tok/s",
                 Double(r.out.count) / (r.prefill + r.decode), r.out.count, r.prefill + r.decode,
                 Double(max(1, r.forwards)) / r.decode))
}
report("short", rShort)
report("long ", rLong)

// ---- gate 7: two sizes and a slope ---------------------------------------------------------------
let nA = Double(rShort.forwards), nB = Double(rLong.forwards)
let slope = (rLong.decode - rShort.decode) / max(1.0, nB - nA)
print("=== gate 7: two sizes and a slope ===")
print(String(format: "  decode %.3f s at %d forwards and %.3f s at %d forwards -> %.4f s per additional token (%.3f tok/s marginal)",
             rShort.decode, Int(nA), rLong.decode, Int(nB), slope, 1.0 / max(1e-9, slope)))
check(nB > nA, "gate 7 the rate was measured at two generation lengths, not one",
               "gate 7 only one size was measured")

if profile {
    print("=== where the time goes (FORM_PROFILE=1: one command buffer PER OP, seams included) ===")
    let tot = profAcc.values.reduce(0, +)
    for (k, v) in profAcc.sorted(by: { $0.value > $1.value }) {
        print(String(format: "  %-20@ %8.3f s  %6.2f%%  over %5d dispatches (%.3f ms each)",
                     k as NSString, v, 100 * v / tot, profN[k]!, 1000 * v / Double(profN[k]!)))
    }
    print(String(format: "  TOTAL          %8.3f s   (A = the attestant's ops, T = Stone 16's twins)", tot))
}

// ---- gates 10-11: the FAST paths generate the ATTESTANT's tokens ------------------------------------
// AN EXTERNAL DENOMINATOR. Every ratio this program has ever reported was against its own attestant —
// true, and silent about the world (corpus row 834, selfgauge). An ollama/llama.cpp oracle measured on
// THIS machine, THIS model and THIS 2.0 GB blob over a 150-token sample is carried here so that no
// speedup below can be read without its absolute cost. It is a MEASUREMENT MADE ELSEWHERE, quoted, not
// re-run by this harness — labelled so, and never mixed into a gate.
let ollamaDecode = 157.83, ollamaPrefill = 640.94
func vsWorld(_ label: String, _ r: Run) {
    let e2e = Double(r.out.count) / (r.prefill + r.decode)
    let dec = Double(max(1, r.forwards)) / r.decode
    let pre = Double(promptIds.count) / r.prefill
    print(String(format: "    vs the world (ollama on this machine, quoted): decode %.3f of %.2f tok/s (%.1fx behind)  |  prefill %.3f of %.2f tok/s (%.1fx behind)  |  end-to-end %.3f tok/s",
                 dec, ollamaDecode, ollamaDecode / dec, pre, ollamaPrefill, ollamaPrefill / pre, e2e))
    _ = label
}
var fLong: Run? = nil
if PARTS > 1 {
    print("=== gate 10: the split path's tokens ===")
    usePartsNow = PARTS; mvNow = .split
    let fS = generate(promptIds, short), fL = generate(promptIds, nsteps)
    report("split-short", fS); report("split-long ", fL); vsWorld("split", fL)
    check(fL.out == rLong.out,
      "gate 10 the split path at parts=\(PARTS) generates the SAME \(rLong.out.count) token ids as the attestant",
      "gate 10 the split path diverged from the attestant: \(fL.out) vs \(rLong.out)")
    print(String(format: "  speedup vs the attestant: decode %.2fx (%.3f s -> %.3f s over %d forwards), end-to-end %.2fx",
                 rLong.decode / fL.decode, rLong.decode, fL.decode, rLong.forwards,
                 (rLong.prefill + rLong.decode) / (fL.prefill + fL.decode)))
    fLong = fL
    usePartsNow = 1
}

// ---- gate 11: THE LANE PATH — Stone 5's answer -----------------------------------------------------
print("=== gate 11: the lane path's tokens (simd_sum, superblock-hoisted) ===")
mvNow = .lane; fastOps = true          // STONE 16's twins ride with the fast matvecs, from here on
let lShort = generate(promptIds, short), lLong = generate(promptIds, nsteps)
report("lane-short", lShort); report("lane-long ", lLong); vsWorld("lane", lLong)
// The lane path's agreement is NOT checked here: it is checked once, together with the slot path's,
// at the single gate 11 below. Two `check` calls would be two gates, and this stone deliberately
// keeps the gate count at 13 while widening what the eleventh one demands.
print("  lane ids match the attestant: \(lLong.out == rLong.out)")
print(String(format: "  speedup vs the attestant: decode %.2fx (%.3f s -> %.3f s over %d forwards), end-to-end %.2fx",
             rLong.decode / lLong.decode, rLong.decode, lLong.decode, rLong.forwards,
             (rLong.prefill + rLong.decode) / (lLong.prefill + lLong.decode)))
if let f = fLong {
    print(String(format: "  speedup vs the SPLIT path (Stone 4's answer): decode %.2fx (%.3f s -> %.3f s), end-to-end %.2fx (%.3f -> %.3f tok/s)",
                 f.decode / lLong.decode, f.decode, lLong.decode,
                 (f.prefill + f.decode) / (lLong.prefill + lLong.decode),
                 Double(f.out.count) / (f.prefill + f.decode),
                 Double(lLong.out.count) / (lLong.prefill + lLong.decode)))
}
// two sizes and a slope on the lane path too — no rate here is one point pretending to be a line
let lSlope = (lLong.decode - lShort.decode) / max(1.0, Double(lLong.forwards - lShort.forwards))
print(String(format: "  lane, two sizes and a slope: decode %.3f s at %d forwards and %.3f s at %d -> %.4f s per additional token (%.3f tok/s marginal)",
             lShort.decode, lShort.forwards, lLong.decode, lLong.forwards, lSlope, 1.0 / max(1e-9, lSlope)))

// ---- STONE 13: THE SLOT PATH, and it is folded into gate 11 rather than given a gate of its own.
// Gate 11's claim has always been "the fast path generates the ATTESTANT's tokens". There are now two
// fast paths, so the gate demands it of BOTH: it falls if either diverges. A fourteenth gate would
// have let the lane path carry a green while the slot path quietly did not, and the token ids are the
// one place where a wrong thread map cannot hide.
print("=== gate 11 (cont.): the slot path's tokens (4-wide superblock slot, simd_sum) ===")
mvNow = .slot
let sShort = generate(promptIds, short), sLong = generate(promptIds, nsteps)
report("slot-short", sShort); report("slot-long ", sLong); vsWorld("slot", sLong)
check(sLong.out == rLong.out && lLong.out == rLong.out,
  "gate 11 BOTH fast paths (lane and slot) generate the SAME \(rLong.out.count) token ids as the attestant",
  "gate 11 a fast path diverged from the attestant: lane \(lLong.out) slot \(sLong.out) vs \(rLong.out)")
print(String(format: "  speedup vs the attestant: decode %.2fx (%.3f s -> %.3f s over %d forwards), end-to-end %.2fx",
             rLong.decode / sLong.decode, rLong.decode, sLong.decode, rLong.forwards,
             (rLong.prefill + rLong.decode) / (sLong.prefill + sLong.decode)))
print(String(format: "  speedup vs the LANE path (Stone 5's answer): decode %.2fx (%.3f s -> %.3f s), end-to-end %.2fx (%.3f -> %.3f tok/s)",
             lLong.decode / sLong.decode, lLong.decode, sLong.decode,
             (lLong.prefill + lLong.decode) / (sLong.prefill + sLong.decode),
             Double(lLong.out.count) / (lLong.prefill + lLong.decode),
             Double(sLong.out.count) / (sLong.prefill + sLong.decode)))
let sSlope = (sLong.decode - sShort.decode) / max(1.0, Double(sLong.forwards - sShort.forwards))
print(String(format: "  slot, two sizes and a slope: decode %.3f s at %d forwards and %.3f s at %d -> %.4f s per additional token (%.3f tok/s marginal)",
             sShort.decode, sShort.forwards, sLong.decode, sLong.forwards, sSlope, 1.0 / max(1e-9, sSlope)))
print(String(format: "  DISPATCHES PER TOKEN: %d on the attestant's ops -> %d on Stone 16's twins (counted, not derived)",
             rLong.disp, sLong.disp))
mvNow = .serial; fastOps = false

// ---- gate 5: the pool is a cache, and the run is reproducible -------------------------------------
let again = generate(promptIds, short)
check(again.out == rShort.out,
      "gate 5 a second run from a freshly zeroed pool reproduces the same ids — the cache is state, not luck",
      "gate 5 the second run diverged: \(again.out) vs \(rShort.out)")

// ---- gate 6b: the ids follow the prompt ----------------------------------------------------------
let other = generate(encode("Once upon a time"), short)
print("  control prompt \"Once upon a time\" -> \(other.out)  \"\(decodeIds(other.out))\"")
check(other.out != rShort.out && rShort.out.allSatisfy { $0 >= 0 && $0 < vocabN },
      "gate 6 real token ids out, legal vocab indices, input-dependent",
      "gate 6 the ids are constant across prompts or out of range")

// A command buffer that failed AFTER the last gate still invalidates every number above it, so the
// offer is consulted once more before any verdict is printed. The gate count is COMPUTED from what
// was actually asked (0 plus the 13 of Stone 13, or 11 without the split path) rather than typed.
let nGates = (PARTS > 1 ? 13 : 11) + 1
if gpuErrors > 0 {
    print("=== VERDICT FAIL — \(gpuErrors) command buffer(s) failed (\(gpuFirstError ?? "?")); the numbers above are not trustworthy ===")
    exit(1)
}
print(failures == 0 ? "=== VERDICT PASS — \(nGates) gates ===" : "=== VERDICT FAIL — \(failures) gate(s) ===")
exit(failures == 0 ? 0 : 1)
SWIFT

# ── the carrier binary, cached across RUNS by its own source's sha256 ─────────────────────────
# Keyed on the SOURCE, exactly like the .metallib above: an edit to a single character of the runner
# is a cache MISS by construction, so there is no state in which a stale binary answers for a changed
# carrier. Without this every ask paid ~9 s of `swiftc -O` to compile a file that had not changed.
RUNNER_SHA="$(shasum -a 256 "$work/runner.swift" | cut -c1-16)"
RUNNER="$CACHE/runner-$RUNNER_SHA"
if [[ -x "$RUNNER" ]]; then
    echo "=== carrier cache HIT: $(basename "$RUNNER") ==="
else
    echo "=== compiling the carrier ==="
    swiftc -O -o "$RUNNER.tmp" "$work/runner.swift" 2>"$work/swift.err" || {
        echo "FAIL  swiftc failed"; tail -30 "$work/swift.err"; exit 1; }
    mv "$RUNNER.tmp" "$RUNNER"
fi

"$RUNNER" "$LIB" "$BLOB" "$TBL" "$CFG" "$VOC" "$REF" "$PROMPT" "$NSTEPS" "$MAXPOS" "$REFID" "$REFROW"
rc=$?
exit $rc
