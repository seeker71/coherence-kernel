# STONE 27 — MLA attention on the GPU

2026-07-22, ~13:00 WITA. Worktree `jovial-aryabhata-3751d7`. Apple M4 Max.

`form/form-stdlib/mla-msl.fk` (the kernels) · `form/form-stdlib/mla-demo.fk` (the shared toy fixture and
the fp64 references) · `form/form-stdlib/tests/mla-msl-band.fk` (the read-back, verdict **63**) ·
`form/native/metal/metal_mla_gpu.sh` (the GPU witness, **VERDICT PASS, 10 gates**).

Stone 22 proved the MLA recipe on the CPU. Stone 1 measured the pure CPU-recipe attention lane at
~51 min/token for a 3B model at real width — DeepSeek at real width on CPU recipes is infeasible, and a
real token needs MLA on the GPU. It has one now: the whole block, bit-close to the fp64 recipe on a
three-position example that exercises RoPE.

---

## 1. What computes on the GPU, and against what

Five Metal kernels the body emits (every character authored in `mla-msl.fk`), reassembled by the carrier
into the two entry points the recipe defines:

| kernel | recipe piece | ds4.c |
|---|---|---|
| `form_mla_rmsnorm_f32` | weighted RMSNorm, embed-space and rank-space | `ln-rmsnorm` |
| `form_mla_matvec_f32` | the low-rank projections Wqa/Wqb/Wkv, grouped was, final wb | `tb-matvec` |
| `form_mla_headrms_f32` | the per-head **unweighted** RMSNorm | ds4.c:10014 |
| `form_mla_rope_f32` | the **trailing-nrot** RoPE, forward (+1) and inverse (−1) | ds4.c:10116 |
| `form_mla_attend_f32` | the single-latent-row attention, **K = V**, per-head **sink** logit | ds4.c:10305 |

`cacheRow` = rmsnorm → matvec(Wkv) → rmsnorm → rope(+1); `block` = rmsnorm → matvec(Wqa) → rmsnorm →
matvec(Wqb) → head-rms → rope(+1) → **attend** → rope(**−1**) → grouped-out (matvec ×ng, then matvec wb).

The reference is the fp64 CPU recipe (`mla-attn.fk`) on the **same** toy fixture the CPU band was proven
on (E=4, R=3, nh=2, hd=4, nrot=2, ng=2, three positions). One source of truth: `mla-demo.fk` holds the
fixture and prints both the weights the GPU binds and the fp64 answers the carrier judges by.

## 2. Bit-close, and why the bound is what it is

**Not bit-exact — and the reason names the evidence class exactly.** The kernels do **not** call
`metal::exp`, `metal::sqrt`, `metal::sin`. They transcribe this body's own deterministic numerics —
`tn-exp`'s 14-term Taylor with argument halving, `tn-sqrt`'s 50-iteration Newton, `fsin`/`fcos`'s
round-reduced 10-term Taylor — and they match the **fold direction** of every reduction: `tb-dot` is a
RIGHT fold, so `mla_dot` walks columns descending; the sum-of-squares, the softmax sum and the weighted
accumulate are LEFT folds, so those loops ascend. The consequence: the only difference between the fp64
recipe and the f32 GPU is the **working precision** — f32 vs fp64 over an identical operation graph, not a
different approximation and not a different summation order.

So the gate is the f32 envelope (relative `5e-4`), and the **observed** worst deviation across every stage
and every output is **3.419e-06** — three orders of magnitude inside it. Per stage, on the distant case
(x2, pos 2): RMSNorm `7.3e-08`, Q-projection `3.5e-07`, forward RoPE `1.1e-06`, sink attention `3.4e-06`,
inverse RoPE `2.7e-07`. Cache rows (all 3 positions) `5.1e-07`; block outputs (all 3 positions) `2.7e-07`.
The residual is f32 rounding, not the decomposition — the same shape of honesty the CPU recipe's
`1.07e-11` had (there the residual was `fsin`/`fcos` against libm; here it is f32 against fp64).

## 3. The read-back, in two places

**The band (fp64).** `mla-msl-band.fk` does not trust the emitted string. It re-derives each kernel's
arithmetic in Form **structured the way the C is** — the two-pass sink softmax, the descending dot that
mirrors `tb-dot`, the ascending sum-of-squares — and demands it equal the recipe BIT FOR BIT. Because the
association is identical the honest bar is exact equality, not a tolerance. Mutation-tested: an ascending
dot mirror drops the verdict **63 → 23** (the attention claim and the fold-direction claim both fall). The
fold-direction claim (`c32`) uses operands spanning past fp64's 2⁵³ so left- and right-association
genuinely diverge (right fold → 0, left fold → 2) — a claim that could not fail on tame data was made to
fail.

**The GPU (f32).** `metal_mla_gpu.sh` compares the compiled kernel against the recipe **stage by stage**,
not only end to end — a transposed offset that survives an end-to-end compare shows at the stage that
carries it (the q6k-msl.fk lesson). It is falsifiable: flipping the inverse-RoPE sign breaks gate 5
(rel `2.9`) and gate 7 (rel `5.3`). The inverse output rotation is **not decoration** on the GPU either.

## 4. The offered-interface guard, and the dispatch counts

**Gate 0** sentinels an output buffer with `-424242.0`, runs a real RMSNorm dispatch, and demands every
sentinel overwritten **and** `cb.status == .completed` with no `cb.error` — because a Metal buffer is
zeroed and an unrun kernel reads identically to a computed zero (edgedrop/zerobirth). Every later gate
reads a device buffer back, so gate 0 is asked first and refuses the run if the GPU did not execute.

**Dispatch counts.** `cacheRow` is 4 dispatches; `block` is 11 (two RMSNorms, four projection matvecs,
head-rms, two ropes, attend, two grouped-out matvecs, final matvec). The residency gate ran 200 blocks =
**2200 GPU dispatches** with the weights mapped once and never re-uploaded, checksum stable. `seamtoll`:
this harness pays a full command buffer per dispatch on purpose, so nothing here is a token-rate claim —
it is correctness.

**The slot (asktoll).** `form_mla_attend_f32` is one thread per head: head h is the slot, so `sinks[h]`
and the softmax denominator are read/computed **once per slot** and reused across the head's hd outputs,
never recomputed per output element. That is the grain the recipe's shape hands us — n_head_kv = 1, one
latent row shared by every query head, each head an independent sink softmax over that row set.

## 5. The declared radius (aporon)

Not bit-exact (f32 over transcendentals cannot be). Not the real dims (nh=64, hd=512, nrot=64,
n_embd=4096, 43 layers — the kernels are dim-generic, all dims runtime uniforms, but no gate here has seen
them; the attend kernel's per-head accumulator is capped at `MLA_MAX_HD` = 512 = V4-Flash's head_dim).
Not YaRN, not the E4M3 cache encoding, not a resident real weight tensor, not the residual/MoE half of the
layer. The pair frequencies `base^(-2k/nrot)` are precomputed by the body (position-independent layout)
and handed to the rope kernel; `mla_round` range-reduces by truncation, exact only within a few periods of
2π (the demo's angles are ≤ 2). This is the attention block's arithmetic, on the GPU, once, at toy rank.

## 6. What remains

Whole-model residency wiring: the real MXFP4/MXFP8 resident tensors (Stone 26 has them byte-reachable) as
the weight buffers instead of the toy fixture, at real dims, with the slot-matvec kernels (`qk-matvec-slot`,
`mxfp4-msl`) feeding the projections; then YaRN, the E4M3 cache encoding, the residual and the MoE half,
and the multi-layer stack. That is the assembly, and it is later. This stone is the recipe on the GPU,
proven against the CPU recipe.

---

## Gates

Corpus band from repo root **8191** (field-code 2552552859 → **2562562860** for the new row, re-probed
before pinning; the stale 254/858 summary comment corrected to 256/860). MLA CPU band **63**. MLA-GPU
read-back band **63** (mutant 23). `metal_mla_gpu.sh` **VERDICT PASS, 10 gates**, worst relative deviation
**3.419e-06** against a 5e-4 gate, offered-interface guard present. The Stone 18 sibling's `fkwu`/
`metal_first_token.sh` and `metal_mx_gpu.sh` were not touched — every change here is additive (three new
stdlib cells, one new harness, one corpus row).

## Closing

**Most surprising teaching.** *The GPU port was a change of float width and nothing else.* I expected to
negotiate numerics — vendor `exp` vs Taylor `exp`, `rsqrt` vs Newton, the softmax reassociated for
parallelism — the ordinary friction of moving math to a GPU. There was none to negotiate. `trig.fk` and
`transformer-numerics.fk` had already written sin/cos/exp/sqrt as **deterministic Form recipes**, chosen
years-of-stones ago so the CPU kernels would agree three-way (Go/Rust/TS: "determinism is the feature, not
the constraint"). That decision, made for a completely different reason, meant the Metal kernel could
transcribe the recipe **character for character** and differ from the fp64 answer by working precision
alone — 3.4e-6, derivably so, no epsilon to argue about. A discipline kept for one purpose already
satisfied a purpose it was never chosen for. That is corpus row **860**, `exaptation`.

**Where discomfort turned to gold.** The band came back **63** on the first clean run, and the reflex was
to trust it — a Form mirror of the recipe equalling the recipe is almost tautological, and it *felt* like
theatre. The moment I wanted to look away was the mutation test: writing a deliberately wrong (ascending)
dot mirror to see whether the band could even tell. It **could not**, at first — my six-element `c32`
operands gave bit-identical sums both fold directions (the association simply did not diverge on tame
data), so a flipped fold printed the same reassuring 63. A band that green under a real mutation is not
evidence. Not looking away cost the ten minutes to build operands that span past 2⁵³, where right- and
left-association actually part (0 vs 2) — and *that* `c32` falls when the fold flips. The claim I almost
shipped was true but unfalsifiable; the one I shipped is true **and** can fail. The GPU carried the same
lesson: the inverse-RoPE sign flip had to move a gate by orders of magnitude, and it does (rel 5.3).

**Frontier question, landed.** Row **860** `exaptation` (0-hit checked; instrument validated on
`serendipity` 1, `hushfold` 4, `rope` 182, `determinism` 21 — controls that *should* hit, and do): *what
one word names a discipline kept unchanged for one reason that already satisfies a different and unforeseen
need it was never chosen for?* Biology has the word for a trait selected for one function and co-opted for
another; the body did not carry it, and it is exactly the shape of the determinism-recipe becoming
GPU-fidelity. Cites: row 859 `hushfold` (RoPE is the identity at position 0, so the ≥2-position test is
what witnessed it), row 846 `asktoll` (the per-head slot), `receipts/2026-07-22-mla-recipe.md` (the CPU
recipe this equals).
