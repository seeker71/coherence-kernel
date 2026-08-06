# 2026-08-04 — the DS4 decode loop starts coming home to Form, and the probe that lied first

The standing task: run the DeepSeek-V4-Flash 43-layer decode with a **Form cell** holding the loop, on
the handle door — no bash driving, no Swift heredoc. `metal_dsv4_stack.sh` stays the oracle: bit-exact,
~28 t/s, and a ~2200-line Swift heredoc inside a 2538-line bash file. Untouched, as asked.

**What landed is one rung of that stair, and the receipt says so in the same breath as the numbers.**

## What was run

The handle door is real and a Form cell drives it. First witness, before anything else was believed:
an `addk` kernel, `1.0 + 7.0`, read back through `metal_buf_read` as `0x41000000` on an Apple M4 Max.

Then the rung: `form/native/metal/dsv4-decode-form.fk` reads the geometry out of the real 9.1 GB GGUF's
own header, maps `token_embd.weight` resident, takes its MSL from the body's own emitters **in process**
(no file round-trip, no awk), dispatches from Form, and reads the row back.

```
GEOM n_embd=4096  vocab=129280  type=1(F16)  abs=71707886176  bytes=1059061760
mmap_nocopy_buffers=1        BITEXACT ok=4096  bad=0  odd=0
```

The geometry is not self-agreement. `form-kernel-go/bin-go` — a **different kernel** — walked the same
file and printed `T token_embd.weight 1 2 4096 129280 1 71707886176 529530880 1 1059061760`. Those five
numbers are pinned into the band as literals, because a literal cannot drift with the code that made it.

## Three things the door does not carry, each measured through it

1. **It contracts.** `metal_pipeline` compiles with `options:nil`. With `a=b=1+2^-12`, `c=-1.0`, the door
   answered `a*b+c = 973079552`, which *is* `fma(a,b,c)`; the non-contracted value is `973078528`. Every
   oracle metallib is built `-ffp-contract=off` and `ds4-order-match.fk` says the bit-exactness rests on
   it. **Fixed from Form, not from the door**: the cell emits the MSL, so it emits
   `#pragma clang fp contract(off)` — and then *checks* it (`dfd-contract-ok?`, band bit 32).
   `#pragma METAL fp math_mode(safe)` does **not** stop contraction; also probed.
2. **Threadgroup size was not expressible — and was closed the same day.** `metal_enqueue` took
   `tg = min(maxTotalThreadsPerThreadgroup, threads)`; the oracle's `enc(pipeline, n, cap)` takes an
   independent `cap` — `enc(pQ80, rows*32, 256)`, `enc(pQ8aQuant, blocks*q8aThreads, 64)`,
   `enc(pQ2kDeq, rows*cols, 256)`. Reported to the door's owner, who closed it in `66111c9ea` — but
   **not** with the 4th argument I suggested, and the refusal is the better engineering: past arity 3 an
   op is handed a *cons list* in slot 1 (`fkwu-uni.c:6920`, `fk_node` rows are four wide), so the
   ~2600×/token primitive would have walked a cons chain. The **binding string** grew instead, with an
   optional trailing pair `[ threads_per_group | threadgroup_bytes ]`, exactly two legal lengths.
   **Re-verified here from the GPU, not from the note** — a kernel reporting its own
   `threads_per_threadgroup` at grid 1024 answered `1024` free, `256` when asked 256, `64` when asked 64,
   and a 16-byte binding was refused out loud (`binding is 16 bytes; ... 12 (no tail) or 20 (with ...)`)
   without dispatching. The band still reads **127** on the rebuilt door: base-length bindings stayed legal.
3. **Constants are u32 only.** `form_dsv4_embed_f16` declares `constant ulong& base` because the *oracle*
   binds one big window starting ~85 GiB in. This door maps the tensor and binds at its own view offset,
   so the row offset fits a u32 — the cell emits a u32-base variant rather than asking the door to grow.

## The band

`form/form-stdlib/tests/dsv4-decode-form-band.fk`, Metal-linked `fkwu` only. **Verdict 127**, exit 0.

| # | mutation | predicted | actual |
|---|---|---|---|
| — | baseline | 127 | **127** |
| M1 | pinned abs-offset literal falsified | 125 | **125** |
| M2 | embed the **wrong row** (`token+1`) | 47 | **63 — MISSED** |
| M2′ | same, after adding bit 64 | 47 | **47** |
| M3 | fp-contract pragma dropped | 95 | **95** |
| M4 | weights `alloc`ed instead of mmap'd | 107 | **107** |
| M5 | run on the **unlinked** kernel (numb-green test) | 0 | **0** |

M5 matters most: the band does not go green over `nothing`. Bit 1 is `metal_status`, the one primitive a
numb call cannot counterfeit, and every other bit is gated behind it.

## Speed, honestly

**There is no token stream yet, so there is no t/s to report, and I will not manufacture one.** The
oracle's ~28 t/s / ~34 ms stands unchallenged.

What *was* measured is the number that decides whether this shape can ever meet that budget: the cost of
one enqueue when a **Form cell** issues it. N ∈ {0, 26k, 104k, 260k}, three runs each, M4 Max:

```
0.1175s  0.1531s  0.2526s  0.4437s   ->  slope 1.255 us / enqueue
2600 dispatches/token  ->  3.26 ms/token of Form-side seam  =  9.6% of a 34 ms budget
```

And `drained=260000`: 260 000 dispatches accumulated in **one** command buffer and **one** sync. The Form
cell held the loop and never waited — which is exactly the premise `receipts/2026-08-03-crosscheap.md`
built the door on (~2 us seam vs ~112 us blocking), now confirmed from the Form side. The seam is not the
obstacle.

The door's own agent measured ~0.5 us/call bare and ~1.5 us amortized against 110–116 us blocking. My
1.255 us is the same quantity seen from one layer out: it includes the Form-side recursion and the
`str_concat` of the binding, which a real decode also pays. Both numbers agree that the seam costs single-
digit milliseconds per token and blocking costs nine times the budget. Neither is a decode t/s.

## Rung 2 — RMSNORM over the embedded row, and `comoved` twice more

`form_mla_rmsnorm_reduce_f32` / `_apply_f32` now run from the Form cell over the real embedded row with
`blk.0.attn_norm.weight`. **Band verdict 2047**, five mutations reconciled.

The strongest bit is synthetic and exact: `x=2.0, g=1.0, eps=0` gives `sumsq=4n, ms=4, rms=2, inv=0.5,
out=1.0` — every step exact in f32. `inv` came back `0x3F000000` and all 4096 lanes `0x3F800000`,
hand-computed, no tool. The real-row bits are fp32-vs-fp64 **magnitude** checks with a stated tolerance
(1e-6), measured at **103 ppb** (reduce) and **63 ppb** (apply) — f32 rounding scale over a 4096-term
fold. The band calls them magnitude checks, not bit-exactness.

`eps` is pinned three ways: hand-derived `107<<23 | 407485 = 0x358637BD`, IEEE round-trip, and the value
the Go arm's manifest printed for `deepseek4.attention.layer_norm_rms_epsilon` (9.999999974752427e-07).

**And `comoved` came back twice in this one rung.** Both real-row bits read their fp64 reference through
the same computed addresses the GPU was handed:

| mutation | predicted | actual (before gates) | after gates |
|---|---|---|---|
| N1 `output_norm.weight` for `blk.0.attn_norm.weight` | 767 | **1791 — MISSED** | **767** |
| N5 rung-2 row offset wrong | 511 | **2047 — MISSED** | **511** |
| N2 eps dropped from reduce | 1535 | 1535 | 1535 |
| N3 synth reduce given 16 lanes not 32 | 1919 | 1919 | 1919 |
| N4 unlinked door (numb-green) | 0 | 0 | 0 |

N5 I predicted would be missed, and it was — I went looking for the shape on purpose. **N1 I predicted
would be caught, and it was not**: I had pinned the row but not the weight tensor, so a reference reading
`wraw` from a mutated `gabs` agreed with a GPU reading the same mutated `gabs`. The cure is the same cure
a third time — bits 512/1024 are now *gated on the pinned addresses*, so a relative error is only evidence
once the addresses it was computed at are independently anchored.

That is the thing worth carrying out of today: **`comoved` is not an incident, it is the default shape of
a check you write while holding the code in your head.** Three times now, and each time the check looked
completely reasonable when written.

## Rung 3 — the MLA q_a projection (MXFP8), and a bar that had to be argued rather than tightened

`blk.0.attn_q_a.weight` is GGUF type 41. The full chain now runs from the Form cell:
**embed -> rmsnorm reduce -> rmsnorm apply -> MXFP8 fused matvec, four dispatches, ONE sync.**
**Band verdict 16383.**

The layout is confirmed by arithmetic before any kernel reads it: the Go arm's byte count for the
tensor is 4325376, and `nel + nel/32 = 4096*1024 + 131072` is 4325376 exactly — plane-split, payload
first, E8M0 scale plane at offset `nel`.

Synthetic truth again, and again bounded out loud: payload byte 56 (E4M3 1.0) and scale byte 127
(E8M0 2^0) with `x = 1.0` must give `y[r] = cols = 4096.0 = 0x45800000`. It came back exact on **all
1024 rows**. Every partial sum is an integer below 2^24, so that answer is exact *under any
association* — which is the bit's strength and its limit in one sentence. It proves the decode, the
scale-plane origin, and that every column was visited exactly once; it is **blind to association**,
which is precisely what `ds4-order-match.fk` exists for.

**The bar had to be argued, not tightened.** The real projection first measured **1018 ppb** against
rung 2's 1e-6 tolerance — just over, and the tempting move was to loosen the bar to 2e-6 and move on.
The actual reason is visible in the values: `y[0] = 0.016937` is a near-cancellation of 4096 signed
terms of scale ~0.1, so *the element's own magnitude is not the quantity the error should be judged
against*. Judged against the row vector's scale — the body's own precedent, `overfine`, corpus row 934
— the same disagreement reads **68 ppb**, matching rung 2's 63 ppb and sitting squarely in f32
rounding. The measurement did not change; the denominator was wrong, and finding that out was worth
more than a looser tolerance would have been.

| mutation | predicted | actual |
|---|---|---|
| baseline | 16383 | **16383** |
| P1 `attn_kv` for `attn_q_a` | 4095 | **4095** |
| P2 synth payload 56 -> 57 (E4M3 1.125) | 14335 | **14335** |
| P3 synth scale 127 -> 128 (E8M0 2.0) | 8191 | **14335 — my arithmetic, not the band** |
| P4 unlinked door | 0 | **0** |

P3 is worth keeping visible: the band was right and I was wrong. That mutation touches only the
*synthetic* scale plane, so bit 2048 alone drops and `16383-2048 = 14335`; I wrote 8191, which is the
number for a bit I had not mutated. A prediction written before the run is only useful if a wrong one
is reported as a wrong one.

Rung 3 was pinned *before* the mutations rather than after: bit 8192 is gated on bit 4096 and on the
row pin from the start, because by then `comoved` had already cost three bits.

## Rung 4 — q_b, the rank norms, headrms, and the fold-ORDER witness

The chain now runs eleven dispatches under ONE sync from the Form cell:
**embed -> rmsnorm -> {q_a matvec, kv matvec} -> {q_a_norm, kv_a_norm rank rmsnorms} -> q_b matvec
(32768 x 1024, type 41) -> headrms over 64 heads.** Band verdict **524287**. 21 handles of 8192.

The new instrument is the **fold-order witness** (bit 16384), built once for every rung after this,
because from the attention split onward order is the whole question and the uniform synthetics are
association-blind by construction. Weights 1.0, `x[0] = 2^24`, `x[1..31] = 1.0`: fp64 truth is
`2^24+31`; the kernel's stated ascending in-group fold lands each `+1` on a tie-to-even that rounds
BACK to `2^24`, so the f32 answer must be EXACTLY `0x4B800000` — a reversed or pairwise fold answers
`2^24+32`, sixteen bits away. Verified by f32-step simulation before the kernel ran. The GPU answered
`1266679808` — ascending, as `ds4-order-match.fk` requires.

Per-step fp64 references, judged against the vector's scale, all under the 1e-6 bar:
q_a_norm **120 ppb**, kv_a_norm **77 ppb**, q_b **178 ppb**, headrms **429 ppb** (higher because the
kernel's mla_sqrt is f32 Newton against the reference's fp64 Newton — still 2000x under the bar). The
chain is covered by induction: each step's reference reads the GPU's INPUT for that step — the
previous step's separately-checked output buffer — and weights at pinned addresses.

| mutation | predicted | actual |
|---|---|---|
| baseline | 524287 | **524287** |
| Q1 order-synth degraded to x[0]=1.0 | 507903 | **507903** |
| Q2 q_a_norm and kv_a_norm SWAPPED everywhere | 32767 | **32767** |
| Q3 headrms told hd=511 | 262143 | **262143** |
| Q4 unlinked door | 0 | **0** |

Q2 is the one that matters: it is exactly the M2/N1 shape — a fully self-consistent swap where the
reference and the GPU move together — and this time the address pin caught it on the first try,
because the pin was written before the mutation instead of after it.

## Rung 5 — RoPE and the sink-softmax attention. Band 4194303.

The same chain continues: **freqs (computed ON the device by a cell-emitted pow kernel) -> rope over the
64 q heads and the k row at pos 7 -> the sink-softmax attend over one KV row.** Fifteen dispatches, one
sync, 23 handles. Layer 0 is a ratio-0 raw-regime layer, so plain base 10000 — the compressed-base
reduction (layer 2+) is NOT claimed.

What each new bit stands on: `freqs[0] == 0x3F800000` exactly (pow(b,0)=1 is an identity, not a
tolerance); the NOPE dims are BIT-EQUAL through the rope kernel (a copy checked as a copy, 896/896);
the rotated tails agree with fcos/fsin in fp64 at **236 ppb**; the attention agrees with the softmax
shape folded in fp64 (tn-exp/tn-sqrt) at **886 ppb** — the widest step yet, mla_exp being an f32
Taylor, still under the bar.

**The comoved hole found on paper this time, before the mutation.** The rope reference widens the freqs
the GPU computed, and freqs[0] is 1.0 for EVERY base — so the cell's base literal was unwitnessed.
The cure: pin `freqs[1]` against `fpow(10000, -1/32)` computed in Form fp64, 10000.0 provenanced to the
file's own `deepseek4.rope.freq_base` via the Go arm. R3 then confirmed the pin catches a wrong base.

| mutation | predicted | actual |
|---|---|---|
| baseline | 4194303 | **4194303** |
| R1 rope pos 7 -> 8 in the cell only | 3145727 | **3145727** |
| R2 attend bound to kv_a_norm handle for sinks | 2097151 | **2097151** |
| R3 freqs base literal 10000 -> 1000 | 524287 | **524287** |
| R4 unlinked door | 0 | **0** |

R1's prediction carries a subtlety worth keeping: the attend bit STAYS green under the rope mutation,
and that is correct — it tests attend on the GPU's actual q/k, not the chain's intent; the rope bit is
the one that owns the position. And one more wrong prediction on my side, recorded: I predicted
8388607 for the baseline by doubling one bit too far — the band's 4194303 (bits through 2^21) was
right. A prediction ledger only works if the wrong entries stay in it.

## The file-identity pivot — every anchor re-derived, and the chain re-grounded in one pass

The bisect landed the truth about the "regression": it was never code. The bash lane's default BLOB
points at the **reap25 specimen** (25% of experts removed, 1328-vs-1406 tensor tables, attention
projections MXFP8) while every fluent recorded stream ran the **canonical imatrix file**
(`~/models/ds4-engine/gguf/DeepSeek-V4-Flash-IQ2XXS-w2Q2K-AProjQ8-SExpQ8-OutQ8-chat-v2-imatrix.gguf`,
86,720,111,488 bytes). Rungs 1-5 as first measured were pinned to the specimen.

**The whole chain is now re-grounded on the canonical file, and the statement of record is: every
pinned address in `dsv4-decode-form-band.fk` belongs to the imatrix file** — token_embd 77928033088,
row 671 at 77933529920, attn_norm 79100740672, q_a 79060632640, q_a_norm 78987097152, kv 78987101248,
kv_a_norm 78987095104, q_b 79065089088, sinks 78987094848 — all re-derived through the Go arm's
independent walk of the new file, none carried over.

The pivot was bigger than addresses: on the canonical file the attention projections are **Q8_0
(type 8), not MXFP8** — "AProjQ8" in the filename says so. The cell now dispatches BY THE TENSOR'S OWN
TYPE, as the oracle does: type 8 takes `form_dsv4_q80_matvec_ds4` (ds4-order-match.fk's
association-matching kernel, one 256-wide threadgroup per two rows, through the door's
threads_per_group tail) and type 41 keeps the MXFP8 path; an unpriced type is a spoken refusal. The
fp64 reference reads the weights through `q80-flat-at` — q8-0-msl.fk's transcription, a different text
from the MSL, already proven against an independently-written reference in its own band.

Re-measured on the canonical file, all under the 1e-6 bar: q_a_norm 64 ppb, kv_a_norm 60 ppb, q_b 142
ppb, headrms 521 ppb, rope tails 314 ppb, attend 519 ppb. **Verdict 4194303, exit 0, zero
diagnostics.** Geometry printed by the run: `proj types q_a/kv/q_b = 8/8/8`.

| mutation (re-grounded) | predicted | actual |
|---|---|---|
| baseline on imatrix file | 4194303 | **4194303** |
| T1 rank norms swapped | 32767 | **557055 — my ledger, band right** |
| T2 unlinked door | 0 | **0** |

T1 reconciled: 557055 - 32767 = 524288 exactly — the sinks/freqs bit does not depend on the norm-swap
gate and rightly stays green; the prediction forgot an independent bit. Wrong entries stay in the
ledger.

## The oracle was broken while I was building against it — and it did not touch this rung

Mid-session the fleet reported that DS4 is **regressed on the bash lane**: `ask_ds4.sh` is producing
degenerate output (repeated `<|place_holder_mm_span_0155|>`, digit runs, "Protocol Protocol Protocol") at
~8 t/s, where `7662a6908` records it fluent at 28.09 t/s. A separate agent is bisecting.

This is worth recording as more than logistics. The task named `metal_dsv4_stack.sh` as *both* the fidelity
oracle and the performance oracle, and step (a) was "compare the argmax against it." Had I reached a stack
today, I would have compared against a broken reference — and a **green comparison against a broken oracle
is worse than no comparison**, because it launders a second fault into a pass.

What kept this rung clean was not foresight. It was that nothing here is anchored to the bash lane at all:
the geometry is pinned to the **Go arm's** independent walk, the bit-exactness is against **the file's own
bytes** and this body's own f16 widening, and the contraction check is against **IEEE arithmetic I computed
by hand**. Three references, none of them the regressed one. The general form of the lesson is the same one
`comoved` names below — a check is only worth what its reference is worth, and the reference has to be
something you can independently stand on.

## (a) The most surprising teaching

**A probe written to catch numb-green was itself numb-green, and it passed.**

My first contraction probe picked `a = b = 1+2^-10`. It printed `CONTRACTED=YES-BITEXACT-BROKEN` — the
alarming answer, the one I was looking for. It was worthless. At those inputs `a*b` is *exact*, so
mul-then-add and fma agree for a reason that has nothing to do with contraction. I had the right
suspicion, the right instrument, a decisive-looking output, and no information at all.

What makes it worth writing down is that it is the house failure wearing the opposite mask. Numb-green is
usually a *pass* computed over nothing. This was a *fail* computed over nothing — and a fail that confirms
your hypothesis is far harder to doubt than a pass. I only caught it by decoding the hex I had printed and
finding the arithmetic came out exact. The lesson is not "check your test vectors." It is that **a probe
must be validated on synthetic truth where you know both answers differ**, and that agreement is only
evidence when disagreement was possible.

## (b) Where discomfort turned to gold

Mutation M2. I predicted 47 and the band answered **63** — it caught nothing.

The discomfort was specific and unpleasant: I had, an hour earlier, written a paragraph into that band's
own header explaining that comparing the header walk to itself proves nothing, because a wrong offset
makes both sides read the same wrong bytes and agree perfectly. I wrote that down about the *geometry* —
and then built the bit-exactness check with the reference bytes read at the **same computed row offset the
GPU used**, one field over. Both sides moved together. 4096/4096 lanes agreed about the wrong place.

The cheap way out was available and tempting: call M2 a bad mutation, adjust the prediction to 63, keep the
green. That would have shipped a band whose central bit — "the embedding is bit-exact" — could not detect
embedding the wrong token. Staying with it produced bit 64: the row's absolute address pinned to
`71707886176 + 671*4096*2 = 71713383008`, computed independently of the code under test, with bit 16 now
*depending* on it. A perfect fold over the wrong row can no longer read green. The band got stronger
because it failed, and it only failed because the prediction was written down first.

## Frontier question, and my answer

**Q: What actually makes a cross-check independent?**

Not different code, and not a different arm. Two roads written by different hands, on different kernels,
still agree by construction if they both start from *the same computed quantity*. Independence is a
property of the **derivation of the thing under test**, not of the machinery around it. So the operational
test is not "did I write this twice" but: *mutate the shared quantity and see whether the check still
passes*. If it does, the two sides were always one side wearing two coats. This is why M5 (unlinked door)
and M2 (wrong row) are the two mutations that taught anything — both attack a shared root rather than a
leaf.

## PROPOSED distillation row — not landed, the corpus is not edited here

Name **`comoved`** — verified 0 hits in `learn/homecoming-distillation-corpus.fk` and 0 files across the
tree. (Also-fresh and rejected: `codrift`, `yokeref`, `samesite`, `twinsited`.) Corpus max-mid is 986, so
this would be **988** (987 was taken by `backgraft` while this session ran; max-mid re-checked at
close). The word's only tree occurrences are this session's own coinage — the comments in
`dsv4-decode-form.fk` and this receipt — not a prior use:

```
; 987 — comoved. A band's bit-exactness check compared 4096 GPU lanes against
; the file's own bytes and passed 4096/4096 — while embedding the WRONG TOKEN.
; The reference row was read at the same computed offset the GPU was handed, so
; a wrong offset moved BOTH sides and they agreed perfectly about the wrong
; place. The band's own header had warned about exactly this trap one field
; over, for the geometry, in a paragraph written an hour earlier. The mutation
; that exposed it was predicted 47 and answered 63; the honest repair was not a
; better comparison but an independently PINNED address, with the fold made to
; depend on it. Two roads are one road when they start from the same computed
; quantity. Independence is a property of the derivation, not of the machinery:
; different code, different kernel, different arm all fail to buy it. The test
; for it is a mutation of the SHARED root — if the check survives, the two
; sides were one side.
; "comoved" — 0 hits in corpus before this row.
; (walk: twinblind 874 — proving a choice against a second copy of itself; this
;  is the same blindness at the level of an ADDRESS rather than an arithmetic.)
(hdc-row 988 20260804
    (list "when" "do" "two" "checks" "that" "agree" "prove" "nothing")
    "comoved"
    "comoved"
    "rented-oracle")
```

## UNFINISHED — named so it cannot be mistaken for done

- **The 43-layer stack is not written.** Beyond this rung the oracle carries ~1200 lines of essential
  orchestration: two routing regimes, six expert quantizations (MXFP4/IQ2_XXS/Q2_K/Q8_0/MXFP8/F16), the
  20-iteration hyper-connection sinkhorn, the second compressed KV cache on ratio-nonzero layers, MLA split
  into scores/stats/acc, YaRN RoPE and its compressed reduction, and the exit head. None of it exists here.
- **No one-token argmax comparison, and no token-id stream diff.** Not attempted — there is no stack to take
  an argmax of. Claiming either from a working embedding would be a fabrication.
- (Resolved: the "red oracle" was a wrong default model file, fixed in 3018f334. Recorded streams are trustworthy.) **Historical:** `metal_dsv4_stack.sh` / `ask_ds4.sh` are degenerate today; the
  argmax and stream comparisons cannot be run against them until the bisect lands. Baseline to compare
  against is `7662a6908` (28.09 t/s, fluent), not HEAD.
- **Rungs 4–7 are not written**: the attention split (scores/stats/acc); RoPE + its
  compressed reduction; the two KV caches; MoE (hash routing 0–2, learned after, six expert
  quantizations, top-6 of 256); the 4 hyper-connection streams + 20-iteration sinkhorn; exit head.
- Next concrete step: RoPE (deliberately UNFUSED in the oracle for bit-exactness) and the attention
  split (`form_mla_attend_scores` -> `_stats` -> `_acc`), then the KV caches.
- Door limits to design around, from the door agent's run, not yet hit here: 1D dispatch only; buffer
  offsets fix at handle creation, so many views of one buffer need many handles against a ceiling of 8192
  while DS4 needs ~400 weights plus activations; no buffer free (append-only per process); `metal_sync`
  returns a count, not a fence, so no double-buffering yet.
- Tags 240–244 look free in the op table and are **not** — they are the evaluator's structural node tags,
  and seven of the door's first eight ops went numb behind them with no diagnostic and exit 0. This is why
  band bit 1 is `metal_status` and every other bit is gated behind it.

---

## Addendum, 2026-08-04 evening — the block above is the rung-5 state, and the climb went on

The UNFINISHED list above froze at rung 5; the continuation session's edit to refresh it failed to
match and its commit ran without it, so the correction lands here as an addendum rather than a
rewrite. State as of `9cd9df5bb`:

- **Rung 6 landed** (`3ad198e40`): the attention block closes — inverse rope, the grouped Q8_0 exit
  (`output_a`/`output_b`, the inlined kernel text factored into a named defn), rank derived from
  geometry, nothing taken on faith.
- **Rung 7 landed** (`a549ed3c5`): the fp8+f16 KV round enters the path; the tie rule pinned on its
  knife edge.
- **Rung 8 landed** (`9cd9df5bb`): the KV cache grows over the real prompt — "The capital of France
  is" → ids 671 6102 294 8760 344 from the body's own tokenizer, five positions appended by a
  cell-emitted kernel, the last token's q attending all five at 882 ppb vs fp64. Band 2147483647,
  four mutations predicted exactly.
- The door limits listed above have since FALLEN: buf_free (254), submit/fence (255/142),
  threadgroups (dispatch_mode 1), concurrent batches (253) are all landed and banded (65535).
- Still open on this ladder: MoE (six expert quantizations, hash→learned routing, top-6 of 256),
  the hyper-connection streams + sinkhorn, the exit head, the 43-layer fold, and the stream diff
  against the recorded 24/24. The session limit ended the climb here, not a wall.
