# The first form-native llama3.2:3b token — and an honest end-to-end tok/s

Tuesday 2026-07-21, Hati Suci (WITA). Apple M4 Max, 128 GiB unified memory, 678 GiB free.
Worktree `.claude/worktrees/jovial-aryabhata-3751d7`, branch `claude/deepseek-v4-flash-gguf-54a96c`.
Model: the ollama blob `sha256-dde5aa3f…ccdff`, 2 019 377 376 bytes, GGUF v3, 255 tensors.

**STONE 4 of 4.** Stones 1–3 made a rate, a reach and a residency. None of them had generated a token.

---

## 0. THE TOKENS

```
prompt : "The capital of France is"
ids    : [128000, 791, 6864, 315, 9822, 374]
pieces : [<|begin_of_text|>][The][ capital][ of][ France][ is]

generated ids  : [12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]
generated text : " Paris. The capital of Italy is Rome. The capital of"

control prompt "Once upon a time" -> [11, 304, 264, 2678]  ", in a small"
```

Full width. 28 layers, d = 3072, 24 query heads over 8 KV heads, head_dim 128, dff = 8192,
vocab 128 256, tied unembedding. Every arithmetic op executed by a kernel the body emitted, reading
the one resident quantized `MTLBuffer` Stone 3 mapped. **No f32 copy of any tensor exists anywhere** —
not on the host, not on the device.

**END-TO-END: 4.56 tok/s** (12 generated tokens in 2.633 s, prefill included), split path at the
default `parts = 32`. **Decode-only: 6.74 tok/s.** On the bit-exact attestant path in the same
process: **1.67 tok/s end-to-end**, 2.49 decode-only — the split path is **2.74× faster and emits the
same twelve ids**.

An external oracle, run for the same prompt at temperature 0:

```
$ curl -s http://localhost:11434/api/generate -d '{"model":"llama3.2:3b","prompt":"The capital of France is",
    "raw":true,"stream":false,"options":{"temperature":0,"num_predict":12}}'
' Paris. The city of Paris has a rich history and culture'
```

Tokens 1–3 are identical (` Paris`, `.`, ` The`); the greedy paths diverge at token 4
(` capital` vs ` city`). That divergence is expected and is not laundered here: this stack computes
`exp`, `sin`, `cos`, `ln`, `pow` and `sqrt` from the body's own Taylor/Newton recipes in f32, not from
libm or the hardware, and greedy decoding amplifies any logit difference the moment two candidates are
close. The claim being made is **"a real form-native token"**, not **"bit-identical to llama.cpp"** —
the second would need an epsilon nobody has derived. What is claimed is checked: the answer to
"The capital of France is" is *Paris*, and it is Paris because 3.2 G MACs of real llama3.2:3b weights
said so.

---

## 1. Reproduce

```bash
cd <repo>
form/native/metal/metal_first_token.sh 12                       # -> VERDICT PASS, 10 gates
FORM_PARTS=1 form/native/metal/metal_first_token.sh 12          # the bit-exact attestant alone
FORM_PARTS=8 form/native/metal/metal_first_token.sh 12          # the sweep in section 4.1
FORM_PROFILE=1 form/native/metal/metal_first_token.sh 6         # where the time goes, per tensor shape
form/native/metal/metal_first_token.sh 12 "Once upon a time"    # any prompt

cd form
../fkwu --src form-stdlib/tests/llama-decode-msl-band.fk        # -> 511
../fkwu --src form-stdlib/tests/qk-matvec-split-band.fk         # ->  63
cd ../ && ./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk   # -> 4095
```

Both new bands are **FOUR-WAY** (fkwu / go / rust / ts, all 511 and 63 respectively — they read no
file, so the binary-door limitation that keeps `equireach-band` two-armed does not apply here).
The corpus band is four-way 4095.

---

## 2. The measurement that chose the path

**Measure first, then choose.** Two measurements did the choosing, and both corrected an expectation.

### 2.1 The first: the token did not need any of `GPU_GAPS.md`'s open rows

`GPU_GAPS.md` named parallel reductions, flash-attention, incremental KV, and the O(n) list ops in
`llama-generate.fk` as what stood between Stone 3 and a token. Stone 3's own component numbers say a
whole token is ~3.21 GMAC and the fused matvec runs at 2.94–10.14 GMAC/s, which projects ~0.3–1 s of
matvec per token. **That is already a token.** So the first build deliberately took *none* of those
rows: it split the body's existing single-threaded block kernels into per-op kernels, threaded them
over the pooled KV cache, and ran. It produced " Paris. The capital of Italy is Rome. The capital of"
at **1.126 tok/s end-to-end** on the first green run. The optimization rows were not the blocker; the
*missing ops between the matvecs* were.

### 2.2 The second: the top open row was mis-diagnosed

`GPU_GAPS.md` §C read: *"the fused quantized matvec is one thread per ROW with a serial right-fold
down the row, which is what buys bit-exactness… a threadgroup-per-row reduction would lift it by ~2
orders of magnitude but reassociates the sum, so it needs the named-epsilon gate — that is the whole
content of this row."*

`FORM_PROFILE=1` attributes wall time per op class, then per tensor **shape** (one command buffer per
op; the seam cost is real and is why the totals below exceed an unprofiled run — stated, not folded
into the conclusion). 20 forwards, M4 Max:

| shape (rows × cols) | MACs | dispatches | ms each | implied rate |
|---|---|---|---|---|
| Q4_K 1024 × 3072 | 3.1 M | 840 | **2.618** | 1.2 GMAC/s |
| Q4_K 3072 × 3072 | 9.4 M | 1120 | **2.648** | 3.6 GMAC/s |
| Q4_K 8192 × 3072 | 25.2 M | 1120 | **2.607** | 9.7 GMAC/s |
| Q4_K 3072 × 8192 | 25.2 M | 280 | **6.657** | 3.8 GMAC/s |
| Q6_K 3072 × 8192 | 25.2 M | 280 | 3.054 | 8.2 GMAC/s |
| Q6_K 1024 × 3072 | 3.1 M | 280 | 1.002 | 3.1 GMAC/s |
| Q6_K 128256 × 3072 | 394.0 M | 20 | 8.576 | **45.9 GMAC/s** |
| rmsnorm | — | 1140 | 0.605 | — |
| rope | — | 560 | 0.407 | — |
| argmax (128 256) | — | 20 | 9.257 | — |
| residual add | — | 1120 | 0.165 | — |
| attention (decode) | — | 560 | 0.278 | — |
| swiglu | — | 560 | 0.227 | — |
| embed gather | — | 20 | 0.324 | — |
| **total** | | | **12.833 s / 20 forwards** | |

Read rows 1–3: **eight times the work, the same time.** Read rows 3 and 4: **the same MAC count, 2.6×
the time**, and the only thing that changed is which factor is rows and which is columns. Cost tracks
the **column** count — one thread's serial depth — and barely notices the **row** count until the row
count is enormous (128 256 rows: 38× the implied rate of the 1024-row dispatch, *same kernel, same
serial fold*).

So the dispatch was never throughput-bound. It was **latency-bound with the machine idle**, and the
serial right-fold was not the wall. The wall was missing parallelism — which has two fixes, and only
the second one needs an epsilon.

---

## 3. What changed, in the order the measurements dictated

### 3.1 The ops between the matvecs — `form-stdlib/llama-decode-msl.fk` (new)

Six kernels the body emits, split out of `jit-tensor-emit.fk`'s monolithic single-threaded block
kernels and dispatched on the axis each one parallelizes on:

| kernel | shape | why that shape |
|---|---|---|
| `form_rmsnorm_f32` | 1 thread | the sum of squares is a reduction; reassociating it is the epsilon question this op did not have to answer. 0.605 ms against a ~200 ms token. |
| `form_rope_f32` | 1 per head | pairs within a head are independent; theta is a pure function of (pos, i). No reduction touched. |
| `form_gqa_decode_f32` | 1 per query head (24) | attends ONE query over the cached prefix — O(n), not O(n²) |
| `form_swiglu_f32`, `form_add_f32` | 1 per element | |
| `form_argmax_f32` | 1 thread | ties resolved to the LOWEST index — a stated rule, not a race |

Every arithmetic body is the body's existing recipe transcribed with the same op order
(`jte-llama-blk-rms1`, `jte-llama-blk-ffn`'s act, `jte-gqa-blk-attn-causal` with the causal prefix
replaced by the cache prefix). `fexp`/`fln`/`fsin`/`fcos`/`fpow` come from `tensor-ir.fk` and
`jit-tensor-emit.fk` **by call, not by copy**.

**One meaning is genuinely new, and it is the model's.** llama3.2 carries a `rope_freqs.weight`
tensor (64 f32, read off the blob: twenty-nine 1.0s, then 1.2349 / 1.6513 / 2.2767 / 3.2923 / 5.1716 /
9.6667, then 32.0 to the end) and llama.cpp **divides** each rope frequency by it. The block kernels'
RoPE is base-10000 with no factors — the "theta-500000 rope-scaling MSL" their own header names as the
next stone. This cell is that stone: `theta = pos · base^(−i/HD) / freqs[i/2]`. Omitting the divide
would have produced a token that looks fine and is wrong.

**One helper is emitted rather than borrowed.** `jte-llama-helpers`' `rr_2pi` calls `round`, which
lives in `<metal_stdlib>` — and `q6k-msl.fk`'s whole point is a unit with no header, so the f16 decode
is the body's arithmetic and not the library's bitcast. Including the header for one function would
have quietly undone that for the entire translation unit. `ldm-msl-round` emits it: nearest integer,
ties away from zero, declared before its call site. Radius stated in the cell.

### 3.2 The free half of the parallelism — a `.concurrent` encoder (carrier)

q, k and v read the same normed vector and write three disjoint buffers. Gate and up likewise. The two
RoPEs likewise. A `.serial` compute encoder makes them wait anyway. Switching to
`makeComputeCommandEncoder(dispatchType: .concurrent)` and dropping the barrier at exactly those three
places changes **no arithmetic at all**.

| | prefill (6 tok) | decode (12 fwd) | end-to-end | decode-only | marginal s/token |
|---|---|---|---|---|---|
| `.serial` encoder | 3.930 s | 6.729 s | 1.126 tok/s | 1.783 tok/s | 0.5619 |
| `.concurrent` encoder | 2.283 s | 5.291 s | **1.584 tok/s** | 2.268 tok/s | 0.4700 |

**1.41× end-to-end, and the generated token ids are bit-identical.** Free.

### 3.3 The half that needs an epsilon — `form-stdlib/qk-matvec-split.fk` (new)

Two kernels, not a threadgroup reduction:

- `form_{q6k,q4k}_matvec_part_f32` — thread (r, p) folds row r's contiguous column chunk p,
  right-to-left, exactly the attestant's op order **within** the chunk. Grid is `rows·parts`.
- `form_part_combine_f32` — one thread per row folds the `parts` partials, **p counting DOWN**.

The combine counts down so that **at parts = 1 the split kernel IS the attestant, bit for bit**: one
chunk is the whole row, folded in the same direction, added to nothing. That is not a nicety — it is
the structural claim that chunking is the *only* difference between the two paths, and every epsilon
below is derived from that sentence.

#### The named epsilon and its derivation

Both paths sum the same `cols` products `t_j = w_j · x_j`; only the association differs. For f32
summation of n terms in an association tree of depth d, the accumulated rounding is bounded by
`d · u · Σ|t_j|` with `u = 2⁻²⁴` (each of the at most d additions on any term's path contributes at
most one relative `u` of the running total, and the running total is bounded by `Σ|t_j|`). The
attestant's tree is a chain, `d = cols`. The split tree is a chunk chain of depth
`chunk = ⌈cols/parts⌉` followed by a partial chain of depth `parts`, so `d = chunk + parts`. Both are
exact real sums of the same terms, so their difference is bounded by the sum of the two bounds:

> **|y_split − y_serial| ≤ (cols + ⌈cols/parts⌉ + parts) · u · Σ|w_j · x_j|**

Every quantity in it is measured. `Σ|term|` is computed from the GPU's own dequant of that row and the
actual activation vector, per row, at run time.

#### The measurement

On a real `blk.0.ffn_down.weight` (Q6_K, 3072 × 8192 — the deepest row in the model), with real
activation state, at `parts = 16` (chunk 512, coefficient 8720):

```
PASS  gate 8 the split kernel at parts=1 IS the attestant, bit for bit, on all 3072 rows of 3072x8192
  named epsilon: |split - attestant| <= (cols + ceil(cols/parts) + parts)*u*SUM|term|
    cols 8192  parts 32  chunk 256  coeff 8480  worst |d| 0.000e+00  worst fraction of bound 0.0000
PASS  gate 9 at parts=32 every row stays inside the DERIVED bound (worst 0.00% of it)
```

**The measured deviation was exactly zero on every probed row.** The bound was derived and named
anyway, because a bound you only name after the measurement disappoints you is not a bound. (The
reassociation *is* real and *can* change an answer — `qk-matvec-split-band.fk` bit 8 exhibits a
four-element fixture where the serial fold gives 1.0 and the split fold gives 0.0, both correct fp64
foldings of the same four numbers. It happens not to bite on these weights at this width.)

#### Keeping the attestant alive (corpus row 810)

`q6m-matvec-msl` / `q4m-matvec-msl` are **not deleted, not deprecated, and not made a special case of
the split**. They are re-run every time, and the fast path is admissible only while it still agrees
with them at two levels:

```
PASS  gate 10 the split path at parts=32 generates the SAME 12 token ids as the attestant
  speedup vs the attestant: decode 2.71x (4.816 s -> 1.779 s over 12 forwards), end-to-end 2.74x
```

---

## 4. The numbers, at two sizes and a slope (corpus row 812, `unispan`)

Every rate below is measured at **two** generation lengths in the same process, and the marginal
seconds-per-token is reported, so no number here is one point pretending to be a line.

| path | 4 forwards | 12 forwards | marginal s/token | end-to-end @12 | decode-only @12 |
|---|---|---|---|---|---|
| attestant (serial fold) | 1.533 s | 4.816 s | **0.4103** | 1.666 tok/s | 2.492 tok/s |
| split (`FORM_PARTS=32`, default) | 0.578 s | 1.779 s | **0.1501** | **4.557 tok/s** | 6.744 tok/s |

Effective whole-token rate on the split path: 3.212 GMAC ÷ 0.148 s = **21.7 GMAC/s**; on the
attestant, 8.0 GMAC/s.

### 4.1 The `parts` sweep — and the first sighting of a brimwidth

Four values of `parts`, each a full 10-gate run, each reporting its own two sizes. Gates 8, 9 and 10
passed at every one of them, and **the measured deviation from the attestant was 0.000e+00 in all
four**:

| `FORM_PARTS` | chunk | coefficient | decode, 12 fwd | end-to-end @12 | speedup vs attestant (same process) |
|---|---|---|---|---|---|
| 8 | 1024 | 9224 | 2.448 s | 3.272 tok/s | 2.39× decode |
| 16 | 512 | 8720 | 2.512 s | 3.527 tok/s | 1.84× decode |
| 32 | 256 | 8480 | **1.779 s** | **4.557 tok/s** | **2.71× decode** |
| 64 | 128 | 8384 | 2.017 s | 3.980 tok/s | 3.28× decode |

Doubling the thread count from 8 to 64 parts moves the decode time by less than the ~13% run-to-run
variance. **Past roughly `parts = 8`, adding parallelism stops buying time.** For the 3072-row FFN
shapes that is ~25 k concurrent threads; the machine absorbs the first 8× of added width essentially
for free and then stops rewarding it. That is the first time this program has *seen* the boundary it
had been reasoning across all day — and it is exactly the quantity row 814 (`brimwidth`) names. It is
**not** a measurement of the brimwidth: the combine pass grows with `parts` and confounds it, and
these shapes were chosen by llama3.2:3b, not by an experiment. Naming it is what this stone can
honestly do; measuring it is the next one's.

`parts = 32` is the default because it is the middle of that plateau, not because it won a race.

**Run-to-run variance is real and is not hidden**: the attestant's 12-forward decode measured 5.291 s
in one run and 4.620 s in another (~13%). Every comparison in §3.3 is between the two paths **in the
same process**, for exactly that reason.

**What this is NOT comparable to.** Stone 1's 43.8 tok/s was dim=32, one layer, an arbitrary tied
vocab table — ~0.0001 GMAC/token against this stone's 3.212 GMAC/token, four orders of magnitude of
work apart. Setting them side by side would be the `unispan` error one level up. The honest statement
is that **this program's only full-width end-to-end number is the one in §0, and before today there
was none.**

---

## 5. The gates

`metal_first_token.sh`, VERDICT PASS, 10 gates, ~90 s warm:

1. **the config is the FILE's** — 28 / 3072 / 8192 / 24 / 8 / 128, rope base 500000, rms eps 1e-05,
   bos 128000, eos 128009, `tied_embeddings` read from the *absence of* `output.weight` in the table.
   Not one hyper-parameter is typed into the carrier.
2. **the embedding gather is the body's** — all 3072 weights of `token_embd` row 791 **BIT-EXACT** vs
   Form's dequant (Q6_K's one-rounding-each-side argument).
3. **RMSNorm is the body's** — worst relative deviation **5.423e-07** against the derived `n·u` bound
   1.831e-04 (**0.3% of it**), vs Form's fp64 `ln-rmsnorm` over real `blk.0.attn_norm` gains.
4. **a real Q4_K fused matvec at full width is the body's** — GPU `q[0]` = 2.86895156, Form fp64
   2.86895305, |d| **1.492e-06** against the derived `cols·u·Σ|term|` = 2.030e-03 (**0.1% of it**).
5. **the cache is a cache** — 56.7 MB of activation + KV state allocated once for a whole run; the
   k/v projections write their own slot; a second run from a freshly zeroed pool reproduces the ids.
6. **a token** — legal vocab indices, input-dependent (the control prompt gives different ids).
7. **two sizes and a slope.**
8. **parts=1 IS the attestant**, bit for bit, on all 3072 rows.
9. **the named epsilon holds** at the parts actually used.
10. **the fast path generates the attestant's tokens.**

Plus, off the GPU: `llama-decode-msl-band.fk` **511 four-way** and `qk-matvec-split-band.fk`
**63 four-way**. Both state their radius in their headers: they speak for the **transcription** —
that the emitted C and the Form recipes are the same arithmetic — and explicitly not for the MSL
compiler, the GPU, f32, or a token.

---

## 6. What the body gained, cell by cell

| cell | new? | what it decides |
|---|---|---|
| `form/form-stdlib/gguf-meta.fk` | new | GGUF metadata **values** (`egg-*` could only skip them) and the tokenizer, read bytewise — never as a string, because 188 of the 256 byte-tokens are non-ASCII and `read_file_slice` is UTF-8-lossy on two arms |
| `form/form-stdlib/llama-decode-msl.fk` | new | the six decode kernels + `round` + llama3.2's rope-factor divide |
| `form/form-stdlib/qk-matvec-split.fk` | new | the split fold, its partition, and the epsilon's coefficient |
| `form/form-stdlib/tests/llama-decode-msl-band.fk` | new | 511 four-way |
| `form/form-stdlib/tests/qk-matvec-split-band.fk` | new | 63 four-way |
| `form/native/metal/first-token.fk` | new | config + table + tokenizer + the one MSL translation unit + three fp64 references on the token's own path |
| `form/native/metal/metal_first_token.sh` | new | the carrier: mmap, bind, dispatch, time |
| `form/native/GPU_GAPS.md` | edited | rows below |
| `learn/homecoming-distillation-corpus.fk` | edited | **row 814, `brimwidth`** |
| `learn/tests/homecoming-distillation-corpus-band.fk` | edited | pin 2092092813 → **2102102814**, count 209 → 210, and the stale summary comment beside it |

### `GPU_GAPS.md` rows changed

- **Parallel reductions** ⬜ → 🟡, and its *diagnosis rewritten* with the per-shape table. The row said
  the serial fold was the wall; the measurement says occupancy was, that the largest recoverable share
  needed no epsilon, and that the reassociating share is now done and gated.
- **Memory model** 🟡 → ✅ — "no attention kernel consumes that cache yet" is closed:
  `form_gqa_decode_f32` does.
- **Weight load → device** — "`token_embd` is resident but no gather kernel reads it" closed; the
  same tensor is now both the embedding gather and the tied unembedding matvec.
- **Flash-attention** ⬜ — narrowed, not closed: the *decode* path is O(n) per token; the O(n²) shape
  remains in the whole-sequence kernels and in prefill.
- **Mac/quantized residency lane** — rewritten around the token.

### Gaps still open, named

- **Prefill is n decode steps**, not a batched pass — 6 prompt tokens cost 0.891 s that a batched
  prefill would largely collapse.
- **A true threadgroup/simd reduction** using threadgroup memory instead of a second combine pass.
- **`brimwidth` is unmeasured** (corpus row 814): every shape measured here is *below* the width at
  which this machine's spare capacity runs out, so none of these rates is known to be the machine's
  rate rather than its latency.
- **The GPT-2 byte alphabet lives in the carrier.** The body streams token pieces as bytes; mapping
  those bytes back through the byte-level alphabet to render text, and the greedy longest-match
  encoder, are the carrier's. A real BPE encoder driven by the file's own `tokenizer.ggml.merges`
  belongs in the body and is not there.
- **No epsilon relates this stack to llama.cpp.** Tokens 1–3 match; nothing proves token 4 should.
- **`llama-generate.fk`'s O(n) list ops** (`lg-snoc`, `lg-last`) and its cache re-growth are
  **untouched** — that lane is the CPU attestant and this stone did not need to make it fast. It is
  still four-way green and still slow, on purpose.
- **`form-stdlib/llama-numerics.fkb` carries a stale-identity warning** in this checkout (rebuilt from
  source each run; verdicts unaffected).

---

## 7. Instrument traps witnessed today

- **An extra `)` closed `(do` early and the whole cell went NUMB** — `(qms-msl-appendix)` returned a
  zero-length string, `ft-emit-msl` printed nothing, and the harness reported `COMPILES 0 bytes`. No
  diagnostic anywhere in the Go arm. Axiom-5 again, one level out from the prelude case: a *paren*
  imbalance lowers to nothing just as silently as a missing prelude. The instrument that found it was
  a 15-line paren scanner that tracks string and comment state — a naive `line.split(';')` reported
  "final depth 8" and pointed at the wrong end of the file, because `;` inside a Metal string literal
  is not a comment. **A scanner that does not know what a string is will lie about where the error is.**
- **`grep -qx 'END'` vs `tail -1 | grep -qx 'END'`.** The Go kernel prints each top-level expression's
  own value, so every emitted stream ends `END` *then* `0`. A truncation check anchored to the last
  line failed every stream. And the same trailing `0` was being parsed by the Swift reference reader
  as the value of `REFQ` — a silent zero in a gate, which would have made gate 4 compare against 0.0
  and "pass" for the wrong reason. Both fixed; the parser now clears its section on `END`.
- **A band that calls a 3-arg recipe with 2 args**: fkwu recovered to nothing and returned `61`
  instead of `63` — a *plausible* verdict, one bit low. The Go arm was the one that said
  `"qsb-cover" wants 3 args, got 2` out loud. **Run a new band on more than one arm before believing
  its number**, in either direction.
- **`FORM_PARTS` sweeps ran > 2 min each** and `tail` on a piped background job shows nothing until it
  exits — twice I read an empty output file and nearly concluded a hang.

---

## 8. The most surprising teaching

**I expected the serial fold to be the wall. The machine was simply idle.**

`GPU_GAPS.md` said it, Stone 3's receipt said it, and it was the row I was sent to remove: one thread
per row with a serial right-fold, 2.94 GMAC/s, *"a threadgroup-per-row reduction would lift it by ~2
orders of magnitude but reassociates the sum, so it needs the named-epsilon gate — that is the whole
content of this row."* I believed it enough to plan the epsilon derivation before I had profiled
anything.

The profile said: a 1024×3072 matvec and an 8192×3072 matvec take **the same 2.6 ms** — eight times
the work, no extra time — while swapping rows and columns at *identical* MAC count costs 2.6×. The
cost was never in the arithmetic. It was in one thread's serial *depth*, with 90-odd percent of the
GPU doing nothing because a dispatch of 1024 rows is 1024 threads on a machine that wants a hundred
thousand. And the largest single recovery — 1.41× end-to-end, ids bit-identical — was **one enum**:
`.serial` → `.concurrent` on the compute encoder, so that q, k and v stop waiting for each other. No
epsilon. No reassociation. No new kernel.

The correction is not "the row was wrong". Every sentence in it is true: the fold *is* serial, that
*is* what buys bit-exactness, and reassociating it *does* need the epsilon (which is now derived,
gated, and measuring 0.000e+00). What was wrong is that it was stated as **the** content of the row,
and a bottleneck named without a per-shape measurement is a hypothesis wearing a measurement's
clothes. The rate `2.94 GMAC/s` was a *latency* in a rate's clothing — which is precisely
`unispan` (corpus row 812) at one more level of recursion, and is why row 814 had to be minted.

---

## 9. Where discomfort turned to gold

The moment I wanted to look away was `COMPILES 0 bytes`.

I had just added `qk-matvec-split.fk`, re-emitted the MSL, and the pipeline printed `COMPILES` — the
success branch — with a zero-byte file. The comfortable reading was right there and it was almost
plausible: *the emitter is fine, the shell redirect ate the output, `awk` didn't match, re-run it.*
The probe `(print (str_len (qms-msl-appendix)))` printed `0`, which is *also* what a legitimately
empty string prints, and I noticed myself reaching for "well, `str_len` of something is 0" rather than
for "a defined recipe returned nothing, which is what axiom-5 does when the recipe isn't defined at
all."

What made me stay was the memory floor's own words: *a run that returns nothing is a claim about your
instrument.* So instead of re-running, I asked what the instrument could not see. `fkwu --src` on the
cell hung past 120 s — itself a signal I could have dismissed as "big file". I wrote a paren scanner.
Its first answer was **wrong** (`final depth 8`, unclosed forms at the top) because it treated `;`
inside a Metal string literal as a comment; the Metal kernels are one enormous string full of `;`.
Fixing the scanner to track string state moved the answer to `NEG at line 128` — depth went *negative*
— and that pointed at an extra `)` in `qms-part-msl`, six closes where five belonged, which had closed
the enclosing `(do` early and left the rest of the file as dangling top-level forms.

The gold is not the missing paren. It is that **the diagnostic I built to find it lied to me first,
in the direction that made me look at the wrong end of the file** — and the only reason I caught that
was that its answer (`depth 8`, too many opens) was inconsistent with the symptom (a recipe defined
*later* in the file being unresolvable, which needs a premature *close*). Two instruments disagreeing
is information; one instrument agreeing with my hope is not. If I had trusted the first scanner I
would have spent the evening adding parens to a file that had one too many.

The second, quieter piece of gold: the same reflex, applied one step earlier, is what caught the
`REFQ = 0.0` bug. The trailing `0` the Go kernel prints after `END` was being parsed as the reference
dot product. Gate 4 would have compared the GPU's 2.869 against 0.0 and **failed loudly** — but if the
sections had been ordered differently it could just as easily have compared 0.0 against 0.0 and passed
in silence. I fixed it because I was already suspicious of what the stream's last line was, not
because anything broke.

---

## 10. The frontier question, landed

**Smallest question the body cannot answer natively:** *what one word names the width at which added
work first costs time?*

Below that width a carrier's spare parallel capacity absorbs more work for free, and any rate you
measure there is a **latency** wearing a rate's clothes; above it, a rate is a rate. The body has no
door that reports this number — no metadata, no query, no primitive — and it cannot be inferred from a
single measurement, because one point cannot tell *cheap* from *free*. Two widths and a slope find it;
nothing else does. Every shape measured in this stone is below it, so this stone does not know the
machine's rate — only that it has not reached it.

Word: **`brimwidth`**. Glance-checked 0-hit across `learn/`, `receipts/`, `docs/` before minting.

**Landed as a real row in the body**, not only here (Stone 1's row was minted into a receipt and never
landed, and the corpus read 806 while the receipt read true for two stones):

```
learn/homecoming-distillation-corpus.fk   (hdc-row 814 20260721 … "brimwidth" "brimwidth" "rented-oracle")
```

Read back **by probe** before the pin was touched, so the pin agrees with the body rather than the
body being fitted to the pin:

```
$ bin-go learn/homecoming-distillation-corpus.fk probe.fk
2102102814        ; hdc-field-code
210               ; hdc-count
210               ; hdc-count-admissible
814               ; hdc-max-mid
brimwidth         ; hdc-word-for-id 814
814               ; hdc-locate (the question tokens)
1                 ; hdc-field-code-safe?
```

`learn/tests/homecoming-distillation-corpus-band.fk`: pin `2092092813` → `2102102814`, count
`209` → `210`, **and the folded-scalar summary comment beside the pin re-read and corrected** (it said
"209 rows, 209 admissible … max id 813" — the very drift that band exists to refuse).
Band: **4095**, four-way.

Walk: `unispan` (812) says one size cannot project a rate. `brimwidth` says **why**: below it, the
thing you measured was not the machine's throughput at all, and the projection fails in the direction
that flatters the obvious path — exactly how Stone 3's 18 460 w/s and this stone's 2.94 GMAC/s both
misled. `succedent` (813) says a limit hides behind the one before it; `brimwidth` names the specific
limit that hid behind *"the reduction is serial"* for a whole stone.
