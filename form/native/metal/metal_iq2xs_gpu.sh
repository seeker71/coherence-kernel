#!/usr/bin/env bash
# metal_iq2xs_gpu.sh — type-17 correctness and fused-matvec timing on Apple Metal.
#
# The body emits the 74 quantized bytes, 256 CPU reference weights, and all MSL. This carrier only
# compiles, binds, dispatches, compares, and times. The benchmark shape is one real Qwen4Exp expert
# projection: 640 rows x 2560 columns. It is a microkernel witness, not a model tokens/s claim.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
ITERS="${1:-20}"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null || ! command -v xcrun >/dev/null; then
    echo "SKIP  IQ2_XS GPU witness requires Darwin, Metal, xcrun, and swiftc"
    exit 2
fi

WORK_DIR="$(mktemp -d "${TMPDIR:-/tmp}/fkiq2xs.XXXXXX")"
trap 'rm -rf "$WORK_DIR"' EXIT
cd "$REPO_ROOT"

./fkwu observe/iq2xs-gpu-run.fk > "$WORK_DIR/body.out"
awk '/^BLOCK$/{s=1;next} /^REF$/{s=0} s{print}' "$WORK_DIR/body.out" > "$WORK_DIR/block.txt"
awk '/^REF$/{s=1;next} /^MSL$/{s=0} s{print}' "$WORK_DIR/body.out" > "$WORK_DIR/ref.txt"
awk '/^MSL$/{s=1;next} /^END$/{s=0} s{print}' "$WORK_DIR/body.out" > "$WORK_DIR/iq2xs.metal"

[[ "$(wc -l < "$WORK_DIR/block.txt" | tr -d ' ')" == 74 ]] || { echo "FAIL  body did not emit 74 block bytes"; exit 1; }
[[ "$(wc -l < "$WORK_DIR/ref.txt" | tr -d ' ')" == 256 ]] || { echo "FAIL  body did not emit 256 reference weights"; exit 1; }
grep -q 'kernel void form_iq2xs_matvec_simd_f32' "$WORK_DIR/iq2xs.metal" || { echo "FAIL  SIMD kernel absent"; exit 1; }
grep -q 'kernel void form_iq2xs_matvec_slot4_f32' "$WORK_DIR/iq2xs.metal" || { echo "FAIL  slot4 kernel absent"; exit 1; }

xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
    -c "$WORK_DIR/iq2xs.metal" -o "$WORK_DIR/iq2xs.air"
xcrun -sdk macosx metallib "$WORK_DIR/iq2xs.air" -o "$WORK_DIR/iq2xs.metallib"

cat > "$WORK_DIR/runner.swift" <<'SWIFT'
import Foundation
import Metal

let args = CommandLine.arguments
let libPath = args[1]
let blockPath = args[2]
let refPath = args[3]
let iters = Int(args[4])!
let rows = 640
let cols = 2560
let blocks = rows * cols / 256

let block = try String(contentsOfFile: blockPath, encoding: .utf8)
    .split(whereSeparator: { $0 == "\n" || $0 == " " })
    .map { UInt8($0)! }
let ref = try String(contentsOfFile: refPath, encoding: .utf8)
    .split(whereSeparator: { $0 == "\n" || $0 == " " })
    .map { Float($0)! }
guard block.count == 74, ref.count == 256 else { print("FAIL fixture shape"); exit(1) }

guard let device = MTLCreateSystemDefaultDevice(), let queue = device.makeCommandQueue() else {
    print("SKIP  no Metal device")
    exit(2)
}
let library = try device.makeLibrary(URL: URL(fileURLWithPath: libPath))
let pDequant = try device.makeComputePipelineState(function: library.makeFunction(name: "form_iq2xs_dequant_f32")!)
let pSerial = try device.makeComputePipelineState(function: library.makeFunction(name: "form_iq2xs_matvec_f32")!)
let pSIMD = try device.makeComputePipelineState(function: library.makeFunction(name: "form_iq2xs_matvec_simd_f32")!)
let pSlot4 = try device.makeComputePipelineState(function: library.makeFunction(name: "form_iq2xs_matvec_slot4_f32")!)
guard pSIMD.maxTotalThreadsPerThreadgroup >= 256 else {
    print("SKIP  device cannot dispatch the kernel's exact 256-thread contract")
    exit(2)
}

var qbytes = [UInt8](repeating: 0, count: blocks * 74)
for b in 0..<blocks {
    qbytes.replaceSubrange((b * 74)..<(b * 74 + 74), with: block)
}
var x = [Float](repeating: 0, count: cols)
for j in 0..<cols { x[j] = Float((j % 23) - 11) / 37.0 }

let qbuf = device.makeBuffer(bytes: qbytes, length: qbytes.count, options: .storageModeShared)!
let xbuf = device.makeBuffer(bytes: x, length: x.count * 4, options: .storageModeShared)!
let deqbuf = device.makeBuffer(length: 256 * 4, options: .storageModeShared)!
let serialBuf = device.makeBuffer(length: rows * 4, options: .storageModeShared)!
let simdBuf = device.makeBuffer(length: rows * 4, options: .storageModeShared)!
let slot4Buf = device.makeBuffer(length: rows * 4, options: .storageModeShared)!

func dispatchDequant() {
    var off = UInt32(0), n = UInt32(256)
    let cb = queue.makeCommandBuffer()!, e = cb.makeComputeCommandEncoder()!
    e.setComputePipelineState(pDequant)
    e.setBuffer(qbuf, offset: 0, index: 0); e.setBuffer(deqbuf, offset: 0, index: 1)
    e.setBytes(&off, length: 4, index: 2); e.setBytes(&n, length: 4, index: 3)
    e.dispatchThreads(MTLSize(width: 256, height: 1, depth: 1),
                      threadsPerThreadgroup: MTLSize(width: 256, height: 1, depth: 1))
    e.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if cb.status != .completed { print("FAIL dequant command buffer: \(String(describing: cb.error))"); exit(1) }
}

func encodeMatvec(_ e: MTLComputeCommandEncoder, _ pipeline: MTLComputePipelineState,
                  _ out: MTLBuffer, simd: Bool, slot4: Bool = false) {
    var r = UInt32(rows), c = UInt32(cols)
    e.setComputePipelineState(pipeline)
    e.setBuffer(qbuf, offset: 0, index: 0); e.setBuffer(xbuf, offset: 0, index: 1)
    e.setBuffer(out, offset: 0, index: 2)
    e.setBytes(&r, length: 4, index: 3); e.setBytes(&c, length: 4, index: 4)
    if slot4 {
        let threads = ((rows + 3) / 4) * 32
        e.dispatchThreads(MTLSize(width: threads, height: 1, depth: 1),
                          threadsPerThreadgroup: MTLSize(width: 256, height: 1, depth: 1))
    } else if simd {
        e.dispatchThreadgroups(MTLSize(width: rows, height: 1, depth: 1),
                               threadsPerThreadgroup: MTLSize(width: 256, height: 1, depth: 1))
    } else {
        e.dispatchThreads(MTLSize(width: rows, height: 1, depth: 1),
                          threadsPerThreadgroup: MTLSize(width: min(rows, pipeline.maxTotalThreadsPerThreadgroup), height: 1, depth: 1))
    }
}

func runOnce(_ pipeline: MTLComputePipelineState, _ out: MTLBuffer, simd: Bool, slot4: Bool = false) {
    let cb = queue.makeCommandBuffer()!, e = cb.makeComputeCommandEncoder()!
    encodeMatvec(e, pipeline, out, simd: simd, slot4: slot4)
    e.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if cb.status != .completed { print("FAIL matvec command buffer: \(String(describing: cb.error))"); exit(1) }
}

func timed(_ pipeline: MTLComputePipelineState, _ out: MTLBuffer, simd: Bool, slot4: Bool = false) -> Double {
    runOnce(pipeline, out, simd: simd, slot4: slot4)
    let cb = queue.makeCommandBuffer()!, e = cb.makeComputeCommandEncoder()!
    for _ in 0..<iters { encodeMatvec(e, pipeline, out, simd: simd, slot4: slot4) }
    e.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if cb.status != .completed { print("FAIL timed command buffer: \(String(describing: cb.error))"); exit(1) }
    return cb.gpuEndTime - cb.gpuStartTime
}

dispatchDequant()
let got = deqbuf.contents().bindMemory(to: Float.self, capacity: 256)
var dequantBad = 0
for i in 0..<256 where got[i].bitPattern != ref[i].bitPattern { dequantBad += 1 }
guard dequantBad == 0 else { print("FAIL dequant: \(dequantBad)/256 differ bitwise"); exit(1) }
print("PASS  dequant: all 256 GPU weights bit-equal the independent Form carver")

var cpu: Float = 0
for j in stride(from: cols - 1, through: 0, by: -1) {
    let p = ref[j % 256] * x[j]
    cpu = p + cpu
}
runOnce(pSerial, serialBuf, simd: false)
runOnce(pSIMD, simdBuf, simd: true)
runOnce(pSlot4, slot4Buf, simd: false, slot4: true)
let serial = serialBuf.contents().bindMemory(to: Float.self, capacity: rows)
let parallel = simdBuf.contents().bindMemory(to: Float.self, capacity: rows)
let slotted = slot4Buf.contents().bindMemory(to: Float.self, capacity: rows)
var serialBad = 0, simdBad = 0, slot4Bad = 0
var maxAbs: Float = 0, maxRel: Float = 0, slot4MaxAbs: Float = 0, slot4MaxRel: Float = 0
for r in 0..<rows {
    if serial[r].bitPattern != cpu.bitPattern { serialBad += 1 }
    let ae = abs(parallel[r] - cpu)
    let re = ae / max(abs(cpu), 1.0)
    maxAbs = max(maxAbs, ae); maxRel = max(maxRel, re)
    if re > 2.0e-5 { simdBad += 1 }
    let se = abs(slotted[r] - cpu)
    let sr = se / max(abs(cpu), 1.0)
    slot4MaxAbs = max(slot4MaxAbs, se); slot4MaxRel = max(slot4MaxRel, sr)
    if sr > 2.0e-5 { slot4Bad += 1 }
}
guard serialBad == 0 else { print("FAIL serial matvec: \(serialBad)/\(rows) rows differ bitwise"); exit(1) }
guard simdBad == 0 else { print("FAIL SIMD matvec: \(simdBad)/\(rows), maxRel=\(maxRel)"); exit(1) }
guard slot4Bad == 0 else { print("FAIL slot4 matvec: \(slot4Bad)/\(rows), maxRel=\(slot4MaxRel)"); exit(1) }
print("PASS  serial fused matvec: \(rows)x\(cols), all rows bit-equal the right-fold CPU reference")
print(String(format: "PASS  SIMD fused matvec: %dx%d, maxAbs %.7g, maxRel %.7g", rows, cols, maxAbs, maxRel))
print(String(format: "PASS  slot4 fused matvec: %dx%d, maxAbs %.7g, maxRel %.7g", rows, cols, slot4MaxAbs, slot4MaxRel))

let ts = timed(pSerial, serialBuf, simd: false)
let tp = timed(pSIMD, simdBuf, simd: true)
let tq = timed(pSlot4, slot4Buf, simd: false, slot4: true)
let work = Double(rows * cols * iters)
let quantBytes = work * 74.0 / 256.0
print(String(format: "BENCH serial %.6f s  %.3f Gweight/s  %.3f logical-quant-GB/s", ts, work / ts / 1e9, quantBytes / ts / 1e9))
print(String(format: "BENCH simd   %.6f s  %.3f Gweight/s  %.3f logical-quant-GB/s  speedup %.2fx", tp, work / tp / 1e9, quantBytes / tp / 1e9, ts / tp))
print(String(format: "BENCH slot4  %.6f s  %.3f Gweight/s  %.3f logical-quant-GB/s  speedup %.2fx vs serial, %.2fx vs simd", tq, work / tq / 1e9, quantBytes / tq / 1e9, ts / tq, tp / tq))
print("PASS  resident quantized matrix: \(qbytes.count) bytes, zero f32 weight materialization, device \(device.name)")
SWIFT

swiftc "$WORK_DIR/runner.swift" -framework Metal -framework Foundation -O -o "$WORK_DIR/runner"
"$WORK_DIR/runner" "$WORK_DIR/iq2xs.metallib" "$WORK_DIR/block.txt" "$WORK_DIR/ref.txt" "$ITERS"
