#!/usr/bin/env bash
# metal_dsv4_layer.sh — Stone 35, Stage 1: ENTER the DeepSeek-V4-Flash MLA attention block at REAL DIMS
# over the windowed-resident 85 GiB file. Stone 33 proved the two ends (metal_dsv4_token.sh: EMBED in,
# MXFP8 vocab out). Stone 34 proved the MoE-FFN middle. This harness proves the ATTENTION block's ENTRY:
# the input RMSNorm, the low-rank Q and KV down-projections (attn_q_a 4096->1024, attn_kv 4096->512, both
# type-41 MXFP8), and the two rank-space RMSNorms (attn_q_a_norm 1024, attn_kv_a_norm 512) — each dispatch
# on the file's OWN blk.0 weights through the overlapping views, each checked against an INDEPENDENT CPU
# carve at the tensor's absolute mmap offset. The kernels are mla-msl.fk's (MLA_MAX_HD=512 = the real
# head_dim) and mxfp8-msl.fk's (the vocab-projection kernel, unchanged), authored by the body.
#
# THE RADIUS (aporon). No external oracle can run this file — ds4/llama.cpp/ollama REFUSE types 40/41, so
# any activation is UNFALSIFIABLE against a reference (selfgauge). Each dispatch stands on the internal
# falsifier: GPU-through-the-view == independent CPU carve of the same bytes, to a stated f32 bound (the
# MXFP8/F32 weight decode is exact; a matvec and a Newton-sqrt reassociate — assocwall, row 866).
#
# THE HONEST BOUND on the INPUT (knownsolved). The true MLA input at layer 0 is the HC-pre of the residual
# stream, not yet wired. So the chain is fed the token's real EMBEDDING as a PROBE vector — exactly Stone
# 33/34's mechanism-witness class: it proves the projections and rank-norms BIND and COMPUTE at real dims
# through the views, NOT that the numbers are the real layer-0 activations. Tensors/dims/offsets ARE real.
#
# THE OFFERED-INTERFACE GUARD (edgedrop/zerobirth). Every output buffer is SENTINELLED (NaN) before its
# dispatch and cb.error/cb.status checked after; every result is required NON-DEGENERATE. A dead read cannot pass.
#
# Run:  form/native/metal/metal_dsv4_layer.sh   (optional: FORM_DS4_PROMPT_TOKEN=<id>)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
BLOB="${FORM_DS4_BLOB:-$HOME/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf}"
CACHE="$ROOT/native/metal/.metallib-cache"
TOKEN="${FORM_DS4_PROMPT_TOKEN:-671}"   # "The capital of France is" -> 671 ...

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the GPU witness needs an Apple GPU + swiftc"; exit 2
fi
if [[ ! -f "$BLOB" ]]; then
    echo "SKIP  the ds4 GGUF is not on this host: $BLOB   (set FORM_DS4_BLOB)"; exit 2
fi
if [[ ! -x "$GO_BIN" ]]; then
    echo "  building the Go kernel..."; (cd "$ROOT/form-kernel-go" && go build -o bin-go .) || { echo "FAIL go build"; exit 1; }
fi
FSIZE=$(stat -f%z "$BLOB")
work="$(mktemp -d)"; trap 'rm -rf "$work"' EXIT
echo "ds4 blob: $FSIZE bytes at $(date '+%H:%M:%S')   MLA-entry probe(token=$TOKEN)"

# ── the `; preludes:` directives are LIVE recursive load instructions; walked, never hand-catted ──
fk_deps(){ awk 'BEGIN{IGNORECASE=1} /^;[ \t]*preludes:/{ s=$0; sub(/^;[ \t]*preludes:[ \t]*/,"",s); n=split(s,a,/[ \t]+/); for(i=1;i<=n;i++){ if(a[i]=="\\"||tolower(a[i])=="none"||tolower(a[i])=="(none)"||a[i]=="")continue; if(a[i]~/\.fk$/)print a[i] } }' "$1" 2>/dev/null; }
fk_path(){ local dir; dir="$(dirname "$1")"; if [[ -f "$dir/$2" ]]; then printf '%s\n' "$dir/$2"; elif [[ -f "$2" ]]; then printf '%s\n' "$2"; elif [[ "$2" == form/* && -f "${2#form/}" ]]; then printf '%s\n' "${2#form/}"; else printf '%s\n' "$dir/$2"; fi; }
fk_expand(){ local f="$1" d p; case " $FK_SEEN " in *" $f "*) return ;; esac; FK_SEEN="$FK_SEEN $f"; while read -r d; do [[ -z "$d" ]] && continue; p="$(fk_path "$f" "$d")"; fk_expand "$p"; done < <(fk_deps "$f"); printf '%s\n' "$f"; }
cd "$ROOT"
FK_SEEN=""; FILES=(); while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/dsv4-mla-real.fk)

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

# ── 2. the residency plan (view/inner/holds per tensor) + the manifest (types & dims) over the file ──
echo "walking the file header for the residency plan and the manifest..."
printf '(wre-emit "%s" %s %s %s)\n' "$BLOB" "$FSIZE" "$MAXBUF" "$PAGE" > "$work/plan.fk"
"$GO_BIN" "${FILES[@]}" "$work/plan.fk" > "$work/plan.out" 2>"$work/plan.err" || { echo "FAIL plan emission"; tail -5 "$work/plan.err"; exit 1; }
grep -qx 'END' "$work/plan.out" || { echo "FAIL plan stream truncated"; exit 1; }
WR=($(awk '$1=="WR"{print; exit}' "$work/plan.out"))
STEP=${WR[7]}; VIEWLIMIT=${WR[5]}; NVIEWS=${WR[9]}
printf '(gm-emit-manifest "%s")\n' "$BLOB" > "$work/man.fk"
"$GO_BIN" "${FILES[@]}" "$work/man.fk" > "$work/man.out" 2>"$work/man.err" || { echo "FAIL manifest emission"; tail -5 "$work/man.err"; exit 1; }
echo "  plan: view_limit=$VIEWLIMIT step=$STEP nviews=$NVIEWS"

tv()  { awk -v n="$1" -v f="$2" '$1=="TV" && $2==n {print $(f); exit}' "$work/plan.out"; }  # 3=abs 4=bytes 5=idx 6=inner 7=holds
trow(){ awk -v n="$1" -v f="$2" '$1=="T"  && $2==n {print $(f); exit}' "$work/man.out"; }    # T name type ndim d0 d1 d2 abs nelslice slices bytes

N_EMBD=4096
# token_embd — F16 [n_embd, vocab]; the probe vector source.
EMB_ABS=$(tv token_embd.weight 3); EMB_IDX=$(tv token_embd.weight 5); EMB_INNER=$(tv token_embd.weight 6); EMB_HOLDS=$(tv token_embd.weight 7)
ROW_OFF=$(( TOKEN * N_EMBD * 2 ))
# attn_norm — F32 [4096]; the input RMSNorm weight.
NORM_ABS=$(tv blk.0.attn_norm.weight 3); NORM_IDX=$(tv blk.0.attn_norm.weight 5); NORM_INNER=$(tv blk.0.attn_norm.weight 6); NORM_HOLDS=$(tv blk.0.attn_norm.weight 7)
# attn_q_a — MXFP8 [in=4096, out=1024]; Q down-projection. rows=out=d1, cols=in=d0, nel=rows*cols.
QA_ABS=$(tv blk.0.attn_q_a.weight 3); QA_IDX=$(tv blk.0.attn_q_a.weight 5); QA_INNER=$(tv blk.0.attn_q_a.weight 6); QA_HOLDS=$(tv blk.0.attn_q_a.weight 7)
QA_IN=$(trow blk.0.attn_q_a.weight 5); QA_OUT=$(trow blk.0.attn_q_a.weight 6)
# attn_q_a_norm — F32 [1024].
QAN_ABS=$(tv blk.0.attn_q_a_norm.weight 3); QAN_IDX=$(tv blk.0.attn_q_a_norm.weight 5); QAN_INNER=$(tv blk.0.attn_q_a_norm.weight 6); QAN_HOLDS=$(tv blk.0.attn_q_a_norm.weight 7)
# attn_q_b — MXFP8 [in=1024, out=32768=n_head*head_dim]; Q up-projection.
QB_ABS=$(tv blk.0.attn_q_b.weight 3); QB_IDX=$(tv blk.0.attn_q_b.weight 5); QB_INNER=$(tv blk.0.attn_q_b.weight 6); QB_HOLDS=$(tv blk.0.attn_q_b.weight 7)
QB_IN=$(trow blk.0.attn_q_b.weight 5); QB_OUT=$(trow blk.0.attn_q_b.weight 6)
# attn_kv — MXFP8 [in=4096, out=512]; KV down-projection (the single latent).
KV_ABS=$(tv blk.0.attn_kv.weight 3); KV_IDX=$(tv blk.0.attn_kv.weight 5); KV_INNER=$(tv blk.0.attn_kv.weight 6); KV_HOLDS=$(tv blk.0.attn_kv.weight 7)
KV_IN=$(trow blk.0.attn_kv.weight 5); KV_OUT=$(trow blk.0.attn_kv.weight 6)
# attn_kv_a_norm — F32 [512].
KVAN_ABS=$(tv blk.0.attn_kv_a_norm.weight 3); KVAN_IDX=$(tv blk.0.attn_kv_a_norm.weight 5); KVAN_INNER=$(tv blk.0.attn_kv_a_norm.weight 6); KVAN_HOLDS=$(tv blk.0.attn_kv_a_norm.weight 7)
# attn_sinks — F32 [64]; the per-head learned sink logit (softmax DENOMINATOR only).
SNK_ABS=$(tv blk.0.attn_sinks.weight 3); SNK_IDX=$(tv blk.0.attn_sinks.weight 5); SNK_INNER=$(tv blk.0.attn_sinks.weight 6); SNK_HOLDS=$(tv blk.0.attn_sinks.weight 7)
# attn_output_a / attn_output_b — MXFP8; the GROUPED output path (Stage 1's resolution from the file).
OA_ABS=$(tv blk.0.attn_output_a.weight 3); OA_IDX=$(tv blk.0.attn_output_a.weight 5); OA_INNER=$(tv blk.0.attn_output_a.weight 6); OA_HOLDS=$(tv blk.0.attn_output_a.weight 7)
OA_IN=$(trow blk.0.attn_output_a.weight 5); OA_OUT=$(trow blk.0.attn_output_a.weight 6)
OB_ABS=$(tv blk.0.attn_output_b.weight 3); OB_IDX=$(tv blk.0.attn_output_b.weight 5); OB_INNER=$(tv blk.0.attn_output_b.weight 6); OB_HOLDS=$(tv blk.0.attn_output_b.weight 7)
OB_IN=$(trow blk.0.attn_output_b.weight 5); OB_OUT=$(trow blk.0.attn_output_b.weight 6)
# the HYPER-CONNECTION frame (Stage 4): hc_attn_fn F16 [16384 -> 24], hc_attn_scale [3], hc_attn_base [24].
HF_ABS=$(tv blk.0.hc_attn_fn.weight 3); HF_IDX=$(tv blk.0.hc_attn_fn.weight 5); HF_INNER=$(tv blk.0.hc_attn_fn.weight 6); HF_HOLDS=$(tv blk.0.hc_attn_fn.weight 7)
HF_IN=$(trow blk.0.hc_attn_fn.weight 5); HF_OUT=$(trow blk.0.hc_attn_fn.weight 6)
HS_ABS=$(tv blk.0.hc_attn_scale.weight 3); HS_IDX=$(tv blk.0.hc_attn_scale.weight 5); HS_INNER=$(tv blk.0.hc_attn_scale.weight 6); HS_HOLDS=$(tv blk.0.hc_attn_scale.weight 7)
HB_ABS=$(tv blk.0.hc_attn_base.weight 3); HB_IDX=$(tv blk.0.hc_attn_base.weight 5); HB_INNER=$(tv blk.0.hc_attn_base.weight 6); HB_HOLDS=$(tv blk.0.hc_attn_base.weight 7)
N_HC=4; HC_ITERS=20; HC_EPS=0.0000009999999975
RMS_EPS=0.0000009999999975
N_HEAD=64; HEAD_DIM=512; N_ROT=64; ROPE_BASE=10000.0; N_GROUPS=8; O_RANK=1024
POS_A=0; POS_B=7      # hushfold (row 859): RoPE is IDENTITY at pos 0 — one position cannot witness it.

# ── 2b. THE RENTED ORACLE (twinblind, row 868). The attention core is a set of CHOICES, so a self-carve
# is blind to it. form-stdlib/tests/dsv4-mla-core-oracle.py is an independent fp64 transcription of ds4.c's
# control flow — it parses this same GGUF itself and shares no code with the band, the MSL or the carrier.
ORACLE="$ROOT/form-stdlib/tests/dsv4-mla-core-oracle.py"
[[ -f "$ORACLE" ]] || { echo "FAIL the rented oracle is missing: $ORACLE"; exit 1; }
for P in "$POS_A" "$POS_B"; do
    mkdir -p "$work/ora$P"
    echo "  renting the oracle at pos=$P (independent fp64 transcription of ds4.c)..."
    DSV4_ORACLE_OUT="$work/ora$P" python3 "$ORACLE" "$BLOB" "$TOKEN" "$P" > "$work/ora$P.txt" 2>"$work/ora$P.err" \
        || { echo "FAIL oracle run at pos=$P"; tail -5 "$work/ora$P.err"; exit 1; }
    grep -qx 'END' "$work/ora$P.txt" || { echo "FAIL oracle stream truncated at pos=$P"; exit 1; }
done
awk '/^OUTPATH/{print "  oracle reads the file: "$0}' "$work/ora$POS_A.txt"
# hushfold, witnessed in the ORACLE's own output before any GPU runs: at pos 0 the RoPE is the identity.
if cmp -s "$work/ora$POS_A/oracle-q_headrms.f64" "$work/ora$POS_A/oracle-q.f64"; then
    echo "  hushfold: at pos $POS_A the oracle's post-RoPE q is BIT-IDENTICAL to its pre-RoPE q — one position cannot witness RoPE"
else
    echo "FAIL hushfold: the oracle's RoPE is not the identity at pos $POS_A"; exit 1
fi
if cmp -s "$work/ora$POS_B/oracle-q_headrms.f64" "$work/ora$POS_B/oracle-q.f64"; then
    echo "FAIL the oracle's RoPE is a no-op at pos $POS_B too — the second position witnesses nothing"; exit 1
fi
echo "  hushfold: at pos $POS_B it is NOT — so pos $POS_A and pos $POS_B together do witness it"
# STAGE 4 — the same oracle in `hc` mode: the MLA's input is no longer a probe but the REAL layer-0 input,
# the embedding broadcast to n_hc streams (ds4.c:9764) and collapsed by hc_pre (ds4.c:9690).
mkdir -p "$work/orahc"
echo "  renting the oracle in HC mode at pos=$POS_A (the complete attention half of a real layer)..."
DSV4_ORACLE_MODE=hc DSV4_ORACLE_OUT="$work/orahc" python3 "$ORACLE" "$BLOB" "$TOKEN" "$POS_A" > "$work/orahc.txt" 2>"$work/orahc.err" \
    || { echo "FAIL oracle hc-mode run"; tail -5 "$work/orahc.err"; exit 1; }
grep -qx 'END' "$work/orahc.txt" || { echo "FAIL oracle hc stream truncated"; exit 1; }
echo "  attn_norm: abs=$NORM_ABS view=$NORM_IDX inner=$NORM_INNER holds=$NORM_HOLDS"
echo "  attn_q_a (MXFP8 $QA_IN->$QA_OUT): abs=$QA_ABS view=$QA_IDX inner=$QA_INNER holds=$QA_HOLDS"
echo "  attn_kv  (MXFP8 $KV_IN->$KV_OUT): abs=$KV_ABS view=$KV_IDX inner=$KV_INNER holds=$KV_HOLDS"

# ── 3. compile the three translation units (embed, MLA norms, MXFP8 matvec), cached by sha ────────────
compile_unit() { # $1 emit-form  $2 grep-token  $3 cache-prefix -> echoes LIB path
    local form="$1" tok="$2" pre="$3" out lib sha
    echo "($form)" > "$work/$pre.fk"
    "$GO_BIN" "${FILES[@]}" "$work/$pre.fk" > "$work/$pre.out" 2>"$work/$pre.err" || { echo "FAIL $pre MSL emission" >&2; cat "$work/$pre.err" >&2; return 1; }
    awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/$pre.out" > "$work/$pre.metal"
    grep -q "$tok" "$work/$pre.metal" || { echo "FAIL $pre kernel $tok not emitted" >&2; return 1; }
    sha="$(shasum -a 256 "$work/$pre.metal" | cut -c1-16)"; lib="$CACHE/$pre-$sha.metallib"
    if [[ ! -f "$lib" ]]; then
        xcrun -sdk macosx metal -O2 -std=metal3.0 -c "$work/$pre.metal" -o "$work/$pre.air" 2>"$work/$pre.merr" \
          && xcrun -sdk macosx metallib "$work/$pre.air" -o "$lib" 2>>"$work/$pre.merr" || { echo "FAIL $pre metal compile" >&2; cat "$work/$pre.merr" >&2; return 1; }
        echo "PASS  $pre metallib compiled: $(basename "$lib")" >&2
    else
        echo "PASS  $pre metallib cache HIT: $(basename "$lib")" >&2
    fi
    printf '%s\n' "$lib"
}
mkdir -p "$CACHE"
LIB_EMB="$(compile_unit dsv4-embed-msl form_dsv4_embed_f16 dsv4emb)" || exit 1
LIB_MLA="$(compile_unit dsv4-mla-unit form_mla_rmsnorm_f32 dsv4mla)" || exit 1
LIB8="$(compile_unit dsv4-mx8-matvec-msl form_dsv4_mx8_matvec dsv4mx8)" || exit 1
LIB_CORE="$(compile_unit dsv4-mla-core-msl form_dsv4_mx8_matvec_grouped dsv4core)" || exit 1
LIB_HC="$(compile_unit dsv4-hc-unit form_hc_split_f32 dsv4hc)" || exit 1

# ── 4. the carrier ────────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

let a = CommandLine.arguments
var ai = 1
func S() -> String { let v = a[ai]; ai += 1; return v }
func I() -> Int { let v = Int(a[ai])!; ai += 1; return v }
func F() -> Float { let v = Float(a[ai])!; ai += 1; return v }
let libEmb = S(), libMla = S(), lib8 = S(), libCore = S(), libHc = S(), blobPath = S()
let step = I(), viewLimit = I(), nviews = I()
let embAbs = I(), rowOff = I(), nEmbd = I(), embIdx = I(), embInner = I(), embHolds = I(), token = I()
let normAbs = I(), normIdx = I(), normInner = I(), normHolds = I()
let qaAbs = I(), qaIdx = I(), qaInner = I(), qaHolds = I(), qaRows = I(), qaCols = I()
let qanAbs = I(), qanIdx = I(), qanInner = I(), qanHolds = I()
let qbAbs = I(), qbIdx = I(), qbInner = I(), qbHolds = I(), qbRows = I(), qbCols = I()
let kvAbs = I(), kvIdx = I(), kvInner = I(), kvHolds = I(), kvRows = I(), kvCols = I()
let kvanAbs = I(), kvanIdx = I(), kvanInner = I(), kvanHolds = I()
let eps = F()
let snkAbs = I(), snkIdx = I(), snkInner = I(), snkHolds = I()
let oaAbs = I(), oaIdx = I(), oaInner = I(), oaHolds = I(), oaRows = I(), oaCols = I()
let obAbs = I(), obIdx = I(), obIdxInner = I(), obHolds = I(), obRows = I(), obCols = I()
let nHead = I(), headDim = I(), nRot = I(), ropeBase = F(), nGroups = I(), oRank = I()
let posA = I(), posB = I(), oraDirA = S(), oraDirB = S()
let hfAbs = I(), hfIdx = I(), hfInner = I(), hfHolds = I(), hfRows = I(), hfCols = I()
let hsAbs = I(), hsIdx = I(), hsInner = I(), hsHolds = I()
let hbAbs = I(), hbIdx = I(), hbInner = I(), hbHolds = I()
let nHc = I(), hcIters = I(), hcEps = F(), oraDirHc = S()

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
let lEmb = try dev.makeLibrary(URL: URL(fileURLWithPath: libEmb))
let lMla = try dev.makeLibrary(URL: URL(fileURLWithPath: libMla))
let l8   = try dev.makeLibrary(URL: URL(fileURLWithPath: lib8))
let lCore = try dev.makeLibrary(URL: URL(fileURLWithPath: libCore))
let lHc = try dev.makeLibrary(URL: URL(fileURLWithPath: libHc))
let queue = dev.makeCommandQueue()!
var failures = 0, gpuErrors = 0
var gpuFirstError: String? = nil
func check(_ ok: Bool, _ pass: String, _ fail: String) { if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 } }

func f16_to_f32(_ h: UInt16) -> Float {
    let sign = UInt32(h & 0x8000) << 16
    let exp  = Int((h >> 10) & 0x1F)
    let mant = UInt32(h & 0x3FF)
    if exp == 0 {
        if mant == 0 { return Float(bitPattern: sign) }
        var m = mant, e = -1
        repeat { m <<= 1; e += 1 } while (m & 0x400) == 0
        m &= 0x3FF
        return Float(bitPattern: sign | UInt32(127 - 15 - e) << 23 | (m << 13))
    } else if exp == 0x1F {
        return Float(bitPattern: sign | 0x7F800000 | (mant << 13))
    }
    return Float(bitPattern: sign | UInt32(exp - 15 + 127) << 23 | (mant << 13))
}
// MXFP8 (E4M3 payload, E8M0 scale) — the carrier's independent decode, textually mxfp8-msl.fk's.
func mxm_pow2(_ e: Int) -> Float {
    var aa: Float = 1.0, k = e
    while k >= 8 { aa *= 256.0; k -= 8 }; while k > 0 { aa *= 2.0; k -= 1 }
    while k <= -8 { aa *= 0.00390625; k += 8 }; while k < 0 { aa *= 0.5; k += 1 }
    return aa
}
func mxm_e8m0(_ e: Int) -> Float { return mxm_pow2(e - 127) }
func mx8_val(_ b: Int) -> Float {
    let mant = b % 8, ex = (b / 8) % 16, sgn = b / 128
    let frac = Float(mant) / 8.0
    let mag = (ex == 0) ? (mxm_pow2(-6) * frac) : (mxm_pow2(ex - 7) * (1.0 + frac))
    return (sgn == 1) ? -mag : mag
}

let fd = open(blobPath, O_RDONLY)
guard fd >= 0 else { print("FAIL cannot open blob"); exit(1) }
var st = stat(); fstat(fd, &st)
let fileLen = Int(st.st_size); let page = Int(getpagesize())
let mapLen = (fileLen + page - 1) / page * page
guard let mapped0 = mmap(nil, mapLen, PROT_READ, MAP_PRIVATE, fd, 0), mapped0 != MAP_FAILED else { print("FAIL mmap failed"); exit(1) }
let base = mapped0.assumingMemoryBound(to: UInt8.self)

// endian/alignment-safe F32 tensor read from the mmap (the RMSNorm weight g).
func readF32(_ off: Int, _ cnt: Int) -> [Float] {
    var out = [Float](repeating: 0, count: cnt)
    for i in 0..<cnt {
        let p = off + i*4
        let u = UInt32(base[p]) | (UInt32(base[p+1])<<8) | (UInt32(base[p+2])<<16) | (UInt32(base[p+3])<<24)
        out[i] = Float(bitPattern: u)
    }
    return out
}
// the CPU reference RMSNorm (ln-rmsnorm): sumsq LEFT fold, out = (x*1/rms)*g.
func rmsnorm(_ x: [Float], _ g: [Float]) -> [Float] {
    var ss: Float = 0; for v in x { ss += v*v }
    let inv = 1.0 / sqrtf(ss/Float(x.count) + eps)
    var o = [Float](repeating: 0, count: x.count)
    for i in 0..<x.count { o[i] = (x[i]*inv)*g[i] }
    return o
}
// the CPU reference MXFP8 fused matvec at a tensor's ABSOLUTE offset; rows=out, cols=in, x is cols-wide.
func mx8matvec(_ absOff: Int, _ x: [Float], _ rows: Int, _ cols: Int) -> [Float] {
    let qb = base.advanced(by: absOff); let nel = rows*cols
    var y = [Float](repeating: 0, count: rows)
    for r in 0..<rows {
        var acc: Float = 0; let rowPay = r*cols; let g0 = rowPay/32; var g = 0
        while g < cols/32 {
            let s = mxm_e8m0(Int(qb[nel + g0 + g])); var a2: Float = 0; let pbase = rowPay + g*32
            for m in 0..<32 { a2 += x[g*32 + m] * mx8_val(Int(qb[pbase + m])) }
            acc += s*a2; g += 1
        }
        y[r] = acc
    }
    return y
}
// compare a GPU buffer to a CPU reference: (ok, maxAbs, maxRel, nan, distinct, min, max).
// The weight decode (MXFP8 E4M3 * E8M0, or F32 g) is bit-exact; a matvec / Newton-sqrt reassociates, so
// the honest bound is float precision: an ABSOLUTE bound everywhere (catches every element), and a
// RELATIVE bound taken only over entries with |cpu| > 1e-3 (below that, an 8e-8 abs diff reads as a large
// relative purely because the denominator is ~0 — a near-zero-denominator artifact, not an error).
func cmp(_ gpu: UnsafeMutablePointer<Float>, _ cpu: [Float]) -> (Bool, Float, Float, Int, Int, Float, Float) {
    var maxAbs: Float = 0, maxRel: Float = 0, nan = 0
    var seen = Set<UInt32>(); var vmin = Float.greatestFiniteMagnitude, vmax = -Float.greatestFiniteMagnitude
    for i in 0..<cpu.count {
        let g = gpu[i]
        if g.isNaN || !g.isFinite { nan += 1; continue }
        let d = abs(g - cpu[i])
        if d > maxAbs { maxAbs = d }
        if abs(cpu[i]) > 1e-3 { let r = d/abs(cpu[i]); if r > maxRel { maxRel = r } }
        seen.insert(g.bitPattern); vmin = min(vmin, g); vmax = max(vmax, g)
    }
    let nonDegen = seen.count > 8 && vmax > vmin
    return (nan == 0 && maxRel < 1e-3 && maxAbs < 1e-4 && nonDegen, maxAbs, maxRel, nan, seen.count, vmin, vmax)
}

// ── GATE 0: the views map. One buffer over the whole file FAILs (onelean); build the overlapping set.
var views: [MTLBuffer] = []
for i in 0..<nviews {
    let vs = i*step; let vlen = min(viewLimit, mapLen - vs)
    guard vs % page == 0 else { print("FAIL view \(i) start not page-aligned"); exit(1) }
    guard let buf = dev.makeBuffer(bytesNoCopy: mapped0.advanced(by: vs), length: vlen, options: .storageModeShared, deallocator: nil) else {
        print("FAIL view \(i) makeBuffer failed"); failures += 1; break
    }
    views.append(buf)
}
check(views.count == nviews,
  "gate 0 the views map: all \(nviews) overlapping page-aligned bytesNoCopy views of the \(fileLen) B file wrap on \(dev.name) (one buffer over the whole file FAILs; \(nviews) views do not)",
  "gate 0 only \(views.count)/\(nviews) views mapped")
if failures > 0 { print("VERDICT FAIL the views did not map"); exit(1) }

// ── GATE 1: the five MLA tensors are each resident in a single view (holds==1).
let resident = embHolds==1 && normHolds==1 && qaHolds==1 && qanHolds==1 && qbHolds==1 && kvHolds==1 && kvanHolds==1
  && snkHolds==1 && oaHolds==1 && obHolds==1
  && embIdx<nviews && normIdx<nviews && qaIdx<nviews && qanIdx<nviews && qbIdx<nviews && kvIdx<nviews && kvanIdx<nviews
  && snkIdx<nviews && oaIdx<nviews && obIdx<nviews
check(resident,
  "gate 1 residency: token_embd(v\(embIdx)), attn_norm(v\(normIdx)), attn_q_a(v\(qaIdx)), attn_q_a_norm(v\(qanIdx)), attn_q_b(v\(qbIdx)), attn_kv(v\(kvIdx)), attn_kv_a_norm(v\(kvanIdx)), attn_sinks(v\(snkIdx)), attn_output_a(v\(oaIdx)), attn_output_b(v\(obIdx)) each lie wholly inside one view",
  "gate 1 an MLA tensor spans views (holds: emb\(embHolds) norm\(normHolds) qa\(qaHolds) qan\(qanHolds) kv\(kvHolds) kvan\(kvanHolds) snk\(snkHolds) oa\(oaHolds) ob\(obHolds))")
if failures > 0 { print("VERDICT FAIL"); exit(1) }

// helper: dispatch the one-thread RMSNorm kernel; x is a device buffer, g bound from a view at gOff.
let pRms = try dev.makeComputePipelineState(function: lMla.makeFunction(name: "form_mla_rmsnorm_f32")!)
func gpuRmsnorm(_ x: MTLBuffer, _ n: Int, _ gView: MTLBuffer, _ gOff: Int) -> MTLBuffer {
    let out = dev.makeBuffer(length: n*4, options: .storageModeShared)!
    let op = out.contents().bindMemory(to: Float.self, capacity: n)
    for i in 0..<n { op[i] = Float.nan }               // SENTINEL
    var n32 = UInt32(n), e = eps
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(pRms)
    enc.setBuffer(x, offset: 0, index: 0)
    enc.setBuffer(gView, offset: gOff, index: 1)
    enc.setBuffer(out, offset: 0, index: 2)
    enc.setBytes(&n32, length: 4, index: 3); enc.setBytes(&e, length: 4, index: 4)
    enc.dispatchThreads(MTLSize(width: 1, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: 1, height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let er = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(er)" } }
    if cb.status != .completed { gpuErrors += 1 }
    return out
}
// helper: dispatch the MXFP8 fused matvec; qb bound from a view at qbOff, x device buffer, -> y[rows].
let p8 = try dev.makeComputePipelineState(function: l8.makeFunction(name: "form_dsv4_mx8_matvec")!)
func gpuMx8(_ qbView: MTLBuffer, _ qbOff: Int, _ x: MTLBuffer, _ rows: Int, _ cols: Int) -> MTLBuffer {
    let out = dev.makeBuffer(length: rows*4, options: .storageModeShared)!
    let op = out.contents().bindMemory(to: Float.self, capacity: rows)
    for i in 0..<rows { op[i] = Float.nan }            // SENTINEL
    var r32 = UInt32(rows), c32 = UInt32(cols), nel32 = UInt32(rows*cols)
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(p8)
    enc.setBuffer(qbView, offset: qbOff, index: 0)
    enc.setBuffer(x, offset: 0, index: 1)
    enc.setBuffer(out, offset: 0, index: 2)
    enc.setBytes(&r32, length: 4, index: 3); enc.setBytes(&c32, length: 4, index: 4); enc.setBytes(&nel32, length: 4, index: 5)
    let tg = min(p8.maxTotalThreadsPerThreadgroup, 256)
    enc.dispatchThreads(MTLSize(width: rows*32, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let er = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(er)" } }
    if cb.status != .completed { gpuErrors += 1 }
    return out
}

// ── the probe vector x0: the token's real F16 embedding, GPU-decoded and CPU-carved (both independent).
let pEmb = try dev.makeComputePipelineState(function: lEmb.makeFunction(name: "form_dsv4_embed_f16")!)
let x0 = dev.makeBuffer(length: nEmbd*4, options: .storageModeShared)!
let x0p = x0.contents().bindMemory(to: Float.self, capacity: nEmbd)
for i in 0..<nEmbd { x0p[i] = Float(bitPattern: 0x7F7FFFFF) }
var b64 = UInt64(embInner + rowOff), c32 = UInt32(nEmbd)
do {
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(pEmb)
    enc.setBuffer(views[embIdx], offset: 0, index: 0); enc.setBuffer(x0, offset: 0, index: 1)
    enc.setBytes(&b64, length: 8, index: 2); enc.setBytes(&c32, length: 4, index: 3)
    let tg = min(pEmb.maxTotalThreadsPerThreadgroup, 256)
    enc.dispatchThreads(MTLSize(width: nEmbd, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: tg, height: 1, depth: 1))
    enc.endEncoding(); cb.commit(); cb.waitUntilCompleted()
    if let er = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(er)" } }
}
let rowAbs = embAbs + rowOff
var x0cpu = [Float](repeating: 0, count: nEmbd)
for i in 0..<nEmbd { x0cpu[i] = f16_to_f32(UInt16(base[rowAbs+i*2]) | (UInt16(base[rowAbs+i*2+1])<<8)) }

// ── GATE 2: the INPUT RMSNorm (attn_norm) at real dims through view \(normIdx).
let xnBuf = gpuRmsnorm(x0, nEmbd, views[normIdx], normInner)
let xnp = xnBuf.contents().bindMemory(to: Float.self, capacity: nEmbd)
let gNorm = readF32(normAbs, nEmbd)
let xnCpu = rmsnorm(x0cpu, gNorm)
let (ok2, ma2, mr2, nn2, ds2, mn2, mx2) = cmp(xnp, xnCpu)
check(ok2 && gpuErrors == 0,
  "gate 2 input RMSNorm at real dims: form_mla_rmsnorm_f32 over the \(nEmbd)-wide probe with attn_norm's F32 g read through view \(normIdx) agrees with the CPU carve (maxRel \(mr2), maxAbs \(ma2); \(ds2) distinct, range [\(mn2),\(mx2)]; \(nn2) NaN)",
  "gate 2 input RMSNorm: maxRel \(mr2) maxAbs \(ma2) nan \(nn2) distinct \(ds2) gpuErrors \(gpuErrors)")

// ── GATE 3: the Q down-projection (attn_q_a, MXFP8 4096->1024) fused decode+matvec through view \(qaIdx).
let qLatBuf = gpuMx8(views[qaIdx], qaInner, xnBuf, qaRows, qaCols)
let qLatp = qLatBuf.contents().bindMemory(to: Float.self, capacity: qaRows)
let qLatCpu = mx8matvec(qaAbs, xnCpu, qaRows, qaCols)
let (ok3, ma3, mr3, nn3, ds3, mn3, mx3) = cmp(qLatp, qLatCpu)
check(ok3 && gpuErrors == 0,
  "gate 3 Q down-projection at real dims: the type-41 attn_q_a (\(qaRows)x\(qaCols)) fused decode+matvec through view \(qaIdx) agrees with the CPU MXFP8 carve on all \(qaRows) latents to float precision (maxRel \(mr3), maxAbs \(ma3); \(ds3) distinct, range [\(mn3),\(mx3)]; \(nn3) NaN)",
  "gate 3 Q down: maxRel \(mr3) maxAbs \(ma3) nan \(nn3) distinct \(ds3) gpuErrors \(gpuErrors)")

// ── GATE 4: the Q rank-space RMSNorm (attn_q_a_norm, 1024) through view \(qanIdx).
let qLatNBuf = gpuRmsnorm(qLatBuf, qaRows, views[qanIdx], qanInner)
let qLatNp = qLatNBuf.contents().bindMemory(to: Float.self, capacity: qaRows)
let gQan = readF32(qanAbs, qaRows)
let qLatNCpu = rmsnorm(qLatCpu, gQan)
let (ok4, ma4, mr4, nn4, ds4, _, _) = cmp(qLatNp, qLatNCpu)
check(ok4 && gpuErrors == 0,
  "gate 4 Q rank-space RMSNorm at real dims: form_mla_rmsnorm_f32 over the \(qaRows)-wide Q latent with attn_q_a_norm's g through view \(qanIdx) agrees with the CPU carve (maxRel \(mr4), maxAbs \(ma4); \(ds4) distinct; \(nn4) NaN)",
  "gate 4 Q rank-norm: maxRel \(mr4) maxAbs \(ma4) nan \(nn4)")

// ── GATE 7: the Q up-projection (attn_q_b, MXFP8 1024->32768 = n_head*head_dim) through view \(qbIdx).
// This completes the MLA PROJECTION surface: q = q_b · q_a_norm(q_a·xn), the per-head query stack.
let qBuf = gpuMx8(views[qbIdx], qbInner, qLatNBuf, qbRows, qbCols)
let qp = qBuf.contents().bindMemory(to: Float.self, capacity: qbRows)
let qCpu = mx8matvec(qbAbs, qLatNCpu, qbRows, qbCols)
let (ok7, ma7, mr7, nn7, ds7, mn7, mx7) = cmp(qp, qCpu)
check(ok7 && gpuErrors == 0,
  "gate 7 Q up-projection at real dims: the type-41 attn_q_b (\(qbRows)x\(qbCols)) fused decode+matvec through view \(qbIdx) agrees with the CPU MXFP8 carve on all \(qbRows) query elements (= n_head 64 * head_dim 512) to float precision (maxRel \(mr7), maxAbs \(ma7); \(ds7) distinct, range [\(mn7),\(mx7)]; \(nn7) NaN)",
  "gate 7 Q up: maxRel \(mr7) maxAbs \(ma7) nan \(nn7) distinct \(ds7) gpuErrors \(gpuErrors)")

// ── GATE 5: the KV down-projection (attn_kv, MXFP8 4096->512) fused decode+matvec through view \(kvIdx).
let kvLatBuf = gpuMx8(views[kvIdx], kvInner, xnBuf, kvRows, kvCols)
let kvLatp = kvLatBuf.contents().bindMemory(to: Float.self, capacity: kvRows)
let kvLatCpu = mx8matvec(kvAbs, xnCpu, kvRows, kvCols)
let (ok5, ma5, mr5, nn5, ds5, mn5, mx5) = cmp(kvLatp, kvLatCpu)
check(ok5 && gpuErrors == 0,
  "gate 5 KV down-projection at real dims: the type-41 attn_kv (\(kvRows)x\(kvCols)) fused decode+matvec through view \(kvIdx) agrees with the CPU MXFP8 carve on all \(kvRows) latents (maxRel \(mr5), maxAbs \(ma5); \(ds5) distinct, range [\(mn5),\(mx5)]; \(nn5) NaN)",
  "gate 5 KV down: maxRel \(mr5) maxAbs \(ma5) nan \(nn5) distinct \(ds5) gpuErrors \(gpuErrors)")

// ── GATE 6: the KV rank-space RMSNorm (attn_kv_a_norm, 512) through view \(kvanIdx).
let kvLatNBuf = gpuRmsnorm(kvLatBuf, kvRows, views[kvanIdx], kvanInner)
let kvLatNp = kvLatNBuf.contents().bindMemory(to: Float.self, capacity: kvRows)
let gKvan = readF32(kvanAbs, kvRows)
let kvLatNCpu = rmsnorm(kvLatCpu, gKvan)
let (ok6, ma6, mr6, nn6, ds6, _, _) = cmp(kvLatNp, kvLatNCpu)
check(ok6 && gpuErrors == 0,
  "gate 6 KV rank-space RMSNorm at real dims: form_mla_rmsnorm_f32 over the \(kvRows)-wide KV latent with attn_kv_a_norm's g through view \(kvanIdx) agrees with the CPU carve (maxRel \(mr6), maxAbs \(ma6); \(ds6) distinct; \(nn6) NaN)",
  "gate 6 KV rank-norm: maxRel \(mr6) maxAbs \(ma6) nan \(nn6)")

// ══════════════════════════════════════════════════════════════════════════════════════════════════
// STONE 36 — THE ATTENTION CORE, the CHOOSING half, proven against the RENTED ORACLE.
//
// Gates 2..7 above are the CANONICAL half: a matvec and an RMSNorm have one right answer, so a self-carve
// (GPU vs an independent CPU decode of the same bytes) is a real falsifier there. From here on it is not.
// Where the sink enters the softmax, whether the KV row is fp8+f16 rounded before it is attended to,
// whether the heads are UN-roped after attending, which output path is taken — all CHOICES, and a
// self-carve inherits the choice on both sides (twinblind, row 868). So every gate below compares the GPU
// against dsv4-mla-core-oracle.py: an INDEPENDENT fp64 transcription of ds4.c, written from the C.
// ══════════════════════════════════════════════════════════════════════════════════════════════════

func readOracle(_ dir: String, _ key: String) -> [Double] {
    let p = dir + "/oracle-" + key + ".f64"
    guard let s = try? String(contentsOfFile: p, encoding: .utf8) else {
        print("FAIL oracle vector missing: \(p)"); exit(1)
    }
    var out: [Double] = []
    s.split(separator: "\n").forEach { if let v = Double($0) { out.append(v) } }
    return out
}
// The comparator (assocwall, row 866). The oracle is fp64 and the GPU is f32 over a real-width reduction,
// so bit-equality is not the question — summation ORDER is. The honest gate is an ABSOLUTE bound over
// every element PLUS a RELATIVE bound taken only above a magnitude floor: below the floor an ~1e-6 absolute
// difference reads as a huge relative purely because the denominator is ~0 (Stone 35's gate 7 went red at
// maxRel 0.008 with maxAbs 8.9e-8 — the comparator, not the arithmetic).
func cmpOra(_ gpu: UnsafeMutablePointer<Float>, _ ref: [Double], _ absBound: Double, _ relBound: Double,
            _ relFloor: Double) -> (Bool, Double, Double, Int, Int, Float, Float) {
    var maxAbs = 0.0, maxRel = 0.0, nan = 0
    var seen = Set<UInt32>(); var vmin = Float.greatestFiniteMagnitude, vmax = -Float.greatestFiniteMagnitude
    for i in 0..<ref.count {
        let g = gpu[i]
        if g.isNaN || !g.isFinite { nan += 1; continue }
        let d = abs(Double(g) - ref[i])
        if d > maxAbs { maxAbs = d }
        if abs(ref[i]) > relFloor { let r = d/abs(ref[i]); if r > maxRel { maxRel = r } }
        seen.insert(g.bitPattern); vmin = min(vmin, g); vmax = max(vmax, g)
    }
    // zerobirth/edgedrop: a dead view or an unrun kernel reads as a sentinel or as one repeated value.
    let nonDegen = seen.count > 8 && vmax > vmin
    return (nan == 0 && maxAbs < absBound && maxRel < relBound && nonDegen, maxAbs, maxRel, nan, seen.count, vmin, vmax)
}

let pHeadrms = try dev.makeComputePipelineState(function: lMla.makeFunction(name: "form_mla_headrms_f32")!)
let pRope    = try dev.makeComputePipelineState(function: lMla.makeFunction(name: "form_mla_rope_f32")!)
let pAttend  = try dev.makeComputePipelineState(function: lMla.makeFunction(name: "form_mla_attend_f32")!)
let pGrouped = try dev.makeComputePipelineState(function: lCore.makeFunction(name: "form_dsv4_mx8_matvec_grouped")!)
let pKvq     = try dev.makeComputePipelineState(function: lCore.makeFunction(name: "form_dsv4_kv_fp8_f16_round")!)

func sentinelled(_ n: Int) -> MTLBuffer {                 // the offered-interface guard, every dispatch
    let b = dev.makeBuffer(length: n*4, options: .storageModeShared)!
    let p = b.contents().bindMemory(to: Float.self, capacity: n)
    for i in 0..<n { p[i] = Float.nan }
    return b
}
func run(_ cb: MTLCommandBuffer) {
    cb.commit(); cb.waitUntilCompleted()
    if let er = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(er)" } }
    if cb.status != .completed { gpuErrors += 1 }
}
func gpuHeadrms(_ x: MTLBuffer, _ nh: Int, _ hd: Int) -> MTLBuffer {
    let out = sentinelled(nh*hd)
    var a = UInt32(nh), b = UInt32(hd), e = eps
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(pHeadrms)
    enc.setBuffer(x, offset: 0, index: 0); enc.setBuffer(out, offset: 0, index: 1)
    enc.setBytes(&a, length: 4, index: 2); enc.setBytes(&b, length: 4, index: 3); enc.setBytes(&e, length: 4, index: 4)
    enc.dispatchThreads(MTLSize(width: nh, height: 1, depth: 1),
                        threadsPerThreadgroup: MTLSize(width: min(pHeadrms.maxTotalThreadsPerThreadgroup, 64), height: 1, depth: 1))
    enc.endEncoding(); run(cb); return out
}
// ds4.c:10102 — freqs[k] = theta_scale^k, theta_scale = freq_base^(-2/n_rot), built by the SAME repeated
// multiply the C uses (theta_extrap *= theta_scale), so the accumulation order matches.
let nPair = nRot/2
let freqBuf = dev.makeBuffer(length: nPair*4, options: .storageModeShared)!
do {
    let fp = freqBuf.contents().bindMemory(to: Float.self, capacity: nPair)
    let thetaScale = powf(ropeBase, -2.0/Float(nRot))
    var f: Float = 1.0
    for k in 0..<nPair { fp[k] = f; f *= thetaScale }
}
func gpuRope(_ v: MTLBuffer, _ nh: Int, _ hd: Int, _ pos: Int, _ inverse: Bool) -> MTLBuffer {
    let out = sentinelled(nh*hd)
    var a = UInt32(nh), b = UInt32(hd), c = UInt32(nRot), p = Float(pos), s: Float = inverse ? -1.0 : 1.0
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(pRope)
    enc.setBuffer(v, offset: 0, index: 0); enc.setBuffer(out, offset: 0, index: 1)
    enc.setBuffer(freqBuf, offset: 0, index: 2)
    enc.setBytes(&a, length: 4, index: 3); enc.setBytes(&b, length: 4, index: 4); enc.setBytes(&c, length: 4, index: 5)
    enc.setBytes(&p, length: 4, index: 6); enc.setBytes(&s, length: 4, index: 7)
    enc.dispatchThreads(MTLSize(width: nh, height: 1, depth: 1),
                        threadsPerThreadgroup: MTLSize(width: min(pRope.maxTotalThreadsPerThreadgroup, 64), height: 1, depth: 1))
    enc.endEncoding(); run(cb); return out
}
func gpuKvRound(_ v: MTLBuffer, _ hd: Int) -> MTLBuffer {
    let out = sentinelled(hd)
    var a = UInt32(hd), b = UInt32(nRot)
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(pKvq)
    enc.setBuffer(v, offset: 0, index: 0); enc.setBuffer(out, offset: 0, index: 1)
    enc.setBytes(&a, length: 4, index: 2); enc.setBytes(&b, length: 4, index: 3)
    enc.dispatchThreads(MTLSize(width: 1, height: 1, depth: 1), threadsPerThreadgroup: MTLSize(width: 1, height: 1, depth: 1))
    enc.endEncoding(); run(cb); return out
}
func gpuAttend(_ q: MTLBuffer, _ rows: MTLBuffer, _ nh: Int, _ hd: Int, _ nrows: Int, _ sinkView: MTLBuffer, _ sinkOff: Int) -> MTLBuffer {
    let out = sentinelled(nh*hd)
    var a = UInt32(nh), b = UInt32(hd), c = UInt32(nrows), sc = 1.0/sqrtf(Float(hd))
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(pAttend)
    enc.setBuffer(q, offset: 0, index: 0); enc.setBuffer(rows, offset: 0, index: 1); enc.setBuffer(out, offset: 0, index: 2)
    enc.setBuffer(sinkView, offset: sinkOff, index: 3)
    enc.setBytes(&a, length: 4, index: 4); enc.setBytes(&b, length: 4, index: 5); enc.setBytes(&c, length: 4, index: 6)
    enc.setBytes(&sc, length: 4, index: 7)
    enc.dispatchThreads(MTLSize(width: nh, height: 1, depth: 1),
                        threadsPerThreadgroup: MTLSize(width: min(pAttend.maxTotalThreadsPerThreadgroup, 32), height: 1, depth: 1))
    enc.endEncoding(); run(cb); return out
}
func gpuGrouped(_ wView: MTLBuffer, _ wOff: Int, _ x: MTLBuffer, _ rows: Int, _ cols: Int, _ rank: Int) -> MTLBuffer {
    let out = sentinelled(rows)
    var r32 = UInt32(rows), c32 = UInt32(cols), nel32 = UInt32(rows*cols), rk = UInt32(rank)
    let cb = queue.makeCommandBuffer()!, enc = cb.makeComputeCommandEncoder()!
    enc.setComputePipelineState(pGrouped)
    enc.setBuffer(wView, offset: wOff, index: 0); enc.setBuffer(x, offset: 0, index: 1); enc.setBuffer(out, offset: 0, index: 2)
    enc.setBytes(&r32, length: 4, index: 3); enc.setBytes(&c32, length: 4, index: 4)
    enc.setBytes(&nel32, length: 4, index: 5); enc.setBytes(&rk, length: 4, index: 6)
    enc.dispatchThreads(MTLSize(width: rows*32, height: 1, depth: 1),
                        threadsPerThreadgroup: MTLSize(width: min(pGrouped.maxTotalThreadsPerThreadgroup, 256), height: 1, depth: 1))
    enc.endEncoding(); run(cb); return out
}

// ── GATE 8: the per-head RMSNorm (ds4.c head_rms_norm_inplace :6646) — the step the PROJECTION surface
// stopped one line short of. It is unweighted and per-head; nothing in the file names it, so only the C says
// it is there at all. Position-independent, so it is proven once.
let qHrBuf = gpuHeadrms(qBuf, nHead, headDim)
let qHrp = qHrBuf.contents().bindMemory(to: Float.self, capacity: nHead*headDim)
let oraQhr = readOracle(oraDirA, "q_headrms")
let (ok8, ma8, mr8, nn8, ds8, mn8, mx8v) = cmpOra(qHrp, oraQhr, 2e-4, 2e-4, 1e-2)
check(ok8 && gpuErrors == 0 && oraQhr.count == nHead*headDim,
  "gate 8 per-head RMSNorm at real dims [RENTED ORACLE]: form_mla_headrms_f32 over all \(nHead) heads x \(headDim) agrees with the fp64 ds4.c transcription (maxAbs \(ma8), maxRel \(mr8) above |1e-2|; \(ds8) distinct, range [\(mn8),\(mx8v)]; \(nn8) NaN)",
  "gate 8 headrms vs oracle: maxAbs \(ma8) maxRel \(mr8) nan \(nn8) distinct \(ds8) n \(oraQhr.count) gpuErrors \(gpuErrors)")

var coreFail = 0
var lastOut: [Float] = []
var outsByPos: [Int: [Float]] = [:]

// the sink view — F32 [64], read through its own window like every other weight.
func runCore(_ pos: Int, _ oraDir: String, _ gateBase: Int) {
    let oq   = readOracle(oraDir, "q")
    let okvr = readOracle(oraDir, "kv_roped")
    let okv  = readOracle(oraDir, "kv")
    let oha  = readOracle(oraDir, "heads_attn")
    let ohd  = readOracle(oraDir, "heads")
    let olow = readOracle(oraDir, "low")
    let oout = readOracle(oraDir, "attn_out")

    // ds4.c:13793 — RoPE forward on q, then on the kv latent row.
    let qRoped = gpuRope(qHrBuf, nHead, headDim, pos, false)
    let qRp = qRoped.contents().bindMemory(to: Float.self, capacity: nHead*headDim)
    let (a1, m1, r1, n1, d1, _, _) = cmpOra(qRp, oq, 3e-4, 3e-4, 1e-2)
    check(a1, "gate \(gateBase) RoPE(q) at pos \(pos) [RENTED ORACLE]: the trailing-\(nRot) rotation over all \(nHead) heads (leading \(headDim - nRot) untouched) agrees with the fp64 ds4.c transcription (maxAbs \(m1), maxRel \(r1); \(d1) distinct; \(n1) NaN)",
      "gate \(gateBase) RoPE(q) pos \(pos): maxAbs \(m1) maxRel \(r1) nan \(n1)")
    if !a1 { coreFail += 1 }

    let kvRoped = gpuRope(kvLatNBuf, 1, headDim, pos, false)
    let kvRp = kvRoped.contents().bindMemory(to: Float.self, capacity: headDim)
    let (a2, m2, r2, n2, d2, _, _) = cmpOra(kvRp, okvr, 3e-5, 3e-5, 1e-2)
    check(a2, "gate \(gateBase+1) RoPE(kv latent) at pos \(pos) [RENTED ORACLE]: head_count_kv is 1, so the single \(headDim)-wide latent rotates once (maxAbs \(m2), maxRel \(r2); \(d2) distinct; \(n2) NaN)",
      "gate \(gateBase+1) RoPE(kv) pos \(pos): maxAbs \(m2) maxRel \(r2) nan \(n2)")
    if !a2 { coreFail += 1 }

    // ds4.c:3211 + :3162 — the kv row is fp8-round-tripped on its NOPE part, then f16-rounded whole. This
    // is IN the forward path: skipping it is wrong by ~1e-2, not by float precision.
    let kvR = gpuKvRound(kvRoped, headDim)
    let kvRp2 = kvR.contents().bindMemory(to: Float.self, capacity: headDim)
    let (a3, m3, r3, n3, d3, _, _) = cmpOra(kvRp2, okv, 1e-5, 1e-5, 1e-2)
    // the round-trip must actually MOVE the row — otherwise the kernel is a memcpy that agrees by accident
    var moved = 0
    for i in 0..<headDim { if kvRp2[i] != kvRp[i] { moved += 1 } }
    check(a3 && moved > headDim/4,
      "gate \(gateBase+2) the KV row's fp8+f16 round-trip at pos \(pos) [RENTED ORACLE]: E4M3FN in \(64)-wide groups over the leading \(headDim - nRot) (the roped tail is NOT fp8-rounded) then f16 over all \(headDim) — agrees with the fp64 ds4.c transcription and MOVED \(moved)/\(headDim) entries (maxAbs \(m3), maxRel \(r3); \(d3) distinct; \(n3) NaN)",
      "gate \(gateBase+2) kv round-trip pos \(pos): maxAbs \(m3) maxRel \(r3) nan \(n3) moved \(moved)")
    if !(a3 && moved > headDim/4) { coreFail += 1 }

    // ds4.c:10305 — the sink-aware softmax. The learned per-head sink logit is in the DENOMINATOR and
    // contributes no value vector, so each row's weights sum to LESS than one.
    let headsA = gpuAttend(qRoped, kvR, nHead, headDim, 1, views[snkIdx], snkInner)
    let hap = headsA.contents().bindMemory(to: Float.self, capacity: nHead*headDim)
    let (a4, m4, r4, n4, d4, _, _) = cmpOra(hap, oha, 3e-4, 3e-4, 1e-2)
    check(a4, "gate \(gateBase+3) the sink softmax at pos \(pos) [RENTED ORACLE]: form_mla_attend_f32 over \(nHead) heads against the single KV latent row, attn_sinks read through view \(snkIdx), agrees with the fp64 ds4.c transcription (maxAbs \(m4), maxRel \(r4); \(d4) distinct; \(n4) NaN)",
      "gate \(gateBase+3) sink softmax pos \(pos): maxAbs \(m4) maxRel \(r4) nan \(n4)")
    if !a4 { coreFail += 1 }

    // ds4.c:13793 — the heads are UN-roped before the output projection (sign = -1).
    let headsU = gpuRope(headsA, nHead, headDim, pos, true)
    let hup = headsU.contents().bindMemory(to: Float.self, capacity: nHead*headDim)
    let (a5, m5, r5, n5, d5, _, _) = cmpOra(hup, ohd, 3e-4, 3e-4, 1e-2)
    check(a5, "gate \(gateBase+4) the INVERSE RoPE on the attention output at pos \(pos) [RENTED ORACLE]: sign -1 over the same trailing \(nRot), agrees with the fp64 ds4.c transcription (maxAbs \(m5), maxRel \(r5); \(d5) distinct; \(n5) NaN)",
      "gate \(gateBase+4) inverse RoPE pos \(pos): maxAbs \(m5) maxRel \(r5) nan \(n5)")
    if !a5 { coreFail += 1 }

    // ds4.c:10356 / :7123 — the GROUPED output, factor a: 8 groups of 8 heads, each -> rank 1024.
    let low = gpuGrouped(views[oaIdx], oaInner, headsU, oaRows, oaCols, oRank)
    let lowp = low.contents().bindMemory(to: Float.self, capacity: oaRows)
    let (a6, m6, r6, n6, d6, _, _) = cmpOra(lowp, olow, 2e-3, 2e-3, 1e-2)
    check(a6, "gate \(gateBase+5) the GROUPED output factor a at pos \(pos) [RENTED ORACLE]: the type-41 attn_output_a (\(oaRows)x\(oaCols)) with row \(oRank)-grouped input addressing — group g's \(oaCols) heads-slice into rows g*\(oRank)..+\(oRank) — agrees with the fp64 ds4.c transcription (maxAbs \(m6), maxRel \(r6); \(d6) distinct; \(n6) NaN)",
      "gate \(gateBase+5) grouped out a pos \(pos): maxAbs \(m6) maxRel \(r6) nan \(n6)")
    if !a6 { coreFail += 1 }

    // ds4.c:10370 — factor b: the concatenated 8*1024 = 8192 latents back to n_embd 4096.
    let attnOut = gpuMx8(views[obIdx], obIdxInner, low, obRows, obCols)
    let aop = attnOut.contents().bindMemory(to: Float.self, capacity: obRows)
    let (a7, m7, r7, n7, d7, mn7b, mx7b) = cmpOra(aop, oout, 6e-3, 6e-3, 1e-2)
    check(a7, "gate \(gateBase+6) the GROUPED output factor b at pos \(pos) [RENTED ORACLE]: the type-41 attn_output_b (\(obRows)x\(obCols)) maps the \(obCols) group-latents back to n_embd \(obRows) — the WHOLE attention block's output — agreeing with the fp64 ds4.c transcription (maxAbs \(m7), maxRel \(r7); \(d7) distinct, range [\(mn7b),\(mx7b)]; \(n7) NaN)",
      "gate \(gateBase+6) grouped out b pos \(pos): maxAbs \(m7) maxRel \(r7) nan \(n7)")
    if !a7 { coreFail += 1 }

    var v = [Float](repeating: 0, count: obRows)
    for i in 0..<obRows { v[i] = aop[i] }
    outsByPos[pos] = v
    lastOut = v
}

runCore(posA, oraDirA, 9)
runCore(posB, oraDirB, 16)

// ── GATE 23: hushfold (row 859) on the BODY's own numbers. RoPE is the identity at position 0, so a core
// checked at one position witnesses nothing about it. The two runs above must therefore DISAGREE with each
// other while each agrees with its own oracle — that, and only that, is the witness.
var posDiff = 0; var maxPosDelta: Float = 0
if let a = outsByPos[posA], let b = outsByPos[posB] {
    for i in 0..<min(a.count, b.count) { if a[i] != b[i] { posDiff += 1; maxPosDelta = max(maxPosDelta, abs(a[i]-b[i])) } }
}
check(posDiff > 0 && coreFail == 0,
  "gate 23 hushfold: the same token's attention output DIFFERS between pos \(posA) and pos \(posB) in \(posDiff)/\(obRows) entries (max delta \(maxPosDelta)) while each run agrees with its OWN oracle — the RoPE is witnessed, not assumed",
  "gate 23 hushfold: the two positions produced identical output (\(posDiff) differing) or a core gate failed (\(coreFail))")

// ══════════════════════════════════════════════════════════════════════════════════════════════════
// STONE 36 STAGE 4 — ONE COMPLETE ATTENTION HALF OF A REAL LAYER: HC-pre -> MLA -> HC-post.
//
// Everything above fed the MLA the token's raw EMBEDDING as a probe (knownsolved). That bound is removed
// here: the residual state is the embedding BROADCAST to all \(nHc) hyper-connection streams (ds4.c:9764),
// hc_pre collapses it (ds4.c:9690) and hc_post recombines the block's output with the same residual and the
// SAME post/comb that hc_pre produced (ds4.c:9772). These are the real layer-0 activations.
// The whole half is proven against the rented oracle in `hc` mode — the choosing class throughout.
// ══════════════════════════════════════════════════════════════════════════════════════════════════
let pHcBcast = try dev.makeComputePipelineState(function: lHc.makeFunction(name: "form_hc_broadcast_f32")!)
let pHcRmsNw = try dev.makeComputePipelineState(function: lHc.makeFunction(name: "form_hc_rmsnorm_nw_f32")!)
let pHcSplit = try dev.makeComputePipelineState(function: lHc.makeFunction(name: "form_hc_split_f32")!)
let pHcWsum  = try dev.makeComputePipelineState(function: lHc.makeFunction(name: "form_hc_wsum_f32")!)
let pHcPost  = try dev.makeComputePipelineState(function: lHc.makeFunction(name: "form_hc_post_f32")!)
let pF16mv   = try dev.makeComputePipelineState(function: lCore.makeFunction(name: "form_dsv4_f16_matvec")!)

func enc1(_ p: MTLComputePipelineState, _ n: Int, _ body: (MTLComputeCommandEncoder) -> Void) {
    let cb = queue.makeCommandBuffer()!, e = cb.makeComputeCommandEncoder()!
    e.setComputePipelineState(p); body(e)
    e.dispatchThreads(MTLSize(width: n, height: 1, depth: 1),
                      threadsPerThreadgroup: MTLSize(width: min(p.maxTotalThreadsPerThreadgroup, 256), height: 1, depth: 1))
    e.endEncoding(); run(cb)
}
let hcDim = nHc * nEmbd
// ds4.c:9764 — the plain embedding broadcast to every stream.
let residHc = sentinelled(hcDim)
do { var a = UInt32(nHc), b = UInt32(nEmbd)
     enc1(pHcBcast, hcDim) { e in e.setBuffer(x0, offset: 0, index: 0); e.setBuffer(residHc, offset: 0, index: 1)
                                  e.setBytes(&a, length: 4, index: 2); e.setBytes(&b, length: 4, index: 3) } }
// ds4.c:9707 — rms_norm_no_weight over the WHOLE n_hc*n_embd state (not per stream).
let hcFlat = sentinelled(hcDim)
do { var n = UInt32(hcDim), e0 = eps
     enc1(pHcRmsNw, 1) { e in e.setBuffer(residHc, offset: 0, index: 0); e.setBuffer(hcFlat, offset: 0, index: 1)
                              e.setBytes(&n, length: 4, index: 2); e.setBytes(&e0, length: 4, index: 3) } }
let hcFlatP = hcFlat.contents().bindMemory(to: Float.self, capacity: hcDim)
let (okF, maF, mrF, nnF, dsF, _, _) = cmpOra(hcFlatP, readOracle(oraDirHc, "hc_flat"), 2e-4, 2e-4, 1e-2)
check(okF, "gate 24 the HC state's unweighted RMSNorm at real dims [RENTED ORACLE]: the embedding broadcast to all \(nHc) streams and normed over the WHOLE \(hcDim)-wide state agrees with the fp64 ds4.c transcription (maxAbs \(maF), maxRel \(mrF); \(dsF) distinct; \(nnF) NaN)",
  "gate 24 hc rmsnorm: maxAbs \(maF) maxRel \(mrF) nan \(nnF)")

// ds4.c:9711 — the F16 mixing projection hc_attn_fn [16384 -> 24].
let hcMix = sentinelled(hfRows)
do { var r = UInt32(hfRows), c = UInt32(hfCols)
     enc1(pF16mv, hfRows) { e in e.setBuffer(views[hfIdx], offset: hfInner, index: 0); e.setBuffer(hcFlat, offset: 0, index: 1)
                                 e.setBuffer(hcMix, offset: 0, index: 2)
                                 e.setBytes(&r, length: 4, index: 3); e.setBytes(&c, length: 4, index: 4) } }
let hcMixP = hcMix.contents().bindMemory(to: Float.self, capacity: hfRows)
let oMix = readOracle(oraDirHc, "hc_mix")
var mixAbs = 0.0, mixRel = 0.0
for i in 0..<min(hfRows, oMix.count) {
    let d = abs(Double(hcMixP[i]) - oMix[i]); mixAbs = max(mixAbs, d)
    if abs(oMix[i]) > 1e-2 { mixRel = max(mixRel, d/abs(oMix[i])) }
}
check(mixRel < 1e-4 && oMix.count == hfRows && gpuErrors == 0,
  "gate 25 the HC mixing projection at real dims [RENTED ORACLE]: hc_attn_fn (F16, \(hfRows)x\(hfCols)) through view \(hfIdx) agrees with the fp64 ds4.c transcription on all \(hfRows) mix logits (maxRel \(mixRel), maxAbs \(mixAbs) on a vector reaching \(oMix.map{abs($0)}.max() ?? 0))",
  "gate 25 hc mix: maxRel \(mixRel) maxAbs \(mixAbs) n \(oMix.count)")

// ds4.c:9592 — the sinkhorn split: pre = sigmoid+eps, post = 2*sigmoid, comb = row-softmax then \(hcIters)
// alternating column/row normalisations. The FIRST normalisation is by column, then the loop starts at 1.
let hcSplit = sentinelled(2*nHc + nHc*nHc)
do { var a = UInt32(nHc), it = UInt32(hcIters), e0 = hcEps
     enc1(pHcSplit, 1) { e in e.setBuffer(hcMix, offset: 0, index: 0)
                              e.setBuffer(views[hsIdx], offset: hsInner, index: 1)
                              e.setBuffer(views[hbIdx], offset: hbInner, index: 2)
                              e.setBuffer(hcSplit, offset: 0, index: 3)
                              e.setBytes(&a, length: 4, index: 4); e.setBytes(&it, length: 4, index: 5)
                              e.setBytes(&e0, length: 4, index: 6) } }
let hcSplitP = hcSplit.contents().bindMemory(to: Float.self, capacity: 2*nHc + nHc*nHc)
let oPost = readOracle(oraDirHc, "hc_post_w"), oComb = readOracle(oraDirHc, "hc_comb")
var splitAbs = 0.0
for i in 0..<nHc { splitAbs = max(splitAbs, abs(Double(hcSplitP[nHc+i]) - oPost[i])) }
for i in 0..<(nHc*nHc) { splitAbs = max(splitAbs, abs(Double(hcSplitP[2*nHc+i]) - oComb[i])) }
var combRowSum = 0.0
for src in 0..<nHc { combRowSum += Double(hcSplitP[2*nHc + src]) }
check(splitAbs < 1e-5 && gpuErrors == 0,
  "gate 26 the HC sinkhorn split at real dims [RENTED ORACLE]: \(hcIters) iterations over the \(nHc)x\(nHc) combine matrix, plus the pre and post gates, agree with the fp64 ds4.c transcription (maxAbs \(splitAbs); post reaches \(oPost.map{abs($0)}.max() ?? 0), comb row 0 sums to \(combRowSum))",
  "gate 26 hc split: maxAbs \(splitAbs)")

// ds4.c:9717 — the weighted collapse of the streams. THIS is the MLA's real input.
let hcCur = sentinelled(nEmbd)
do { var a = UInt32(nHc), b = UInt32(nEmbd)
     enc1(pHcWsum, nEmbd) { e in e.setBuffer(residHc, offset: 0, index: 0); e.setBuffer(hcSplit, offset: 0, index: 1)
                                 e.setBuffer(hcCur, offset: 0, index: 2)
                                 e.setBytes(&a, length: 4, index: 3); e.setBytes(&b, length: 4, index: 4) } }
let hcCurP = hcCur.contents().bindMemory(to: Float.self, capacity: nEmbd)
let (okC, maC, mrC, nnC, dsC, mnC, mxC) = cmpOra(hcCurP, readOracle(oraDirHc, "hc_cur"), 2e-5, 2e-5, 1e-2)
check(okC, "gate 27 the HC-pre collapse at real dims [RENTED ORACLE]: the \(nHc) streams weighted by the split's pre gates give the REAL layer-0 MLA input — no longer a probe (maxAbs \(maC), maxRel \(mrC); \(dsC) distinct, range [\(mnC),\(mxC)]; \(nnC) NaN)",
  "gate 27 hc-pre collapse: maxAbs \(maC) maxRel \(mrC) nan \(nnC)")

// ── the whole MLA block, re-run on the REAL input. Same kernels, same views, same order as gates 2..22.
func mlaBlock(_ input: MTLBuffer, _ pos: Int) -> MTLBuffer {
    let xn = gpuRmsnorm(input, nEmbd, views[normIdx], normInner)
    let ql = gpuMx8(views[qaIdx], qaInner, xn, qaRows, qaCols)
    let qln = gpuRmsnorm(ql, qaRows, views[qanIdx], qanInner)
    let qq = gpuMx8(views[qbIdx], qbInner, qln, qbRows, qbCols)
    let qh = gpuHeadrms(qq, nHead, headDim)
    let qr = gpuRope(qh, nHead, headDim, pos, false)
    let kl = gpuMx8(views[kvIdx], kvInner, xn, kvRows, kvCols)
    let kln = gpuRmsnorm(kl, kvRows, views[kvanIdx], kvanInner)
    let kr = gpuRope(kln, 1, headDim, pos, false)
    let kq = gpuKvRound(kr, headDim)
    let ha = gpuAttend(qr, kq, nHead, headDim, 1, views[snkIdx], snkInner)
    let hu = gpuRope(ha, nHead, headDim, pos, true)
    let lo = gpuGrouped(views[oaIdx], oaInner, hu, oaRows, oaCols, oRank)
    return gpuMx8(views[obIdx], obIdxInner, lo, obRows, obCols)
}
let realAttnOut = mlaBlock(hcCur, posA)
let raop = realAttnOut.contents().bindMemory(to: Float.self, capacity: obRows)
let (okR, maR, mrR, nnR, dsR, mnR, mxR) = cmpOra(raop, readOracle(oraDirHc, "attn_out"), 6e-3, 6e-3, 1e-2)
check(okR, "gate 28 the WHOLE MLA block on the REAL layer-0 input at pos \(posA) [RENTED ORACLE]: the same 13 dispatches gates 2-22 proved, re-run on hc_pre's output instead of a probe, agree with the fp64 ds4.c transcription end to end (maxAbs \(maR), maxRel \(mrR); \(dsR) distinct, range [\(mnR),\(mxR)]; \(nnR) NaN)",
  "gate 28 real MLA block: maxAbs \(maR) maxRel \(mrR) nan \(nnR)")

// ds4.c:9772 — hc_post: block_out*post[dst] + sum_src comb[dst + src*n_hc]*resid[src]. The combine matrix
// is addressed [dst, src] — transposing it is a choice a self-carve cannot see.
let afterAttn = sentinelled(hcDim)
do { var a = UInt32(nHc), b = UInt32(nEmbd)
     enc1(pHcPost, nHc) { e in e.setBuffer(realAttnOut, offset: 0, index: 0); e.setBuffer(residHc, offset: 0, index: 1)
                               e.setBuffer(hcSplit, offset: nHc*4, index: 2)
                               e.setBuffer(hcSplit, offset: 2*nHc*4, index: 3)
                               e.setBuffer(afterAttn, offset: 0, index: 4)
                               e.setBytes(&a, length: 4, index: 5); e.setBytes(&b, length: 4, index: 6) } }
let aap = afterAttn.contents().bindMemory(to: Float.self, capacity: hcDim)
let (okA, maA, mrA, nnA, dsA, mnA, mxA) = cmpOra(aap, readOracle(oraDirHc, "after_attn_hc"), 2e-5, 2e-5, 1e-2)
check(okA, "gate 29 the HC-post recombination at real dims [RENTED ORACLE]: out[dst][d] = attn_out[d]*post[dst] + sum_src comb[dst + src*\(nHc)]*resid[src][d] over all \(hcDim) — the COMPLETE attention half of a real layer-0, HC-pre -> MLA -> HC-post, agreeing with the fp64 ds4.c transcription (maxAbs \(maA), maxRel \(mrA); \(dsA) distinct, range [\(mnA),\(mxA)]; \(nnA) NaN)",
  "gate 29 hc-post: maxAbs \(maA) maxRel \(mrA) nan \(nnA)")

if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER ERROR(S) — first: \(gpuFirstError ?? "unknown") ===") }
print(String(format: "      Q latent[0..3] = %.6f %.6f %.6f %.6f", qLatp[0], qLatp[1], qLatp[2], qLatp[3]))
print(String(format: "      KV latent[0..3] = %.6f %.6f %.6f %.6f", kvLatp[0], kvLatp[1], kvLatp[2], kvLatp[3]))
print(String(format: "      device.currentAllocatedSize = %ld B (%.2f GiB) — the model is mmapped and wrapped, not copied (onelean)", dev.currentAllocatedSize, Double(dev.currentAllocatedSize)/1073741824.0))

print(String(format: "      attn_out(pos %d)[0..3] = %.6f %.6f %.6f %.6f", posB, lastOut[0], lastOut[1], lastOut[2], lastOut[3]))

let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  30 gates — Stone 35's MLA projection surface at real dims (gates 0-7, CANONICAL, self-carve) PLUS Stone 36's whole ATTENTION CORE at real dims (gates 8-23, CHOOSING, vs a rented fp64 ds4.c transcription) at two positions: per-head RMSNorm, RoPE fwd on q and kv, the KV fp8+f16 round-trip, the sink softmax, the inverse RoPE, and the GROUPED output a then b — the block's whole output — PLUS Stone 36 Stage 4 (gates 24-29): ONE COMPLETE ATTENTION HALF of a real layer, HC-pre -> MLA -> HC-post on the REAL layer-0 activations, no probe") }
else { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT
swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || { echo "FAIL swiftc runner"; tail -30 "$work/swift.err"; exit 1; }

"$work/runner" "$LIB_EMB" "$LIB_MLA" "$LIB8" "$LIB_CORE" "$LIB_HC" "$BLOB" \
    "$STEP" "$VIEWLIMIT" "$NVIEWS" \
    "$EMB_ABS" "$ROW_OFF" "$N_EMBD" "$EMB_IDX" "$EMB_INNER" "$EMB_HOLDS" "$TOKEN" \
    "$NORM_ABS" "$NORM_IDX" "$NORM_INNER" "$NORM_HOLDS" \
    "$QA_ABS" "$QA_IDX" "$QA_INNER" "$QA_HOLDS" "$QA_OUT" "$QA_IN" \
    "$QAN_ABS" "$QAN_IDX" "$QAN_INNER" "$QAN_HOLDS" \
    "$QB_ABS" "$QB_IDX" "$QB_INNER" "$QB_HOLDS" "$QB_OUT" "$QB_IN" \
    "$KV_ABS" "$KV_IDX" "$KV_INNER" "$KV_HOLDS" "$KV_OUT" "$KV_IN" \
    "$KVAN_ABS" "$KVAN_IDX" "$KVAN_INNER" "$KVAN_HOLDS" \
    "$RMS_EPS" \
    "$SNK_ABS" "$SNK_IDX" "$SNK_INNER" "$SNK_HOLDS" \
    "$OA_ABS" "$OA_IDX" "$OA_INNER" "$OA_HOLDS" "$OA_OUT" "$OA_IN" \
    "$OB_ABS" "$OB_IDX" "$OB_INNER" "$OB_HOLDS" "$OB_OUT" "$OB_IN" \
    "$N_HEAD" "$HEAD_DIM" "$N_ROT" "$ROPE_BASE" "$N_GROUPS" "$O_RANK" \
    "$POS_A" "$POS_B" "$work/ora$POS_A" "$work/ora$POS_B" \
    "$HF_ABS" "$HF_IDX" "$HF_INNER" "$HF_HOLDS" "$HF_OUT" "$HF_IN" \
    "$HS_ABS" "$HS_IDX" "$HS_INNER" "$HS_HOLDS" \
    "$HB_ABS" "$HB_IDX" "$HB_INNER" "$HB_HOLDS" \
    "$N_HC" "$HC_ITERS" "$HC_EPS" "$work/orahc"
rc=$?
exit $rc
