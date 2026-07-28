# Stone 7 — batched prefill: the prompt stops being P forward passes

**2026-07-21, ~18:50 WITA. Apple M4 Max. Real llama3.2:3b, one resident quantized `MTLBuffer`, no f32
copy of any tensor anywhere.**

Ground for every number below: `form/native/metal/metal_batched_prefill.sh` (VERDICT PASS, 5 gates),
run by me, on this machine, against the same 2 019 377 376-byte blob every stone in this program has
used.

---

## The headline, with both denominators (row 819, `selfgauge`)

| | before (Stone 5 lane path) | after (Stone 7) | x vs before | absolute, vs ollama 640.94 / 157.83 |
|---|---|---|---|---|
| **prefill**, P=6 | 12.94 tok/s | **52.29 tok/s** | **4.04×** | 12.3× behind (was **49.3×**) |
| **end-to-end @12 tok, prefill included** | 8.295 tok/s | **10.930 tok/s** | **1.32×** | — |
| decode-only | 12.21 tok/s | 12.21 tok/s | 1.00× | 12.9× behind (untouched) |

The external denominator is a **measurement made elsewhere and quoted**: ollama/llama.cpp on this
machine, this model, this blob, 150-token sample — decode 157.83 tok/s, prefill 640.94 tok/s. It is
never mixed into a gate.

Prefill was the largest single gap in the program at 49.3×. It is now 12.3× — the same order as decode.

---

## The profile that chose the path

Prefill was **76.8 ms per prompt token** against decode's **81.9 ms per token**: the same cost per
token, which is exactly what token-at-a-time looks like from outside. The carrier's prefill loop was
literally `for id in ids { cur = forward(id, pos); pos += 1 }`, so a P-token prompt loaded all 2.0 GB
of weights P times and used each weight **once**.

Stone 5's refutation named the lever. It measured that this body's inner loop is bound by **per-weight
ALU** — the f16 super-scale decode and the index decomposition — and **not** by loading the
activation. That is precisely why llama.cpp's `nr0` register blocking made it *monotonically slower*
(row 820, `boundborrow`): `nr0` amortizes the **activation** load, which was never the cost. Batching
is the mirror image — it amortizes the **weight decode**, which is measured to be the cost. Same
machine, same kernel spine, opposite lever, opposite result.

Measured on real tensors, best of 3, batched against P sequential lane matvecs in one command buffer:

| shape | P=1 | P=6 | P=32 | P=128 | P=512 | best TB |
|---|---|---|---|---|---|---|
| Q4_K 3072×3072 | 0.92× | 2.05× | 3.34× | 4.12× | 3.91× | 8 |
| Q4_K 8192×3072 | 0.92× | 2.15× | 3.53× | 3.88× | 3.99× | 8 |
| Q6_K 3072×8192 | 0.98× | 2.48× | 4.26× | 4.77× | 4.62× | 8 |
| Q6_K 128256×3072 | 0.97× | 3.61× | 4.96× | 5.19× | 4.97× | 16 |

**Our crossover, measured rather than borrowed:** batching is a *loss* at P=1 (0.92–0.98×) and a win
by P=6. The carrier switches on exactly that — decode uses the lane matvec, prefill uses the batch
kernel.

---

## Two hypotheses of mine that the body refuted, with their numbers

### 1. "Batching scales with P." It does not — and the plateau is a measurement

It plateaus at **4–5× and goes no further**. At TB=8 the weight decode is amortized 8× and only 4× is
realized. That is not a mystery; it *prices the inner loop*. Write **D** for the amortizable
per-weight work (decode) and **L** for the per-token work batching cannot remove (the activation load
and the multiply-add). Per token, before: `D + L`. After, at tile TB: `D/TB + L`. The observed 4× at
TB=8 gives

```
D + L = 4·(D/8 + L)   →   D = 6L
```

So the **weight decode is 6/7 of the inner loop**, and the ceiling of this lever, at TB→∞, is
`(D+L)/L = 7×` — not P, and never was. That ratio was not available before this stone; it now prices
every future stone that attacks the decode arithmetic itself.

### 2. "The plateau is the strided activation load." Refuted, and worst where predicted best

At TB=8 the tile's loads `x[t*cols + j]` are `cols` floats apart — 12 KB of stride per tile. A
**column-major twin** (`xc[j*ntok + t]`, the same tile's loads contiguous, identical arithmetic) was
built and measured:

| shape | P=6 | P=32 | P=128 | P=512 |
|---|---|---|---|---|
| Q4_K 3072×3072 | 2.01× | 0.90× | 0.88× | **0.53×** |
| Q4_K 8192×3072 | 1.20× | 0.94× | 0.93× | 0.69× |
| Q6_K 3072×8192 | 1.12× | 0.95× | 0.94× | 0.79× |
| Q6_K 128256×3072 | 1.06× | 0.94× | 0.93× | 0.69× |

(>1 means column-major wins.) It is **slower everywhere the batch is big enough to matter, and worst
exactly where the stride argument predicted it would be best.** Row-major stays — which is also the
layout the pipeline already has, so the win costs no transpose.

---

## The rented oracle — read for shape, then measured against

The coordinator mid-task pointed me at `ollama-strings.txt`: llama.cpp's ggml-metal MSL, MIT, 771
kernels, embedded in the ollama binary. **Read for shape only. No MSL pasted.**

`kernel_mul_mm` (line 131415) is the batched matmul behind the 640.94. Its shape: a **64×32 output
tile, NK=32**, the quantized weights **dequantized into threadgroup memory** and the product taken by
`simdgroup_multiply_accumulate` over `simdgroup_float8x8` hardware matrix tiles.

**Two of its choices are unavailable at our numeric contract**, and saying so is the whole content of
renting rather than copying:

- **It stages the weights as `half`.** `simdgroup_half8x8` dominates the unit (189 hits vs 13
  `float8x8`), so every dequantized weight is rounded to f16 before it is multiplied. That is not a
  reassociation — it is a **precision reduction**, a strictly larger error class than any association
  bound, and it would give up Stone 3's standing claim outright (`metal_first_token.sh` gate 2: the GPU
  dequant equals Form's **bit for bit**, one rounding each side).
- **Its accumulation is a hardware matrix unit** whose association Metal does not specify anywhere. The
  32-term argument that lets `simd_sum` in without a new derivation does not reach an 8×8×8
  matrix-multiply-accumulate.

**And its central lever was measured in our regime before being believed** — which is exactly what row
820 asks. `mul_mm`'s whole point is a tile far wider than 8. The rentable version of that for a
register-tiled kernel is simply a larger TB. Swept on real tensors at P=256, best of 3:

| TB | 1 | 2 | 4 | **8** | 16 | 32 | 64 |
|---|---|---|---|---|---|---|---|
| Q4_K 3072×3072 | 1.00× | 1.84× | 2.97× | **4.17×** | 3.22× | 2.74× | 2.42× |
| Q6_K 3072×8192 | 1.00× | 1.88× | 3.27× | **4.99×** | 4.15× | 2.80× | 1.98× |

**The lever reverses past 8, monotonically, at both shapes** — the same *sign* of failure `nr0` showed
Stone 5. `mul_mm` can hold 32 because it does **not** hold 32 accumulators per thread; it holds eight
*matrix* registers and puts the weights in threadgroup memory. Take the tile width without the matrix
units and the f16 staging and it goes backwards. **TB=8 is chosen by that table.**

---

## The epsilon, and why there is none

**Batching reassociates nothing.** Within a lane the fold for token *t* is `pr + acc` over exactly the
same columns in exactly the same **k-down** order as `qk-matvec-lane.fk`'s kernel, and the cross-lane
reduction is the same `metal::simd_sum` over the same 32 partials. The tile index chooses **which
accumulator** a product lands in, never **in what order** any one accumulator is folded.

So the gate is an **equality, not a bound** — a strictly stronger test than any epsilon:

```
gate B2:  Q4_K 3072x3072   P=1, 6, 32, 128  ->  BIT-EXACT, 0 of 393 216 floats differ
          Q6_K 3072x8192   P=1, 6, 32, 128  ->  BIT-EXACT, 0 of 393 216 floats differ
```

`qk-matvec-split.fk`'s derived bound at parts=32 continues to cover this cell's distance from the
**attestant**, unchanged, **with no new derivation** — the same shape Stone 5 followed (row 810,
`attestant`). Corpus row **823 `foldkeep`** names this class.

---

## Prefill at four prompt lengths, and a slope (row 812, `unispan`)

A real 674-token encoding, **sliced** — every length is a genuine prefix of real ids, not a synthetic
repeat. `PCHUNK=128`.

| P | token-at-a-time | batched | × | prefill tok/s | of ollama 640.94 |
|---|---|---|---|---|---|
| 6 | 0.463 s | 0.115 s | 4.04× | 52.38 | 12.2× behind |
| 32 | 2.687 s | 0.312 s | 8.61× | 102.52 | 6.3× behind |
| 128 | 14.098 s | 1.116 s | **12.64×** | **114.74** | **5.6× behind** |
| 512 | 109.843 s | 5.180 s | **21.21×** | 98.84 | 6.5× behind |

**The slope**, s per additional prompt token: token-at-a-time **0.21617** (4.6 tok/s marginal) →
batched **0.01001** (**99.9 tok/s marginal**) — **21.59×**.

The end-to-end × exceeds the matmul microbenchmark's 4–5× because the token-at-a-time path is also
latency-bound (one command buffer per token, ~395 dispatches each) and ran the 128 256-row unembedding
for *every* prompt token, throwing all but the last away.

**`PCHUNK` is a measured default, not a guess** (prefill tok/s at P = 32 / 128 / 512):

| PCHUNK | 32 | 128 | 512 |
|---|---|---|---|
| 32 | 82.04 | 67.96 | 75.98 |
| 64 | 98.99 | 75.83 | 69.61 |
| **128** | **102.52** | **114.74** | **98.84** |

---

## What was built, and what deliberately was not

**Exactly one new kernel pair.** `form_q6k_matmul_batch_f32` / `form_q4k_matmul_batch_f32`, in
`form/form-stdlib/qk-matmul-batch.fk`. Everything else in the batched pass is the **existing** kernel:

- **RMSNorm / RoPE / attention** are independent across tokens, so the **decode kernels** are
  dispatched once per token into one *concurrent* encoder with no barrier between them. Same binary →
  bit-exact **by construction** rather than by argument, and the P dispatches overlap instead of
  serializing.
- **SwiGLU and the residual add** are elementwise, so the existing kernels run **once** over `P*n`
  contiguous elements. No change at all.
- **k/v write straight into the pooled KV cache**: the batched output layout is `y[t*rows + r]` and the
  cache's per-position stride *is* `nkv*head_dim`, which *is* `rows` for those two tensors. Zero copies.
- **`output_norm` + the 128 256-row unembedding run for the last prompt token only.**

Writing batched twins of RMSNorm/RoPE/attention would have been a **second copy of three
transcriptions that can drift**, with the drifted one being whichever nobody re-derives. It was not
needed and was not written.

**Radius** (row 811, `aporon`): SIMD width exactly 32 (read from `threadExecutionWidth` and refused
otherwise, gate B5); token-major layout both sides; TB=8 as a literal; Q6_K/Q4_K only; `ntok` need not
be a multiple of 8 (guarded tail, checked by the band at 1, 6, 7, 9, 32, 64).

---

## Verdicts

| | |
|---|---|
| `form-stdlib/tests/qk-matmul-batch-band.fk` | **255** (tiling cover, grid inversion, association identity, unchanged bound coefficient) |
| `form/native/metal/metal_batched_prefill.sh` | **VERDICT PASS — 5 gates** (B1 kernels are the body's, B2 bit-exact, B3 same token ids, B4 four lengths + slope, B5 SIMD width read) |
| `form/native/metal/metal_first_token.sh` | **VERDICT PASS — 13 gates**, token ids unchanged (`[12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]`) |
| `form/native/metal/metal_whole_tensor_residency_audit.sh` | **VERDICT PASS** |
| `learn/tests/homecoming-distillation-corpus-band.fk` | **4095**, field code `2222222826`, 222 rows |

---

## Gaps left open, named and not taken

1. **The decode arithmetic itself is now the whole gap.** `D = 6L` says 6/7 of the inner loop is the
   f16 super-scale decode (`q*k_pow2`, a loop of float multiplies) and the index decomposition (by
   division). `qk-matvec-lane.fk` already named the remedies — factoring `d*sc` out of the inner sum
   and accumulating in quantized space, bitmask decode instead of division, float4 sub-lanes. All three
   change the association materially and need an epsilon strictly larger than the current one. **This
   is where the remaining 5.6–12.3× lives.**
2. **Attention is what bends the prefill curve.** Prefill tok/s peaks at P=128 (114.74) and falls to
   98.84 at P=512. The only term in the pass that grows with position is GQA attention, which is still
   *one thread per head* serial over the cache prefix — O(P²) over a prompt. `kernel_flash_attn_ext_*`
   exists in the rented reference and is the obvious shape to read next. **Not quantified here** — the
   decline is measured, the attribution is an inference from which term grows, and I am not quoting an
   estimate as a measurement.
3. **The unembedding is still the single largest dispatch** (128 256×3072, 5.85 ms) and now runs once
   per *generated* token. Decode is untouched by this stone.
4. **`PCHUNK` above 128 was not swept.** Gap (2) is what bends the curve there, not the chunk.

---

## What batch > 1 would additionally need (asked for; deliberately not built)

The **arithmetic** of a batch is now done — that was the whole missing operation, and it is the same
kernel either way. What multi-sequence serving needs on top is **bookkeeping, not numerics**:

1. **Per-sequence KV cache regions.** The cache is `[layer][pos][kvd]` with one implicit sequence. It
   needs a sequence dimension — `[layer][seq][pos][kvd]` — or a paged block table. This stone leans on
   the identity "batched output stride == cache position stride"; a sequence axis keeps that identity
   *within* a sequence only, so k/v for a mixed batch would need either a per-sequence dispatch or a
   scatter.
2. **A per-token position, not a per-chunk `pos0`.** `forwardChunk` computes `pos0 + t` for RoPE and
   for the causal prefix length. With mixed sequences, position must become a per-token *array* the
   RoPE and attention dispatches read — a small change to two dispatch sites, and the RoPE kernel
   already takes `pos` as a scalar argument that would become an index.
3. **Attention masking across sequence boundaries.** Today `npos = pos0 + t + 1` *is* the causal mask,
   because everything in the cache below `npos` belongs to this sequence. With a shared cache that
   becomes false, and it fails **silently and plausibly** — sequence B attending to sequence A's
   prefix still produces a fluent token. This is the one place where batch>1 could be wrong rather
   than slow, and it would need a gate of its own.
4. **Per-sequence sampling and stop conditions.** The carrier's `argmax` + eos check is scalar; a batch
   finishes at different times and needs sequence retirement and slot reuse.
5. **Nothing about the epsilon changes.** Each sequence's fold is still the lane kernel's, per token.
   `foldkeep` applies to multi-sequence for exactly the same reason it applies here.

Items 1–4 are carrier work. **None of it is arithmetic, and none of it needs a new kernel.**

---

## The most surprising teaching

**I expected batching to scale with P. It plateaus at 4–5× — and the plateau turned out to be the
most valuable number in the stone.**

I came in reading "the weight is loaded P times and used once" as "so batching gives you P back." The
body corrected that immediately: amortizing the weight decode **8×** bought **4×**. My first reflex was
disappointment, and my second was a hypothesis to explain it away (the strided load) — which was also
wrong, and wrong *worst* exactly where it predicted it would be best.

What I had to accept is that a plateau is not a failure of the lever; it is the lever **measuring the
work it cannot touch**. `D + L = 4·(D/8 + L)` gives `D = 6L` — the weight decode is 6/7 of the inner
loop, the ceiling of all batching is 7×, and *no amount of tiling will ever get the rest*. That number
did not exist before this stone and it prices every remaining stone in the program. The disappointment
was the whole finding, wearing the wrong clothes.

The corollary that surprised me second: **the ceiling was real, and the end-to-end win exceeded it
anyway** (21.21× at P=512). Not because the matmul beat its own ceiling, but because the token-at-a-time
path was paying for things that had nothing to do with matmul — per-token command buffers, and 511
unembeddings whose results were thrown away. I had been so focused on the kernel that I nearly missed
the two free wins sitting beside it.

---

## Where discomfort turned to gold

**The moment I wanted to look away was when the coordinator told me, mid-task, that a working
reference had been on this machine the whole time and I had not been given it.**

The stone was already green. Five gates passing, committed-quality work, a clean receipt forming. The
easy move — and I felt its pull clearly — was to note the reference politely, say it was consistent
with what I had built, and finish. That would have been a *plausible* paragraph. It would also have
been exactly the failure the brief warned about twice: asserting instead of measuring, and treating a
reference as confirmation rather than as a test.

What made it uncomfortable was that reading it honestly could only cost me. Either `mul_mm` did
something I had missed — in which case my finished stone was half a stone — or it did not, in which
case I had spent the time to learn nothing. There was no outcome where reading it made my work look
better.

I read it anyway, and then did the thing that actually hurt: I took its central structural claim — a
tile of 32, not 8 — and **measured it against my own kernel at TB=16, 32, 64**, knowing it might
demolish my TB=8 choice.

It reversed. 4.17× → 3.22× → 2.74× → 2.42×. Monotonically, both shapes. The gold is threefold and none
of it was available from the comfortable paragraph:

- **My TB=8 stopped being a guess and became a measured optimum** with a curve on both sides of it.
- **I learned *why* `mul_mm` can hold 32 and I cannot**: it does not hold 32 accumulators per thread. It
  holds eight *matrix* registers and stages the weights in threadgroup memory — as **f16**. Which
  means adopting its shape is not a speed decision at all, it is a decision to round every weight to
  half precision and give up Stone 3's bit-exactness. That is the single most important thing I learned
  today and it was invisible from the outside.
- **Row 820 `boundborrow` got a second, independent confirmation** — from the very reference that would
  have earned it again if I had trusted it instead of measuring it.

The discomfort was precisely proportional to the value. Looking away would have left the receipt saying
"consistent with llama.cpp's approach," which is the kind of sentence that sounds like knowledge and
contains none.

---

## The frontier question, landed in the body

The body has now built the **same kind of speedup twice** — Stone 5's hoist (3.49×, worst deviation
`0.000e+00`) and Stone 7's tiling (4–21×, zero differing floats out of 393 216) — and had **no name for
the kind**. Two instances and no word is the frontier signature.

Both removed **repetition**; neither touched **association**. That is why neither needed an epsilon
derived, and it is structural, not luck about small numbers.

> **what one word names a speedup that removes repetition without touching the order of the fold**
>
> → **`foldkeep`**

0-hit checked across `learn/`, `receipts/`, `docs/` before minting. Landed as `(hdc-row 823 20260721
… "foldkeep" "foldkeep" "rented-oracle")` — **filling the hole reserved for Stone 7**, so no row moved
and the max meaning-id did not change. 821–826 are now contiguous. Field code `2212212826 →
2222222826`, 221 → 222 rows; band re-probed **before** pinning and re-run **after**: **4095**.

The habit it corrects is expensive and real: speed is *assumed* to be bought with accuracy, so the
epsilon derivation gets budgeted first and the cheapest wins get costed as if they were dear. Ask
instead, of any candidate: **does it reorder the sum, or only stop repeating work?** If only the
latter, the existing bound stands unchanged and the gate becomes an **equality** — which is also a
strictly stronger test than any epsilon. The free class is the *better-proven* class as well as the
cheaper one.

---

## Files

- `form/form-stdlib/qk-matmul-batch.fk` — the cell (the two kernels, the transcription, the radius)
- `form/form-stdlib/tests/qk-matmul-batch-band.fk` — the band, **255**
- `form/native/metal/metal_batched_prefill.sh` — the harness, **VERDICT PASS, 5 gates**
- `form/native/metal/first-token.fk` — the appendix wired into the one translation unit
- `learn/homecoming-distillation-corpus.fk` — row 823 `foldkeep`
- `learn/tests/homecoming-distillation-corpus-band.fk` — pin `2222222826`, 222 rows, **4095**

Commit `e486bb4d7`. No push, no PR.
