#!/usr/bin/env bash
# metal_moe_token.sh — THE FIRST REAL MIXTURE-OF-EXPERTS TOKEN GENERATED FORM-NATIVE.
#
# dolphin-2.9-mixtral-8x22b: 141 B parameters, 56 layers, d=6144, dff=16384, 48/8 GQA, 8 experts
# routed top-2, Q6_K, 115 529 748 672 bytes (107.595 GiB) on this disk. Real weights, full width, real
# tokenizer ids in and real token ids out, every arithmetic op executed by a kernel the BODY emitted,
# off the QUANTIZED bytes with no f32 copy of any tensor materialized anywhere.
#
# WHAT WAS MISSING BEFORE THIS, and it is three facts about ADDRESSING, not about arithmetic. Stone 11
# (receipts/2026-07-21-ds4-metal-gap-map.md) measured them; this carrier closes them:
#
#   1  THE `one` IN "ONE MTLBUFFER" WAS NEVER CHOSEN. maxBufferLength on this M4 Max is 80.64 GiB.
#      Every Metal proof this body owns says "the WHOLE blob mapped into ONE MTLBuffer" — and that
#      `one` is what a 2.01 GB model happened to permit. This model is 26.95 GiB past it. The remedy
#      is OVERLAPPING PAGE-ALIGNED VIEWS (ds4_metal.m:1706-1812, read for shape and cited, not
#      copied): N MTLBuffers over the SAME mmap, adjacent views overlapping by (largest tensor + one
#      page), so every tensor lies wholly inside at least one view and every kernel still receives one
#      buffer and one inner offset. ZERO MSL CHANGE, ZERO EPSILON IMPACT — the arithmetic never learns
#      that the buffer was cut.
#   2  THE EXPERT GATHER IS AN OFFSET, NOT A KERNEL. The gap map proposed adding an `ids` device
#      parameter and an `nb02` uniform to the matvec, transcribing ds4's kernel_mul_mv_id
#      (metal/moe.metal:3521-3597). For a DECODE of one token that is unnecessary: `t.off + e*nb02` is
#      a number the HOST already has to compute, because it is the host that binds the buffer. So the
#      gather costs no new MSL, no new epsilon, and the Q6_K bit-exactness argument carries through
#      literally unchanged — it is the same kernel on the same bytes. What it costs is a ROUND TRIP:
#      the routing ids must reach the host before the expert matvecs can be encoded, so the per-token
#      command buffer is CUT ONCE PER LAYER. That is named, measured below, and is a decode-only
#      bargain — a prefill routing differently per position would want the device-side gather after all.
#   3  Q8_0 EXISTED NOWHERE. 112 tensors here are Q8_0 (every attn_k, every attn_v). q8-0-msl.fk is
#      the carver; gate 5 judges it at the tensor's real width against Form's fp64.
#
#   NOT a blocker, and the gap map said so: top-k routing. ds4 needs a bitonic sort for top-6 of 256;
#   this routes top-2 of 8 and a serial scan is exact. There is no sort in this lane.
#
# THE GATES (all must pass before any token is believed):
#   1  THE CONFIG IS THE FILE'S      56/6144/16384/48/8/128, 8 experts, 2 used, rope base and rms eps
#                                    read; tied=0 and has_rope_freqs=0 read from the TABLE.
#   2  THE TABLE ACCOUNTS FOR THE FILE  sum of all 563 tensors' bytes + the data base == the file's
#                                    own size, to the byte. A table with a wrong length for ANY type
#                                    could not close.
#   3  THE VIEWS COVER EVERY TENSOR  arithmetic, before any dispatch: every tensor [off, off+len) lies
#                                    wholly inside its assigned view, and every view is <= maxBufferLength.
#   4  THE EMBEDDING IS THE BODY'S   the GPU gather of token_embd row `id` equals Form's dequant BIT
#                                    FOR BIT (Q6_K's one-rounding argument).
#   5  Q8_0 IS THE BODY'S            the GPU's Q8_0 fused matvec of a real blk.0.attn_k row at width
#                                    6144 is Form's fp64 answer, inside the DERIVED cols*u*SUM|term|.
#   6  THE ROUTER IS THE BODY'S      the F16 gate matvec reproduces all 8 of Form's fp64 logits, and
#                                    the route kernel picks the SAME experts with the same weights.
#   7  THE EXPERT GATHER IS THE BODY'S  a matvec bound at t.off + e*nb02 equals Form's fp64 dot of
#                                    THAT expert's row — the gather judged at the 3-D offset.
#   8  THE FAR REGION IS READABLE    output.weight begins 113 386 576 224 bytes in, 32.7 GiB PAST the
#                                    one-buffer ceiling. Its row 0 AND its last row are bit-exact.
#                                    Nothing in this body has ever read a byte that far through a GPU.
#   9  A TOKEN                       greedy decode emits ids; legal vocab indices; input-dependent.
#
# Run:  form/native/metal/metal_moe_token.sh [nsteps] ["prompt"]     (defaults 4, a fixed prompt)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every other Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
NSTEPS="${1:-4}"
PROMPT="${2:-The capital of France is}"
BLOB="${FORM_MOE_BLOB:-$HOME/.ollama/models/blobs/sha256-550981a79100990c3083054da771af4f3a9658eb15aa5081e23b2085a74448f4}"
REFID="${FORM_REF_ID:-1234}"
REFROW="${FORM_REF_ROW:-0}"
REFEXP="${FORM_REF_EXPERT:-3}"
MAXPOS="${FORM_MAXPOS:-32}"
CACHE="$ROOT/native/metal/.moe-token-cache"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the GPU witness needs an Apple GPU + swiftc"
    exit 2
fi
if [[ ! -f "$BLOB" ]]; then
    echo "SKIP  the dolphin-mixtral-8x22b Q6_K GGUF blob is not on this host: $BLOB"
    echo "      (ollama pull dolphin-mixtral:8x22b-v2.9-q6_K, or set FORM_MOE_BLOB)"
    exit 2
fi
if [[ ! -x "$GO_BIN" ]]; then
    echo "  building go kernel..." >&2
    (cd "$ROOT/form-kernel-go" && go build -o bin-go .)
fi

work="$(mktemp -d "${TMPDIR:-/tmp}/fkmoetok.XXXXXX")"
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
while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/moe-token.fk)

mkdir -p "$CACHE"
key="$( { shasum -a 256 "${FILES[@]}" | awk '{print $1}'; printf '%s %s\n' "$BLOB" "$(stat -f %z "$BLOB")"; } | shasum -a 256 | cut -c1-16 )"
CFG="$CACHE/cfg-$key.txt"; TBL="$CACHE/tbl-$key.txt"; VOC="$CACHE/voc-$key.txt"; MSL="$CACHE/msl-$key.metal"
REF="$CACHE/ref-$key-$REFID-$REFROW-$REFEXP.txt"

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
emit "$CFG" "config"  "(mtk-emit-config \"$BLOB\")"
emit "$TBL" "table"   "(mtk-emit-table \"$BLOB\")"
emit "$VOC" "vocab"   "(mtk-emit-vocab \"$BLOB\")"
emit "$REF" "refs"    "(mtk-emit-ref \"$BLOB\" $REFID $REFROW $REFEXP)"
if [[ -s "$MSL" ]]; then
    echo "  body cache HIT  msl"
else
    printf '(mtk-emit-msl)\n' > "$work/e.fk"
    "$GO_BIN" "${FILES[@]}" "$work/e.fk" > "$work/msl.out" 2>"$work/e.err" || {
        echo "FAIL  MSL emission failed"; cat "$work/e.err"; exit 1; }
    awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/msl.out" > "$MSL"
    echo "  body cache MISS msl — $(wc -c < "$MSL" | tr -d ' ') bytes"
fi
for k in form_q6k_dequant_f32 form_q6k_matvec_f32 form_q6k_matvec_lane_f32 \
         form_q8_0_dequant_f32 form_q8_0_matvec_f32 form_q8_0_matvec_lane_f32 \
         form_f16_matvec_f32 form_moe_route_f32 form_scale_f32 form_axpy_f32 form_rope_plain_f32 \
         form_rmsnorm_f32 form_gqa_decode_f32 form_swiglu_f32 form_add_f32 form_argmax_f32; do
    grep -q "kernel void $k" "$MSL" || { echo "FAIL  kernel $k was not emitted"; exit 1; }
done
head -c 200 "$MSL" | grep -q '#include <metal_stdlib>' || { echo "FAIL  metal_stdlib header is not at the top of the unit"; exit 1; }
grep -q 'using namespace metal' "$MSL" && { echo "FAIL  the unit emitted 'using namespace metal;' — the body's round becomes ambiguous"; exit 1; }
echo "  $(grep -o 'kernel void ' "$MSL" | wc -l | tr -d ' ') kernels emitted, $(wc -c < "$MSL" | tr -d ' ') bytes, every character authored by a .fk cell"

msl_sha="$(shasum -a 256 "$MSL" | cut -c1-16)"
LIB="$CACHE/moe-$msl_sha.metallib"
if [[ -f "$LIB" ]]; then
    echo "  metallib cache HIT: $(basename "$LIB")"
else
    xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
          -c "$MSL" -o "$work/moe.air" 2>"$work/metal.err" \
      && xcrun -sdk macosx metallib "$work/moe.air" -o "$LIB" 2>>"$work/metal.err" || {
        echo "FAIL  offline metal compile failed"; cat "$work/metal.err"; exit 1; }
    echo "  metallib cache MISS -> compiled and cached: $(basename "$LIB")"
fi

# ── the carrier ───────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
// The first form-native Mixture-of-Experts token. CARRIER ONLY: it maps, binds, dispatches and times.
// Every number it judges came from the body; every kernel it runs was emitted by a .fk cell.
import Metal
import Foundation

let a = CommandLine.arguments
let libPath = a[1], blobPath = a[2], tablePath = a[3], cfgPath = a[4]
let vocPath = a[5], refPath = a[6], prompt = a[7]
let nsteps = Int(a[8])!, maxpos = Int(a[9])!, refId = Int(a[10])!, refRow = Int(a[11])!
let refExpert = Int(a[12])!

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
let nExpert = ci("llama.expert_count"), nExpertUsed = ci("llama.expert_used_count")
let bosId = ci("tokenizer.ggml.bos_token_id"), eosId = ci("tokenizer.ggml.eos_token_id")
let ropeBase = Float(cfg["llama.rope.freq_base"]!)
let rmsEps = Float(cfg["llama.attention.layer_norm_rms_epsilon"]!)
let tied = ci("tied_embeddings") == 1
let hasRopeFreqs = ci("has_rope_freqs") == 1
let kvd = nKV * headDim
let scale = Float(1.0 / Double(headDim).squareRoot())

print("=== gate 1: the config is the FILE's ===")
print("  layers \(nLayer)  d \(dModel)  dff \(dFF)  heads \(nHead)/\(nKV)  head_dim \(headDim)  vocab \(vocabN)")
print("  experts \(nExpert) routed top-\(nExpertUsed)  rope_base \(ropeBase)  rms_eps \(rmsEps)  bos \(bosId)  eos \(eosId)")
print("  tied \(tied)  has_rope_freqs \(hasRopeFreqs)   <- both READ FROM THE TABLE, not assumed")
check(nLayer > 0 && dModel > 0 && nHead % nKV == 0 && headDim * nHead == dModel
      && nExpert > 1 && nExpertUsed >= 1 && nExpertUsed <= nExpert && !tied && !hasRopeFreqs,
      "gate 1 config read from the blob's own metadata KVs, self-consistent, MoE and untied",
      "gate 1 config is not self-consistent")
// the route kernel's declared radius, checked rather than trusted
if nExpert > 64 || nExpertUsed > 8 {
    print("SKIP  form_moe_route_f32 speaks for ne<=64 and nsel<=8; this file is \(nExpert)/\(nExpertUsed)")
    exit(2)
}

// ---- the body's tensor table (THREE dimensions, four ggml types) ---------------------------------
struct TInfo { let type: Int; let nd: Int; let d0: Int; let d1: Int; let d2: Int; let off: Int; let len: Int }
var table: [String: TInfo] = [:]
var dataBase = 0, nTensors = 0, totalBytes = 0, maxTensorBytes = 0
for l in try String(contentsOfFile: tablePath, encoding: .utf8).split(separator: "\n") {
    let p = l.split(separator: " ")
    if p.count == 2 && p[0] == "DATABASE" { dataBase = Int(p[1])!; continue }
    if p.count == 2 && p[0] == "NTENSORS" { nTensors = Int(p[1])!; continue }
    guard p.count == 9, p[0] == "T" else { continue }
    let t = TInfo(type: Int(p[2])!, nd: Int(p[3])!, d0: Int(p[4])!, d1: Int(p[5])!, d2: Int(p[6])!,
                  off: Int(p[7])!, len: Int(p[8])!)
    table[String(p[1])] = t
    totalBytes += t.len
    if t.len > maxTensorBytes { maxTensorBytes = t.len }
}
func T(_ n: String) -> TInfo {
    guard let t = table[n] else { print("FAIL  tensor \(n) is not in the body's table"); exit(1) }
    return t
}
// ---- gate 2: the table accounts for the file, to the byte -----------------------------------------
let fdProbe = open(blobPath, O_RDONLY)
guard fdProbe >= 0 else { print("FAIL cannot open blob"); exit(1) }
var stProbe = stat(); fstat(fdProbe, &stProbe)
let fileLen = Int(stProbe.st_size)
print("=== gate 2: the body's table accounts for the whole file ===")
print("  \(nTensors) tensors, data base \(dataBase), sum of lengths \(totalBytes), file \(fileLen)")
print("  largest single tensor \(maxTensorBytes) B = \(String(format: "%.2f", Double(maxTensorBytes)/1073741824.0)) GiB")
check(table.count == nTensors && dataBase + totalBytes == fileLen,
      "gate 2 all \(nTensors) tensors' bytes + the data base == the file's own size, exactly",
      "gate 2 the table does not account for the file: \(dataBase) + \(totalBytes) != \(fileLen)")

// ---- the device ---------------------------------------------------------------------------------
guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
let queue = dev.makeCommandQueue()!
let maxBuf = dev.maxBufferLength
let recWS = dev.recommendedMaxWorkingSetSize
print("=== the machine, measured — never quoted ===")
print("  \(dev.name)  maxBufferLength \(maxBuf) B = \(String(format: "%.2f", Double(maxBuf)/1073741824.0)) GiB")
print("  recommendedMaxWorkingSetSize \(recWS) B = \(String(format: "%.2f", Double(recWS)/1073741824.0)) GiB")
print("  the model \(fileLen) B = \(String(format: "%.3f", Double(fileLen)/1073741824.0)) GiB")
print("  the model exceeds maxBufferLength by \(String(format: "%.2f", Double(fileLen - maxBuf)/1073741824.0)) GiB — ONE buffer is impossible")

// ---- BLOCKER 1: OVERLAPPING PAGE-ALIGNED VIEWS ---------------------------------------------------
// One mmap; N MTLBuffers over slices of it. Adjacent views overlap by (largest tensor + one page), so
// every tensor lies wholly inside at least one view. The shape is ds4_metal.m:1706-1812's, read for
// its reason and rederived here; the invariant below is CHECKED arithmetically before a single
// dispatch, because "every tensor fits" is exactly the kind of claim that is true for the tensors you
// happened to test (corpus row 826).
let page = Int(getpagesize())
func roundUp(_ x: Int, _ a: Int) -> Int { (x + a - 1) / a * a }
func roundDn(_ x: Int, _ a: Int) -> Int { x / a * a }
let mapLen = roundUp(fileLen, page)
let overlap = roundUp(maxTensorBytes + page, page)
let viewCap = roundDn(min(maxBuf, Int(Double(1073741824) * Double(ProcessInfo.processInfo.environment["FORM_VIEW_GIB"].flatMap { Double($0) } ?? 40.0))), page)
guard viewCap > overlap * 2 else { print("FAIL  view cap \(viewCap) is not larger than 2x the overlap \(overlap)"); exit(1) }
let stride = roundDn(viewCap - overlap, page)
var viewStart: [Int] = [], viewLen: [Int] = []
do {
    var s = 0
    while true {
        let len = min(viewCap, mapLen - s)
        viewStart.append(s); viewLen.append(len)
        if s + len >= mapLen { break }
        s += stride
    }
}
guard let mapped = mmap(nil, mapLen, PROT_READ, MAP_PRIVATE, fdProbe, 0), mapped != MAP_FAILED
else { print("FAIL  mmap over the model failed"); exit(1) }
var views: [MTLBuffer] = []
for i in 0..<viewStart.count {
    guard let b = dev.makeBuffer(bytesNoCopy: mapped.advanced(by: viewStart[i]), length: viewLen[i],
                                 options: .storageModeShared, deallocator: nil)
    else { print("FAIL  bytesNoCopy view \(i) (start \(viewStart[i]), len \(viewLen[i])) was refused"); exit(1) }
    views.append(b)
}
print("=== BLOCKER 1 CLOSED: \(views.count) overlapping page-aligned views over ONE mmap ===")
print("  view cap \(viewCap) B = \(String(format: "%.2f", Double(viewCap)/1073741824.0)) GiB (<= maxBufferLength)")
print("  overlap  \(overlap) B = largest tensor \(maxTensorBytes) + one \(page)-byte page, rounded up")
print("  stride   \(stride) B")
for i in 0..<views.count {
    print("  view \(i): [\(viewStart[i]), \(viewStart[i] + viewLen[i]))  "
          + String(format: "%.2f GiB", Double(viewLen[i]) / 1073741824.0))
}
// resolve an absolute file offset to (view, inner offset)
func viewOf(_ off: Int) -> Int { min(off / stride, views.count - 1) }
struct Res { let buf: MTLBuffer; let off: Int }
func res(_ off: Int) -> Res { let v = viewOf(off); return Res(buf: views[v], off: off - viewStart[v]) }

// ---- gate 3: the view invariant, checked for ALL 563 tensors before any dispatch ------------------
var badTensor: String? = nil
var farTensors = 0
for (name, t) in table {
    let v = viewOf(t.off)
    if t.off < viewStart[v] || t.off + t.len > viewStart[v] + viewLen[v] { badTensor = name; break }
    if t.off >= maxBuf { farTensors += 1 }
}
var badView = false
for l in viewLen where l > maxBuf { badView = true }
print("=== gate 3: the view invariant ===")
print("  \(farTensors) of \(nTensors) tensors begin PAST the \(String(format: "%.2f", Double(maxBuf)/1073741824.0)) GiB one-buffer ceiling")
check(badTensor == nil && !badView,
      "gate 3 every one of \(nTensors) tensors lies WHOLLY inside its assigned view, and no view exceeds maxBufferLength",
      "gate 3 tensor \(badTensor ?? "-") straddles a view boundary, or a view exceeds maxBufferLength")

// ---- the body's tokenizer -------------------------------------------------------------------------
// This file's tokenizer.ggml.model is "llama" — SentencePiece, NOT llama3's byte-level BPE. Two pieces
// of tokenizer knowledge live in the carrier and are named rather than pretended away:
//   (a) U+2581 (LOWER ONE EIGHTH BLOCK) stands for a space, and the text is prefixed with one;
//   (b) a piece spelled "<0xNN>" is the raw byte NN.
// Everything else — the pieces themselves — is the FILE's, streamed as hex so nothing re-encodes.
var pieceOf: [Int: String] = [:]
var idOfPiece: [String: Int] = [:]
var byteFallback: [UInt8: Int] = [:]
do {
    for l in try String(contentsOfFile: vocPath, encoding: .utf8).split(separator: "\n") {
        let p = l.split(separator: " ", maxSplits: 1, omittingEmptySubsequences: false)
        guard p.count == 2, let id = Int(p[0]) else { continue }
        let hex = p[1]; var bytes: [UInt8] = []; var it = hex.startIndex
        while it < hex.endIndex {
            let nx = hex.index(it, offsetBy: 2)
            bytes.append(UInt8(hex[it..<nx], radix: 16)!); it = nx
        }
        let s = String(decoding: bytes, as: UTF8.self)
        pieceOf[id] = s
        if idOfPiece[s] == nil { idOfPiece[s] = id }
        if s.count == 6, s.hasPrefix("<0x"), s.hasSuffix(">"),
           let b = UInt8(s.dropFirst(3).dropLast(), radix: 16) { byteFallback[b] = id }
    }
}
let SPM_SPACE = "\u{2581}"
func decodeIds(_ ids: [Int], stripLeadingSpace: Bool) -> String {
    var out: [UInt8] = []
    for id in ids {
        guard let p = pieceOf[id] else { continue }
        if p.count == 6, p.hasPrefix("<0x"), p.hasSuffix(">"), let b = UInt8(p.dropFirst(3).dropLast(), radix: 16) {
            out.append(b); continue
        }
        out.append(contentsOf: Array(p.replacingOccurrences(of: SPM_SPACE, with: " ").utf8))
    }
    var s = String(decoding: out, as: UTF8.self)
    if stripLeadingSpace && s.hasPrefix(" ") { s.removeFirst() }
    return s
}
// greedy LONGEST-MATCH over the vocabulary. Not BPE's merge order and not SentencePiece's Viterbi —
// stated, not hidden. The harness prints the pieces so a reader can see exactly what went in.
func encode(_ s: String) -> [Int] {
    let chars = Array((SPM_SPACE + s.replacingOccurrences(of: " ", with: SPM_SPACE)))
    var ids = [bosId]; var i = 0
    while i < chars.count {
        var take = 0, took = -1
        var j = min(chars.count, i + 32)
        while j > i {
            if let id = idOfPiece[String(chars[i..<j])] { take = j - i; took = id; break }
            j -= 1
        }
        if took < 0 {
            // no piece matches this character: fall back to its raw UTF-8 bytes, the SPM way
            for b in Array(String(chars[i]).utf8) { if let bid = byteFallback[b] { ids.append(bid) } }
            i += 1; continue
        }
        ids.append(took); i += take
    }
    return ids
}

// ---- the body's fp64 references --------------------------------------------------------------------
var refEmb: [Double] = [], refRms: [Double] = [], refQ: Double = 0, refK: Double = 0
var refQAbs: Double = 0, refKAbs: Double = 0, refExpAbs: Double = 0
var refGate: [Double] = [], refRouteIds: [Int] = [], refRouteW: [Double] = []
var refExp: Double = 0, refFar: [Double] = [], refFarLast: [Double] = []
var refNb02 = 0, refFarRow = 0
do {
    var sec = ""; var routeSeen = 0
    for l in try String(contentsOfFile: refPath, encoding: .utf8).split(separator: "\n") {
        let s = String(l)
        if s.hasPrefix("REFEMB") { sec = "E"; continue }
        if s.hasPrefix("REFRMS") { sec = "R"; continue }
        if s.hasPrefix("REFQABS") { sec = "QA"; continue }
        if s.hasPrefix("REFKABS") { sec = "KA"; continue }
        if s.hasPrefix("REFEXPABS") { sec = "XA"; continue }
        if s.hasPrefix("REFQ")   { sec = "Q"; continue }
        if s.hasPrefix("REFK")   { sec = "K"; continue }
        if s.hasPrefix("REFGATE") { sec = "G"; continue }
        if s.hasPrefix("REFROUTE") { sec = "T"; routeSeen = 0; continue }
        if s.hasPrefix("REFEXP") {
            sec = "X"; let p = s.split(separator: " "); if p.count == 4 { refNb02 = Int(p[3])! }; continue
        }
        if s.hasPrefix("REFFARLAST") {
            sec = "L"; let p = s.split(separator: " "); if p.count == 2 { refFarRow = Int(p[1])! }; continue
        }
        if s.hasPrefix("REFFAR") { sec = "F"; continue }
        if s == "END" { sec = ""; continue }
        guard let v = Double(s) else { continue }
        switch sec {
        case "E": refEmb.append(v)
        case "R": refRms.append(v)
        case "Q": refQ = v; sec = ""
        case "K": refK = v; sec = ""
        case "QA": refQAbs = v; sec = ""
        case "KA": refKAbs = v; sec = ""
        case "XA": refExpAbs = v; sec = ""
        case "G": refGate.append(v)
        case "T":
            if routeSeen < nExpertUsed { refRouteIds.append(Int(v)) } else { refRouteW.append(v) }
            routeSeen += 1
        case "X": refExp = v; sec = ""
        case "F": refFar.append(v)
        case "L": refFarLast.append(v)
        default: break
        }
    }
}

func pipe(_ n: String) throws -> MTLComputePipelineState {
    try dev.makeComputePipelineState(function: lib.makeFunction(name: n)!)
}
let pQ6D = try pipe("form_q6k_dequant_f32"), pQ6M = try pipe("form_q6k_matvec_f32")
let pQ6L = try pipe("form_q6k_matvec_lane_f32")
let pQ8D = try pipe("form_q8_0_dequant_f32"), pQ8M = try pipe("form_q8_0_matvec_f32")
let pQ8L = try pipe("form_q8_0_matvec_lane_f32")
let pF16 = try pipe("form_f16_matvec_f32"), pRoute = try pipe("form_moe_route_f32")
let pScale = try pipe("form_scale_f32"), pAxpy = try pipe("form_axpy_f32")
let pRms = try pipe("form_rmsnorm_f32"), pRope = try pipe("form_rope_plain_f32")
let pAttn = try pipe("form_gqa_decode_f32"), pSwi = try pipe("form_swiglu_f32")
let pAdd = try pipe("form_add_f32"), pArg = try pipe("form_argmax_f32")
// the lane kernels assume a SIMD width of exactly 32 — read from the pipeline and refused, never assumed
let SIMDW = pQ6L.threadExecutionWidth
if SIMDW != 32 {
    print("SKIP  this GPU's threadExecutionWidth is \(SIMDW), not 32 — the lane kernels do not speak for it")
    exit(2)
}

func buf(_ n: Int) -> MTLBuffer { dev.makeBuffer(length: max(n, 16) * 4, options: .storageModeShared)! }
// ---- THE POOL. Allocated ONCE, before any token, and never reallocated. --------------------------
let bX = buf(dModel), bXb = buf(dModel), bQ = buf(dModel)
let bAttn = buf(dModel), bProj = buf(dModel), bFfn = buf(dModel), bExpOut = buf(dModel)
let bGate = buf(dFF), bUp = buf(dFF), bAct = buf(dFF)
let bLogits = buf(vocabN)
let bCacheK = buf(nLayer * maxpos * kvd), bCacheV = buf(nLayer * maxpos * kvd)
let bScratch = buf(2 * nHead * maxpos)
let bRouter = buf(nExpert)
let bIds = dev.makeBuffer(length: 64, options: .storageModeShared)!
let bWts = buf(16)
let bOutI = dev.makeBuffer(length: 16, options: .storageModeShared)!
let bOutV = buf(4)
let bRef = buf(max(dModel, dFF))
let poolBytes = (dModel * 7 + dFF * 3 + vocabN + 2 * nLayer * maxpos * kvd + 2 * nHead * maxpos + nExpert + 16) * 4
print(String(format: "pooled: %.1f MB of activation + KV state, allocated ONCE for the whole run",
             Double(poolBytes) / 1048576.0))

// counted across the WHOLE run, gates and generation alike
var gpuErrors = 0
var gpuFirstError: String? = nil

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
    // A command buffer that FAILS writes nothing, and every readback buffer in this
    // carrier is freshly allocated and therefore zeroed — so an unchecked failure does
    // not look like an error, it looks like arithmetic that disagrees with Form at
    // almost every weight. This check is the difference between "the GPU is wrong" and
    // "the GPU did not run", and those are not the same sentence.
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

// a quantized matvec straight off the resident views: rows = d1, cols = d0. `base` is an ABSOLUTE file
// offset, which is how BOTH the multi-view resolution and the expert gather enter — the kernel never
// learns that either happened.
enum MVPath: String { case serial, lane }
var mvNow: MVPath = .lane
func matvecAt(_ s: Step, type: Int, base: Int, rows: Int, cols: Int,
              _ x: MTLBuffer, _ y: MTLBuffer, yOff: Int = 0, barrier: Bool = true, force: MVPath? = nil) {
    var r = UInt32(rows), c = UInt32(cols)
    let R = res(base)
    let path = force ?? mvNow
    let p: MTLComputePipelineState
    switch (type, path) {
    case (14, .lane): p = pQ6L
    case (14, .serial): p = pQ6M
    case (8, .lane): p = pQ8L
    case (8, .serial): p = pQ8M
    case (1, _): p = pF16
    default: print("FAIL  no matvec kernel for ggml type \(type)"); exit(1)
    }
    let width = (type != 1 && path == .lane) ? rows * 32 : rows
    s.go(p, width, barrier: barrier, tgMul32: (type != 1 && path == .lane)) { e in
        e.setBuffer(R.buf, offset: R.off, index: 0)
        e.setBuffer(x, offset: 0, index: 1)
        e.setBuffer(y, offset: yOff, index: 2)
        e.setBytes(&r, length: 4, index: 3); e.setBytes(&c, length: 4, index: 4)
    }
}
func matvec(_ s: Step, _ t: TInfo, _ x: MTLBuffer, _ y: MTLBuffer, yOff: Int = 0, barrier: Bool = true,
            expert: Int = 0, force: MVPath? = nil) {
    // THE EXPERT GATHER, and this line is all of it: nb02 is the byte length of ONE expert's slice,
    // which is the tensor's own length divided by its third dimension — a number the body's table
    // printed. The kernel is untouched; the epsilon is untouched; the bytes are the same bytes.
    let nb02 = t.d2 > 1 ? t.len / t.d2 : 0
    matvecAt(s, type: t.type, base: t.off + expert * nb02, rows: t.d1, cols: t.d0,
             x, y, yOff: yOff, barrier: barrier, force: force)
}
func dequantAt(_ s: Step, type: Int, base: Int, off: Int, n: Int, _ y: MTLBuffer) {
    var o = UInt32(off), nn = UInt32(n)
    let R = res(base)
    s.go(type == 14 ? pQ6D : pQ8D, n) { e in
        e.setBuffer(R.buf, offset: R.off, index: 0)
        e.setBuffer(y, offset: 0, index: 1)
        e.setBytes(&o, length: 4, index: 2); e.setBytes(&nn, length: 4, index: 3)
    }
}
func rmsnorm(_ s: Step, _ x: MTLBuffer, _ gain: TInfo, _ y: MTLBuffer) {
    var n = UInt32(dModel), eps = rmsEps
    let R = res(gain.off)
    s.go(pRms, 1) { e in
        e.setBuffer(x, offset: 0, index: 0)
        e.setBuffer(R.buf, offset: R.off, index: 1)     // F32 gains, read in place
        e.setBuffer(y, offset: 0, index: 2)
        e.setBytes(&n, length: 4, index: 3); e.setBytes(&eps, length: 4, index: 4)
    }
}
// RoPE with NO frequency factors — the correct kernel for a file with no rope_freqs.weight, and the
// carrier only reaches it because the CONFIG said has_rope_freqs 0.
func rope(_ s: Step, _ v: MTLBuffer, off: Int, heads: Int, pos: Int, barrier: Bool = true) {
    var nh = UInt32(heads), hd = UInt32(headDim), p = UInt32(pos), b = ropeBase
    s.go(pRope, heads, barrier: barrier) { e in
        e.setBuffer(v, offset: off, index: 0)
        e.setBytes(&nh, length: 4, index: 1); e.setBytes(&hd, length: 4, index: 2)
        e.setBytes(&p, length: 4, index: 3); e.setBytes(&b, length: 4, index: 4)
    }
}
func elem(_ s: Step, _ p: MTLComputePipelineState, _ x: MTLBuffer, _ y: MTLBuffer, _ z: MTLBuffer, _ n: Int) {
    var nn = UInt32(n)
    s.go(p, n) { e in
        e.setBuffer(x, offset: 0, index: 0); e.setBuffer(y, offset: 0, index: 1)
        e.setBuffer(z, offset: 0, index: 2); e.setBytes(&nn, length: 4, index: 3)
    }
}
func axpyLike(_ s: Step, _ p: MTLComputePipelineState, _ x: MTLBuffer, _ y: MTLBuffer, _ aa: Float, _ n: Int) {
    var nn = UInt32(n), av = aa
    s.go(p, n) { e in
        e.setBuffer(x, offset: 0, index: 0); e.setBuffer(y, offset: 0, index: 1)
        e.setBytes(&av, length: 4, index: 2); e.setBytes(&nn, length: 4, index: 3)
    }
}
func fvals(_ b: MTLBuffer, _ n: Int) -> [Float] {
    let p = b.contents().bindMemory(to: Float.self, capacity: n)
    return (0..<n).map { p[$0] }
}

let embT = T("token_embd.weight"), onormT = T("output_norm.weight"), outT = T("output.weight")
struct Layer { let an, q, k, v, o, fn, gi, ge, gu, gd: TInfo }
let L: [Layer] = (0..<nLayer).map { l in
    Layer(an: T("blk.\(l).attn_norm.weight"), q: T("blk.\(l).attn_q.weight"),
          k: T("blk.\(l).attn_k.weight"), v: T("blk.\(l).attn_v.weight"),
          o: T("blk.\(l).attn_output.weight"), fn: T("blk.\(l).ffn_norm.weight"),
          gi: T("blk.\(l).ffn_gate_inp.weight"), ge: T("blk.\(l).ffn_gate_exps.weight"),
          gu: T("blk.\(l).ffn_up_exps.weight"), gd: T("blk.\(l).ffn_down_exps.weight"))
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
func route(_ s: Step) {
    var ne = UInt32(nExpert), ns = UInt32(nExpertUsed)
    s.go(pRoute, 1) { e in
        e.setBuffer(bRouter, offset: 0, index: 0)
        e.setBuffer(bIds, offset: 0, index: 1)
        e.setBuffer(bWts, offset: 0, index: 2)
        e.setBytes(&ne, length: 4, index: 3); e.setBytes(&ns, length: 4, index: 4)
    }
}

// ---- ONE forward pass at position `pos` for token `id` -> logits, then argmax --------------------
// THE COMMAND BUFFER IS CUT ONCE PER LAYER, at the router, and that seam is the honest price of
// closing the expert gather on the host side. It is counted, not hidden: `routeRoundTrips` below.
var routeRoundTrips = 0
var routeLog: [[Int]] = []
var routeWeightFaults = 0      // a routed layer whose chosen weights did not sum to 1
var routeDupFaults = 0         // a routed layer that "chose" the same expert twice
func forward(_ id: Int, _ pos: Int, logRoute: Bool = false) -> Int {
    var s = Step()
    dequantAt(s, type: embT.type, base: embT.off, off: id * dModel, n: dModel, bX)
    for l in 0..<nLayer {
        let kOff = (l * maxpos + pos) * kvd * 4
        rmsnorm(s, bX, L[l].an, bXb)
        matvec(s, L[l].q, bXb, bQ, barrier: false)
        matvec(s, L[l].k, bXb, bCacheK, yOff: kOff, barrier: false)
        matvec(s, L[l].v, bXb, bCacheV, yOff: kOff)
        rope(s, bQ, off: 0, heads: nHead, pos: pos, barrier: false)
        rope(s, bCacheK, off: kOff, heads: nKV, pos: pos)
        attn(s, l, pos)
        matvec(s, L[l].o, bAttn, bProj)
        elem(s, pAdd, bX, bProj, bX, dModel)
        rmsnorm(s, bX, L[l].fn, bXb)
        // the router gate (F16), then the routing decision — both on the GPU, both the body's kernels
        matvec(s, L[l].gi, bXb, bRouter)
        route(s)
        // THE SEAM. The ids must be a host number before the expert matvecs can be BOUND.
        s.done(); routeRoundTrips += 1
        let idp = bIds.contents().bindMemory(to: UInt32.self, capacity: nExpertUsed)
        let wtp = bWts.contents().bindMemory(to: Float.self, capacity: nExpertUsed)
        let chosen = (0..<nExpertUsed).map { Int(idp[$0]) }
        let weights = (0..<nExpertUsed).map { wtp[$0] }
        if logRoute { routeLog.append(chosen) }
        // THE LIVE VOICE CANARY, checked on every one of the routed layers rather than
        // once at the end: softmax-then-renormalize makes these weights sum to exactly 1
        // by construction, so a sum of 0 is not a bad route — it is no route.
        let wsum = weights.reduce(0) { $0 + $1 }
        if abs(Double(wsum) - 1.0) > 1e-4 { routeWeightFaults += 1 }
        if chosen.allSatisfy({ $0 == chosen[0] }) && nExpertUsed > 1 { routeDupFaults += 1 }
        s = Step()
        for k in 0..<nExpertUsed {
            let e = chosen[k]
            matvec(s, L[l].ge, bXb, bGate, barrier: false, expert: e)
            matvec(s, L[l].gu, bXb, bUp, expert: e)
            elem(s, pSwi, bGate, bUp, bAct, dFF)
            matvec(s, L[l].gd, bAct, bExpOut, expert: e)
            // the first chosen expert SETS the accumulator; the rest add into it. No host memset.
            axpyLike(s, k == 0 ? pScale : pAxpy, bExpOut, bFfn, weights[k], dModel)
        }
        elem(s, pAdd, bX, bFfn, bX, dModel)
    }
    rmsnorm(s, bX, onormT, bXb)
    matvec(s, outT, bXb, bLogits)          // NOT tied: this file has its own output.weight, 113 GiB in
    var nn = UInt32(vocabN)
    s.go(pArg, 1) { e in
        e.setBuffer(bLogits, offset: 0, index: 0); e.setBuffer(bOutI, offset: 0, index: 1)
        e.setBuffer(bOutV, offset: 0, index: 2); e.setBytes(&nn, length: 4, index: 3)
    }
    s.done()
    return Int(bOutI.contents().bindMemory(to: UInt32.self, capacity: 1)[0])
}

// ---- gates 4-8: the body judges the GPU at six points, before a token is asked for ----------------
let u: Double = 5.960464477539063e-08      // 2^-24, the f32 unit roundoff
func derivedBound(_ terms: [Double], _ cols: Int) -> Double {
    var s = 0.0; for t in terms { s += abs(t) }
    return Double(cols) * u * s
}
// ---- gate 0: DID THE GPU RUN. Asked first, because every gate below reads a buffer
// back, and a buffer that was never written reads as zeros — which is a NUMBER, and
// numbers get compared and reported as disagreement. The CPU writes a sentinel no
// kernel would ever produce; a kernel must overwrite it. If this fails, nothing below
// this line means anything and the harness says so instead of grading silence.
print("=== gate 0: did the GPU run at all ===")
do {
    let sentinel: Float = -424242.0
    let sp = bRef.contents().bindMemory(to: Float.self, capacity: 4)
    for i in 0..<4 { sp[i] = sentinel }
    let before = gpuErrors
    let s = Step()
    // the cheapest real kernel in the unit: dequant 4 weights of token_embd
    dequantAt(s, type: embT.type, base: embT.off, off: 0, n: 4, bRef)
    s.done()
    let after = fvals(bRef, 4)
    let untouched = after.allSatisfy { $0 == sentinel }
    if gpuErrors > before {
        print("  command buffer ERROR: \(gpuFirstError ?? "unknown")")
    }
    print("  sentinel \(sentinel) -> \(after)")
    check(gpuErrors == before && !untouched,
          "gate 0 the GPU executes: a kernel overwrote the CPU's sentinel, and no command buffer errored",
          "gate 0 THE GPU DID NOT RUN — the sentinel survived and/or a command buffer failed. Every gate below would grade an unwritten buffer as an arithmetic disagreement; they are not run.")
    if failures > 0 {
        print("")
        print("  This is a RESIDENCY failure, not an arithmetic one. This carrier asks Metal to")
        print("  make \(String(format: "%.2f", Double(dev.currentAllocatedSize)/1073741824.0)) GiB resident against a recommendedMaxWorkingSetSize of")
        print("  \(String(format: "%.2f", Double(recWS)/1073741824.0)) GiB. Whether that succeeds depends on what else holds memory on")
        print("  this machine RIGHT NOW — it is not a property of the code or of the model.")
        print("  Free memory and re-run; if it persists, lower FORM_VIEW_GIB (default 40).")
        print("=== VERDICT FAIL — the GPU did not run; no arithmetic was witnessed ===")
        exit(1)
    }
}

print("=== gates 4-8: the body judges the GPU, at every NEW thing ===")

// --- gate 4: the embedding gather, Q6_K, bit for bit ---
do {
    let s = Step(); dequantAt(s, type: embT.type, base: embT.off, off: refId * dModel, n: dModel, bX); s.done()
    let got = fvals(bX, dModel)
    var bad = 0
    for i in 0..<dModel where got[i] != Float(refEmb[i]) { bad += 1 }
    check(bad == 0 && refEmb.count == dModel,
          "gate 4 embedding gather: all \(dModel) weights of token_embd row \(refId) BIT-EXACT vs Form (Q6_K, one rounding each side)",
          "gate 4 embedding gather differs from Form at \(bad) of \(dModel) weights")
}
// --- the normed vector every remaining reference folds against ---
do {
    let s = Step(); rmsnorm(s, bX, L[0].an, bXb); s.done()
    let got = fvals(bXb, dModel)
    var worst = 0.0
    for i in 0..<dModel { worst = max(worst, abs(Double(got[i]) - refRms[i])) }
    let bound = Double(dModel) * u * refRms.map { abs($0) }.reduce(0, +) / Double(dModel) + 1e-6
    print(String(format: "  RMSNorm vs Form's fp64: worst |delta| %.3e (informational; the gated ops are below)", worst))
    _ = bound
}
// --- gate 5: Q8_0, the NEW carver, at the tensor's real width ---
do {
    let s = Step()
    matvecAt(s, type: L[0].k.type, base: L[0].k.off, rows: L[0].k.d1, cols: L[0].k.d0, bXb, bRef, force: .serial)
    s.done()
    let got = Double(fvals(bRef, L[0].k.d1)[refRow])
    // THE DERIVED BOUND: the GPU folds cols f32 terms right-to-left, so its worst case is
    // cols * u * SUM|w_j * x_j| — and SUM|term| is a number the BODY emitted (REFKABS), because the
    // carrier could only compute it by materializing the f32 row this lane exists to never build.
    let bound = Double(dModel) * u * refKAbs
    let delta = abs(got - refK)
    print("  Q8_0 matvec row \(refRow) at width \(dModel): "
          + String(format: "GPU %.9f  Form %.9f  |delta| %.3e  DERIVED bound cols*u*SUM|term| = %d*u*%.4f = %.3e (%.2f%% of it)",
                   got, refK, delta, dModel, refKAbs, bound, 100.0 * delta / bound))
    check(delta <= bound,
          "gate 5 a real Q8_0 fused matvec at width \(dModel) is the body's answer — THE CARVER THE BODY DID NOT HAVE",
          "gate 5 the Q8_0 matvec is outside the derived bound")
}
// --- gate 6: the router — the F16 gate, and the routing decision ---
do {
    let s = Step()
    matvecAt(s, type: L[0].gi.type, base: L[0].gi.off, rows: L[0].gi.d1, cols: L[0].gi.d0, bXb, bRouter)
    route(s)
    s.done()
    let got = fvals(bRouter, nExpert).map { Double($0) }
    var worstL = 0.0
    for e in 0..<nExpert { worstL = max(worstL, abs(got[e] - refGate[e])) }
    let idp = bIds.contents().bindMemory(to: UInt32.self, capacity: nExpertUsed)
    let wtp = bWts.contents().bindMemory(to: Float.self, capacity: nExpertUsed)
    let gotIds = (0..<nExpertUsed).map { Int(idp[$0]) }
    let gotW = (0..<nExpertUsed).map { Double(wtp[$0]) }
    var worstW = 0.0
    for k in 0..<nExpertUsed { worstW = max(worstW, abs(gotW[k] - refRouteW[k])) }
    print("  router logits: GPU \(got.map { String(format: "%.5f", $0) })")
    print("                 Form \(refGate.map { String(format: "%.5f", $0) })")
    print("  chosen experts: GPU \(gotIds) weights \(gotW.map { String(format: "%.6f", $0) })")
    print("                  Form \(refRouteIds) weights \(refRouteW.map { String(format: "%.6f", $0) })")
    check(gotIds == refRouteIds && worstW < 1e-5 && worstL < 1e-3,
          "gate 6 the F16 router gate reproduces all \(nExpert) of Form's fp64 logits and the route kernel picks the SAME top-\(nExpertUsed) with the same weights",
          "gate 6 the router disagrees with Form (worst logit delta \(worstL), worst weight delta \(worstW))")
}
// --- gate 7: THE EXPERT GATHER, judged at the 3-D offset ---
do {
    let t = L[0].ge
    let nb02 = t.len / t.d2
    let s = Step()
    matvecAt(s, type: t.type, base: t.off + refExpert * nb02, rows: t.d1, cols: t.d0, bXb, bRef, force: .serial)
    s.done()
    let got = Double(fvals(bRef, t.d1)[refRow])
    let bound = Double(dModel) * u * refExpAbs
    let delta = abs(got - refExp)
    print("  nb02: body's table says \(nb02), the reference emission said \(refNb02)")
    print("  expert \(refExpert), row \(refRow), width \(dModel): "
          + String(format: "GPU %.9f  Form %.9f  |delta| %.3e  DERIVED bound %.3e (%.2f%% of it)",
                   got, refExp, delta, bound, 100.0 * delta / bound))
    check(nb02 == refNb02 && delta <= bound,
          "gate 7 a matvec bound at t.off + \(refExpert)*nb02 is expert \(refExpert)'s OWN row — THE EXPERT GATHER, and it needed no kernel",
          "gate 7 the expert gather disagrees with Form")
}
// --- gate 8: THE FAR REGION, past the one-buffer ceiling ---
do {
    let far = outT.off
    print("  output.weight begins at \(far) B = "
          + String(format: "%.3f GiB — %.2f GiB PAST maxBufferLength",
                   Double(far)/1073741824.0, Double(far - maxBuf)/1073741824.0))
    let s = Step(); dequantAt(s, type: outT.type, base: far, off: 0, n: dModel, bRef); s.done()
    let got0 = fvals(bRef, dModel)
    var bad0 = 0
    for i in 0..<dModel where got0[i] != Float(refFar[i]) { bad0 += 1 }
    let lastRowStart = refFarRow * dModel
    let s2 = Step(); dequantAt(s2, type: outT.type, base: far, off: lastRowStart, n: dModel, bRef); s2.done()
    let got1 = fvals(bRef, dModel)
    var bad1 = 0
    for i in 0..<dModel where got1[i] != Float(refFarLast[i]) { bad1 += 1 }
    print("  row 0 (view \(viewOf(far))) mismatches: \(bad0) of \(dModel);  row \(refFarRow) (the LAST row, \(String(format: "%.3f", Double(far + outT.len)/1073741824.0)) GiB in) mismatches: \(bad1) of \(dModel)")
    check(bad0 == 0 && bad1 == 0 && refFar.count == dModel && refFarLast.count == dModel,
          "gate 8 BOTH ENDS of output.weight — 32.7 GiB past the one-buffer ceiling — are BIT-EXACT vs Form. THE `one` IS GONE.",
          "gate 8 the far region does not read back bit-exactly (\(bad0)/\(bad1) mismatches)")
}

// ---- gate 9: A TOKEN -------------------------------------------------------------------------------
print("=== gate 9: a token ===")
let ids0 = encode(prompt)
print("  prompt: \(prompt)")
print("  ids: \(ids0)")
print("  pieces: \(ids0.map { pieceOf[$0] ?? "?" })")
if ids0.count + nsteps > maxpos {
    print("SKIP  prompt (\(ids0.count)) + \(nsteps) steps exceeds maxpos \(maxpos)")
    exit(2)
}

func generate(_ n: Int, path: MVPath, logRoute: Bool = false) -> ([Int], Double, Double) {
    mvNow = path
    memset(bCacheK.contents(), 0, bCacheK.length)
    memset(bCacheV.contents(), 0, bCacheV.length)
    routeLog.removeAll()
    var ids = ids0, out: [Int] = []
    let t0 = Date()
    var cur = -1
    for p in 0..<(ids.count) { cur = forward(ids[p], p, logRoute: logRoute) }
    let tPre = Date().timeIntervalSince(t0)
    let t1 = Date()
    var pos = ids.count
    for _ in 0..<n {
        out.append(cur)
        if cur == eosId { break }
        ids.append(cur)
        cur = forward(cur, pos, logRoute: logRoute); pos += 1
    }
    let tDec = Date().timeIntervalSince(t1)
    return (out, tPre, tDec)
}

let t0all = Date()
let (outIds, tPre, tDec) = generate(nsteps, path: mvNow, logRoute: true)
let tAll = Date().timeIntervalSince(t0all)
print("  GENERATED TOKEN IDS: \(outIds)")
print("  pieces: \(outIds.map { pieceOf[$0] ?? "?" })")
print("  text  : \(decodeIds(outIds, stripLeadingSpace: false))")
print(String(format: "  prefill %d tokens in %.2f s (%.3f tok/s) | decode %d tokens in %.2f s (%.3f tok/s) | end-to-end %.3f tok/s",
             ids0.count, tPre, Double(ids0.count)/tPre, outIds.count, tDec, Double(outIds.count)/tDec,
             Double(outIds.count)/tAll))
let perFwdPrefill = tPre / Double(ids0.count)
let perFwdDecode = tDec / Double(outIds.count)
print(String(format: "  TWO COUNTS OF THE SAME OP: %.3f s per forward over %d prefill forwards, %.3f s per forward over %d decode forwards (%.1f%% apart)",
             perFwdPrefill, ids0.count, perFwdDecode, outIds.count,
             100.0 * abs(perFwdPrefill - perFwdDecode) / max(perFwdPrefill, perFwdDecode)))
print("  DENOMINATORS (selfgauge — never an x without naming what it is against):")
print(String(format: "    the model is %.3f GiB; a decode touches ~%.1f GiB of it (attn %.2f + 2 of 8 experts %.2f, x %d layers, + output)",
             Double(fileLen)/1073741824.0,
             Double(nLayer) * (Double(L[0].q.len + L[0].o.len + L[0].k.len + L[0].v.len)
                               + 2.0 * Double(L[0].ge.len + L[0].gu.len + L[0].gd.len) / Double(nExpert))/1073741824.0
             + Double(outT.len)/1073741824.0,
             Double(L[0].q.len + L[0].o.len + L[0].k.len + L[0].v.len)/1073741824.0,
             2.0 * Double(L[0].ge.len + L[0].gu.len + L[0].gd.len) / Double(nExpert)/1073741824.0,
             nLayer))
let touchedGiB = Double(nLayer) * (Double(L[0].q.len + L[0].o.len + L[0].k.len + L[0].v.len)
                  + 2.0 * Double(L[0].ge.len + L[0].gu.len + L[0].gd.len) / Double(nExpert))/1073741824.0
                  + Double(outT.len)/1073741824.0
print(String(format: "    so one forward at %.2f s is %.2f GiB/s of QUANTIZED weight actually read — against this machine, not against a claim",
             perFwdDecode, touchedGiB / perFwdDecode))
print("  router round trips (the honest price of the host-side gather): \(routeRoundTrips) = \(nLayer) layers x \(ids0.count + outIds.count - 1) forwards")
if !routeLog.isEmpty {
    print("  experts chosen, layer 0..7 of the first forward: \(routeLog.prefix(8).map { $0 })")
    var counts = [Int](repeating: 0, count: nExpert)
    for r in routeLog { for e in r { counts[e] += 1 } }
    print("  expert usage over all \(routeLog.count) routed layers: \(counts)  <- a MoE that used one expert would show it here")
}
// GATE 9, REBUILT. "legal vocab indices" is satisfied by [0,0,0,0], so the old form of
// this gate could not distinguish a working 141 B model from a GPU that wrote nothing —
// an aporon (corpus row 826) sitting under the headline claim. Four witnesses now, and
// every one of them is FALSE for a degenerate run:
//   a  legal indices                     (the old, necessary-but-far-from-sufficient one)
//   b  the ids are not all the same token (all-<unk> fails; so does any constant)
//   c  the argmax's own logit VALUE is non-zero (an all-zero logits buffer has argmax 0
//      with value 0 — that is the exact signature, so it is named and refused)
//   d  the router's chosen weights summed to 1 at ALL \(routeRoundTrips) routed layers, and
//      no layer "chose" the same expert twice. This is the strongest of the four because
//      it is checked live, 560 times, deep inside the run — not once at the end.
let legal = outIds.allSatisfy { $0 >= 0 && $0 < vocabN }
let varied = Set(outIds).count > 1
let argmaxVal = fvals(bOutV, 1)[0]
let routerHealthy = routeWeightFaults == 0 && routeDupFaults == 0 && routeRoundTrips > 0
print("  witnesses: legal \(legal) | non-constant \(varied) | final argmax logit \(argmaxVal) | router faults \(routeWeightFaults) weight / \(routeDupFaults) duplicate over \(routeRoundTrips) routed layers | command-buffer errors \(gpuErrors)")
check(!outIds.isEmpty && legal && varied && argmaxVal != 0.0 && routerHealthy && gpuErrors == 0,
      "gate 9 real token ids out of a 141 B Mixture-of-Experts: legal, NON-CONSTANT, a non-zero winning logit, and a router that summed to 1 at all \(routeRoundTrips) routed layers",
      "gate 9 the output is degenerate or the GPU did not fully run (legal \(legal), non-constant \(varied), argmax value \(argmaxVal), router faults \(routeWeightFaults)/\(routeDupFaults), cb errors \(gpuErrors))")

// ---- residency, measured after the fact ------------------------------------------------------------
print("=== residency, measured ===")
print("  \(views.count) views over one mmap of \(mapLen) B; the GPU never received a host copy of any tensor")
print(String(format: "  device currentAllocatedSize %.3f GiB", Double(dev.currentAllocatedSize)/1073741824.0))

if gpuErrors > 0 {
    print("=== \(gpuErrors) COMMAND BUFFER(S) FAILED during this run — first: \(gpuFirstError ?? "unknown") ===")
    print("    Nothing above this line that reads a buffer back can be trusted.")
}
if failures == 0 { print("=== VERDICT PASS — 10 gates ===") } else { print("=== VERDICT FAIL — \(failures) gate(s) ===") }
exit(failures == 0 ? 0 : 1)
SWIFT

SWIFTBIN="$CACHE/moe-runner-$(shasum -a 256 "$work/runner.swift" | cut -c1-16)"
if [[ ! -x "$SWIFTBIN" ]]; then
    echo "  compiling the carrier..."
    swiftc -O "$work/runner.swift" -o "$SWIFTBIN" 2>"$work/swift.err" || {
        echo "FAIL  swiftc failed"; cat "$work/swift.err"; exit 1; }
fi

"$SWIFTBIN" "$LIB" "$BLOB" "$TBL" "$CFG" "$VOC" "$REF" "$PROMPT" "$NSTEPS" "$MAXPOS" "$REFID" "$REFROW" "$REFEXP"
rc=$?
exit $rc
