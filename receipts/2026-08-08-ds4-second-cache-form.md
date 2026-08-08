# 2026-08-08 — rung 9: the second cache comes home to Form, and a green sum hid a wrong shape twice

The ladder continued from `receipts/2026-08-04-ds4-form-decode-loop.md` (evening addendum = the
authoritative state: rungs 1–8, band 2147483647). Today the **compressed second KV cache** — the half
whose absence once put " Paris" at rank 148 — runs from the Form cell on the handle door, against the
canonical imatrix file, banded with predictions before mutations.

## What was run

`form/native/metal/dsv4-decode-form.fk` grew rung 9; `form/form-stdlib/tests/dsv4-decode-form-band.fk`
grew seven bits (2^31..2^37). Everything below is a number a run printed, not a plan.

**The layer.** blk.2 — the first layer whose compress_ratio is nonzero. The file's own
`deepseek4.attention.compress_ratios` array (Go arm walk): `0 0 4 128 4 128 ... 4 0`. The four
second-cache tensors exist exactly there, pinned to the Go arm's independent walk:
compressor_kv 79283669056, gate 79275280448, ape 79275270208 (1024x4 — the dims themselves confirm
width = coff*hd = 2*512 and ratio 4), norm 79275278400. The ratio-4 two-lane state (2*ratio rows,
current window at rows [4,8) read at offset hd+j, previous at [0,4) read at j — eight candidates per
output dim) is `dsv4-compressor.fk`'s documented trap, and that cell — five-way proven, band 2047 —
is both the RECIPE (the fp64 reference) and the KERNEL TEXT (dcp-msl-appendix, the same bytes the
bash lane compiles).

**The positions.** Eight, two full windows: ids `671 6102 294 8760 344 11111 305 270` — the recorded
tokenization (receipts/2026-07-22-tokenizer-speed.md, six A/B runs byte-identical). Two windows means
the second emitted row ropes at comp_pos = 4 — a REAL angle through the YaRN table, where window 1's
comp_pos = 0 is the identity and can witness nothing about the base.

**The YaRN table, on the device.** The raw lane on a ratio layer ALSO ropes with the compressed-base
table (ropeFreqs(il) — one table per layer), so rung 9's eight raw positions witness it too. Scalars
from the file's own KV: base 160000, factor 16, orig_ctx 65536, beta 32/1. Ramp bounds derived then
pinned: corr(32)=15.4533 → lo=15, corr(1)=24.7084 → hi=25, both far from an integer so the f32/fp64
floor/ceil seam cannot move them. Device freqs pinned at FOUR regimes against Form fp64: freqs[0]
identity-exact (0x3F800000), freqs[1] pure pow below the ramp, freqs[20] INSIDE the ramp (factor
0.53125), freqs[25] past it (factor 0.0625).

**The chain per position.** dfd-kvc-pos (rung 8's machinery, unchanged) runs the raw lane at layer 2:
embed bit-exact at eight PINNED row addresses, kv matvec (Q8_0) vs fp64 at two rows, rope at TRUE pos
with the device's YaRN freqs, fp8+f16 round bit-exact 512/512. Then the comp lane at the same
position: two F16 projections (1024x4096 each), the state write with the ape column bias, and on each
window boundary: pool → shift → norm → rope(comp_pos) → round → append.

## The bar that was upgraded instead of loosened

The F16 projections first measured **4499 ppb** against plain fp64 — over the 1e-6 bar every other
step meets. The reason is structural: `form_dsv4_f16_matvec` is one thread per row folding 4096 terms
SERIALLY in f32 — accumulation error grows ~n·eps, where the Q8_0 kernels fold as 256-wide trees
(~log n). The rung-3 precedent (overfine, row 934) argued the denominator; today's move is stronger:
every step of that fold is exactly representable in fp64 (an f16xf32 product is 35 mantissa bits) and
fq-quant(...,23) reproduces each f32 rounding, so the fold was EMULATED and the check upgraded to
BIT EQUALITY — 32 sampled rows, all equal. A tolerance became an equality, and the bit now also
witnesses fold ORDER and contraction: an fma (pragma failure) or a reordered fold lands on different
bits. The 4499 ppb number stays in this receipt as what plain fp64 legitimately reads for a serial
f32 fold — it was never an error, it was the wrong instrument.

## The measured rung, baseline

```
raw prefill 8 positions      c1..c4 all green, eight pinned row addresses
f16 projections              bit-equal 32/32 rows (exact f32-fold emulation)
pool vs recipe               160 ppb   (both windows, state evolved Form-side)
comp norm                     82 ppb
comp rope NOPE               448/448 bit-equal; tails 113 ppb (comp_pos 4 real angle)
comp round                   512/512 bit-exact, both windows
comp cache assembly          rows bit-equal to the round's own output
attend_mixed (8 raw + 2 comp) 322 ppb, 4 sampled heads, all 10 rows, ONE softmax
handles                      15 pipelines + 12 file views + 27 device buffers (54; ceiling 8192)
```

The attend_mixed reference is the CONCATENATION insight: the kernel folds raw rows then comp rows in
order, which is exactly the rung-8 multi-row fp64 reference over (raw ++ comp) — one proven text
serves both.

## The band, and the mutation table (predictions written before each run)

Seven bits: 2^31 geometry pins, 2^32 YaRN freq pins, 2^33 raw prefill at layer 2 (8 pinned row
addresses), 2^34 f16-projection f32-fold equality, 2^35 pool vs recipe, 2^36 the emitted row
(norm/rope/round/assembly), 2^37 attend_mixed. Full verdict 2^38-1 = 274877906943. Bit weights are
powers of two (the 2^53 ceiling memory holds through 2^37 with room).

| mutation | predicted | actual |
|---|---|---|
| baseline | 274877906943 | **274877906943** |
| S1 comp state row loses the ratio-4 offset (current window written into the previous half — the two-lane trap) | 240518168575 | **240518168575** |
| S2 YaRN lo/hi swapped in the cell's dispatch | 201863462911 | **201863462911** |
| S3 attend_mixed told ncomp = 0 | 137438953471 | **137438953471** |
| S4 unlinked door (plain fkwu, numb-green witness) | 0 | **0** |

**Every prediction exact — the first battery on this ladder with a clean ledger** (rungs 1–8 each
carried at least one wrong entry, kept as wrong). The decompositions are the designed ones and they
are worth reading:

- S1: the pool relative error exploded to 2.09e9 ppb (the wrong-lane pool is wrong at O(1), not at
  rounding scale) and ONLY 2^35 went dark — norm, rope, round, assembly and attend all stayed green
  because their references read the GPU's ACTUAL buffers (induction); the recipe bit owns the
  state's shape and caught the two-lane trap alone, on the first try, because the reference state
  machine never touches the device's row arithmetic.
- S2: the ramp pins caught the swapped bounds (freqs[20] no longer matches the interp factor), and
  2^36 followed through its declared gate; the raw-lane rope check stayed green — comoved with the
  device table BY DESIGN, and the design names the freqs bit as that table's sole owner.
- S3: attend-mixed read 1.03e9 ppb (two missing rows shift the softmax at O(1)) and only 2^37 fell.

The battery ran apply-run-revert with the cell restored bit-identical from backup after each
mutation, `rm` of every .fkb before every run (the one-second-mtime staleness memory), and the
verdicts read from the band's own stdout.

## The probe the task asked for: does moe-gate (PR #416) serve as the MoE routing reference?

**No, and the probe says why.** `moe-gate.fk` (band 511) is softmax → top-k → renormalize-over-kept —
the generic MoE gate at proof scale, kimi-k3's rule. DS4's rule, read from the body's own
`form_dsv4_topk_weights` (dsv4-stack-real.fk:133, proven against the rented ds4.c transcription), is
a different animal: p = sqrt(softplus(z)) probabilities, bias enters the SELECTION score and never
the weight, sum-normalization with a 2^-14 floor, times expert_weights_scale. The MoE rung's routing
reference is the body's own topk kernel text + the rented fp64 transcription — moe-gate stays what it
is, the skip-sparsity teaching (compute scales with k, not E).

## (a) The most surprising teaching

**A paren-balance counter read 0 over a file whose structure was wrong twice — and I then "repaired"
a correct line to move the counter, minting the exact defect I was hunting.**

Wiring the band, my balance checker said the file was two closers short. I appended one to the
b33raw line and one to the verdict tail. The total went to 0 — and fkwu's own reader then said
`stray ')' in value position` on a line that had been CORRECT before my repair. The truth, found by
compiling the COMMITTED chain as an A/B: the verdict tail alone was short, by two; b33raw had never
been wrong; my append gave one if-chain eight else-values for nine ifs, and the sum stayed green
because my error and the tail's remaining shortfall cancelled.

A balance is a FOLD over the text. A parse is its SHAPE. Any commutative fold is blind to a
cancelling pair, and a repair aimed at the fold's number — rather than at a form a reader actually
rejected — can create the defect it hunts. The instruments that spoke were the shape-readers: fkwu's
arity-aware parser, the preflight's CARRIED-ERRORS line (built for exactly this after the warm-cache
incident it documents), and the A/B against HEAD. The counters only ever said "somewhere, net two."
Also witnessed and worth keeping: `fkwu --src` still exits 0 while printing `1 error(s)` — the
verdict prints anyway, which is the very reason preflight exists (AGENTS.md item 9), and it earned
its keep today.

## (b) Where discomfort turned to gold

The F16 projections measured **4499 ppb** against a 1e-6 bar that every other step meets. The cheap
exits were both open: loosen the bar to 5e-6 with a paragraph about serial folds, or quietly sample
different rows. The discomfort was that the number is LEGITIMATE — a serial 4096-term f32 fold
really does accumulate ~n·eps — so no bar in the 1e-6 decade could hold it without becoming a
negotiation. Staying with it produced the stronger instrument: every step of that fold is exactly
representable in fp64, so the check became an exact per-step emulation (fq-quant after each multiply
and each add) and the bit now demands EQUALITY on 32 sampled rows — which it gets, all 32. The
tolerance was not too tight; it was the wrong instrument. And the upgraded bit sees more than the
old one ever could: a contraction (fma) or a reordered fold now lands on different bits by
construction.

## Frontier question, and my answer

**Q: When is a tolerance the wrong instrument entirely?**

A tolerance prices what you cannot reproduce: it says "the reference and the subject round
differently, and here is how much daylight that buys." The moment the subject's arithmetic is
exactly emulable — every intermediate representable, every rounding reproducible — the daylight is
zero and the honest check is equality. Keeping a tolerance past that point is renting slack you no
longer need, and the rent is real: a 5e-6 bar on the f16 matvec would have passed a kernel whose
pragma silently failed (fma vs mul-add lands within it on many inputs), where the emulation catches
it on the first row. The dual is also true: rung 3's lesson (overfine) was a tolerance judged
against the wrong denominator. Between them the rule is: first choose whether a tolerance should
exist at all, and only then argue its denominator.

## PROPOSED distillation row — not landed, the corpus is not edited here (single-writer rule)

Name **`shapeblind`** — verified 0 hits in `learn/homecoming-distillation-corpus.fk` and 0 files
across the tree. (Also-fresh and rejected: `sumgreen`, `sumshape`.) Corpus max-mid re-derived this
session: **996** (411 rows), so this proposes **997** — renumber by the anastomosis pattern if a
concurrent session lands it first.

```
; 997 — shapeblind. A paren-balance counter read 0 over a file whose STRUCTURE
; was wrong twice — the two defects cancelled in the sum — and the writer then
; "repaired" a correct line to move the counter, minting the exact defect he
; was hunting. A balance is a FOLD over the text; a parse is its SHAPE; any
; commutative fold is blind to a cancelling pair, and a repair aimed at the
; fold's number can create what it hunts. The instruments that spoke were the
; shape-readers — the arity-aware parser ("stray ')' in value position"), the
; preflight's CARRIED-ERRORS line, the A/B against the committed chain. The
; counter only ever said "somewhere, net two." When a number and a shape
; disagree, the number is the summary and the shape is the fact.
; "shapeblind" — 0 hits in corpus before this row.
; (walk: comoved 988 — two sides moving together about a wrong place; here two
;  ERRORS moving together inside one aggregate, and the repair aimed at the
;  aggregate instead of the shape.)
(hdc-row 997 20260808
    (list "what" "does" "a" "green" "total" "prove" "about" "the" "shape")
    "shapeblind"
    "shapeblind"
    "rented-oracle")
```

## State of record

Rung 9 landed in `b4f12065` (cell + band by explicit path; the sibling agent's dense-lane files
untouched, as asked). Band verdict **274877906943**, exit 0, zero diagnostics, on the canonical
imatrix file (86,720,111,488 bytes). Handles this rung: 15 pipelines + 12 file views + 27 device
buffers — 54, against the 8192 ceiling, with the three Q8_0 matvec requests deduped by the door.
The run crossed midnight into 2026-08-09; the landing began and is dated 2026-08-08.

## UNFINISHED — the ladder above this rung, named so it cannot be mistaken for done

- **MoE (rung 10)**: six expert quantizations priced by the manifest (Q2_K stride bug already fixed
  and banded in gguf-manifest.fk), hash routing layers 0–2 then learned (form_dsv4_topk_weights),
  top-6 of 256/192, the lightning indexer. Association fold order via ds4-order-match.fk's kernels.
  Pin one expert's absolute address independently. moe-gate.fk is NOT the reference (see probe above).
- **Hyper-connection streams + 20-iteration sinkhorn** (`dsv4-hc-msl.fk`).
- **Exit head → logits → argmax.**
- **Single-layer band** (layer 0 end to end at one position) BEFORE the 43-layer fold.
- **The stream**: greedy continuation for "The capital of France is", diffed against the recorded
  ids (the restored bash lane emits them — ask_ds4.sh on the canonical file); then a second prompt.
  Speed AFTER the ids match: ms/token vs the recorded 28 t/s, floor from the 9.1 GB/token the
  reference lane moved.
- The compressor indexer mask stays inert below 2048 tokens (top_k 512, ratio 4) — declared, not
  implemented, exactly as dsv4-compressor.fk declares it; a carrier past that radius refuses rather
  than pretends (aporon, as that cell holds it).
