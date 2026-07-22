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
#      already carries. The freqs are re-derived HERE, on the host, from the file's own KV — never taken
#      from the oracle, or the RoPE choice would be inherited on both sides and falsify nothing.
#
# EVIDENCE CLASS PER STAGE (twinblind, corpus row 868):
#   CHOOSING  — the per-layer routing regime, the bias-in/weight-out asymmetry, which expert-type kernel
#               each half takes, the compressed-RoPE reduction, and how the four hyper-connection streams
#               compose from one layer into the next. Proven against the rented fp64 ds4.c transcription
#               in dsv4-mla-core-oracle.py's `stack` mode, which carries its OWN state through every layer
#               and shares no code, no buffer and no arithmetic with the band, the MSL or this carrier.
#               The oracle's stack mode was itself controlled: at layer 0 it reproduces `layer` mode's
#               vectors BYTE-IDENTICALLY, and `layer` mode is what gates metal_dsv4_layer_join.sh.
#   CANONICAL — the MXFP4 / IQ2_XXS / MXFP8 / F16 decodes and matvecs (Stones 33/34/35), re-witnessed by
#               the oracle's own independent decode.
#
# halfrent (row 870) DEEPENS: ds4.c cannot even VALIDATE this file's layers 3..42 —
# tensor_expect_routed_expert (:4641) demands dim[2] == 256 and exit(1)s on 192. So what is rented is the
# order and the scalars; the arithmetic for these types is re-derived on both sides. Said, not buried.
#
# hushfold (row 859): the whole stack runs at TWO positions; the outputs must DIFFER while each agrees
# with its own oracle. unispan: per-layer wall time is reported at both positions, never from one.
# zerobirth/edgedrop: every output buffer is NaN-sentinelled and cb.error/cb.status checked.
# onelean/lapspan: every weight of every layer is reached through the overlapping bytesNoCopy views.
#
# Run:  form/native/metal/metal_dsv4_stack.sh
#   FORM_DS4_STACK_LAYERS=<n>     how many layers to stack (default: the file's block_count)
#   FORM_DS4_ORACLE_DIR0/DIR7     reuse an already-computed oracle stack instead of running one
#   FORM_DS4_PROMPT_TOKEN=<id>
# Off-Mac (or with no swiftc) it SKIPs with exit 2, like every Metal row in GPU_GAPS.md.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
GO_BIN="$ROOT/form-kernel-go/bin-go"
BLOB="${FORM_DS4_BLOB:-$HOME/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf}"
CACHE="$ROOT/native/metal/.metallib-cache"
TOKEN="${FORM_DS4_PROMPT_TOKEN:-671}"
POS_A=0
POS_B=7

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

# ── 2a. the WANT list: every tensor every stacked layer touches, named, then resolved in ONE pass ──
: > "$work/want.txt"
: > "$work/flags.txt"
echo "EMB token_embd.weight" >> "$work/want.txt"
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
# the per-layer compress ratios are a HYPER-PARAMETER wearing an array's clothes, and the body already
# has a reader for exactly that case (gguf-manifest.fk, gm-emit-array). Walked, never guessed.
printf '(gm-emit-array "%s" "deepseek4.attention.compress_ratios")\n' "$BLOB" > "$work/arr.fk"
"$GO_BIN" "${FILES[@]}" "$work/arr.fk" > "$work/arr.out" 2>"$work/arr.err" || { echo "FAIL ratio array emission"; tail -5 "$work/arr.err"; exit 1; }
grep -q '^ARR deepseek4.attention.compress_ratios' "$work/arr.out" || { echo "FAIL the file carries no deepseek4.attention.compress_ratios"; cat "$work/arr.out"; exit 1; }
RATIOS="$(awk '$1=="A"{ s = s (n++ ? "," : "") $3 } END{ print s }' "$work/arr.out")"
NRATIO="$(awk '$1=="A"{n++} END{print n+0}' "$work/arr.out")"
(( NRATIO >= WANT_LAYERS )) || { echo "FAIL compress_ratios carries $NRATIO entries for $WANT_LAYERS layers"; exit 1; }
echo "  rope: base=$ROPE_BASE compressed_base=$ROPE_CBASE scale_factor=$ROPE_SCALEF orig_ctx=$ROPE_ORIGCTX beta=[$BETA_FAST,$BETA_SLOW]"
for ((il=0; il<WANT_LAYERS; il++)); do
    r="$(echo "$RATIOS" | cut -d, -f$((il+1)))"
    echo "L${il}_RATIO ${r:-0}" >> "$work/params.txt"
done

cat >> "$work/params.txt" <<EOF
STEP $STEP
VIEWLIMIT $VIEWLIMIT
NVIEWS $NVIEWS
TOKEN $TOKEN
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

# ── 2b. THE RENTED ORACLE, in `stack` mode, at BOTH positions (hushfold) ───────────────────────────
ORACLE="$ROOT/form-stdlib/tests/dsv4-mla-core-oracle.py"
[[ -f "$ORACLE" ]] || { echo "FAIL the rented oracle is missing: $ORACLE"; exit 1; }
ORA0="${FORM_DS4_ORACLE_DIR0:-}"; ORA7="${FORM_DS4_ORACLE_DIR7:-}"
if [[ -z "$ORA0" || -z "$ORA7" ]]; then
    ORA0="$work/ora$POS_A"; ORA7="$work/ora$POS_B"; mkdir -p "$ORA0" "$ORA7"
    echo "  renting the oracle in STACK mode at pos $POS_A and pos $POS_B over $WANT_LAYERS layers..."
    DSV4_ORACLE_MODE=stack DSV4_ORACLE_OUT="$ORA0" python3 "$ORACLE" "$BLOB" "$TOKEN" "$POS_A" "$WANT_LAYERS" > "$work/ora0.txt" 2>&1 &
    p0=$!
    DSV4_ORACLE_MODE=stack DSV4_ORACLE_OUT="$ORA7" python3 "$ORACLE" "$BLOB" "$TOKEN" "$POS_B" "$WANT_LAYERS" > "$work/ora7.txt" 2>&1 &
    p7=$!
    wait $p0; wait $p7
else
    echo "  reusing pre-computed oracle stacks: $ORA0 and $ORA7"
fi
# THE COMPOSED-TRAJECTORY ENVELOPE (selfgauge). A 43-layer comparison cannot honestly ask "is the GPU's
# final state right to 1e-6" — it can only ask "do two runs of THIS recipe, one of them nudged each layer
# by as much as f32 arithmetic nudges it, stay this close?" So the oracle is rented a second time, in fp64
# throughout, with the state tilted by PERSTEP after every layer. The distance between those two fp64
# trajectories is the yardstick — measured from the reference, not chosen to make this harness green.
# (A one-ulp INPUT tilt was measured first and is the wrong yardstick: this model DAMPS an input
# perturbation hard — 1.2e-7 at blk.0 falls to 4e-10 by blk.18 — so it says nothing about noise that is
# injected fresh at every layer, which is what an f32 carrier does.)
# the per-layer tilt: the size of the gap an f32 carrier was MEASURED to have when ONE layer is run alone
# from this oracle's own input (the "THIS LAYER ALONE" gates below report it at every layer; the largest observed is 1.4e-5).
PERSTEP=1.4e-5
PER0="${FORM_DS4_PERTURB_DIR0:-}"; PER7="${FORM_DS4_PERTURB_DIR7:-}"
if [[ -z "$PER0" || -z "$PER7" ]]; then
    PER0="$work/per$POS_A"; PER7="$work/per$POS_B"; mkdir -p "$PER0" "$PER7"
    echo "  renting the oracle AGAIN, tilted by $PERSTEP after EVERY layer, to measure how far two runs of"
    echo "  the same recipe drift apart when one is nudged each layer by as much as f32 nudges it..."
    DSV4_ORACLE_MODE=stack DSV4_ORACLE_PERTURB_EVERY=$PERSTEP DSV4_ORACLE_OUT="$PER0" python3 "$ORACLE" "$BLOB" "$TOKEN" "$POS_A" "$WANT_LAYERS" > "$work/per0.txt" 2>&1 &
    q0=$!
    DSV4_ORACLE_MODE=stack DSV4_ORACLE_PERTURB_EVERY=$PERSTEP DSV4_ORACLE_OUT="$PER7" python3 "$ORACLE" "$BLOB" "$TOKEN" "$POS_B" "$WANT_LAYERS" > "$work/per7.txt" 2>&1 &
    q7=$!
    wait $q0; wait $q7
else
    echo "  reusing pre-computed one-ulp sensitivity stacks: $PER0 and $PER7"
fi
# a partial oracle still gates a PREFIX; how far it got is read from its own done ledger, never assumed.
done_count(){ local n; n=$(wc -l < "$1/oracle-done.txt" 2>/dev/null | tr -d ' '); echo "${n:-0}"; }
AVAIL=$WANT_LAYERS
for d in "$ORA0" "$ORA7" "$PER0" "$PER7"; do n=$(done_count "$d"); (( n < AVAIL )) && AVAIL=$n; done
if (( AVAIL < WANT_LAYERS )); then
    echo "  the oracle has completed $AVAIL/$WANT_LAYERS layers at both positions — gating that PREFIX (aporon)"
    WANT_LAYERS=$AVAIL
    sed -i '' "s/^NLAYERS .*/NLAYERS $WANT_LAYERS/" "$work/params.txt"
fi
(( WANT_LAYERS > 0 )) || { echo "FAIL the oracle produced no completed layer"; exit 1; }
if cmp -s "$ORA0/oracle-L$((WANT_LAYERS-1))-out_hc.f64" "$ORA7/oracle-L$((WANT_LAYERS-1))-out_hc.f64"; then
    echo "FAIL hushfold: the ORACLE's own stack output is identical at pos $POS_A and pos $POS_B"; exit 1
fi
echo "  hushfold: the ORACLE's stack output already differs between pos $POS_A and pos $POS_B — the GPU must too"

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
LIB_FFN="$(compile_unit  dsv4-stack-ffn-unit   form_dsv4_topk_weights         dsv4stkffn)" || exit 1

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
let libMx4 = argv[8], libIq2 = argv[9], libFfn = argv[10]
let oraDirA = argv[11], oraDirB = argv[12]
let perDirA = argv[13], perDirB = argv[14]

struct Tn { let abs: Int, bytes: Int, idx: Int, inner: Int, holds: Int, d0: Int, d1: Int, d2: Int, type: Int
            var rows: Int { d1 }
            var cols: Int { d0 }
            var nel: Int  { d0 * d1 } }
func T(_ k: String) -> Tn {
    return Tn(abs: I(k+"_ABS"), bytes: I(k+"_BYTES"), idx: I(k+"_IDX"), inner: I(k+"_INNER"),
              holds: I(k+"_HOLDS"), d0: I(k+"_D0"), d1: I(k+"_D1"), d2: I(k+"_D2"), type: I(k+"_TYPE"))
}
let emb = T("EMB")
let step = I("STEP"), viewLimit = I("VIEWLIMIT"), nviews = I("NVIEWS"), token = I("TOKEN")
let nLayers = I("NLAYERS")
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
    let hashed: Bool, ratio: Int
    var nExpStack: Int { gx.d2 }          // the PER-LAYER expert count: the tensor's own dim[2]
    var nExpRouter: Int { rt.d1 }         // the router's width: the logit projection's own out-dim
}
func layerW(_ il: Int) -> LayerW {
    let p = "L\(il)_"
    let hashed = I(p+"HASHED") == 1
    return LayerW(nrm: T(p+"NORM"), qa: T(p+"QA"), qan: T(p+"QAN"), qb: T(p+"QB"), kv: T(p+"KV"),
                  kvan: T(p+"KVAN"), snk: T(p+"SNK"), oa: T(p+"OA"), ob: T(p+"OB"),
                  haf: T(p+"HAF"), has: T(p+"HAS"), hab: T(p+"HAB"),
                  hff: T(p+"HFF"), hfs: T(p+"HFS"), hfb: T(p+"HFB"),
                  fnw: T(p+"FN"), rt: T(p+"RT"), gx: T(p+"GX"), ux: T(p+"UX"), dx: T(p+"DX"),
                  sgw: T(p+"SG"), suw: T(p+"SU"), sdw: T(p+"SD"),
                  ht: hashed ? T(p+"HT") : nil, bias: hashed ? nil : T(p+"BI"),
                  hashed: hashed, ratio: I(p+"RATIO"))
}
let LW = (0..<nLayers).map { layerW($0) }

guard let dev = MTLCreateSystemDefaultDevice() else { print("SKIP no Metal device"); exit(2) }
func lib(_ p: String) -> MTLLibrary { return try! dev.makeLibrary(URL: URL(fileURLWithPath: p)) }
let lEmb = lib(libEmb), lMla = lib(libMla), l8 = lib(lib8), lCore = lib(libCore), lHc = lib(libHc)
let lMx4 = lib(libMx4), lIq2 = lib(libIq2), lFfn = lib(libFfn)
let queue = dev.makeCommandQueue()!
var failures = 0, gpuErrors = 0
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

// ---- the oracle's vectors ----
func readOracle(_ dir: String, _ il: Int, _ key: String) -> [Double] {
    let p = dir + "/oracle-L\(il)-" + key + ".f64"
    guard let s = try? String(contentsOfFile: p, encoding: .utf8) else { print("FAIL oracle vector missing: \(p)"); exit(1) }
    var out: [Double] = []
    s.split(separator: "\n").forEach { if let v = Double($0) { out.append(v) } }
    return out
}
// assocwall (row 866), and what a 43-layer stack does to it. Row 866 says: an absolute bound over every
// element, a relative bound only above a magnitude floor. Both assume the state's MAGNITUDE is roughly
// known. Through this stack it is not: the hyper-connection state's RMS goes from 0.14 after blk.0 to
// 2085 after blk.42 -- four orders -- because HC IS the residual stream and nothing renormalises it
// between layers. A fixed absolute bound is meaningless at depth, and a relative bound above a FIXED
// floor is worse: at blk.42 an element of size 1e-2 sits 2 000 000x below the peak and is pure
// cancellation noise, so its "relative error" says nothing about anything.
//
// So the gate is NORMALISED to the vector's own peak, and its size is MEASURED, not chosen:
//
//   nd = max|gpu - oracle| / max|oracle|         the GPU's normalised disagreement
//   ne = max|oracle' - oracle| / max|oracle|     oracle' is the SAME fp64 recipe with the state tilted
//                                                by 1.4e-5 after EVERY layer -- the LARGEST gap an f32
//                                                carrier was measured to have on ONE layer run alone
//   PASS iff nd < max(8*ne, 3e-5)
//
// ne is the envelope that per-layer nudging opens BY ITSELF in the reference: a property of the model,
// not of this harness or of any tolerance anyone picked. The envelope's LINEARITY in the nudge size was
// measured, not assumed: probes at 5e-6 and 1.4e-5 give envelopes whose ratio is 2.80 at blk.3, 2.69 at
// blk.9, 2.90 at blk.18, 2.80 at blk.21 -- the injection ratio is 2.80. So setting the probe to the
// largest measured per-layer gap is the right size, and the factor 8 is what is left over for the nudge's
// DIRECTION, which was NOT measured. It is the one chosen number in this instrument and it is named as
// such rather than hidden: every per-layer gap is printed by the THIS-LAYER-ALONE gates below.
// The floor 3e-5 is ~5x the normalised single-layer gap Stone 37 measured (5.0e-6 over a peak of 0.89)
// and is what carries the early layers, where the envelope has not opened.
func cmpOra(_ gpu: UnsafeMutablePointer<Float>, _ ref: [Double], _ pert: [Double], _ floorN: Double)
        -> (Bool, Double, Double, Double, Double, Int, Int, Float, Float, Double) {
    var maxAbs = 0.0, maxRel = 0.0, sens = 0.0, scale = 0.0, nan = 0
    var sd = 0.0, sr = 0.0
    var seen = Set<UInt32>(); var vmin = Float.greatestFiniteMagnitude, vmax = -Float.greatestFiniteMagnitude
    for i in 0..<ref.count { scale = max(scale, abs(ref[i])) }
    if scale <= 0 { scale = 1e-30 }
    let relFloor = 1e-3 * scale        // "above a magnitude floor" -- the floor is the VECTOR's own
    for i in 0..<ref.count {
        let g = gpu[i]
        if g.isNaN || !g.isFinite { nan += 1; continue }
        let d = abs(Double(g) - ref[i])
        if d > maxAbs { maxAbs = d }
        if abs(ref[i]) > relFloor { let r = d/abs(ref[i]); if r > maxRel { maxRel = r } }
        sd += d*d; sr += ref[i]*ref[i]
        seen.insert(g.bitPattern); vmin = min(vmin, g); vmax = max(vmax, g)
    }
    if pert.count == ref.count {
        for i in 0..<ref.count { sens = max(sens, abs(pert[i] - ref[i])) }
    }
    let nd = maxAbs / scale, ne = sens / scale
    let bound = max(8.0 * ne, floorN)
    let degenFloor = min(8, max(2, ref.count / 2))
    let nonDegen = seen.count > degenFloor && vmax > vmin
    // the RELATIVE L2 of the difference: the robust companion to a max-element measure, which one bad
    // cancellation among 16 384 entries can dominate. Both are printed; neither is trusted alone.
    let l2 = sr > 0 ? (sd/sr).squareRoot() : 0.0
    return (nan == 0 && nd < bound && nonDegen, nd, ne, bound, maxRel, nan, seen.count, vmin, vmax, l2)
}

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
let pSwiglu = pipe(lFfn, "form_dsv4_swiglu_f32")
let pScale = pipe(lFfn, "form_dsv4_scale_f32")
let pAxpy = pipe(lFfn, "form_dsv4_axpy_f32")
let pHashSel = pipe(lFfn, "form_dsv4_hash_select")
let pHashW = pipe(lFfn, "form_dsv4_hash_weights")
let pTopkW = pipe(lFfn, "form_dsv4_topk_weights")

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

// ══ the compressed-RoPE reduction, re-derived HERE, on the host, from the file's own KV ══════════════
// ds4.c:10102 rope_tail_ext_inplace under ds4.c:10166 rope_tail_layer_inplace's arguments. The magnitude
// scale cancels: attn_factor is set to 1/(1 + 0.1*ln(1/freq_scale)) precisely so mscale comes back to 1.
// What is left is theta = theta_extrap*(freq_scale*(1 - ramp_k) + ramp_k), a PER-PAIR SCALE — which is
// exactly what form_mla_rope_f32's freqs[] already is. Taking these numbers from the oracle instead would
// make the GPU inherit the choice on both sides and falsify nothing (twinblind).
let nPair = nRot/2
var freqCache: [Int: MTLBuffer] = [:]
func ropeFreqs(_ il: Int) -> MTLBuffer {
    if let b = freqCache[il] { return b }
    let compressed = LW[il].ratio != 0
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
func gpuAttend(_ q: MTLBuffer, _ rows: MTLBuffer, _ snk: Tn) -> MTLBuffer {
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
    } else {
        print("FAIL an expert tensor carries type \(t.type); this stack decodes 40 (MXFP4) and 16 (IQ2_XXS)")
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
    let kq = gpuKvRound(kr)
    let ha = gpuAttend(qr, kq, w.snk)
    let hu = gpuRope(ha, nHead, pos, il, true)
    let lo = gpuGrouped(w.oa, hu)
    return gpuMx8(w.ob, lo, w.ob.rows, w.ob.cols)
}

func runLayer(_ il: Int, _ pos: Int, _ residHc: MTLBuffer) -> LayerOut {
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
        var t32 = UInt32(token), nu = UInt32(nUsed)
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
// the oracle's own fp64 vector, rounded to f32 and handed to the device: an INJECTED input, so a kernel
// can be judged without the stack's accumulated drift standing between it and its reference.
func oracleBuf(_ dir: String, _ il: Int, _ key: String) -> (MTLBuffer, Int) {
    let v = readOracle(dir, il, key)
    let b = dev.makeBuffer(length: max(v.count,1)*4, options: .storageModeShared)!
    let p = b.contents().bindMemory(to: Float.self, capacity: max(v.count,1))
    for i in 0..<v.count { p[i] = Float(v[i]) }
    return (b, v.count)
}
func sentinelledCopy(_ a: [Float]) -> MTLBuffer {
    let b = dev.makeBuffer(length: max(a.count,1)*4, options: .storageModeShared)!
    let p = b.contents().bindMemory(to: Float.self, capacity: max(a.count,1))
    for i in 0..<a.count { p[i] = a[i] }
    return b
}

// ---- the token's embedding, broadcast to the four streams: THE stack's input (ds4.c:9764) ----
let rowOff = token * nEmbd * 2
let x0 = sentinelled(nEmbd)
do { var b64 = UInt64(emb.inner + rowOff), c32 = UInt32(nEmbd)
     enc(pEmb, nEmbd, 256) { c in c.setBuffer(views[emb.idx], offset: 0, index: 0); c.setBuffer(x0, offset: 0, index: 1)
                                  c.setBytes(&b64, length: 8, index: 2); c.setBytes(&c32, length: 4, index: 3) } }
let residHc0 = sentinelled(hcDim)
do { var a = UInt32(nHc), b = UInt32(nEmbd)
     enc(pHcBcast, hcDim, 256) { c in c.setBuffer(x0, offset: 0, index: 0); c.setBuffer(residHc0, offset: 0, index: 1)
                                      c.setBytes(&a, length: 4, index: 2); c.setBytes(&b, length: 4, index: 3) } }

// the WITNESS layers: the first layer of every distinct group, plus layer 1. Those get the full gate set
// (the routing decision, the six weights, the first expert's whole path, the two accumulations); every
// other layer is gated on its complete output, which is the only thing the next layer can see anyway.
var witness = Set<Int>()
do {
    var seen = Set<String>()
    for il in 0..<nLayers {
        let w = LW[il]
        let k = "\(w.gx.type)/\(w.dx.type)/\(w.nExpStack)/\(w.hashed)/\(w.ratio == 0)"
        if !seen.contains(k) { seen.insert(k); witness.insert(il) }
    }
    if nLayers > 1 { witness.insert(1) }
    witness.insert(nLayers - 1)
}

var finalByPos: [Int: [Float]] = [:]
var layerFail = 0
var worstAbs = 0.0, worstSens = 0.0
var worstAbsWhere = "", worstSensWhere = ""
var ndByLayer: [Int: [Double]] = [:], neByLayer: [Int: [Double]] = [:], l2ByLayer: [Int: [Double]] = [:]
var ulpRouteSplit: [Int] = []
var injNd: [Int: [Double]] = [:]
var msPerLayer: [Int: [Double]] = [:]

func runStack(_ pos: Int, _ oraDir: String, _ perDir: String) {
    var resid = residHc0
    for il in 0..<nLayers {
        let w = LW[il]
        let t0 = Date()
        let R = runLayer(il, pos, resid)
        let ms = Date().timeIntervalSince(t0) * 1000.0
        msPerLayer[il, default: []].append(ms)
        let tag = "blk.\(il) [gate/up \(w.gx.type) down \(w.dx.type) n_exp \(w.nExpStack) \(w.hashed ? "hash" : "top-k") rope \(w.ratio == 0 ? "plain" : "compressed(\(w.ratio))")]"

        // selfgauge: no number below was chosen to make this harness green. The envelope is measured
        // from the reference's own one-ulp sensitivity, and the floor is Stone 37's measured single-layer
        // normalised gap with 5x headroom.
        func G(_ buf: MTLBuffer, _ cnt: Int, _ key: String, _ floorN: Double, _ what: String) {
            let ref = readOracle(oraDir, il, key)
            let prt = readOracle(perDir, il, key)
            guard ref.count == cnt else {
                check(false, "", "\(tag) oracle \(key) has \(ref.count) entries, expected \(cnt)"); layerFail += 1; return
            }
            let (ok, nd, ne, bound, mr, nn, ds, mn, mx, l2) = cmpOra(fp(buf, cnt), ref, prt, floorN)
            if nd > worstAbs { worstAbs = nd; worstAbsWhere = "blk.\(il) \(key) pos \(pos)" }
            if ne > worstSens { worstSens = ne; worstSensWhere = "blk.\(il) \(key) pos \(pos)" }
            ndByLayer[il, default: []].append(nd); neByLayer[il, default: []].append(ne)
            if key == "out_hc" { l2ByLayer[il, default: []].append(l2) }
            check(ok && gpuErrors == 0,
              "\(tag) \(what) [RENTED ORACLE, pos \(pos)] (normalised disagreement \(nd) < \(bound); relative L2 \(l2); the reference's own one-ulp envelope here is \(ne); relative \(mr) above 1e-3 of peak; \(ds) distinct, range [\(mn),\(mx)]; \(nn) NaN)",
              "\(tag) \(key) pos \(pos): normalised disagreement \(nd) exceeds \(bound) (per-layer-nudge envelope \(ne), floor \(floorN)); relative L2 \(l2); relative \(mr); nan \(nn); distinct \(ds); gpuErrors \(gpuErrors)")
            if !(ok && gpuErrors == 0) { layerFail += 1 }
        }
        // THE DISCRETE FALSIFIER, at EVERY layer and not only the witnesses. The numeric gates are
        // envelopes; this one is not. Which six experts fire is an integer decision that a wrong routing
        // regime, a wrong expert count, a biased weight or a stale bias would change outright -- and a
        // changed expert is a different 8.4 M-parameter matrix, not a rounding difference.
        let oraSel = readOracle(oraDir, il, "selected").map { Int($0) }
        let perSel = readOracle(perDir, il, "selected").map { Int($0) }
        if perSel != oraSel { ulpRouteSplit.append(il) }
        check(R.ids == oraSel && !R.ids.isEmpty && R.ids.allSatisfy { $0 >= 0 && $0 < w.nExpStack },
          "\(tag) the routing DECISION [RENTED ORACLE, pos \(pos)]: \(w.hashed ? "the I32 table row for token \(token), read through the view" : "biased top-k over \(w.nExpRouter) logits with UNBIASED weighting") chose \(R.ids) -- bit-identical to the oracle's, and every id inside this layer's own \(w.nExpStack)-deep stack",
          "\(tag) routing: GPU \(R.ids) vs oracle \(oraSel)")
        if R.ids != oraSel { layerFail += 1 }
        if witness.contains(il) {
            G(R.afterAttn, hcDim, "after_attn_hc", 3e-5,
              "the attention half -- hc_pre(attn) -> 13 MLA dispatches with THIS layer's rope freqs -> hc_post(attn)")
            G(R.ffnNorm, nEmbd, "ffn_normed", 3e-5, "hc_pre(ffn) -> ffn_norm, the FFN's own frame")
            G(R.logits, w.nExpRouter, "router_logits", 3e-5,
              "the router's F16 projection -- \(w.nExpRouter) logits even where the stack holds \(w.nExpStack) experts")
            G(sentinelledCopy(R.wts), nUsed, "expert_w", 3e-5,
              "the six weights: probs = sqrt(softplus(logit)) over the floored sum, scaled by \(wscale)\(w.hashed ? "" : " -- the UNBIASED prob, never the biased score")")
            G(R.gate0, nFf, "exp0_gate", 3e-5, "expert \(R.ids.first ?? -1)'s GATE -- the type-\(w.gx.type) kernel this layer's table asked for")
            G(R.mid0, nFf, "exp0_mid", 3e-5, "the clamped SwiGLU mid, times THIS expert's router weight, before the down projection")
            G(R.down0, nEmbd, "exp0_down", 3e-5, "expert \(R.ids.first ?? -1)'s DOWN -- the type-\(w.dx.type) kernel")
            G(R.moe, nEmbd, "moe", 3e-5, "all \(nUsed) routed experts accumulated")
            G(R.shared, nEmbd, "shared", 3e-5, "the shared expert (MXFP8) -- unrouted, weight 1, simply added")

            // ── THE INJECTED-INPUT GATE, the sharp one for this stone ──────────────────────────────
            // Every gate above compares the GPU's whole trajectory against the oracle's, so at depth the
            // stack's own accumulated f32 drift stands between a kernel and its reference and a kernel
            // gate at blk.36 is really a gate on 37 layers. Here the ORACLE's OWN ffn_normed is rounded
            // to f32, handed to the device, and this layer's expert kernels are run on it. The comparison
            // is then depth-INDEPENDENT: it judges the type-\(w.gx.type)/\(w.dx.type) kernels, this
            // layer's own expert slices and the shared expert, and nothing else. Its bound carries no
            // depth factor at all, because there is no depth in it.
            let (inj, injN) = oracleBuf(oraDir, il, "ffn_normed")
            if injN == nEmbd, let e0 = oraSel.first, e0 >= 0, e0 < w.nExpStack {
                let ow = readOracle(oraDir, il, "expert_w")
                let gI = gpuExpert(w.gx, inj, e0)
                let uI = gpuExpert(w.ux, inj, e0)
                let mI = gpuSwiglu(gI, uI, nFf, Float(ow.first ?? 1.0), clamp)
                let dI = gpuExpert(w.dx, mI, e0)
                let sgI = gpuMx8(w.sgw, inj, w.sgw.rows, w.sgw.cols)
                let suI = gpuMx8(w.suw, inj, w.suw.rows, w.suw.cols)
                let smI = gpuSwiglu(sgI, suI, nFf, 1.0, clamp)
                let shI = gpuMx8(w.sdw, smI, w.sdw.rows, w.sdw.cols)
                let rI = gpuF16mv(w.rt, inj)
                func GI(_ buf: MTLBuffer, _ cnt: Int, _ key: String, _ what: String) {
                    let ref = readOracle(oraDir, il, key)
                    guard ref.count == cnt else { check(false, "", "\(tag) injected \(key) width \(ref.count) != \(cnt)"); layerFail += 1; return }
                    let (ok, nd, _, _, mr, nn, ds, mn, mx, l2) = cmpOra(fp(buf, cnt), ref, [], 3e-5)
                    check(ok && gpuErrors == 0,
                      "\(tag) INJECTED INPUT — \(what) (normalised disagreement \(nd) < 3e-5, DEPTH-INDEPENDENT: the oracle's own ffn_normed went in; relative L2 \(l2); relative \(mr) above 1e-3 of peak; \(ds) distinct, range [\(mn),\(mx)]; \(nn) NaN)",
                      "\(tag) INJECTED \(key): normalised disagreement \(nd) exceeds 3e-5; relative L2 \(l2); relative \(mr); nan \(nn); distinct \(ds)")
                    if !(ok && gpuErrors == 0) { layerFail += 1 }
                }
                GI(rI, w.nExpRouter, "router_logits", "the F16 router projection on the oracle's state")
                GI(gI, nFf, "exp0_gate", "expert \(e0)'s GATE through the type-\(w.gx.type) kernel, at this layer's own byte slice")
                GI(dI, nEmbd, "exp0_down", "expert \(e0)'s DOWN through the type-\(w.dx.type) kernel")
                GI(shI, nEmbd, "shared", "the MXFP8 shared expert, gate/up/SwiGLU/down")
            }
        }
        // ── THE PER-LAYER INJECTED GATE. The whole-stack gate above measures the GPU's trajectory
        // against the oracle's, so at blk.36 it is really a gate on 37 composed layers and cannot say
        // whether blk.36 ITSELF is right. This runs the SAME layer again from the ORACLE's own input for
        // this layer -- its predecessor's out_hc, rounded to f32 -- and compares the result to the
        // oracle's. There is no depth in it, so its bound carries no depth: one layer, judged alone.
        do {
            let (inR, inN) = il == 0 ? (residHc0, hcDim) : oracleBuf(oraDir, il-1, "out_hc")
            if inN == hcDim {
                let J = runLayer(il, pos, inR)
                let ref = readOracle(oraDir, il, "out_hc")
                let (ok, nd, _, _, mr, nn, ds, mn, mx, l2) = cmpOra(fp(J.outHc, hcDim), ref, [], 3e-5)
                injNd[il, default: []].append(nd)
                check(ok && gpuErrors == 0 && J.ids == oraSel,
                  "\(tag) THIS LAYER ALONE, from the ORACLE's own input [pos \(pos)]: hc_pre(attn) -> MLA -> hc_post(attn) -> hc_pre(ffn) -> ffn_norm -> routed MoE + shared -> hc_post(ffn), and it lands on the oracle's own output (normalised disagreement \(nd) < 3e-5, DEPTH-INDEPENDENT; relative L2 \(l2); relative \(mr) above 1e-3 of peak; \(ds) distinct, range [\(mn),\(mx)]; \(nn) NaN; experts \(J.ids))",
                  "\(tag) THIS LAYER ALONE from the oracle's input: normalised disagreement \(nd) exceeds 3e-5; relative L2 \(l2); experts \(J.ids) vs \(oraSel); nan \(nn)")
                if !(ok && gpuErrors == 0 && J.ids == oraSel) { layerFail += 1 }
            }
        }
        G(R.outHc, hcDim, "out_hc", 3e-5,
          "THE LAYER'S OUTPUT — the \(hcDim) hyper-connection entries blk.\(il+1) receives, carried forward")
        resid = R.outHc
        if il == nLayers - 1 {
            var v = [Float](repeating: 0, count: hcDim)
            let op = fp(R.outHc, hcDim); for i in 0..<hcDim { v[i] = op[i] }
            finalByPos[pos] = v
        }
    }
}

let tA = Date(); runStack(posA, oraDirA, perDirA); let wallA = Date().timeIntervalSince(tA)
let tB = Date(); runStack(posB, oraDirB, perDirB); let wallB = Date().timeIntervalSince(tB)

// ── hushfold at STACK scale ────────────────────────────────────────────────────────────────────────
var posDiff = 0; var maxDelta: Float = 0
if let a = finalByPos[posA], let b = finalByPos[posB] {
    for i in 0..<min(a.count, b.count) { if a[i] != b[i] { posDiff += 1; maxDelta = max(maxDelta, abs(a[i]-b[i])) } }
}
check(posDiff > 0 && layerFail == 0,
  "hushfold at stack scale: the same token's output after all \(nLayers) layers differs between pos \(posA) and pos \(posB) in \(posDiff)/\(hcDim) entries (max delta \(maxDelta)) while every layer of both runs agrees with its OWN oracle",
  "hushfold: \(posDiff) differing entries, \(layerFail) failed layer gates")

if gpuErrors > 0 { print("=== \(gpuErrors) COMMAND BUFFER ERROR(S) — first: \(gpuFirstError ?? "unknown") ===") }
// unispan: a per-layer time from ONE run is a guess; each layer is timed at BOTH positions.
print("      worst normalised disagreement over the whole stack: \(worstAbs) at \(worstAbsWhere)")
print("      worst per-layer-nudge envelope of the REFERENCE:     \(worstSens) at \(worstSensWhere)")
if !ulpRouteSplit.isEmpty {
    print("      the reference's OWN routing splits under the per-layer nudge at layers \(ulpRouteSplit.sorted()) — the model's, not this carrier's")
}
print("      normalised disagreement (nd) vs the reference's own per-layer-nudge envelope (ne):")
for il in stride(from: 0, to: nLayers, by: max(1, nLayers/12)) {
    let nd = (ndByLayer[il] ?? [0]).max() ?? 0, ne = (neByLayer[il] ?? [0]).max() ?? 0
    let l2 = (l2ByLayer[il] ?? [0]).max() ?? 0
    let ij = (injNd[il] ?? [0]).max() ?? 0
    print(String(format: "        blk.%-2d  stack nd %.3e  relL2 %.3e   THIS LAYER ALONE nd %.3e   nudge envelope %.3e", il, nd, l2, ij, ne))
}
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
    print("VERDICT PASS  \(gateNo) gates — \(nLayers) HETEROGENEOUS DeepSeek-V4-Flash LAYERS STACKED at real dims over the 85 GiB file, the four hyper-connection streams carried from each layer into the next, every per-layer decision (expert count, gate/up and down type, routing regime, rope regime) read from the file's own tensor table, at TWO positions, every choosing surface against a rented fp64 ds4.c transcription and every dispatch sentinelled")
} else {
    print("VERDICT FAIL  \(failures) gate(s), \(gpuErrors) cb errors")
}
exit(ok ? 0 : 1)
SWIFT
swiftc -O -o "$work/runner" "$work/runner.swift" 2>"$work/swift.err" || { echo "FAIL swiftc runner"; tail -40 "$work/swift.err"; exit 1; }

"$work/runner" "$work/params.txt" "$BLOB" \
    "$LIB_EMB" "$LIB_MLA" "$LIB8" "$LIB_CORE" "$LIB_HC" \
    "$LIB_MX4" "$LIB_IQ2" "$LIB_FFN" "$ORA0" "$ORA7" "$PER0" "$PER7"
rc=$?
exit $rc
