#!/usr/bin/env bash
# metal_route_wide_gpu.sh — DISPATCH the wide softmax router on the device and judge it against
# this body's own fp64 recipe.
#
# metal_route_wide.sh proved the body emits Metal and Metal accepts it. That is a syntax claim.
# This is the arithmetic claim: the kernel runs on an Apple GPU and its selection and weights are
# compared, element by element, against mm-route-weights computed in Form.
#
# THE JUDGE IS THE BODY'S OWN RECIPE, never a second hand-rolled reference — the discipline the
# DS4 GPU stones keep. And the recipe itself was checked against a closed form (i = 53*r mod 257,
# from the modular inverse of 97) that never enters a softmax, so the chain is:
#     closed-form integers  ->  Form fp64 recipe  ->  Metal f32 kernel
# with gate 3 re-checking the first link inside this harness rather than trusting it.
#
# TWO DIFFERENT KINDS OF CHECK, on purpose:
#   ids     EXACT. The fixture's logits sit >= 1/32 apart, ~6 orders above f32 epsilon near 1, so
#           no rounding can flip the selection. An exact gate here is honest, not lucky.
#   weights ENVELOPE. Recipe folds fp64, kernel folds f32. The bound is 1e-6 relative, stated
#           ahead of the run, and the worst observed deviation is printed whether it passes or not.
#
# Off-Mac or without swiftc this SKIPs with exit 2. A skip is not a pass.
set -u

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT" || exit 1
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT

ENVELOPE="1e-6"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null 2>&1; then
    echo "SKIP  no Darwin/swiftc on this host — the GPU witness needs an Apple GPU"; exit 2
fi
if [ ! -x ./fkwu ]; then
    echo "FAIL  no ./fkwu — cc -O2 -o fkwu runtime/fkwu-uni.c"; exit 1
fi

PRE='; preludes: form-stdlib/core.fk form-stdlib/str-byte-at.fk form-stdlib/equireach.fk form-stdlib/format-arith.fk form-stdlib/f16-decode.fk form-stdlib/q6k-dequant.fk form-stdlib/q6k-msl.fk form-stdlib/transformer-numerics.fk form-stdlib/moe-msl.fk form-stdlib/moe-route-radius.fk form-stdlib/moe-route-wide-demo.fk'

# ── 1. the fixture, emitted on fkwu (the runtime), not on a proof sibling ───────
{ echo "$PRE"; echo '(mrwd-emit-all)'; } > "$work/demo.fk"
# Take lines up to the emitter's own END sentinel. NEVER `grep -v '^0$'`: fkwu prints the
# top-level result on its own line, and filtering that by CONTENT also deletes any legitimate
# data line that happens to be exactly "0". This harness lost the one expert whose logit is
# precisely 0.0 that way, shifting every later index by one — and six of eight gates still
# passed, weights agreeing to 1.1e-7. A terminator and a datum printing the same character is
# the failure the TTS ingest named; the sentinel is content-independent and cannot confuse them.
./fkwu --src "$work/demo.fk" 2>"$work/demo.err" | sed -n '1,/^END$/p' > "$work/demo.txt"
grep -qx 'END' "$work/demo.txt" || { echo "FAIL  fixture stream truncated"; tail -5 "$work/demo.err"; exit 1; }
echo "PASS  fixture emitted on fkwu ($(wc -l < "$work/demo.txt" | tr -d ' ') lines)"

# ── 2. the MSL, emitted by the body, compiled to a metallib ────────────────────
{ echo '; preludes: form-stdlib/core.fk form-stdlib/moe-route-wide-msl.fk'; echo '(print_str (mrw-msl-appendix))'; } > "$work/emit.fk"
{ printf '#include <metal_stdlib>\ninline float fexp(float x) { return metal::exp(x); }\n'
  # `sed '$d'` drops fkwu's trailing result line BY POSITION, not by content.
  ./fkwu --src "$work/emit.fk" 2>/dev/null | sed '$d'; } > "$work/router.metal"
xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
      -c "$work/router.metal" -o "$work/router.air" 2>"$work/metal.err" \
  && xcrun -sdk macosx metallib "$work/router.air" -o "$work/router.metallib" 2>>"$work/metal.err" \
  || { echo "FAIL  metallib build"; head -10 "$work/metal.err"; exit 1; }
echo "PASS  body-emitted MSL compiled to a metallib"

# ── 3. dispatch and judge ──────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

let libPath = CommandLine.arguments[1]
let demoPath = CommandLine.arguments[2]
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
        if s == "ROUTEWIDEDEMO" || s == "END" { continue }
        guard let v = Double(s) else { continue }
        if mode == "S" { scalars[name] = v; mode = "" } else if mode == "V" { vecs[name]!.append(v) }
    }
}
let ne = Int(scalars["ne"]!), nsel = Int(scalars["nsel"]!)
let logits    = vecs["logitsE9"]!.map { Float($0 / 1e9) }
let expectIds = vecs["expectIds"]!.map { Int($0) }
let expectWts = vecs["expectWtsE9"]!.map { $0 / 1e9 }
let closedIds = vecs["closedFormIds"]!.map { Int($0) }

var failures = 0
func check(_ ok: Bool, _ msg: String) {
    print((ok ? "PASS  " : "FAIL  ") + msg); if !ok { failures += 1 }
}

// gate 3 — the judge is itself re-checked against the closed form, here, not on trust
check(expectIds == closedIds,
      "the Form recipe's ids equal the closed form i = 53*r mod 257 (\(expectIds.prefix(4))…)")

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP  no Metal device"); exit(2) }
let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
guard let fn = lib.makeFunction(name: "form_moe_route_wide_f32") else {
    print("FAIL  kernel form_moe_route_wide_f32 not in the metallib"); exit(1)
}
let pipe = try dev.makeComputePipelineState(function: fn)
let queue = dev.makeCommandQueue()!

let bLogits = dev.makeBuffer(bytes: logits, length: logits.count * 4, options: .storageModeShared)!
let bIds = dev.makeBuffer(length: max(4, nsel * 4), options: .storageModeShared)!
let bWts = dev.makeBuffer(length: max(4, nsel * 4), options: .storageModeShared)!
var uNe = UInt32(ne), uNsel = UInt32(nsel)

let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
enc.setComputePipelineState(pipe)
enc.setBuffer(bLogits, offset: 0, index: 0)
enc.setBuffer(bIds, offset: 0, index: 1)
enc.setBuffer(bWts, offset: 0, index: 2)
enc.setBytes(&uNe, length: 4, index: 3)
enc.setBytes(&uNsel, length: 4, index: 4)
enc.dispatchThreads(MTLSize(width: 1, height: 1, depth: 1),
                    threadsPerThreadgroup: MTLSize(width: 1, height: 1, depth: 1))
enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
check(cb.error == nil && cb.status == .completed,
      "the command buffer completed without error")

let gotIds = (0..<nsel).map { Int(bIds.contents().bindMemory(to: UInt32.self, capacity: nsel)[$0]) }
let gotWts = (0..<nsel).map { Double(bWts.contents().bindMemory(to: Float.self, capacity: nsel)[$0]) }

// gate 5 — EXACT. Selection cannot be flipped by f32 at this fixture's spacing.
check(gotIds == expectIds, "GPU ids match the recipe EXACTLY  gpu=\(gotIds)")

// gate 6 — ENVELOPE, with the worst deviation printed either way.
var worst = 0.0, worstAt = -1
for i in 0..<nsel {
    let d = abs(gotWts[i] - expectWts[i]) / max(abs(expectWts[i]), 1e-30)
    if d > worst { worst = d; worstAt = i }
}
check(worst <= envelope,
      String(format: "GPU weights within %.0e relative — worst %.3e at slot %d", envelope, worst, worstAt))

// gate 7 — the kernel's own renormalization holds on the device, not just in Form
let s = gotWts.reduce(0, +)
check(abs(s - 1.0) < 1e-5, String(format: "GPU weights sum to 1 on the device (got %.9f)", s))

// gate 8 — the radius refusal is live: ne beyond 256 must write NOTHING.
let sentinel: UInt32 = 0xDEADBEEF
bIds.contents().bindMemory(to: UInt32.self, capacity: nsel)[0] = sentinel
var bigNe = UInt32(257)
let cb2 = queue.makeCommandBuffer()!, e2 = cb2.makeComputeCommandEncoder()!
e2.setComputePipelineState(pipe)
e2.setBuffer(bLogits, offset: 0, index: 0); e2.setBuffer(bIds, offset: 0, index: 1)
e2.setBuffer(bWts, offset: 0, index: 2)
e2.setBytes(&bigNe, length: 4, index: 3); e2.setBytes(&uNsel, length: 4, index: 4)
e2.dispatchThreads(MTLSize(width: 1, height: 1, depth: 1),
                   threadsPerThreadgroup: MTLSize(width: 1, height: 1, depth: 1))
e2.endEncoding(); cb2.commit(); cb2.waitUntilCompleted()
let after = bIds.contents().bindMemory(to: UInt32.self, capacity: nsel)[0]
check(after == sentinel, "ne=257 is REFUSED — the sentinel survives untouched (radius is live)")

print(failures == 0 ? "VERDICT PASS" : "VERDICT FAIL \(failures)")
exit(failures == 0 ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc"; tail -20 "$work/swift.err"; exit 1; }

out="$("$work/runner" "$work/router.metallib" "$work/demo.txt" "$ENVELOPE" 2>&1)"
echo "$out"
rc=$?
case "$out" in
    *"VERDICT PASS"*) echo; echo "metal_route_wide_gpu: VERDICT PASS — the kernel ran on the device and agrees with the recipe"; exit 0;;
    *"SKIP"*)         echo; echo "metal_route_wide_gpu: SKIPPED (no device) — a skip is not a pass"; exit 2;;
    *)                echo; echo "metal_route_wide_gpu: VERDICT FAIL"; exit 1;;
esac
