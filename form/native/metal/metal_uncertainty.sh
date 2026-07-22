#!/usr/bin/env bash
# metal_uncertainty.sh — STONE 43. What the local model's OWN logit vector says about how decided it
# was, measured on the real llama3.2:3b forward that metal_first_token.sh already proves.
#
# WHAT THIS IS, exactly: a READ-ONLY EXTENSION of metal_first_token.sh. It does not fork that file and
# it does not edit it. It reads it, asserts that four anchor lines are still there character for
# character, splices a measurement block into a COPY, and runs the copy. If any anchor has moved the
# splice REFUSES rather than patching something else — a fork would silently drift instead, and the
# thing it would drift away from is the only proven forward this program has.
#
# WHY THE EXTENSION AND NOT A NEW RUNNER: the logit vector is already computed, on the device, in a
# host-visible buffer, one dispatch before the argmax reads it. Every uncertainty signal in this stone
# is a property of THAT vector. A second runner would have to re-prove it computes the same logits,
# forever, against drift nobody watches — metal_first_token.sh's own FORM_GEN_ONLY header makes that
# argument about itself, and this file keeps it.
#
# WHAT IS MEASURED, per forward (all from one host-side read of `bLogits`, vocab 128 256):
#   margin      logit_1 - logit_2 — the top-1 margin, in logit units
#   p1          softmax probability of the chosen token = exp(l1 - logsumexp)
#   entropy     H = logsumexp - E_p[l], the full-distribution entropy in nats (max ln 128256 = 11.762)
#   entropy_k   the same over the top 32 only, renormalized — the cheap truncation
#   sd          standard deviation of the whole logit vector — the aliveness witness
#   ties        how many entries equal the maximum
#   alive       the DEAD-FORWARD GUARD: all entries finite, exactly one maximum, sd above a floor,
#               and the CPU's argmax agrees with the GPU's. A zeroed buffer has margin 0, sd 0 and
#               128 256 ties; an unwritten one full of one repeated garbage value has the same shape.
#               Neither may ever read as "confident" — so `alive` is reported beside every number and
#               local-uncertainty.fk refuses to score a step that is not alive.
#
# probetoll (corpus row 858): the instrument charges the cost it measures. Every UNC line is produced
# by ~4 host passes over 128 256 floats; UNCCOST prints that price in seconds and per-step milliseconds
# next to the decode time it perturbs, and the same run prints the decode rate it actually achieved.
#
# Run:  form/native/metal/metal_uncertainty.sh [nsteps] ["prompt"]
# Emits the same PASS/VERDICT lines as metal_first_token.sh (FORM_GEN_ONLY's 9-gate suite) plus the
# UNC block. Off-Mac it SKIPs with exit 2 exactly as its host does.
set -uo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
HOST="$HERE/metal_first_token.sh"
PATCHED="$HERE/.metal_uncertainty_patched.sh"     # dot-prefixed, regenerated every run, never committed

[[ -f "$HOST" ]] || { echo "FAIL  the host harness is missing: $HOST"; exit 1; }

python3 - "$HOST" "$PATCHED" <<'PY' || exit 1
import sys
src, dst = sys.argv[1], sys.argv[2]
t = open(src).read()

BLOCK = r'''
// ==== STONE 43 (metal_uncertainty.sh, read-only extension): the logit vector's own decisiveness ====
// Spliced in by metal_uncertainty.sh. metal_first_token.sh itself is never modified. Everything below
// reads `bLogits` AFTER the token's single command buffer has completed, so the bytes are final.
let uncOn = ProcessInfo.processInfo.environment["FORM_UNC"] == "1"
var uncRecording = false
let uncK = 32                       // the truncation width for the cheap entropy
struct UncStat {
    let id: Int, ties: Int
    let l1: Double, l2: Double, margin: Double, p1: Double, ent: Double, entK: Double, sd: Double
    let alive: Bool
}
var uncStats: [UncStat] = []
var uncSeconds = 0.0
// PRICED SEPARATELY, because the stone has to CHOOSE one signal and "cheap matters: this runs per
// token" is a claim about seconds. tScan is the max/second-max/sd pass every signal needs. tExp is
// the full-vocabulary logsumexp pass that p1 and the full entropy need (128 256 exp() calls). tTopK
// is the selection pass plus 32 exps. A signal's real price is tScan plus its own column.
var uncTScan = 0.0, uncTExp = 0.0, uncTTopK = 0.0
func uncMeasure() {
    let t0 = Date()
    let p = bLogits.contents().bindMemory(to: Float.self, capacity: vocabN)
    var m = -Double.infinity, m2 = -Double.infinity
    var iMax = -1, ties = 0, nonFinite = 0
    var sum = 0.0, sumsq = 0.0
    for i in 0..<vocabN {
        let v = Double(p[i])
        if !v.isFinite { nonFinite += 1; continue }
        sum += v; sumsq += v * v
        if v > m { m2 = m; m = v; iMax = i; ties = 1 }
        else if v == m { ties += 1 }
        else if v > m2 { m2 = v }
    }
    let n = Double(vocabN)
    let mean = sum / n
    let sd = max(0.0, sumsq / n - mean * mean).squareRoot()
    let tA = Date(); uncTScan += tA.timeIntervalSince(t0)
    // one more pass: the softmax denominator and E_p[l], both shifted by the max exactly as
    // form-stdlib/transformer-numerics.fk's tn-softmax shifts it.
    var Z = 0.0, wsum = 0.0
    for i in 0..<vocabN {
        let v = Double(p[i]); if !v.isFinite { continue }
        let e = exp(v - m); Z += e; wsum += e * v
    }
    let lse = m + log(Z)
    let p1 = exp(m - lse)
    let ent = lse - wsum / Z                     // H = logsumexp - E_p[l], in nats
    let tB = Date(); uncTExp += tB.timeIntervalSince(tA)
    // the top-k truncation, renormalized over the k kept
    var top = [Double](repeating: -Double.infinity, count: uncK)
    for i in 0..<vocabN {
        let v = Double(p[i]); if !v.isFinite { continue }
        if v > top[uncK - 1] {
            var j = uncK - 1
            while j > 0 && top[j - 1] < v { top[j] = top[j - 1]; j -= 1 }
            top[j] = v
        }
    }
    var Zk = 0.0, wk = 0.0
    for v in top { let e = exp(v - m); Zk += e; wk += e * v }
    let entK = (m + log(Zk)) - wk / Zk
    uncTTopK += Date().timeIntervalSince(tB)
    let chosen = Int(bOutI.contents().bindMemory(to: UInt32.self, capacity: 1)[0])
    // THE DEAD-FORWARD GUARD. A zeroed pool, an unrun dispatch and a NaN blowout all produce a vector
    // an unguarded margin/probability would happily score. None of them may read as decided.
    let alive = (nonFinite == 0) && (ties == 1) && (sd > 1e-6) && (m > m2) && (chosen == iMax)
    uncStats.append(UncStat(id: chosen, ties: ties, l1: m, l2: m2, margin: m - m2,
                            p1: p1, ent: ent, entK: entK, sd: sd, alive: alive))
    uncSeconds += Date().timeIntervalSince(t0)
}
func uncReport(_ nPrompt: Int) {
    print("=== UNCERTAINTY — the logit vector, per forward (STONE 43) ===")
    print("  UNCPROMPT \(nPrompt) prompt forwards; step < \(nPrompt) is prefill, and step \(nPrompt - 1) is the FIRST CONTENT TOKEN's decision")
    print("  UNCVOCAB \(vocabN) ln(V)=\(String(format: "%.6f", log(Double(vocabN)))) topk=\(uncK)")
    print("  UNCHEAD step id margin p1 entropy entropy_topk top1 top2 sd ties alive text")
    for (i, s) in uncStats.enumerated() {
        print(String(format: "  UNC %d %d %.6f %.6f %.6f %.6f %.6f %.6f %.6f %d %d",
                     i, s.id, s.margin, s.p1, s.ent, s.entK, s.l1, s.l2, s.sd, s.ties, s.alive ? 1 : 0)
              + " [" + decodeIds([s.id]) + "]")
    }
    let n = max(1, uncStats.count)
    print(String(format: "  UNCCOST %.6f s of host CPU across %d measurements = %.3f ms each — the instrument's own price, charged (row 858)",
                 uncSeconds, uncStats.count, 1000.0 * uncSeconds / Double(n)))
    print(String(format: "  UNCCOSTSPLIT per step, ms: scan(max,2nd,sd) %.3f | full-vocab exp pass (p1, entropy) %.3f | top-%d select+exp (entropy_topk) %.3f",
                 1000.0 * uncTScan / Double(n), 1000.0 * uncTExp / Double(n), uncK,
                 1000.0 * uncTTopK / Double(n)))
    print(String(format: "  UNCPRICE per step, ms: margin %.3f | p1 %.3f | entropy %.3f | entropy_topk %.3f  (each = scan + its own pass)",
                 1000.0 * uncTScan / Double(n), 1000.0 * (uncTScan + uncTExp) / Double(n),
                 1000.0 * (uncTScan + uncTExp) / Double(n),
                 1000.0 * (uncTScan + uncTTopK) / Double(n)))
}
// ==== end STONE 43 extension ====
'''

ANCHORS = [
    ("func forward(_ id: Int, _ pos: Int) -> Int {", BLOCK + "\n", "before"),
    ("    return Int(bOutI.contents().bindMemory(to: UInt32.self, capacity: 1)[0])",
     "    if uncRecording { uncMeasure() }\n", "before"),
    ("    usePartsNow = 1; fastOps = true; mvNow = .slot",
     "\n    uncRecording = uncOn", "after"),
    ("                 encodeS, g.prefill, promptIds.count, g.decode, g.forwards, encodeS + g.prefill + g.decode))",
     "\n    if uncOn { uncReport(promptIds.count) }", "after"),
]

for anchor, ins, where_ in ANCHORS:
    n = t.count(anchor)
    if n != 1:
        sys.stderr.write("REFUSED  the anchor is not unique (%d occurrences) in %s:\n  %s\n"
                         "         metal_first_token.sh has drifted; this extension will not guess.\n"
                         % (n, src, anchor))
        sys.exit(1)
    t = t.replace(anchor, (ins + anchor) if where_ == "before" else (anchor + ins), 1)

open(dst, "w").write(t)
print("  splice OK — 4 anchors matched exactly, %d bytes -> %d" % (len(open(src).read()), len(t)))
PY

chmod +x "$PATCHED"
echo "=== running the patched carrier (FORM_GEN_ONLY=1 FORM_UNC=1) ==="
FORM_GEN_ONLY=1 FORM_UNC=1 "$PATCHED" "$@"
rc=$?
exit $rc
