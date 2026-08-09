# 2026-08-09 — rungs 10-14: the MoE, the frames, the head, the joined layer, and the stack

The ladder continued from receipts/2026-08-08-ds4-second-cache-form.md (rung 9, band 274877906943,
re-witnessed at this session's start: 274877906943, exit 0, on the canonical imatrix file,
86,720,111,488 bytes). Everything below is a number a run printed. Model file: the canonical
imatrix GGUF only. Pure Form + Form-emitted MSL through the handle door; the door did not grow
mid-flight (no metal_enqueue_seq in the carrier; binding string base/base+8/base+16 as documented).

## Rung 10 — the MoE, both routing regimes  [see battery; probes re-witnessed at close]

blk.0 (hash) and blk.3 (learned) at token 671, embed -> ffn_norm rmsnorm as the activation
(declared; the real wiring is rung 13's). Ground truth read from the file and pinned:
- every layer's gate/up experts are IQ2_XXS [4096x2048x256] and every down is Q2_K
  [2048x4096x256] — dim[2] = 256 at ALL 43 layers on this file (no REAP pruning here; that lives
  on the reap25 specimen). Strides by arithmetic: 553648128/256 = 2162688, 704643072/256 = 2752512.
- expert_used_count 6, expert_weights_scale 1.5, gating func 4 (sqrt-softplus), hash_layer_count 3,
  swiglu_clamp_exp = 10.0 at all 43 layers (Go-arm array walk).
- token 671's hash row, read independently (raw bytes at 5333824 + 671*24): 147 78 30 248 217 179
  — matches dsv4-layer-real.fk's documented forepick. Expert 147's slice pinned by arithmetic:
  14641984 + 147*2162688 = 332557120.
Measured (baseline): router logits F16-fold BIT-EQUAL 8 rows; probs 157 ppb; weights within 4 ulp
(below); iq2 gate/up 325/244 ppb (12 sampled rows each, iq2xxs-dequant's carver in fp64); swiglu
98 ppb + the clamp witnessed ON DEVICE (gate=up=12 > lim=10: got 99.995461 vs clamped 99.99546 vs
unclamped 143.999115); q2k down 611 ppb; reduce EMULATED bit-equal 4096/4096; shared lane 107 ppb
+ shared swiglu 94 ppb; final add bit-equal 4096/4096. blk.3: logits bit-equal, probs 296 ppb,
sc = probs+bias bit-equal 256/256, tg6 selection reproduced by the kernel's own scan over the
GPU's sc — ids EQUAL (54 116 197 77 133 95) — weights within 4 ulp computed from PROBS, never sc.

## The instrument that had to be re-chosen twice: fast-math division

The weight check w = p/s * 1.5 was first written as BIT EQUALITY against fq-quant emulation:
blk.0 read 6/6, blk.3 read 2/6, mismatches all one ulp. First diagnosis: double rounding in the
emulation (fp64 then f32) — repaired with an EXACT instrument, the residual argmin (the correctly
rounded f32 quotient minimizes |p - c*s|, which is exact fp64 because c*s is a 48-bit product;
ties to the even mantissa). Still 2/6. The real cause is one level deeper: THE DOOR COMPILES
options:nil AND THE ORACLE'S OWN METALLIBS COMPILE WITH ONLY -ffp-contract=off — both run Metal
fast-math division (reciprocal-shaped, <= 2.5 ulp by the MSL precision table). A correctly-rounded
reference cannot be bit-equal to it, on our lane or the reference's; the recorded stream's own
division is the fast one. So the check became a DERIVED bound: 2.5 (div) + 0.5 (the *wscale round)
with margin -> 4 ulp of the correctly-rounded weight. The bias-in-weight asymmetry it exists to
catch sits ~1e6 ulp away. blk.0's first 6/6 was luck, and only running the second regime exposed it.

## Rung 11 — the hyper-connection frame  [see battery]
blk.0 attn frame, real weights, HC_ITERS=20, HC_EPS=f32(1e-6): broadcast bit-equal 16384/16384;
rmsnorm-nw 100 ppb; hc_attn_fn matvec EMULATED bit-equal all 24 rows; split+Sinkhorn(20) vs
dsv4-hc.fk's recipe 104 ppb — the Sinkhorn CONVERGES, so 20 iterations do not accumulate error;
slice copies bit-equal; wsum EMULATED bit-equal 4096/4096; hc_post EMULATED bit-equal 16384/16384
including the transpose read comb[dst + src*n_hc] on asymmetric real weights.

## Rung 12 — the exit head  [see battery]
rmsnorm-nw 100 ppb; output_hc_fn matvec bit-equal 4/4 rows; headw sigmoid 85 ppb; collapse
bit-equal 4096/4096; output_norm 30/58 ppb; Q8_0 unembedding 136 ppb at 8 rows including the
argmax row and runner-up. Head-on-embedding argmax 123327/20.83 (runner 83480/19.73) — a DECLARED
stand-in input; the model's argmax is rung 14's, not this rung's.

## Rung 13 — one complete layer, end to end  [see battery]
blk.0 at pos 0: every seam checked against the GPU's actual upstream buffers: attn wsum bit-equal,
attn_norm 210 ppb, kv round 512/512, attend 1134 ppb, output_b 146 ppb, attn hcPost bit-equal
16384, ffn wsum bit-equal, ffn_norm 57 ppb, iq2 gate 208 ppb, reduce/add/out-post bit-equal, both
splits 104/136 ppb. THE ATTEND BAR IS 4e-6 AND DERIVED: mla_exp is a 14-term f32 Taylor with
argument halving (~20 roundings) whose reduction crosses simd_sum (a hardware tree no fp64 fold
can reproduce); rungs 5/8 measured 886/882 ppb — always ~90% of the generic bar. The bar is sized
from the series, not from the miss.

## Rung 14 — the stack and the stream

native/metal/dsv4-decode-stack.fk holds the loop (no new arithmetic; it preludes the proof cell).
First witness: FIRST TOKEN = 11111 (" Paris"). Then the stream, 25 steps:

    11111 16 455 6102 294 8760 344 (x3) 11111 16 455 6102 294

**24/24 identical to ds4's recorded ids and 25/25 including the bash lane's own recorded 25th
(receipts/2026-07-30-order-match.md), plus a 26th (294) continuing the period-7 cycle.** The run:
67,014 dispatches over 30 positions (~2,234/token), 26 syncs — ONE per generated token — and
1432 handles (1202 nocopy file views + 230 device buffers) of the 8192 ceiling, pipelines 36.
No timing is claimed: the run shared the machine with the mutation battery and a fleet sibling
(the floorwalk discipline; speed comes after, quiet).

## Deviation, named
Rungs 10-13 land in one commit rather than four. The batteries are full-band runs (~45 min each on
this day's loaded machine); landing each rung separately would have serialized ~10 hours of
apply-run-revert before the stack could be attempted inside this session's budget. Each rung was
probed green independently before banding, the rung-10-only band state was baselined at exactly
2^45-1, and the battery below runs against the full band. The per-rung history lives in this
receipt rather than in four commits; fabricating intermediate file states mechanically risked the
shapeblind failure the previous receipt documents, and I chose the honest deviation over the
cosmetic history.

## Mutation ledger skeleton (predictions WRITTEN BEFORE the runs; full = 2^53-1 = 9007199254740991)
| mutation | predicted | actual |
|---|---|---|
| baseline rungs 1-13 | 9007199254740991 | **9007199254740991** |
| V1 down-experts handed the gate stride (both call sites, global sed) | 9002801208229887 (only 2^42) | **9002801208229887** |
| V2 hash select told token+1 (both sites) | 8999502673346559 (2^40 + gated 2^41, 2^42; rung-13 references follow the GPU's ids buffer and stay green — the pin literals own the catch) | **8999502673346559** |
| V4 the weight REFERENCE fed the biased score sc instead of probs | 8989607068696575 (2^44 alone: proves the check can SEE the bias asymmetry) | **8989607068696575** |
| W1 Sinkhorn told iters=1 (three sites: rung-11 + both rung-13 frames) | 4362862139015167 (2^47 AND 2^52) | **4362862139015167** |
| W2 hc_post handed flat for residual (rung-11 site) | 8725724278030335 (2^48 alone) | **8725724278030335** |
| N unlinked door (plain fkwu) | 0 | **0** (all 54 bits dark, 2.6 s — bit 1 gates every runner, so the numb arm refuses in seconds and fabricates nothing) |

## Frontier question, and my answer

**Q: What may an equality check assume about the machine underneath it?**

Only what is PINNED. Rung 9's rule said: when the subject's arithmetic is exactly emulable, the
honest check is equality. Today found the boundary of "emulable": it is not a property of the
arithmetic alone but of WHO OWNS ITS ROUNDING. An add or a multiply on this device is IEEE and the
compiler leaves it alone (the reduce, the wsum, the hc_post all held bit equality across tens of
thousands of lanes); a division under fast math belongs to the compiler, which may lower it to a
reciprocal shape — on our door AND in the oracle's own metallibs, because -ffp-contract=off pins
contraction and nothing else. An instrument that demands bit equality through an unpinned op is
not exacting, it is wrong — twice over, since the value it would demand is one no lane in the
comparison actually computes. The ladder of instruments is: equality for pinned ops, derived
bounds (from the op's own documented ulp or the series' own rounding count) for unpinned ones,
and the denominator argued only after the instrument is chosen.

## PROPOSED distillation row — not landed here (single-writer; max-mid re-derived at close:
## corpus tops at 996; 997 double-held (lanewake pipestamp + the second-cache receipt), 998
## seamclock (floorwalk), 999 cacheflatter (streamwake, COMMITTED in 11e18b34d — the draft's
## 999 collided with it and renumbered here) — so this takes 1000; renumber again at reunion
## by the anastomosis rule

; 1000 — unpinnedop. A weight check demanded BIT EQUALITY against an exact
; emulation of p/s*w and read 6/6 on one layer, 2/6 on the next, misses all one
; ulp. The first repair fixed the EMULATION's double rounding (residual argmin:
; the correctly rounded quotient minimizes |p - c*s|, exact in fp64) — and the
; mismatch stayed, because the defect was one level deeper: the door and the
; oracle's own metallibs both compile fast-math division, an op the COMPILER
; owns, lowered to a reciprocal shape no IEEE emulation can equal. Equality is
; for PINNED ops (the adds and muls held it across 16384 lanes); an op whose
; rounding the compiler owns gets a bound DERIVED from its documented ulp. The
; luck of 6/6 on the first layer was the trap: one regime's pass is not the
; instrument's proof.
; "unpinnedop" — 0 hits in corpus before this row.
; (walk: overfine 934 argued the denominator; shapeblind 997 (proposed) caught
;  the summary hiding the shape; this row is about WHO owns the rounding.)
(hdc-row 1000 20260809
    (list "when" "is" "bit" "equality" "the" "wrong" "demand")
    "unpinnedop"
    "unpinnedop"
    "rented-oracle")

## (a) The most surprising teaching

**The right repair at the wrong depth left the mismatch standing — and the passing layer was the
misleading one.** The weight check read 6/6 on blk.0 and 2/6 on blk.3 from the SAME emulation. I
diagnosed double rounding in my own fp64-then-f32 division, built the exact instrument (residual
argmin, provably correct rounding), and the mismatch did not move — because the defect was not in
my rounding at all: the DEVICE's division is fast-math, on this door and in the oracle's own
metallibs alike, and no IEEE emulation at any depth can equal an op the compiler lowered to a
reciprocal. The surprise is not that the check failed; it is that blk.0's 6/6 pass was LUCK — six
quotients that happened to land where fast and correct rounding agree — and without the second
routing regime in the same rung, the wrong instrument would have shipped green.

## (b) Where discomfort turned to gold

Giving up bit equality felt like weakening the band — equality was the rung-9 upgrade, the
strongest instrument this ladder had, and the 4-ulp bound reads like a retreat. Staying with the
discomfort instead of either keeping a check that could never pass or silently widening a
tolerance produced the sharper rule: equality is for PINNED ops, and the adds and multiplies
proved they are pinned by holding bit equality across every lane asked (reduce 4096/4096, wsum
4096/4096, post 16384/16384, sc 256/256); the division is not, and its bound is DERIVED from the
MSL precision table, not negotiated from the miss. The band came out stronger in the honest place:
it now states which operations it may demand equality of, and why.

## Second witness (the session that wrote the draft above exited mid-landing; a successor
## re-ran everything below on 2026-08-09 afternoon before believing it)

Re-witnessed, fresh caches, rebuilt fkwu-metal from source (the documented cc line):
- FIRST TOKEN = 11111 (" Paris"), exit 0, 13,356 dispatches / 2 syncs over 6 positions,
  last_error=none, 821.73 s wall (cold compile).
- THE STREAM: 26 generated ids, `STREAM MATCH count vs recorded 25 = 25` — 25/25 against the
  receipt-recorded ids, plus the 26th (294) continuing the period-7 cycle. 67,014 dispatches
  over 30 positions, 26 syncs (one per generated token), 1432 handles (1202 nocopy file views +
  230 device buffers) of the 8192 ceiling, 36 pipelines, last_error=none. 1137.39 s wall /
  973.03 s user.
- Rung probes re-run warm: moe-probe / hc-probe / exit-probe / layer-join-probe all exit 0;
  the pins printed match the band's literals (expert 147 slice 332557120 = 14641984 +
  147*2162688; hc frame 79129609376 / 79130395808; exit head 86157337440 / 86157468512; the
  embed row pin 77933529920 in all four).
- The stream ids and the second prompt's ids below were re-decoded through the body's own
  tokenizer cell today; both verbatim strings reproduce exactly.

## ms/token, said honestly at last — and the two ledgers the spread demands

The ids match, so the speed conversation may start. Steady state today: the two witnessed
walls differ by 24 positions with the compile common-moded, (1137.39 - 821.73) / 24 =
**13.2 s wall per token** (load avg ~5-6, shared machine, one sync per token). The reference
lane's recorded ~28 t/s is 35.7 ms/token. The gap is ~370x, and the two ledgers below say
where it lives — Ledger A is what the next id REQUIRES, Ledger B is where the 13.2 s actually
goes, and the diff is the work order.

**Ledger A — bottom-up, from the file's own 1328-row tensor table (gm-emit-manifest, re-run
today), the minimal per-token set:**

| requirement | amount | source |
|---|---|---|
| dense blk projections + norms + exit head | 7,733,014,876 B | full bytes of every non-expert tensor |
| routed experts, 6 of 256 | 1,826,095,104 B | 77,913,391,104 x 6/256 |
| embed row + hash rows | 8,192 + 1,032 B | one F16 row; 24 B x 43 layers |
| KV traffic at pos<30 | ~MBs | grows with position; negligible here |
| **bytes total** | **9,559,119,204 B** | |
| floor at flat 477 GB/s | **20.0 ms/token** (50 t/s) | big-span bus, this machine |
| floor span-priced (borrowed rates) | ~25-29 ms/token | dense ~460-470 GB/s (8-34 MB spans); expert slices are 2.1-2.7 MB and the dense walk read 1-4 MB spans at 96-231 GB/s — a DS4-shaped span walk is the missing instrument |
| dispatches | ~500-700 (fused shape, ~10-16/layer) | dense lanes' proven fusion; current cell runs 2,234 |
| host crossings | 4 B out (the id); 0 in | with a resident id chain: N x 4 B once per generation |
| syncs | 1 per generation | current: 1 per token |

**Ledger B — top-down, where 13.2 s/token actually goes. The first attribution (bindings)
was WRONG and the ladder of small looks killed it in minutes — recorded because the misdiff
is the teaching:**

The method had to change mid-session (Urs: the looking needs to change by 4 orders of
magnitude). A progressive ladder replaced the 19-minute full-stream instrument: level 1 = one
dispatch, level 2 = its decomposition, level 3 = one position through 43 layers, full stream
only as a landing seal. Levels 1-2 close in SECONDS (dsl-ladder-probe.fk, ~35 s compile, ~2 s
of measured questions) and they overturned the presumed ledger before any deletion was coded:

| category | measured (ladder, this machine) | evidence |
|---|---|---|
| binding construction | **1-2 us per dispatch => ~5 ms/token** | L2a/L2c x1000: fresh dfd-u32/str_concat binding + enqueue = 2 us — NOT the seconds the first draft presumed |
| enqueue seam | **<1 us per call** | L2b x1000 prebuilt binding |
| host argmax pair, interpreted | **~2.4 s/token** | L2d: dfd-argmax-go + dfd-arg2-go over 129,280 real-byte lanes = 1220 + 1224 ms; same on zero bytes; FLAT after a 2M-node heap bloat (L2e — the heap-growth theory measured dead in one run) |
| one Q8_0 34 MB matvec (plain lane) | 125-250 us (floor 74 us; 142-285 GB/s) | L1 x64 — the plain kernel is ~2x off its span floor; the grouped lane is the stack's own oa lane |
| GPU busy on required bytes | ~20-30 ms/token | Ledger A |
| GPU busy on EXTRA work | **0** | no fp64 reference arithmetic rides the decode path; verification lives in the band only |

And then the level-3 living session (dsl-l3-probe.fk: setup timed by phase once, then every
position split into encode / drain / head) CAUGHT THE 13.2 AS AN ARTIFACT. Both stream runs
carried an unmeasured ~720 s common term — dfd-st-build's setup — and subtracting two big
walls that share an unmeasured term put its load-variance into the "per-token" quotient. The
direct look, 43 layers, per position: **encode 5-10 ms, drain 483-504 ms prefill / 1.37 s
decode, head 1.59-1.85 s** — the token costs ~3.0 s, not 13. (The same session exposed its
own first defect the honest way: my inlined copy built the kv-round pipeline from the MLA
unit instead of dfd-kvq-msl, handle 0, wrong ids, `last_error` named it, and a 34-line
pipes-probe found it in one look — dsl-pipes-probe.fk stays as the door's handle-census
instrument.)

**THE HOG, cornered and killed the same hour (Urs's two witnessed cuts pointed at it):
SETUP views_43_layers_ms=720617.** dfd-st-build re-walked the 1328-row header ~50 times per
layer through egg-find-tensor — the same disease the dense lanes cured with the one-walk
dth-table (74-96 s -> 1.6-28.5 s) and the stack cell had never inherited. The one-walk table
(dfd-stt, one pass, rows (name type d0 d1 d2 absoff bytes)) landed in dsv4-decode-stack.fk:
table build 399-404 ms, **views 720,617 -> 11,912 ms (60x)**, session open ~13 s, and the full
43-layer level-3 gate — with id correctness — now runs in **69.5 s wall** (12.7 s user), the
seconds-scale iteration cadence the method correction demanded. The first six greedy ids
through the rebuilt setup: 11111 16 455 6102 294 8760 — the recorded stream's own. A second
misattribution died with it: the "~700 s compile" was never compile — compile is ~30-60 s;
the 700 s was always this walk, hiding inside every run's opening silence.

The landing seal on the table-rebuilt stack: **STREAM MATCH 25 = 25, exit 0, the whole
30-position run in 150.25 s wall** — the same seal that took 1137.4 s this morning. Change
landed, seal green, 7.6x on the full run before a single kernel was touched.

The plain-arm stream (all metal calls numb, isolated tree): 824.4 s wall / 812.2 s user for
the same 30-position walk — CPU-bound end to end, and the unlinked door generates 26 zeros
with STREAM MATCH = 0: the numb arm fabricates nothing.

**The diff, as a work order — RE-RANKED at close by the level-3 measurements (largest
first; the stream's 25/25 is the only seal a change may claim). The measured spend per
token after the table landed: head 1.6-1.85 s, decode drain 1.37 s, encode 7 ms — so the
order below is the measured order, and the original interpreter-loop item fell from first
place to a 7 ms footnote:**

0. (LANDED THIS SESSION, stream-sealed) the one-walk tensor table — setup 720.6 s -> ~13 s,
   full level-3 gate 69.5 s, iteration cadence restored.
0b. NEXT LARGEST, ~1.8 s: GPU argmax kernel + resident id chain (dense round three's landed
   pattern) — the head's 517,120-byte read and 2 x 129,280 interpreted f32-decodes become a
   4-byte read.
0c. THEN ~1.37 s: the decode drain — its floor is ~30 ms of bytes + ~27 ms of stage quanta
   (2,234 x ~12 us); what the other ~1.3 s is, is the ladder's next single question (the
   prefill drain is 0.48 s at the same dispatch count — the difference between them is
   where to look first).
0d. Session open 13 s -> ms: write the resolved state (table + view offsets) to a flat
   artifact once, load bytes after (the .metallib-cache precedent).
Below, the original order as first drafted — its item 1 (per-token interpreter construction)
MEASURED at 7 ms/token and is no longer the lead; its sub-answers stand:
1. Per-token interpreter construction -> ZERO. The design intent (to validate, resize, or
   refuse with numbers — round three refused enqueue_seq the same way): the dispatch sequence
   becomes a Form-AUTHORED program emitted once at setup — every pipeline ref, binding, grid
   shape — replayed natively per token (the CUDA door's shape: Form authors, the organ
   replays and decides nothing). Position and the id chain live on device so every binding
   byte is token-invariant. TWO OPEN QUESTIONS SIT UNDER THIS, in order:
   a. The feared load-bearing unknown — expert selection varying per token breaking binding
      invariance — is ALREADY ANSWERED IN THE BODY, found by reading before probing:
      form_dsv4_iq2_matvec_experts4 and form_dsv4_q2k_matvec_experts (ds4-order-match.fk)
      take `device const uint* ids [[buffer(3)]]` and compute `ebase = ids[slot] * stride`
      INSIDE the kernel; the routing kernels write idsb/wtsb on device; bit 2^41/2^42's fp64
      references compute their slice addresses FROM the GPU's own ids buffer readback
      (dfd-iq2e-md). Expert selection never touches the host in the current decode path, and
      every MoE dispatch's binding is ALREADY token-invariant. What is NOT yet invariant is
      exactly two families: the embed gather's host-fed id offset (token*8192 — the dense
      dequant_idx/copy_u32 pattern removes it) and the position family (rope pos, kv-append
      index, attend window count, comp state row pos%ratio, posbits) — a device position
      counter a tiny kernel increments, read by those kernels, removes the rest.
   b. ICB is NOT assumed viable: Metal indirect command buffers do not take setBytes-style
      inline constants, and this door's binding contract rests on setBytes for constants —
      an ICB path is a door-contract change (constants into device buffers), not an
      acceleration. Probe supportIndirectCommandBuffers and the legal binding forms; choose
      ICB vs carrier-side replay with a measurement and the constraint written down.
2. Host argmax + 517 KB readback -> a GPU argmax kernel + resident id chain + form_copy_u32
   into the embed gather (dense round three's landed pattern), host reads N x 4 B once per
   generation.
3. Dispatches 2,234 -> ~600 by stage fusion under the bit-exact boundary (the dense lanes'
   fused shapes), only after 1-2 land.
4. The DS4-shaped span walk prices the true floor (routed-expert slices walked as the token
   walks them, 6 slices x 43 layers per pass, working set past the cache).
Any door growth mirrors in flatten/form-flatten.fk, form-stdlib/form-flatten.fk,
native-op-manifest.fk, and extends metal-handle-door-band.fk — tag bounds respected (no 256;
probe free tags), predictions before mutations, replay's 25 ids equal the interpreter-driven
25 before any speed number is quoted.

## The second prompt

"The largest planet in our solar system is" — the recorded tokenization (671 9152 13540 295 1132
11250 1487 344, receipts/2026-08-04-ant-colony-comparison.md, round-tripped by the body's own
tokenizer cell). No receipt carries this prompt's fluent id STREAM — the fluent era recorded only
the qualitative "correct fluent Jupiter continuation at ~28 t/s" — so this run is a FIRST RECORD,
not a comparison, and it is said so. Sixteen greedy steps from the Form stack:

    49475 16 983 344 832 3734 396 440 1494 7377 710 270 915 35454 295 436 16

decoded by the body's own tokenizer, verbatim:

    " Jupiter. It is so big that you could fit all the other planets in it."

Correct, fluent, and now on the record at the id level for the next diff.

## State of record

Rungs 10-13 landed in 1886467f (cell + band by explicit path; the sibling's dense-lane files
untouched). Rung 14 (the stack cell + the band's stream bit) lands in the following commit; the
final full-band verdict rides there, re-run at close on a from-source fkwu-metal with every
.fkb/.sym swept first: **Verdict 18014398509481983 = 2^54 - 1, all 54 bits lit, exit 0,
zero dark bits, 1771.5 s wall.**

Handles at the stream run: 1432 (1202 nocopy file views + 230 device buffers) of the 8192
ceiling, 36 pipelines — counted from metal_status, not estimated. 67,014 dispatches over 30
positions; 26 syncs, one per generated token.

The 2^53 verdict bit sits exactly AT the two-power the "verdict fold 2^53 ceiling" memory names;
this band has ONE home arm (metal-linked fkwu, int64-exact) and the ceiling memory's rounding
lives on the TS arm, which never runs this band. Said here so the next reader does not have to
re-derive the safety.

Budget note, honest: the mutation battery's composite runner was killed by the task lifetime
mid-V4 with the V4 mutation live on disk; the cell was restored from the pristine backup by
byte-compare before anything else ran, and V4/W1/W2/N re-ran as single-run tasks. The kill cost
one partial band run and nothing else.
