#!/usr/bin/env bash
# metal_gdn_gpu.sh — DISPATCH the linear-attention layer's four kernels on the device, two decode
# steps, judged against gated-deltanet-layer.fk's own answer.
#
# TWO STEPS AND NOT ONE. A single step cannot tell a stateful layer from a stateless one, and
# carrying state is what this layer is for. The runner threads the device state exactly as
# gdl-step threads the Form one — the conv window buffer and the per-head S buffers are created
# once and never reset between tokens — so any failure to carry shows up on token 2 alone while
# token 1 stays green.
#
# THE JUDGE IS THE BODY'S OWN RECIPE. alpha and beta cross as VALUES rather than being recomputed
# on the runner side from a[] and b[]: recomputing them would quietly test two implementations of
# the gate against each other instead of testing the kernels.
#
# ids-style exactness is not available here — every output is a float fold — so both tokens are
# judged on a relative envelope, declared ahead of the run, with the worst deviation printed
# whether it passes or fails.
#
# Off-Mac or without swiftc this SKIPs with exit 2. A skip is not a pass.
set -u

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT" || exit 1
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT

ENVELOPE="2e-6"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null 2>&1; then
    echo "SKIP  no Darwin/swiftc on this host"; exit 2
fi
[ -x ./fkwu ] || { echo "FAIL  no ./fkwu"; exit 1; }

PRE='; preludes: form-stdlib/core.fk form-stdlib/trig.fk form-stdlib/transformer-numerics.fk form-stdlib/llama-numerics.fk form-stdlib/kimi-kda.fk form-stdlib/gated-deltanet-conv.fk form-stdlib/gated-deltanet-gates.fk form-stdlib/gated-deltanet-layer.fk form-stdlib/gated-deltanet-demo.fk'

{ echo "$PRE"; echo '(gdd-emit-all)'; } > "$work/demo.fk"
./fkwu --src "$work/demo.fk" 2>"$work/demo.err" | sed -n '1,/^END$/p' > "$work/demo.txt"
grep -qx 'END' "$work/demo.txt" || { echo "FAIL  fixture truncated"; tail -5 "$work/demo.err"; exit 1; }
echo "PASS  fixture emitted on fkwu ($(wc -l < "$work/demo.txt" | tr -d ' ') lines)"

{ echo '; preludes: form-stdlib/core.fk form-stdlib/gated-deltanet-msl.fk'; echo '(print_str (gdm-msl-appendix))'; } > "$work/emit.fk"
{ printf '#include <metal_stdlib>\ninline float fexp(float x) { return metal::exp(x); }\ninline float fsqrt(float x) { return metal::sqrt(x); }\n'
  ./fkwu --src "$work/emit.fk" 2>/dev/null | sed '$d'; } > "$work/gdn.metal"
xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
      -c "$work/gdn.metal" -o "$work/gdn.air" 2>"$work/metal.err" \
  && xcrun -sdk macosx metallib "$work/gdn.air" -o "$work/gdn.metallib" 2>>"$work/metal.err" \
  || { echo "FAIL  metallib build"; head -12 "$work/metal.err"; exit 1; }
echo "PASS  body-emitted MSL compiled to a metallib"

cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

let libPath = CommandLine.arguments[1], demoPath = CommandLine.arguments[2]
let envelope = Double(CommandLine.arguments[3])!

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
        if s == "GDNDEMO" || s == "END" { continue }
        guard let v = Double(s) else { continue }
        if mode == "S" { scalars[name] = v; mode = "" } else if mode == "V" { vecs[name]!.append(v) }
    }
}
func I(_ n: String) -> Int { Int(scalars[n]!) }
func V(_ n: String) -> [Float] { vecs[n]!.map { Float($0 / 1e9) } }
func D(_ n: String) -> [Double] { vecs[n]!.map { $0 / 1e9 } }

let nk = I("nk"), dk = I("dk"), nv = I("nv"), dv = I("dv"), nch = I("nch"), kw = I("kw")
let taps = V("tapsE9"), x1 = V("x1E9"), x2 = V("x2E9")
let alphas = D("alphaE9"), betas = D("betaE9"), zv = V("zE9")
let exp1 = D("expectO1E9"), exp2 = D("expectO2E9")

var failures = 0
func check(_ ok: Bool, _ m: String) { print((ok ? "PASS  " : "FAIL  ") + m); if !ok { failures += 1 } }

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP  no Metal device"); exit(2) }
let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
let queue = dev.makeCommandQueue()!
func pipe(_ n: String) throws -> MTLComputePipelineState {
    guard let f = lib.makeFunction(name: n) else { print("FAIL  kernel \(n) missing"); exit(1) }
    return try dev.makeComputePipelineState(function: f)
}
let pConv = try pipe("form_gdn_conv_f32"), pNorm = try pipe("form_gdn_l2norm_f32")
let pDelta = try pipe("form_gdn_delta_f32"), pGate = try pipe("form_gdn_gate_f32")

// state buffers: created ONCE, never reset between tokens — this is the threading
let bWin = dev.makeBuffer(length: nch * kw * 4, options: .storageModeShared)!
let bS   = dev.makeBuffer(length: nv * dv * dk * 4, options: .storageModeShared)!
memset(bWin.contents(), 0, nch * kw * 4)
memset(bS.contents(), 0, nv * dv * dk * 4)

// form_gdn_conv_f32 folds PER-CHANNEL taps (w[c*kw + j]) since the 2026-08-04 repair; the
// demo's one shared tap set is therefore bound REPLICATED, nch copies, which computes exactly
// what the old broadcast computed on this fixture and keeps the recorded expectations valid.
let tapsFull = (0..<nch).flatMap { _ in taps }
let bTaps = dev.makeBuffer(bytes: tapsFull, length: tapsFull.count * 4, options: .storageModeShared)!
let bZ    = dev.makeBuffer(bytes: zv, length: zv.count * 4, options: .storageModeShared)!
let bY    = dev.makeBuffer(length: nch * 4, options: .storageModeShared)!
let bO    = dev.makeBuffer(length: nv * dv * 4, options: .storageModeShared)!

var gpuErr = 0
func run(_ p: MTLComputePipelineState, _ width: Int, _ bind: (MTLComputeCommandEncoder) -> Void) {
    let cb = queue.makeCommandBuffer()!, e = cb.makeComputeCommandEncoder()!
    e.setComputePipelineState(p); bind(e)
    let tg = min(p.maxTotalThreadsPerThreadgroup, max(1, width))
    e.dispatchThreads(MTLSize(width: max(1, width), height: 1, depth: 1),
                      threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
    e.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if cb.error != nil || cb.status != .completed { gpuErr += 1 }
}

func step(_ x: [Float]) -> [Double] {
    let bX = dev.makeBuffer(bytes: x, length: x.count * 4, options: .storageModeShared)!
    var uNch = UInt32(nch), uKw = UInt32(kw)
    run(pConv, nch) { e in
        e.setBuffer(bWin, offset: 0, index: 0); e.setBuffer(bX, offset: 0, index: 1)
        e.setBuffer(bTaps, offset: 0, index: 2); e.setBuffer(bY, offset: 0, index: 3)
        e.setBytes(&uNch, length: 4, index: 4); e.setBytes(&uKw, length: 4, index: 5)
    }
    var uNk = UInt32(nk), uDk = UInt32(dk), eps: Float = 1e-6
    // q occupies [0, nk*dk); k occupies [nk*dk, 2*nk*dk); v the rest
    for off in [0, nk * dk] {
        run(pNorm, nk) { e in
            e.setBuffer(bY, offset: off * 4, index: 0)
            e.setBytes(&uNk, length: 4, index: 1); e.setBytes(&uDk, length: 4, index: 2)
            e.setBytes(&eps, length: 4, index: 3)
        }
    }
    var uDv = UInt32(dv)
    for j in 0..<nv {
        let src = (j * nk) / nv
        var a = Float(alphas[j]), b = Float(betas[j])
        run(pDelta, dv) { e in
            e.setBuffer(bS, offset: j * dv * dk * 4, index: 0)
            e.setBuffer(bY, offset: (nk * dk + src * dk) * 4, index: 1)   // k of the shared key head
            e.setBuffer(bY, offset: (2 * nk * dk + j * dv) * 4, index: 2) // v of this value head
            e.setBuffer(bY, offset: (src * dk) * 4, index: 3)             // q of the shared key head
            e.setBuffer(bO, offset: j * dv * 4, index: 4)
            e.setBytes(&a, length: 4, index: 5); e.setBytes(&b, length: 4, index: 6)
            e.setBytes(&uDv, length: 4, index: 7); e.setBytes(&uDk, length: 4, index: 8)
        }
    }
    var uN = UInt32(nv * dv)
    run(pGate, nv * dv) { e in
        e.setBuffer(bO, offset: 0, index: 0); e.setBuffer(bZ, offset: 0, index: 1)
        e.setBytes(&uN, length: 4, index: 2)
    }
    let p = bO.contents().bindMemory(to: Float.self, capacity: nv * dv)
    return (0..<(nv * dv)).map { Double(p[$0]) }
}

let got1 = step(x1)
let got2 = step(x2)
check(gpuErr == 0, "every command buffer completed without error")

func dev_(_ g: [Double], _ e: [Double]) -> (Double, Int) {
    var w = 0.0, at = -1
    for i in 0..<e.count {
        let d = abs(g[i] - e[i]) / max(abs(e[i]), 1e-30)
        if d > w { w = d; at = i }
    }
    return (w, at)
}
let (w1, a1) = dev_(got1, exp1)
let (w2, a2) = dev_(got2, exp2)
check(w1 <= envelope, String(format: "token 1 within %.0e — worst %.3e at %d", envelope, w1, a1))
check(w2 <= envelope, String(format: "token 2 within %.0e — worst %.3e at %d", envelope, w2, a2))

// THE STATE ACTUALLY CARRIED. Token 2's answer must differ from token 1's; if the device state
// were being reset the two tokens would still each be "close to something" but the layer would be
// memoryless. The recipe's own two outputs differ, so requiring the same of the device is a real
// claim and not a tautology.
// ABSOLUTE, not relative. A relative metric here divides by exp1's smallest element, which is
// near zero, and reports 1e+29 — a number that says "very different" while meaning nothing. A
// difference check and an error check want different metrics, and using the error metric for both
// because it was already written is how a meaningless magnitude ends up in a receipt.
var absDiff = 0.0
for i in 0..<exp1.count { absDiff = max(absDiff, abs(got2[i] - exp1[i])) }
check(absDiff > 1e-3, String(format: "token 2 differs from token 1 on the device — the state carried (abs %.4f)", absDiff))

print(failures == 0 ? "VERDICT PASS" : "VERDICT FAIL \(failures)")
exit(failures == 0 ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc"; tail -25 "$work/swift.err"; exit 1; }

out="$("$work/runner" "$work/gdn.metallib" "$work/demo.txt" "$ENVELOPE" 2>&1)"
echo "$out"
case "$out" in
    *"VERDICT PASS"*) echo; echo "metal_gdn_gpu: VERDICT PASS — the layer ran on the device and agrees with the recipe"; exit 0;;
    *"SKIP"*)         echo; echo "metal_gdn_gpu: SKIPPED — a skip is not a pass"; exit 2;;
    *)                echo; echo "metal_gdn_gpu: VERDICT FAIL"; exit 1;;
esac
