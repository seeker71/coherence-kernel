#!/usr/bin/env bash
# metal_mx_gpu.sh — GGUF types 40 and 41 on the GPU, made a witness.
#
# THE CLAIM, and nothing wider: the plane-split QUANTIZED bytes of
# DeepSeek-V4-Flash-REAP25-DSpark-ds4-GGUF live on the GPU unchanged, and a Metal kernel the BODY
# emitted (mxfp4-msl.fk / mxfp8-msl.fk) dequantizes them THERE, bit-exactly equal to the CPU carver
# (mxfp4-plane-dequant.fk / mxfp8-plane-dequant.fk), at BOTH ENDS of a real 8 388 608-element slice and
# again in a DISTANT slice of the same tensor. ds4 itself cannot read one byte of these tensors: it
# refuses both types ("unsupported GGUF type 40") before it reads their geometry.
#
# WHAT THIS DOES NOT CLAIM, stated before the gates and not after.
#   * The GEOMETRY remains Stone 15's grade: STRUCTURAL and statistically corroborated, NOT bit-exact
#     against an independent implementation, because no independent implementation of either type
#     exists anywhere — not ds4, not llama.cpp/ggml, not MLX. The GPU agreeing with the CPU proves the
#     TRANSCRIPTION and the residency. Both sides read the same hypothesis about the layout.
#   * The type-40 NIBBLE ORDER remains UNPROVEN. Nothing here tests it and nothing here pretends to.
#   * MXFP8's witness runs on the committed 1056-byte FIXTURE, not on a tensor of the live file, and
#     that is a fact about this machine at this hour: type-41 data begins at absolute 72 766 954 336 and
#     the download has reached ~28 GB. The fixture is real bytes of blk.0.attn_kv.weight fetched by
#     byte range in Stone 15, so the arithmetic is witnessed on real data — but the RADIUS is 1024
#     elements, not both ends of a tensor. Gate 7 prints that in its own line rather than letting the
#     word "PASS" imply the wider thing.
#
# Who decides what (the dumb-carrier discipline):
#   the BODY  native/metal/mx-residency.fk   — where the tensors are, what the weights ARE at any flat
#                                              offset, what a row's dot should be (fp64), and the
#                                              summation bound's coefficient.
#   the BODY  form-stdlib/mxfp4-msl.fk / mxfp8-msl.fk — the Metal source. Not one character here.
#   the CARRIER (this file + the Swift runner it writes) — mmap, bind, dispatch, compare.
#
# THE GATES:
#   1  DEQUANT IS THE BODY'S, AT THE HEAD      GPU weights 0..4095 of slice 0 equal Form's, bit for bit
#   2  ... AND AT THE TAIL                     the LAST 4096 elements of the same slice, likewise. The
#                                              tail tile's payload bytes end where the SCALE PLANE
#                                              begins, so an off-by-one in the plane base shows here and
#                                              nowhere else (corpus row 826, aporon)
#   3  ... AND IN A DISTANT SLICE              slice 255 (expert 255, 1 136 394 240 bytes on) — a layout
#                                              that fits slice 0 beautifully is tested far away
#                                              (corpus row snugcause), and the SLICE STRIDE is what is
#                                              actually under test here
#   4  THE WHOLE SLICE DEQUANTS                all 8 388 608 elements in ONE dispatch; head and tail read
#                                              back OUT of that result still equal Form's
#   5  THE FUSED KERNEL IS THE SAME MEANING    the slot matvec agrees with an f32 fold over the
#                                              dequantized buffer within the body's derived bound
#   6  IT IS THE BODY'S ANSWER                 sampled rows match Form's fp64 dot at the real width
#                                              within (cols + chunk + 32) * u * SUM|term|
#   7  MXFP8 DECODES ON THE GPU                head and tail of the committed real-byte fixture, bit for
#                                              bit, plus its fused matvec within the same bound
#   8  RESIDENCY IS REAL                       ITERS dispatches, zero re-uploads, checksum stable
#   9  ONE HEADER, ONE SPINE, CACHED LIBRARY   exactly one metal_stdlib and one mxm_pow2 in the unit;
#                                              the .metallib is keyed by the source's own sha256
#
# Run:  form/native/metal/metal_mx_gpu.sh [iters]        (default 200)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every other Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
ITERS="${1:-200}"
BLOB="${FORM_DS4_BLOB:-$HOME/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf}"
FIX8="$ROOT/form-stdlib/tests/fixtures/mxfp8-plane-1024.bin"
W40="${FORM_MX_TENSOR:-blk.0.ffn_gate_exps.weight}"
TILE=4096
MVROWS=64
CACHE="$ROOT/native/metal/.metallib-cache"
TABLE_CACHE="$ROOT/native/metal/.mx-table-cache"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the GPU witness needs an Apple GPU + swiftc"
    exit 2
fi
if [[ ! -f "$BLOB" ]]; then
    echo "SKIP  the ds4 GGUF is not on this host: $BLOB   (set FORM_DS4_BLOB)"
    exit 2
fi
if [[ ! -f "$FIX8" ]]; then echo "FAIL  the committed MXFP8 fixture is missing: $FIX8"; exit 1; fi
if [[ ! -x "$GO_BIN" ]]; then
    echo "  building go kernel..." >&2
    (cd "$ROOT/form-kernel-go" && go build -o bin-go .)
fi

# THE FILE IS INCOMPLETE AND GROWING — a live curl is resuming it. Every offset this harness trusts is
# re-checked against the CURRENT size, right here, and the run refuses rather than reading a hole.
FSIZE=$(stat -f%z "$BLOB")
echo "ds4 blob: $FSIZE bytes at $(date '+%H:%M:%S') — the download is live; every offset below is checked against this"

work="$(mktemp -d "${TMPDIR:-/tmp}/fkmx.XXXXXX")"
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
while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/mx-residency.fk)
# the windowed-residency mouth: onelean (row 850) — the 85 GiB file exceeds maxBufferLength, so one buffer
# cannot span it. The body prices the largest tensor; the runner builds overlapping views from it.
WRFILES=()
while read -r x; do WRFILES+=("$x"); done < <(FK_SEEN=""; fk_expand native/metal/windowed-residency-emit.fk)

# ── 1. the body emits the Metal source (four kernels, one header, one spine) ───────────────────
echo '(mxr-emit-msl)' > "$work/msl.fk"
"$GO_BIN" "${FILES[@]}" "$work/msl.fk" > "$work/msl.out" 2>"$work/msl.err" || {
    echo "FAIL  MSL emission failed"; cat "$work/msl.err"; exit 1; }
awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/msl.out" > "$work/mx.metal"
MSL="$work/mx.metal"
for k in form_mxfp4_dequant_f32 form_mxfp4_matvec_slot_f32 form_mxfp8_dequant_f32 form_mxfp8_matvec_slot_f32; do
    grep -q "kernel void $k" "$MSL" || { echo "FAIL  kernel $k was not emitted"; exit 1; }
done
# gate 9a: ONE header, at the top, and no `using namespace metal;` — this body's `round` goes ambiguous
# against metal_stdlib's otherwise, and the unit stops compiling. The one library call is qualified.
head -c 200 "$MSL" | grep -q '#include <metal_stdlib>' || { echo "FAIL  gate 9: metal_stdlib is not at the top of the unit"; exit 1; }
nhdr=$(grep -c 'metal_stdlib' "$MSL"); nusing=$(grep -c 'using namespace' "$MSL")
nspine=$(grep -o 'float mxm_pow2' "$MSL" | wc -l | tr -d ' ')
[[ "$nhdr" == 1 && "$nusing" == 0 && "$nspine" == 1 ]] || {
    echo "FAIL  gate 9: header count $nhdr (want 1), using-namespace $nusing (want 0), mxm_pow2 definitions $nspine (want 1)"; exit 1; }
echo "PASS  gate 9a one metal_stdlib, no using-namespace, ONE mxm_ spine: $(wc -c < "$MSL" | tr -d ' ') bytes, 4 kernels, every byte authored by mxfp4-msl.fk / mxfp8-msl.fk"

# ── 2. gate 9b: the .metallib, cached across RUNS by the source's own sha256 ───────────────────
mkdir -p "$CACHE"
msl_sha="$(shasum -a 256 "$MSL" | cut -c1-16)"
LIB="$CACHE/mx-$msl_sha.metallib"
if [[ -f "$LIB" ]]; then
    echo "PASS  gate 9b metallib cache HIT: $(basename "$LIB") (emitted source unchanged; no compile this run)"
else
    xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math \
          -c "$MSL" -o "$work/mx.air" 2>"$work/metal.err" \
      && xcrun -sdk macosx metallib "$work/mx.air" -o "$LIB" 2>>"$work/metal.err" || {
        echo "FAIL  offline metal compile failed"; cat "$work/metal.err"; exit 1; }
    echo "PASS  gate 9b metallib cache MISS -> compiled and cached: $(basename "$LIB")"
fi

# ── 3. the body: the tensor table, ONE header walk over 1406 tensor-info rows ──────────────────
# The walk costs ~22 s and the header cannot change under us (it is the completed prefix of the file),
# so it is cached by the blob's own size-independent header bytes. The GEOMETRY still comes from the
# body every run; only the walk is reused.
mkdir -p "$TABLE_CACHE"
hdr_sha="$(head -c 5339744 "$BLOB" | shasum -a 256 | cut -c1-16)"
TBL="$TABLE_CACHE/tbl-$hdr_sha.txt"
if [[ ! -f "$TBL" ]]; then
    echo "walking the file's own header ONCE for all 1406 tensor-info rows (~22 s)..."
    printf '(mxr-emit-table "%s")\n' "$BLOB" > "$work/table.fk"
    "$GO_BIN" "${FILES[@]}" "$work/table.fk" > "$TBL.tmp" 2>"$work/table.err" || {
        echo "FAIL  table emission failed"; tail -5 "$work/table.err"; rm -f "$TBL.tmp"; exit 1; }
    grep -qx 'END' "$TBL.tmp" || { echo "FAIL  table stream truncated"; rm -f "$TBL.tmp"; exit 1; }
    mv "$TBL.tmp" "$TBL"
else
    echo "tensor table: cached for this header ($(basename "$TBL"))"
fi
COUNTS=($(awk '$1=="COUNTS"{print $2, $3}' "$TBL"))
echo "  ${COUNTS[0]} type-40 (MXFP4) tensors, ${COUNTS[1]} type-41 (MXFP8) tensors, out of $(awk '$1=="NTENSORS"{print $2}' "$TBL")"

# ── 3b. onelean (row 850): the largest tensor over ALL types, so the runner can build overlapping views.
# The whole 85 GiB file exceeds maxBufferLength — one MTLBuffer cannot span it (this is what regressed).
# Cached by the same header sha; the geometry is windowed-residency.fk's, proven by its band and agreed
# byte-for-byte over all 1406 tensors by metal_windowed_residency.sh.
MTBL="$TABLE_CACHE/maxtb-$hdr_sha.txt"
if [[ ! -f "$MTBL" ]]; then
    echo "pricing the largest tensor over all types (once, for the view set)..."
    printf '(wre-maxtb-only "%s")\n' "$BLOB" > "$work/maxtb.fk"
    "$GO_BIN" "${WRFILES[@]}" "$work/maxtb.fk" > "$MTBL.tmp" 2>"$work/maxtb.err" || {
        echo "FAIL  max-tensor pricing failed"; tail -5 "$work/maxtb.err"; rm -f "$MTBL.tmp"; exit 1; }
    grep -q '^MAXTB ' "$MTBL.tmp" || { echo "FAIL  MAXTB not emitted"; rm -f "$MTBL.tmp"; exit 1; }
    mv "$MTBL.tmp" "$MTBL"
fi
MAXTB=$(awk '$1=="MAXTB"{print $2; exit}' "$MTBL")
echo "  max tensor over all types: $MAXTB B — the view overlap must exceed it"

ROW=($(awk -v n="$W40" '$1=="T" && $2==n {print; exit}' "$TBL"))
[[ "${#ROW[@]}" -eq 11 ]] || { echo "FAIL  $W40 is not in the table"; exit 1; }
# T name type ndims d0 d1 d2 abs nel slices bytes
[[ "${ROW[2]}" == 40 ]] || { echo "FAIL  $W40 is ggml type ${ROW[2]}, not MXFP4(40)"; exit 1; }
ABS=${ROW[7]}; NEL=${ROW[8]}; SLICES=${ROW[9]}; TBYTES=${ROW[10]}; COLS=${ROW[4]}
SLICEB=$(( TBYTES / SLICES ))
FAR=$(( SLICES - 1 ))
ABSFAR=$(( ABS + FAR * SLICEB ))
TAIL=$(( NEL - TILE ))
echo "  $W40: type 40, ${ROW[4]}x${ROW[5]}x${ROW[6]}, $SLICES slices of $NEL elements ($SLICEB B each), $TBYTES B at $ABS"

# THE OFFSET CHECK — the file is still growing, so nothing below is assumed to exist.
END4=$(( ABS + TBYTES ))
if (( END4 > FSIZE )); then
    echo "SKIP  $W40 ends at $END4 but the download has only reached $FSIZE — re-run when it passes that"
    exit 2
fi
echo "  offsets checked against the live file: the tensor ends at $END4, $(( FSIZE - END4 )) bytes inside the downloaded prefix"

# ── 4. the body: reference tiles at both ends of slice 0, a distant slice, x, fp64 rows, the bound ──
echo "dequantizing reference tiles (head, tail, distant slice), x, and $((MVROWS>4?4:MVROWS)) fp64 rows per lane..."
{
  printf '(do (mxr-line "LANE MX4")\n'
  printf '    (mxr-emit-tile 40 "%s" %s %s 0 %s)\n'  "$BLOB" "$ABS" "$NEL" "$TILE"
  printf '    (mxr-emit-tile 40 "%s" %s %s %s %s)\n' "$BLOB" "$ABS" "$NEL" "$TAIL" "$TILE"
  printf '    (mxr-emit-x 40 "%s" %s %s 0 %s)\n'     "$BLOB" "$(( ABS + SLICEB ))" "$NEL" "$COLS"
  printf '    (mxr-emit-rows 40 "%s" %s %s 0 4 %s %s %s 0)\n' "$BLOB" "$ABS" "$NEL" "$COLS" "$(( ABS + SLICEB ))" "$NEL"
  printf '    (mxr-emit-bound %s)\n' "$COLS"
  printf '    (mxr-line "LANE MX4FAR")\n'
  printf '    (mxr-emit-tile 40 "%s" %s %s 0 %s)\n'  "$BLOB" "$ABSFAR" "$NEL" "$TILE"
  printf '    (mxr-line "LANE MX8")\n'
  printf '    (mxr-emit-tile 41 "%s" 0 1024 0 512)\n'   "$FIX8"
  printf '    (mxr-emit-tile 41 "%s" 0 1024 512 512)\n' "$FIX8"
  printf '    (mxr-emit-x 41 "%s" 0 1024 0 512)\n'      "$FIX8"
  printf '    (mxr-emit-rows 41 "%s" 0 1024 0 2 512 0 1024 0)\n' "$FIX8"
  printf '    (mxr-emit-bound 512)\n'
  printf '    (mxr-line "END"))\n'
} > "$work/ref.fk"
rf0=$(date +%s)
"$GO_BIN" "${FILES[@]}" "$work/ref.fk" > "$work/ref.txt" 2>"$work/ref.err" || {
    echo "FAIL  reference emission failed"; tail -5 "$work/ref.err"; exit 1; }
rf1=$(date +%s)
grep -qx 'END' "$work/ref.txt" || { echo "FAIL  reference stream truncated"; exit 1; }
echo "  body reference time: $((rf1-rf0)) s"

# ── 5. the carrier ────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
// MX-on-the-GPU witness. Carrier only: every number it JUDGES came from the body, and every number it
// computes itself is a plain f32 fold in a stated order.
import Metal
import Foundation

let a = CommandLine.arguments
let libPath = a[1], refPath = a[2], blobPath = a[3], fixPath = a[4]
let iters = Int(a[5])!, tile = Int(a[6])!
let abs4 = Int(a[7])!, nel4 = Int(a[8])!, cols4 = Int(a[9])!, tail4 = Int(a[10])!
let absFar = Int(a[11])!, mvRows = Int(a[12])!, farSlice = Int(a[13])!
let maxTensorBytes = Int(a[14])!    // the body's price of the largest tensor, for the view set

struct Ref { var tiles: [Int: [Double]] = [:]; var x: [Double] = []; var rows: [Int: Double] = [:]; var coeff = 0 }
var refs: [String: Ref] = [:]
do {
    var lane = "", section = "", tileOff = -1, pendingRow = -1
    for line in try String(contentsOfFile: refPath, encoding: .utf8).split(separator: "\n", omittingEmptySubsequences: false) {
        let s = String(line)
        if s.hasPrefix("LANE ") { lane = String(s.dropFirst(5)); refs[lane] = Ref(); section = ""; continue }
        if s.hasPrefix("TILE ") { tileOff = Int(s.split(separator: " ")[1])!; refs[lane]!.tiles[tileOff] = []; section = "T"; continue }
        if s.hasPrefix("X ")    { section = "X"; continue }
        if s.hasPrefix("ROW ")  { pendingRow = Int(s.split(separator: " ")[1])!; section = "R"; continue }
        if s.hasPrefix("BOUND "){ refs[lane]!.coeff = Int(s.split(separator: " ")[2])!; section = ""; continue }
        if s == "END" { continue }
        guard let v = Double(s) else { continue }
        switch section {
        case "T": refs[lane]!.tiles[tileOff]!.append(v)
        case "X": refs[lane]!.x.append(v)
        case "R": refs[lane]!.rows[pendingRow] = v; section = ""
        default: break
        }
    }
}
guard let mx4 = refs["MX4"], let mx4far = refs["MX4FAR"], let mx8 = refs["MX8"] else {
    print("FAIL  reference stream is missing a lane"); exit(1)
}

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let lib = try dev.makeLibrary(URL: URL(fileURLWithPath: libPath))
let queue = dev.makeCommandQueue()!
var failures = 0, dispatches = 0
func check(_ ok: Bool, _ pass: String, _ fail: String) {
    if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 }
}
// axiom-4: a command buffer meets us through cb.status/cb.error. Every gate reads a device
// buffer back and those buffers are freshly zeroed, so a failed dispatch reads as a dequant
// that disagrees with the carver at every element — "the GPU is wrong" wearing the costume
// of "the GPU did not run". Counted over the whole run.
var gpuErrors = 0
var gpuFirstError: String? = nil

// --- the plane-split bytes, RESIDENT AS OVERLAPPING VIEWS. onelean (corpus row 850): the 85 GiB file
//     exceeds maxBufferLength, so a single MTLBuffer over the whole mmap FAILs — that is exactly what
//     regressed. mmap the whole file (bytesNoCopy = no copy at all), then wrap it in a small set of
//     overlapping, page-aligned views, arranged so every tensor lies wholly inside one. The geometry is
//     windowed-residency.fk's (proven by its band; agreed byte-for-byte over all 1406 tensors by
//     metal_windowed_residency.sh); the runner applies the same arithmetic to its own device facts. Every
//     tensor bind below goes through viewFor(absOffset) -> (a view, an inner offset). The file is still
//     being appended to; only the already-written prefix is read, and the shell checked that. -----------
let fd = open(blobPath, O_RDONLY)
guard fd >= 0 else { print("FAIL  cannot open blob"); exit(1) }
var st = stat(); fstat(fd, &st)
let fileLen = Int(st.st_size)
let page = Int(getpagesize())
let mapLen = (fileLen + page - 1) / page * page
guard let mapped = mmap(nil, mapLen, PROT_READ, MAP_PRIVATE, fd, 0), mapped != MAP_FAILED else {
    print("FAIL  mmap failed"); exit(1)
}
// the view geometry (windowed-residency.fk): view_limit = maxBufferLength floored to a page; overlap =
// round_up(max_tensor_bytes, page) + page; step = view_limit - overlap. Views at 0, step, 2*step, ...
let viewLimit = Int(dev.maxBufferLength) / page * page
let overlap = ((maxTensorBytes + page - 1) / page * page) + page
guard viewLimit > overlap else {
    print("FAIL  maxBufferLength \(dev.maxBufferLength) too small for a \(maxTensorBytes) B tensor + overlap"); exit(1)
}
let step = viewLimit - overlap
let tUp0 = Date()
var views: [MTLBuffer] = []
var voff = 0
while voff < mapLen {
    let vlen = min(viewLimit, mapLen - voff)
    guard voff % page == 0 else { print("FAIL  view start \(voff) not page-aligned"); exit(1) }
    guard let v = dev.makeBuffer(bytesNoCopy: mapped.advanced(by: voff), length: vlen,
                                 options: .storageModeShared, deallocator: nil) else {
        print("FAIL  view makeBuffer(bytesNoCopy:) failed at \(voff) length \(vlen)"); exit(1)
    }
    v.label = "mx_view_\(views.count)"
    views.append(v)
    if voff + vlen >= mapLen { break }
    voff += step
}
let upSecs = Date().timeIntervalSince(tUp0)
// viewFor: the lookup a hot path passes — the view wholly holding a tensor at `abs`, and its inner offset.
func viewFor(_ abs: Int) -> (MTLBuffer, Int) {
    let idx = abs / step
    return (views[idx], abs - idx * step)
}
// %ld, not %d: String(format:) takes a 32-bit CInt for %d and this file is 85 GiB, which would print as
// a NEGATIVE byte count. A carrier that misreports the size of the thing it mapped is one character from
// misreporting whether it mapped the right thing.
print(String(format: "resident: %ld bytes of the ds4 file mapped as %d overlapping page-aligned views (view_limit %ld B, step %ld B) on %@ in %.4f s, ZERO copies — one buffer over the whole file FAILs; the views do not",
             fileLen, views.count, viewLimit, step, dev.name, upSecs))
// the MXFP4 tensor's slice 0 and its distant slice, each resolved to (view, inner offset) once.
let (mv4, in4) = viewFor(abs4)
let (mvFar, inFar) = viewFor(absFar)
if views.count > 1 { print("      view for slice 0: index \(abs4/step) inner \(in4); distant slice: index \(absFar/step) inner \(inFar)") }
let fixData = try Data(contentsOf: URL(fileURLWithPath: fixPath))
let fixBuf = dev.makeBuffer(bytes: [UInt8](fixData), length: fixData.count, options: .storageModeShared)!

func pipeline(_ n: String) throws -> MTLComputePipelineState { try dev.makeComputePipelineState(function: lib.makeFunction(name: n)!) }
func dispatch(_ p: MTLComputePipelineState, width: Int, _ bind: (MTLComputeCommandEncoder) -> Void) {
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(p); bind(enc)
    let tg = min(p.maxTotalThreadsPerThreadgroup, 256)
    enc.dispatchThreads(MTLSize(width: width, height: 1, depth: 1),
                        threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let e = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(e)" } }
    if cb.status != .completed { gpuErrors += 1
        if gpuFirstError == nil { gpuFirstError = "command buffer status \(cb.status.rawValue), not .completed" } }
    dispatches += 1
}
let u = 5.960464477539063e-08   // 2^-24, the f32 unit roundoff

let p4deq = try pipeline("form_mxfp4_dequant_f32")
let p4mv  = try pipeline("form_mxfp4_matvec_slot_f32")
let p8deq = try pipeline("form_mxfp8_dequant_f32")
let p8mv  = try pipeline("form_mxfp8_matvec_slot_f32")

func dequant(_ p: MTLComputePipelineState, buf: MTLBuffer, base: Int, off: Int, cnt: Int, nel: Int, into out: MTLBuffer) {
    var o32 = UInt32(off), c32 = UInt32(cnt), n32 = UInt32(nel)
    dispatch(p, width: cnt) { enc in
        enc.setBuffer(buf, offset: base, index: 0)
        enc.setBuffer(out, offset: 0, index: 1)
        enc.setBytes(&o32, length: 4, index: 2)
        enc.setBytes(&c32, length: 4, index: 3)
        enc.setBytes(&n32, length: 4, index: 4)
    }
}
// EQUALITY, NO EPSILON — and that is derived, not hoped for. Every MX weight is 2^(e-127) * v where v
// is exactly an f32 (E2M1 needs 2 significand bits, E4M3 needs 4) and the scale is an exact power of
// two, so the product is an exact f32: there is no rounding for the two sides to disagree about.
func bitBad(_ g: UnsafeMutablePointer<Float>, _ r: [Double], _ n: Int) -> Int {
    var bad = 0
    for i in 0..<n where g[i].bitPattern != Float(r[i]).bitPattern { bad += 1 }
    return bad
}

// ================= MXFP4, on real bytes of the live file =================
let tileBuf = dev.makeBuffer(length: tile * 4, options: .storageModeShared)!
let tg = tileBuf.contents().bindMemory(to: Float.self, capacity: tile)

guard let head = mx4.tiles[0], let tailT = mx4.tiles[tail4], let farHead = mx4far.tiles[0],
      head.count == tile, tailT.count == tile, farHead.count == tile else {
    print("FAIL  MX4 reference tile shape"); exit(1)
}

// --- GATE 0: DID THE GPU RUN. Asked before gate 1, which reads tileBuf back — and a tile
// nothing wrote is zeros, a NUMBER the bit-exact gates would grade as disagreement at every
// element. The CPU sentinels tileBuf; a real MXFP4 dequant off the resident file must overwrite
// every element. A survived sentinel is a residency condition (this maps 60+ GiB of a live
// file), not an arithmetic one.
do {
    let sentinel: Float = -424242.0
    for i in 0..<tile { tg[i] = sentinel }
    let before = gpuErrors
    dequant(p4deq, buf: mv4, base: in4, off: 0, cnt: tile, nel: nel4, into: tileBuf)
    var survived = 0
    for i in 0..<tile where tg[i] == sentinel { survived += 1 }
    if gpuErrors > before { print("  command buffer ERROR: \(gpuFirstError ?? "unknown")") }
    check(gpuErrors == before && survived == 0,
      "gate 0 the GPU executes: an MXFP4 dequant off the resident file overwrote all \(tile) sentinels, no command buffer errored",
      "gate 0 THE GPU DID NOT RUN — \(survived)/\(tile) sentinels survived, \(gpuErrors - before) cb error(s); a residency condition, not arithmetic")
    if failures > 0 {
        print("  Gates 1-8 below all read a device buffer back; unwritten it reads as zeros they would")
        print("  grade as disagreement. Refusing them. Free memory and re-run.")
        print("VERDICT FAIL  the GPU did not run; no MX arithmetic was witnessed"); exit(1)
    }
}

dequant(p4deq, buf: mv4, base: in4, off: 0, cnt: tile, nel: nel4, into: tileBuf)
let bh = bitBad(tg, head, tile)
check(bh == 0, "gate 1 MXFP4 head tile bit-exact: all \(tile) GPU-dequantized weights at element 0 of slice 0 equal the CPU carver's",
              "gate 1 MXFP4: \(bh) of \(tile) head weights differ from the carver's")
dequant(p4deq, buf: mv4, base: in4, off: tail4, cnt: tile, nel: nel4, into: tileBuf)
let bt = bitBad(tg, tailT, tile)
check(bt == 0, "gate 2 MXFP4 TAIL tile bit-exact: all \(tile) weights at element \(tail4) (the last of \(nel4), where the payload plane ENDS and the scale plane begins) equal the carver's",
              "gate 2 MXFP4: \(bt) of \(tile) tail weights differ from the carver's")
dequant(p4deq, buf: mvFar, base: inFar, off: 0, cnt: tile, nel: nel4, into: tileBuf)
let bf = bitBad(tg, farHead, tile)
check(bf == 0, "gate 3 MXFP4 DISTANT SLICE bit-exact: all \(tile) weights of slice \(farSlice) (\(absFar - abs4) bytes on — the slice stride, not the layout, is what this tests) equal the carver's",
              "gate 3 MXFP4: \(bf) of \(tile) weights of slice \(farSlice) differ from the carver's")

// gate 4: the WHOLE slice in one dispatch, then read both ends back out of it
let wbuf = dev.makeBuffer(length: nel4 * 4, options: .storageModeShared)!
let tD0 = Date()
dequant(p4deq, buf: mv4, base: in4, off: 0, cnt: nel4, nel: nel4, into: wbuf)
let dqSecs = Date().timeIntervalSince(tD0)
let wg = wbuf.contents().bindMemory(to: Float.self, capacity: nel4)
var badWhole = 0
for i in 0..<tile {
    if wg[i].bitPattern != Float(head[i]).bitPattern { badWhole += 1 }
    if wg[tail4 + i].bitPattern != Float(tailT[i]).bitPattern { badWhole += 1 }
}
check(badWhole == 0,
      String(format: "gate 4 MXFP4 whole slice: all %d elements dequantized in ONE dispatch in %.4f s (%.1fM weights/s); head AND tail read back out of it still equal the carver's", nel4, dqSecs, Double(nel4)/dqSecs/1e6),
      "gate 4 MXFP4: \(badWhole) weights of the whole-slice dequant differ from the carver's")

// gate 5 / 6: the fused slot matvec
let Xf = mx4.x.map { Float($0) }
guard Xf.count == cols4 else { print("FAIL  MX4 x width \(Xf.count) != \(cols4)"); exit(1) }
let xbuf = dev.makeBuffer(bytes: Xf, length: cols4 * 4, options: .storageModeShared)!
let ybuf = dev.makeBuffer(length: mvRows * 4, options: .storageModeShared)!
var r32 = UInt32(mvRows), c32 = UInt32(cols4), n32 = UInt32(nel4)
func matvec4() {
    dispatch(p4mv, width: mvRows * 32) { enc in
        enc.setBuffer(mv4, offset: in4, index: 0)
        enc.setBuffer(xbuf, offset: 0, index: 1)
        enc.setBuffer(ybuf, offset: 0, index: 2)
        enc.setBytes(&r32, length: 4, index: 3)
        enc.setBytes(&c32, length: 4, index: 4)
        enc.setBytes(&n32, length: 4, index: 5)
    }
}
let tM0 = Date(); matvec4(); let mvSecs = Date().timeIntervalSince(tM0)
let y = ybuf.contents().bindMemory(to: Float.self, capacity: mvRows)
var gpu = [Float](repeating: 0, count: mvRows)
for i in 0..<mvRows { gpu[i] = y[i] }

// The slot map splits each row across 32 lanes, so its association differs from a straight fold. The
// bound is the BODY's derived coefficient, not a tolerance invented here.
let coeff4 = Double(mx4.coeff)
var worstFold = 0.0, liveRows = 0
for i in 0..<mvRows {
    var acc: Float = 0.0, absSum = 0.0
    var j = cols4
    while j > 0 { j -= 1; let p = wg[i * cols4 + j] * Xf[j]; acc = p + acc; absSum += abs(Double(p)) }
    let bound = coeff4 * u * absSum
    // A ROW WITH A ZERO BOUND IS NOT AGREEMENT, IT IS AN ABSENT MEASUREMENT. Counted, and demanded to
    // be every row: without this, an all-zero output would print the same reassuring 0.000 that a
    // genuinely exact answer prints, and nothing in the gate could tell them apart.
    if bound > 0 { liveRows += 1; worstFold = max(worstFold, abs(Double(gpu[i]) - Double(acc)) / bound) }
}
check(worstFold < 1.0 && liveRows == mvRows,
      String(format: "gate 5 MXFP4 fused slot matvec: all %d rows (all with a NONZERO bound) agree with the f32 fold over the dequantized buffer at %.3f of the body's derived (cols+chunk+32)*u*SUM|term| bound (%.2f ms, %.2f GMAC/s)", mvRows, worstFold, mvSecs*1e3, Double(mvRows*cols4)/mvSecs/1e9),
      String(format: "gate 5 MXFP4: max deviation %.3f of the derived bound, %d of %d rows had a live bound", worstFold, liveRows, mvRows))

var worstRatio = 0.0, worstRel = 0.0, checked = 0, live6 = 0
for (r, refv) in mx4.rows.sorted(by: { $0.key < $1.key }) {
    guard r < mvRows else { continue }
    var absSum = 0.0
    for j in 0..<cols4 { absSum += abs(Double(wg[r * cols4 + j]) * Double(Xf[j])) }
    let bound = coeff4 * u * absSum
    if bound > 0 { live6 += 1; worstRatio = max(worstRatio, abs(Double(gpu[r]) - refv) / bound) }
    worstRel = max(worstRel, abs(Double(gpu[r]) - refv) / max(abs(refv), 1e-30))
    checked += 1
    // the actual numbers, printed. "0.000 of the bound" is only meaningful next to a real value.
    print(String(format: "      row %d: gpu %.17g   form(fp64) %.17g   SUM|term| %.6g", r, Double(gpu[r]), refv, absSum))
}
// EXACT AGREEMENT IS EXPECTED HERE, AND IT IS DERIVABLE RATHER THAN LUCKY — which is why the gate does
// not merely accept it. Every MXFP4 weight is a multiple of 2^-8 (E2M1 values are multiples of 0.5 and
// this tensor's exponents are 2^-7..2^-5) and every x here is another such weight, so every product is
// a multiple of 2^-16 and every partial sum is too, bounded well under 2^8. A multiple of 2^-16 below
// 2^8 needs at most 24 significand bits — exactly f32's — so the f32 accumulation is EXACT and equals
// the fp64 one bit for bit. The bound is still the gate; the zero is a consequence, not the claim.
check(checked > 0 && live6 == checked && worstRatio < 1.0,
      String(format: "gate 6 MXFP4 is the BODY's answer: %d rows (all with a live bound) match Form's fp64 dot at the real width (cols=%d) at %.3f of the derived bound (value-relative max %.3e — exact, and derivably so: every product is a multiple of 2^-16 and every partial sum fits f32's 24 bits)", checked, cols4, worstRatio, worstRel),
      String(format: "gate 6 MXFP4: max |gpu-form| is %.3f of the derived bound over %d rows (%d live)", worstRatio, checked, live6))

// ================= MXFP8, on the committed real-byte fixture =================
// THE RADIUS, printed rather than implied: no type-41 tensor is inside the downloaded prefix of the
// file (type-41 data begins at absolute 72 766 954 336), so this lane witnesses 1024 real elements of
// blk.0.attn_kv.weight at both ends of the fixture — not both ends of a tensor.
print("--- MXFP8: the committed 1056-byte fixture (real bytes of blk.0.attn_kv.weight); no type-41 tensor is inside the downloaded prefix yet")
let f8buf = dev.makeBuffer(length: 512 * 4, options: .storageModeShared)!
let f8g = f8buf.contents().bindMemory(to: Float.self, capacity: 512)
guard let h8 = mx8.tiles[0], let t8 = mx8.tiles[512], h8.count == 512, t8.count == 512 else {
    print("FAIL  MX8 reference tile shape"); exit(1)
}
dequant(p8deq, buf: fixBuf, base: 0, off: 0, cnt: 512, nel: 1024, into: f8buf)
let b8h = bitBad(f8g, h8, 512)
dequant(p8deq, buf: fixBuf, base: 0, off: 512, cnt: 512, nel: 1024, into: f8buf)
let b8t = bitBad(f8g, t8, 512)
var w8 = [Float](repeating: 0, count: 1024)
let w8buf = dev.makeBuffer(length: 1024 * 4, options: .storageModeShared)!
dequant(p8deq, buf: fixBuf, base: 0, off: 0, cnt: 1024, nel: 1024, into: w8buf)
let w8g = w8buf.contents().bindMemory(to: Float.self, capacity: 1024)
for i in 0..<1024 { w8[i] = w8g[i] }
check(b8h == 0 && b8t == 0,
      "gate 7a MXFP8 bit-exact on the GPU: all 512 head weights AND all 512 tail weights of the fixture equal the CPU carver's (radius: 1024 elements, not a whole tensor)",
      "gate 7a MXFP8: \(b8h) head and \(b8t) tail weights differ from the carver's")

let X8 = mx8.x.map { Float($0) }
let x8buf = dev.makeBuffer(bytes: X8, length: 512 * 4, options: .storageModeShared)!
let y8buf = dev.makeBuffer(length: 2 * 4, options: .storageModeShared)!
var r8 = UInt32(2), c8 = UInt32(512), n8 = UInt32(1024)
dispatch(p8mv, width: 2 * 32) { enc in
    enc.setBuffer(fixBuf, offset: 0, index: 0)
    enc.setBuffer(x8buf, offset: 0, index: 1)
    enc.setBuffer(y8buf, offset: 0, index: 2)
    enc.setBytes(&r8, length: 4, index: 3)
    enc.setBytes(&c8, length: 4, index: 4)
    enc.setBytes(&n8, length: 4, index: 5)
}
let y8 = y8buf.contents().bindMemory(to: Float.self, capacity: 2)
let coeff8 = Double(mx8.coeff)
var worst8 = 0.0, checked8 = 0, live8 = 0
for (r, refv) in mx8.rows.sorted(by: { $0.key < $1.key }) {
    var absSum = 0.0
    for j in 0..<512 { absSum += abs(Double(w8[r * 512 + j]) * Double(X8[j])) }
    let bound = coeff8 * u * absSum
    if bound > 0 { live8 += 1; worst8 = max(worst8, abs(Double(y8[r]) - refv) / bound) }
    checked8 += 1
    print(String(format: "      row %d: gpu %.17g   form(fp64) %.17g   SUM|term| %.6g", r, Double(y8[r]), refv, absSum))
}
check(checked8 == 2 && live8 == 2 && worst8 < 1.0,
      String(format: "gate 7b MXFP8 fused slot matvec: %d rows (both with a live bound) match Form's fp64 dot at %.3f of the body's derived bound", checked8, worst8),
      String(format: "gate 7b MXFP8: max |gpu-form| is %.3f of the derived bound (%d of %d rows live)", worst8, live8, checked8))

// ================= residency =================
var sumFirst = 0.0; for i in 0..<mvRows { sumFirst += Double(gpu[i]) }
let before = dispatches
let tR0 = Date()
for _ in 0..<iters { matvec4() }
let rSecs = Date().timeIntervalSince(tR0)
var sumAfter = 0.0; for i in 0..<mvRows { sumAfter += Double(y[i]) }
check(sumAfter == sumFirst,
      String(format: "gate 8 residency: %d fused dispatches in %.4f s (%.0f us each) with ZERO re-uploads; output checksum unchanged", iters, rSecs, rSecs/Double(iters)*1e6),
      "gate 8: the output checksum changed across dispatches")
print("      dispatch count: \(before) for the whole correctness audit (one command buffer + one encoder each), \(dispatches - before) more for the residency loop — seamtoll: this harness pays a full command buffer per dispatch on purpose, so the numbers above are correctness, never a token-rate claim")

if gpuErrors > 0 {
    print("=== \(gpuErrors) COMMAND BUFFER(S) FAILED during this run — first: \(gpuFirstError ?? "unknown") ===")
    print("    Nothing above this line that reads a device buffer back can be trusted.")
}
let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  \(10) gates, MX on the GPU") } else { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT

swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || {
    echo "FAIL  swiftc failed"; tail -20 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB" "$work/ref.txt" "$BLOB" "$FIX8" "$ITERS" "$TILE" \
    "$ABS" "$NEL" "$COLS" "$TAIL" "$ABSFAR" "$MVROWS" "$FAR" "$MAXTB"
rc=$?
exit $rc
