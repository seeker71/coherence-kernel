#!/usr/bin/env bash
# metal_dsv4_token.sh — Stone 33: the DeepSeek-V4-Flash first token, wired at REAL DIMS over the
# windowed-resident 85 GiB file, one proven stage at a time, with the offered-interface guard.
#
# THE RADIUS, and nothing wider (aporon). No external oracle can execute this exact file — ds4,
# llama.cpp, ollama and LM Studio all REFUSE GGUF types 40/41 (`unsupported GGUF type 40`, then
# `tensor points outside GGUF file`). So a whole-forward output is UNFALSIFIABLE against any reference
# on this machine (selfgauge — there is no external denominator here, and this harness says so). What
# each stage stands on is named at its gate.
#
# THE EVIDENCE CLASS (knownsolved — the components are seen to work without an oracle for the whole):
#   * EMBED (this stage) is BIT-EXACT: the F16 embedding row decoded on the GPU through the resident
#     view equals an independent mmap carve decoded by the carrier's own f16→f32, index for index. A
#     pure table read with no accumulation, so bit-exactness is a true and checkable claim.
#   * The full token, when the layer stack lands, will be "a composition of individually-proven
#     components, wired in forward_first_token_cpu's order, producing a non-degenerate, stable,
#     input-dependent token" — NOT bit-exact end to end. That triple is the falsifiable core.
#
# THE OFFERED-INTERFACE GUARD (edgedrop/zerobirth, axiom-4/5). An unrun kernel reads as a computed
# zero; a dead view reads as zeros. So: the output buffer is SENTINELLED (0x7F = a huge f32) before
# every dispatch, cb.error and cb.status are checked after every dispatch, and the embed gate REQUIRES
# a NON-DEGENERATE result (not all-equal, real variance) — an all-zero or constant embedding is a dead
# read, not an embedding.
#
# Who decides what (the dumb-carrier discipline):
#   the BODY  native/metal/windowed-residency.fk / -emit.fk  — the view geometry, the lookup, the plan.
#   the BODY  native/metal/dsv4-token.fk                     — every character of Metal, the row offset.
#   the CARRIER (this file + the Swift it writes) — measure the device, mmap, wrap the views, dispatch,
#                                                  memcmp/decode-compare against a direct mmap carve.
#
# Run:  form/native/metal/metal_dsv4_token.sh   (optional: FORM_DS4_PROMPT_TOKEN=<id>)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
BLOB="${FORM_DS4_BLOB:-$HOME/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf}"
CACHE="$ROOT/native/metal/.metallib-cache"
# forward_first_token_cpu(prompt->v[0]); "The capital of France is" -> 671 6102 294 8760 344, so v[0]=671.
TOKEN="${FORM_DS4_PROMPT_TOKEN:-671}"
SAMPLE="${FORM_DS4_SAMPLE:-4096}"   # n_embd; embed decodes the whole row and checks all of it

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
echo "ds4 blob: $FSIZE bytes at $(date '+%H:%M:%S')   forward_first_token_cpu(token=$TOKEN)"

work="$(mktemp -d "${TMPDIR:-/tmp}/fkdsv4.XXXXXX")"
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
while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/dsv4-token.fk)

# ── 1. measure the device: maxBufferLength and page ──────────────────────────────────────────────
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

# ── 2. the body's residency plan + every tensor's (view, inner, holds) over the LIVE file ─────────
echo "walking the file header for the residency plan and every tensor row..."
printf '(wre-emit "%s" %s %s %s)\n' "$BLOB" "$FSIZE" "$MAXBUF" "$PAGE" > "$work/plan.fk"
"$GO_BIN" "${FILES[@]}" "$work/plan.fk" > "$work/plan.out" 2>"$work/plan.err" || {
    echo "FAIL  plan emission failed"; tail -5 "$work/plan.err"; exit 1; }
grep -qx 'END' "$work/plan.out" || { echo "FAIL  plan stream truncated"; exit 1; }
WR=($(awk '$1=="WR"{print; exit}' "$work/plan.out"))
[[ "${#WR[@]}" -eq 10 ]] || { echo "FAIL  WR line malformed: ${WR[*]}"; exit 1; }
# WR filelen maxbuf maxtb page view_limit overlap step mapped nviews
MAXTB=${WR[3]}; VIEWLIMIT=${WR[5]}; STEP=${WR[7]}; NVIEWS=${WR[9]}
NT=$(awk '$1=="NT"{print $2; exit}' "$work/plan.out")
echo "  plan: max_tensor_bytes=$MAXTB view_limit=$VIEWLIMIT step=$STEP nviews=$NVIEWS over $NT tensors"

# token_embd.weight — F16 [n_embd, vocab]; its (view, inner, holds) from the body's plan.
EMB_TV=($(awk '$1=="TV" && $2=="token_embd.weight"{print; exit}' "$work/plan.out"))
[[ "${#EMB_TV[@]}" -eq 7 ]] || { echo "FAIL  token_embd.weight not in the plan stream"; exit 1; }
# TV name abs bytes idx inner holds
EMB_ABS=${EMB_TV[2]}; EMB_BYTES=${EMB_TV[3]}; EMB_IDX=${EMB_TV[4]}; EMB_INNER=${EMB_TV[5]}; EMB_HOLDS=${EMB_TV[6]}
N_EMBD=4096
ROW_OFF=$(( TOKEN * N_EMBD * 2 ))          # byte offset of token's row inside the F16 tensor
echo "  token_embd: abs=$EMB_ABS bytes=$EMB_BYTES view=$EMB_IDX inner=$EMB_INNER holds=$EMB_HOLDS  row_off(t=$TOKEN)=$ROW_OFF"

# ── 3. the body's embed MSL, compiled + cached by its own sha256 ──────────────────────────────────
echo '(dsv4-embed-msl)' > "$work/msl.fk"
"$GO_BIN" "${FILES[@]}" "$work/msl.fk" > "$work/msl.out" 2>"$work/msl.err" || {
    echo "FAIL  embed MSL emission failed"; cat "$work/msl.err"; exit 1; }
awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/msl.out" > "$work/dsv4.metal"
MSL="$work/dsv4.metal"
grep -q 'kernel void form_dsv4_embed_f16' "$MSL" || { echo "FAIL  embed kernel not emitted"; exit 1; }
mkdir -p "$CACHE"
msl_sha="$(shasum -a 256 "$MSL" | cut -c1-16)"
LIB="$CACHE/dsv4tok-$msl_sha.metallib"
if [[ -f "$LIB" ]]; then
    echo "PASS  embed metallib cache HIT: $(basename "$LIB")"
else
    xcrun -sdk macosx metal -O2 -std=metal3.0 -c "$MSL" -o "$work/dsv4.air" 2>"$work/metal.err" \
      && xcrun -sdk macosx metallib "$work/dsv4.air" -o "$LIB" 2>>"$work/metal.err" || {
        echo "FAIL  offline metal compile failed"; cat "$work/metal.err"; exit 1; }
    echo "PASS  embed metallib compiled and cached: $(basename "$LIB")"
fi

# ── 4. the carrier ────────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
// Stone 33 carrier, Stage 1 (EMBED). The geometry and every (view, inner) came from the body; the
// carrier measures the device, maps the overlapping views (the thing one buffer cannot do), dispatches
// the body's F16 embed kernel through the resident view, and decode-compares to a direct mmap carve —
// an independent read of the same file at the tensor's ABSOLUTE offset, decoded by the carrier's own
// f16→f32. A wrong inner reads the wrong row and the compare fails.
import Metal
import Foundation

let a = CommandLine.arguments
let libPath = a[1], blobPath = a[2]
let step = Int(a[3])!, viewLimit = Int(a[4])!, nviews = Int(a[5])!
let embAbs = Int(a[6])!, rowOff = Int(a[7])!, nEmbd = Int(a[8])!
let embIdx = Int(a[9])!, embInner = Int(a[10])!, embHolds = Int(a[11])!
let token = Int(a[12])!

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
let queue = dev.makeCommandQueue()!
var failures = 0, gpuErrors = 0
var gpuFirstError: String? = nil
func check(_ ok: Bool, _ pass: String, _ fail: String) {
    if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 }
}
// IEEE-754 half -> single, the carrier's own independent decode (matches Metal's float(half)).
func f16_to_f32(_ h: UInt16) -> Float {
    let sign = UInt32(h & 0x8000) << 16
    let exp  = Int((h >> 10) & 0x1F)
    let mant = UInt32(h & 0x3FF)
    if exp == 0 {
        if mant == 0 { return Float(bitPattern: sign) }
        var m = mant, e = -1
        repeat { m <<= 1; e += 1 } while (m & 0x400) == 0
        m &= 0x3FF
        let bits = sign | UInt32(127 - 15 - e) << 23 | (m << 13)
        return Float(bitPattern: bits)
    } else if exp == 0x1F {
        let bits = sign | 0x7F800000 | (mant << 13)
        return Float(bitPattern: bits)
    }
    let bits = sign | UInt32(exp - 15 + 127) << 23 | (mant << 13)
    return Float(bitPattern: bits)
}

// mmap the whole file once — the direct-carve reference.
let fd = open(blobPath, O_RDONLY)
guard fd >= 0 else { print("FAIL  cannot open blob"); exit(1) }
var st = stat(); fstat(fd, &st)
let fileLen = Int(st.st_size)
let page = Int(getpagesize())
let mapLen = (fileLen + page - 1) / page * page
guard let mapped0 = mmap(nil, mapLen, PROT_READ, MAP_PRIVATE, fd, 0), mapped0 != MAP_FAILED else {
    print("FAIL  mmap failed"); exit(1)
}
let base = mapped0.assumingMemoryBound(to: UInt8.self)

// ── GATE 0: the views map. One buffer over the whole file FAILs (onelean); build the overlapping set.
var views: [MTLBuffer] = []
for i in 0..<nviews {
    let vs = i * step
    let vlen = min(viewLimit, mapLen - vs)
    guard vs % page == 0 else { print("FAIL  view \(i) start \(vs) not page-aligned"); exit(1) }
    guard let buf = dev.makeBuffer(bytesNoCopy: mapped0.advanced(by: vs), length: vlen,
                                   options: .storageModeShared, deallocator: nil) else {
        print("FAIL  view \(i) makeBuffer(bytesNoCopy:) failed at \(vs) len \(vlen)"); failures += 1; break
    }
    buf.label = "dsv4_view_\(i)"
    views.append(buf)
}
check(views.count == nviews,
  "gate 0 the views map: all \(nviews) overlapping page-aligned bytesNoCopy views of the \(fileLen) B file wrap on \(dev.name) (one buffer over the whole file FAILs; \(nviews) views do not)",
  "gate 0 only \(views.count)/\(nviews) views mapped")
if failures > 0 { print("VERDICT FAIL  the views did not map"); exit(1) }

// ── GATE 1: the token's embedding row is resident in a real view (holds==1).
check(embHolds == 1 && embIdx < nviews,
  "gate 1 the token_embd tensor lies wholly inside view \(embIdx) at inner \(embInner) — the embedding row is resident",
  "gate 1 token_embd does not fit a single view (holds=\(embHolds) idx=\(embIdx))")
if failures > 0 { print("VERDICT FAIL"); exit(1) }

// ── GATE 2: EMBED byte/decode-exact + non-degenerate.
let p = try dev.makeComputePipelineState(function: lib.makeFunction(name: "form_dsv4_embed_f16")!)
let n = nEmbd
let dst = dev.makeBuffer(length: n * MemoryLayout<Float>.stride, options: .storageModeShared)!
let dp = dst.contents().bindMemory(to: Float.self, capacity: n)
// SENTINEL: an unrun kernel / dead view must not pass by echoing zeros. Fill with a huge value so a
// non-dispatch leaves an out-of-range vector the non-degeneracy + carve checks both reject.
for i in 0..<n { dp[i] = Float(bitPattern: 0x7F7FFFFF) }   // ~3.4e38
// base = the row's byte offset inside the view = inner(tensor) + rowOff(token within tensor).
var b64 = UInt64(embInner + rowOff), c32 = UInt32(n)
let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
enc.setComputePipelineState(p)
enc.setBuffer(views[embIdx], offset: 0, index: 0)
enc.setBuffer(dst, offset: 0, index: 1)
enc.setBytes(&b64, length: 8, index: 2)
enc.setBytes(&c32, length: 4, index: 3)
let tg = min(p.maxTotalThreadsPerThreadgroup, 256)
enc.dispatchThreads(MTLSize(width: n, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
if let e = cb.error { gpuErrors += 1; gpuFirstError = "\(e)" }
if cb.status != .completed { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "cb status \(cb.status.rawValue)" } }

// the independent carve: decode the same F16 row straight from the mmap at the ABSOLUTE offset.
let rowAbs = embAbs + rowOff
var carve = [Float](repeating: 0, count: n)
for i in 0..<n {
    let lo = UInt16(base[rowAbs + i*2]); let hi = UInt16(base[rowAbs + i*2 + 1])
    carve[i] = f16_to_f32(lo | (hi << 8))
}
var mism = 0, firstMis = -1
for i in 0..<n where dp[i].bitPattern != carve[i].bitPattern { mism += 1; if firstMis < 0 { firstMis = i } }
// non-degeneracy: a real embedding is not constant. Measure min/max/#distinct over the GPU output.
var vmin = Float.greatestFiniteMagnitude, vmax = -Float.greatestFiniteMagnitude, nz = 0
var seen = Set<UInt32>()
for i in 0..<n { let v = dp[i]; vmin = min(vmin, v); vmax = max(vmax, v); if v != 0 { nz += 1 }; seen.insert(v.bitPattern) }
let nonDegen = (vmax > vmin) && (seen.count > 8) && (vmax < 1e30) && (vmin > -1e30)

check(mism == 0 && gpuErrors == 0,
  "gate 2 EMBED bit-exact: the GPU decoded token \(token)'s \(n)-wide F16 embedding row through view \(embIdx), and every index equals the carrier's independent mmap f16→f32 carve at absolute \(rowAbs)",
  "gate 2 EMBED: \(mism) of \(n) indices differ from the mmap carve (first at \(firstMis)); gpuErrors=\(gpuErrors)")
check(nonDegen,
  "gate 3 EMBED non-degenerate: the decoded embedding is a real vector — min \(vmin) max \(vmax), \(seen.count) distinct values, \(nz) nonzero (a dead view / unrun kernel would leave the 0x7F7FFFFF sentinel or all-zero)",
  "gate 3 EMBED degenerate: min \(vmin) max \(vmax) distinct \(seen.count) nz \(nz) — looks like a dead read")

if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER ERROR(S) — first: \(gpuFirstError ?? "unknown") ===") }
// report a few decoded values so the token's embedding is a visible fact, not a claim.
print(String(format: "      embed[0..4] = %.6f %.6f %.6f %.6f", dp[0], dp[1], dp[2], dp[3]))
print(String(format: "      L2 norm of the embedding = %.6f", sqrt((0..<n).reduce(Float(0)){ $0 + dp[$1]*dp[$1] })))

let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  4 gates — Stage 1 EMBED at real dims through the windowed views, bit-exact") }
else { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc failed"; tail -20 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB" "$BLOB" \
    "$STEP" "$VIEWLIMIT" "$NVIEWS" \
    "$EMB_ABS" "$ROW_OFF" "$N_EMBD" "$EMB_IDX" "$EMB_INNER" "$EMB_HOLDS" "$TOKEN"
rc=$?
exit $rc
