#!/usr/bin/env bash
# metal_weight_residency_audit.sh — GPU_GAPS §C "Weight load -> device", made a witness.
#
# The claim, and nothing wider: REAL llama3.2:3b Q6_K weights, located by NAME in the 2 GB blob and
# dequantized by Form's own recipes, are uploaded ONCE into an MTLBuffer, stay RESIDENT there across
# many dispatches, and drive the already-proven Form-emitted Metal matvec kernel. Everything on both
# sides of that seam was green before; the seam itself was the ⬜.
#
# Who decides what (the dumb-carrier discipline the Metal/PTX lanes already keep):
#   the BODY  form/native/metal/q6k-device-tile.fk — which tensor, where its bytes are (egg-find-tensor
#             over the file's own 7.8 MB header), what every weight IS (ewl-weights, the equireach Q6_K
#             reach), and what the answer should be (tb-matvec's right fold).
#   the BODY  form-stdlib/jit-tensor-emit.fk — the MSL kernel TEXT (jte-matvec-msl). Not one character
#             of the kernel is authored here.
#   the CARRIER (this file + the Swift runner it writes) — parse, upload, dispatch, read back, compare.
#
# THREE GATES, and the first two are bit-exact:
#   1  RESIDENCY IS FAITHFUL      the device weight buffer, read back after upload, equals the f32
#                                 rounding of every one of the rows*cols weights Form dequantized.
#                                 Bit-exact, all of them, no sampling. This is the gap row's content.
#   2  THE RESIDENT BUFFER DRIVES the GPU y equals an f32 right-fold (j counts DOWN, mul then add as
#      THE PROVEN KERNEL          two roundings) over that same resident buffer — the lane's own
#                                 conversion chain, per format-arith. Bit-exact, every row.
#   3  IT IS THE BODY'S ANSWER    the GPU y matches Form's fp64 tb-matvec within the DERIVED bound for
#                                 a sequential fp32 sum, cols * 2^-24 * SUM|term|. An fp32 accumulator
#                                 over `cols` terms cannot equal an fp64 one, and a round tolerance
#                                 pulled out of the air would be a fudge; the bound is the arithmetic's
#                                 own. The value-relative error is printed alongside (it is larger,
#                                 8.9e-05 at 256x256, because some rows' products cancel).
#   plus RESIDENCY IS REAL        the buffer is written once and dispatched ITERS times with no
#                                 re-upload; the audit prints per-dispatch wall time and re-checks the
#                                 output checksum after the last dispatch.
#
# Run:  form/native/metal/metal_weight_residency_audit.sh [rows cols iters]   (defaults 256 256 200)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every other Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
ROWS="${1:-256}"; COLS="${2:-256}"; ITERS="${3:-200}"
BLOB="${FORM_GGUF_BLOB:-$HOME/.ollama/models/blobs/sha256-dde5aa3fc5ffc17176b5e8bdc82f587b24b2678c6c66101bf7da77af9f7ccdff}"
WTENSOR="${FORM_W_TENSOR:-blk.0.ffn_down.weight}"
XTENSOR="${FORM_X_TENSOR:-blk.0.attn_norm.weight}"

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

work="$(mktemp -d "${TMPDIR:-/tmp}/fkresident.XXXXXX")"
trap 'rm -rf "$work"' EXIT

# ── the `; preludes:` directives are LIVE recursive load instructions; they are walked, never
#    hand-catted. Same expansion validate.sh uses to build a kernel's argument list. ────────────
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
while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/q6k-device-tile.fk)

# ── 1. the body emits the MSL kernel (jte-matvec-msl); the carrier authors none of it ──────────
echo '(qdt-emit-msl "form_matvec_f32")' > "$work/emit_msl.fk"
"$GO_BIN" "${FILES[@]}" "$work/emit_msl.fk" > "$work/emit_msl.out" 2>"$work/emit_msl.err" || {
    echo "FAIL  MSL emission failed — see $work/emit_msl.err"; cat "$work/emit_msl.err"; exit 1; }
# keep exactly the emitted kernel: the kernel also echoes the program's own value on the last line
grep '^kernel void ' "$work/emit_msl.out" > "$work/matvec.metal"
grep -q 'kernel void form_matvec_f32' "$work/matvec.metal" || {
    echo "FAIL  emission did not produce the MSL kernel"; cat "$work/matvec.metal"; exit 1; }
echo "emitted MSL: $(wc -c < "$work/matvec.metal" | tr -d ' ') bytes, every byte authored by the Form recipe"

# ── 2. the body locates the tensor, dequantizes the tile, and states the reference ─────────────
echo "locating $WTENSOR by name in the real blob (walking its own 7.8 MB header) and dequantizing ${ROWS}x${COLS}..."
printf '(qdt-emit "%s" "%s" "%s" %s %s)\n' "$BLOB" "$WTENSOR" "$XTENSOR" "$ROWS" "$COLS" > "$work/tile.fk"
tile_start=$(date +%s)
"$GO_BIN" "${FILES[@]}" "$work/tile.fk" > "$work/tile.txt" 2>"$work/tile.err" || {
    echo "FAIL  tile emission failed — see $work/tile.err"; tail -5 "$work/tile.err"; exit 1; }
tile_secs=$(( $(date +%s) - tile_start ))
# the stream must be complete. (The last LINE is the kernel echoing the program's own value, so the
# END marker is the second-to-last line, not the last — check for the marker, not for its position.)
grep -qx 'END' "$work/tile.txt" || { echo "FAIL  tile stream is truncated — no END marker"; exit 1; }
sed -n '1,5p' "$work/tile.txt"
echo "body time (locate + dequant ${ROWS}x${COLS} + fp64 reference): ${tile_secs}s"

# ── 3. the carrier: upload once, keep resident, dispatch many, compare ─────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
// metal weight-residency witness. Carrier only: no numeric decision is made here that the body
// did not already make. The f32 reference below is the LANE's own conversion chain (load exact ->
// fp32 right fold, j counting DOWN, mul then add as two roundings) — the same op order the Form
// recipe emitted into the MSL text, so bit-exactness is the honest gate, not a tolerance.
import Metal
import Foundation

let a = CommandLine.arguments
let mslPath = a[1], tilePath = a[2], fname = a[3], iters = Int(a[4])!

// --- parse the body's stream -------------------------------------------------------------------
var rows = 0, cols = 0
var W: [Double] = [], X: [Double] = [], Yref: [Double] = []
do {
    var section = ""
    for line in try String(contentsOfFile: tilePath, encoding: .utf8).split(separator: "\n", omittingEmptySubsequences: false) {
        let s = String(line)
        if s.hasPrefix("ROWS ") { rows = Int(s.dropFirst(5))!; continue }
        if s.hasPrefix("COLS ") { cols = Int(s.dropFirst(5))!; continue }
        if s == "W" || s == "X" || s == "Y" { section = s; continue }
        if s == "END" { break }
        if s.hasPrefix("TENSOR") || s.hasPrefix("TYPE") || s.hasPrefix("D0")
            || s.hasPrefix("D1") || s.hasPrefix("ABS") || s.isEmpty { continue }
        guard let v = Double(s) else { continue }
        switch section { case "W": W.append(v); case "X": X.append(v); case "Y": Yref.append(v); default: break }
    }
}
guard rows > 0, cols > 0, W.count == rows * cols, X.count == cols, Yref.count == rows else {
    print("FAIL  stream shape: rows=\(rows) cols=\(cols) W=\(W.count) X=\(X.count) Y=\(Yref.count)"); exit(1)
}
// one rounding, fp64 -> f32: this is the only conversion the carrier performs on the body's numbers
let Wf = W.map { Float($0) }, Xf = X.map { Float($0) }

// --- device, kernel, buffers -------------------------------------------------------------------
guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let opts = MTLCompileOptions()
opts.mathMode = .safe   // IEEE-conformant: no fast-math reassociation or contraction
let lib = try dev.makeLibrary(source: try String(contentsOfFile: mslPath, encoding: .utf8), options: opts)
let pipe = try dev.makeComputePipelineState(function: lib.makeFunction(name: fname)!)
let queue = dev.makeCommandQueue()!

// Counted across every dispatch in this run. A command buffer that FAILS writes nothing,
// and ybuf below is freshly allocated and therefore zeroed — so an unchecked failure does
// not read as an error, it reads as a matvec that disagrees with Form. This is the
// difference between "the GPU is wrong" and "the GPU did not run" (axiom-4: passage not
// through the offered interface — cb.status/cb.error — is breach, and breach is observable).
var gpuErrors = 0
var gpuFirstError: String? = nil

// THE RESIDENT WEIGHTS. Written once, never rewritten, alive for every dispatch below.
let wbuf = dev.makeBuffer(bytes: Wf, length: Wf.count * 4, options: .storageModeShared)!
let xbuf = dev.makeBuffer(bytes: Xf, length: Xf.count * 4, options: .storageModeShared)!
let ybuf = dev.makeBuffer(length: rows * 4, options: .storageModeShared)!
var r32 = UInt32(rows), c32 = UInt32(cols)
print("resident: weights \(Wf.count * 4) bytes on \(dev.name), uploaded once")

// --- GATE 1: the device buffer IS the body's weights, bit for bit -------------------------------
let wback = wbuf.contents().bindMemory(to: Float.self, capacity: Wf.count)
var badResident = 0, firstBad = -1
for i in 0..<Wf.count where wback[i].bitPattern != Wf[i].bitPattern {
    badResident += 1; if firstBad < 0 { firstBad = i }
}
if badResident == 0 { print("PASS  gate 1 residency bit-exact: all \(Wf.count) f32 weights on the device equal Form's") }
else { print("FAIL  gate 1: \(badResident) resident weights differ, first at index \(firstBad)") }

func dispatchOnce() {
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(pipe)
    enc.setBuffer(wbuf, offset: 0, index: 0)   // <- the SAME resident buffer, every time
    enc.setBuffer(xbuf, offset: 0, index: 1)
    enc.setBuffer(ybuf, offset: 0, index: 2)
    enc.setBytes(&r32, length: 4, index: 3)
    enc.setBytes(&c32, length: 4, index: 4)
    let tg = min(pipe.maxTotalThreadsPerThreadgroup, 256)
    enc.dispatchThreads(MTLSize(width: rows, height: 1, depth: 1),
                        threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let e = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(e)" } }
    if cb.status != .completed { gpuErrors += 1
        if gpuFirstError == nil { gpuFirstError = "command buffer status \(cb.status.rawValue), not .completed" } }
}

// --- GATE 0: DID THE GPU RUN. Asked before gates 2/3, which read ybuf back — and a buffer
// nothing wrote reads as zeros, a NUMBER that gate 2/3 would grade as an arithmetic
// disagreement. The CPU writes a sentinel no matvec would produce; the kernel must overwrite
// every one of the `rows` outputs. If any survives, this is a residency condition, not an
// arithmetic one, and the harness says so instead of grading silence.
do {
    let sentinel: Float = -424242.0
    let yp = ybuf.contents().bindMemory(to: Float.self, capacity: rows)
    for i in 0..<rows { yp[i] = sentinel }
    let before = gpuErrors
    dispatchOnce()
    var survived = 0
    for i in 0..<rows where yp[i] == sentinel { survived += 1 }
    if gpuErrors > before { print("      command buffer ERROR: \(gpuFirstError ?? "unknown")") }
    if gpuErrors == before && survived == 0 {
        print("PASS  gate 0 the GPU executes: a kernel overwrote all \(rows) sentinels, no command buffer errored")
    } else {
        print("FAIL  gate 0 THE GPU DID NOT RUN — \(survived)/\(rows) sentinels survived, \(gpuErrors - before) cb error(s).")
        print("      This is a RESIDENCY condition, not an arithmetic one. Gates 2 and 3 below would grade")
        print("      an unwritten buffer as disagreement; the verdict refuses them. Free memory and re-run.")
        print("VERDICT FAIL"); exit(1)
    }
}

dispatchOnce()
let y = ybuf.contents().bindMemory(to: Float.self, capacity: rows)
var gpu = [Float](repeating: 0, count: rows)
for i in 0..<rows { gpu[i] = y[i] }

// --- GATE 2: an f32 right fold over the resident buffer, the kernel's own op order ---------------
var badLane = 0, firstLane = -1
for i in 0..<rows {
    var acc: Float = 0.0
    var j = cols
    while j > 0 { j -= 1; let p = wback[i * cols + j] * Xf[j]; acc = p + acc }
    if acc.bitPattern != gpu[i].bitPattern { badLane += 1; if firstLane < 0 { firstLane = i } }
}
if badLane == 0 { print("PASS  gate 2 kernel bit-exact: all \(rows) GPU rows equal the f32 right-fold over the resident buffer") }
else { print("FAIL  gate 2: \(badLane) rows differ, first at row \(firstLane)") }

// --- GATE 3: the body's fp64 answer, within a DERIVED bound --------------------------------------
// The GPU accumulates in fp32 and Form in fp64, so equality is impossible and a round number pulled
// out of the air would be a fudge. The bound is the textbook one for a sequential fp32 sum of `cols`
// terms: |fl(S) - S| <= cols * u * SUM|term|, with u = 2^-24. The condition number SUM|term|/|S| is
// what makes the plain value-relative error large — a row whose products cancel has a tiny |S| and a
// correspondingly amplified relative error, and that is arithmetic, not a defect. Both numbers are
// printed: the value-relative error (which the condition number inflates) and the bound-relative one
// (the honest gate). Measured at 256x256 on the real weights: value-relative max 8.902e-05, which is
// exactly the cancelling-row story, while the derived bound holds with room to spare.
let u = 5.960464477539063e-08   // 2^-24, the f32 unit roundoff
var maxRel = 0.0, worstRatio = 0.0, worstCond = 0.0
for i in 0..<rows {
    let ref = Yref[i], got = Double(gpu[i])
    maxRel = max(maxRel, abs(got - ref) / max(abs(ref), 1e-30))
    var absSum = 0.0
    for j in 0..<cols { absSum += abs(Double(wback[i * cols + j]) * Double(Xf[j])) }
    let bound = Double(cols) * u * absSum
    if bound > 0 { worstRatio = max(worstRatio, abs(got - ref) / bound) }
    worstCond = max(worstCond, absSum / max(abs(ref), 1e-30))
}
let gate3 = worstRatio < 1.0
print(String(format: "      value-relative max %.3e (worst row condition number %.1f — cancellation, not error)", maxRel, worstCond))
if gate3 { print(String(format: "PASS  gate 3 derived bound: max |gpu-form| is %.3f of the cols*u*SUM|term| fp32 summation bound", worstRatio)) }
else { print(String(format: "FAIL  gate 3: max |gpu-form| is %.3f of the fp32 summation bound — beyond what the accumulator width explains", worstRatio)) }
let maxRelOK = gate3

// --- RESIDENCY IS REAL: many dispatches, one upload ----------------------------------------------
let t0 = Date()
for _ in 0..<iters { dispatchOnce() }
let dt = Date().timeIntervalSince(t0)
var sumAfter = 0.0
for i in 0..<rows { sumAfter += Double(y[i]) }
var sumFirst = 0.0
for i in 0..<rows { sumFirst += Double(gpu[i]) }
let stable = (sumAfter == sumFirst)
print(String(format: "resident dispatches: %d in %.4f s (%.1f us each), zero re-uploads; checksum after == checksum before: %@",
             iters, dt, dt / Double(iters) * 1e6, stable ? "yes" : "NO"))
if gpuErrors > 0 {
    print("=== \(gpuErrors) COMMAND BUFFER(S) FAILED during this run — first: \(gpuFirstError ?? "unknown") ===")
    print("    Nothing above this line that reads ybuf back can be trusted.")
}
if badResident == 0 && badLane == 0 && maxRelOK && stable && gpuErrors == 0 { print("VERDICT PASS") } else { print("VERDICT FAIL"); exit(1) }
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc could not build the carrier — see below"; cat "$work/swift.err"; exit 1; }
"$work/runner" "$work/matvec.metal" "$work/tile.txt" form_matvec_f32 "$ITERS"
rc=$?
exit $rc
