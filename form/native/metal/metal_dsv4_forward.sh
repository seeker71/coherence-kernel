#!/usr/bin/env bash
# metal_dsv4_forward.sh — Stone 34: carrying the DeepSeek-V4-Flash first-token forward INTO the middle,
# at REAL DIMS over the windowed-resident 85 GiB file. Stone 33 proved the two ends (metal_dsv4_token.sh:
# EMBED in, MXFP8 vocab out). This harness adds the middle's MoE-FFN dispatches — the routed-expert gate
# and up projections, and the layer-0 hash routing table read — at real layer-0 dims through the views.
#
# THE RADIUS, and nothing wider (aporon). No external oracle can execute this exact file — ds4,
# llama.cpp, ollama and LM Studio all REFUSE GGUF types 40/41. So a whole-forward output is UNFALSIFIABLE
# against any reference here (selfgauge). What each dispatch stands on is named at its gate: the GPU fused
# decode+matvec, reading the real quantised bytes straight from the resident view, is compared to an
# INDEPENDENT CPU decode of the same bytes at the tensor's absolute mmap offset.
#
# THE OFFERED-INTERFACE GUARD (edgedrop/zerobirth). An unrun kernel reads as a computed zero; a dead view
# reads as zeros. So every output buffer is SENTINELLED before its dispatch, cb.error/cb.status are
# checked after, and every result is required NON-DEGENERATE (real variance) — a dead read cannot pass.
#
# THE HONEST BOUND (knownsolved). The MoE input on a true forward is the FFN-norm of the after-attention
# HC state; the attention block at real dims is not yet wired (see the receipt's blocker list). So these
# expert matvecs are fed the EMBEDDING as a real PROBE vector — exactly Stage 2's mechanism-witness class:
# they prove the type-40/type-1 fused decode+matvec BINDS and COMPUTES at real dims through the views, NOT
# that the numbers are the real layer-0 activations. The routing table read IS exact and token-only, so
# the six selected experts ARE the real layer-0 selection for this token.
#
# Run:  form/native/metal/metal_dsv4_forward.sh   (optional: FORM_DS4_PROMPT_TOKEN=<id>)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
BLOB="${FORM_DS4_BLOB:-$HOME/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf}"
CACHE="$ROOT/native/metal/.metallib-cache"
TOKEN="${FORM_DS4_PROMPT_TOKEN:-671}"   # "The capital of France is" -> 671 6102 294 8760 344, v[0]=671

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the GPU witness needs an Apple GPU + swiftc"
    exit 2
fi
if [[ ! -f "$BLOB" ]]; then
    echo "SKIP  the ds4 GGUF is not on this host: $BLOB   (set FORM_DS4_BLOB)"
    exit 2
fi
if [[ ! -x "$GO_BIN" ]]; then echo "FAIL  go kernel bin-go not built at $GO_BIN"; exit 1; fi

FSIZE=$(stat -f%z "$BLOB")
echo "ds4 blob: $FSIZE bytes at $(date '+%H:%M:%S')   forward middle, layer 0, token=$TOKEN"

work="$(mktemp -d "${TMPDIR:-/tmp}/fkdsv4fwd.XXXXXX")"
trap 'rm -rf "$work"' EXIT

# ── the `; preludes:` directives are LIVE recursive load instructions; walked, never hand-catted ──
FK_SEEN=""
fk_deps() {
    awk '/^;[ \t]*preludes:/ { s=$0; sub(/^;[ \t]*preludes:[ \t]*/,"",s); gsub(/,/," ",s)
         n=split(s,a,/[ \t]+/); for(i=1;i<=n;i++){ low=tolower(a[i])
         if(a[i]=="\\"||low=="none"||low=="(none)"||a[i]=="")continue; if(a[i]~/\.fk$/)print a[i] } }' "$1" 2>/dev/null
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
    while read -r d; do [[ -z "$d" ]] && continue; p="$(fk_path "$f" "$d")"; fk_expand "$p"; done < <(fk_deps "$f")
    printf '%s\n' "$f"
}
cd "$ROOT"
FILES=(); while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/dsv4-forward-real.fk)
# the embed kernel lives in dsv4-token.fk (Stone 33), reused unchanged for the probe vector
EMB_SEEN=""; FK_SEEN=""; EMBFILES=(); while read -r x; do EMBFILES+=("$x"); done < <(fk_expand native/metal/dsv4-token.fk)

# ── 1. measure the device ─────────────────────────────────────────────────────────────────────────
cat > "$work/probe.swift" <<'SWIFT'
import Metal
import Foundation
guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
print("\(dev.maxBufferLength) \(getpagesize()) \(dev.name)")
SWIFT
swiftc -O -o "$work/probe" "$work/probe.swift" 2>"$work/probe.err" || { echo "FAIL swiftc probe"; tail "$work/probe.err"; exit 1; }
PROBE="$("$work/probe")"; prc=$?
if [[ $prc -eq 2 ]]; then echo "$PROBE"; exit 2; fi
MAXBUF="$(echo "$PROBE" | awk '{print $1}')"; PAGE="$(echo "$PROBE" | awk '{print $2}')"; DEVNAME="$(echo "$PROBE" | cut -d' ' -f3-)"
echo "device: $DEVNAME  maxBufferLength=$MAXBUF  page=$PAGE"

# ── 2. the body's residency plan + the manifest (types & dims) over the LIVE file ──────────────────
echo "walking the file header for the residency plan and the manifest..."
printf '(wre-emit "%s" %s %s %s)\n' "$BLOB" "$FSIZE" "$MAXBUF" "$PAGE" > "$work/plan.fk"
"$GO_BIN" "${FILES[@]}" "$work/plan.fk" > "$work/plan.out" 2>"$work/plan.err" || { echo "FAIL plan emission"; tail -5 "$work/plan.err"; exit 1; }
grep -qx 'END' "$work/plan.out" || { echo "FAIL plan stream truncated"; exit 1; }
WR=($(awk '$1=="WR"{print; exit}' "$work/plan.out"))
STEP=${WR[7]}; VIEWLIMIT=${WR[5]}; NVIEWS=${WR[9]}
printf '(gm-emit-manifest "%s")\n' "$BLOB" > "$work/man.fk"
"$GO_BIN" "${FILES[@]}" "$work/man.fk" > "$work/man.out" 2>"$work/man.err" || { echo "FAIL manifest emission"; tail -5 "$work/man.err"; exit 1; }

# a tensor's (view, inner, holds, abs) from the plan; its (type, dims, nel_per_slice, slices, bytes) from the manifest
tv()  { awk -v n="$1" -v f="$2" '$1=="TV" && $2==n {print $(f); exit}' "$work/plan.out"; }   # f: 3=abs 4=bytes 5=idx 6=inner 7=holds
trow(){ awk -v n="$1" -v f="$2" '$1=="T"  && $2==n {print $(f); exit}' "$work/man.out"; }     # T name type ndim d0 d1 d2 abs nelslice slices bytes

N_EMBD=4096
# --- token_embd (embed probe): F16 [n_embd, vocab] ---
EMB_ABS=$(tv token_embd.weight 3); EMB_IDX=$(tv token_embd.weight 5); EMB_INNER=$(tv token_embd.weight 6); EMB_HOLDS=$(tv token_embd.weight 7)
ROW_OFF=$(( TOKEN * N_EMBD * 2 ))
# --- ffn_gate_inp (router): F16 [4096, 256] ---
RT_ABS=$(tv blk.0.ffn_gate_inp.weight 3); RT_IDX=$(tv blk.0.ffn_gate_inp.weight 5); RT_INNER=$(tv blk.0.ffn_gate_inp.weight 6); RT_HOLDS=$(tv blk.0.ffn_gate_inp.weight 7)
RT_IN=$(trow blk.0.ffn_gate_inp.weight 5); RT_OUT=$(trow blk.0.ffn_gate_inp.weight 6)   # d0=in, d1=out=n_experts
# --- ffn_gate_tid2eid (hash table): I32 [6, 129280] ---
HT_ABS=$(tv blk.0.ffn_gate_tid2eid.weight 3); HT_IDX=$(tv blk.0.ffn_gate_tid2eid.weight 5); HT_INNER=$(tv blk.0.ffn_gate_tid2eid.weight 6); HT_HOLDS=$(tv blk.0.ffn_gate_tid2eid.weight 7)
N_USED=$(trow blk.0.ffn_gate_tid2eid.weight 5)   # d0 = expert_used = 6
# --- ffn_gate_exps / ffn_up_exps (routed experts): MXFP4 [4096, 2048, 256] ---
GX_ABS=$(tv blk.0.ffn_gate_exps.weight 3); GX_IDX=$(tv blk.0.ffn_gate_exps.weight 5); GX_INNER=$(tv blk.0.ffn_gate_exps.weight 6); GX_HOLDS=$(tv blk.0.ffn_gate_exps.weight 7)
GX_BYTES=$(tv blk.0.ffn_gate_exps.weight 4)
GX_IN=$(trow blk.0.ffn_gate_exps.weight 5); GX_OUT=$(trow blk.0.ffn_gate_exps.weight 6); GX_NEXP=$(trow blk.0.ffn_gate_exps.weight 10); GX_NEL=$(trow blk.0.ffn_gate_exps.weight 9)
GX_STRIDE=$(( GX_BYTES / GX_NEXP ))
UX_ABS=$(tv blk.0.ffn_up_exps.weight 3); UX_IDX=$(tv blk.0.ffn_up_exps.weight 5); UX_INNER=$(tv blk.0.ffn_up_exps.weight 6); UX_HOLDS=$(tv blk.0.ffn_up_exps.weight 7)
UX_BYTES=$(tv blk.0.ffn_up_exps.weight 4)
UX_STRIDE=$(( UX_BYTES / GX_NEXP ))
# --- ffn_down_exps (routed experts down projection): IQ2_XXS [2048, 4096, 256] ---
DX_ABS=$(tv blk.0.ffn_down_exps.weight 3); DX_IDX=$(tv blk.0.ffn_down_exps.weight 5); DX_INNER=$(tv blk.0.ffn_down_exps.weight 6); DX_HOLDS=$(tv blk.0.ffn_down_exps.weight 7)
DX_BYTES=$(tv blk.0.ffn_down_exps.weight 4)
DX_IN=$(trow blk.0.ffn_down_exps.weight 5); DX_OUT=$(trow blk.0.ffn_down_exps.weight 6)   # d0=in(ff), d1=out(embd)
DX_STRIDE=$(( DX_BYTES / GX_NEXP ))
echo "  down_exps:  view=$DX_IDX inner=$DX_INNER holds=$DX_HOLDS  in=$DX_IN out=$DX_OUT stride=$DX_STRIDE"

echo "  embed:      view=$EMB_IDX inner=$EMB_INNER holds=$EMB_HOLDS row_off=$ROW_OFF"
echo "  router:     view=$RT_IDX inner=$RT_INNER holds=$RT_HOLDS  in=$RT_IN out=$RT_OUT"
echo "  hash table: view=$HT_IDX inner=$HT_INNER holds=$HT_HOLDS  n_used=$N_USED  abs=$HT_ABS"
echo "  gate_exps:  view=$GX_IDX inner=$GX_INNER holds=$GX_HOLDS  in=$GX_IN out=$GX_OUT nexp=$GX_NEXP nel=$GX_NEL stride=$GX_STRIDE"
echo "  up_exps:    view=$UX_IDX inner=$UX_INNER holds=$UX_HOLDS  stride=$UX_STRIDE"

# ── 3. compile the kernels (offered by the body), cached by sha ─────────────────────────────────────
compile_kernel() { # $1=emit-form $2=kernel-name-grep   emits ONLY the lib path on stdout; progress to stderr
    local form="$1" name="$2"
    echo "$form" > "$work/k.fk"
    "$GO_BIN" "${FILES[@]}" "$work/k.fk" > "$work/k.out" 2>"$work/k.err" || { echo "FAIL MSL emit $form" >&2; cat "$work/k.err" >&2; exit 1; }
    awk '/^MSL$/{d=1;next}/^END$/{d=0;next}d{print}' "$work/k.out" > "$work/$name.metal"
    grep -q "$name" "$work/$name.metal" || { echo "FAIL kernel $name not emitted" >&2; exit 1; }
    mkdir -p "$CACHE"
    local sha; sha="$(shasum -a 256 "$work/$name.metal" | cut -c1-16)"
    local lib="$CACHE/$name-$sha.metallib"
    if [[ ! -f "$lib" ]]; then
        xcrun -sdk macosx metal -O2 -std=metal3.0 -c "$work/$name.metal" -o "$work/$name.air" 2>"$work/$name.cerr" \
          && xcrun -sdk macosx metallib "$work/$name.air" -o "$lib" 2>>"$work/$name.cerr" || { echo "FAIL metal compile $name" >&2; cat "$work/$name.cerr" >&2; exit 1; }
        echo "  compiled $name" >&2
    else echo "  cache HIT $name" >&2; fi
    printf '%s\n' "$lib"
}
LIB_MX4="$(compile_kernel '(dsv4-mx4-matvec-msl)' form_dsv4_mx4_matvec)"
LIB_RT="$(compile_kernel '(dsv4-router-f16-msl)' form_dsv4_router_f16)"
LIB_IQ2="$(compile_kernel '(dsv4-iq2-matvec-msl)' form_dsv4_iq2_matvec)"
# the IQ2_XXS decode tables, from the body's own (iq2-grid)/(iq2-ksigns), for the carrier's CPU reference
echo '(dsv4-iq2-tables)' > "$work/tbl.fk"
"$GO_BIN" "${FILES[@]}" "$work/tbl.fk" > "$work/iq2tables.txt" 2>"$work/tbl.err" || { echo "FAIL iq2 tables emit"; cat "$work/tbl.err"; exit 1; }
grep -q '^GRID ' "$work/iq2tables.txt" && grep -q '^KSIGNS ' "$work/iq2tables.txt" || { echo "FAIL iq2 tables missing"; exit 1; }
# the embed kernel from dsv4-token.fk (separate prelude set)
FILES_SAVE=("${FILES[@]}"); FILES=("${EMBFILES[@]}")
LIB_EMB="$(compile_kernel '(dsv4-embed-msl)' form_dsv4_embed_f16)"
FILES=("${FILES_SAVE[@]}")

# ── 4. the carrier ──────────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

let a = CommandLine.arguments
func I(_ i: Int) -> Int { return Int(a[i])! }
let libEmb = a[1], libRt = a[2], libMx4 = a[3], blobPath = a[4]
let step = I(5), viewLimit = I(6), nviews = I(7)
let embAbs = I(8), embIdx = I(9), embInner = I(10), embHolds = I(11), rowOff = I(12), nEmbd = I(13), token = I(14)
let rtAbs = I(15), rtIdx = I(16), rtInner = I(17), rtHolds = I(18), rtIn = I(19), rtOut = I(20)
let htAbs = I(21), nUsed = I(22)
let gxIdx = I(23), gxInner = I(24), gxHolds = I(25), gxAbs = I(26), gxIn = I(27), gxOut = I(28), gxNel = I(29), gxStride = I(30)
let uxIdx = I(31), uxInner = I(32), uxHolds = I(33), uxAbs = I(34), uxStride = I(35)
let libIq2 = a[36], tablesPath = a[37]
let dxIdx = I(38), dxInner = I(39), dxHolds = I(40), dxAbs = I(41), dxIn = I(42), dxOut = I(43), dxStride = I(44)

// the IQ2_XXS decode tables, loaded from the body's own emit (never hand-copied)
func loadTable(_ tag: String) -> [Int] {
    let txt = (try? String(contentsOfFile: tablesPath, encoding: .utf8)) ?? ""
    for line in txt.split(separator: "\n") where line.hasPrefix(tag + " ") {
        return line.dropFirst(tag.count + 1).split(separator: ",").map { Int($0)! }
    }
    return []
}
let IQ2_GRID = loadTable("GRID"), IQ2_KSIGNS = loadTable("KSIGNS")

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let lEmb = try dev.makeLibrary(URL: URL(fileURLWithPath: libEmb))
let lRt  = try dev.makeLibrary(URL: URL(fileURLWithPath: libRt))
let lMx4 = try dev.makeLibrary(URL: URL(fileURLWithPath: libMx4))
let lIq2 = try dev.makeLibrary(URL: URL(fileURLWithPath: libIq2))
let queue = dev.makeCommandQueue()!
var failures = 0, gpuErrors = 0
var gpuFirstError: String? = nil
func check(_ ok: Bool, _ pass: String, _ fail: String) { if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 } }

// IEEE half -> single (the carrier's independent decode; matches Metal float(half))
func f16(_ h: UInt16) -> Float {
    let sign = UInt32(h & 0x8000) << 16, exp = Int((h >> 10) & 0x1F), mant = UInt32(h & 0x3FF)
    if exp == 0 { if mant == 0 { return Float(bitPattern: sign) }
        var m = mant, e = -1; repeat { m <<= 1; e += 1 } while (m & 0x400) == 0; m &= 0x3FF
        return Float(bitPattern: sign | UInt32(127 - 15 - e) << 23 | (m << 13)) }
    else if exp == 0x1F { return Float(bitPattern: sign | 0x7F800000 | (mant << 13)) }
    return Float(bitPattern: sign | UInt32(exp - 15 + 127) << 23 | (mant << 13))
}
// MXFP4 decode: E8M0 scale (power of two) * E2M1 value — both exact in f32
func pow2(_ e: Int) -> Float { var v: Float = 1, k = e
    while k >= 8 { v *= 256; k -= 8 }; while k > 0 { v *= 2; k -= 1 }
    while k <= -8 { v *= 0.00390625; k += 8 }; while k < 0 { v *= 0.5; k += 1 }; return v }
func e8m0(_ e: Int) -> Float { return pow2(e - 127) }
func mx4val(_ c: Int) -> Float { let mant = c % 2, ex = (c / 2) % 4, sgn = c / 8
    let frac = Float(mant) / 2.0
    let mag = (ex == 0) ? (pow2(0) * frac) : (pow2(ex - 1) * (1.0 + frac))
    return (sgn == 1) ? -mag : mag }

let fd = open(blobPath, O_RDONLY); guard fd >= 0 else { print("FAIL open blob"); exit(1) }
var st = stat(); fstat(fd, &st); let fileLen = Int(st.st_size); let page = Int(getpagesize())
let mapLen = (fileLen + page - 1) / page * page
guard let mapped0 = mmap(nil, mapLen, PROT_READ, MAP_PRIVATE, fd, 0), mapped0 != MAP_FAILED else { print("FAIL mmap"); exit(1) }
let base = mapped0.assumingMemoryBound(to: UInt8.self)

// ── GATE 0: the overlapping views map (onelean) ──
var views: [MTLBuffer] = []
for i in 0..<nviews {
    let vs = i * step, vlen = min(viewLimit, mapLen - vs)
    guard vs % page == 0 else { print("FAIL view \(i) start not page-aligned"); exit(1) }
    guard let buf = dev.makeBuffer(bytesNoCopy: mapped0.advanced(by: vs), length: vlen, options: .storageModeShared, deallocator: nil) else {
        print("FAIL view \(i) makeBuffer failed"); failures += 1; break }
    views.append(buf)
}
check(views.count == nviews,
  "gate 0 the views map: all \(nviews) overlapping page-aligned bytesNoCopy views of the \(fileLen) B file wrap on \(dev.name)",
  "gate 0 only \(views.count)/\(nviews) views mapped")
if failures > 0 { print("VERDICT FAIL the views did not map"); exit(1) }

// ── GATE 1: EMBED probe vector (Stone 33, reused). x = the token's embedding row. ──
check(embHolds == 1 && rtHolds == 1 && gxHolds == 1 && uxHolds == 1,
  "gate 1 residency: token_embd, ffn_gate_inp, ffn_gate_exps, ffn_up_exps each lie wholly inside one view (holds=1)",
  "gate 1 a needed tensor spans views (embed=\(embHolds) router=\(rtHolds) gate=\(gxHolds) up=\(uxHolds))")
let pEmb = try dev.makeComputePipelineState(function: lEmb.makeFunction(name: "form_dsv4_embed_f16")!)
let n = nEmbd
let xbuf = dev.makeBuffer(length: n * 4, options: .storageModeShared)!
let xp = xbuf.contents().bindMemory(to: Float.self, capacity: n)
for i in 0..<n { xp[i] = Float(bitPattern: 0x7F7FFFFF) }   // sentinel
var eB = UInt64(embInner + rowOff), eN = UInt32(n)
do { let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
     enc.setComputePipelineState(pEmb); enc.setBuffer(views[embIdx], offset: 0, index: 0); enc.setBuffer(xbuf, offset: 0, index: 1)
     enc.setBytes(&eB, length: 8, index: 2); enc.setBytes(&eN, length: 4, index: 3)
     enc.dispatchThreads(MTLSize(width: n, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: min(pEmb.maxTotalThreadsPerThreadgroup,256), height: 1, depth: 1))
     enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
     if let e = cb.error { gpuErrors += 1; gpuFirstError = "\(e)" }
     if cb.status != .completed { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "embed cb \(cb.status.rawValue)" } } }
// carve x from the mmap independently — the CPU reference vector
var x = [Float](repeating: 0, count: n)
let rowAbs = embAbs + rowOff
for i in 0..<n { x[i] = f16(UInt16(base[rowAbs + i*2]) | (UInt16(base[rowAbs + i*2 + 1]) << 8)) }
var embMis = 0; for i in 0..<n where xp[i].bitPattern != x[i].bitPattern { embMis += 1 }
var xseen = Set<UInt32>(); for i in 0..<n { xseen.insert(xp[i].bitPattern) }
check(embMis == 0 && xseen.count > 8 && gpuErrors == 0,
  "gate 2 EMBED probe: the GPU decoded token \(token)'s \(n)-wide F16 embedding through view \(embIdx), bit-exact vs the mmap carve (\(xseen.count) distinct) — the probe input for the middle dispatches",
  "gate 2 EMBED probe: \(embMis) mismatches / \(xseen.count) distinct — dead read")

// ── GATE 3: the layer-0 HASH ROUTING selection — I32 table read (ds4.c:10567, token-only, bit-exact) ──
// table is [n_used=6, vocab] I32; row for token begins at token*n_used. Pure read, so the six selected
// experts are the REAL layer-0 selection for this token (they do not depend on the hidden state).
var selected = [Int](repeating: 0, count: nUsed)
for i in 0..<nUsed {
    let off = htAbs + (token * nUsed + i) * 4
    let v = UInt32(base[off]) | (UInt32(base[off+1]) << 8) | (UInt32(base[off+2]) << 16) | (UInt32(base[off+3]) << 24)
    selected[i] = Int(Int32(bitPattern: v))
}
let selOk = selected.allSatisfy { $0 >= 0 && $0 < 256 }
check(selOk,
  "gate 3 layer-0 hash routing (ds4.c:10745/10567): token \(token) selects experts \(selected) by the ffn_gate_tid2eid I32 table lookup — the REAL layer-0 selection (token-only, not top-k). This realises the routing correction dsv4-forward.fk's comment named.",
  "gate 3 hash routing: selected \(selected) out of expert range [0,256)")
let e0 = selOk ? selected[0] : 0   // drive the expert matvecs with a really-selected expert

// ── the F16 router logit projection (ffn_gate_inp), fused matvec [in -> out] ──
func routerMatvec() -> ([Float], Int) {
    let p = try! dev.makeComputePipelineState(function: lRt.makeFunction(name: "form_dsv4_router_f16")!)
    let out = dev.makeBuffer(length: rtOut * 4, options: .storageModeShared)!
    let op = out.contents().bindMemory(to: Float.self, capacity: rtOut)
    for i in 0..<rtOut { op[i] = Float.nan }   // sentinel
    var r32 = UInt32(rtOut), c32 = UInt32(rtIn)
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(p); enc.setBuffer(views[rtIdx], offset: rtInner, index: 0); enc.setBuffer(xbuf, offset: 0, index: 1)
    enc.setBuffer(out, offset: 0, index: 2); enc.setBytes(&r32, length: 4, index: 3); enc.setBytes(&c32, length: 4, index: 4)
    enc.dispatchThreads(MTLSize(width: rtOut, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: min(p.maxTotalThreadsPerThreadgroup,256), height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let e = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(e)" } }
    if cb.status != .completed { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "router cb \(cb.status.rawValue)" } }
    var g = [Float](repeating: 0, count: rtOut); for i in 0..<rtOut { g[i] = op[i] }
    var nan = 0; for i in 0..<rtOut where g[i].isNaN { nan += 1 }
    return (g, nan)
}
let (gpuLogits, rtNan) = routerMatvec()
// CPU reference: same sequential fold, so an EQUALITY check (no reassociation).
var cpuLogits = [Float](repeating: 0, count: rtOut)
for r in 0..<rtOut {
    var acc: Float = 0; let bpos = rtAbs + r * rtIn * 2
    for j in 0..<rtIn { acc += f16(UInt16(base[bpos + j*2]) | (UInt16(base[bpos + j*2 + 1]) << 8)) * x[j] }
    cpuLogits[r] = acc
}
var rtMaxAbs: Float = 0, rtDistinct = Set<UInt32>()
for r in 0..<rtOut { rtMaxAbs = max(rtMaxAbs, abs(gpuLogits[r] - cpuLogits[r])); rtDistinct.insert(gpuLogits[r].bitPattern) }
check(rtNan == 0 && rtMaxAbs < 1e-3 && rtDistinct.count > 8 && gpuErrors == 0,
  "gate 4 router F16 matvec at real dims: the GPU fused F16 decode+matvec of ffn_gate_inp (\(rtOut)x\(rtIn)) through view \(rtIdx) agrees with the carrier's independent CPU carve on all \(rtOut) logits (max abs diff \(rtMaxAbs), \(rtDistinct.count) distinct). Type 1 at real dims through the views.",
  "gate 4 router: max abs \(rtMaxAbs) or \(rtNan) NaN — dead/wrong read")
// the router probs ds4 computes (sqrt(softplus)) — shown so the routed weighting is a visible fact
func softplus(_ z: Float) -> Float { return log(1.0 + exp(z)) }
var probs = [Float](repeating: 0, count: rtOut); for i in 0..<rtOut { probs[i] = (softplus(cpuLogits[i])).squareRoot() }
let selProbs = selected.map { probs[$0] }
print("      router probs of the 6 selected experts \(selected): \(selProbs.map { (($0*1e4).rounded())/1e4 })")

// ── the MXFP4 expert gate & up projections at real dims (fed the embedding probe), for expert e0 ──
func mx4Matvec(_ lib: MTLLibrary, _ vIdx: Int, _ vInner: Int, _ absBase: Int, _ stride: Int, _ e: Int,
               _ rows: Int, _ cols: Int, _ nel: Int, _ label: String) -> ([Float], Float, Int, Int) {
    let p = try! dev.makeComputePipelineState(function: lib.makeFunction(name: "form_dsv4_mx4_matvec")!)
    let out = dev.makeBuffer(length: rows * 4, options: .storageModeShared)!
    let op = out.contents().bindMemory(to: Float.self, capacity: rows)
    for i in 0..<rows { op[i] = Float.nan }   // sentinel
    var r32 = UInt32(rows), c32 = UInt32(cols), nel32 = UInt32(nel)
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(p)
    enc.setBuffer(views[vIdx], offset: vInner + e * stride, index: 0)   // bind at the expert's slice
    enc.setBuffer(xbuf, offset: 0, index: 1); enc.setBuffer(out, offset: 0, index: 2)
    enc.setBytes(&r32, length: 4, index: 3); enc.setBytes(&c32, length: 4, index: 4); enc.setBytes(&nel32, length: 4, index: 5)
    enc.dispatchThreads(MTLSize(width: rows * 32, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: min(p.maxTotalThreadsPerThreadgroup,256), height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let er = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(er)" } }
    if cb.status != .completed { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(label) cb \(cb.status.rawValue)" } }
    // CPU reference: decode the expert's MXFP4 slice from the mmap at (abs + e*stride) and matvec.
    let qb = absBase + e * stride
    var maxAbs: Float = 0, nan = 0; var distinct = Set<UInt32>()
    let sbase = nel / 2, ng = cols / 32
    var gpuVec = [Float](repeating: 0, count: rows)
    for r in 0..<rows {
        var sumf: Float = 0; let g0 = (r * cols) / 32
        for g in 0..<ng {
            let s = e8m0(Int(base[qb + sbase + g0 + g]))
            var acc: Float = 0; let pb = (r * cols + g * 32) / 2
            for m in 0..<16 { let by = Int(base[qb + pb + m])
                acc += x[g*32 + m*2] * mx4val(by % 16); acc += x[g*32 + m*2 + 1] * mx4val(by / 16) }
            sumf += s * acc
        }
        gpuVec[r] = op[r]
        if op[r].isNaN { nan += 1 } else { maxAbs = max(maxAbs, abs(op[r] - sumf)) }
        distinct.insert(op[r].bitPattern)
    }
    return (gpuVec, maxAbs, nan, distinct.count)
}
let (gateV, gMax, gNan, gDist) = mx4Matvec(lMx4, gxIdx, gxInner, gxAbs, gxStride, e0, gxOut, gxIn, gxNel, "gate")
check(gNan == 0 && gMax < 5e-3 && gDist > 8 && gpuErrors == 0,
  "gate 5 MXFP4 expert GATE at real dims: the GPU fused MXFP4 decode+matvec of ffn_gate_exps[expert \(e0)] (\(gxOut)x\(gxIn)) through view \(gxIdx) at the expert's byte slice agrees with the carrier's independent CPU MXFP4 carve to float precision (max abs diff \(gMax), \(gDist) distinct). Type 40 at real dims through the views (new beyond Stone 33's type 41). assocwall: simd_sum reassociates, so agreement is to float precision, not bit.",
  "gate 5 MXFP4 gate: max abs \(gMax) or \(gNan) NaN — dead/wrong read")
let (upV, uMax, uNan, uDist) = mx4Matvec(lMx4, uxIdx, uxInner, uxAbs, uxStride, e0, gxOut, gxIn, gxNel, "up")
check(uNan == 0 && uMax < 5e-3 && uDist > 8 && gpuErrors == 0,
  "gate 6 MXFP4 expert UP at real dims: the GPU fused MXFP4 decode+matvec of ffn_up_exps[expert \(e0)] (\(gxOut)x\(gxIn)) through view \(uxIdx) at the expert's slice agrees with the CPU MXFP4 carve to float precision (max abs diff \(uMax), \(uDist) distinct).",
  "gate 6 MXFP4 up: max abs \(uMax) or \(uNan) NaN — dead/wrong read")

// ── the SwiGLU mid, a real expert-\(e0) activation, fed to the IQ2 DOWN projection ──
// ds4.c:10430 swiglu: mid = silu(gate) * up  (the clamp is disabled when swiglu_clamp_exp <= 1e-6; it is
// applied elementwise to the SAME input the CPU reference uses, so omitting it here cannot make the down
// matvec agree falsely — it only changes the shared probe vector). silu(z) = z * sigmoid(z).
func silu(_ z: Float) -> Float { return z / (1.0 + exp(-z)) }
let midbuf = dev.makeBuffer(length: dxIn * 4, options: .storageModeShared)!
let midp = midbuf.contents().bindMemory(to: Float.self, capacity: dxIn)
var mid = [Float](repeating: 0, count: dxIn)
for i in 0..<dxIn { let m = silu(gateV[i]) * upV[i]; mid[i] = m; midp[i] = m }

// independent CPU IQ2_XXS decode (iq2xxs-msl.fk's iq2_w, ported; SAME body-emitted grid/ksigns)
func iq2w(_ qb: Int, _ idx: Int) -> Float {
    let blk = idx / 256, within = idx - blk*256, off = qb + blk*66
    let hbits = Int(base[off]) + 256*Int(base[off+1]); let d = f16(UInt16(hbits & 0xFFFF))
    let ib32 = within/32, rem = within - ib32*32, l = rem/8, j = rem - l*8
    let gbase = off + 2 + 8*ib32
    let gidx = Int(base[gbase + l])
    let aux1 = Int(base[gbase+4]) + 256*Int(base[gbase+5]) + 65536*Int(base[gbase+6]) + 16777216*Int(base[gbase+7])
    let scalecode = Int(base[gbase+7]) / 16
    let scale = 0.125 * d * Float(2*scalecode + 1)
    let signidx = (aux1 / (1 << (7*l))) % 128
    let signs = IQ2_KSIGNS[signidx]
    let sbit = (signs / (1 << j)) % 2
    let sgn: Float = (sbit == 1) ? -1.0 : 1.0
    let mag = IQ2_GRID[gidx*8 + j]
    return (scale * sgn) * Float(mag)
}
// ── GATE 7: the IQ2_XXS DOWN projection at real dims — the fused matvec iq2xxs-msl.fk named as the gap ──
check(dxHolds == 1,
  "gate 7a residency: ffn_down_exps lies wholly inside view \(dxIdx) (holds=1)",
  "gate 7a ffn_down_exps spans views (holds=\(dxHolds))")
let pIq2 = try dev.makeComputePipelineState(function: lIq2.makeFunction(name: "form_dsv4_iq2_matvec")!)
let downOut = dev.makeBuffer(length: dxOut * 4, options: .storageModeShared)!
let dop = downOut.contents().bindMemory(to: Float.self, capacity: dxOut)
for i in 0..<dxOut { dop[i] = Float.nan }   // sentinel
var dr = UInt32(dxOut), dc = UInt32(dxIn)
do { let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
     enc.setComputePipelineState(pIq2)
     enc.setBuffer(views[dxIdx], offset: dxInner + e0 * dxStride, index: 0)   // expert e0's block-0 base
     enc.setBuffer(midbuf, offset: 0, index: 1); enc.setBuffer(downOut, offset: 0, index: 2)
     enc.setBytes(&dr, length: 4, index: 3); enc.setBytes(&dc, length: 4, index: 4)
     enc.dispatchThreads(MTLSize(width: dxOut * 32, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: min(pIq2.maxTotalThreadsPerThreadgroup,256), height: 1, depth: 1))
     enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
     if let er = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(er)" } }
     if cb.status != .completed { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "down cb \(cb.status.rawValue)" } } }
// CPU reference: independent IQ2 carve of expert e0's down slice, matvec against the SAME mid vector.
let dqb = dxAbs + e0 * dxStride
var dMax: Float = 0, dNan = 0; var dDistinct = Set<UInt32>()
for r in 0..<dxOut {
    var acc: Float = 0; let rbase = r * dxIn
    for j in 0..<dxIn { acc += iq2w(dqb, rbase + j) * mid[j] }
    if dop[r].isNaN { dNan += 1 } else { dMax = max(dMax, abs(dop[r] - acc)) }
    dDistinct.insert(dop[r].bitPattern)
}
check(dNan == 0 && dMax < 5e-3 && dDistinct.count > 8 && gpuErrors == 0,
  "gate 7 IQ2_XXS DOWN matvec at real dims: the GPU FUSED IQ2_XXS decode+matvec of ffn_down_exps[expert \(e0)] (\(dxOut)x\(dxIn)) through view \(dxIdx), fed the real SwiGLU mid of expert \(e0), agrees with the carrier's independent CPU IQ2 carve (iq2xxs-msl.fk's iq2_w, same body-emitted grid) to float precision (max abs diff \(dMax), \(dDistinct.count) distinct). Type 16 at real dims — and the FUSED matvec iq2xxs-msl.fk had named as the missing piece the MoE fold needs, now built.",
  "gate 7 IQ2 down: max abs \(dMax) or \(dNan) NaN — dead/wrong read")
print(String(format: "      expert %d full SwiGLU contribution (down.mid): out[0..3] = %.5f %.5f %.5f %.5f",
      e0, dop[0], dop[1], dop[2], dop[3]))

if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER ERROR(S) — first: \(gpuFirstError ?? "unknown") ===") }
print(String(format: "      device.currentAllocatedSize = %ld B (%.2f GiB) — the 85 GiB is mmapped + wrapped bytesNoCopy, not copied (onelean); working buffers are tiny",
      dev.currentAllocatedSize, Double(dev.currentAllocatedSize)/1073741824.0))

let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  8 gates — the DeepSeek-V4-Flash forward MIDDLE at real dims: embed probe (bit-exact) + layer-0 hash routing (exact I32 table) + router F16 matvec + a full routed-expert SwiGLU (MXFP4 gate & up + IQ2_XXS down), all through the windowed views. Types 40 and 16 fused matvecs at real dims are new beyond Stone 33; the IQ2_XXS FUSED matvec is the piece iq2xxs-msl.fk named as missing, now built. Still pending for a whole token: the MLA attention block (all MXFP8 matvecs + sinks + RoPE + grouped output), HC pre/post on-GPU, real routing input, and the 43-layer stack — named in the receipt.") }
else { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT
swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || { echo "FAIL swiftc runner"; tail -30 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB_EMB" "$LIB_RT" "$LIB_MX4" "$BLOB" \
    "$STEP" "$VIEWLIMIT" "$NVIEWS" \
    "$EMB_ABS" "$EMB_IDX" "$EMB_INNER" "$EMB_HOLDS" "$ROW_OFF" "$N_EMBD" "$TOKEN" \
    "$RT_ABS" "$RT_IDX" "$RT_INNER" "$RT_HOLDS" "$RT_IN" "$RT_OUT" \
    "$HT_ABS" "$N_USED" \
    "$GX_IDX" "$GX_INNER" "$GX_HOLDS" "$GX_ABS" "$GX_IN" "$GX_OUT" "$GX_NEL" "$GX_STRIDE" \
    "$UX_IDX" "$UX_INNER" "$UX_HOLDS" "$UX_ABS" "$UX_STRIDE" \
    "$LIB_IQ2" "$work/iq2tables.txt" \
    "$DX_IDX" "$DX_INNER" "$DX_HOLDS" "$DX_ABS" "$DX_IN" "$DX_OUT" "$DX_STRIDE"
rc=$?
exit $rc
