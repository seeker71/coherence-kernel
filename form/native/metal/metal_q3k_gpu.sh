#!/usr/bin/env bash
# metal_q3k_gpu.sh — dispatch the body-emitted Q3_K decode on the device and judge all 256 weights
# of one superblock against q3k-dequant.fk's own answer.
#
# THE CHAIN, with no link on trust:
#     ggml's dequantize_row_q3_K  ->  q3k-dequant.fk (band 255, exact, no epsilon)
#                                 ->  q3k-msl.fk on the GPU (this gate)
# The recipe was judged against an independent transcription of the C in a throwaway scratch
# language whose OUTPUT — the sixteen 6-bit scales and the weights — is what landed as Form literals
# in q3k-dequant-band.fk. Nothing of that scratch is in the tree; the witness is.
#
# EXACTNESS IS AVAILABLE HERE AND IS THEREFORE DEMANDED. The fixture's super-scale is f16 0x3400 =
# 0.25, a power of two, and every quant is a small integer, so each weight is exact in binary on
# both sides. A tolerance would be a place for a one-ulp scale error to hide, and the scale unpack
# is the only risky arithmetic in the cell. So: bit-for-bit on all 256, no envelope.
#
# Off-Mac or without swiftc this SKIPs with exit 2. A skip is not a pass.
set -u

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT" || exit 1
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null 2>&1; then
    echo "SKIP  no Darwin/swiftc on this host"; exit 2
fi
[ -x ./fkwu ] || { echo "FAIL  no ./fkwu"; exit 1; }

PRE='; preludes: form-stdlib/core.fk form-stdlib/f16-decode.fk form-stdlib/q3k-dequant.fk form-stdlib/q3k-demo.fk'
{ echo "$PRE"; echo '(q3d-emit-all)'; } > "$work/demo.fk"
./fkwu "$work/demo.fk" 2>"$work/demo.err" | sed -n '1,/^END$/p' > "$work/demo.txt"
grep -qx 'END' "$work/demo.txt" || { echo "FAIL  fixture truncated"; tail -5 "$work/demo.err"; exit 1; }
echo "PASS  fixture emitted on fkwu ($(wc -l < "$work/demo.txt" | tr -d ' ') lines)"

{ echo '; preludes: form-stdlib/core.fk form-stdlib/q3k-msl.fk'; echo '(print_str (q3m-msl-appendix))'; } > "$work/emit.fk"
# fh16 is the spine this appendix expects; the harness supplies it, as the cell's contract says.
{ printf '#include <metal_stdlib>\ninline float fh16(uint b) { return float(as_type<half>(ushort(b))); }\n'
  ./fkwu "$work/emit.fk" 2>/dev/null | sed '$d'; } > "$work/q3k.metal"
xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
      -c "$work/q3k.metal" -o "$work/q3k.air" 2>"$work/metal.err" \
  && xcrun -sdk macosx metallib "$work/q3k.air" -o "$work/q3k.metallib" 2>>"$work/metal.err" \
  || { echo "FAIL  metallib build"; head -12 "$work/metal.err"; exit 1; }
echo "PASS  body-emitted MSL compiled to a metallib"

cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

let libPath = CommandLine.arguments[1], demoPath = CommandLine.arguments[2]
var scalars: [String: Double] = [:]
var vecs: [String: [Double]] = [:]
do {
    var mode = "", name = ""
    for raw in try String(contentsOfFile: demoPath, encoding: .utf8)
                    .split(separator: "\n", omittingEmptySubsequences: false) {
        let s = String(raw)
        if s.hasPrefix("SCALAR ") { mode = "S"; name = String(s.dropFirst(7)); continue }
        if s.hasPrefix("VEC ")    { mode = "V"; name = String(s.dropFirst(4)); vecs[name] = []; continue }
        if s == "ENDVEC" { mode = ""; continue }
        if s == "Q3KDEMO" || s == "END" { continue }
        guard let v = Double(s) else { continue }
        if mode == "S" { scalars[name] = v; mode = "" } else if mode == "V" { vecs[name]!.append(v) }
    }
}
let nblocks = Int(scalars["nblocks"]!)
let blk = vecs["blockBytes"]!.map { UInt8($0) }
let expect = vecs["expectE9"]!.map { $0 / 1e9 }

var failures = 0
func check(_ ok: Bool, _ m: String) { print((ok ? "PASS  " : "FAIL  ") + m); if !ok { failures += 1 } }
check(blk.count == 110 * nblocks, "the fixture is \(110 * nblocks) bytes — the struct's own stride")
check(expect.count == 256 * nblocks, "the recipe supplied all \(256 * nblocks) weights as the judge")

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP  no Metal device"); exit(2) }
let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
guard let fn = lib.makeFunction(name: "form_q3k_dequant_f32") else {
    print("FAIL  kernel form_q3k_dequant_f32 not in the metallib"); exit(1)
}
let pipe = try dev.makeComputePipelineState(function: fn)
let queue = dev.makeCommandQueue()!
let bQ = dev.makeBuffer(bytes: blk, length: blk.count, options: .storageModeShared)!
let bY = dev.makeBuffer(length: 256 * nblocks * 4, options: .storageModeShared)!
var uN = UInt32(nblocks)
let cb = queue.makeCommandBuffer()!, e = cb.makeComputeCommandEncoder()!
e.setComputePipelineState(pipe)
e.setBuffer(bQ, offset: 0, index: 0); e.setBuffer(bY, offset: 0, index: 1)
e.setBytes(&uN, length: 4, index: 2)
let w = 256 * nblocks
e.dispatchThreads(MTLSize(width: w, height: 1, depth: 1),
                  threadsPerThreadgroup: MTLSize(width: min(pipe.maxTotalThreadsPerThreadgroup, w), height: 1, depth: 1))
e.endEncoding(); cb.commit(); cb.waitUntilCompleted()
check(cb.error == nil && cb.status == .completed, "the command buffer completed without error")

let p = bY.contents().bindMemory(to: Float.self, capacity: w)
var mismatches = 0, firstAt = -1
var gotFirst = 0.0, expFirst = 0.0
for i in 0..<w {
    if Double(p[i]) != expect[i] {
        mismatches += 1
        if firstAt < 0 { firstAt = i; gotFirst = Double(p[i]); expFirst = expect[i] }
    }
}
check(mismatches == 0,
      mismatches == 0 ? "all \(w) weights BIT-EXACT against the recipe — no epsilon"
                      : "\(mismatches) of \(w) differ; first at \(firstAt) got \(gotFirst) expected \(expFirst)")

// the fixture is not degenerate: a decoder that returned a constant would pass an
// all-zero comparison against an all-zero expectation
let distinct = Set((0..<w).map { p[$0] }).count
check(distinct >= 8, "the decoded block is non-degenerate: \(distinct) distinct values")

print(failures == 0 ? "VERDICT PASS" : "VERDICT FAIL \(failures)")
exit(failures == 0 ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc"; tail -25 "$work/swift.err"; exit 1; }

out="$("$work/runner" "$work/q3k.metallib" "$work/demo.txt" 2>&1)"
echo "$out"
case "$out" in
    *"VERDICT PASS"*) echo; echo "metal_q3k_gpu: VERDICT PASS — Q3_K decodes on the device, bit-exact against the recipe"; exit 0;;
    *"SKIP"*)         echo; echo "metal_q3k_gpu: SKIPPED — a skip is not a pass"; exit 2;;
    *)                echo; echo "metal_q3k_gpu: VERDICT FAIL"; exit 1;;
esac
