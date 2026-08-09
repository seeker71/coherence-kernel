#!/usr/bin/env bash
# metal_kat_block0.sh — the first step INTO KAT-Coder's middle: block 0's front half.
#
# embed(t) -> attn_norm -> attn_qkv (Q4_K) -> the causal conv -> q/k/v split
#
# metal_kat_exit.sh proved the door out. This proves the door in: that a BLOCK's weights bind, that
# a Q4_K projection at real width runs, and that the 8192-wide q||k||v the layer cells were built
# against is really what comes out of this file.
#
# WHAT THIS IS AND IS NOT. Three of a linear block's stages, at real width, on real weights:
# attn_norm, the fused q||k||v projection, and the causal conv over that whole concatenation. It is
# NOT a completed block and NOT a token. The delta rule, the output gate, the ssm_out projection and
# the 256-expert MoE FFN are not wired here, and 40 more blocks follow this one.
#
# WHAT IT SETTLES. gated-deltanet-layer.fk was built against a q||k||v of 16x128 + 16x128 + 32x128
# read from config.json and confirmed from the tensor table. This runs the projection and gets 8192
# back from the file itself — the third and last way to be sure of that number, and the one that
# involves no reading at all.
#
# EVERY KERNEL IS ONE THE BODY ALREADY EMITS: form_q3k_dequant_f32, form_mla_rmsnorm_f32,
# form_q4_k_matvec_f32, form_gdn_conv_f32. Nothing new was authored in MSL.
#
# THE FILE IS WRAPPED, NOT COPIED. mmap + bytesNoCopy over the whole 17.39 GB (onelean).
#
# Off-Mac or without swiftc this SKIPs with exit 2. A skip is not a pass.
set -uo pipefail

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT" || exit 1
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT

GGUF="${FORM_KAT_BLOB:-$HOME/models/katcoder/kat-coder-v2.5-dev-compact.gguf}"
TOKEN="${FORM_KAT_TOKEN:-100}"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null 2>&1; then
    echo "SKIP  no Darwin/swiftc on this host"; exit 2
fi
[ -x ./fkwu ] || { echo "FAIL  no ./fkwu"; exit 1; }
[ -f "$GGUF" ] || { echo "SKIP  no $GGUF — a skip is not a pass"; exit 2; }

# ── 1. the body reads its own header and tells the carrier where things are ──────────────────
cat > "$work/hdr.fk" <<'EOF'
; preludes: form-stdlib/core.fk form-stdlib/str-byte-at.fk form-stdlib/equireach.fk form-stdlib/equireach-gguf.fk form-stdlib/kat-coder-embed.fk
(do
  (defn emit (n v) (print_str (str_concat n (int_to_str v))))
  (let h (kce-hdr))
  (let base (egg-data-base h 32))
  (let oe (egg-find-tensor h "token_embd.weight"))
  (let oo (egg-find-tensor h "output.weight"))
  (let on (egg-find-tensor h "output_norm.weight"))
  (emit "EMBED_ABS "  (add base (egg-ti-dataoff h oe)))
  (emit "EMBED_NE0 "  (egg-ti-dim h oe 0))
  (emit "EMBED_TYPE " (egg-ti-type h oe))
  (emit "OUT_ABS "    (add base (egg-ti-dataoff h oo)))
  (emit "OUT_ROWS "   (egg-ti-dim h oo 1))
  (emit "OUT_COLS "   (egg-ti-dim h oo 0))
  (emit "OUT_TYPE "   (egg-ti-type h oo))
  (emit "NORM_ABS "   (add base (egg-ti-dataoff h on)))
  (emit "NORM_TYPE "  (egg-ti-type h on))
  (let an (egg-find-tensor h "blk.0.attn_norm.weight"))
  (let qk (egg-find-tensor h "blk.0.attn_qkv.weight"))
  (let cv (egg-find-tensor h "blk.0.ssm_conv1d.weight"))
  (emit "AN_ABS "   (add base (egg-ti-dataoff h an)))
  (emit "QKV_ABS "  (add base (egg-ti-dataoff h qk)))
  (emit "QKV_ROWS " (egg-ti-dim h qk 1))
  (emit "QKV_COLS " (egg-ti-dim h qk 0))
  (emit "QKV_TYPE " (egg-ti-type h qk))
  (emit "CV_ABS "   (add base (egg-ti-dataoff h cv)))
  (emit "CV_TAPS "  (egg-ti-dim h cv 0))
  (emit "CV_CH "    (egg-ti-dim h cv 1))
  (print_str "END")
  0)
EOF
./fkwu "$work/hdr.fk" 2>/dev/null | sed -n '1,/^END$/p' > "$work/hdr.txt"
grep -qx 'END' "$work/hdr.txt" || { echo "FAIL  header read failed"; exit 1; }
get() { grep "^$1 " "$work/hdr.txt" | head -1 | awk '{print $2}'; }
EMBED_ABS=$(get EMBED_ABS); EMBED_NE0=$(get EMBED_NE0); EMBED_TYPE=$(get EMBED_TYPE)
OUT_ABS=$(get OUT_ABS);   OUT_ROWS=$(get OUT_ROWS);   OUT_COLS=$(get OUT_COLS); OUT_TYPE=$(get OUT_TYPE)
NORM_ABS=$(get NORM_ABS); NORM_TYPE=$(get NORM_TYPE)
AN_ABS=$(get AN_ABS); QKV_ABS=$(get QKV_ABS); QKV_ROWS=$(get QKV_ROWS); QKV_COLS=$(get QKV_COLS); QKV_TYPE=$(get QKV_TYPE)
CV_ABS=$(get CV_ABS); CV_TAPS=$(get CV_TAPS); CV_CH=$(get CV_CH)
echo "PASS  block 0: attn_qkv ${QKV_ROWS}x${QKV_COLS} type $QKV_TYPE, ssm_conv1d ${CV_TAPS}x${CV_CH}"
[ "$QKV_TYPE" = "12" ] || { echo "FAIL  attn_qkv is not Q4_K"; exit 1; }
echo "PASS  the body located its own tensors: embed@$EMBED_ABS type $EMBED_TYPE, out@$OUT_ABS ${OUT_ROWS}x${OUT_COLS} type $OUT_TYPE, norm@$NORM_ABS type $NORM_TYPE"
[ "$EMBED_TYPE" = "11" ] || { echo "FAIL  embed is not Q3_K"; exit 1; }
[ "$OUT_TYPE" = "8" ]    || { echo "FAIL  output is not Q8_0"; exit 1; }
[ "$NORM_TYPE" = "0" ]   || { echo "FAIL  output_norm is not F32"; exit 1; }

# ── 2. the MSL, emitted by the body ──────────────────────────────────────────────────────────
cat > "$work/emit.fk" <<'EOF'
; preludes: form-stdlib/core.fk form-stdlib/str-byte-at.fk form-stdlib/equireach.fk form-stdlib/format-arith.fk form-stdlib/f16-decode.fk form-stdlib/q6k-dequant.fk form-stdlib/q6k-msl.fk form-stdlib/q8-0-msl.fk form-stdlib/q3k-msl.fk form-stdlib/q4k-dequant.fk form-stdlib/q4k-msl.fk form-stdlib/gated-deltanet-msl.fk form-stdlib/transformer-numerics.fk form-stdlib/mla-msl.fk
; q3k-msl.fk's appendix calls `fh16` for the super-scale; the q6k/q80 spine
; already carries the same decode as `q6k_f16`. One alias, so there is still
; exactly ONE f16 decode in the unit rather than a second implementation.
(print_str (str_concat (q80-msl-unit "form_q8_0_dequant_f32" "form_q8_0_matvec_f32" "form_q8_0_matvec_lane_f32")
           (str_concat "static inline float fh16(uint b) { return q6k_f16(int(b)); }"
           (str_concat (q3m-msl-appendix) (str_concat (q4m-matvec-msl "form_q4_k_matvec_f32") (str_concat (gdm-conv-msl "form_gdn_conv_f32") (str_concat (mla-msl-spine) (mla-rmsnorm-body))))))))
EOF
{ printf '#include <metal_stdlib>\n'
  ./fkwu "$work/emit.fk" 2>"$work/emit.err" | sed '$d'; } > "$work/kat.metal"
if [ "$(wc -c < "$work/kat.metal")" -lt 500 ]; then
    echo "FAIL  emission short"; head -5 "$work/emit.err"; exit 1; fi
xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
      -c "$work/kat.metal" -o "$work/kat.air" 2>"$work/metal.err" \
  && xcrun -sdk macosx metallib "$work/kat.air" -o "$work/kat.metallib" 2>>"$work/metal.err" \
  || { echo "FAIL  metallib build"; head -14 "$work/metal.err"; exit 1; }
echo "PASS  body-emitted MSL compiled ($(wc -c < "$work/kat.metal") bytes)"

# ── 3. dispatch ──────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

let a = CommandLine.arguments
let path = a[1], libPath = a[2]
let embedAbs = Int(a[3])!, ne0 = Int(a[4])!
let outAbs = Int(a[5])!, rows = Int(a[6])!, cols = Int(a[7])!
let normAbs = Int(a[8])!, token = Int(a[9])!
let anAbs = Int(a[10])!, qkvAbs = Int(a[11])!, qkvRows = Int(a[12])!, qkvCols = Int(a[13])!
let cvAbs = Int(a[14])!, cvTaps = Int(a[15])!

var failures = 0
func check(_ ok: Bool, _ m: String) { print((ok ? "PASS  " : "FAIL  ") + m); if !ok { failures += 1 } }

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP  no Metal device"); exit(2) }
let fd = open(path, O_RDONLY)
guard fd >= 0 else { print("FAIL  open"); exit(1) }
var st = stat(); fstat(fd, &st)
let fileLen = Int(st.st_size)
guard let map = mmap(nil, fileLen, PROT_READ, MAP_PRIVATE, fd, 0), map != MAP_FAILED else {
    print("FAIL  mmap"); exit(1) }
// wrapped, not copied
guard let bAll = dev.makeBuffer(bytesNoCopy: map, length: fileLen, options: .storageModeShared,
                                deallocator: nil) else { print("FAIL  bytesNoCopy"); exit(1) }
check(true, "wrapped \(fileLen) bytes with bytesNoCopy — device.currentAllocatedSize = \(dev.currentAllocatedSize) B")

let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
func pipe(_ n: String) throws -> MTLComputePipelineState {
    guard let f = lib.makeFunction(name: n) else { print("FAIL  kernel \(n) missing"); exit(1) }
    return try dev.makeComputePipelineState(function: f) }
let pQ3 = try pipe("form_q3k_dequant_f32")
let pNrm = try pipe("form_mla_rmsnorm_f32")
let pQ4 = try pipe("form_q4_k_matvec_f32")
let pCv = try pipe("form_gdn_conv_f32")
let queue = dev.makeCommandQueue()!
var gpuErr = 0
func run(_ p: MTLComputePipelineState, _ w: Int, _ bind: (MTLComputeCommandEncoder) -> Void) {
    let cb = queue.makeCommandBuffer()!, e = cb.makeComputeCommandEncoder()!
    e.setComputePipelineState(p); bind(e)
    e.dispatchThreads(MTLSize(width: w, height: 1, depth: 1),
        threadsPerThreadgroup: MTLSize(width: min(p.maxTotalThreadsPerThreadgroup, w), height: 1, depth: 1))
    e.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if cb.error != nil || cb.status != .completed { gpuErr += 1 } }

// ── embed: one Q3_K row = ne0/256 blocks of 110 bytes ──
let rowBytes = ne0 / 256 * 110
let bEmb = dev.makeBuffer(length: ne0 * 4, options: .storageModeShared)!
var nblocks = UInt32(ne0 / 256)
let t0 = Date()
run(pQ3, ne0) { e in
    e.setBuffer(bAll, offset: embedAbs + token * rowBytes, index: 0)
    e.setBuffer(bEmb, offset: 0, index: 1)
    e.setBytes(&nblocks, length: 4, index: 2) }
let emb = bEmb.contents().bindMemory(to: Float.self, capacity: ne0)
var nz = 0; var mx: Float = 0
for i in 0..<ne0 { if emb[i] != 0 { nz += 1 }; mx = max(mx, abs(emb[i])) }
check(nz > ne0 / 2 && mx > 0 && mx < 1,
      "embed row \(token) decoded on device: \(nz)/\(ne0) nonzero, max |w| \(mx)")

// ── block 0: attn_norm over the embedding ──
let bH = dev.makeBuffer(length: ne0 * 4, options: .storageModeShared)!
var un = UInt32(ne0); var eps: Float = 1e-6
run(pNrm, 1) { e in
    e.setBuffer(bEmb, offset: 0, index: 0)
    e.setBuffer(bAll, offset: anAbs, index: 1)
    e.setBuffer(bH, offset: 0, index: 2)
    e.setBytes(&un, length: 4, index: 3); e.setBytes(&eps, length: 4, index: 4) }
let h = bH.contents().bindMemory(to: Float.self, capacity: ne0)
var hnz = 0; for i in 0..<ne0 { if h[i] != 0 { hnz += 1 } }
check(hnz > ne0 / 2, "blk.0.attn_norm applied: \(hnz)/\(ne0) nonzero")

// ── the fused q||k||v projection, Q4_K at real width ──
let bQkv = dev.makeBuffer(length: qkvRows * 4, options: .storageModeShared)!
var qr = UInt32(qkvRows), qc = UInt32(qkvCols)
run(pQ4, qkvRows) { e in
    e.setBuffer(bAll, offset: qkvAbs, index: 0)
    e.setBuffer(bH, offset: 0, index: 1)
    e.setBuffer(bQkv, offset: 0, index: 2)
    e.setBytes(&qr, length: 4, index: 3); e.setBytes(&qc, length: 4, index: 4) }
let qkv = bQkv.contents().bindMemory(to: Float.self, capacity: qkvRows)
var qnz = 0; var qmx: Float = 0; var qfin = 0
for i in 0..<qkvRows { if qkv[i] != 0 { qnz += 1 }; if qkv[i].isFinite { qfin += 1 }; qmx = max(qmx, abs(qkv[i])) }
check(qfin == qkvRows, "attn_qkv projected: all \(qkvRows) entries finite")
check(qnz > qkvRows / 2 && qmx > 0 && qmx < 1e3,
      "q||k||v non-degenerate: \(qnz)/\(qkvRows) nonzero, max |v| \(qmx)")
check(qkvRows == 8192, "the projection is 8192 wide — q 16x128 + k 16x128 + v 32x128, the split gated-deltanet-layer.fk was built against")

// ── the causal conv over that whole concatenation, position 0 ──
let bWin = dev.makeBuffer(length: qkvRows * cvTaps * 4, options: .storageModeShared)!
memset(bWin.contents(), 0, qkvRows * cvTaps * 4)
let bCv = dev.makeBuffer(length: qkvRows * 4, options: .storageModeShared)!
var nch = UInt32(qkvRows), kw = UInt32(cvTaps)
run(pCv, qkvRows) { e in
    e.setBuffer(bWin, offset: 0, index: 0)
    e.setBuffer(bQkv, offset: 0, index: 1)
    e.setBuffer(bAll, offset: cvAbs, index: 2)
    e.setBuffer(bCv, offset: 0, index: 3)
    e.setBytes(&nch, length: 4, index: 4); e.setBytes(&kw, length: 4, index: 5) }
let cv = bCv.contents().bindMemory(to: Float.self, capacity: qkvRows)
var cnz = 0; var cfin = 0
for i in 0..<qkvRows { if cv[i] != 0 { cnz += 1 }; if cv[i].isFinite { cfin += 1 } }
check(cfin == qkvRows, "causal conv ran over all \(qkvRows) channels: all finite")
check(cnz > qkvRows / 4, "conv output non-degenerate: \(cnz)/\(qkvRows) nonzero")
let wall = Date().timeIntervalSince(t0)
check(gpuErr == 0, "every command buffer completed without error")
print(String(format: "BLOCK0 front half ran: embed -> attn_norm -> attn_qkv(Q4_K %dx%d) -> causal conv(%d taps x %d ch)  wall %.3f s",
             qkvRows, qkvCols, cvTaps, qkvRows, wall))
print(failures == 0 ? "VERDICT PASS" : "VERDICT FAIL \(failures)")
exit(failures == 0 ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc"; tail -25 "$work/swift.err"; exit 1; }

out="$("$work/runner" "$GGUF" "$work/kat.metallib" "$EMBED_ABS" "$EMBED_NE0" \
        "$OUT_ABS" "$OUT_ROWS" "$OUT_COLS" "$NORM_ABS" "$TOKEN" "$AN_ABS" "$QKV_ABS" "$QKV_ROWS" "$QKV_COLS" "$CV_ABS" "$CV_TAPS" 2>&1)"
echo "$out"
case "$out" in
  *"VERDICT PASS"*) echo; echo "metal_kat_block0: VERDICT PASS — block 0 front half runs on the device."
                    echo "  claimed: a block s weights bind and its front half computes at real width —"
                    echo "           attn_norm, the Q4_K q||k||v projection, and the causal conv over the concatenation."
                    echo "  NOT claimed: a completed block, and not a token. The delta rule, the output gate, the"
                    echo "               ssm_out projection and the MoE FFN are not wired here, and 40 blocks follow."
                    exit 0;;
  *"SKIP"*) echo; echo "metal_kat_block0: SKIPPED — a skip is not a pass"; exit 2;;
  *) echo; echo "metal_kat_block0: VERDICT FAIL"; exit 1;;
esac
