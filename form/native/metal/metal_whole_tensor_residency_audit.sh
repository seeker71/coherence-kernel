#!/usr/bin/env bash
# metal_whole_tensor_residency_audit.sh — GPU_GAPS §C beyond one tile, made a witness.
#
# The claim, and nothing wider: the WHOLE of llama3.2:3b lives on the GPU in its QUANTIZED form —
# every one of its 3 212 749 888 weights, all 28 layers, 2 011 539 712 bytes — mapped once and never
# copied; and a Metal kernel the BODY emitted (q6k-msl.fk) dequantizes Q6_K weights out of those
# resident bytes, bit-exactly equal to Form's own dequant, at BOTH ends of a 25 165 824-weight tensor.
#
# WHY QUANTIZED AND NOT F32 — the measurement that chose this path, reproduced by the harness at the
# bottom of this file. Stone 2 reported Form dequant at 18 460 weights/s and projected ~23 min for one
# tensor. Re-measuring at two sizes instead of one shows that is not a rate but a curve, and the curve
# is the OUTPUT list, not the arithmetic: ewl-weights takes 4.98 s at 65 536 weights and 39.5 s at
# 262 144 (7.9x for 4x n — superlinear), while the same dequant as a fold with no list is linear
# (0.70 / 1.50 / 6.45 s at 65 536 / 262 144 / 1 048 576 = ~162k w/s). Honest projections for ONE
# 25.2M-weight tensor: ~10.4 h through the list, ~154 s through the fold. Neither is payable per run,
# and neither has to be: the quantized bytes are 1/4 the size and the dequant is 8 integer ops.
#
# Who decides what (the dumb-carrier discipline the Metal/PTX lanes keep):
#   the BODY  native/metal/whole-tensor-residency.fk — where all 255 tensors are (ONE header walk),
#             what the weights ARE at any flat offset, and what a row's dot should be (fp64).
#   the BODY  form-stdlib/q6k-msl.fk — the Metal source. Not one character is authored here.
#   the CARRIER (this file + the Swift runner it writes) — mmap, bind, dispatch, compare.
#
# THE GATES:
#   1  DEQUANT IS THE BODY'S, AT THE HEAD   GPU-dequantized weights 0..4095 equal Form's, bit for bit.
#   2  ... AND AT THE TAIL                  the LAST 4096 weights of the tensor, likewise. A tile audit
#                                           that only looks at superblock 0 is correct exactly where it
#                                           looked (corpus row 811, aporon); this one looks at both ends
#                                           of 98 304 superblocks.
#   3  THE WHOLE TENSOR DEQUANTS            all 25 165 824 weights in one dispatch, and the head and
#                                           tail tiles read back OUT of that whole-tensor result still
#                                           equal Form's — so the dispatch covered what it claims.
#   4  THE FUSED KERNEL IS THE SAME MEANING q6k-matvec (quantized-resident, dequant inside the dot)
#                                           equals an f32 right-fold over the dequantized buffer, all
#                                           3072 rows, bit-exact.
#   5  IT IS THE BODY'S ANSWER              sampled rows match Form's fp64 dot at the tensor's REAL
#                                           width (8192) within the derived cols*2^-24*SUM|term| bound.
#   6  RESIDENCY IS REAL                    ITERS dispatches, zero re-uploads, checksum stable.
#   7  MULTI-LAYER                          all 28 blk.N.ffn_down tensors dispatched from ONE resident
#                                           model buffer by offset — no per-layer upload exists.
#   8  THE LIBRARY IS CACHED ACROSS RUNS    the emitted MSL is compiled to a .metallib keyed by its own
#                                           sha256; a second run loads it instead of compiling.
#   9  KV-CACHE + WORKSPACE ARE POOLED      real llama3.2:3b KV geometry allocated ONCE, written across
#                                           many steps with zero reallocation, contents verified.
#
# Run:  form/native/metal/metal_whole_tensor_residency_audit.sh [iters] [steps]     (defaults 200 64)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every other Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
ITERS="${1:-200}"; STEPS="${2:-64}"
BLOB="${FORM_GGUF_BLOB:-$HOME/.ollama/models/blobs/sha256-dde5aa3fc5ffc17176b5e8bdc82f587b24b2678c6c66101bf7da77af9f7ccdff}"
WTENSOR="${FORM_W_TENSOR:-blk.0.ffn_down.weight}"
XTENSOR="${FORM_X_TENSOR:-blk.1.ffn_down.weight}"
TILE="${FORM_TILE:-4096}"
CACHE="$ROOT/native/metal/.metallib-cache"

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

work="$(mktemp -d "${TMPDIR:-/tmp}/fkwhole.XXXXXX")"
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
while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/whole-tensor-residency.fk)

# ── 1. the body emits the Metal source (both kernels, one helper spine) ────────────────────────
echo '(wtr-emit-msl)' > "$work/msl.fk"
"$GO_BIN" "${FILES[@]}" "$work/msl.fk" > "$work/msl.out" 2>"$work/msl.err" || {
    echo "FAIL  MSL emission failed"; cat "$work/msl.err"; exit 1; }
awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/msl.out" > "$work/q6k.metal"
for k in form_q6k_dequant_f32 form_q6k_matvec_f32 form_q4k_dequant_f32 form_q4k_matvec_f32; do
    grep -q "kernel void $k" "$work/q6k.metal" || { echo "FAIL  kernel $k was not emitted"; exit 1; }
done
echo "emitted MSL: $(wc -c < "$work/q6k.metal" | tr -d ' ') bytes, 4 kernels, every byte authored by q6k-msl.fk + q4k-msl.fk"

# ── 2. gate 8: the .metallib, cached across RUNS by the source's own sha256 ────────────────────
mkdir -p "$CACHE"
msl_sha="$(shasum -a 256 "$work/q6k.metal" | cut -c1-16)"
LIB="$CACHE/qk-$msl_sha.metallib"
if [[ -f "$LIB" ]]; then
    echo "PASS  gate 8 metallib cache HIT: $LIB (emitted source unchanged; no compile this run)"
    lib_state=hit
else
    t0=$(python3 -c 'import time;print(time.time())')
    xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
          -c "$work/q6k.metal" -o "$work/q6k.air" 2>"$work/metal.err" \
      && xcrun -sdk macosx metallib "$work/q6k.air" -o "$LIB" 2>>"$work/metal.err" || {
        echo "FAIL  offline metal compile failed"; cat "$work/metal.err"; exit 1; }
    t1=$(python3 -c 'import time;print(time.time())')
    printf 'PASS  gate 8 metallib cache MISS -> compiled in %.2f s and cached: %s\n' \
        "$(python3 -c "print($t1-$t0)")" "$LIB"
    lib_state=miss
fi

# ── 3. the body: one header walk, all 255 tensors ─────────────────────────────────────────────
echo "walking the file's own 7.8 MB header ONCE for all 255 tensor-info rows..."
printf '(wtr-emit-table "%s")\n' "$BLOB" > "$work/table.fk"
tw0=$(date +%s)
"$GO_BIN" "${FILES[@]}" "$work/table.fk" > "$work/table.txt" 2>"$work/table.err" || {
    echo "FAIL  table emission failed"; tail -5 "$work/table.err"; exit 1; }
tw1=$(date +%s)
grep -qx 'END' "$work/table.txt" || { echo "FAIL  table stream truncated"; exit 1; }
ntensors=$(grep -c '^T ' "$work/table.txt")
echo "  $ntensors tensor rows in $((tw1-tw0)) s (the per-tensor egg-tensor-abs path costs ~10 s EACH)"

row_of() { awk -v n="$1" '$1=="T" && $2==n {print; exit}' "$work/table.txt"; }
pick() { awk -v n="$1" '$1=="T" && $2==n {print; exit}' "$work/table.txt"; }
W6=($(pick "blk.0.ffn_down.weight")); X6=($(pick "blk.1.ffn_down.weight"))
W4=($(pick "blk.0.ffn_gate.weight")); X4=($(pick "blk.0.attn_norm.weight"))
for r in W6 X6 W4 X4; do
    eval "n=\${#$r[@]}"
    [[ "$n" -eq 8 ]] || { echo "FAIL  a required tensor is missing from the table ($r)"; exit 1; }
done
[[ "${W6[2]}" == 14 ]] || { echo "FAIL  ${W6[1]} is ggml type ${W6[2]}, not Q6_K(14)"; exit 1; }
[[ "${W4[2]}" == 12 ]] || { echo "FAIL  ${W4[1]} is ggml type ${W4[2]}, not Q4_K(12)"; exit 1; }
[[ "${X4[2]}" == 0  ]] || { echo "FAIL  ${X4[1]} is ggml type ${X4[2]}, not F32(0)"; exit 1; }
# GGUF stores dim0 as the fastest-varying axis: the matrix is d1 rows of d0 columns
R6=${W6[5]}; C6=${W6[4]}; A6=${W6[6]}; T6=$(( R6*C6 - TILE ))
R4=${W4[5]}; C4=${W4[4]}; A4=${W4[6]}; T4=$(( R4*C4 - TILE ))
DATABASE=$(awk '$1=="DATABASE"{print $2}' "$work/table.txt")
echo "  Q6_K ${W6[1]}: ${R6}x${C6} = $((R6*C6)) weights, ${W6[7]} bytes at $A6"
echo "  Q4_K ${W4[1]}: ${R4}x${C4} = $((R4*C4)) weights, ${W4[7]} bytes at $A4"

# ── 4. the body: reference tiles at BOTH ends of BOTH tensors, real x, and fp64 rows ───────────
echo "dequantizing reference tiles at both ends of both tensors, real x, and 4 fp64 rows per lane..."
{
  printf '(do (wtr-line "LANE Q6_K")\n'
  printf '    (wtr-emit-tile 14 "%s" %s 0 %s)\n' "$BLOB" "$A6" "$TILE"
  printf '    (wtr-emit-tile 14 "%s" %s %s %s)\n' "$BLOB" "$A6" "$T6" "$TILE"
  printf '    (wtr-emit-x 14 "%s" %s %s)\n' "$BLOB" "${X6[6]}" "$C6"
  printf '    (wtr-emit-rows 14 "%s" %s %s 0 4 %s 0)\n' "$BLOB" "$A6" "${X6[6]}" "$C6"
  printf '    (wtr-line "LANE Q4_K")\n'
  printf '    (wtr-emit-tile 12 "%s" %s 0 %s)\n' "$BLOB" "$A4" "$TILE"
  printf '    (wtr-emit-tile 12 "%s" %s %s %s)\n' "$BLOB" "$A4" "$T4" "$TILE"
  printf '    (wtr-emit-x-f32 "%s" %s %s)\n' "$BLOB" "${X4[6]}" "$C4"
  printf '    (wtr-emit-rows 12 "%s" %s %s 0 4 %s 1)\n' "$BLOB" "$A4" "${X4[6]}" "$C4"
  printf '    (wtr-line "END"))\n'
} > "$work/ref.fk"
rf0=$(date +%s)
"$GO_BIN" "${FILES[@]}" "$work/ref.fk" > "$work/ref.txt" 2>"$work/ref.err" || {
    echo "FAIL  reference emission failed"; tail -5 "$work/ref.err"; exit 1; }
rf1=$(date +%s)
grep -qx 'END' "$work/ref.txt" || { echo "FAIL  reference stream truncated"; exit 1; }
echo "  body reference time: $((rf1-rf0)) s"

# ── 5. the carrier ────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
// Whole-tensor / whole-MODEL residency witness. Carrier only: every number it judges came from the
// body, and every number it computes itself is the LANE's own conversion chain (f32 right fold, j
// counting DOWN, mul then add as two roundings) — the same op order the Form recipes emitted.
import Metal
import Foundation

let a = CommandLine.arguments
let libPath = a[1], refPath = a[2], blobPath = a[3], tablePath = a[4]
let iters = Int(a[5])!, steps = Int(a[6])!, tile = Int(a[7])!
struct Lane {
    let quant: String, abs: Int, rows: Int, cols: Int, tailOff: Int
    let deq: String, mv: String, bitExact: Bool
}
let lanes = [
    Lane(quant: "Q6_K", abs: Int(a[8])!,  rows: Int(a[9])!,  cols: Int(a[10])!, tailOff: Int(a[11])!,
         deq: "form_q6k_dequant_f32", mv: "form_q6k_matvec_f32", bitExact: true),
    Lane(quant: "Q4_K", abs: Int(a[12])!, rows: Int(a[13])!, cols: Int(a[14])!, tailOff: Int(a[15])!,
         deq: "form_q4k_dequant_f32", mv: "form_q4k_matvec_f32", bitExact: false),
]

// --- the body's stream, one section per lane ------------------------------------------------------
struct Ref { var tiles: [Int: [Double]] = [:]; var x: [Double] = []; var rows: [Int: Double] = [:] }
var refs: [String: Ref] = [:]
do {
    var lane = "", section = "", tileOff = -1, pendingRow = -1
    for line in try String(contentsOfFile: refPath, encoding: .utf8).split(separator: "\n", omittingEmptySubsequences: false) {
        let s = String(line)
        if s.hasPrefix("LANE ") { lane = String(s.dropFirst(5)); refs[lane] = Ref(); section = ""; continue }
        if s.hasPrefix("TILE ") { tileOff = Int(s.split(separator: " ")[1])!; refs[lane]!.tiles[tileOff] = []; section = "T"; continue }
        if s.hasPrefix("X ")    { section = "X"; continue }
        if s.hasPrefix("ROW ")  { pendingRow = Int(s.split(separator: " ")[1])!; section = "R"; continue }
        if s == "END" { continue }
        guard let v = Double(s) else { continue }
        switch section {
        case "T": refs[lane]!.tiles[tileOff]!.append(v)
        case "X": refs[lane]!.x.append(v)
        case "R": refs[lane]!.rows[pendingRow] = v; section = ""
        default: break
        }
    }
}

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
let queue = dev.makeCommandQueue()!
var failures = 0
func check(_ ok: Bool, _ pass: String, _ fail: String) {
    if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 }
}

// --- THE WHOLE MODEL, RESIDENT. mmap the blob and hand the GPU the mapped pages themselves: with
//     bytesNoCopy there is no copy at all, so "upload" is a page-table fact, not a memcpy. ---------
let fd = open(blobPath, O_RDONLY)
guard fd >= 0 else { print("FAIL  cannot open blob"); exit(1) }
var st = stat(); fstat(fd, &st)
let fileLen = Int(st.st_size)
let page = Int(getpagesize())
let mapLen = (fileLen + page - 1) / page * page
guard let mapped = mmap(nil, mapLen, PROT_READ, MAP_PRIVATE, fd, 0), mapped != MAP_FAILED else {
    print("FAIL  mmap failed"); exit(1)
}
let tUp0 = Date()
guard let modelBuf = dev.makeBuffer(bytesNoCopy: mapped, length: mapLen, options: .storageModeShared, deallocator: nil) else {
    print("FAIL  makeBuffer(bytesNoCopy:) over the mapped model failed"); exit(1)
}
let upSecs = Date().timeIntervalSince(tUp0)
print(String(format: "resident: the WHOLE %d-byte blob mapped into one MTLBuffer on %@ in %.4f s, ZERO copies",
             fileLen, dev.name, upSecs))

func pipeline(_ n: String) throws -> MTLComputePipelineState {
    try dev.makeComputePipelineState(function: lib.makeFunction(name: n)!)
}
func dispatch(_ p: MTLComputePipelineState, width: Int, _ bind: (MTLComputeCommandEncoder) -> Void) {
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(p)
    bind(enc)
    let tg = min(p.maxTotalThreadsPerThreadgroup, 256)
    enc.dispatchThreads(MTLSize(width: width, height: 1, depth: 1),
                        threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
}

let u = 5.960464477539063e-08   // 2^-24, the f32 unit roundoff

for lane in lanes {
    guard let ref = refs[lane.quant], let head = ref.tiles[0], let tail = ref.tiles[lane.tailOff],
          head.count == tile, tail.count == tile, ref.x.count == lane.cols else {
        print("FAIL  \(lane.quant) reference stream shape"); failures += 1; continue
    }
    print("--- \(lane.quant): \(lane.rows)x\(lane.cols) = \(lane.rows * lane.cols) weights at file offset \(lane.abs)")
    let nw = lane.rows * lane.cols
    let pDeq = try pipeline(lane.deq), pMv = try pipeline(lane.mv)
    let Xf = ref.x.map { Float($0) }

    func dequant(off: Int, n: Int, into out: MTLBuffer) {
        var o32 = UInt32(off), n32 = UInt32(n)
        dispatch(pDeq, width: n) { enc in
            enc.setBuffer(modelBuf, offset: lane.abs, index: 0)   // the resident model, by offset
            enc.setBuffer(out, offset: 0, index: 1)
            enc.setBytes(&o32, length: 4, index: 2)
            enc.setBytes(&n32, length: 4, index: 3)
        }
    }

    // --- GATES 1 and 2: the head tile and the TAIL tile, against Form ------------------------------
    // Q6_K: w = (d*sc)*q needs <= 25 significand bits, so one f32 rounding on each side of the same
    // exact real number is the SAME f32 — equality is the honest gate. Q4_K ends in a subtraction of
    // two products and cancellation can eat bits, so its gate is the derived ONE-ROUNDING bound
    // |gpu - form| <= u*|form|. Claiming bit-exactness there would be a lie that usually passes.
    let tileBuf = dev.makeBuffer(length: tile * 4, options: .storageModeShared)!
    func tileBad(_ off: Int, _ r: [Double]) -> Int {
        dequant(off: off, n: tile, into: tileBuf)
        let g = tileBuf.contents().bindMemory(to: Float.self, capacity: tile)
        var bad = 0
        for i in 0..<tile {
            if lane.bitExact { if g[i].bitPattern != Float(r[i]).bitPattern { bad += 1 } }
            else if abs(Double(g[i]) - r[i]) > u * abs(r[i]) { bad += 1 }
        }
        return bad
    }
    let how = lane.bitExact ? "bit-exact" : "within the derived u*|w| one-rounding bound"
    let badHead = tileBad(0, head)
    check(badHead == 0,
          "gate 1 \(lane.quant) head tile \(how): all \(tile) GPU-dequantized weights at flat 0 equal Form's",
          "gate 1 \(lane.quant): \(badHead) of \(tile) head weights differ from Form's")
    let badTail = tileBad(lane.tailOff, tail)
    check(badTail == 0,
          "gate 2 \(lane.quant) TAIL tile \(how): all \(tile) weights at flat \(lane.tailOff) (superblock \(lane.tailOff/256) of \(nw/256)) equal Form's",
          "gate 2 \(lane.quant): \(badTail) of \(tile) tail weights differ from Form's")

    // --- GATE 3: the WHOLE tensor in one dispatch, then read the same two ends back out of it -----
    let wf32 = dev.makeBuffer(length: nw * 4, options: .storageModeShared)!
    let tD0 = Date()
    dequant(off: 0, n: nw, into: wf32)
    let dqSecs = Date().timeIntervalSince(tD0)
    let wg = wf32.contents().bindMemory(to: Float.self, capacity: nw)
    var badWhole = 0
    for i in 0..<tile {
        if lane.bitExact {
            if wg[i].bitPattern != Float(head[i]).bitPattern { badWhole += 1 }
            if wg[lane.tailOff + i].bitPattern != Float(tail[i]).bitPattern { badWhole += 1 }
        } else {
            if abs(Double(wg[i]) - head[i]) > u * abs(head[i]) { badWhole += 1 }
            if abs(Double(wg[lane.tailOff + i]) - tail[i]) > u * abs(tail[i]) { badWhole += 1 }
        }
    }
    check(badWhole == 0,
          String(format: "gate 3 \(lane.quant) whole tensor: all %d weights dequantized in ONE dispatch in %.4f s (%.1fM weights/s); head AND tail read back out of it still equal Form's",
                 nw, dqSecs, Double(nw) / dqSecs / 1e6),
          "gate 3 \(lane.quant): \(badWhole) weights of the whole-tensor dequant differ from Form's")

    // --- GATE 4: the FUSED kernel — quantized-resident, dequant inside the dot ---------------------
    let xbuf = dev.makeBuffer(bytes: Xf, length: lane.cols * 4, options: .storageModeShared)!
    let ybuf = dev.makeBuffer(length: lane.rows * 4, options: .storageModeShared)!
    var r32 = UInt32(lane.rows), c32 = UInt32(lane.cols)
    func matvec(tensorAbs: Int) {
        dispatch(pMv, width: lane.rows) { enc in
            enc.setBuffer(modelBuf, offset: tensorAbs, index: 0)
            enc.setBuffer(xbuf, offset: 0, index: 1)
            enc.setBuffer(ybuf, offset: 0, index: 2)
            enc.setBytes(&r32, length: 4, index: 3)
            enc.setBytes(&c32, length: 4, index: 4)
        }
    }
    let tM0 = Date(); matvec(tensorAbs: lane.abs); let mvSecs = Date().timeIntervalSince(tM0)
    let y = ybuf.contents().bindMemory(to: Float.self, capacity: lane.rows)
    var gpu = [Float](repeating: 0, count: lane.rows)
    for i in 0..<lane.rows { gpu[i] = y[i] }
    var badFused = 0, firstFused = -1
    for i in 0..<lane.rows {
        var acc: Float = 0.0
        var j = lane.cols
        while j > 0 { j -= 1; let p = wg[i * lane.cols + j] * Xf[j]; acc = p + acc }
        if acc.bitPattern != gpu[i].bitPattern { badFused += 1; if firstFused < 0 { firstFused = i } }
    }
    check(badFused == 0,
          String(format: "gate 4 \(lane.quant) fused kernel bit-exact: all %d rows of the QUANTIZED-resident matvec equal the f32 right-fold over the dequantized buffer (%.1f ms, %.2f GMAC/s)",
                 lane.rows, mvSecs * 1e3, Double(nw) / mvSecs / 1e9),
          "gate 4 \(lane.quant): \(badFused) rows differ, first at row \(firstFused)")

    // --- GATE 5: Form's fp64 rows, within the derived fp32 summation bound -------------------------
    // |fl(S) - S| <= cols * u * SUM|term|. A tolerance pulled out of the air would be a fudge.
    var worstRatio = 0.0, worstRel = 0.0, worstCond = 0.0, checked = 0
    for (r, refv) in ref.rows.sorted(by: { $0.key < $1.key }) {
        let got = Double(gpu[r])
        var absSum = 0.0
        for j in 0..<lane.cols { absSum += abs(Double(wg[r * lane.cols + j]) * Double(Xf[j])) }
        let bound = Double(lane.cols) * u * absSum
        if bound > 0 { worstRatio = max(worstRatio, abs(got - refv) / bound) }
        worstRel = max(worstRel, abs(got - refv) / max(abs(refv), 1e-30))
        worstCond = max(worstCond, absSum / max(abs(refv), 1e-30))
        checked += 1
    }
    print(String(format: "      value-relative max %.3e over %d fp64 reference rows (worst row condition number %.1f — cancellation, not error)", worstRel, checked, worstCond))
    check(checked > 0 && worstRatio < 1.0,
          String(format: "gate 5 \(lane.quant) derived bound: max |gpu-form| is %.3f of the cols*u*SUM|term| fp32 bound at the tensor's real width (cols=%d)", worstRatio, lane.cols),
          String(format: "gate 5 \(lane.quant): max |gpu-form| is %.3f of the fp32 summation bound", worstRatio))

    // --- GATE 6: residency is real ------------------------------------------------------------------
    var sumFirst = 0.0; for i in 0..<lane.rows { sumFirst += Double(gpu[i]) }
    let tR0 = Date()
    for _ in 0..<iters { matvec(tensorAbs: lane.abs) }
    let rSecs = Date().timeIntervalSince(tR0)
    var sumAfter = 0.0; for i in 0..<lane.rows { sumAfter += Double(y[i]) }
    check(sumAfter == sumFirst,
          String(format: "gate 6 \(lane.quant) residency: %d fused dispatches in %.4f s (%.0f us each) with ZERO re-uploads; checksum unchanged", iters, rSecs, rSecs / Double(iters) * 1e6),
          "gate 6 \(lane.quant): the output checksum changed across dispatches")
}

// --- GATE 7: MULTI-LAYER. Every blk.N.ffn_down of BOTH quants, from the ONE resident buffer -------
// llama3.2:3b is mixed-quant: 14 of its 28 ffn_down tensors are Q6_K and 14 are Q4_K. A Q6_K-only
// lane dispatches exactly half of them, which is how this gate found the Q4_K gap in the first place.
struct Tensor { let name: String; let type: String; let abs: Int; let rows: Int; let cols: Int }
var layers: [Tensor] = []
var modelBytes = 0, q6Bytes = 0, q4Bytes = 0
for line in try String(contentsOfFile: tablePath, encoding: .utf8).split(separator: "\n") {
    let f = line.split(separator: " ").map(String.init)
    guard f.count == 8, f[0] == "T" else { continue }
    modelBytes += Int(f[7])!
    if f[2] == "14" { q6Bytes += Int(f[7])! }
    if f[2] == "12" { q4Bytes += Int(f[7])! }
    guard f[1].hasPrefix("blk."), f[1].hasSuffix(".ffn_down.weight") else { continue }
    layers.append(Tensor(name: f[1], type: f[2], abs: Int(f[6])!, rows: Int(f[5])!, cols: Int(f[4])!))
}
do {
    let pipes = ["14": try pipeline("form_q6k_matvec_f32"), "12": try pipeline("form_q4k_matvec_f32")]
    var distinct = Set<UInt32>(), finite = true, dispatched = 0
    let tL0 = Date()
    for t in layers {
        guard let p = pipes[t.type] else { continue }
        let xb = dev.makeBuffer(length: t.cols * 4, options: .storageModeShared)!
        let xp = xb.contents().bindMemory(to: Float.self, capacity: t.cols)
        for j in 0..<t.cols { xp[j] = Float(j % 17) * 0.125 - 1.0 }
        let yb = dev.makeBuffer(length: t.rows * 4, options: .storageModeShared)!
        var r32 = UInt32(t.rows), c32 = UInt32(t.cols)
        dispatch(p, width: t.rows) { enc in
            enc.setBuffer(modelBuf, offset: t.abs, index: 0)      // no upload — already resident
            enc.setBuffer(xb, offset: 0, index: 1)
            enc.setBuffer(yb, offset: 0, index: 2)
            enc.setBytes(&r32, length: 4, index: 3)
            enc.setBytes(&c32, length: 4, index: 4)
        }
        let yp = yb.contents().bindMemory(to: Float.self, capacity: t.rows)
        var s: Float = 0; for i in 0..<t.rows { s += yp[i] }
        if !s.isFinite { finite = false }
        distinct.insert(s.bitPattern); dispatched += 1
    }
    let lSecs = Date().timeIntervalSince(tL0)
    check(finite && dispatched == layers.count && distinct.count == layers.count,
          String(format: "gate 7 multi-layer: all %d blk.N.ffn_down tensors across BOTH quants dispatched from ONE resident buffer in %.4f s — %d distinct finite checksums, zero per-layer uploads (model resident: %.2f GB total, %.2f GB Q6_K + %.2f GB Q4_K)",
                 layers.count, lSecs, distinct.count, Double(modelBytes)/1e9, Double(q6Bytes)/1e9, Double(q4Bytes)/1e9),
          "gate 7: dispatched \(dispatched) of \(layers.count) layers, \(distinct.count) distinct checksums")
}

// --- GATE 9: KV-cache device buffers + workspace pooling ------------------------------------------
// Real llama3.2:3b decode geometry: 28 layers, 8 KV heads, head_dim 128, K and V, f32.
do {
    let nLayers = 28, nKVHeads = 8, headDim = 128, maxSeq = 2048
    let kvFloats = nLayers * nKVHeads * headDim * maxSeq * 2
    let kvBuf = dev.makeBuffer(length: kvFloats * 4, options: .storageModeShared)!
    let wsBuf = dev.makeBuffer(length: 8 * 8192 * 4, options: .storageModeShared)!   // scratch pool
    let allocations = 2
    let kv = kvBuf.contents().bindMemory(to: Float.self, capacity: kvFloats)
    let ws = wsBuf.contents().bindMemory(to: Float.self, capacity: 8 * 8192)
    let p6 = try pipeline("form_q6k_matvec_f32")
    let t = layers.first(where: { $0.type == "14" })!
    let xb = dev.makeBuffer(length: t.cols * 4, options: .storageModeShared)!
    let yb = dev.makeBuffer(length: t.rows * 4, options: .storageModeShared)!
    let xp = xb.contents().bindMemory(to: Float.self, capacity: t.cols)
    let yp = yb.contents().bindMemory(to: Float.self, capacity: t.rows)
    var r32 = UInt32(t.rows), c32 = UInt32(t.cols)
    func step(_ s: Int) {
        for j in 0..<t.cols { xp[j] = Float((j &+ s) % 17) * 0.125 - 1.0 }   // the step's own input
        dispatch(p6, width: t.rows) { enc in
            enc.setBuffer(modelBuf, offset: t.abs, index: 0)
            enc.setBuffer(xb, offset: 0, index: 1)
            enc.setBuffer(yb, offset: 0, index: 2)
            enc.setBytes(&r32, length: 4, index: 3)
            enc.setBytes(&c32, length: 4, index: 4)
        }
    }
    for s in 0..<steps {
        step(s)
        let slot = s * headDim                                  // layer 0, K, position s
        for k in 0..<headDim { kv[slot + k] = yp[k] }           // into the POOLED cache, no alloc
        for k in 0..<headDim { ws[k] = yp[k] }                  // and through the scratch pool
    }
    var kvOK = true
    for s in 0..<steps {
        step(s)
        let slot = s * headDim
        for k in 0..<headDim where kv[slot + k] != yp[k] { kvOK = false }
    }
    check(kvOK && allocations == 2,
          String(format: "gate 9 pooling: a %.0f MB KV cache (%d layers x %d kv-heads x %d head-dim x %d seq x K/V, f32) and a %d-byte workspace allocated ONCE and reused across %d steps, each step re-running a real kernel; every cached slot re-verified against a replay",
                 Double(kvFloats * 4) / 1e6, nLayers, nKVHeads, headDim, maxSeq, wsBuf.length, steps),
          "gate 9: the pooled KV cache did not read back what was written")
}

if failures == 0 { print("VERDICT PASS") } else { print("VERDICT FAIL (\(failures) gates)"); exit(1) }
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc could not build the carrier"; cat "$work/swift.err"; exit 1; }
"$work/runner" "$LIB" "$work/ref.txt" "$BLOB" "$work/table.txt" "$ITERS" "$STEPS" "$TILE" \
               "$A6" "$R6" "$C6" "$T6" "$A4" "$R4" "$C4" "$T4"
rc=$?
echo "(metallib was a cache $lib_state this run: $LIB)"
exit $rc
