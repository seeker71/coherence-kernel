#!/usr/bin/env bash
# metal_windowed_residency.sh — the whole 85 GiB model made resident on the GPU as OVERLAPPING VIEWS.
#
# THE CLAIM, and nothing wider: the DeepSeek V4 Flash file is 91 321 404 640 B and the M4 Max
# maxBufferLength is 86 586 540 032 B, so NO single MTLBuffer can span it (onelean, corpus row 850 —
# makeBuffer(bytesNoCopy:) over the whole mmap FAILs). The body's windowed-residency.fk computes a set of
# overlapping page-aligned views of the one mmap such that EVERY one of the 1406 tensors lies wholly
# inside at least one, plus the lookup (tensor) -> (view index, inner offset). This harness proves, on the
# LIVE file:
#   * all the views actually map on the real device (the thing one buffer could not do),
#   * the all-1406-tensors invariant the body emits is arithmetically true, re-checked here independently,
#   * and, byte for byte, the GPU reading a view at the body's inner offset sees exactly the file's tensor
#     bytes at its absolute offset — for the HEAD tensor (view 0), the ONE tensor that STRADDLES the naive
#     view_limit boundary (view 1 — a naive two-tile split would cut it in half), and a tensor PAST the
#     buffer ceiling (view 1). snugcause: the head reading right is tested one past the ceiling and one
#     across a boundary; unispan: three tensors in two views, not one sample.
#
# WHAT THIS DOES NOT CLAIM: it does not DECODE the tensors — that is metal_mx_gpu.sh's job, which now runs
# a real MXFP4/MXFP8 decode driven through this same view set. This harness proves the RESIDENCY and the
# LOOKUP, byte-exact; the decode lanes prove the arithmetic.
#
# Who decides what (the dumb-carrier discipline):
#   the BODY  native/metal/windowed-residency.fk       — the view geometry and the lookup.
#   the BODY  native/metal/windowed-residency-emit.fk  — max_tensor_bytes, the plan, the all-tensor
#                                                        invariant stream, and the probe kernel text.
#   the CARRIER (this file + the Swift it writes) — measure the device, mmap, wrap the views, dispatch,
#                                                  memcmp against a direct mmap carve.
#
# THE GATES:
#   0  THE VIEWS MAP            all nviews bytesNoCopy buffers are non-nil and no command buffer errors
#                              (edgedrop/zerobirth — a view that failed to map reads as zeros; the exact
#                              failure being fixed, so this harness must not fall into it)
#   1  ALL 1406 FIT            the body's TVDONE says nfit==ntensors and ntoolarge==0, AND the carrier
#                              re-derives (idx, inner, holds) for every TV row and agrees on all of them
#   2  HEAD BYTE-EXACT         GPU reads view 0 at the body's inner offset; first/last sampled bytes equal
#                              a direct mmap carve at the tensor's absolute offset
#   3  STRADDLING BYTE-EXACT   the tensor crossing the naive view_limit boundary, read from view 1 at the
#                              body's inner offset, incl. bytes on BOTH sides of where a naive tiling would
#                              have cut — bit-exact vs the mmap carve
#   4  PAST-CEILING BYTE-EXACT a tensor whose absolute offset is beyond maxBufferLength, read from view 1
#
# Run:  form/native/metal/metal_windowed_residency.sh
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every other Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
BLOB="${FORM_DS4_BLOB:-$HOME/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf}"
CACHE="$ROOT/native/metal/.metallib-cache"
SAMPLE="${FORM_WR_SAMPLE:-65536}"

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

work="$(mktemp -d "${TMPDIR:-/tmp}/fkwr.XXXXXX")"
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
while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/windowed-residency-emit.fk)

# ── 1. measure the device: maxBufferLength and the page size, so the BODY's geometry is over the REAL
#       device, not a number this shell guessed. ────────────────────────────────────────────────────
cat > "$work/probe.swift" <<'SWIFT'
import Metal
import Foundation
guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
print("\(dev.maxBufferLength) \(getpagesize()) \(dev.name)")
SWIFT
swiftc -O -o "$work/probe" "$work/probe.swift" 2>"$work/probe.err" || { echo "FAIL  swiftc probe failed"; tail "$work/probe.err"; exit 1; }
PROBE="$("$work/probe")"; prc=$?
if [[ $prc -eq 2 ]]; then echo "$PROBE"; exit 2; fi
MAXBUF="$(echo "$PROBE" | awk '{print $1}')"
PAGE="$(echo "$PROBE" | awk '{print $2}')"
DEVNAME="$(echo "$PROBE" | cut -d' ' -f3-)"
echo "device: $DEVNAME  maxBufferLength=$MAXBUF  page=$PAGE"

# ── 2. the body: the plan + the all-tensor invariant stream, over the measured device ───────────────
echo "walking the file's header for the plan and every tensor row..."
printf '(wre-emit "%s" %s %s %s)\n' "$BLOB" "$FSIZE" "$MAXBUF" "$PAGE" > "$work/plan.fk"
"$GO_BIN" "${FILES[@]}" "$work/plan.fk" > "$work/plan.out" 2>"$work/plan.err" || {
    echo "FAIL  plan emission failed"; tail -5 "$work/plan.err"; exit 1; }
grep -qx 'END' "$work/plan.out" || { echo "FAIL  plan stream truncated"; exit 1; }
WR=($(awk '$1=="WR"{print; exit}' "$work/plan.out"))
[[ "${#WR[@]}" -eq 10 ]] || { echo "FAIL  WR line malformed: ${WR[*]}"; exit 1; }
# WR filelen maxbuf maxtb page view_limit overlap step mapped nviews
MAXTB=${WR[3]}; VIEWLIMIT=${WR[5]}; OVERLAP=${WR[6]}; STEP=${WR[7]}; MAPPED=${WR[8]}; NVIEWS=${WR[9]}
NT=$(awk '$1=="NT"{print $2; exit}' "$work/plan.out")
echo "  plan: max_tensor_bytes=$MAXTB view_limit=$VIEWLIMIT overlap=$OVERLAP step=$STEP mapped=$MAPPED nviews=$NVIEWS over $NT tensors"

# pick the three witness tensors from the stream: the head (first TV), the one straddling the naive
# view_limit boundary (abs < view_limit < abs+bytes), and one past the ceiling (abs >= view_limit).
HEAD_TV=($(awk '$1=="TV"{print; exit}' "$work/plan.out"))
STRAD_TV=($(awk -v vl="$VIEWLIMIT" '$1=="TV" && ($3+0)<vl && ($3+0+$4+0)>vl {print; exit}' "$work/plan.out"))
PAST_TV=($(awk -v vl="$VIEWLIMIT" '$1=="TV" && ($3+0)>=vl {print; exit}' "$work/plan.out"))
[[ "${#HEAD_TV[@]}" -eq 7 ]] || { echo "FAIL  no head TV row"; exit 1; }
if [[ "${#STRAD_TV[@]}" -ne 7 ]]; then echo "  (no tensor straddles the naive boundary on this file/device — reporting head+past only)"; STRAD_TV=("${HEAD_TV[@]}"); fi
if [[ "${#PAST_TV[@]}" -ne 7 ]]; then echo "SKIP  no tensor lies past the buffer ceiling on this device (maxBufferLength >= file); windowing not exercised — nothing to witness beyond view 0"; exit 2; fi
# TV name abs bytes idx inner holds
echo "  witness tensors:"
echo "    HEAD     ${HEAD_TV[1]}  abs=${HEAD_TV[2]} bytes=${HEAD_TV[3]} view=${HEAD_TV[4]} inner=${HEAD_TV[5]}"
echo "    STRADDLE ${STRAD_TV[1]}  abs=${STRAD_TV[2]} bytes=${STRAD_TV[3]} view=${STRAD_TV[4]} inner=${STRAD_TV[5]}"
echo "    PAST     ${PAST_TV[1]}  abs=${PAST_TV[2]} bytes=${PAST_TV[3]} view=${PAST_TV[4]} inner=${PAST_TV[5]}"

# ── 3. the probe kernel (body-authored), compiled + cached by its own sha256 ────────────────────────
echo '(wre-probe-msl)' > "$work/msl.fk"
"$GO_BIN" "${FILES[@]}" "$work/msl.fk" > "$work/msl.out" 2>"$work/msl.err" || {
    echo "FAIL  probe MSL emission failed"; cat "$work/msl.err"; exit 1; }
awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/msl.out" > "$work/wr.metal"
MSL="$work/wr.metal"
grep -q 'kernel void form_wr_probe' "$MSL" || { echo "FAIL  probe kernel not emitted"; exit 1; }
mkdir -p "$CACHE"
msl_sha="$(shasum -a 256 "$MSL" | cut -c1-16)"
LIB="$CACHE/wrprobe-$msl_sha.metallib"
if [[ -f "$LIB" ]]; then
    echo "PASS  probe metallib cache HIT: $(basename "$LIB")"
else
    xcrun -sdk macosx metal -O2 -std=metal3.0 -c "$MSL" -o "$work/wr.air" 2>"$work/metal.err" \
      && xcrun -sdk macosx metallib "$work/wr.air" -o "$LIB" 2>>"$work/metal.err" || {
        echo "FAIL  offline metal compile failed"; cat "$work/metal.err"; exit 1; }
    echo "PASS  probe metallib compiled and cached: $(basename "$LIB")"
fi

# ── 4. the carrier ─────────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
// Windowed-residency witness. Carrier only: the geometry and every (view, inner) came from the body; the
// carrier measures the device, maps the views, dispatches the body's probe, and memcmps against a direct
// mmap carve — an independent read of the same file at the tensor's ABSOLUTE offset, so a wrong inner
// reads the wrong tensor and fails.
import Metal
import Foundation

let a = CommandLine.arguments
let libPath = a[1], planPath = a[2], blobPath = a[3]
let sample = Int(a[4])!
let ntensors = Int(a[5])!, step = Int(a[6])!, viewLimit = Int(a[7])!, mapped = Int(a[8])!, nviews = Int(a[9])!
// three witness rows: name abs bytes idx inner
let hAbs = Int(a[10])!, hBytes = Int(a[11])!, hIdx = Int(a[12])!, hInner = Int(a[13])!
let sAbs = Int(a[14])!, sBytes = Int(a[15])!, sIdx = Int(a[16])!, sInner = Int(a[17])!
let pAbs = Int(a[18])!, pBytes = Int(a[19])!, pIdx = Int(a[20])!, pInner = Int(a[21])!

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
let queue = dev.makeCommandQueue()!
var failures = 0, gpuErrors = 0
var gpuFirstError: String? = nil
func check(_ ok: Bool, _ pass: String, _ fail: String) {
    if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 }
}

// mmap the whole file once.
let fd = open(blobPath, O_RDONLY)
guard fd >= 0 else { print("FAIL  cannot open blob"); exit(1) }
var st = stat(); fstat(fd, &st)
let fileLen = Int(st.st_size)
let page = Int(getpagesize())
let mapLen = (fileLen + page - 1) / page * page
guard let mapped0 = mmap(nil, mapLen, PROT_READ, MAP_PRIVATE, fd, 0), mapped0 != MAP_FAILED else {
    print("FAIL  mmap failed"); exit(1)
}
let base = mapped0.assumingMemoryBound(to: UInt8.self)   // the direct-carve pointer, over the whole file

// ── GATE 0: build the views. Each view i starts at i*step (page-aligned by construction) and spans
// view_limit, the last clipped to the mapped file. A single buffer over the whole file is what FAILED;
// this builds the overlapping set and demands every one map.
var views: [MTLBuffer] = []
var viewStarts: [Int] = []
for i in 0..<nviews {
    let vs = i * step
    let vlen = min(viewLimit, mapLen - vs)
    guard vs % page == 0 else { print("FAIL  view \(i) start \(vs) is not page-aligned"); exit(1) }
    guard let buf = dev.makeBuffer(bytesNoCopy: mapped0.advanced(by: vs), length: vlen,
                                   options: .storageModeShared, deallocator: nil) else {
        print("FAIL  view \(i) makeBuffer(bytesNoCopy:) failed at start \(vs) length \(vlen)"); failures += 1
        break
    }
    buf.label = "wr_view_\(i)"
    views.append(buf); viewStarts.append(vs)
}
check(views.count == nviews,
  "gate 0 the views map: all \(nviews) overlapping page-aligned bytesNoCopy views of the \(fileLen) B file wrap on \(dev.name) (one buffer over the whole file FAILs; \(nviews) views do not)",
  "gate 0 only \(views.count)/\(nviews) views mapped")
if failures > 0 { print("VERDICT FAIL  the views did not map"); exit(1) }
for i in 0..<nviews { print(String(format: "      view %d: [%ld, %ld)  %ld B", i, viewStarts[i], viewStarts[i]+views[i].length, views[i].length)) }

// ── GATE 1: the all-1406-tensors invariant. The body streamed (idx, inner, holds) for every tensor and a
// TVDONE fold; re-derive it here independently and demand full agreement, then that nfit==ntensors.
var nfit = 0, ntl = 0, nAgree = 0, nRows = 0
var bodyNfit = -1, bodyNtl = -1
for line in try String(contentsOfFile: planPath, encoding: .utf8).split(separator: "\n") {
    let f = line.split(separator: " ").map(String.init)
    if f.first == "TV", f.count == 7 {
        let abs = Int(f[2])!, bytes = Int(f[3])!, bIdx = Int(f[4])!, bInner = Int(f[5])!, bHolds = Int(f[6])!
        let idx = abs / step
        let inner = abs - idx * step
        let holds = (inner >= 0 && inner + bytes <= viewLimit && idx < nviews) ? 1 : 0
        nRows += 1
        if idx == bIdx && inner == bInner && holds == bHolds { nAgree += 1 }
        if holds == 1 { nfit += 1 }
        if bytes > viewLimit { ntl += 1 }
    } else if f.first == "TVDONE", f.count == 3 { bodyNfit = Int(f[1])!; bodyNtl = Int(f[2])! }
}
check(nRows == ntensors && nAgree == ntensors && nfit == ntensors && ntl == 0 && bodyNfit == ntensors && bodyNtl == 0,
  "gate 1 all \(ntensors) tensors fit: the body and the carrier independently agree on (view, inner, holds) for every one, all hold, none exceeds a view",
  "gate 1 invariant: rows \(nRows) agree \(nAgree) fit \(nfit) toolarge \(ntl) (body nfit \(bodyNfit) ntl \(bodyNtl)) of \(ntensors)")

// the probe: GPU reads `cnt` bytes of view `idx` starting at `inner+off`, writes them to dst; compare to
// the direct mmap carve at abs+off. A wrong inner reads a wrong tensor and the memcmp fails.
let p = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_wr_probe")!)
func probeEq(_ idx: Int, _ inner: Int, _ abs: Int, _ off: Int, _ cnt: Int) -> Bool {
    let dst = dev.makeBuffer(length: cnt, options: .storageModeShared)!
    let dp = dst.contents().bindMemory(to: UInt8.self, capacity: cnt)
    // sentinel the output so an unrun kernel cannot pass by echoing zeros that happen to match
    for i in 0..<cnt { dp[i] = 0xA5 }
    var b64 = UInt64(inner + off), c32 = UInt32(cnt)
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(p)
    enc.setBuffer(views[idx], offset: 0, index: 0)
    enc.setBuffer(dst, offset: 0, index: 1)
    enc.setBytes(&b64, length: 8, index: 2)
    enc.setBytes(&c32, length: 4, index: 3)
    let tg = min(p.maxTotalThreadsPerThreadgroup, 256)
    enc.dispatchThreads(MTLSize(width: cnt, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let e = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(e)" }; return false }
    if cb.status != .completed { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "cb status \(cb.status.rawValue)" }; return false }
    return memcmp(dp, base.advanced(by: abs + off), cnt) == 0
}
// sample the head, the last `sample` bytes, and (for the straddler) a window centered on where a naive
// tiling would have cut, i.e. at (view_limit - abs) inside the tensor.
func witness(_ label: String, _ idx: Int, _ inner: Int, _ abs: Int, _ bytes: Int, gate: Int, boundaryCut: Int) {
    let n = min(sample, bytes)
    var ok = probeEq(idx, inner, abs, 0, n)                       // head bytes
    ok = ok && probeEq(idx, inner, abs, bytes - n, n)            // tail bytes
    var extra = ""
    if boundaryCut > 0 && boundaryCut < bytes {                   // bytes on BOTH sides of the naive cut
        let start = max(0, boundaryCut - n/2)
        let cnt = min(n, bytes - start)
        ok = ok && probeEq(idx, inner, abs, start, cnt)
        extra = String(format: ", and %d bytes spanning the naive cut at tensor-offset %d", cnt, boundaryCut)
    }
    check(ok,
      "gate \(gate) \(label) byte-exact: the GPU reads view \(idx) at the body's inner offset \(inner) and its first and last \(n) bytes\(extra) equal a direct mmap carve at absolute \(abs)",
      "gate \(gate) \(label): GPU read through view \(idx) at inner \(inner) differs from the mmap carve at absolute \(abs)")
}
witness("HEAD",     hIdx, hInner, hAbs, hBytes, gate: 2, boundaryCut: 0)
witness("STRADDLE", sIdx, sInner, sAbs, sBytes, gate: 3, boundaryCut: viewLimit - sAbs)
witness("PAST",     pIdx, pInner, pAbs, pBytes, gate: 4, boundaryCut: 0)

if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER(S) FAILED — first: \(gpuFirstError ?? "unknown") ===") }
let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  5 gates, the 85 GiB model resident as \(nviews) overlapping views") }
else { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc failed"; tail -20 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB" "$work/plan.out" "$BLOB" "$SAMPLE" \
    "$NT" "$STEP" "$VIEWLIMIT" "$MAPPED" "$NVIEWS" \
    "${HEAD_TV[2]}" "${HEAD_TV[3]}" "${HEAD_TV[4]}" "${HEAD_TV[5]}" \
    "${STRAD_TV[2]}" "${STRAD_TV[3]}" "${STRAD_TV[4]}" "${STRAD_TV[5]}" \
    "${PAST_TV[2]}" "${PAST_TV[3]}" "${PAST_TV[4]}" "${PAST_TV[5]}"
rc=$?
exit $rc
