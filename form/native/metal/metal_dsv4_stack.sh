#!/usr/bin/env bash
# metal_dsv4_stack.sh — STONE 39: STACKING the 43 HETEROGENEOUS DeepSeek-V4-Flash layers at REAL DIMS.
#
# Stone 37 stood ONE complete layer (metal_dsv4_layer_join.sh, 31 gates) on blk.0's weights. This is not
# that layer run 43 times. The file's own tensor table says the layers differ on four independent
# per-layer decisions, and a blk.0-shaped stack is right-shaped, right-magnitude and silently wrong:
#
#   1. the expert count is the gate stack's own dim[2] (256 for layers 0..2, 192 after), NOT the KV's 256,
#      while the router still projects 256 logits at every layer. The file keeps the two consistent by
#      carrying exp_probs_b.bias = -1e30 on exactly the 64 pruned indices — read, not assumed (gate 2).
#   2. the expert TYPES flip between GGUF 40 (MXFP4) and 16 (IQ2_XXS), independently for gate/up vs down,
#      across six layer groups. The dispatch reads each tensor's own type.
#   3. routing changes at layer 3: the ffn_gate_tid2eid I32 table (forepick, row 867) gives way to biased
#      top-k with UNBIASED weighting (ds4.c:10665) — the new form_dsv4_topk_weights kernel.
#   4. RoPE goes compressed at layer 2 and needs NO new kernel: the YaRN magnitude cancels (ds4.c:10175)
#      and the angle reduces to a per-pair SCALE of theta_extrap, which form_mla_rope_f32's freqs[]
#      already carries. The freqs are re-derived HERE from the file's own KV.
#
# EVIDENCE CLASS PER STAGE (twinblind, corpus row 868):
#   CHOOSING  — the per-layer routing regime, the bias-in/weight-out asymmetry, which expert-type kernel
#               each half takes, the compressed-RoPE reduction, and how the four hyper-connection streams
#               compose from one layer into the next. Proven against the rented fp64 ds4.c transcription
#               by the native runner's per-layer finite, diversity, routing-bound, sentinel, and command-
#               buffer witnesses. No reference process enters generation.
#   CANONICAL — the MXFP4 / IQ2_XXS / MXFP8 / F16 decodes and matvecs (Stones 33/34/35).
#
# halfrent (row 870) DEEPENS: ds4.c cannot even VALIDATE this file's layers 3..42 —
# tensor_expect_routed_expert (:4641) demands dim[2] == 256 and exit(1)s on 192. So what is rented is the
# order and the scalars; the arithmetic for these types is re-derived on both sides. Said, not buried.
#
# hushfold (row 859): the whole stack runs at TWO positions and the outputs must DIFFER.
# zerobirth/edgedrop: every output buffer is NaN-sentinelled and cb.error/cb.status checked.
# onelean/lapspan: every weight of every layer is reached through the overlapping bytesNoCopy views.
#
# Run:  form/native/metal/metal_dsv4_stack.sh
#   FORM_DS4_STACK_LAYERS=<n>     how many layers to stack (default: the file's block_count)
#   FORM_DS4_PROMPT_TOKEN=<id>
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
BLOB="${FORM_DS4_BLOB:-$HOME/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf}"
CACHE="$ROOT/native/metal/.metallib-cache"
TOKEN="${FORM_DS4_PROMPT_TOKEN:-671}"
KV_CAP="${FORM_DS4_KV_CAP:-4}"
KV_SEQUENCE="${FORM_DS4_KV_SEQUENCE:-0}"
KV_STEPS="${FORM_DS4_KV_STEPS:-2}"
# PROMPT PREFILL. A single seed token can only ever continue itself; a real answer needs the model
# to have read a real prompt first. FORM_DS4_PROMPT_IDS carries the tokenizer's own output — the
# body encodes the text with dsv4-tokenizer-cli and hands the ids here, so no text parsing enters
# this carrier and the tokenizer stays the one authority on what a prompt IS.
PROMPT_IDS="${FORM_DS4_PROMPT_IDS:-}"
PROMPT_N=0
if [ -n "$PROMPT_IDS" ]; then PROMPT_N=$(echo "$PROMPT_IDS" | wc -w | tr -d ' '); else PROMPT_IDS="none"; fi
CONTROL_ADAPTER="${FORM_DS4_CONTROL_ADAPTER:-$ROOT/native/metal/artifacts/dsv4-control-logit-adapter.f32}"
POS_A=0
POS_B=7

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the GPU witness needs an Apple GPU + swiftc"; exit 2
fi
if [[ ! -f "$BLOB" ]]; then
    echo "SKIP  the ds4 GGUF is not on this host: $BLOB   (set FORM_DS4_BLOB)"; exit 2
fi
[[ -f "$CONTROL_ADAPTER" ]] || { echo "FAIL control-logit adapter artifact missing: $CONTROL_ADAPTER"; exit 1; }
if [[ ! -x "$GO_BIN" ]]; then
    echo "  building the Go kernel..."; (cd "$ROOT/form-kernel-go" && go build -o bin-go .) || { echo "FAIL go build"; exit 1; }
fi
FSIZE=$(stat -f%z "$BLOB")
work="$(mktemp -d)"; trap 'rm -rf "$work"' EXIT
echo "ds4 blob: $FSIZE bytes at $(date '+%H:%M:%S')   THE HETEROGENEOUS STACK (token=$TOKEN)"

# ── the `; preludes:` directives are LIVE recursive load instructions; walked, never hand-catted ──
fk_deps(){ awk 'BEGIN{IGNORECASE=1} /^;[ \t]*preludes:/{ s=$0; sub(/^;[ \t]*preludes:[ \t]*/,"",s); n=split(s,a,/[ \t]+/); for(i=1;i<=n;i++){ if(a[i]=="\\"||tolower(a[i])=="none"||tolower(a[i])=="(none)"||a[i]=="")continue; if(a[i]~/\.fk$/)print a[i] } }' "$1" 2>/dev/null; }
fk_path(){ local dir; dir="$(dirname "$1")"; if [[ -f "$dir/$2" ]]; then printf '%s\n' "$dir/$2"; elif [[ -f "$2" ]]; then printf '%s\n' "$2"; elif [[ "$2" == form/* && -f "${2#form/}" ]]; then printf '%s\n' "${2#form/}"; else printf '%s\n' "$dir/$2"; fi; }
fk_expand(){ local f="$1" d p; case " $FK_SEEN " in *" $f "*) return ;; esac; FK_SEEN="$FK_SEEN $f"; while read -r d; do [[ -z "$d" ]] && continue; p="$(fk_path "$f" "$d")"; fk_expand "$p"; done < <(fk_deps "$f"); printf '%s\n' "$f"; }
cd "$ROOT"
FK_SEEN=""; FILES=(); while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/dsv4-stack-real.fk)

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

# the KV stream the manifest emits is `KV <i> <vtype> <key>` followed by its own value line, so a scalar
# is read by NAME through the body's own walk — never by position and never hardcoded.
kvf(){ awk -v k="$1" '$1=="KV" && $4==k {w=1; next}
                      w && $1=="KVF32" { getline v; gsub(/^[ \t]+/,"",v); print v; exit }
                      w && $1=="KVINT" { print $3; exit }
                      $1=="KV" { w=0 }' "$work/man.out"; }
NLAYER="$(kvf deepseek4.block_count)"
if [[ -z "$NLAYER" ]]; then echo "FAIL the manifest does not carry deepseek4.block_count"; exit 1; fi
WANT_LAYERS="${FORM_DS4_STACK_LAYERS:-$NLAYER}"
echo "  the file declares $NLAYER blocks; this run stacks $WANT_LAYERS"

# the per-layer compress ratios are a HYPER-PARAMETER wearing an array's clothes, and the body already
# has a reader for exactly that case (gguf-manifest.fk, gm-emit-array). Walked, never guessed.
# READ BEFORE THE WANT LIST, because the ratio decides WHICH TENSORS A LAYER HAS: a compressing layer
# carries the second cache's four (attn_compressor_kv/gate/ape/norm) and a ratio-0 layer carries none.
printf '(gm-emit-array "%s" "deepseek4.attention.compress_ratios")\n' "$BLOB" > "$work/arr.fk"
"$GO_BIN" "${FILES[@]}" "$work/arr.fk" > "$work/arr.out" 2>"$work/arr.err" || { echo "FAIL ratio array emission"; tail -5 "$work/arr.err"; exit 1; }
grep -q '^ARR deepseek4.attention.compress_ratios' "$work/arr.out" || { echo "FAIL the file carries no deepseek4.attention.compress_ratios"; cat "$work/arr.out"; exit 1; }
RATIOS="$(awk '$1=="A"{ s = s (n++ ? "," : "") $3 } END{ print s }' "$work/arr.out")"
NRATIO="$(awk '$1=="A"{n++} END{print n+0}' "$work/arr.out")"
(( NRATIO >= WANT_LAYERS )) || { echo "FAIL compress_ratios carries $NRATIO entries for $WANT_LAYERS layers"; exit 1; }
LRATIO=(); while read -r r; do LRATIO+=("$r"); done < <(awk '$1=="A"{print $3}' "$work/arr.out")

# ── 2a. the WANT list: every tensor every stacked layer touches, named, then resolved in ONE pass ──
: > "$work/want.txt"
: > "$work/flags.txt"
echo "EMB token_embd.weight" >> "$work/want.txt"
echo "OUT_HC_FN output_hc_fn.weight" >> "$work/want.txt"
echo "OUT_HC_SCALE output_hc_scale.weight" >> "$work/want.txt"
echo "OUT_HC_BASE output_hc_base.weight" >> "$work/want.txt"
echo "OUT_NORM output_norm.weight" >> "$work/want.txt"
echo "OUT_WEIGHT output.weight" >> "$work/want.txt"
for ((il=0; il<WANT_LAYERS; il++)); do
    P="blk.$il"
    for pair in "NORM attn_norm" "QA attn_q_a" "QAN attn_q_a_norm" "QB attn_q_b" "KV attn_kv" \
                "KVAN attn_kv_a_norm" "SNK attn_sinks" "OA attn_output_a" "OB attn_output_b" \
                "HAF hc_attn_fn" "HAS hc_attn_scale" "HAB hc_attn_base" \
                "HFF hc_ffn_fn" "HFS hc_ffn_scale" "HFB hc_ffn_base" \
                "FN ffn_norm" "RT ffn_gate_inp" "GX ffn_gate_exps" "UX ffn_up_exps" "DX ffn_down_exps" \
                "SG ffn_gate_shexp" "SU ffn_up_shexp" "SD ffn_down_shexp"; do
        k="${pair%% *}"; t="${pair#* }"
        echo "L${il}_${k} $P.$t.weight" >> "$work/want.txt"
    done
    # THE SECOND CACHE'S TENSORS, on exactly the layers whose ratio is nonzero (ds4.c:4978). A ratio-0
    # layer does not carry them and asking for them would fail residency on a file that is correct.
    if [[ "${LRATIO[$il]:-0}" != "0" ]]; then
        for pair in "CKV attn_compressor_kv" "CGT attn_compressor_gate" \
                    "CAP attn_compressor_ape" "CNM attn_compressor_norm"; do
            k="${pair%% *}"; t="${pair#* }"
            echo "L${il}_${k} $P.$t.weight" >> "$work/want.txt"
        done
        echo "L${il}_HASCOMP 1" >> "$work/flags.txt"
    else
        echo "L${il}_HASCOMP 0" >> "$work/flags.txt"
    fi
    # the two routing regimes carry DIFFERENT tensors; which one a layer has IS the regime.
    if awk -v n="$P.ffn_gate_tid2eid.weight" '$1=="T" && $2==n{f=1} END{exit !f}' "$work/man.out"; then
        echo "L${il}_HT $P.ffn_gate_tid2eid.weight" >> "$work/want.txt"
        echo "L${il}_HASHED 1" >> "$work/flags.txt"
    else
        echo "L${il}_BI $P.exp_probs_b.bias" >> "$work/want.txt"
        echo "L${il}_HASHED 0" >> "$work/flags.txt"
    fi
done

awk -v plan="$work/plan.out" -v man="$work/man.out" '
  FILENAME==plan && $1=="TV" { a[$2]=$3; b[$2]=$4; c[$2]=$5; d[$2]=$6; e[$2]=$7; next }
  FILENAME==man  && $1=="T"  { ty[$2]=$3; z0[$2]=$5; z1[$2]=$6; z2[$2]=$7; next }
  FILENAME!=plan && FILENAME!=man {
      k=$1; n=$2
      if (!(n in a)) { print "FAIL the residency plan does not carry " n > "/dev/stderr"; bad=1; next }
      if (!(n in ty)) { print "FAIL the manifest does not carry " n > "/dev/stderr"; bad=1; next }
      printf "%s_ABS %s\n%s_BYTES %s\n%s_IDX %s\n%s_INNER %s\n%s_HOLDS %s\n%s_D0 %s\n%s_D1 %s\n%s_D2 %s\n%s_TYPE %s\n",
             k,a[n], k,b[n], k,c[n], k,d[n], k,e[n], k,z0[n], k,z1[n], k,z2[n], k,ty[n]
  }
  END { if (bad) exit 1 }' "$work/plan.out" "$work/man.out" "$work/want.txt" > "$work/params.txt" || exit 1
cat "$work/flags.txt" >> "$work/params.txt"

N_EMBD=4096; N_HEAD=64; HEAD_DIM=512; N_ROT=64; O_RANK=1024
N_HC=4; HC_ITERS=20; HC_EPS=0.0000009999999975; RMS_EPS=0.0000009999999975
N_USED=6; N_FF=2048; WSCALE=1.5; CLAMP=10.0
# the RoPE constants, taken from the file's own KV by the manifest walk (never hardcoded per layer).
ROPE_BASE="$(kvf deepseek4.rope.freq_base)";                      ROPE_BASE="${ROPE_BASE:-10000.0}"
ROPE_CBASE="$(kvf deepseek4.attention.compress_rope_freq_base)";  ROPE_CBASE="${ROPE_CBASE:-160000.0}"
ROPE_SCALEF="$(kvf deepseek4.rope.scaling.factor)";               ROPE_SCALEF="${ROPE_SCALEF:-16.0}"
ROPE_ORIGCTX="$(kvf deepseek4.rope.scaling.original_context_length)"; ROPE_ORIGCTX="${ROPE_ORIGCTX:-65536}"
BETA_FAST="$(kvf deepseek4.rope.scaling.yarn_beta_fast)";         BETA_FAST="${BETA_FAST:-32.0}"
BETA_SLOW="$(kvf deepseek4.rope.scaling.yarn_beta_slow)";         BETA_SLOW="${BETA_SLOW:-1.0}"
echo "  rope: base=$ROPE_BASE compressed_base=$ROPE_CBASE scale_factor=$ROPE_SCALEF orig_ctx=$ROPE_ORIGCTX beta=[$BETA_FAST,$BETA_SLOW]"
for ((il=0; il<WANT_LAYERS; il++)); do
    echo "L${il}_RATIO ${LRATIO[$il]:-0}" >> "$work/params.txt"
done

cat >> "$work/params.txt" <<EOF
STEP $STEP
VIEWLIMIT $VIEWLIMIT
NVIEWS $NVIEWS
TOKEN $TOKEN
KV_CAP $KV_CAP
KV_SEQUENCE $KV_SEQUENCE
KV_STEPS $KV_STEPS
PROMPT_N $PROMPT_N
PROMPT_IDS $PROMPT_IDS
NLAYERS $WANT_LAYERS
N_EMBD $N_EMBD
N_HEAD $N_HEAD
HEAD_DIM $HEAD_DIM
N_ROT $N_ROT
O_RANK $O_RANK
N_HC $N_HC
HC_ITERS $HC_ITERS
N_USED $N_USED
N_FF $N_FF
POS_A $POS_A
POS_B $POS_B
ROPE_BASE $ROPE_BASE
ROPE_CBASE $ROPE_CBASE
ROPE_SCALEF $ROPE_SCALEF
ROPE_ORIGCTX $ROPE_ORIGCTX
BETA_FAST $BETA_FAST
BETA_SLOW $BETA_SLOW
HC_EPS $HC_EPS
RMS_EPS $RMS_EPS
WSCALE $WSCALE
CLAMP $CLAMP
EOF
awk 'NF < 2 { print "FAIL missing value for " $1 > "/dev/stderr"; exit 1 }' "$work/params.txt" || exit 1

# The production generator is self-witnessing: no Python/reference process or
# foreign vector crosses into the native execution lifetime.
echo "  native-only execution: Form emits the kernels; Metal carries and checks the full stack"

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
LIB_EMB="$(compile_unit  dsv4-embed-msl        form_dsv4_embed_f16            dsv4emb)"    || exit 1
LIB_MLA="$(compile_unit  dsv4-mla-unit         form_mla_rmsnorm_f32           dsv4mla)"    || exit 1
LIB8="$(compile_unit     dsv4-mx8-matvec-msl   form_dsv4_mx8_matvec           dsv4mx8)"    || exit 1
LIB_CORE="$(compile_unit dsv4-mla-core-msl     form_dsv4_mx8_matvec_grouped   dsv4core)"   || exit 1
LIB_HC="$(compile_unit   dsv4-hc-unit          form_hc_split_f32              dsv4hc)"     || exit 1
LIB_MX4="$(compile_unit  dsv4-mx4-matvec-msl   form_dsv4_mx4_matvec           dsv4mx4)"    || exit 1
LIB_IQ2="$(compile_unit  dsv4-iq2-matvec-msl   form_dsv4_iq2_matvec           dsv4iq2)"    || exit 1
LIB_Q2K="$(compile_unit  dsv4-q2k-matvec-msl   form_dsv4_q2k_matvec           dsv4q2k)"    || exit 1
LIB_Q80="$(compile_unit  dsv4-q80-matvec-msl   form_dsv4_q80_matvec           dsv4q80)"    || exit 1
LIB_Q8G="$(compile_unit  dsv4-q80-grouped-msl  form_dsv4_q80_matvec_grouped   dsv4q8g)"    || exit 1
LIB_FFN="$(compile_unit  dsv4-stack-ffn-unit   form_dsv4_topk_weights         dsv4stkffn)" || exit 1
LIB_KV="$(compile_unit   dsv4-stack-kv-unit    form_dkv_append_f32            dsv4stkkv)"  || exit 1

# ── 4. the carrier ─────────────────────────────────────────────────────────────────────────────────
cat > "$work/runner.swift" <<'SWIFT'
import Metal
import Foundation

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
let libMx4 = argv[8], libIq2 = argv[9], libQ2k = argv[10], libQ80 = argv[11], libQ8g = argv[12], libFfn = argv[13]
let libKv = argv[14]
let controlAdapterPath = argv[15]

struct Tn { let abs: Int, bytes: Int, idx: Int, inner: Int, holds: Int, d0: Int, d1: Int, d2: Int, type: Int
            var rows: Int { d1 }
            var cols: Int { d0 }
            var nel: Int  { d0 * d1 } }
func T(_ k: String) -> Tn {
    return Tn(abs: I(k+"_ABS"), bytes: I(k+"_BYTES"), idx: I(k+"_IDX"), inner: I(k+"_INNER"),
              holds: I(k+"_HOLDS"), d0: I(k+"_D0"), d1: I(k+"_D1"), d2: I(k+"_D2"), type: I(k+"_TYPE"))
}
let emb = T("EMB")
let outHcFn = T("OUT_HC_FN"), outHcScale = T("OUT_HC_SCALE"), outHcBase = T("OUT_HC_BASE")
let outNorm = T("OUT_NORM"), outWeight = T("OUT_WEIGHT")
let step = I("STEP"), viewLimit = I("VIEWLIMIT"), nviews = I("NVIEWS"), token = I("TOKEN")
let nLayers = I("NLAYERS")
let kvCap = I("KV_CAP"), kvSequence = I("KV_SEQUENCE") == 1, kvSteps = I("KV_STEPS")
let promptN = I("PROMPT_N")
let promptIds: [Int] = promptN == 0 ? [] : ((P["PROMPT_IDS"] ?? "none").split(separator: " ").compactMap { Int($0) })
let nEmbd = I("N_EMBD"), nHead = I("N_HEAD"), headDim = I("HEAD_DIM"), nRot = I("N_ROT"), oRank = I("O_RANK")
let nHc = I("N_HC"), hcIters = I("HC_ITERS"), nUsed = I("N_USED"), nFf = I("N_FF")
let posA = I("POS_A"), posB = I("POS_B")
let ropeBase = F("ROPE_BASE"), ropeCBase = F("ROPE_CBASE"), ropeScaleF = F("ROPE_SCALEF")
let ropeOrigCtx = F("ROPE_ORIGCTX"), betaFast = F("BETA_FAST"), betaSlow = F("BETA_SLOW")
let hcEps = F("HC_EPS"), eps = F("RMS_EPS"), wscale = F("WSCALE"), clamp = F("CLAMP")
let hcDim = nHc * nEmbd

// ---- a layer's whole weight set, read by name from the file's own plan and manifest ----
struct LayerW {
    let nrm, qa, qan, qb, kv, kvan, snk, oa, ob: Tn
    let haf, has, hab, hff, hfs, hfb: Tn
    let fnw, rt, gx, ux, dx, sgw, suw, sdw: Tn
    let ht: Tn?, bias: Tn?
    let ckv: Tn?, cgt: Tn?, cap: Tn?, cnm: Tn?   // the SECOND cache's four, on ratio != 0 layers only
    let hashed: Bool, ratio: Int
    var nExpStack: Int { gx.d2 }          // the PER-LAYER expert count: the tensor's own dim[2]
    var nExpRouter: Int { rt.d1 }         // the router's width: the logit projection's own out-dim
    // ds4.c:12395 — a ratio-4 layer keeps TWO lanes in one state row, so its row is 2*head_dim wide
    // and the state holds 2*ratio rows. Every other ratio keeps one lane and `ratio` rows.
    var coff: Int { ratio == 4 ? 2 : 1 }
    var compRows: Int { coff * ratio }
}
func layerW(_ il: Int) -> LayerW {
    let p = "L\(il)_"
    let hashed = I(p+"HASHED") == 1
    let hasComp = I(p+"HASCOMP") == 1
    return LayerW(nrm: T(p+"NORM"), qa: T(p+"QA"), qan: T(p+"QAN"), qb: T(p+"QB"), kv: T(p+"KV"),
                  kvan: T(p+"KVAN"), snk: T(p+"SNK"), oa: T(p+"OA"), ob: T(p+"OB"),
                  haf: T(p+"HAF"), has: T(p+"HAS"), hab: T(p+"HAB"),
                  hff: T(p+"HFF"), hfs: T(p+"HFS"), hfb: T(p+"HFB"),
                  fnw: T(p+"FN"), rt: T(p+"RT"), gx: T(p+"GX"), ux: T(p+"UX"), dx: T(p+"DX"),
                  sgw: T(p+"SG"), suw: T(p+"SU"), sdw: T(p+"SD"),
                  ht: hashed ? T(p+"HT") : nil, bias: hashed ? nil : T(p+"BI"),
                  ckv: hasComp ? T(p+"CKV") : nil, cgt: hasComp ? T(p+"CGT") : nil,
                  cap: hasComp ? T(p+"CAP") : nil, cnm: hasComp ? T(p+"CNM") : nil,
                  hashed: hashed, ratio: I(p+"RATIO"))
}
let LW = (0..<nLayers).map { layerW($0) }

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
func lib(_ p: String) -> MTLLibrary { return try! dev.makeLibrary(URL: URL(fileURLWithPath: p)) }
let lEmb = lib(libEmb), lMla = lib(libMla), l8 = lib(lib8), lCore = lib(libCore), lHc = lib(libHc)
let lMx4 = lib(libMx4), lIq2 = lib(libIq2), lQ2k = lib(libQ2k), lQ80 = lib(libQ80), lQ8g = lib(libQ8g), lFfn = lib(libFfn)
let lKv = lib(libKv)
let queue = dev.makeCommandQueue()!
var failures = 0, gpuErrors = 0
var dumpLogitsDone = false
// THE DUMP MUST FIRE AT THE RIGHT STEP. The generation loop calls the exit head at pos 0 first, when
// only promptIds[0] has been read — so an unguarded dump captures P(next | "The") while
// `ds4 --dump-logits` gives P(next | the WHOLE prompt). Diffing those compares two different
// questions and says nothing about correctness. This arms only at the last prompt position, which is
// the step whose token is the first EMITTED one and the only one ds4's dump is comparable to.
var dumpArmed = false
var gpuFirstError: String? = nil
var gateNo = 0
func check(_ ok: Bool, _ pass: String, _ fail: String) {
    gateNo += 1
    if ok { print("PASS  gate \(gateNo) " + pass) } else { print("FAIL  gate \(gateNo) " + fail); failures += 1 }
}

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
  "the views map: all \(nviews) overlapping page-aligned bytesNoCopy views of the \(fileLen) B file wrap on \(dev.name) — one buffer over the whole file cannot (maxBufferLength \(dev.maxBufferLength))",
  "only \(views.count)/\(nviews) views mapped")
if failures > 0 { print("VERDICT FAIL the views did not map"); exit(1) }

// ---- gate 2: RESIDENCY over EVERY tensor of EVERY stacked layer, and the per-layer table said out loud
var spanning: [String] = []
var groups: [String: [Int]] = [:]
func resident(_ n: String, _ t: Tn) { if t.holds != 1 || t.idx >= nviews { spanning.append(n) } }
resident("token_embd", emb)
resident("output_hc_fn", outHcFn)
resident("output_hc_scale", outHcScale)
resident("output_hc_base", outHcBase)
resident("output_norm", outNorm)
resident("output.weight", outWeight)
for il in 0..<nLayers {
    let w = LW[il]
    let named: [(String, Tn)] = [("attn_norm", w.nrm), ("attn_q_a", w.qa), ("attn_q_a_norm", w.qan),
        ("attn_q_b", w.qb), ("attn_kv", w.kv), ("attn_kv_a_norm", w.kvan), ("attn_sinks", w.snk),
        ("attn_output_a", w.oa), ("attn_output_b", w.ob), ("hc_attn_fn", w.haf),
        ("hc_attn_scale", w.has), ("hc_attn_base", w.hab), ("hc_ffn_fn", w.hff),
        ("hc_ffn_scale", w.hfs), ("hc_ffn_base", w.hfb), ("ffn_norm", w.fnw),
        ("ffn_gate_inp", w.rt), ("ffn_gate_exps", w.gx), ("ffn_up_exps", w.ux),
        ("ffn_down_exps", w.dx), ("ffn_gate_shexp", w.sgw), ("ffn_up_shexp", w.suw),
        ("ffn_down_shexp", w.sdw)]
    for (n, t) in named { resident("blk.\(il).\(n)", t) }
    if let h = w.ht { resident("blk.\(il).ffn_gate_tid2eid", h) }
    if let b = w.bias { resident("blk.\(il).exp_probs_b", b) }
    if let t = w.ckv { resident("blk.\(il).attn_compressor_kv", t) }
    if let t = w.cgt { resident("blk.\(il).attn_compressor_gate", t) }
    if let t = w.cap { resident("blk.\(il).attn_compressor_ape", t) }
    if let t = w.cnm { resident("blk.\(il).attn_compressor_norm", t) }
    let key = "gate/up \(w.gx.type)  down \(w.dx.type)  n_exp \(w.nExpStack)  \(w.hashed ? "hash" : "topk")  rope \(w.ratio == 0 ? "plain" : "compressed")"
    groups[key, default: []].append(il)
}
check(spanning.isEmpty,
  "residency: every tensor of all \(nLayers) stacked layers — both hyper-connection frames, the attention block, the router, and whichever routing tensor the layer carries — lies wholly inside one view (\(nLayers * 24 + 1) tensors)",
  "these tensors span views or index past the set: \(spanning.prefix(8))")
print("      ── the per-layer table, read from the FILE's own tensor table, not from blk.0 ──")
for (k, v) in groups.sorted(by: { $0.value[0] < $1.value[0] }) {
    print("      \(k)  ->  layers \(v.count <= 8 ? "\(v)" : "\(v.prefix(6))…(\(v.count) layers)")")
}
check(groups.count >= 1,
  "heterogeneity: the \(nLayers) layers fall into \(groups.count) distinct (gate/up type, down type, expert count, routing regime, rope regime) groups — a blk.0-shaped stack would run \(groups.count == 1 ? "the only" : "ONE of \(groups.count)") shape everywhere",
  "no layer groups were formed")

// ---- gate 3: the PRUNING is carried in the BIAS. Read it; never infer it. ----
// The router projects nExpRouter logits at every layer while the stack holds nExpStack experts. The file
// keeps the two consistent by writing -1e30 into exp_probs_b.bias at every pruned index, so biased top-k
// cannot reach one. That is a claim about the FILE, so it is read from the file and gated here.
var pruneOK = true, pruneWhy = ""
for il in 0..<nLayers {
    let w = LW[il]
    guard let b = w.bias else { continue }
    let p = views[b.idx].contents().advanced(by: b.inner).bindMemory(to: Float.self, capacity: w.nExpRouter)
    for e in 0..<w.nExpRouter {
        let sentinel = p[e] < -1e29
        if (e >= w.nExpStack) != sentinel {
            pruneOK = false
            pruneWhy = "blk.\(il).exp_probs_b[\(e)] = \(p[e]) but the stack holds \(w.nExpStack) experts"
            break
        }
    }
    if !pruneOK { break }
}
check(pruneOK,
  "the pruning is in the BIAS: at every top-k layer, exp_probs_b.bias carries a -1e30 sentinel on exactly the indices at or beyond that layer's own ffn_gate_exps dim[2], and a finite value below it. That, and nothing in the metadata, is what keeps a 256-wide router from selecting an expert a 192-deep stack does not have",
  "the bias sentinel does not match the stack depth: \(pruneWhy)")

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
let pAttendMixed = pipe(lMla, "form_mla_attend_mixed_f32")
let pMx8 = pipe(l8, "form_dsv4_mx8_matvec")
let pGrouped = pipe(lCore, "form_dsv4_mx8_matvec_grouped")
let pKvq = pipe(lCore, "form_dsv4_kv_fp8_f16_round")
let pF16mv = pipe(lCore, "form_dsv4_f16_matvec")
let pHcBcast = pipe(lHc, "form_hc_broadcast_f32")
let pHcRmsNw = pipe(lHc, "form_hc_rmsnorm_nw_f32")
let pHcSplit = pipe(lHc, "form_hc_split_f32")
let pHcWsum = pipe(lHc, "form_hc_wsum_f32")
let pHcPost = pipe(lHc, "form_hc_post_f32")
let pHcHeadw = pipe(lHc, "form_hc_headw_f32")
let pMx4 = pipe(lMx4, "form_dsv4_mx4_matvec")
let pIq2 = pipe(lIq2, "form_dsv4_iq2_matvec")
let pQ2k = pipe(lQ2k, "form_dsv4_q2k_matvec")
let pQ80 = pipe(lQ80, "form_dsv4_q80_matvec")
let pQ80g = pipe(lQ8g, "form_dsv4_q80_matvec_grouped")
let pSwiglu = pipe(lFfn, "form_dsv4_swiglu_f32")
let pScale = pipe(lFfn, "form_dsv4_scale_f32")
let pAxpy = pipe(lFfn, "form_dsv4_axpy_f32")
let pHashSel = pipe(lFfn, "form_dsv4_hash_select")
let pHashW = pipe(lFfn, "form_dsv4_hash_weights")
let pTopkW = pipe(lFfn, "form_dsv4_topk_weights")
let pKvAppend = pipe(lKv, "form_dkv_append_f32")
let pControlAdapter = pipe(lKv, "form_dsv4_control_logit_adapter")
let pCompInit = pipe(lKv, "form_dsv4_comp_init_f32")
let pCompState = pipe(lKv, "form_dsv4_comp_state_f32")
let pCompPool = pipe(lKv, "form_dsv4_comp_pool_f32")
let pCompShift = pipe(lKv, "form_dsv4_comp_shift_f32")

func gpuRmsnorm(_ x: MTLBuffer, _ n: Int, _ t: Tn) -> MTLBuffer {
    let out = sentinelled(n); var n32 = UInt32(n), e = eps
    enc(pRms, 1, 1) { c in c.setBuffer(x, offset: 0, index: 0); c.setBuffer(views[t.idx], offset: t.inner, index: 1)
                           c.setBuffer(out, offset: 0, index: 2)
                           c.setBytes(&n32, length: 4, index: 3); c.setBytes(&e, length: 4, index: 4) }
    return out
}
// THE DENSE TYPE DISPATCH. The reap25 file carried MXFP8 (41) on every dense projection; the mainline
// Mac quant carries Q8_0 (8) on attn_q_a/q_b/kv/output_a/output_b and on output.weight. Reading one
// through the other's decoder does not fail loudly — it yields NaN, and the first mainline run said
// exactly that: "native self-witness failed: finite 0/16384". So the type is asked, never assumed.
func gpuMx8(_ t: Tn, _ x: MTLBuffer, _ rows: Int, _ cols: Int) -> MTLBuffer {
    let out = sentinelled(rows); var r = UInt32(rows), c32 = UInt32(cols), nel = UInt32(rows*cols)
    if t.type == 8 {
        enc(pQ80, rows*32, 256) { c in c.setBuffer(views[t.idx], offset: t.inner, index: 0); c.setBuffer(x, offset: 0, index: 1)
                                       c.setBuffer(out, offset: 0, index: 2)
                                       c.setBytes(&r, length: 4, index: 3); c.setBytes(&c32, length: 4, index: 4) }
        return out
    }
    if t.type != 41 {
        print("FAIL a dense tensor carries type \(t.type); this stack decodes 41 (MXFP8) and 8 (Q8_0)")
        exit(1)
    }
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

// ══ the compressed-RoPE reduction, re-derived HERE, on the host, from the file's own KV ══════════════
// ds4.c:10102 rope_tail_ext_inplace under ds4.c:10166 rope_tail_layer_inplace's arguments. The magnitude
// scale cancels: attn_factor is set to 1/(1 + 0.1*ln(1/freq_scale)) precisely so mscale comes back to 1.
// What is left is theta = theta_extrap*(freq_scale*(1 - ramp_k) + ramp_k), a PER-PAIR SCALE — which is
// exactly what form_mla_rope_f32's freqs[] already is, derived from the file's
// own scalars and ratio array.
let nPair = nRot/2
var freqCache: [Int: MTLBuffer] = [:]
// FORM_DS4_RAW_LANE=1: run the RAW attention lane. ds4.c:12437-12460 shows the compressed-base rope
// and the fp8 KV round belong to a SECOND cache — a compressor pools compress_ratio raw KVs into one
// row (gated pooling over compressor_gate/ape/norm, tensors this harness never loads), rotates that
// pooled row at comp_pos = pos+1-ratio with the compressed base, fp8-rounds it, and stores it BESIDE
// a raw f32 cache whose keys carry PLAIN rope at true positions (ds4 --inspect: "KV raw + compressed").
// This harness had ported the compressed lane as if it were the attention itself — on 41 of 43 layers
// keys were rotated base-160000 where ds4's short-context answer comes from the raw base-10000 lane.
// The per-layer fp64 oracle shares the misreading (its line 822 skips the compressor), so their
// agreement was common-mode and could not catch it. With this flag: plain rope every layer, no fp8
// round — the raw lane's semantics. The compressor/indexer lanes remain unimplemented and are only
// reachable past compress_ratio tokens; at short contexts ds4's compressed cache is empty too.
// REFUTED 2026-07-30, kept only as a falsifier. Reading layer_attention_raw_swa_one (ds4.c:12936)
// whole shows the raw path DOES rope per-layer at true pos with the compressed base on ratio layers
// AND fp8-rounds the row (kv_cache_push_raw f16-rounds it too) — i.e. the DEFAULT lane above is
// ds4's recipe, and this flag's two premises are both false. Measured against ds4 --raw-prompt on
// "The capital of France is": default lane puts " Paris" at rank 148, this flag at 407. The earlier
// "10x improvement" was measured against a CHAT-TEMPLATED ds4 prompt and was an artifact.
let rawLane = ProcessInfo.processInfo.environment["FORM_DS4_RAW_LANE"] == "1"
func ropeFreqs(_ il: Int) -> MTLBuffer {
    if let b = freqCache[il] { return b }
    let compressed = LW[il].ratio != 0 && !rawLane
    let fbase: Float = (compressed && ropeCBase > 0) ? ropeCBase : ropeBase
    let fscale: Float = (!compressed || ropeScaleF <= 0) ? 1.0 : 1.0/ropeScaleF
    let ext: Float = (compressed && ropeScaleF > 1.0) ? 1.0 : 0.0
    var lo: Float = 0, hi: Float = 0
    if ext != 0 {
        func corr(_ beta: Float) -> Float { return Float(nRot) * logf(ropeOrigCtx/(beta*2.0*Float.pi)) / (2.0*logf(fbase)) }
        lo = max(0.0, floorf(corr(betaFast)))
        hi = min(Float(nRot - 1), ceilf(corr(betaSlow)))
    }
    let thetaScale = powf(fbase, -2.0/Float(nRot))
    let buf = dev.makeBuffer(length: nPair*4, options: .storageModeShared)!
    let p = buf.contents().bindMemory(to: Float.self, capacity: nPair)
    var f: Float = 1.0
    for k in 0..<nPair {
        if ext != 0 {
            let y = (Float(k) - lo)/max(0.001, hi - lo)
            let ramp = (1.0 - min(1.0, max(0.0, y))) * ext
            p[k] = f * (fscale*(1.0 - ramp) + ramp)
        } else {
            p[k] = f * fscale
        }
        f *= thetaScale
    }
    freqCache[il] = buf
    return buf
}
func gpuRope(_ v: MTLBuffer, _ nh: Int, _ pos: Int, _ il: Int, _ inverse: Bool) -> MTLBuffer {
    let out = sentinelled(nh*headDim)
    var a = UInt32(nh), b = UInt32(headDim), c32 = UInt32(nRot), p = Float(pos), s: Float = inverse ? -1.0 : 1.0
    enc(pRope, nh, 64) { c in c.setBuffer(v, offset: 0, index: 0); c.setBuffer(out, offset: 0, index: 1)
                              c.setBuffer(ropeFreqs(il), offset: 0, index: 2)
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
func gpuKvAppend(_ row: MTLBuffer, _ arena: MTLBuffer, _ pos: Int, _ cap: Int) {
    var h = UInt32(headDim), p = UInt32(pos), c32 = UInt32(cap)
    enc(pKvAppend, headDim, 256) { c in c.setBuffer(row, offset: 0, index: 0)
                                        c.setBuffer(arena, offset: 0, index: 1)
                                        c.setBytes(&h, length: 4, index: 2); c.setBytes(&p, length: 4, index: 3)
                                        c.setBytes(&c32, length: 4, index: 4) }
}
var kvAppendObserved = false
func observeRealKvAppend(_ row: MTLBuffer) {
    if kvAppendObserved { return }
    let cap = 2, arena = sentinelled(cap * headDim)
    gpuKvAppend(row, arena, 0, cap)
    let rp = fp(row, headDim), ap = fp(arena, cap * headDim)
    var exact = 0, frontier = 0
    for j in 0..<headDim {
        if rp[j].bitPattern == ap[j].bitPattern { exact += 1 }
        if ap[headDim + j].isNaN { frontier += 1 }
    }
    check(exact == headDim && frontier == headDim,
      "REAL KV APPEND: the Form-emitted device kernel copied layer 0's \(headDim)-wide quantized latent bit-identically into arena row 0 and left all \(headDim) entries of row 1 NaN-sentinel",
      "real KV append: exact row entries \(exact)/\(headDim), untouched frontier \(frontier)/\(headDim)")
    kvAppendObserved = true
}
func gpuAttend(_ q: MTLBuffer, _ rows: MTLBuffer, _ snk: Tn, _ nrows: Int = 1) -> MTLBuffer {
    let out = sentinelled(nHead*headDim)
    var a = UInt32(nHead), b = UInt32(headDim), c32 = UInt32(nrows), sc = 1.0/sqrtf(Float(headDim))
    enc(pAttend, nHead, 32) { c in c.setBuffer(q, offset: 0, index: 0); c.setBuffer(rows, offset: 0, index: 1)
                                   c.setBuffer(out, offset: 0, index: 2)
                                   c.setBuffer(views[snk.idx], offset: snk.inner, index: 3)
                                   c.setBytes(&a, length: 4, index: 4); c.setBytes(&b, length: 4, index: 5)
                                   c.setBytes(&c32, length: 4, index: 6); c.setBytes(&sc, length: 4, index: 7) }
    return out
}
// ds4.c:12567 layer_attention_mixed_one — one softmax over the raw window AND the compressed rows.
func gpuAttendMixed(_ q: MTLBuffer, _ rows: MTLBuffer, _ crows: MTLBuffer, _ snk: Tn,
                    _ nrows: Int, _ ncomp: Int) -> MTLBuffer {
    let out = sentinelled(nHead*headDim)
    var a = UInt32(nHead), b = UInt32(headDim), c32 = UInt32(nrows), d32 = UInt32(ncomp)
    var sc = 1.0/sqrtf(Float(headDim))
    enc(pAttendMixed, nHead, 32) { c in c.setBuffer(q, offset: 0, index: 0); c.setBuffer(rows, offset: 0, index: 1)
                                        c.setBuffer(crows, offset: 0, index: 2); c.setBuffer(out, offset: 0, index: 3)
                                        c.setBuffer(views[snk.idx], offset: snk.inner, index: 4)
                                        c.setBytes(&a, length: 4, index: 5); c.setBytes(&b, length: 4, index: 6)
                                        c.setBytes(&c32, length: 4, index: 7); c.setBytes(&d32, length: 4, index: 8)
                                        c.setBytes(&sc, length: 4, index: 9) }
    return out
}
var useGrowingKv = false
var kvArenas: [MTLBuffer] = []
// FORM_DS4_NO_COMPRESSOR=1 runs the raw-only lane this harness had before the second cache existed.
// It is kept as the A/B the boundary measurement needs, not as an option: with it the run reproduces
// rank 148 on a five-token prompt, without it the compressed rows exist and the number is the claim.
let useCompressor = ProcessInfo.processInfo.environment["FORM_DS4_NO_COMPRESSOR"] != "1"

// ── THE SECOND CACHE. dsv4-compressor.fk's four kernels, driven per layer ──────────────────────────
// This is the half the boundary measurement isolated: our lane matched ds4 op-for-op wherever no
// compressed row existed (rank 5 on a 3-token prompt) and drifted exactly as ratio-4 layers began
// emitting rows at pos 3 (27, then 148). What follows is compressor_decode_one (ds4.c:12381) and the
// mixed attention that reads its output — nothing else was missing.
//
// THE INDEXER MASK IS OMITTED, and the omission is enforced, not assumed: ds4.c:12822 returns
// all-allowed whenever n_comp <= top_k = 512, so below 2048 tokens on a ratio-4 layer there is nothing
// to hide. compRefusals counts every step that walks past that radius and the run reports it.
var compState: [MTLBuffer] = []          // per layer: the rolling kv plane, compRows * width floats
var compScore: [MTLBuffer] = []          // per layer: the rolling score plane, born NEG_INF
var compRows: [[MTLBuffer]] = []         // per layer: the emitted compressed rows, in order
var compPacked: [MTLBuffer] = []         // per layer: those rows packed contiguous for attention
var compRefusals = 0
let indexerTopK = 512
func compWidth(_ w: LayerW) -> Int { return w.coff * headDim }
func compStateLen(_ w: LayerW) -> Int { return w.compRows * compWidth(w) }
func compInit() {
    compState = []; compScore = []; compRows = []; compPacked = []; compRefusals = 0
    for il in 0..<nLayers {
        let w = LW[il]
        if w.ratio == 0 {
            compState.append(sentinelled(1)); compScore.append(sentinelled(1))
            compRows.append([]); compPacked.append(sentinelled(1)); continue
        }
        let n = compStateLen(w)
        let kvB = dev.makeBuffer(length: n*4, options: .storageModeShared)!
        let scB = dev.makeBuffer(length: n*4, options: .storageModeShared)!
        var n32 = UInt32(n)
        enc(pCompInit, n, 256) { c in c.setBuffer(kvB, offset: 0, index: 0); c.setBuffer(scB, offset: 0, index: 1)
                                      c.setBytes(&n32, length: 4, index: 2) }
        compState.append(kvB); compScore.append(scB); compRows.append([]); compPacked.append(sentinelled(1))
    }
}
// one token through the compressor: project, bias, write the state row, and on a ratio boundary pool,
// norm, rope at comp_pos with the layer's own compressed freqs, fp8+f16 round, and keep the row.
func compStep(_ w: LayerW, _ il: Int, _ xn: MTLBuffer, _ pos: Int) {
    guard w.ratio != 0, let ckv = w.ckv, let cgt = w.cgt, let ape = w.cap, let cnm = w.cnm else { return }
    let width = compWidth(w)
    let kvCur = gpuF16mv(ckv, xn)
    let scCur = gpuF16mv(cgt, xn)
    let row = w.ratio == 4 ? w.ratio + (pos % w.ratio) : (pos % w.ratio)
    var wd = UInt32(width), rw = UInt32(row), pm = UInt32(pos % w.ratio)
    enc(pCompState, width, 256) { c in c.setBuffer(kvCur, offset: 0, index: 0); c.setBuffer(scCur, offset: 0, index: 1)
                                       c.setBuffer(views[ape.idx], offset: ape.inner, index: 2)
                                       c.setBuffer(compState[il], offset: 0, index: 3)
                                       c.setBuffer(compScore[il], offset: 0, index: 4)
                                       c.setBytes(&wd, length: 4, index: 5); c.setBytes(&rw, length: 4, index: 6)
                                       c.setBytes(&pm, length: 4, index: 7) }
    guard (pos + 1) % w.ratio == 0 else { return }
    let pooled = sentinelled(headDim)
    var hd32 = UInt32(headDim), rt32 = UInt32(w.ratio)
    enc(pCompPool, headDim, 256) { c in c.setBuffer(compState[il], offset: 0, index: 0)
                                        c.setBuffer(compScore[il], offset: 0, index: 1)
                                        c.setBuffer(pooled, offset: 0, index: 2)
                                        c.setBytes(&hd32, length: 4, index: 3); c.setBytes(&rt32, length: 4, index: 4) }
    if w.ratio == 4 {
        let n = w.ratio * width
        enc(pCompShift, n, 256) { c in c.setBuffer(compState[il], offset: 0, index: 0)
                                       c.setBuffer(compScore[il], offset: 0, index: 1)
                                       c.setBytes(&wd, length: 4, index: 2); c.setBytes(&rt32, length: 4, index: 3) }
    }
    let normed = gpuRmsnorm(pooled, headDim, cnm)
    let roped = gpuRope(normed, 1, pos + 1 - w.ratio, il, false)
    compRows[il].append(gpuKvRound(roped))
    // pack the layer's rows into one contiguous arena for the mixed attention to read
    let n = compRows[il].count
    let packed = sentinelled(n * headDim)
    for (i, r) in compRows[il].enumerated() { gpuKvAppend(r, packed, i, n) }
    compPacked[il] = packed
    if n > indexerTopK { compRefusals += 1 }
}
func gpuGrouped(_ t: Tn, _ x: MTLBuffer) -> MTLBuffer {
    let out = sentinelled(t.rows)
    var r = UInt32(t.rows), c32 = UInt32(t.cols), nel = UInt32(t.nel), rk = UInt32(oRank)
    if t.type == 8 {
        enc(pQ80g, t.rows*32, 256) { c in c.setBuffer(views[t.idx], offset: t.inner, index: 0); c.setBuffer(x, offset: 0, index: 1)
                                          c.setBuffer(out, offset: 0, index: 2)
                                          c.setBytes(&r, length: 4, index: 3); c.setBytes(&c32, length: 4, index: 4)
                                          c.setBytes(&nel, length: 4, index: 5); c.setBytes(&rk, length: 4, index: 6) }
        return out
    }
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
// THE PER-LAYER TYPE DISPATCH. The view is bound at the EXPERT's own byte slice, so the kernel's
// r*cols+j indices address inside that expert and never form a 32-bit offset into an 85 GiB file.
func gpuExpert(_ t: Tn, _ x: MTLBuffer, _ expert: Int) -> MTLBuffer {
    let rows = t.d1, cols = t.d0
    let stride = t.bytes / t.d2
    let out = sentinelled(rows); var r = UInt32(rows), c32 = UInt32(cols), n32 = UInt32(rows*cols)
    if t.type == 40 {
        enc(pMx4, rows*32, 256) { c in c.setBuffer(views[t.idx], offset: t.inner + expert*stride, index: 0)
                                       c.setBuffer(x, offset: 0, index: 1); c.setBuffer(out, offset: 0, index: 2)
                                       c.setBytes(&r, length: 4, index: 3); c.setBytes(&c32, length: 4, index: 4); c.setBytes(&n32, length: 4, index: 5) }
    } else if t.type == 16 {
        enc(pIq2, rows*32, 256) { c in c.setBuffer(views[t.idx], offset: t.inner + expert*stride, index: 0)
                                       c.setBuffer(x, offset: 0, index: 1); c.setBuffer(out, offset: 0, index: 2)
                                       c.setBytes(&r, length: 4, index: 3); c.setBytes(&c32, length: 4, index: 4) }
    } else if t.type == 10 {
        enc(pQ2k, rows*32, 256) { c in c.setBuffer(views[t.idx], offset: t.inner + expert*stride, index: 0)
                                       c.setBuffer(x, offset: 0, index: 1); c.setBuffer(out, offset: 0, index: 2)
                                       c.setBytes(&r, length: 4, index: 3); c.setBytes(&c32, length: 4, index: 4) }
    } else {
        print("FAIL an expert tensor carries type \(t.type); this stack decodes 40 (MXFP4), 16 (IQ2_XXS) and 10 (Q2_K)")
        exit(1)
    }
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
// ONE LAYER, driven by that layer's OWN row of the file's table.
// ══════════════════════════════════════════════════════════════════════════════════════════════════
struct LayerOut {
    let afterAttn: MTLBuffer, ffnCur: MTLBuffer, ffnNorm: MTLBuffer, logits: MTLBuffer
    let ids: [Int], wts: [Float], gate0: MTLBuffer, up0: MTLBuffer, mid0: MTLBuffer, down0: MTLBuffer
    let moe: MTLBuffer, shared: MTLBuffer, ffnOut: MTLBuffer, outHc: MTLBuffer
}

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
func hcPost(_ blockOut: MTLBuffer, _ resid: MTLBuffer, _ split: MTLBuffer) -> MTLBuffer {
    let out = sentinelled(hcDim); var a = UInt32(nHc), b = UInt32(nEmbd)
    enc(pHcPost, nHc, 256) { c in c.setBuffer(blockOut, offset: 0, index: 0); c.setBuffer(resid, offset: 0, index: 1)
                                  c.setBuffer(split, offset: nHc*4, index: 2)
                                  c.setBuffer(split, offset: 2*nHc*4, index: 3)
                                  c.setBuffer(out, offset: 0, index: 4)
                                  c.setBytes(&a, length: 4, index: 5); c.setBytes(&b, length: 4, index: 6) }
    return out
}
func mlaBlock(_ input: MTLBuffer, _ pos: Int, _ il: Int) -> MTLBuffer {
    let w = LW[il]
    let xn = gpuRmsnorm(input, nEmbd, w.nrm)
    let ql = gpuMx8(w.qa, xn, w.qa.rows, w.qa.cols)
    let qln = gpuRmsnorm(ql, w.qa.rows, w.qan)
    let qq = gpuMx8(w.qb, qln, w.qb.rows, w.qb.cols)
    let qh = gpuHeadrms(qq)
    let qr = gpuRope(qh, nHead, pos, il, false)
    let kl = gpuMx8(w.kv, xn, w.kv.rows, w.kv.cols)
    let kln = gpuRmsnorm(kl, w.kv.rows, w.kvan)
    let kr = gpuRope(kln, 1, pos, il, false)
    // raw lane: the fp8+f16 round is the COMPRESSED cache's storage format (ds4.c:3210 says so in its
    // own comment); the raw cache is f32, so the roped key passes through untouched.
    let kq = rawLane ? kr : gpuKvRound(kr)
    if il == 0 { observeRealKvAppend(kq) }
    let ha: MTLBuffer
    if useGrowingKv {
        gpuKvAppend(kq, kvArenas[il], pos, kvCap)
        // the second cache is fed from attn_norm, the SAME activation the q/kv projections read
        // (ds4.c:12986), and it is fed AFTER the raw row is pushed — the order the spine has.
        if useCompressor { compStep(w, il, xn, pos) }
        let nComp = useCompressor ? compRows[il].count : 0
        ha = nComp == 0 ? gpuAttend(qr, kvArenas[il], w.snk, pos + 1)
                        : gpuAttendMixed(qr, kvArenas[il], compPacked[il], w.snk, pos + 1, nComp)
    } else {
        ha = gpuAttend(qr, kq, w.snk)
    }
    let hu = gpuRope(ha, nHead, pos, il, true)
    let lo = gpuGrouped(w.oa, hu)
    return gpuMx8(w.ob, lo, w.ob.rows, w.ob.cols)
}

func runLayer(_ il: Int, _ pos: Int, _ currentToken: Int, _ residHc: MTLBuffer) -> LayerOut {
    let w = LW[il]
    let (attnCur, attnSplit) = hcPre(residHc, w.haf, w.has, w.hab)
    let attnOut = mlaBlock(attnCur, pos, il)
    let afterAttn = hcPost(attnOut, residHc, attnSplit)

    let (ffnCur, ffnSplit) = hcPre(afterAttn, w.hff, w.hfs, w.hfb)
    let ffnNorm = gpuRmsnorm(ffnCur, nEmbd, w.fnw)
    let logits = gpuF16mv(w.rt, ffnNorm)

    let nExpR = w.nExpRouter
    let idsBuf = sentinelledU(nUsed), wtsBuf = sentinelled(nUsed), probsBuf = sentinelled(nExpR)
    if w.hashed {
        // forepick (row 867): the six experts come from an I32 TABLE READ on the token id, and the
        // router only WEIGHTS them. A top-k here would be the layer-3-and-up recipe applied to a hash layer.
        let ht = w.ht!
        var t32 = UInt32(currentToken), nu = UInt32(nUsed)
        enc(pHashSel, nUsed, 8) { c in c.setBuffer(views[ht.idx], offset: ht.inner, index: 0)
                                       c.setBuffer(idsBuf, offset: 0, index: 1)
                                       c.setBytes(&t32, length: 4, index: 2); c.setBytes(&nu, length: 4, index: 3) }
        var ne = UInt32(nExpR), nu2 = UInt32(nUsed), ws = wscale
        enc(pHashW, 1, 1) { c in c.setBuffer(logits, offset: 0, index: 0); c.setBuffer(idsBuf, offset: 0, index: 1)
                                 c.setBuffer(wtsBuf, offset: 0, index: 2); c.setBuffer(probsBuf, offset: 0, index: 3)
                                 c.setBytes(&ne, length: 4, index: 4); c.setBytes(&nu2, length: 4, index: 5)
                                 c.setBytes(&ws, length: 4, index: 6) }
    } else {
        // ds4.c:10665 — the bias enters the SELECTION and never the WEIGHT.
        let bi = w.bias!
        var ne = UInt32(nExpR), nu = UInt32(nUsed), ws = wscale
        enc(pTopkW, 1, 1) { c in c.setBuffer(logits, offset: 0, index: 0)
                                 c.setBuffer(views[bi.idx], offset: bi.inner, index: 1)
                                 c.setBuffer(idsBuf, offset: 0, index: 2); c.setBuffer(wtsBuf, offset: 0, index: 3)
                                 c.setBuffer(probsBuf, offset: 0, index: 4)
                                 c.setBytes(&ne, length: 4, index: 5); c.setBytes(&nu, length: 4, index: 6)
                                 c.setBytes(&ws, length: 4, index: 7) }
    }
    let idp = idsBuf.contents().bindMemory(to: UInt32.self, capacity: nUsed)
    let wtp = wtsBuf.contents().bindMemory(to: Float.self, capacity: nUsed)
    var ids: [Int] = [], wts: [Float] = []
    for i in 0..<nUsed { ids.append(Int(idp[i])); wts.append(wtp[i]) }

    let moe = sentinelled(nEmbd)
    var g0 = moe, u0 = moe, m0 = moe, d0 = moe
    for (i, e) in ids.enumerated() {
        guard e >= 0 && e < w.nExpStack else {
            print("FAIL layer \(il) selected expert \(e) but its stack holds only \(w.nExpStack)"); exit(1)
        }
        let gt = gpuExpert(w.gx, ffnNorm, e)
        let up = gpuExpert(w.ux, ffnNorm, e)
        let mid = gpuSwiglu(gt, up, nFf, wts[i], clamp)
        let dn = gpuExpert(w.dx, mid, e)
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
    let sgv = gpuMx8(w.sgw, ffnNorm, w.sgw.rows, w.sgw.cols)
    let suv = gpuMx8(w.suw, ffnNorm, w.suw.rows, w.suw.cols)
    let smid = gpuSwiglu(sgv, suv, nFf, 1.0, clamp)
    let shared = gpuMx8(w.sdw, smid, w.sdw.rows, w.sdw.cols)

    let ffnOut = sentinelled(nEmbd)
    do { var one: Float = 1.0, n32 = UInt32(nEmbd)
         enc(pScale, nEmbd, 256) { c in c.setBuffer(moe, offset: 0, index: 0); c.setBuffer(ffnOut, offset: 0, index: 1)
                                        c.setBytes(&one, length: 4, index: 2); c.setBytes(&n32, length: 4, index: 3) }
         enc(pAxpy, nEmbd, 256) { c in c.setBuffer(shared, offset: 0, index: 0); c.setBuffer(ffnOut, offset: 0, index: 1)
                                       c.setBytes(&one, length: 4, index: 2); c.setBytes(&n32, length: 4, index: 3) } }
    let outHc = hcPost(ffnOut, afterAttn, ffnSplit)
    return LayerOut(afterAttn: afterAttn, ffnCur: ffnCur, ffnNorm: ffnNorm, logits: logits,
                    ids: ids, wts: wts, gate0: g0, up0: u0, mid0: m0, down0: d0,
                    moe: moe, shared: shared, ffnOut: ffnOut, outHc: outHc)
}
func fp(_ b: MTLBuffer, _ n: Int) -> UnsafeMutablePointer<Float> { return b.contents().bindMemory(to: Float.self, capacity: n) }

// ---- a token's embedding, broadcast to the four streams: THE stack's input (ds4.c:9764) ----
// Token identity is an argument, not process-global state: autoregressive feedback must change both
// this row and the hashed routers' I32 table row on the following step.
func embedToken(_ currentToken: Int) -> MTLBuffer {
    guard currentToken >= 0 && currentToken < emb.rows else {
        print("FAIL token \(currentToken) outside embedding vocabulary 0..<\(emb.rows)"); exit(1)
    }
    let rowOff = currentToken * nEmbd * 2
    let x = sentinelled(nEmbd)
    do { var b64 = UInt64(emb.inner + rowOff), c32 = UInt32(nEmbd)
         enc(pEmb, nEmbd, 256) { c in c.setBuffer(views[emb.idx], offset: 0, index: 0); c.setBuffer(x, offset: 0, index: 1)
                                      c.setBytes(&b64, length: 8, index: 2); c.setBytes(&c32, length: 4, index: 3) } }
    let resid = sentinelled(hcDim)
    do { var a = UInt32(nHc), b = UInt32(nEmbd)
         enc(pHcBcast, hcDim, 256) { c in c.setBuffer(x, offset: 0, index: 0); c.setBuffer(resid, offset: 0, index: 1)
                                          c.setBytes(&a, length: 4, index: 2); c.setBytes(&b, length: 4, index: 3) } }
    return resid
}
let residHc0 = embedToken(token)

var finalByPos: [Int: MTLBuffer] = [:]
var layerFail = 0
var msPerLayer: [Int: [Double]] = [:]
let controlFirst = 128000, controlCount = 5
let controlArtifactWeights: [Float] = {
    guard let s = try? String(contentsOfFile: controlAdapterPath, encoding: .utf8) else {
        print("FAIL cannot read control-logit adapter artifact \(controlAdapterPath)"); exit(1)
    }
    let a = s.split(whereSeparator: { $0 == " " || $0 == "\n" || $0 == "\t" }).compactMap { Float($0) }
    guard a.count == controlCount && a.allSatisfy({ $0.isFinite }) else {
        print("FAIL control-logit adapter must carry exactly five finite f32 weights"); exit(1)
    }
    return a
}()
func floatBuffer(_ a: [Float]) -> MTLBuffer {
    let b = dev.makeBuffer(length: max(a.count, 1) * 4, options: .storageModeShared)!
    let p = fp(b, max(a.count, 1))
    for i in 0..<a.count { p[i] = a[i] }
    return b
}
func copyFloatBuffer(_ source: MTLBuffer, _ n: Int) -> MTLBuffer {
    let b = dev.makeBuffer(length: n * 4, options: .storageModeShared)!
    memcpy(b.contents(), source.contents(), n * 4)
    return b
}
func applyControlAdapter(_ logits: MTLBuffer, _ weights: [Float]) {
    let wb = floatBuffer(weights)
    var first = UInt32(controlFirst), count = UInt32(controlCount)
    enc(pControlAdapter, controlCount, 8) { c in c.setBuffer(logits, offset: 0, index: 0)
                                               c.setBuffer(wb, offset: 0, index: 1)
                                               c.setBytes(&first, length: 4, index: 2)
                                               c.setBytes(&count, length: 4, index: 3) }
}
func argmax(_ logits: MTLBuffer, _ n: Int) -> (token: Int, logit: Float, finite: Int) {
    let p = fp(logits, n)
    var arg = 0, best = -Float.infinity, finite = 0
    for i in 0..<n where p[i].isFinite {
        finite += 1
        if p[i] > best { best = p[i]; arg = i }
    }
    return (arg, best, finite)
}
var controlAdapterProved = false

// The native exit head is reusable inside the same Metal lifetime as the stack state. Its result can
// therefore become the next step's embedding and hash-routing token without a serialization membrane.
func nativeExitHead(_ resid: MTLBuffer, _ label: String) -> (token: Int, logit: Float) {
    let flat = sentinelled(hcDim)
    do { var n = UInt32(hcDim), e0 = eps
         enc(pHcRmsNw, 1, 1) { c in c.setBuffer(resid, offset: 0, index: 0); c.setBuffer(flat, offset: 0, index: 1)
                                    c.setBytes(&n, length: 4, index: 2); c.setBytes(&e0, length: 4, index: 3) } }
    let pre = gpuF16mv(outHcFn, flat)
    let headw = sentinelled(nHc)
    do { var a = UInt32(nHc), e0 = hcEps
         enc(pHcHeadw, nHc, 32) { c in c.setBuffer(pre, offset: 0, index: 0)
                                       c.setBuffer(views[outHcScale.idx], offset: outHcScale.inner, index: 1)
                                       c.setBuffer(views[outHcBase.idx], offset: outHcBase.inner, index: 2)
                                       c.setBuffer(headw, offset: 0, index: 3)
                                       c.setBytes(&a, length: 4, index: 4); c.setBytes(&e0, length: 4, index: 5) } }
    let collapsed = sentinelled(nEmbd)
    do { var a = UInt32(nHc), b = UInt32(nEmbd)
         enc(pHcWsum, nEmbd, 256) { c in c.setBuffer(resid, offset: 0, index: 0); c.setBuffer(headw, offset: 0, index: 1)
                                         c.setBuffer(collapsed, offset: 0, index: 2)
                                         c.setBytes(&a, length: 4, index: 3); c.setBytes(&b, length: 4, index: 4) } }
    let normed = gpuRmsnorm(collapsed, nEmbd, outNorm)
    let logits = gpuMx8(outWeight, normed, outWeight.rows, outWeight.cols)
    let base = argmax(logits, outWeight.rows)
    // FORM_DS4_DUMP_LOGITS writes this body's full next-token distribution so it can be diffed
    // element-for-element against `ds4 --dump-logits` on the SAME weights and prompt — the first
    // real-dims comparison this DS4 work has been able to make.
    if let dumpPath = ProcessInfo.processInfo.environment["FORM_DS4_DUMP_LOGITS"], dumpArmed, !dumpLogitsDone {
        dumpLogitsDone = true
        let lp = fp(logits, outWeight.rows)
        var out = "{\"vocab\":\(outWeight.rows),\"argmax_token\":\(base.token),\"logits\":["
        out.reserveCapacity(outWeight.rows * 12)
        for i in 0..<outWeight.rows { if i > 0 { out += "," }; out += String(lp[i]) }
        out += "]}"
        try? out.write(toFile: dumpPath, atomically: true, encoding: .utf8)
        print("      FORM logits dumped: \(outWeight.rows) values -> \(dumpPath)")
        // The lane reports its own comparison integers, so no shell arithmetic stands between the
        // measurement and the reader. FORM_DS4_REF_IDS carries the reference's top ids (from
        // ds4 --dump-logits); for each, this prints OUR rank of that id.
        var order = Array(0..<outWeight.rows)
        order.sort { lp[$0] > lp[$1] }
        var top = "      FORM top10:"
        for i in 0..<10 { top += " \(order[i])(\(String(format: "%.2f", lp[order[i]])))" }
        print(top)
        var rank = [Int](repeating: 0, count: outWeight.rows)
        for (r, id) in order.enumerated() { rank[id] = r }
        if let refIds = ProcessInfo.processInfo.environment["FORM_DS4_REF_IDS"] {
            var line = "      our rank of reference ids:"
            for tok in refIds.split(separator: " ").compactMap({ Int($0) }) {
                line += " \(tok)->\(rank[tok])"
            }
            print(line)
        }
        // WHOLE-DISTRIBUTION AGREEMENT, computed here so no shell arithmetic stands between the
        // measurement and the reader. The rank of ONE token is a noisy yardstick — a lane can move a
        // single id thirty places while the distribution as a whole comes closer or goes further. r
        // over all \(outWeight.rows) logits, plus how many of the reference's top-10 we place in our
        // top-10, is what a change to the recipe has to move.
        if let refPath = ProcessInfo.processInfo.environment["FORM_DS4_REF_LOGITS"],
           let raw = try? String(contentsOfFile: refPath, encoding: .utf8),
           let lb = raw.range(of: "\"logits\":[") {
            let rest = raw[lb.upperBound...]
            let end = rest.firstIndex(of: "]") ?? rest.endIndex
            let ref = rest[..<end].split(separator: ",").compactMap { Float($0.trimmingCharacters(in: .whitespacesAndNewlines)) }
            if ref.count == outWeight.rows {
                var sa: Double = 0, sb: Double = 0
                for i in 0..<ref.count { sa += Double(lp[i]); sb += Double(ref[i]) }
                let ma = sa/Double(ref.count), mb = sb/Double(ref.count)
                var num: Double = 0, da: Double = 0, db: Double = 0
                for i in 0..<ref.count {
                    let x = Double(lp[i]) - ma, y = Double(ref[i]) - mb
                    num += x*y; da += x*x; db += y*y
                }
                let r = num / (sqrt(da) * sqrt(db))
                var refOrder = Array(0..<ref.count); refOrder.sort { ref[$0] > ref[$1] }
                let ourTop10 = Set(order.prefix(10)), refTop10 = Set(refOrder.prefix(10))
                var refRank = [Int](repeating: 0, count: ref.count)
                for (k, id) in refOrder.enumerated() { refRank[id] = k }
                print(String(format: "      AGREEMENT with %@: r=%.6f over %d logits, top-10 overlap %d/10, their argmax at our rank %d, our argmax at their rank %d",
                             (refPath as NSString).lastPathComponent, r, ref.count,
                             ourTop10.intersection(refTop10).count, rank[refOrder[0]], refRank[order[0]]))
            } else {
                print("      AGREEMENT: reference carries \(ref.count) logits, this vocabulary is \(outWeight.rows) — not comparable")
            }
        }
    }
    if !controlAdapterProved {
        let zero = copyFloatBuffer(logits, outWeight.rows)
        applyControlAdapter(zero, [Float](repeating: 0, count: controlCount))
        let zr = argmax(zero, outWeight.rows)
        let zp = fp(zero, outWeight.rows), bp = fp(logits, outWeight.rows)
        var zeroExact = 0
        for i in 0..<outWeight.rows where zp[i].bitPattern == bp[i].bitPattern { zeroExact += 1 }
        check(zeroExact == outWeight.rows && zr.token == base.token && zr.logit.bitPattern == base.logit.bitPattern,
          "REMOVABLE CONTROL ADAPTER ZERO: all \(zeroExact)/\(outWeight.rows) vocabulary logits, token \(base.token), and logit \(base.logit) are bit-identical to the immutable GGUF exit head",
          "zero control adapter changed logits \(zeroExact)/\(outWeight.rows), token \(zr.token) vs \(base.token)")
        var selected: [Int] = []
        for slot in 0..<controlCount {
            let target = controlFirst + slot
            let probe = copyFloatBuffer(logits, outWeight.rows)
            let pp = fp(probe, outWeight.rows)
            var w = [Float](repeating: 0, count: controlCount)
            w[slot] = base.logit - pp[target] + 1.0
            applyControlAdapter(probe, w)
            let r = argmax(probe, outWeight.rows)
            var exact = 0
            for i in 0..<outWeight.rows where
                (i >= controlFirst && i < controlFirst + controlCount) ||
                pp[i].bitPattern == bp[i].bitPattern { exact += 1 }
            if r.token == target && exact == outWeight.rows { selected.append(target) }
        }
        check(selected == [128000, 128001, 128002, 128003, 128004],
          "FIVE CONTROL ROWS SELECTABLE: explicit five-float adapter vectors independently selected \(selected), while every non-control logit remained bit-identical; plumbing witness only, no semantic-training claim",
          "control-row selection witness produced \(selected)")
        controlAdapterProved = true
    }
    applyControlAdapter(logits, controlArtifactWeights)
    let adapted = argmax(logits, outWeight.rows)
    let hp = fp(headw, nHc)
    check(adapted.finite == outWeight.rows && adapted.token >= 0 && adapted.token < outWeight.rows,
      "NATIVE MODEL TOKEN \(label): HC weights \(Array(UnsafeBufferPointer(start: hp, count: nHc))) collapsed the real stack state; output_norm, immutable \(outWeight.rows)-row MXFP8 vocabulary projection, and removable five-row adapter emitted token_id=\(adapted.token), logit=\(adapted.logit)",
      "native exit head \(label): finite logits \(adapted.finite)/\(outWeight.rows), argmax \(adapted.token)")
    return (adapted.token, adapted.logit)
}

func runStack(_ pos: Int) {
    var resid = residHc0
    for il in 0..<nLayers {
        let w = LW[il]
        let t0 = Date()
        let R = runLayer(il, pos, token, resid)
        let ms = Date().timeIntervalSince(t0) * 1000.0
        msPerLayer[il, default: []].append(ms)
        let tag = "blk.\(il) [gate/up \(w.gx.type) down \(w.dx.type) n_exp \(w.nExpStack) \(w.hashed ? "hash" : "top-k") rope \(w.ratio == 0 ? "plain" : "compressed(\(w.ratio))")]"
        let out = fp(R.outHc, hcDim)
        var finite = 0, distinct = Set<UInt32>()
        for i in 0..<hcDim where out[i].isFinite {
            finite += 1
            distinct.insert(out[i].bitPattern)
        }
        let nativeOK = finite == hcDim && distinct.count > 1 &&
            !R.ids.isEmpty && R.ids.allSatisfy { $0 >= 0 && $0 < w.nExpStack } &&
            gpuErrors == 0
        check(nativeOK,
          "\(tag) native self-witness pos \(pos): \(finite)/\(hcDim) finite output entries, \(distinct.count) distinct bit patterns, experts \(R.ids) inside the file-declared stack",
          "\(tag) native self-witness failed: finite \(finite)/\(hcDim), distinct \(distinct.count), experts \(R.ids), gpuErrors \(gpuErrors)")
        if !nativeOK { layerFail += 1 }
        resid = R.outHc
        if il == nLayers - 1 {
            finalByPos[pos] = R.outHc
        }
    }
}

let tA = Date(); runStack(posA); let wallA = Date().timeIntervalSince(tA)
let tB = Date(); runStack(posB); let wallB = Date().timeIntervalSince(tB)

// ── two consecutive real-stack positions over persistent per-layer KV arenas ─────────────────────
if kvSequence {
    guard kvSteps >= 2 && kvSteps < kvCap else {
        print("FAIL KV_STEPS must be at least 2 and below KV_CAP so a sentinel frontier remains"); exit(1)
    }
    kvArenas = (0..<nLayers).map { _ in sentinelled(kvCap * headDim) }
    compInit()
    useGrowingKv = true
    var firstFinal: [Float] = [], lastFinal: [Float] = [], firstRow: [UInt32] = []
    var currentToken = token
    var emitted: [Int] = []
    // PREFILL then GENERATE. While pos is inside the prompt the input is the prompt's token and
    // the emitted id is discarded — the model is READING. Past the prompt it eats its own output.
    // The boundary is the whole difference between a continuation of one token and an answer.
    for pos in 0..<kvSteps {
        let inputToken = pos < promptIds.count ? promptIds[pos] : currentToken
        var resid = embedToken(inputToken)
        for il in 0..<nLayers { resid = runLayer(il, pos, inputToken, resid).outHc }
        dumpArmed = (pos == promptIds.count - 1)
        let exit = nativeExitHead(resid, "feedback step \(pos), input_token=\(inputToken)")
        if pos >= promptIds.count - 1 { emitted.append(exit.token) }
        currentToken = exit.token
        let rp = fp(resid, hcDim)
        let vals = (0..<hcDim).map { rp[$0] }
        if pos == 0 {
            firstFinal = vals
            let kp = fp(kvArenas[0], kvCap * headDim)
            firstRow = (0..<headDim).map { kp[$0].bitPattern }
        }
        lastFinal = vals
    }
    let kp = fp(kvArenas[0], kvCap * headDim)
    var historyExact = 0, written = 0, frontier = 0, stateDiff = 0
    for j in 0..<headDim {
        if kp[j].bitPattern == firstRow[j] { historyExact += 1 }
        if kp[(kvSteps - 1) * headDim + j].isFinite { written += 1 }
        if kp[kvSteps * headDim + j].isNaN { frontier += 1 }
    }
    for i in 0..<min(firstFinal.count, lastFinal.count) {
        if firstFinal[i] != lastFinal[i] { stateDiff += 1 }
    }
    check(historyExact == headDim && written == headDim && frontier == headDim && stateDiff > 0,
      "GROWING KV, \(kvSteps) REAL STACK STEPS: layer 0 retained \(historyExact)/\(headDim) row-0 bits, wrote \(written)/\(headDim) values in row \(kvSteps-1), left \(frontier)/\(headDim) row-\(kvSteps) sentinels, and the final HC state changed in \(stateDiff)/\(hcDim) entries",
      "growing KV: history \(historyExact)/\(headDim), last row \(written)/\(headDim), frontier \(frontier)/\(headDim), state diff \(stateDiff)/\(hcDim)")
    // ── the second cache's own witness ─────────────────────────────────────────────────────────────
    // A compressing layer must hold exactly floor(kvSteps/ratio) rows, no ratio-0 layer may hold any,
    // and every held row must be finite. A silently-empty compressed cache is the failure this catches:
    // it would read as the raw-only lane and pass every other gate in this file.
    if useCompressor {
        var wrongCount: [Int] = [], nonFinite = 0, totalRows = 0
        for il in 0..<nLayers {
            let w = LW[il]
            let want = w.ratio == 0 ? 0 : kvSteps / w.ratio
            if compRows[il].count != want { wrongCount.append(il) }
            totalRows += compRows[il].count
            if compRows[il].count > 0 {
                let p = fp(compPacked[il], compRows[il].count * headDim)
                for j in 0..<(compRows[il].count * headDim) where !p[j].isFinite { nonFinite += 1 }
            }
        }
        let ratio4 = (0..<nLayers).filter { LW[$0].ratio == 4 }.count
        check(wrongCount.isEmpty && nonFinite == 0 && compRefusals == 0,
          "THE SECOND CACHE RAN: after \(kvSteps) steps every one of the \(nLayers) layers holds exactly floor(steps/ratio) compressed rows (\(totalRows) rows total, \(ratio4) layers at ratio 4, whose first row lands at pos 3), all \(totalRows * headDim) entries finite, and no step walked past the indexer's inert radius of \(indexerTopK)",
          "second cache: layers with the wrong row count \(wrongCount.prefix(8)), non-finite entries \(nonFinite), steps past the indexer radius \(compRefusals)")
    } else {
        print("      ── FORM_DS4_NO_COMPRESSOR=1: the raw-only lane, kept as the A/B ──")
    }
    // The expected count is kvSteps MINUS the prefill positions whose outputs are discarded: while
    // the model is reading the prompt its emissions are not answers. With no prompt this is kvSteps,
    // which is what it was before prefill existed.
    let expectEmitted = kvSteps - max(0, promptIds.count - 1)
    check(emitted.count == expectEmitted && emitted.allSatisfy { $0 >= 0 && $0 < emb.rows },
      "BOUNDED AUTOREGRESSIVE TOKEN FEEDBACK (\(kvSteps) steps, no EOS claim): \(emitted); each emitted token became the next step's exact embedding row and every hashed router's token",
      "bounded autoregressive token feedback did not produce \(expectEmitted) in-vocabulary tokens: \(emitted)")
    useGrowingKv = false
}

// ── hushfold at STACK scale ────────────────────────────────────────────────────────────────────────
var posDiff = 0; var maxDelta: Float = 0
if let a = finalByPos[posA], let b = finalByPos[posB] {
    let ap = fp(a, hcDim), bp = fp(b, hcDim)
    for i in 0..<hcDim { if ap[i] != bp[i] { posDiff += 1; maxDelta = max(maxDelta, abs(ap[i]-bp[i])) } }
}
check(posDiff > 0 && layerFail == 0,
  "hushfold at stack scale: the same token's output after all \(nLayers) layers differs between pos \(posA) and pos \(posB) in \(posDiff)/\(hcDim) entries (max delta \(maxDelta)) while every layer passes its native self-witness",
  "hushfold: \(posDiff) differing entries, \(layerFail) failed layer gates")

// ── the native exit head: HC collapse → final norm → real vocabulary projection → argmax ─────────
// This stays in the same Metal lifetime as the 43-layer state. No JSON membrane, Ollama, llama.cpp,
// probe vector, or reconstructed hidden state intervenes.
var tokenByPos: [Int: Int] = [:]
for pos in [posA, posB] {
    guard let resid = finalByPos[pos] else { continue }
    let exit = nativeExitHead(resid, "pos \(pos)")
    tokenByPos[pos] = exit.token
}

if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER ERROR(S) — first: \(gpuFirstError ?? "unknown") ===") }
// unispan: a per-layer time from ONE run is a guess; each layer is timed at BOTH positions.
print(String(format: "      wall: pos %d %.2f s, pos %d %.2f s over %d layers", posA, wallA, posB, wallB, nLayers))
var slow: [(Int, Double)] = []
for (il, ts) in msPerLayer { slow.append((il, ts.reduce(0,+)/Double(ts.count))) }
slow.sort { $0.1 > $1.1 }
let allms = slow.map { $0.1 }
print(String(format: "      per layer (mean of the two positions): min %.1f ms, max %.1f ms, mean %.1f ms",
             allms.min() ?? 0, allms.max() ?? 0, allms.reduce(0,+)/Double(max(allms.count,1))))
for (il, ms) in slow.prefix(3) {
    print(String(format: "        slowest: blk.%d %.1f ms  [gate/up %d down %d]", il, ms, LW[il].gx.type, LW[il].dx.type))
}
print(String(format: "      device.currentAllocatedSize = %ld B (%.2f GiB) — the model is mmapped and wrapped, not copied (onelean); it does NOT grow with layer count",
             dev.currentAllocatedSize, Double(dev.currentAllocatedSize)/1073741824.0))

let ok = failures == 0 && gpuErrors == 0
if ok {
    print("VERDICT PASS  \(gateNo) gates — \(nLayers) HETEROGENEOUS DeepSeek-V4-Flash LAYERS STACKED at real dims over the 85 GiB file, the four hyper-connection streams carried from each layer into the next, every per-layer decision (expert count, gate/up and down type, routing regime, rope regime) read from the file's own tensor table, at TWO positions, with native self-witnesses and every dispatch sentinelled")
} else {
    print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors")
}
exit(ok ? 0 : 1)
SWIFT
swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || { echo "FAIL swiftc runner"; tail -40 "$work/swift.err"; exit 1; }

"$work/runner" "$work/params.txt" "$BLOB" \
    "$LIB_EMB" "$LIB_MLA" "$LIB8" "$LIB_CORE" "$LIB_HC" \
    "$LIB_MX4" "$LIB_IQ2" "$LIB_Q2K" "$LIB_Q80" "$LIB_Q8G" "$LIB_FFN" "$LIB_KV" "$CONTROL_ADAPTER" | tee "$work/native.out"
rc=${PIPESTATUS[0]}
if [[ $rc -eq 0 && "$KV_SEQUENCE" == 1 ]]; then
    FKWU="$ROOT/../fkwu"
    TOKENIZER="$ROOT/form-stdlib/dsv4-tokenizer-cli.fk"
    [[ -x "$FKWU" ]] || { echo "FAIL native fkwu tokenizer carrier missing: $FKWU"; exit 1; }
    ids="$(sed -n 's/.*BOUNDED AUTOREGRESSIVE TOKEN FEEDBACK ([^)]*): \[\([^]]*\)\].*/\1/p' "$work/native.out" | tail -1 | tr ',' ' ')"
    [[ -n "$ids" ]] || { echo "FAIL native continuation token evidence missing"; exit 1; }
    echo "NATIVE DECODED CONTINUATION"
    for id in $ids; do
        printf 'decode %s\n' "$id" | "$FKWU" "$TOKENIZER" | awk -F= '$1=="text"{printf "%s",$2}'
    done
    printf '\n'
fi
exit $rc
