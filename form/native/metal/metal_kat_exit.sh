#!/usr/bin/env bash
# metal_kat_exit.sh — KAT-Coder's EXIT path on the device: a token id in, a token id out.
#
# embed(t) -> output_norm -> output.weight -> argmax
#
# WHAT THIS IS AND IS NOT. This is the DS4 lane's Stage 2 shape
# (receipts/2026-07-22-first-deepseek-token.md), and it carries that stone's honesty with it: the
# vector fed to the vocabulary projection is the EMBEDDING, not the hidden state of 41 blocks. So
# the id that comes out is a MECHANISM witness — proof that the entrance decodes, the norm runs,
# the 540 MB Q8_0 projection reaches all 248 320 rows, and an argmax falls out — and NOT the token
# this model would emit for that input. Saying otherwise would be the fabrication the whole lane
# exists to refuse.
#
# EVERY KERNEL IS ONE THE BODY ALREADY EMITS:
#   form_q3k_dequant_f32   q3k-msl.fk      the Q3_K entrance, 256/256 bit-exact on device
#   form_mla_rmsnorm_f32   mla-msl.fk      RMSNorm with gain
#   form_q8_0_matvec_f32   q8-0-msl.fk     the fused Q8_0 matvec, one thread per row
# Nothing new is authored here. The harness maps the file, binds three buffers and dispatches.
#
# THE FILE IS WRAPPED, NOT COPIED. mmap + bytesNoCopy over the whole 17.39 GB, the way the DS4 lane
# holds 85 GiB (onelean). The projection alone is 540 344 320 bytes.
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
  (print_str "END")
  0)
EOF
./fkwu --src "$work/hdr.fk" 2>/dev/null | sed -n '1,/^END$/p' > "$work/hdr.txt"
grep -qx 'END' "$work/hdr.txt" || { echo "FAIL  header read failed"; exit 1; }
get() { grep "^$1 " "$work/hdr.txt" | head -1 | awk '{print $2}'; }
EMBED_ABS=$(get EMBED_ABS); EMBED_NE0=$(get EMBED_NE0); EMBED_TYPE=$(get EMBED_TYPE)
OUT_ABS=$(get OUT_ABS);   OUT_ROWS=$(get OUT_ROWS);   OUT_COLS=$(get OUT_COLS); OUT_TYPE=$(get OUT_TYPE)
NORM_ABS=$(get NORM_ABS); NORM_TYPE=$(get NORM_TYPE)
echo "PASS  the body located its own tensors: embed@$EMBED_ABS type $EMBED_TYPE, out@$OUT_ABS ${OUT_ROWS}x${OUT_COLS} type $OUT_TYPE, norm@$NORM_ABS type $NORM_TYPE"
[ "$EMBED_TYPE" = "11" ] || { echo "FAIL  embed is not Q3_K"; exit 1; }
[ "$OUT_TYPE" = "8" ]    || { echo "FAIL  output is not Q8_0"; exit 1; }
[ "$NORM_TYPE" = "0" ]   || { echo "FAIL  output_norm is not F32"; exit 1; }

# ── 2. the MSL, emitted by the body ──────────────────────────────────────────────────────────
cat > "$work/emit.fk" <<'EOF'
; preludes: form-stdlib/core.fk form-stdlib/str-byte-at.fk form-stdlib/equireach.fk form-stdlib/format-arith.fk form-stdlib/f16-decode.fk form-stdlib/q6k-dequant.fk form-stdlib/q6k-msl.fk form-stdlib/q8-0-msl.fk form-stdlib/q3k-msl.fk form-stdlib/transformer-numerics.fk form-stdlib/mla-msl.fk
; q3k-msl.fk's appendix calls `fh16` for the super-scale; the q6k/q80 spine
; already carries the same decode as `q6k_f16`. One alias, so there is still
; exactly ONE f16 decode in the unit rather than a second implementation.
(print_str (str_concat (q80-msl-unit "form_q8_0_dequant_f32" "form_q8_0_matvec_f32" "form_q8_0_matvec_lane_f32")
           (str_concat "static inline float fh16(uint b) { return q6k_f16(int(b)); }"
           (str_concat (q3m-msl-appendix) (str_concat (mla-msl-spine) (mla-rmsnorm-body))))))
EOF
{ printf '#include <metal_stdlib>\n'
  ./fkwu --src "$work/emit.fk" 2>"$work/emit.err" | sed '$d'; } > "$work/kat.metal"
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
let pMv = try pipe("form_q8_0_matvec_f32")
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

// ── output_norm (F32 gain, read straight out of the wrapped file) ──
let bH = dev.makeBuffer(length: ne0 * 4, options: .storageModeShared)!
var un = UInt32(ne0); var eps: Float = 1e-6
run(pNrm, 1) { e in
    e.setBuffer(bEmb, offset: 0, index: 0)
    e.setBuffer(bAll, offset: normAbs, index: 1)
    e.setBuffer(bH, offset: 0, index: 2)
    e.setBytes(&un, length: 4, index: 3); e.setBytes(&eps, length: 4, index: 4) }
let h = bH.contents().bindMemory(to: Float.self, capacity: ne0)
var hnz = 0; for i in 0..<ne0 { if h[i] != 0 { hnz += 1 } }
check(hnz > ne0 / 2, "output_norm applied: \(hnz)/\(ne0) nonzero")

// ── the 540 MB Q8_0 vocabulary projection ──
let bLog = dev.makeBuffer(length: rows * 4, options: .storageModeShared)!
var uR = UInt32(rows), uC = UInt32(cols)
run(pMv, rows) { e in
    e.setBuffer(bAll, offset: outAbs, index: 0)
    e.setBuffer(bH, offset: 0, index: 1)
    e.setBuffer(bLog, offset: 0, index: 2)
    e.setBytes(&uR, length: 4, index: 3); e.setBytes(&uC, length: 4, index: 4) }
let wall = Date().timeIntervalSince(t0)
check(gpuErr == 0, "every command buffer completed without error")

let lg = bLog.contents().bindMemory(to: Float.self, capacity: rows)
var best = 0; var bestV = -Float.infinity; var finite = 0; var distinct = Set<Float>()
for i in 0..<rows {
    let v = lg[i]
    if v.isFinite { finite += 1 }
    if distinct.count < 4096 { distinct.insert(v) }
    if v > bestV { bestV = v; best = i } }
check(finite == rows, "all \(rows) logits finite")
check(distinct.count > 1000, "logits non-degenerate: \(distinct.count)+ distinct values sampled")
print(String(format: "TOKEN  argmax id=%d logit=%.6f  over %d rows x %d cols  wall %.3f s", best, bestV, rows, cols, wall))
print(failures == 0 ? "VERDICT PASS" : "VERDICT FAIL \(failures)")
exit(failures == 0 ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc"; tail -25 "$work/swift.err"; exit 1; }

out="$("$work/runner" "$GGUF" "$work/kat.metallib" "$EMBED_ABS" "$EMBED_NE0" \
        "$OUT_ABS" "$OUT_ROWS" "$OUT_COLS" "$NORM_ABS" "$TOKEN" 2>&1)"
echo "$out"
case "$out" in
  *"VERDICT PASS"*) echo; echo "metal_kat_exit: VERDICT PASS — the exit path runs on the device."
                    echo "  claimed: embed decodes, norm runs, the 540 MB Q8_0 projection reaches all rows, argmax falls out."
                    echo "  NOT claimed: that this id is the token KAT-Coder would emit. The projection was fed the"
                    echo "               EMBEDDING, not the hidden state of 41 blocks. A mechanism witness, not a token."
                    exit 0;;
  *"SKIP"*) echo; echo "metal_kat_exit: SKIPPED — a skip is not a pass"; exit 2;;
  *) echo; echo "metal_kat_exit: VERDICT FAIL"; exit 1;;
esac
