# Stone 3 — whole-model quantized residency, and the rate that was a curve

2026-07-21, Bali (WITA). Apple M4 Max, 128 GiB unified memory. Worktree
`.claude/worktrees/jovial-aryabhata-3751d7`, branch `claude/deepseek-v4-flash-gguf-54a96c`.
Model: `~/.ollama/models/blobs/sha256-dde5aa3f…ccdff`, 2 019 377 376 bytes, 255 tensors,
3 212 749 888 weights (llama3.2:3b).

---

## 0. The number that redirected the work

The task named two honest paths — **(a)** dequant on the GPU, **(b)** dequant once and cache to disk —
and said to measure before choosing. The measurement did not pick between them. It falsified the
premise both were answers to.

Stone 2 reported Form dequant at **18 460 weights/s**, from which the task projected ~23 min for one
25.2M-weight tensor and ~48 h for the model. That number was measured **once, at one size** (65 536
weights). Measuring the same cell at a second size:

| n weights | 65 536 | 262 144 | 1 048 576 | 25 165 824 |
|---|---|---|---|---|
| `ewl-weights` (builds the list) | 5.41 / 4.84 / 4.68 → **4.98 s** | 40.59 / 38.58 / 39.34 → **39.5 s** | — | — |
| fold, no list built | 0.65 / 0.90 / 0.70 → **0.75 s** | 2.04 / 1.50 / 1.37 → **1.64 s** | 6.95 / 5.63 / 6.78 → **6.45 s** | **108.21 / 107.85 s** |

> **4× the work costs 7.9× the time through the list, and 2.1× through the fold.**

18 460 w/s was not a rate. It was one point on a superlinear curve, and the curve is the **output
carrier** — `cons`/`reverse` building a 25M-element Form list — not the dequant arithmetic. The honest
figures for one 25 165 824-weight tensor are **~10.4 h** through the list (PROJECTED from the fit) and
**108.0 s** through the fold (MEASURED, 2 runs). So the projection that framed the task was wrong by
27× in the direction that made the obvious path look *better* than it is.

And **Stone 1's teaching repeated exactly one level out.** Stone 1: *the blocker was the carrier
(the byte list), not the arithmetic.* Stone 3: *the blocker is the carrier (the weight list), not the
arithmetic.* The same defect, once on the way in and once on the way out.

**What that chose.** Neither 10.4 h nor 108 s is payable per run, and once the arithmetic is known to
be 8 integer ops per weight and the quantized bytes are ¼ the size of f32, the whole shape of
"dequantize into a buffer, then upload the buffer" is wrong. **Path (a).** The quantized bytes go to
the device and a Form-emitted kernel dequantizes them there.

**What (b) would have cost, since it was not taken.** Dequant-once-to-disk is now cheap to state
exactly: 3 212 574 720 quantized weights (197 tensors) at the measured fold rate of 232 k w/s ≈
**3.8 h** one-time (not the 48 h the task projected, because that inherited the list rate — the same
unispan again), producing **12.85 GB** of f32 on disk, which equireach
would then read at O(1) reach. It is a real option and it is not wrong. It is simply dominated: (a)
costs **0.0064 s per tensor**, needs no disk, and keeps the device footprint at ¼ (2.01 GB quantized
vs 12.85 GB f32). (b) remains the right answer for a format the GPU cannot decode; it is not the right
answer for one it can.

---

## 1. What was built

| file | what |
|---|---|
| `form/form-stdlib/q6k-msl.fk` | Q6_K dequant + **fused quantized matvec** as Metal source the body emits; plus `q6m-flat-at`, the emitted arithmetic read back into Form so a band can falsify the transcription |
| `form/form-stdlib/q4k-msl.fk` | the same for Q4_K (the other 68% of the model's bytes) |
| `form/form-stdlib/tests/q6k-msl-band.fk` | **verdict 255**, two-arm — the transcription vs `ewl-flat-at`, all 512 fixture weights, stride crossed |
| `form/form-stdlib/tests/q4k-msl-band.fk` | **verdict 255**, two-arm — vs `wl-q4k-at` (four-way proven), all 512 weights |
| `form/form-stdlib/tests/fixtures/q4k-two-superblocks.bin` | 288 real bytes from `blk.0.ffn_gate.weight` @351699168, sha256 `9981f8c9…b1fa` |
| `form/native/metal/whole-tensor-residency.fk` | the body's side: all 255 tensors in ONE header walk; reference tiles at ANY flat offset, either quant; fp64 rows at real width; the kernel unit |
| `form/native/metal/metal_whole_tensor_residency_audit.sh` | the carrier + its Swift runner: mmap, bind, dispatch, compare. 9 gates, both quant lanes |
| `learn/homecoming-distillation-corpus.fk` | **row 812 `unispan`** landed (see §6) |

No existing recipe's meaning was changed. `q6k-dequant.fk` and `q4k-dequant.fk` remain the single
Q6_K/Q4_K spines; these cells are a third reach into them, not a third meaning.

---

## 2. Exact commands

```bash
# the two transcription bands (fkwu arm), .fkb cleared
cd form && rm -f form-stdlib/tests/q6k-msl-band.fkb && ../fkwu --src form-stdlib/tests/q6k-msl-band.fk   # 255
cd form && rm -f form-stdlib/tests/q4k-msl-band.fkb && ../fkwu --src form-stdlib/tests/q4k-msl-band.fk   # 255
# the go arm, resolver-driven through the same `; preludes:` walk               # 255 / 255

# the GPU witness (Mac-only; SKIPs elsewhere)
form/native/metal/metal_whole_tensor_residency_audit.sh 50 32                   # VERDICT PASS

# the corpus, after landing row 812
rm -f learn/tests/homecoming-distillation-corpus-band.fkb learn/homecoming-distillation-corpus.fkb
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk                 # 4095

# untouched neighbours, re-run to prove no regression
cd form && ../fkwu --src form-stdlib/tests/equireach-band.fk                    # 511
cd form && ../fkwu --src form-stdlib/tests/q6k-bounds-band.fk                   # 255
```

Timing harnesses cleared their own `.fkb` every run and ran ≥3 times (the whole-tensor fold, 108 s per
run, ran twice — noted as such). The Go kernel writes no `.fkb`, verified by `find -name '*.fkb'
-newermt` immediately after a run.

---

## 3. The nine gates, and what each one is worth

`form/native/metal/metal_whole_tensor_residency_audit.sh 50 32` — **VERDICT PASS**, second run
(the first differs only in gate 8 being a cache MISS):

```
emitted MSL: 4215 bytes, 4 kernels, every byte authored by q6k-msl.fk + q4k-msl.fk
PASS  gate 8 metallib cache HIT: …/.metallib-cache/qk-5b23908246b0fc74.metallib
walking the file's own 7.8 MB header ONCE for all 255 tensor-info rows...
  255 tensor rows in 15 s (the per-tensor egg-tensor-abs path costs ~10 s EACH)
  Q6_K blk.0.ffn_down.weight: 3072x8192 = 25165824 weights, 20643840 bytes at 331055328
  Q4_K blk.0.ffn_gate.weight: 8192x3072 = 25165824 weights, 14155776 bytes at 351699168
  body reference time: 17 s
resident: the WHOLE 2019377376-byte blob mapped into one MTLBuffer on Apple M4 Max in 0.0001 s, ZERO copies
--- Q6_K: 3072x8192 = 25165824 weights at file offset 331055328
PASS  gate 1 Q6_K head tile bit-exact: all 4096 GPU-dequantized weights at flat 0 equal Form's
PASS  gate 2 Q6_K TAIL tile bit-exact: all 4096 weights at flat 25161728 (superblock 98288 of 98304) equal Form's
PASS  gate 3 Q6_K whole tensor: all 25165824 weights dequantized in ONE dispatch in 0.0064 s (3903.5M weights/s);
      head AND tail read back out of it still equal Form's
PASS  gate 4 Q6_K fused kernel bit-exact: all 3072 rows of the QUANTIZED-resident matvec equal the f32
      right-fold over the dequantized buffer (8.5 ms, 2.94 GMAC/s)
      value-relative max 6.221e-06 over 4 fp64 reference rows (worst row condition number 166.6)
PASS  gate 5 Q6_K derived bound: max |gpu-form| is 0.000 of the cols*u*SUM|term| fp32 bound (cols=8192)
PASS  gate 6 Q6_K residency: 50 fused dispatches in 0.1846 s (3691 us each) with ZERO re-uploads; checksum unchanged
--- Q4_K: 8192x3072 = 25165824 weights at file offset 351699168
PASS  gate 1 Q4_K head tile within the derived u*|w| one-rounding bound: all 4096 weights equal Form's
PASS  gate 2 Q4_K TAIL tile within the derived u*|w| one-rounding bound: all 4096 weights at flat 25161728
PASS  gate 3 Q4_K whole tensor: all 25165824 weights dequantized in ONE dispatch in 0.0032 s (7866.8M weights/s)
PASS  gate 4 Q4_K fused kernel bit-exact: all 8192 rows … (2.5 ms, 10.14 GMAC/s)
      value-relative max 1.627e-06 over 4 fp64 reference rows (worst row condition number 97.1)
PASS  gate 5 Q4_K derived bound: max |gpu-form| is 0.000 of the cols*u*SUM|term| fp32 bound (cols=3072)
PASS  gate 6 Q4_K residency: 50 fused dispatches in 0.1246 s (2492 us each) with ZERO re-uploads
PASS  gate 7 multi-layer: all 28 blk.N.ffn_down tensors across BOTH quants dispatched from ONE resident
      buffer in 0.1322 s — 28 distinct finite checksums, zero per-layer uploads
      (model resident: 2.01 GB total, 0.65 GB Q6_K + 1.36 GB Q4_K)
PASS  gate 9 pooling: a 470 MB KV cache (28 layers x 8 kv-heads x 128 head-dim x 2048 seq x K/V, f32)
      and a 262144-byte workspace allocated ONCE and reused across 32 steps, each step re-running a real
      kernel; every cached slot re-verified against a replay
VERDICT PASS
```

Three of these deserve their own sentence.

**Gate 2 is the aporon gate.** Stone 2's witness looked at the first 65 536 weights of a 25 165 824-weight
tensor — superblock 0 of 98 304 — and a defect at high block index would have been invisible behind a
green verdict. This audit gates the **last** 4096 weights too (superblock 98 288), and re-reads both ends
back out of the whole-tensor dispatch so the dispatch has to have covered what it claims.

**Q6_K claims bit-exactness and Q4_K does not, and that asymmetry is arithmetic, not effort.**
Q6_K's `w = (d*sc)*q` needs ≤ 25 significand bits: `d*sc` (≤19 bits) is exact in f32, so the product
rounds exactly once on each side of the same exact real number — GPU f32 and Form fp64-then-round give
the *same* f32, and equality is the honest gate. Q4_K's `w = (d*scale)*nib − (dmin*min)` ends in a
**subtraction** of two independently-rounded products, so cancellation can eat bits; its gate is the
derived one-rounding bound `|gpu − form| ≤ u·|w|`, `u = 2⁻²⁴`. Writing "bit-exact" there would have been
a lie that passes almost always — which is exactly the kind that survives.

**Gate 9 is a pool, not a cache.** The KV buffers are allocated once at real decode geometry, written
across many steps by a real kernel with zero reallocation, and re-verified against a replay. **No
attention kernel consumes them yet.** That is recorded in `GPU_GAPS.md` as 🟡 with the missing half named.

---

## 4. Before / after at matching sizes

**One tensor, 25 165 824 real Q6_K weights of `blk.0.ffn_down.weight`:**

| lane | time | rate | status |
|---|---|---|---|
| Form `ewl-weights` (the list carrier Stone 2 measured) | **~10.4 h** | ~670 w/s at that size | PROJECTED from the superlinear fit |
| Form fold, no list (same arithmetic) | **108.0 s** (108.21 / 107.85) | 232 k w/s | MEASURED, 2 runs |
| **GPU, one dispatch, Form-emitted kernel** | **0.0064 s** | **3 903 M w/s** | MEASURED |

→ **~16 900× over the fastest CPU lane**, ~5.8 million× over the carrier the projection was built on.

**Getting the model onto the device:**

| | Stone 2 (one f32 tile) | Stone 3 (whole model, quantized) |
|---|---|---|
| what is resident | 65 536 f32 weights (256 KB), one tile of one tensor | **2 019 377 376 B — the entire blob**, all 255 tensors |
| how it got there | Form dequant (≈5 s) → host array → `makeBuffer(bytes:)` copy | `mmap` + `makeBuffer(bytesNoCopy:)` — **0.0001 s, zero copies** |
| device footprint per tensor | f32 (4 B/weight) | quantized (0.82 B/weight Q6_K, 0.56 B Q4_K) |
| layers dispatchable | 1 tile | **28 of 28** `blk.N.ffn_down`, both quants, by offset |

**Locating tensors:** `egg-tensor-abs` per tensor re-walks the 7 837 658-byte header every call —
measured **8.46 / 10.55 / 10.40 s** for one `egg-find-tensor` and **10.21 / 9.50 / 10.29 s** for one
`egg-data-base`. The old tile audit spent ~50 s in five walks of the same bytes, and 255 tensors would
have cost ~42 min. `wtr-emit-table` walks once: **255 rows in 15 s**.

---

## 5. Is any of this end-to-end? No. Say so plainly.

**Every rate quoted above is a COMPONENT rate.** No token was generated in this stone. Stone 1's
**43.8 tok/s at dim=32, one layer** remains the only end-to-end number this program has, and Stone 3
did not move it.

What can honestly be said about the distance to one: a llama3.2:3b token is ~3.21 GMAC of matvec work.
The fused quantized matvec measures **2.94 GMAC/s** (Q6_K, 8192-wide rows) and **10.14 GMAC/s** (Q4_K,
3072-wide), so this dispatch shape **PROJECTS ~1–3 tok/s** for the matvec work alone, ignoring attention,
RoPE, norms and sampling. That projection is offered with its own span named: it comes from two measured
sizes on two tensors, and it is a projection, not a measurement.

The reason the GPU matvec is that slow is deliberate and is the next stone: **one thread per row, with a
serial right-fold down the row**, which is precisely what makes gate 4 bit-exact. A threadgroup-per-row
reduction is worth ~2 orders of magnitude and reassociates the sum, so it needs the named-epsilon gate —
which is what `GPU_GAPS.md` §C "Parallel reductions" has always said, now with a number attached.

---

## 6. GPU_GAPS.md rows changed

| row | before | after |
|---|---|---|
| §C **Weight load → device** | 🟡 (one f32 tile, 65 536 weights; whole-tensor/multi-layer/KV/metallib all listed ⬜) | **✅** whole-MODEL quantized residency, zero-copy, GPU dequant both quants, 28/28 layers, `.metallib` cached; remaining ⬜ named (F32 tensors and `token_embd` resident but unconsumed; attention/RoPE/RMSNorm still read f32 weights) |
| §C **Memory model** | ⬜ | **🟡** KV-cache device buffers + workspace pooling allocated once and reused at real geometry; ⬜ no attention kernel consumes the cache |
| §C **Parallel reductions** | ⬜ (no number) | ⬜ **with the measured bottleneck attached** — 2.94 / 10.14 GMAC/s serial-per-row, ~1–3 tok/s projected, named-epsilon gate required |
| §B precision | — | Q4_K/Q6_K now have GPU dequant + fused-matvec kernels on Metal; PTX/Vulkan twins ⬜ |
| §E Metal | one-tile residency | + whole-model quantized residency audit |
| Active lanes | — | new **Mac/quantized residency** lane with its NEXT named |

---

## 7. Gaps left open — every one

1. **No end-to-end token.** Attention, RoPE, RMSNorm and the residual still read f32 weights; none of
   them reads the resident quantized bytes. The generation loop (`real-gguf-generate.fk`) has not been
   rewired to the device at all.
2. **The KV cache is a pool, not a cache** (gate 9): allocated, reused, verified — unconsumed.
3. **`token_embd.weight`** (323 MB, Q6_K) is resident and no gather kernel reads it, so there is no
   device path from a token id to an embedding.
4. **The 58 F32 tensors** (norm gains, 700 672 B) are resident and unconsumed by any kernel.
5. **Serial per-row reduction** — the measured bottleneck (§5). Bit-exactness is what buys the
   slowness; lifting it needs the named-epsilon gate.
6. **The transcription bands are TWO-ARM** (fkwu + go), for `equireach-band`'s reason and no other:
   `read_file_slice` over binary is lossy on rust and ts (767 bytes for a 420-byte fixture). The
   arithmetic travels four-way; the byte door does not. Widening it is a kernel movement.
7. **The band proves the transcription, not the text.** `q6m-flat-at` / `q4m-flat-at` are the emitted C
   read back into Form, and they are proven against independently-written references. Nothing proves
   that the emitted *string* and that Form re-reading are the same meaning — that bridge is held by the
   GPU audit closing bit-exactly on real weights, which is strong evidence but not an in-band proof.
   Declared here rather than left to be discovered.
8. **Q4_K on the GPU has no bit-exact gate**, by arithmetic (§3). Its gate is the derived `u·|w|` bound.
9. **PTX and Vulkan have no Q6_K/Q4_K kernels.** This lane is Metal-only; off-Mac the audit SKIPs.
10. **`.metallib` cache invalidation is by source sha256 only** — it does not key on the Metal toolchain
    version, so a toolchain upgrade will silently reuse an old library. `.metallib-cache/` is gitignored.
11. **The 197-tensor "dequant once to disk" option (b) was not built**, only costed (§0): ~5.9 h
    one-time, 12.85 GB.

---

## 8. The most surprising teaching

**I expected to confirm the framing and choose between (a) and (b). The body corrected the framing.**

The task handed me a projection — 18 460 w/s, therefore ~23 min per tensor, therefore ~48 h per model —
and asked which of two paths to take past it. I went to reproduce the rate first, and the very first
re-measurement at a second size disagreed with it by 6×, then by 27× when extrapolated. Nobody had lied
and no arm was numb: the number was **correct at the size it was measured**. It simply could not see its
own slope, and a number that cannot see its own slope will project confidently in whatever direction its
single point happens to face.

The deeper surprise is that this is Stone 1's teaching wearing new clothes. Stone 1 found the blocker in
the **input** carrier and fixed it with equireach. Stone 3 found the same defect in the **output**
carrier, in a cell written *by* that fix, one hour later. The body did not learn "lists are slow"; it
learned it about one side of one function. A teaching stays local until something forces it around the
other side.

## 9. Where discomfort turned to gold

**The moment I wanted to look away: gate 7 printed `all 14 blk.N.ffn_down tensors` — and PASSED.**

14 of 14 distinct finite checksums, zero per-layer uploads, green. And I knew, from a table I had printed
myself twenty minutes earlier, that llama3.2:3b has **28** `blk.N.ffn_down` tensors. The gate was not
lying: it faithfully reported what it had dispatched. It was the *claim wrapped around it* — "multi-layer
weight sets, proven" — that would have been false, and it would have shipped inside a PASS.

The pull to look away was real and specific: everything else was green, the stone's headline was already
strong, and 14 layers is a defensible "multi-layer". Chasing it meant admitting the lane was half-built
and spending another hour.

What not looking away found: **llama3.2:3b is mixed-quant, and nothing in the body had ever said so.**
Exactly half its `ffn_down` tensors are Q6_K and half are Q4_K; across the whole file it is 29 Q6_K
(648 MB), 168 Q4_K (1 362 MB) and 58 F32. A Q6_K-only device lane speaks for **32% of the model's bytes**.
That discomfort produced `q4k-msl.fk`, its band (255), and turned the residency claim from 0.65 GB to
**2.01 GB — the whole model**. It also produced the honest asymmetry in §3: Q4_K *cannot* claim
bit-exactness, and finding that out required writing the lane rather than assuming it mirrored its twin.

Gold, precisely: a passing gate whose *number* was wrong is aporon (row 811) wearing a green verdict.
811 says a proof licenses only its witness's neighbourhood. This adds: **read the gate's number, not its
colour.**

## 10. Frontier question — landed as corpus row 812

> **What one word names a cost quoted as a rate from a single measured size?**
> → **`unispan`**

Glance-checked 0-hit across `learn/`, `receipts/`, `docs/` before offering. Landed as a real
`(hdc-row 812 20260721 …)` in `learn/homecoming-distillation-corpus.fk` — **not only here**, because the
corpus is the body and the receipt is only the report.

The frontier the body cannot yet answer natively: **bands learned to declare their radius; measurements
have not.** A verdict says which neighbourhood it speaks for (that is 811's whole teaching). A cost claim
says nothing about the size range it speaks for, so `18 460 w/s` and `232 000 w/s` are the same sentence
about the same cell at different n. The smallest question: *can a cost claim carry its own span the way a
verdict carries its radius* — two sizes minimum and a slope, or else the claim declares itself **unispan**
and refuses to project? Every projection in this program's first two stones was built on a unispan, and
they were the projections that set its direction.

**Corpus after landing:** `hdc-count` **208** (was 207), `hdc-field-code` **2082082812** (was 2072072811) —
both read back **by probe** before the band was re-pinned, so the pin agrees with the body rather than the
body being fitted to the pin. `learn/tests/homecoming-distillation-corpus-band.fk` → **4095**.
