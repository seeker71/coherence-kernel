#!/usr/bin/env bash
# metal_ask.sh — the ASK verb's native generation lane: a real llama3.2:3b answer, generated
# form-native, staged with a cost receipt in which EVERY cost appears twice — DECLARED as a bound
# derived from the model's own shape before the run, and MEASURED after, as a fraction of that bound.
#
# WHAT THIS CARRIER IS AND IS NOT. It is a dumb carrier, on purpose and by the same discipline the
# rest of the Metal lane keeps:
#   the BODY  native/metal/ask-declared-cost.fk   every DECLARED bound — the shape read from the
#             file's metadata, the byte totals summed from all 255 tensors at their OWN ggml types,
#             the MAC and dispatch counts folded by form-stdlib/ask-cost-receipt.fk.
#   the BODY  form-stdlib/ask-cost-receipt.fk     the arithmetic AND the rendering of every cost line,
#             so the fraction beside a bound is computed by the cell that computed the bound.
#   the BODY  native/metal/first-token.fk + the kernels  the generation itself.
#   the CARRIER (this file)                       a stopwatch, a subprocess, and a text file.
# It formats no cost line and derives no bound. Every number it contributes was read off a clock or
# counted out of the generation lane's own output.
#
# WHY IT INVOKES metal_first_token.sh RATHER THAN RE-IMPLEMENTING THE LOOP. That harness IS the proven
# generation lane — 13 gates, from "the config is the file's" through "the split and lane paths emit
# the SAME token ids as the attestant". Re-implementing decode here would create a second path that
# nobody gates, and the one that drifts is always the one nobody ran. The cost is that every ask pays
# for the full gate suite (~28 s warm, of which the generation itself is ~1.4 s). That is named here
# as a gap rather than hidden: a lean generation-only runner is a later stone, and it will have to
# prove it emits the same ids as this one before it is allowed to answer anything.
#
# THE TWO DENOMINATORS (corpus row 834, selfgauge). Every rate is quoted against BOTH: our own
# previous rate, and ollama running the SAME model from the SAME blob on THIS machine. A tok/s that
# names only the first is the failure this program spent a day finding. The roofline bandwidth is
# likewise not a vendor brochure number by default — it is DEMONSTRATED: ollama's measured decode rate
# times the bytes a decode token must touch is a bandwidth this machine has actually been seen to
# deliver, and a floor built on it cannot be accused of being aspirational.
#
# JOULES ARE PENDING AND STAY PENDING. The field exists, is named `pending`, and carries the exact
# command that will fill it. /usr/bin/powermetrics needs sudo, which is not grantable unattended. No
# number is estimated from utilisation or TDP: a modelled joule wearing a measurement's name is
# precisely the failure this receipt is built to make impossible.
#
# Run:  form/native/metal/metal_ask.sh [nsteps] ["question"]
# Off-Mac (or with no swiftc, or with no blob) it SKIPs with exit 2, like every Metal row in GPU_GAPS.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"      # .../form
REPO="$(cd "$ROOT/.." && pwd)"
GO_BIN="$ROOT/form-kernel-go/bin-go"
NSTEPS="${1:-12}"
QUESTION="${2:-The capital of France is}"
BLOB="${FORM_GGUF_BLOB:-$HOME/.ollama/models/blobs/sha256-dde5aa3fc5ffc17176b5e8bdc82f587b24b2678c6c66101bf7da77af9f7ccdff}"
MODEL_NAME="${FORM_ASK_MODEL:-llama3.2:3b}"
CACHE="$ROOT/native/metal/.ask-cache"
STAGE="${FORM_ASK_STAGE:-$REPO/.coherence-network/ask-native}"
ARTIFACT="$STAGE/answer.txt"

# THE WORLD'S NUMBER ON THIS MACHINE, same model, same 2 019 377 376-byte blob, measured 2026-07-21
# through ollama's own eval_count/eval_duration:
#     64 tok -> 161.395 tok/s     150 tok -> 159.683     300 tok -> 158.449     150 tok again -> 157.696
# Four points, flat to within 2.3% over a 4.7x span, so this is a RATE and not one sample pretending
# to be a line (corpus row 827, unispan). The 300-token figure is quoted: longest, most amortized.
#
# PREFILL IS QUOTED COLD, AND THAT DISTINCTION IS NOT PEDANTRY. Re-sending the same prompt reported
# 5417 and 5570 tok/s of "prefill" — ollama's prompt cache returning, not a machine ingesting tokens.
# The cold first measurement, 634.79 tok/s over 36 prompt tokens, is the only one that measures what
# the name says. A benchmark that took the warm number would have overstated the world's prefill by
# 8.7x and made our gap look far worse than it is.
WORLD_DECODE_TOKPS_MILLI="${FORM_WORLD_DECODE_TOKPS_MILLI:-158449}"
WORLD_PREFILL_TOKPS_MILLI="${FORM_WORLD_PREFILL_TOKPS_MILLI:-634790}"

if [[ "$(uname -s)" != "Darwin" ]] || ! command -v swiftc >/dev/null; then
    echo "SKIP  no Darwin/Metal toolchain on this host — the native ask lane needs an Apple GPU + swiftc"
    exit 2
fi
if [[ ! -f "$BLOB" ]]; then
    echo "SKIP  the llama3.2:3b GGUF blob is not on this host: $BLOB"
    exit 2
fi
if [[ ! -x "$GO_BIN" ]]; then
    echo "  building go kernel..." >&2
    (cd "$ROOT/form-kernel-go" && go build -o bin-go .)
fi

work="$(mktemp -d "${TMPDIR:-/tmp}/fkask.XXXXXX")"
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
while read -r x; do FILES+=("$x"); done < <(fk_expand native/metal/ask-declared-cost.fk)

# ── 1. the DECLARED shape, from the file. Cached by (cells + blob identity), so a changed recipe or
#       a changed model is a cache MISS by construction and never a stale bound. ────────────────────
mkdir -p "$CACHE"
key="$( { shasum -a 256 "${FILES[@]}" | awk '{print $1}'; printf '%s %s\n' "$BLOB" "$(stat -f %z "$BLOB")"; } | shasum -a 256 | cut -c1-16 )"
SHAPE="$CACHE/shape-$key.txt"
if [[ -s "$SHAPE" ]] && grep -qx 'END' "$SHAPE"; then
    echo "  body cache HIT  declared shape"
else
    echo "  body cache MISS declared shape — walking all 255 tensors at their own ggml types..."
    printf '(adc-emit-shape "%s")\n' "$BLOB" > "$work/shape.fk"
    "$GO_BIN" "${FILES[@]}" "$work/shape.fk" > "$SHAPE.tmp" 2>"$work/shape.err" || {
        echo "FAIL  declared-shape emission failed"; tail -5 "$work/shape.err"; exit 1; }
    grep -qx 'END' "$SHAPE.tmp" || { echo "FAIL  declared-shape stream truncated"; exit 1; }
    mv "$SHAPE.tmp" "$SHAPE"
fi
decl() { awk -v k="$1" '$1=="DECL" && $2==k {print $3}' "$SHAPE"; }
NLAYER=$(decl nlayer); DMODEL=$(decl d); DFF=$(decl dff); NHEAD=$(decl nhead)
NKV=$(decl nkv); HEADDIM=$(decl headdim); VOCAB=$(decl vocab)
WBYTES=$(decl decode_weight_bytes); EMBROW=$(decl embed_row_bytes)
FILE_IMPLIED=$(decl file_bytes_implied); FILE_ACTUAL=$(stat -f %z "$BLOB")

# THE IDENTITY THAT MAKES THE STRIDE RULE MORE THAN AN ASSERTION. GGUF stores no per-tensor byte
# length; the body derived all 255 of them from each tensor's ggml type. Their sum plus the data base
# must be the file's size on disk, EXACTLY. A wrong stride for any one type cannot survive this.
if [[ "$FILE_IMPLIED" != "$FILE_ACTUAL" ]]; then
    echo "FAIL  declared byte identity broken: body implies $FILE_IMPLIED, file is $FILE_ACTUAL"
    echo "      the per-type stride rule in ask-cost-receipt.fk does not describe this file"
    exit 1
fi
echo "  gate D1 the derived tensor bytes + data base ARE the file's $FILE_ACTUAL bytes, exactly"

# ── 2. generate, through the PROVEN lane. The harness is the witness; we read its output. ─────────
echo "  generating through form/native/metal/metal_first_token.sh (the gated lane)..."
gen_t0=$(python3 -c 'import time;print(int(time.time()*1000000))')
"$ROOT/native/metal/metal_first_token.sh" "$NSTEPS" "$QUESTION" > "$work/gen.txt" 2>&1
gen_rc=$?
gen_t1=$(python3 -c 'import time;print(int(time.time()*1000000))')
if [[ $gen_rc -eq 2 ]]; then
    echo "SKIP  the generation lane skipped:"; sed -n '1,3p' "$work/gen.txt"; exit 2
fi
VERDICT=$(awk '/^=== VERDICT/{print}' "$work/gen.txt" | tail -1)
if [[ $gen_rc -ne 0 || "$VERDICT" != *PASS* ]]; then
    echo "FAIL  the generation lane did not pass its own gates — refusing to stage an answer"
    echo "      $VERDICT"; tail -20 "$work/gen.txt"; exit 1
fi
GATES=$(echo "$VERDICT" | sed -E 's/.*PASS — ([0-9]+) gates.*/\1-PASS/')
echo "  the lane passed its own gates: $GATES"

# WHICH PATH ANSWERED. The harness runs the attestant, the split twin and the lane kernel, and they
# are gated to emit the SAME ids. We quote the fastest one that is present, and we NAME it — a
# receipt that silently switched paths between runs would make its own rates incomparable.
PATH_TAG=""
for cand in lane-long split-long long; do
    if grep -q "^  $cand" "$work/gen.txt"; then PATH_TAG="$cand"; break; fi
done
[[ -z "$PATH_TAG" ]] && { echo "FAIL  no generation result line found in the lane's output"; exit 1; }
case "$PATH_TAG" in
    lane-long)  KERNEL_PATH="lane-simd-hoisted";;
    split-long) KERNEL_PATH="split-parts32";;
    *)          KERNEL_PATH="attestant-serial";;
esac
echo "  the answer came from the $KERNEL_PATH path"

# the harness prints, for the chosen path:
#   "  <tag>: prefill A s for N prompt tokens; decode B s for M further forwards"
#   "    ids  : [...]"  "    text : \"...\""  "    END-TO-END X tok/s ... decode-only Y tok/s"
RES=$(grep -A3 "^  $PATH_TAG" "$work/gen.txt" | head -4)
PREFILL_S=$(echo "$RES" | awk 'NR==1{for(i=1;i<=NF;i++) if($i=="prefill"){print $(i+1);exit}}')
NPROMPT=$(echo "$RES"  | awk 'NR==1{for(i=1;i<=NF;i++) if($i=="for" && $(i+2)=="prompt"){print $(i+1);exit}}')
DECODE_S=$(echo "$RES" | awk 'NR==1{for(i=1;i<=NF;i++) if($i=="decode"){print $(i+1);exit}}')
NFWD_DEC=$(echo "$RES" | awk 'NR==1{for(i=1;i<=NF;i++) if($i=="further"){print $(i-1);exit}}')
IDS=$(echo "$RES"      | awk -F': *' '/ids  :/{print $2;exit}')
ANSWER=$(echo "$RES"   | sed -n 's/^ *text *: *"\(.*\)"$/\1/p')
NGEN=$(echo "$IDS" | tr ',' '\n' | grep -c '[0-9]')
DECODE_TOKPS_MILLI=$(grep -A3 "^  $PATH_TAG" "$work/gen.txt" | awk '/decode-only/{for(i=1;i<=NF;i++) if($i=="decode-only"){printf "%d\n", $(i+1)*1000; exit}}')
E2E_TOKPS_MILLI=$(grep -A3 "^  $PATH_TAG" "$work/gen.txt" | awk '/END-TO-END/{for(i=1;i<=NF;i++) if($i=="END-TO-END"){printf "%d\n", $(i+1)*1000; exit}}')

for v in PREFILL_S NPROMPT DECODE_S NFWD_DEC ANSWER; do
    [[ -z "${!v}" ]] && { echo "FAIL  could not read $v out of the lane's output — the harness's format moved"; exit 1; }
done

# ── gate D2: DID THE LANE ANSWER, or only survive. This harness dispatches no Metal of its own —
# its whole GPU-liveness guarantee is inherited from the lane's VERDICT PASS. But a PASS is a
# claim about the lane's gates; STAGING is a claim about the ids we are about to publish, and
# those are different sentences (axiom-4: the answer meets the world through THIS artifact, so
# THIS harness must consult the answer, not only the lane's verdict). The Stone 14 signature is a
# constant id stream — [0,0,0,0], every token the same — which reads as a legal, non-empty answer
# and would stage cleanly. We refuse it here, independently of the lane. A one-token answer cannot
# be constant-tested and is exempted by count, named so the exemption is not silent.
if [[ -z "$IDS" ]]; then
    echo "FAIL  gate D2: the lane passed but emitted no token ids to stage"; exit 1
fi
NDISTINCT=$(echo "$IDS" | tr ',' '\n' | grep -oE '[0-9]+' | sort -u | wc -l | tr -d ' ')
ILLEGAL=$(echo "$IDS" | tr ',' '\n' | grep -oE '[0-9]+' | awk -v v="$VOCAB" 'NF && ($1<0 || $1>=v){c++} END{print c+0}')
if [[ "$ILLEGAL" -gt 0 ]]; then
    echo "FAIL  gate D2: $ILLEGAL of the staged ids are outside [0,$VOCAB) — not legal vocab indices"; exit 1
fi
if [[ "$NGEN" -ge 2 && "$NDISTINCT" -lt 2 ]]; then
    echo "FAIL  gate D2: the lane returned $NGEN tokens but only $NDISTINCT distinct value — a CONSTANT id stream"
    echo "      is the Stone 14 signature of a GPU that produced zeros, not an answer. Refusing to stage it,"
    echo "      independently of the lane's PASS. (A legal, non-empty answer is not proof the GPU ran.)"
    exit 1
fi
if [[ "$NGEN" -lt 2 ]]; then
    echo "  gate D2: $NGEN generated token — too few to test for a constant stream; legality checked, non-constancy exempt by count"
else
    echo "  gate D2 the staged answer is non-degenerate: $NGEN legal ids, $NDISTINCT distinct — not the constant stream a dead GPU stages"
fi

PREFILL_US=$(awk -v s="$PREFILL_S" 'BEGIN{printf "%d", s*1000000}')
DECODE_US=$(awk  -v s="$DECODE_S"  'BEGIN{printf "%d", s*1000000}')
NFWD_MEASURED=$(( NPROMPT + NFWD_DEC ))
NFWD_DECLARED=$(( NPROMPT + NSTEPS ))

# ── 3. the DECLARED per-run bounds, from the body, for both the declared and the measured lengths ──
mkrun() {   # mkrun <nfwd> -> DECL lines
    printf '(adc-emit-run %s %s %s %s %s %s %s %s %s %s)\n' \
        "$NLAYER" "$DMODEL" "$DFF" "$NHEAD" "$NKV" "$HEADDIM" "$VOCAB" "$WBYTES" "$EMBROW" "$1" > "$work/run.fk"
    "$GO_BIN" "${FILES[@]}" "$work/run.fk" 2>/dev/null
}
mkrun "$NFWD_DECLARED" > "$work/decl.txt"
mkrun "$NFWD_MEASURED" > "$work/meas.txt"
dv() { awk -v k="$2" '$1=="DECL" && $2==k {print $3}' "$1"; }
BYTES_D=$(dv "$work/decl.txt" bytes_run);      BYTES_M=$(dv "$work/meas.txt" bytes_run)
MACS_D=$(dv "$work/decl.txt" macs_run);        MACS_M=$(dv "$work/meas.txt" macs_run)
DISP_D=$(dv "$work/decl.txt" dispatch_run_lane); DISP_M=$(dv "$work/meas.txt" dispatch_run_lane)
BYTES_PER_FWD=$(dv "$work/decl.txt" bytes_per_forward)

# ── 4. the ROOFLINE — and TWO bandwidths, because one of them cannot bound us honestly ────────────
# A roofline turns the byte declaration into a refutable TIME: nothing can touch B bytes faster than
# B/bandwidth. Which bandwidth is not a detail.
#
#   DEMONSTRATED  = ollama's measured decode rate x the bytes a decode token must touch. It is
#                   measured on this machine — and it is DERIVED FROM OLLAMA'S RATE, so a tok/s
#                   ceiling built on it is definitionally ollama's rate, and "fraction of ceiling"
#                   would be the same number as "fraction of ollama" wearing a second name. That is a
#                   mechanism that fits the evidence so comfortably nobody looks for the counter-
#                   example, and it is not used as the bound for exactly that reason.
#   VENDOR PEAK   = Apple's stated unified-memory bandwidth for this chip. NOT measured here, and
#                   labelled vendor-stated wherever it appears. It is independent of ollama, of us,
#                   and of anything either of us does, which is what a bound has to be.
#
# The floors below use the VENDOR PEAK. The demonstrated figure is carried as context and as the
# second denominator, never as the bound.
BW_VENDOR_MBS="${FORM_ASK_BW_VENDOR_MBS:-546000}"   # Apple M4 Max, vendor-stated, 546 GB/s
BW_DEMO_MBS=$(awk -v b="$BYTES_PER_FWD" -v r="$WORLD_DECODE_TOKPS_MILLI" 'BEGIN{printf "%d", b*(r/1000)/1000000}')
BW_MBS="${FORM_ASK_BW_MBS:-$BW_VENDOR_MBS}"
PREFILL_FLOOR_US=$(awk -v b="$BYTES_PER_FWD" -v n="$NPROMPT" -v w="$BW_MBS" 'BEGIN{printf "%d", b*n/w}')
DECODE_FLOOR_US=$(awk  -v b="$BYTES_PER_FWD" -v n="$NFWD_DEC" -v w="$BW_MBS" 'BEGIN{printf "%d", b*n/w}')
TOKPS_CEIL_MILLI=$(awk -v b="$BYTES_PER_FWD" -v w="$BW_MBS" 'BEGIN{printf "%d", (w*1000000.0/b)*1000}')
ACHIEVED_MBS=$(awk -v b="$BYTES_PER_FWD" -v n="$NFWD_DEC" -v us="$DECODE_US" 'BEGIN{if(us>0) printf "%d", b*n/us; else print 0}')
PCT_OF_VENDOR=$(awk -v a="$ACHIEVED_MBS" -v w="$BW_VENDOR_MBS" 'BEGIN{printf "%.2f", 100.0*a/w}')
PCT_OF_DEMO=$(awk   -v a="$ACHIEVED_MBS" -v w="$BW_DEMO_MBS"   'BEGIN{printf "%.2f", 100.0*a/w}')

# ── 5. the receipt, rendered BY THE BODY ──────────────────────────────────────────────────────────
printf '(adc-emit-receipt %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s)\n' \
    "$NFWD_DECLARED" "$NFWD_MEASURED" "$BYTES_D" "$BYTES_M" "$MACS_D" "$MACS_M" \
    "$DISP_D" "$DISP_M" "$PREFILL_FLOOR_US" "$PREFILL_US" "$DECODE_FLOOR_US" "$DECODE_US" \
    "$TOKPS_CEIL_MILLI" "$DECODE_TOKPS_MILLI" "$WORLD_DECODE_TOKPS_MILLI" > "$work/rcpt.fk"
"$GO_BIN" "${FILES[@]}" "$work/rcpt.fk" 2>"$work/rcpt.err" | grep -E '^(COST|RATE) ' > "$work/receipt.txt" || true
[[ -s "$work/receipt.txt" ]] || { echo "FAIL  the body did not render a receipt"; cat "$work/rcpt.err"; exit 1; }

# a few carrier-measured context lines, clearly marked as context and not as declared/measured pairs
{
  echo "CTX kernel_path $KERNEL_PATH"
  echo "CTX bytes_per_forward $BYTES_PER_FWD"
  echo "CTX bandwidth_mbs_vendor_stated $BW_VENDOR_MBS (Apple M4 Max unified memory; vendor-stated, NOT measured here; the bound above uses this)"
  echo "CTX bandwidth_mbs_demonstrated $BW_DEMO_MBS (ollama decode ${WORLD_DECODE_TOKPS_MILLI}/1000 tok/s x $BYTES_PER_FWD B/token, measured here; NOT used as the bound - it is derived from ollama)"
  echo "CTX bandwidth_mbs_achieved_by_us $ACHIEVED_MBS"
  echo "CTX bandwidth_pct_of_vendor_peak $PCT_OF_VENDOR"
  echo "CTX bandwidth_pct_of_demonstrated $PCT_OF_DEMO"
  echo "CTX prompt_tokens $NPROMPT"
  echo "CTX generated_tokens $NGEN"
  echo "CTX end_to_end_tokps_milli $E2E_TOKPS_MILLI"
  echo "CTX world_prefill_tokps_milli_cold $WORLD_PREFILL_TOKPS_MILLI"
  echo "CTX token_ids $IDS"
} >> "$work/receipt.txt"

# ── 6. stage the artifact, bound to THIS question by its own sha256 ───────────────────────────────
QSHA=$(printf '%s' "$QUESTION" | shasum -a 256 | cut -d' ' -f1)
BSHA=$(basename "$BLOB" | sed 's/^sha256-//')
mkdir -p "$STAGE"
{
  echo "ASK-NATIVE v1"
  echo "LANE metal-gguf-native"
  echo "QUESTION-SHA256 $QSHA"
  echo "MODEL $MODEL_NAME"
  echo "BLOB-SHA256 $BSHA"
  echo "GATES $GATES"
  echo "KERNEL-PATH $KERNEL_PATH"
  echo "RECEIPT-BEGIN"
  cat "$work/receipt.txt"
  echo "RECEIPT-END"
  echo "ANSWER-BEGIN"
  printf '%s\n' "$ANSWER"
  echo "ANSWER-END"
} > "$ARTIFACT"

echo
echo "=== staged: $ARTIFACT ==="
echo "question: \"$QUESTION\""
echo "answer  : \"$ANSWER\""
echo
cat "$work/receipt.txt"
echo
echo "=== VERDICT PASS — native ask lane staged, bound to its question, every cost declared and measured ==="
