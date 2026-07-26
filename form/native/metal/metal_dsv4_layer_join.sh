#!/usr/bin/env bash
# metal_dsv4_layer_join.sh — STONE 37, Stage 1: ONE COMPLETE DeepSeek-V4-Flash LAYER at REAL DIMS.
#
# Two halves were standing and had never been joined:
#   * the ATTENTION half — Stone 36, metal_dsv4_layer.sh (30 gates): HC-pre -> MLA -> HC-post on the real
#     layer-0 activations, proven against a rented fp64 ds4.c transcription.
#   * the FFN half — Stone 34, metal_dsv4_forward.sh (8 gates): the hash table read, the F16 router
#     matvec, MXFP4 gate/up and the fused IQ2_XXS down, each self-carved at real dims.
# The join is the SECOND hyper-connection frame. HC is this model's residual stream — there is no plain
# residual anywhere in it — so a complete layer is two blocks inside two independent HC frames:
#     hc_pre(attn) -> MLA -> hc_post(attn) -> hc_pre(ffn) -> ffn_norm -> MoE+shared -> hc_post(ffn)
# and that whole chain, on the file's own blk.0 weights through the overlapping views, is what this
# harness runs and gates. Its output is out_hc: the four hyper-connection streams the NEXT layer receives.
#
# THE EVIDENCE CLASS PER STAGE (twinblind, corpus row 868), named because they are not the same:
#   CHOOSING  — the two HC frames, ffn_norm's placement, the gating function sqrt(softplus(.)), the hash
#               selection on the TOKEN id, the floored-sum weight normalisation and the 1.5 scale, the
#               clamp's asymmetry, the router weight multiplying the MID, the shared expert being added.
#               A self-carve inherits every one of those on BOTH sides and is blind to all of them, so
#               they are proven against form-stdlib/tests/dsv4-mla-core-oracle.py in `layer` mode: an
#               independent fp64 transcription of ds4.c's control flow that parses the same GGUF itself
#               and shares no code, no buffer and no arithmetic with the band, the MSL or this carrier.
#   CANONICAL — the MXFP4 / IQ2_XXS / MXFP8 / F16 decodes. One right answer; Stones 33/34/35 self-carved
#               them at real dims (GPU through the view vs an independent CPU decode of the same bytes),
#               and those harnesses still gate them. Here the oracle's own independent decode re-witnesses
#               them, which is a strictly stronger check than a second copy of the same code.
#
# THE RECIPE GAP, said out loud (aporon). ds4.c cannot execute this file's FFN: matvec_experts_mid_prequant
# (:9349) refuses type-40 gate/up and layer_shared_ffn_one (:10460) demands a Q8_0 shared expert where
# this file carries type 41. So the oracle rents ds4.c's ORDER and its scalars and feeds each expert
# matvec the EXACT activation — ds4.c's own ds4_vec_dot_iq2_xxs_f32 (:3779) control flow — rather than its
# Q8_K-prequantised path. What is proven stops exactly there.
#
# hushfold (corpus row 859): RoPE is the identity at position 0, so the whole layer is run at TWO
# positions and the two outputs are required to DIFFER while each agrees with its own oracle.
# zerobirth/edgedrop: every output buffer is NaN-sentinelled before its dispatch and cb.error / cb.status
# checked after; a dead view or an unrun kernel reads as a sentinel the comparator rejects.
# onelean/lapspan: the 85 GiB file exceeds maxBufferLength, so every weight is reached through the
# overlapping page-aligned bytesNoCopy view set the body's own residency plan lays out.
#
# Run:  form/native/metal/metal_dsv4_layer_join.sh   (optional: FORM_DS4_PROMPT_TOKEN=<id>)
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
BLOB="${FORM_DS4_BLOB:-$HOME/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf}"
CACHE="$ROOT/native/metal/.metallib-cache"
TOKEN="${FORM_DS4_PROMPT_TOKEN:-671}"
LAYER=0

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
echo "ds4 blob: $FSIZE bytes at $(date '+%H:%M:%S')   ONE COMPLETE LAYER (blk.$LAYER, token=$TOKEN)"

# ── the `; preludes:` directives are LIVE recursive load instructions; walked, never hand-catted ──
fk_deps(){ awk 'BEGIN{IGNORECASE=1} /^;[ \t]*preludes:/{ s=$0; sub(/^;[ \t]*preludes:[ \t]*/,"",s); n=split(s,a,/[ \t]+/); for(i=1;i<=n;i++){ if(a[i]=="\\"||tolower(a[i])=="none"||tolower(a[i])=="(none)"||a[i]=="")continue; if(a[i]~/\.fk$/)print a[i] } }' "$1" 2>/dev/null; }
fk_path(){ local dir; dir="$(dirname "$1")"; if [[ -f "$dir/$2" ]]; then printf '%s\n' "$dir/$2"; elif [[ -f "$2" ]]; then printf '%s\n' "$2"; elif [[ "$2" == form/* && -f "${2#form/}" ]]; then printf '%s\n' "${2#form/}"; else printf '%s\n' "$dir/$2"; fi; }
fk_expand(){ local f="$1" d p; case " $FK_SEEN " in *" $f "*) return ;; esac; FK_SEEN="$FK_SEEN $f"; while read -r d; do [[ -z "$d" ]] && continue; p="$(fk_path "$f" "$d")"; fk_expand "$p"; done < <(fk_deps "$f"); printf '%s\n' "$f"; }
cd "$ROOT"
FK_SEEN=""; FILES=(); while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/dsv4-layer-real.fk)

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

# ── 2. the body's residency plan + the manifest, walked over the LIVE file ─────────────────────────
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

# every tensor this layer touches, emitted as KEY value lines the carrier parses by name — a positional
# argument list of ninety numbers is a place a wrong number hides in plain sight.
P="blk.$LAYER"
emit_tensor(){ # $1 KEY  $2 tensor name
    local k="$1" n="$2"
    printf '%s_ABS %s\n%s_BYTES %s\n%s_IDX %s\n%s_INNER %s\n%s_HOLDS %s\n%s_D0 %s\n%s_D1 %s\n%s_D2 %s\n' \
        "$k" "$(tv "$n" 3)" "$k" "$(tv "$n" 4)" "$k" "$(tv "$n" 5)" "$k" "$(tv "$n" 6)" "$k" "$(tv "$n" 7)" \
        "$k" "$(trow "$n" 5)" "$k" "$(trow "$n" 6)" "$k" "$(trow "$n" 7)" >> "$work/params.txt"
}
: > "$work/params.txt"
emit_tensor EMB   token_embd.weight
emit_tensor NORM  $P.attn_norm.weight
emit_tensor QA    $P.attn_q_a.weight
emit_tensor QAN   $P.attn_q_a_norm.weight
emit_tensor QB    $P.attn_q_b.weight
emit_tensor KV    $P.attn_kv.weight
emit_tensor KVAN  $P.attn_kv_a_norm.weight
emit_tensor SNK   $P.attn_sinks.weight
emit_tensor OA    $P.attn_output_a.weight
emit_tensor OB    $P.attn_output_b.weight
emit_tensor HAF   $P.hc_attn_fn.weight
emit_tensor HAS   $P.hc_attn_scale.weight
emit_tensor HAB   $P.hc_attn_base.weight
emit_tensor HFF   $P.hc_ffn_fn.weight
emit_tensor HFS   $P.hc_ffn_scale.weight
emit_tensor HFB   $P.hc_ffn_base.weight
emit_tensor FN    $P.ffn_norm.weight
emit_tensor RT    $P.ffn_gate_inp.weight
emit_tensor HT    $P.ffn_gate_tid2eid.weight
emit_tensor GX    $P.ffn_gate_exps.weight
emit_tensor UX    $P.ffn_up_exps.weight
emit_tensor DX    $P.ffn_down_exps.weight
emit_tensor SG    $P.ffn_gate_shexp.weight
emit_tensor SU    $P.ffn_up_shexp.weight
emit_tensor SD    $P.ffn_down_shexp.weight

N_EMBD=4096; N_HEAD=64; HEAD_DIM=512; N_ROT=64; ROPE_BASE=10000.0; O_RANK=1024
N_HC=4; HC_ITERS=20; HC_EPS=0.0000009999999975; RMS_EPS=0.0000009999999975
N_EXPERT=256; N_USED=6; N_FF=2048; WSCALE=1.5; CLAMP=10.0
POS_A=0; POS_B=7
cat >> "$work/params.txt" <<EOF
STEP $STEP
VIEWLIMIT $VIEWLIMIT
NVIEWS $NVIEWS
TOKEN $TOKEN
N_EMBD $N_EMBD
N_HEAD $N_HEAD
HEAD_DIM $HEAD_DIM
N_ROT $N_ROT
O_RANK $O_RANK
N_HC $N_HC
HC_ITERS $HC_ITERS
N_EXPERT $N_EXPERT
N_USED $N_USED
N_FF $N_FF
POS_A $POS_A
POS_B $POS_B
ROPE_BASE $ROPE_BASE
HC_EPS $HC_EPS
RMS_EPS $RMS_EPS
WSCALE $WSCALE
CLAMP $CLAMP
EOF
awk '{ if ($2 == "" ) { print "FAIL missing value for " $1 > "/dev/stderr"; exit 1 } }' "$work/params.txt" || exit 1

# ── 2b. THE RENTED ORACLE, in `layer` mode, at BOTH positions (hushfold) ───────────────────────────
ORACLE="$ROOT/form-stdlib/tests/dsv4-mla-core-oracle.py"
[[ -f "$ORACLE" ]] || { echo "FAIL the rented oracle is missing: $ORACLE"; exit 1; }
for POS in "$POS_A" "$POS_B"; do
    mkdir -p "$work/ora$POS"
    echo "  renting the oracle in LAYER mode at pos=$POS (independent fp64 transcription of ds4.c)..."
    DSV4_ORACLE_MODE=layer DSV4_ORACLE_OUT="$work/ora$POS" python3 "$ORACLE" "$BLOB" "$TOKEN" "$POS" "$LAYER" \
        > "$work/ora$POS.txt" 2>"$work/ora$POS.err" \
        || { echo "FAIL oracle layer-mode run at pos=$POS"; tail -5 "$work/ora$POS.err"; exit 1; }
    grep -qx 'END' "$work/ora$POS.txt" || { echo "FAIL oracle stream truncated at pos=$POS"; exit 1; }
done
awk '/^FFN /{print "  oracle: "$0}' "$work/ora$POS_A.txt"
# the oracle's own witness that the two positions are not the same run
if cmp -s "$work/ora$POS_A/oracle-out_hc.f64" "$work/ora$POS_B/oracle-out_hc.f64"; then
    echo "FAIL hushfold: the oracle's own layer output is identical at pos $POS_A and pos $POS_B"; exit 1
fi
echo "  hushfold: the ORACLE's layer output already differs between pos $POS_A and pos $POS_B — the GPU must too"

# ── 3. compile the translation units, cached by sha ────────────────────────────────────────────────
compile_unit() { # $1 emit-form  $2 grep-token  $3 cache-prefix -> echoes LIB path
    local form="$1" tok="$2" pre="$3" lib sha
    echo "($form)" > "$work/$pre.fk"
    "$GO_BIN" "${FILES[@]}" "$work/$pre.fk" > "$work/$pre.out" 2>"$work/$pre.err" || { echo "FAIL $pre MSL emission" >&2; cat "$work/$pre.err" >&2; return 1; }
    awk '/^MSL$/{d=1;next} /^END$/{d=0;next} d{print}' "$work/$pre.out" > "$work/$pre.metal"
    grep -q "$tok" "$work/$pre.metal" || { echo "FAIL $pre kernel $tok not emitted" >&2; return 1; }
    sha="$(shasum -a 256 "$work/$pre.metal" | cut -c1-16)"; lib="$CACHE/$pre-$sha.metallib"
    if [[ ! -f "$lib" ]]; then
        xcrun -sdk macosx metal -O2 -std=metal3.0 -ffp-contract=off -c "$work/$pre.metal" -o "$work/$pre.air" 2>"$work/$pre.merr" \
          && xcrun -sdk macosx metallib "$work/$pre.air" -o "$lib" 2>>"$work/$pre.merr" || { echo "FAIL $pre metal compile" >&2; cat "$work/$pre.merr" >&2; return 1; }
        echo "PASS  $pre metallib compiled: $(basename "$lib")" >&2
    else
        echo "PASS  $pre metallib cache HIT: $(basename "$lib")" >&2
    fi
    printf '%s\n' "$lib"
}
mkdir -p "$CACHE"
LIB_EMB="$(compile_unit  dsv4-embed-msl        form_dsv4_embed_f16            dsv4emb)"  || exit 1
LIB_MLA="$(compile_unit  dsv4-mla-unit         form_mla_rmsnorm_f32           dsv4mla)"  || exit 1
LIB8="$(compile_unit     dsv4-mx8-matvec-msl   form_dsv4_mx8_matvec           dsv4mx8)"  || exit 1
LIB_CORE="$(compile_unit dsv4-mla-core-msl     form_dsv4_mx8_matvec_grouped   dsv4core)" || exit 1
LIB_HC="$(compile_unit   dsv4-hc-unit          form_hc_split_f32              dsv4hc)"   || exit 1
LIB_MX4="$(compile_unit  dsv4-mx4-matvec-msl   form_dsv4_mx4_matvec           dsv4mx4)"  || exit 1
LIB_IQ2="$(compile_unit  dsv4-iq2-matvec-msl   form_dsv4_iq2_matvec           dsv4iq2)"  || exit 1
LIB_RT="$(compile_unit   dsv4-router-f16-msl   form_dsv4_router_f16           dsv4rt)"   || exit 1
LIB_FFN="$(compile_unit  dsv4-ffn-unit         form_dsv4_hash_weights         dsv4ffn)"  || exit 1

# ── 4. the carrier ─────────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

// ---- the parameter file: KEY value, read by NAME. No positional numbers.
let argv = CommandLine.arguments
let paramPath = argv[1], blobPath = argv[2]
var P: [String: String] = [:]
for line in (try! String(contentsOfFile: paramPath, encoding: .utf8)).split(separator: "\n") {
    let f = line.split(separator: " ", maxSplits: 1)
    if f.count == 2 { P[String(f[0])] = String(f[1]).trimmingCharacters(in: .whitespaces) }
}
func I(_ k: String) -> Int { guard let v = P[k], let n = Int(v) else { print("FAIL missing int param \(k)"); exit(1) }; return n }
func F(_ k: String) -> Float { guard let v = P[k], let n = Float(v) else { print("FAIL missing float param \(k)"); exit(1) }; return n }
let libEmb = argv[3], libMla = argv[4], lib8 = argv[5], libCore = argv[6], libHc = argv[7]
let libMx4 = argv[8], libIq2 = argv[9], libRt = argv[10], libFfn = argv[11]
let oraDirA = argv[12], oraDirB = argv[13]

// a tensor's coordinates, by name, as the plan and the manifest gave them.
struct Tn { let abs: Int, bytes: Int, idx: Int, inner: Int, holds: Int, d0: Int, d1: Int, d2: Int
            var rows: Int { d1 }          // [in, out] -> matvec rows = out
            var cols: Int { d0 }          //             matvec cols = in
            var nel: Int  { d0 * d1 } }
func T(_ k: String) -> Tn {
    return Tn(abs: I(k+"_ABS"), bytes: I(k+"_BYTES"), idx: I(k+"_IDX"), inner: I(k+"_INNER"),
              holds: I(k+"_HOLDS"), d0: I(k+"_D0"), d1: I(k+"_D1"), d2: I(k+"_D2"))
}
let emb = T("EMB"), nrm = T("NORM"), qa = T("QA"), qan = T("QAN"), qb = T("QB")
let kv = T("KV"), kvan = T("KVAN"), snk = T("SNK"), oa = T("OA"), ob = T("OB")
let haf = T("HAF"), has = T("HAS"), hab = T("HAB")
let hff = T("HFF"), hfs = T("HFS"), hfb = T("HFB")
let fnw = T("FN"), rt = T("RT"), ht = T("HT")
let gx = T("GX"), ux = T("UX"), dx = T("DX"), sgw = T("SG"), suw = T("SU"), sdw = T("SD")

let step = I("STEP"), viewLimit = I("VIEWLIMIT"), nviews = I("NVIEWS"), token = I("TOKEN")
let nEmbd = I("N_EMBD"), nHead = I("N_HEAD"), headDim = I("HEAD_DIM"), nRot = I("N_ROT"), oRank = I("O_RANK")
let nHc = I("N_HC"), hcIters = I("HC_ITERS"), nExpert = I("N_EXPERT"), nUsed = I("N_USED"), nFf = I("N_FF")
let posA = I("POS_A"), posB = I("POS_B")
let ropeBase = F("ROPE_BASE"), hcEps = F("HC_EPS"), eps = F("RMS_EPS"), wscale = F("WSCALE"), clamp = F("CLAMP")

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
func lib(_ p: String) -> MTLLibrary { return try! dev.makeLibrary(URL: URL(fileURLWithPath: p)) }
let lEmb = lib(libEmb), lMla = lib(libMla), l8 = lib(lib8), lCore = lib(libCore), lHc = lib(libHc)
let lMx4 = lib(libMx4), lIq2 = lib(libIq2), lRt = lib(libRt), lFfn = lib(libFfn)
let queue = dev.makeCommandQueue()!
var failures = 0, gpuErrors = 0
var gpuFirstError: String? = nil
func check(_ ok: Bool, _ pass: String, _ fail: String) { if ok { print("PASS  " + pass) } else { print("FAIL  " + fail); failures += 1 } }

// ---- the file, mmapped once and wrapped in the body's own overlapping views (onelean/lapspan) ----
let fd = open(blobPath, O_RDONLY)
guard fd >= 0 else { print("FAIL cannot open blob"); exit(1) }
var st = stat(); fstat(fd, &st)
let fileLen = Int(st.st_size); let page = Int(getpagesize())
let mapLen = (fileLen + page - 1) / page * page
guard let mapped0 = mmap(nil, mapLen, PROT_READ, MAP_PRIVATE, fd, 0), mapped0 != MAP_FAILED else { print("FAIL mmap failed"); exit(1) }

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
  "gate 0 the views map: all \(nviews) overlapping page-aligned bytesNoCopy views of the \(fileLen) B file wrap on \(dev.name) — one buffer over the whole file cannot (maxBufferLength \(dev.maxBufferLength))",
  "gate 0 only \(views.count)/\(nviews) views mapped")
if failures > 0 { print("VERDICT FAIL the views did not map"); exit(1) }

let all: [(String, Tn)] = [("token_embd", emb), ("attn_norm", nrm), ("attn_q_a", qa), ("attn_q_a_norm", qan),
    ("attn_q_b", qb), ("attn_kv", kv), ("attn_kv_a_norm", kvan), ("attn_sinks", snk),
    ("attn_output_a", oa), ("attn_output_b", ob), ("hc_attn_fn", haf), ("hc_attn_scale", has),
    ("hc_attn_base", hab), ("hc_ffn_fn", hff), ("hc_ffn_scale", hfs), ("hc_ffn_base", hfb),
    ("ffn_norm", fnw), ("ffn_gate_inp", rt), ("ffn_gate_tid2eid", ht), ("ffn_gate_exps", gx),
    ("ffn_up_exps", ux), ("ffn_down_exps", dx), ("ffn_gate_shexp", sgw), ("ffn_up_shexp", suw),
    ("ffn_down_shexp", sdw)]
let spanning = all.filter { $0.1.holds != 1 || $0.1.idx >= nviews }
check(spanning.isEmpty,
  "gate 1 residency: all \(all.count) tensors a complete layer touches — the attention block, BOTH hyper-connection frames, the router, the hash table, the 256-expert stacks and the shared expert — each lie wholly inside one view",
  "gate 1 these tensors span views or index past the set: \(spanning.map { $0.0 })")
if failures > 0 { print("VERDICT FAIL"); exit(1) }

// ---- the oracle's vectors ----
func readOracle(_ dir: String, _ key: String) -> [Double] {
    let p = dir + "/oracle-" + key + ".f64"
    guard let s = try? String(contentsOfFile: p, encoding: .utf8) else { print("FAIL oracle vector missing: \(p)"); exit(1) }
    var out: [Double] = []
    s.split(separator: "\n").forEach { if let v = Double($0) { out.append(v) } }
    return out
}
// assocwall (row 866): the oracle is fp64 over a real-width reduction and the GPU is f32, so the honest
// gate is an ABSOLUTE bound over EVERY element plus a RELATIVE bound taken only above a magnitude floor —
// below the floor an ~1e-6 absolute difference reads as a huge relative purely because the denominator
// is ~0. zerobirth/edgedrop: a sentinel, a NaN or a degenerate spread fails regardless of the bounds.
func cmpOra(_ gpu: UnsafeMutablePointer<Float>, _ ref: [Double], _ absB: Double, _ relB: Double, _ floor: Double)
        -> (Bool, Double, Double, Int, Int, Float, Float) {
    var maxAbs = 0.0, maxRel = 0.0, nan = 0
    var seen = Set<UInt32>(); var vmin = Float.greatestFiniteMagnitude, vmax = -Float.greatestFiniteMagnitude
    for i in 0..<ref.count {
        let g = gpu[i]
        if g.isNaN || !g.isFinite { nan += 1; continue }
        let d = abs(Double(g) - ref[i])
        if d > maxAbs { maxAbs = d }
        if abs(ref[i]) > floor { let r = d/abs(ref[i]); if r > maxRel { maxRel = r } }
        seen.insert(g.bitPattern); vmin = min(vmin, g); vmax = max(vmax, g)
    }
    // The degeneracy floor must SCALE with the vector, or it becomes a false accuser: the six routed
    // expert weights cannot have nine distinct values however alive they are. A guard that fails an
    // honest result is exactly as much a defect as one that passes a dead read, so the floor is
    // half the width, capped at 8 — still "a sentinel, a memset or one repeated value cannot pass".
    let floorN = min(8, max(2, ref.count / 2))
    let nonDegen = seen.count > floorN && vmax > vmin
    return (nan == 0 && maxAbs < absB && maxRel < relB && nonDegen, maxAbs, maxRel, nan, seen.count, vmin, vmax)
}

// ---- dispatch helpers; every output buffer is sentinelled, every command buffer checked ----
func sentinelled(_ n: Int) -> MTLBuffer {
    let b = dev.makeBuffer(length: max(n,1)*4, options: .storageModeShared)!
    let p = b.contents().bindMemory(to: Float.self, capacity: max(n,1))
    for i in 0..<max(n,1) { p[i] = Float.nan }
    return b
}
func sentinelledU(_ n: Int) -> MTLBuffer {
    let b = dev.makeBuffer(length: max(n,1)*4, options: .storageModeShared)!
    let p = b.contents().bindMemory(to: UInt32.self, capacity: max(n,1))
    for i in 0..<max(n,1) { p[i] = 0xFFFFFFFF }
    return b
}
func run(_ cb: MTLCommandBuffer) {
    cb.commit(); cb.waitUntilCompleted()
    if let er = cb.error { gpuErrors += 1; if gpuFirstError == nil { gpuFirstError = "\(er)" } }
    if cb.status != .completed { gpuErrors += 1 }
}
func pipe(_ l: MTLLibrary, _ n: String) -> MTLComputePipelineState {
    guard let f = l.makeFunction(name: n) else { print("FAIL kernel \(n) is not in its library"); exit(1) }
    return try! dev.makeComputePipelineState(function: f)
}
func enc(_ p: MTLComputePipelineState, _ n: Int, _ cap: Int, _ body: (MTLComputeCommandEncoder) -> Void) {
    let cb = queue.makeCommandBuffer()!, e = cb.makeComputeCommandEncoder()!
    e.setComputePipelineState(p); body(e)
    e.dispatchThreads(MTLSize(width: n, height: 1, depth: 1),
                      threadsPerThreadgroup: MTLSize(width: min(p.maxTotalThreadsPerThreadgroup, cap), height: 1, depth: 1))
    e.endEncoding(); run(cb)
}

let pEmb = pipe(lEmb, "form_dsv4_embed_f16")
let pRms = pipe(lMla, "form_mla_rmsnorm_f32")
let pHeadrms = pipe(lMla, "form_mla_headrms_f32")
let pRope = pipe(lMla, "form_mla_rope_f32")
let pAttend = pipe(lMla, "form_mla_attend_f32")
let pMx8 = pipe(l8, "form_dsv4_mx8_matvec")
let pGrouped = pipe(lCore, "form_dsv4_mx8_matvec_grouped")
let pKvq = pipe(lCore, "form_dsv4_kv_fp8_f16_round")
let pF16mv = pipe(lCore, "form_dsv4_f16_matvec")
let pHcBcast = pipe(lHc, "form_hc_broadcast_f32")
let pHcRmsNw = pipe(lHc, "form_hc_rmsnorm_nw_f32")
let pHcSplit = pipe(lHc, "form_hc_split_f32")
let pHcWsum = pipe(lHc, "form_hc_wsum_f32")
let pHcPost = pipe(lHc, "form_hc_post_f32")
let pMx4 = pipe(lMx4, "form_dsv4_mx4_matvec")
let pIq2 = pipe(lIq2, "form_dsv4_iq2_matvec")
let pRtF16 = pipe(lRt, "form_dsv4_router_f16")
let pSwiglu = pipe(lFfn, "form_dsv4_swiglu_f32")
let pScale = pipe(lFfn, "form_dsv4_scale_f32")
let pAxpy = pipe(lFfn, "form_dsv4_axpy_f32")
let pHashSel = pipe(lFfn, "form_dsv4_hash_select")
let pHashW = pipe(lFfn, "form_dsv4_hash_weights")

func gpuRmsnorm(_ x: MTLBuffer, _ n: Int, _ t: Tn) -> MTLBuffer {
    let out = sentinelled(n); var n32 = UInt32(n), e = eps
    enc(pRms, 1, 1) { c in c.setBuffer(x, offset: 0, index: 0); c.setBuffer(views[t.idx], offset: t.inner, index: 1)
                           c.setBuffer(out, offset: 0, index: 2)
                           c.setBytes(&n32, length: 4, index: 3); c.setBytes(&e, length: 4, index: 4) }
    return out
}
func gpuMx8(_ t: Tn, _ x: MTLBuffer, _ rows: Int, _ cols: Int) -> MTLBuffer {
    let out = sentinelled(rows); var r = UInt32(rows), c32 = UInt32(cols), nel = UInt32(rows*cols)
    enc(pMx8, rows*32, 256) { c in c.setBuffer(views[t.idx], offset: t.inner, index: 0); c.setBuffer(x, offset: 0, index: 1)
                                   c.setBuffer(out, offset: 0, index: 2)
                                   c.setBytes(&r, length: 4, index: 3); c.setBytes(&c32, length: 4, index: 4); c.setBytes(&nel, length: 4, index: 5) }
    return out
}
func gpuHeadrms(_ x: MTLBuffer) -> MTLBuffer {
    let out = sentinelled(nHead*headDim); var a = UInt32(nHead), b = UInt32(headDim), e = eps
    enc(pHeadrms, nHead, 64) { c in c.setBuffer(x, offset: 0, index: 0); c.setBuffer(out, offset: 0, index: 1)
                                    c.setBytes(&a, length: 4, index: 2); c.setBytes(&b, length: 4, index: 3); c.setBytes(&e, length: 4, index: 4) }
    return out
}
let nPair = nRot/2
let freqBuf = dev.makeBuffer(length: nPair*4, options: .storageModeShared)!
do {
    let fp = freqBuf.contents().bindMemory(to: Float.self, capacity: nPair)
    let thetaScale = powf(ropeBase, -2.0/Float(nRot))
    var f: Float = 1.0
    for k in 0..<nPair { fp[k] = f; f *= thetaScale }
}
func gpuRope(_ v: MTLBuffer, _ nh: Int, _ pos: Int, _ inverse: Bool) -> MTLBuffer {
    let out = sentinelled(nh*headDim)
    var a = UInt32(nh), b = UInt32(headDim), c32 = UInt32(nRot), p = Float(pos), s: Float = inverse ? -1.0 : 1.0
    enc(pRope, nh, 64) { c in c.setBuffer(v, offset: 0, index: 0); c.setBuffer(out, offset: 0, index: 1)
                              c.setBuffer(freqBuf, offset: 0, index: 2)
                              c.setBytes(&a, length: 4, index: 3); c.setBytes(&b, length: 4, index: 4); c.setBytes(&c32, length: 4, index: 5)
                              c.setBytes(&p, length: 4, index: 6); c.setBytes(&s, length: 4, index: 7) }
    return out
}
func gpuKvRound(_ v: MTLBuffer) -> MTLBuffer {
    let out = sentinelled(headDim); var a = UInt32(headDim), b = UInt32(nRot)
    enc(pKvq, 1, 1) { c in c.setBuffer(v, offset: 0, index: 0); c.setBuffer(out, offset: 0, index: 1)
                           c.setBytes(&a, length: 4, index: 2); c.setBytes(&b, length: 4, index: 3) }
    return out
}
func gpuAttend(_ q: MTLBuffer, _ rows: MTLBuffer) -> MTLBuffer {
    let out = sentinelled(nHead*headDim)
    var a = UInt32(nHead), b = UInt32(headDim), c32 = UInt32(1), sc = 1.0/sqrtf(Float(headDim))
    enc(pAttend, nHead, 32) { c in c.setBuffer(q, offset: 0, index: 0); c.setBuffer(rows, offset: 0, index: 1)
                                   c.setBuffer(out, offset: 0, index: 2)
                                   c.setBuffer(views[snk.idx], offset: snk.inner, index: 3)
                                   c.setBytes(&a, length: 4, index: 4); c.setBytes(&b, length: 4, index: 5)
                                   c.setBytes(&c32, length: 4, index: 6); c.setBytes(&sc, length: 4, index: 7) }
    return out
}
func gpuGrouped(_ t: Tn, _ x: MTLBuffer) -> MTLBuffer {
    let out = sentinelled(t.rows)
    var r = UInt32(t.rows), c32 = UInt32(t.cols), nel = UInt32(t.nel), rk = UInt32(oRank)
    enc(pGrouped, t.rows*32, 256) { c in c.setBuffer(views[t.idx], offset: t.inner, index: 0); c.setBuffer(x, offset: 0, index: 1)
                                         c.setBuffer(out, offset: 0, index: 2)
                                         c.setBytes(&r, length: 4, index: 3); c.setBytes(&c32, length: 4, index: 4)
                                         c.setBytes(&nel, length: 4, index: 5); c.setBytes(&rk, length: 4, index: 6) }
    return out
}
func gpuF16mv(_ t: Tn, _ x: MTLBuffer) -> MTLBuffer {
    let out = sentinelled(t.rows); var r = UInt32(t.rows), c32 = UInt32(t.cols)
    enc(pF16mv, t.rows, 256) { c in c.setBuffer(views[t.idx], offset: t.inner, index: 0); c.setBuffer(x, offset: 0, index: 1)
                                    c.setBuffer(out, offset: 0, index: 2)
                                    c.setBytes(&r, length: 4, index: 3); c.setBytes(&c32, length: 4, index: 4) }
    return out
}
// the MXFP4 expert matvec: the view is bound at the EXPERT's own byte slice, so the kernel's r*cols+j
// indices address inside that expert and never form a 32-bit offset into an 85 GiB file.
func gpuMx4Expert(_ t: Tn, _ x: MTLBuffer, _ expert: Int) -> MTLBuffer {
    let rows = t.d1, cols = t.d0, nel = rows*cols
    let stride = t.bytes / t.d2
    let out = sentinelled(rows); var r = UInt32(rows), c32 = UInt32(cols), n32 = UInt32(nel)
    enc(pMx4, rows*32, 256) { c in c.setBuffer(views[t.idx], offset: t.inner + expert*stride, index: 0)
                                   c.setBuffer(x, offset: 0, index: 1); c.setBuffer(out, offset: 0, index: 2)
                                   c.setBytes(&r, length: 4, index: 3); c.setBytes(&c32, length: 4, index: 4); c.setBytes(&n32, length: 4, index: 5) }
    return out
}
func gpuIq2Expert(_ t: Tn, _ x: MTLBuffer, _ expert: Int) -> MTLBuffer {
    let rows = t.d1, cols = t.d0
    let stride = t.bytes / t.d2
    let out = sentinelled(rows); var r = UInt32(rows), c32 = UInt32(cols)
    enc(pIq2, rows*32, 256) { c in c.setBuffer(views[t.idx], offset: t.inner + expert*stride, index: 0)
                                   c.setBuffer(x, offset: 0, index: 1); c.setBuffer(out, offset: 0, index: 2)
                                   c.setBytes(&r, length: 4, index: 3); c.setBytes(&c32, length: 4, index: 4) }
    return out
}
func gpuSwiglu(_ gate: MTLBuffer, _ up: MTLBuffer, _ n: Int, _ w: Float, _ lim: Float) -> MTLBuffer {
    let out = sentinelled(n); var n32 = UInt32(n), ww = w, ll = lim
    enc(pSwiglu, n, 256) { c in c.setBuffer(gate, offset: 0, index: 0); c.setBuffer(up, offset: 0, index: 1)
                                c.setBuffer(out, offset: 0, index: 2)
                                c.setBytes(&n32, length: 4, index: 3); c.setBytes(&ww, length: 4, index: 4); c.setBytes(&ll, length: 4, index: 5) }
    return out
}

// ══════════════════════════════════════════════════════════════════════════════════════════════════
// THE LAYER. Everything below is ONE pass of blk.0 on the file's own weights, at a given position.
// ══════════════════════════════════════════════════════════════════════════════════════════════════
struct LayerRun {
    let afterAttn: MTLBuffer, ffnCur: MTLBuffer, ffnNorm: MTLBuffer, logits: MTLBuffer
    let ids: [Int], wts: [Float], gate0: MTLBuffer, up0: MTLBuffer, mid0: MTLBuffer, down0: MTLBuffer
    let moe: MTLBuffer, shared: MTLBuffer, ffnOut: MTLBuffer, outHc: MTLBuffer
}

// the token's real F16 embedding, decoded on the GPU through the view.
let rowOff = token * nEmbd * 2
let x0 = sentinelled(nEmbd)
do { var b64 = UInt64(emb.inner + rowOff), c32 = UInt32(nEmbd)
     enc(pEmb, nEmbd, 256) { c in c.setBuffer(views[emb.idx], offset: 0, index: 0); c.setBuffer(x0, offset: 0, index: 1)
                                  c.setBytes(&b64, length: 8, index: 2); c.setBytes(&c32, length: 4, index: 3) } }
let hcDim = nHc * nEmbd
// ds4.c:9764 — the plain embedding broadcast to every hyper-connection stream. THE layer-0 input.
let residHc = sentinelled(hcDim)
do { var a = UInt32(nHc), b = UInt32(nEmbd)
     enc(pHcBcast, hcDim, 256) { c in c.setBuffer(x0, offset: 0, index: 0); c.setBuffer(residHc, offset: 0, index: 1)
                                      c.setBytes(&a, length: 4, index: 2); c.setBytes(&b, length: 4, index: 3) } }

// ds4.c:9690 hc_pre — rms(no weight) over the WHOLE n_hc*n_embd state, an F16 mix projection, the
// sinkhorn split, then the weighted collapse. Returns (cur, split) where split holds pre|post|comb.
func hcPre(_ resid: MTLBuffer, _ fn: Tn, _ sc: Tn, _ bs: Tn) -> (MTLBuffer, MTLBuffer) {
    let flat = sentinelled(hcDim)
    do { var n = UInt32(hcDim), e0 = eps
         enc(pHcRmsNw, 1, 1) { c in c.setBuffer(resid, offset: 0, index: 0); c.setBuffer(flat, offset: 0, index: 1)
                                    c.setBytes(&n, length: 4, index: 2); c.setBytes(&e0, length: 4, index: 3) } }
    let mix = gpuF16mv(fn, flat)
    let split = sentinelled(2*nHc + nHc*nHc)
    do { var a = UInt32(nHc), it = UInt32(hcIters), e0 = hcEps
         enc(pHcSplit, 1, 1) { c in c.setBuffer(mix, offset: 0, index: 0)
                                    c.setBuffer(views[sc.idx], offset: sc.inner, index: 1)
                                    c.setBuffer(views[bs.idx], offset: bs.inner, index: 2)
                                    c.setBuffer(split, offset: 0, index: 3)
                                    c.setBytes(&a, length: 4, index: 4); c.setBytes(&it, length: 4, index: 5)
                                    c.setBytes(&e0, length: 4, index: 6) } }
    let cur = sentinelled(nEmbd)
    do { var a = UInt32(nHc), b = UInt32(nEmbd)
         enc(pHcWsum, nEmbd, 256) { c in c.setBuffer(resid, offset: 0, index: 0); c.setBuffer(split, offset: 0, index: 1)
                                         c.setBuffer(cur, offset: 0, index: 2)
                                         c.setBytes(&a, length: 4, index: 3); c.setBytes(&b, length: 4, index: 4) } }
    return (cur, split)
}
// ds4.c:9772 hc_post — out[dst][d] = block_out[d]*post[dst] + sum_src comb[dst + src*n_hc]*resid[src][d].
// The combine matrix is addressed [dst, src]; transposing it is a choice no self-carve can see.
func hcPost(_ blockOut: MTLBuffer, _ resid: MTLBuffer, _ split: MTLBuffer) -> MTLBuffer {
    let out = sentinelled(hcDim); var a = UInt32(nHc), b = UInt32(nEmbd)
    enc(pHcPost, nHc, 256) { c in c.setBuffer(blockOut, offset: 0, index: 0); c.setBuffer(resid, offset: 0, index: 1)
                                  c.setBuffer(split, offset: nHc*4, index: 2)
                                  c.setBuffer(split, offset: 2*nHc*4, index: 3)
                                  c.setBuffer(out, offset: 0, index: 4)
                                  c.setBytes(&a, length: 4, index: 5); c.setBytes(&b, length: 4, index: 6) }
    return out
}
// the MLA block, exactly the thirteen dispatches metal_dsv4_layer.sh gates 2..22 proved.
func mlaBlock(_ input: MTLBuffer, _ pos: Int) -> MTLBuffer {
    let xn = gpuRmsnorm(input, nEmbd, nrm)
    let ql = gpuMx8(qa, xn, qa.rows, qa.cols)
    let qln = gpuRmsnorm(ql, qa.rows, qan)
    let qq = gpuMx8(qb, qln, qb.rows, qb.cols)
    let qh = gpuHeadrms(qq)
    let qr = gpuRope(qh, nHead, pos, false)
    let kl = gpuMx8(kv, xn, kv.rows, kv.cols)
    let kln = gpuRmsnorm(kl, kv.rows, kvan)
    let kr = gpuRope(kln, 1, pos, false)
    let kq = gpuKvRound(kr)
    let ha = gpuAttend(qr, kq)
    let hu = gpuRope(ha, nHead, pos, true)
    let lo = gpuGrouped(oa, hu)
    return gpuMx8(ob, lo, ob.rows, ob.cols)
}

func runLayer(_ pos: Int) -> LayerRun {
    // ---- the attention half (Stone 36, re-run here so the FFN's input is computed, never asserted) ----
    let (attnCur, attnSplit) = hcPre(residHc, haf, has, hab)
    let attnOut = mlaBlock(attnCur, pos)
    let afterAttn = hcPost(attnOut, residHc, attnSplit)

    // ---- the FFN half's own hyper-connection frame: hc_ffn_*, NOT hc_attn_* ----
    let (ffnCur, ffnSplit) = hcPre(afterAttn, hff, hfs, hfb)
    let ffnNorm = gpuRmsnorm(ffnCur, nEmbd, fnw)

    // ---- the router: F16 logits over the NORMED state, then the hash selection and its weights ----
    let logits = gpuF16mv(rt, ffnNorm)
    let idsBuf = sentinelledU(nUsed)
    do { var t32 = UInt32(token), nu = UInt32(nUsed)
         enc(pHashSel, nUsed, 8) { c in c.setBuffer(views[ht.idx], offset: ht.inner, index: 0)
                                        c.setBuffer(idsBuf, offset: 0, index: 1)
                                        c.setBytes(&t32, length: 4, index: 2); c.setBytes(&nu, length: 4, index: 3) } }
    let wtsBuf = sentinelled(nUsed), probsBuf = sentinelled(nExpert)
    do { var ne = UInt32(nExpert), nu = UInt32(nUsed), ws = wscale
         enc(pHashW, 1, 1) { c in c.setBuffer(logits, offset: 0, index: 0); c.setBuffer(idsBuf, offset: 0, index: 1)
                                  c.setBuffer(wtsBuf, offset: 0, index: 2); c.setBuffer(probsBuf, offset: 0, index: 3)
                                  c.setBytes(&ne, length: 4, index: 4); c.setBytes(&nu, length: 4, index: 5)
                                  c.setBytes(&ws, length: 4, index: 6) } }
    let idp = idsBuf.contents().bindMemory(to: UInt32.self, capacity: nUsed)
    let wtp = wtsBuf.contents().bindMemory(to: Float.self, capacity: nUsed)
    var ids: [Int] = [], wts: [Float] = []
    for i in 0..<nUsed { ids.append(Int(idp[i])); wts.append(wtp[i]) }

    // ---- the six routed experts: MXFP4 gate/up -> clamped SwiGLU x router weight -> IQ2_XXS down ----
    let moe = sentinelled(nEmbd)
    var g0 = moe, u0 = moe, m0 = moe, d0 = moe
    for (i, e) in ids.enumerated() {
        guard e >= 0 && e < nExpert else { print("FAIL hash-selected expert \(e) is outside [0,\(nExpert))"); exit(1) }
        let gt = gpuMx4Expert(gx, ffnNorm, e)
        let up = gpuMx4Expert(ux, ffnNorm, e)
        let mid = gpuSwiglu(gt, up, nFf, wts[i], clamp)
        let dn = gpuIq2Expert(dx, mid, e)
        var one: Float = 1.0, n32 = UInt32(nEmbd)
        if i == 0 {
            enc(pScale, nEmbd, 256) { c in c.setBuffer(dn, offset: 0, index: 0); c.setBuffer(moe, offset: 0, index: 1)
                                           c.setBytes(&one, length: 4, index: 2); c.setBytes(&n32, length: 4, index: 3) }
            g0 = gt; u0 = up; m0 = mid; d0 = dn
        } else {
            enc(pAxpy, nEmbd, 256) { c in c.setBuffer(dn, offset: 0, index: 0); c.setBuffer(moe, offset: 0, index: 1)
                                          c.setBytes(&one, length: 4, index: 2); c.setBytes(&n32, length: 4, index: 3) }
        }
    }

    // ---- the shared expert: it runs for EVERY token, is not routed, and its weight is 1 ----
    let sgv = gpuMx8(sgw, ffnNorm, sgw.rows, sgw.cols)
    let suv = gpuMx8(suw, ffnNorm, suw.rows, suw.cols)
    let smid = gpuSwiglu(sgv, suv, nFf, 1.0, clamp)
    let shared = gpuMx8(sdw, smid, sdw.rows, sdw.cols)

    // ---- ffn_out = moe + shared, then the SECOND hc_post over the FFN frame's own post/comb ----
    let ffnOut = sentinelled(nEmbd)
    do { var one: Float = 1.0, n32 = UInt32(nEmbd)
         enc(pScale, nEmbd, 256) { c in c.setBuffer(moe, offset: 0, index: 0); c.setBuffer(ffnOut, offset: 0, index: 1)
                                        c.setBytes(&one, length: 4, index: 2); c.setBytes(&n32, length: 4, index: 3) }
         enc(pAxpy, nEmbd, 256) { c in c.setBuffer(shared, offset: 0, index: 0); c.setBuffer(ffnOut, offset: 0, index: 1)
                                       c.setBytes(&one, length: 4, index: 2); c.setBytes(&n32, length: 4, index: 3) } }
    let outHc = hcPost(ffnOut, afterAttn, ffnSplit)
    return LayerRun(afterAttn: afterAttn, ffnCur: ffnCur, ffnNorm: ffnNorm, logits: logits,
                    ids: ids, wts: wts, gate0: g0, up0: u0, mid0: m0, down0: d0,
                    moe: moe, shared: shared, ffnOut: ffnOut, outHc: outHc)
}

func fp(_ b: MTLBuffer, _ n: Int) -> UnsafeMutablePointer<Float> { return b.contents().bindMemory(to: Float.self, capacity: n) }

var outsByPos: [Int: [Float]] = [:]
var layerFail = 0
func gateLayer(_ pos: Int, _ oraDir: String, _ base: Int, _ verbose: Bool) {
    let R = runLayer(pos)
    func G(_ n: Int, _ buf: MTLBuffer, _ cnt: Int, _ key: String, _ absB: Double, _ relB: Double,
           _ passText: String) {
        let ref = readOracle(oraDir, key)
        guard ref.count == cnt else { print("FAIL gate \(n) oracle \(key) has \(ref.count) entries, expected \(cnt)"); failures += 1; layerFail += 1; return }
        let (ok, ma, mr, nn, ds, mn, mx) = cmpOra(fp(buf, cnt), ref, absB, relB, 1e-2)
        check(ok && gpuErrors == 0,
          "gate \(n) \(passText) [RENTED ORACLE, pos \(pos)] (maxAbs \(ma), maxRel \(mr) above |1e-2|; \(ds) distinct, range [\(mn),\(mx)]; \(nn) NaN)",
          "gate \(n) \(key) pos \(pos): maxAbs \(ma) maxRel \(mr) nan \(nn) distinct \(ds) gpuErrors \(gpuErrors)")
        if !(ok && gpuErrors == 0) { layerFail += 1 }
    }
    // the attention half, end to end — Stone 36's result, recomputed here as the FFN's real input.
    G(base, R.afterAttn, hcDim, "after_attn_hc", 2e-5, 2e-5,
      "the ATTENTION half of a real layer: hc_pre(attn) -> the 13 MLA dispatches -> hc_post(attn) over all \(hcDim) hyper-connection entries")
    // THE JOIN: the FFN's own hyper-connection frame, on the attention half's output.
    G(base+1, R.ffnCur, nEmbd, "ffn_cur", 3e-5, 3e-5,
      "THE JOIN — hc_pre(ffn) on the attention half's output: rms-no-weight over the whole \(hcDim)-wide state, the F16 hc_ffn_fn mix, a \(hcIters)-iteration sinkhorn split from hc_ffn_scale/hc_ffn_base, and the collapse of the \(nHc) streams. A SECOND, INDEPENDENT frame — hc_ffn_*, never hc_attn_*")
    G(base+2, R.ffnNorm, nEmbd, "ffn_normed", 2e-4, 2e-4,
      "ffn_norm over the joined state (ds4.c:11477) — the FFN's input, not the attention's")
    G(base+3, R.logits, nExpert, "router_logits", 3e-4, 3e-4,
      "the router's F16 logit projection ffn_gate_inp (\(rt.rows)x\(rt.cols)) over the NORMED state, through view \(rt.idx)")
    // forepick (row 867): the selection is a TABLE READ on the token id, and it is bit-exact.
    let oraSel = readOracle(oraDir, "selected").map { Int($0) }
    check(R.ids == oraSel && !R.ids.isEmpty,
      "gate \(base+4) forepick — the layer-\(0) HASH selection [RENTED ORACLE, pos \(pos)]: the GPU read ffn_gate_tid2eid's I32 row for token \(token) through view \(ht.idx) and got experts \(R.ids), bit-identical to the oracle's. The router did NOT choose these; the table did (ds4.c:4806/:10567, n_hash_layer 3)",
      "gate \(base+4) hash selection: GPU \(R.ids) vs oracle \(oraSel)")
    if R.ids != oraSel { failures += 1; layerFail += 1 }
    G(base+5, sentinelledCopy(R.wts), nUsed, "expert_w", 2e-5, 2e-5,
      "the routed weights: probs = sqrt(softplus(logit)) — gating func 4, NOT a softmax — gathered at the six table-selected experts, divided by their floored sum and scaled by \(wscale)")
    G(base+6, R.gate0, nFf, "exp0_gate", 3e-3, 3e-3,
      "expert \(R.ids.first ?? -1)'s MXFP4 (type 40) GATE projection (\(gx.d1)x\(gx.d0)) fused decode+matvec at the expert's own byte slice of the 256-expert stack")
    G(base+7, R.up0, nFf, "exp0_up", 3e-3, 3e-3,
      "expert \(R.ids.first ?? -1)'s MXFP4 UP projection through the same view at the same slice")
    G(base+8, R.mid0, nFf, "exp0_mid", 3e-4, 3e-4,
      "the clamped SwiGLU mid: gate clamped ABOVE only, up clamped BOTH ways at \(clamp), silu(g)*u, then multiplied by THIS expert's router weight — before the down projection, not after")
    G(base+9, R.down0, nEmbd, "exp0_down", 3e-4, 3e-4,
      "expert \(R.ids.first ?? -1)'s IQ2_XXS (type 16) DOWN projection (\(dx.d1)x\(dx.d0)) — the fused 2-bit matvec, the trained grid decoded on the device")
    G(base+10, R.moe, nEmbd, "moe", 2e-3, 2e-3,
      "all \(nUsed) routed experts accumulated — 18 quantised matvecs over 6 slices of two type-40 stacks and one type-16 stack")
    G(base+11, R.shared, nEmbd, "shared", 6e-3, 6e-3,
      "the SHARED expert (MXFP8, type 41): it runs for every token, is not routed, and carries weight 1")
    G(base+12, R.ffnOut, nEmbd, "ffn_out", 6e-3, 6e-3,
      "ffn_out = routed + shared, the FFN block's whole output")
    G(base+13, R.outHc, hcDim, "out_hc", 3e-4, 3e-4,
      "ONE COMPLETE LAYER — hc_post(ffn) closes the second frame over the SAME residual and the SAME post/comb its own hc_pre produced. These \(hcDim) numbers are the \(nHc) hyper-connection streams blk.1 receives")
    var v = [Float](repeating: 0, count: hcDim)
    let op = fp(R.outHc, hcDim); for i in 0..<hcDim { v[i] = op[i] }
    outsByPos[pos] = v
    if verbose {
        print("      selected \(R.ids)  weights \(R.wts.map { (($0*1e6).rounded())/1e6 })")
        print(String(format: "      out_hc[0..3] = %.6f %.6f %.6f %.6f", v[0], v[1], v[2], v[3]))
    }
}
// a tiny helper so a Swift array can go through the same oracle comparator as a device buffer.
func sentinelledCopy(_ a: [Float]) -> MTLBuffer {
    let b = dev.makeBuffer(length: max(a.count,1)*4, options: .storageModeShared)!
    let p = b.contents().bindMemory(to: Float.self, capacity: max(a.count,1))
    for i in 0..<a.count { p[i] = a[i] }
    return b
}

gateLayer(posA, oraDirA, 2, true)
gateLayer(posB, oraDirB, 16, true)

// ── hushfold (row 859) at LAYER scale: RoPE is the identity at position 0, so a layer checked at one
// position witnesses nothing about it. The two complete layers must DISAGREE with each other while each
// agrees with its own oracle — that, and only that, is the witness.
var posDiff = 0; var maxDelta: Float = 0
if let a = outsByPos[posA], let b = outsByPos[posB] {
    for i in 0..<min(a.count, b.count) { if a[i] != b[i] { posDiff += 1; maxDelta = max(maxDelta, abs(a[i]-b[i])) } }
}
check(posDiff > 0 && layerFail == 0,
  "gate 30 hushfold at layer scale: the same token's COMPLETE LAYER output differs between pos \(posA) and pos \(posB) in \(posDiff)/\(hcDim) entries (max delta \(maxDelta)) while each run agrees with its OWN oracle",
  "gate 30 hushfold: \(posDiff) differing entries, \(layerFail) failed layer gates")

if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER ERROR(S) — first: \(gpuFirstError ?? "unknown") ===") }
print(String(format: "      device.currentAllocatedSize = %ld B (%.2f GiB) — the model is mmapped and wrapped, not copied (onelean)", dev.currentAllocatedSize, Double(dev.currentAllocatedSize)/1073741824.0))

let ok = failures == 0 && gpuErrors == 0
if ok { print("VERDICT PASS  31 gates — ONE COMPLETE DeepSeek-V4-Flash LAYER at real dims over the 85 GiB file: hc_pre(attn) -> MLA -> hc_post(attn) -> hc_pre(ffn) -> ffn_norm -> hash-routed MoE over 6 of 256 experts + the shared expert -> hc_post(ffn), at TWO positions, every choosing surface against a rented fp64 ds4.c transcription and every dispatch sentinelled") }
else { print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors") }
exit(ok ? 0 : 1)
SWIFT
swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || { echo "FAIL swiftc runner"; tail -40 "$work/swift.err"; exit 1; }

"$work/runner" "$work/params.txt" "$BLOB" \
    "$LIB_EMB" "$LIB_MLA" "$LIB8" "$LIB_CORE" "$LIB_HC" \
    "$LIB_MX4" "$LIB_IQ2" "$LIB_RT" "$LIB_FFN" \
    "$work/ora$POS_A" "$work/ora$POS_B"
rc=$?
exit $rc
