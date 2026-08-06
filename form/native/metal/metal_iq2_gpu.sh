#!/usr/bin/env bash
# metal_iq2_gpu.sh — GGUF type 16 (IQ2_XXS) on the GPU, made a witness.
#
# THE CLAIM, and nothing wider: the IQ2_XXS quantized bytes of the DeepSeek V4 Flash file live on the GPU
# unchanged, and a Metal kernel the BODY emitted (iq2xxs-msl.fk) dequantizes them THERE, bit-exactly
# equal to the CPU carver (iq2xxs-dequant.fk), at the HEAD block (expert 0) AND at a DISTANT block
# (expert 100 and the LAST expert) of blk.0.ffn_down_exps.weight — a real type-16 tensor. IQ2_XXS is the
# single largest byte-share of the model (42.6%, the expert weights of 31 of 43 layers) and was the last
# dequant type still CPU-only.
#
# WHAT THIS DOES NOT CLAIM, stated before the gates.
#   * The GEOMETRY (66 B / 256 el) is gguf-manifest.fk's, proven from THIS file's own offset chain. A GPU
#     agreeing with the CPU carver proves the TRANSCRIPTION and the residency. But UNLIKE types 40/41,
#     the IQ2 carver itself already carries BIT-EXACTNESS against an independent from-spec oracle
#     (iq2xxs-dequant-band = 2^30-1 over 61 952 real weights), so this witness inherits that grade: the
#     device now equals a reading that equals the spec.
#   * A FUSED matvec-over-IQ2 is NOT built here — this is the dequant kernel. The receipt records whether
#     the fused path is reachable on IQ2's block geometry.
#
# Who decides what (the dumb-carrier discipline):
#   the BODY  native/metal/iq2-residency.fk  — where the tensor is, its block geometry, what the weights
#                                              ARE at any block (the CPU reference), and the Metal source.
#   the BODY  form-stdlib/iq2xxs-msl.fk       — the Metal source. Not one character here.
#   the CARRIER (this file + the Swift runner it writes) — mmap, bind, dispatch, compare.
#
# THE GATES:
#   0  DID THE GPU RUN            a CPU-sentinelled buffer must be fully overwritten by a real dequant off
#                                 the resident file; a survived sentinel is residency, not arithmetic
#                                 (edgedrop/zerobirth — an unrun kernel reads as a computed zero)
#   1  HEAD BLOCK BIT-EXACT       all 256 GPU weights of expert 0, block 0 equal the CPU carver's, bit for
#                                 bit, no epsilon (the carver's exactness: w is an exact f32)
#   2  DISTANT BLOCK BIT-EXACT    expert 100 (216 268 800 bytes on) — snugcause: a layout that fits the
#                                 head is tested far away, and the expert stride is what is under test
#   3  LAST-EXPERT BLOCK EXACT    expert 255 (the far wall of the tensor) — unispan, the widest reach
#   4  PARITY IS CARRIED          the ksigns table on the device reproduces the 8th-of-8 sign at every one
#                                 of the 32 octets of the head block (paritylock)
#   5  RESIDENCY IS REAL          ITERS dispatches, zero re-uploads, checksum stable
#
# Run:  form/native/metal/metal_iq2_gpu.sh [iters]      (default 100)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every other Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
ITERS="${1:-100}"
BLOB="${FORM_DS4_BLOB:-$HOME/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf}"
TENSOR="${FORM_IQ2_TENSOR:-blk.0.ffn_down_exps.weight}"
CACHE="$ROOT/native/metal/.metallib-cache"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the GPU witness needs an Apple GPU + swiftc"
    exit 2
fi
if [[ ! -f "$BLOB" ]]; then
    echo "SKIP  the ds4 GGUF is not on this host: $BLOB   (set FORM_DS4_BLOB)"
    exit 2
fi
if [[ ! -x "$GO_BIN" ]]; then
    echo "  building go kernel..." >&2
    (cd "$ROOT/form-kernel-go" && go build -o bin-go .)
fi

FSIZE=$(stat -f%z "$BLOB")
echo "ds4 blob: $FSIZE bytes at $(date '+%H:%M:%S')"

work="$(mktemp -d "${TMPDIR:-/tmp}/fkiq2.XXXXXX")"
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
while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/iq2-residency.fk)

# ── 1. the body emits the Metal source (one kernel, two constant tables, no header) ────────────
echo '(iqr-emit-msl)' > "$work/msl.fk"
"$GO_BIN" "${FILES[@]}" "$work/msl.fk" > "$work/msl.out" 2>"$work/msl.err" || {
    echo "FAIL  MSL emission failed"; cat "$work/msl.err"; exit 1; }
awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/msl.out" > "$work/iq2.metal"
MSL="$work/iq2.metal"
grep -q 'kernel void form_iq2xxs_dequant_f32' "$MSL" || { echo "FAIL  kernel not emitted"; exit 1; }
# gate: exactly ONE of each table and helper, and NO metal_stdlib / using-namespace (the unit is bare)
ngrid=$(grep -c 'constant uchar iq2_grid' "$MSL"); nksig=$(grep -c 'constant uchar iq2_ksigns' "$MSL")
nhdr=$(grep -c 'metal_stdlib' "$MSL"); nusing=$(grep -c 'using namespace' "$MSL")
nf16=$(grep -o 'float iq2_f16' "$MSL" | wc -l | tr -d ' ')
[[ "$ngrid" == 1 && "$nksig" == 1 && "$nhdr" == 0 && "$nusing" == 0 && "$nf16" == 1 ]] || {
    echo "FAIL  emit shape: grid $ngrid ksigns $nksig header $nhdr using $nusing f16 $nf16 (want 1 1 0 0 1)"; exit 1; }
# the tables carry their full complement: 2048 grid entries (2047 commas), 128 ksigns (127 commas)
gcommas=$(grep 'iq2_grid\[2048\]' "$MSL" | tr -cd ',' | wc -c | tr -d ' ')
kcommas=$(grep 'iq2_ksigns\[128\]' "$MSL" | tr -cd ',' | wc -c | tr -d ' ')
[[ "$gcommas" == 2047 && "$kcommas" == 127 ]] || { echo "FAIL  table entry counts: grid commas $gcommas (want 2047), ksigns commas $kcommas (want 127)"; exit 1; }
echo "PASS  emit: $(wc -c < "$MSL" | tr -d ' ') bytes, 1 kernel, grid[2048]+ksigns[128] as constant arrays, NO header (no library call), every byte authored by iq2xxs-msl.fk"

# ── 2. the .metallib, cached across runs by the source's own sha256 ────────────────────────────
mkdir -p "$CACHE"
msl_sha="$(shasum -a 256 "$MSL" | cut -c1-16)"
LIB="$CACHE/iq2-$msl_sha.metallib"
if [[ -f "$LIB" ]]; then
    echo "PASS  metallib cache HIT: $(basename "$LIB")"
else
    xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
          -c "$MSL" -o "$work/iq2.air" 2>"$work/metal.err" \
      && xcrun -sdk macosx metallib "$work/iq2.air" -o "$LIB" 2>>"$work/metal.err" || {
        echo "FAIL  offline metal compile failed"; cat "$work/metal.err"; exit 1; }
    echo "PASS  metallib compiled and cached: $(basename "$LIB")"
fi

# ── 3. the body: the tensor's geometry, and the three block offsets ────────────────────────────
printf '(iqr-emit-info "%s" "%s")\n' "$BLOB" "$TENSOR" > "$work/info.fk"
"$GO_BIN" "${FILES[@]}" "$work/info.fk" > "$work/info.out" 2>"$work/info.err" || {
    echo "FAIL  info emission failed"; tail -5 "$work/info.err"; exit 1; }
INFO=($(awk '$1=="INFO"{print; exit}' "$work/info.out"))
[[ "${#INFO[@]}" -eq 11 ]] || { echo "FAIL  INFO line malformed: ${INFO[*]}"; exit 1; }
# INFO abs type ndims d0 d1 d2 nel slices slicebytes total
ABS=${INFO[1]}; TYPE=${INFO[2]}; NEL=${INFO[7]}; SLICES=${INFO[8]}; SB=${INFO[9]}; TBYTES=${INFO[10]}
[[ "$TYPE" == 16 ]] || { echo "FAIL  $TENSOR is ggml type $TYPE, not IQ2_XXS(16)"; exit 1; }
FAR=100
LAST=$(( SLICES - 1 ))
HEAD_OFF=$ABS
FAR_OFF=$(( ABS + FAR * SB ))
LAST_OFF=$(( ABS + LAST * SB ))
echo "  $TENSOR: type 16, ${INFO[4]}x${INFO[5]}x${INFO[6]}, $SLICES experts of $NEL elements ($SB B each), $TBYTES B at $ABS"

END16=$(( ABS + TBYTES ))
if (( END16 > FSIZE )); then
    echo "SKIP  $TENSOR ends at $END16 but the file is only $FSIZE — re-run when it grows"
    exit 2
fi
echo "  offsets checked against the live file: the tensor ends at $END16, $(( FSIZE - END16 )) bytes inside the file"

# ── 4. the body: the CPU reference for the three blocks ────────────────────────────────────────
{
  printf '(iqr-emit-block "%s" %s)\n' "$BLOB" "$HEAD_OFF"
  printf '(iqr-emit-block "%s" %s)\n' "$BLOB" "$FAR_OFF"
  printf '(iqr-emit-block "%s" %s)\n' "$BLOB" "$LAST_OFF"
} > "$work/ref.fk"
"$GO_BIN" "${FILES[@]}" "$work/ref.fk" > "$work/ref.txt" 2>"$work/ref.err" || {
    echo "FAIL  reference emission failed"; tail -5 "$work/ref.err"; exit 1; }
nblk=$(grep -c '^BLK' "$work/ref.txt")
[[ "$nblk" == 3 ]] || { echo "FAIL  expected 3 reference blocks, got $nblk"; exit 1; }

# ── 5. the carrier ─────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
// IQ2-on-the-GPU witness. Carrier only: every number it JUDGES came from the body.
import Metal
import Foundation

let a = CommandLine.arguments
let libPath = a[1], refPath = a[2], blobPath = a[3]
let iters = Int(a[4])!
let headOff = Int(a[5])!, farOff = Int(a[6])!, lastOff = Int(a[7])!

// reference blocks, keyed by file offset, in the order BLK lines appear
var refs: [Int: [Double]] = [:]
var order: [Int] = []
do {
    var cur = -1
    for line in try String(contentsOfFile: refPath, encoding: .utf8).split(separator: "\n", omittingEmptySubsequences: false) {
        let s = String(line)
        if s.hasPrefix("BLK ") { cur = Int(s.split(separator: " ")[1])!; refs[cur] = []; order.append(cur); continue }
        // cap at 256 per block: bin-go prints the program's final return value (0) after the last block,
        // which parses as a valid Double — a 257th number that is not a weight. Ignore anything past 256.
        if let v = Double(s), cur >= 0, refs[cur]!.count < 256 { refs[cur]!.append(v) }
    }
}
guard let head = refs[headOff], let far = refs[farOff], let last = refs[lastOff],
      head.count == 256, far.count == 256, last.count == 256 else {
    print("FAIL  reference block shape (head \(refs[headOff]?.count ?? -1), far \(refs[farOff]?.count ?? -1), last \(refs[lastOff]?.count ?? -1))"); exit(1)
}

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
let queue = dev.makeCommandQueue()!
var failures = 0, dispatches = 0, gpuErrors = 0
var gpuFirstError: String? = nil
func check(_ ok: Bool, _ pass: String, _ fail: String) {
    if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 }
}

// the resident bytes: mmap the whole file, then hand the GPU a PAGE-ALIGNED WINDOW of the mapped pages
// covering all three blocks (bytesNoCopy = no copy). The whole 91 GB file exceeds Metal's maxBufferLength
// (~one MTLBuffer cannot span it), and it need not: the three blocks live in a ~553 MB stretch of the
// tensor, so the window is that stretch. mmap is still over the whole file — only the buffer is windowed,
// and the bytes it exposes are the real, unmodified file pages.
let fd = open(blobPath, O_RDONLY)
guard fd >= 0 else { print("FAIL  cannot open blob"); exit(1) }
var st = stat(); fstat(fd, &st)
let fileLen = Int(st.st_size)
let page = Int(getpagesize())
let mapLen = (fileLen + page - 1) / page * page
guard let mapped = mmap(nil, mapLen, PROT_READ, MAP_PRIVATE, fd, 0), mapped != MAP_FAILED else {
    print("FAIL  mmap failed"); exit(1)
}
// page-aligned window [winStart, winEnd) covering headOff..lastOff+66
let winStart = (min(headOff, min(farOff, lastOff)) / page) * page
let winEndRaw = max(headOff, max(farOff, lastOff)) + 66
let winEnd = (winEndRaw + page - 1) / page * page
let winLen = winEnd - winStart
if winLen > dev.maxBufferLength {
    print("FAIL  window \(winLen) B exceeds device maxBufferLength \(dev.maxBufferLength) B"); exit(1)
}
guard let modelBuf = dev.makeBuffer(bytesNoCopy: mapped.advanced(by: winStart), length: winLen, options: .storageModeShared, deallocator: nil) else {
    print("FAIL  makeBuffer(bytesNoCopy:) over the mapped window failed"); exit(1)
}
print(String(format: "resident: a %ld B page-aligned window of the %ld B ds4 file mapped into one MTLBuffer on %@ (maxBufferLength %ld B), ZERO copies", winLen, fileLen, dev.name, dev.maxBufferLength))

let p = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_iq2xxs_dequant_f32")!)
let outBuf = dev.makeBuffer(length: 256 * 4, options: .storageModeShared)!
let og = outBuf.contents().bindMemory(to: Float.self, capacity: 256)

// dequant one 256-weight block at file offset `blkoff`: bind the model buffer AT that offset (a 64-bit
// Int, so no 32-bit file offset is ever formed for a 91 GB file — the kernel indexes from 0), off=0.
func dequantBlock(_ blkoff: Int) {
    var o32 = UInt32(0), c32 = UInt32(256)
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(p)
    enc.setBuffer(modelBuf, offset: blkoff - winStart, index: 0)
    enc.setBuffer(outBuf, offset: 0, index: 1)
    enc.setBytes(&o32, length: 4, index: 2)
    enc.setBytes(&c32, length: 4, index: 3)
    let tg = min(p.maxTotalThreadsPerThreadgroup, 256)
    enc.dispatchThreads(MTLSize(width: 256, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let e = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(e)" } }
    if cb.status != .completed { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "cb status \(cb.status.rawValue)" } }
    dispatches += 1
}
func bitBad(_ g: UnsafeMutablePointer<Float>, _ r: [Double]) -> Int {
    var bad = 0
    for i in 0..<256 where g[i].bitPattern != Float(r[i]).bitPattern { bad += 1 }
    return bad
}

// GATE 0: did the GPU run. A block nothing wrote is zeros — a NUMBER the bit-exact gates would grade as
// disagreement. Sentinel, dequant, demand every sentinel overwritten and no cb error (edgedrop/zerobirth).
do {
    let sentinel: Float = -424242.0
    for i in 0..<256 { og[i] = sentinel }
    let before = gpuErrors
    dequantBlock(headOff)
    var survived = 0
    for i in 0..<256 where og[i] == sentinel { survived += 1 }
    if gpuErrors > before { print("  command buffer ERROR: \(gpuFirstError ?? "unknown")") }
    check(gpuErrors == before && survived == 0,
      "gate 0 the GPU executes: an IQ2_XXS dequant off the resident file overwrote all 256 sentinels, no cb error",
      "gate 0 THE GPU DID NOT RUN — \(survived)/256 sentinels survived, \(gpuErrors - before) cb error(s)")
    if failures > 0 { print("VERDICT FAIL  the GPU did not run; no IQ2 arithmetic was witnessed"); exit(1) }
}

// gate 1: head block bit-exact
dequantBlock(headOff)
let bh = bitBad(og, head)
check(bh == 0, "gate 1 HEAD block bit-exact: all 256 GPU weights of expert 0 block 0 equal the CPU carver's",
              "gate 1 HEAD: \(bh) of 256 weights differ from the carver's")

// gate 4 (measured on the head block we just have): paritylock — the 8th sign of all 32 octets. Weight
// l*8+7 of each octet o (o = ib32*4 + l) carries the parity-born 8th sign; if the device dropped the
// ksigns table it would be wrong here, invisibly, at up to 1-in-8. Read from the SAME GPU result.
var parityBad = 0
for o in 0..<32 {
    let idx = o * 8 + 7
    if og[idx].bitPattern != Float(head[idx]).bitPattern { parityBad += 1 }
}
check(parityBad == 0,
  "gate 4 PARITYLOCK: the device ksigns table reproduces the 8th-of-8 sign at all 32 octets of the head block (a dropped table would be wrong 1-in-8, invisibly)",
  "gate 4 PARITYLOCK: \(parityBad) of 32 octet-parity signs differ — the 8th sign is not carried")

// gate 2: distant block (expert 100)
dequantBlock(farOff)
let bf = bitBad(og, far)
check(bf == 0, "gate 2 DISTANT block bit-exact: all 256 weights of expert 100 (\(farOff - headOff) B on — the expert stride is under test) equal the carver's",
              "gate 2 DISTANT: \(bf) of 256 weights of expert 100 differ from the carver's")

// gate 3: last expert (far wall)
dequantBlock(lastOff)
let bl = bitBad(og, last)
check(bl == 0, "gate 3 LAST-EXPERT block bit-exact: all 256 weights of the final expert (\(lastOff - headOff) B on, the far wall of the tensor) equal the carver's",
              "gate 3 LAST-EXPERT: \(bl) of 256 weights differ from the carver's")

// gate 5: residency — many dispatches, zero re-uploads, checksum stable
dequantBlock(headOff)
var sumFirst = 0.0; for i in 0..<256 { sumFirst += Double(og[i]) }
let before = dispatches
let t0 = Date()
for _ in 0..<iters { dequantBlock(headOff) }
let secs = Date().timeIntervalSince(t0)
var sumAfter = 0.0; for i in 0..<256 { sumAfter += Double(og[i]) }
check(sumAfter == sumFirst,
  String(format: "gate 5 residency: %d dequant dispatches in %.4f s (%.0f us each) with ZERO re-uploads; head-block checksum unchanged", iters, secs, secs/Double(iters)*1e6),
  "gate 5: the head-block checksum changed across dispatches")
print("      dispatch count: \(before) for the correctness audit, \(dispatches - before) more for the residency loop — seamtoll: a full command buffer per dispatch on purpose, these are correctness numbers, not a token rate")

if gpuErrors > 0 {
    print("=== \(gpuErrors) COMMAND BUFFER(S) FAILED — first: \(gpuFirstError ?? "unknown") ===")
}
let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  6 gates, IQ2_XXS on the GPU") } else { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc failed"; tail -20 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB" "$work/ref.txt" "$BLOB" "$ITERS" "$HEAD_OFF" "$FAR_OFF" "$LAST_OFF"
rc=$?
exit $rc
