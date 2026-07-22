# STONE 28 — hyper-connections on the GPU

**2026-07-22, ~11:20–13:10 WITA.** Worktree `jovial-aryabhata-3751d7`, branch
`claude/deepseek-v4-flash-gguf-54a96c`. Four cells committed:
`form/form-stdlib/dsv4-hc-msl.fk` (the emitted Metal), `form/form-stdlib/dsv4-hc-demo.fk`
(the shared toy fixture + fp64 references), `form/form-stdlib/tests/dsv4-hc-msl-band.fk`
(the fp64 read-back gate, verdict **63**), `form/native/metal/metal_hc_gpu.sh` (the GPU
witness, **VERDICT PASS, 10 gates**). Plus corpus row 868 `sumkeel`.

Hyper-connections are CORE to every DeepSeek-V4-Flash layer: the whole forward pass runs
in `n_hc = 4` parallel streams, the residual stream *is* the HC state, every sublayer is
wrapped by a learned `hc_pre` / `hc_post`, and the output head collapses the streams
(Stone 25, `receipts/2026-07-22-dsv4-subsystems.md`). It was CPU-only. This stone put it
on the GPU, bit-close against the fp64 recipe, stage by stage.

---

## 0. Radius (`aporon`), before anything is believed

- The kernels are emitted by `dsv4-hc-msl.fk` and equal the CPU recipe `dsv4-hc.fk`
  (Stone 25's band 63, agreeing with an independent fp64 transcription of ds4.c to 1e-9).
  This stone does not re-establish the recipe's correctness; it inherits it and shows the
  GPU computes the same graph.
- **NOT bit-exact.** HC has one transcendental — `exp`, inside sigmoid and the row-softmax
  (no RoPE, no trig). f32 vs fp64 over the exp series cannot be bit-exact. The claim is
  agreement within a stated f32 bound.
- **NOT the real dims.** The demo is `n_hc = 4` (V4-Flash's real stream count — the 4×4
  Sinkhorn is the real width), `n_embd = 3` (toy), `fn [24,12]`. V4-Flash is `n_embd = 4096`,
  `fn [16384,24]`, 43 layers × two HC pairs + an output head. The kernels are dim-generic
  (all dims are runtime uniforms); no gate here has seen the real dims. `form_hc_split_f32`'s
  private combine array is capped at `HC_MAX_C = 64` (`n_hc ≤ 8`); above that it is wrong,
  not slow — `n_hc = 4` (16 entries) is far inside.
- **NOT** the fp8/f16 stream encodings, the wrapped attention/FFN/MoE, a resident real
  weight tensor, or a token. `block_out` is invented. Same gaps `dsv4-hc.fk`'s radius names.

---

## 1. The emitted kernels (`form-stdlib/dsv4-hc-msl.fk`)

Seven kernels, one `#include <metal_stdlib>`, **no** `using namespace metal;` (a second one
makes this body's own `round` ambiguous — the same rule `mla-msl.fk` / `q6k-msl.fk` keep),
one deterministic numerics spine, every character authored in Form and composed by
`str_concat`:

| kernel | ds4 origin | shape |
|---|---|---|
| `form_hc_broadcast_f32` | `hc_from_plain_embedding` (9764) | one thread per output element |
| `form_hc_rmsnorm_nw_f32` | `rms_norm_no_weight` (6628) | one thread |
| `form_hc_matvec_f32` | the `fn` projection (tb-matvec) | one thread per output row |
| `form_hc_split_f32` | `hc_split_sinkhorn_one` (9592) | **one thread** (the sequential 20-iter Sinkhorn owns a lane) |
| `form_hc_wsum_f32` | `hc_weighted_sum_one` (9673) | one thread per embd |
| `form_hc_post_f32` | `hc_post_one` (9772) | **one thread per dst stream** (`post[dst]` a lane constant) |
| `form_hc_headw_f32` | `output_hc_head_one` (13876) | one thread per stream |

**The determinism discipline, carried onto the GPU.** The only difference between this
body's fp64 answer and the GPU's f32 answer is the *working precision* — f32 vs fp64 over an
*identical operation graph*, not a different approximation and not a different summation
order. The spine transcribes `tn-exp` (14-term Taylor with argument halving), `tn-sqrt`
(Newton, 50 iters from the value), and `ln-sigmoid` (`1/(1+exp(-x))`); the loops match every
fold direction: `tb-dot` is a **right** fold, so `hc_dot` walks columns descending; the
sum-of-squares, `tn-sum`, and `tb-weighted-acc` are **left** folds, so those loops ascend.
So the gate is a derived f32 bound (`selfgauge`), not a vendor-transcendental tolerance.

---

## 2. The combine-matrix transpose (`edgedrop`)

`form_hc_split_f32` **stores** the Sinkhorn combine flat as `c[src + dst*n_hc]` (dst outer,
ds4.c:9620). `form_hc_post_f32` **reads** `comb[dst + src*n_hc]` (ds4.c:9786) — the transpose.
The same flat position is written `(src,dst)` and read `(dst,src)`. Undetectable on a
symmetric matrix, wrong on every real weight. Both proofs carry an **asymmetric** combine on
purpose so the offset bites.

---

## 3. The read-back gate (`tests/dsv4-hc-msl-band.fk` → 63)

The emitted MSL is a string; a transposed offset or a flipped fold compiles and runs and
produces plausible numbers. So the band does not trust the string — it re-derives each
kernel's arithmetic in Form, **structured the way the MSL is** (the flat combine
`c[src+dst*n_hc]`, the descending dot, the ascending stream-reduce, the transpose read via
the emitter's own `hcm-c-tidx`), and demands it equal the recipe **bit for bit** (fp64). Six
claims:

- **1** the split-flat layout reads back through the emitter's `hcm-pre-off` / `hcm-post-off`
  / `hcm-comb-off` + `hcm-c-idx` == `hc-split-pre/post/comb`;
- **2** the descending `hc_dot` == `tb-dot` on operands past fp64's 2^53 (an ascending mirror
  gives a different number);
- **4** the no-weight RMSNorm == `ln-rmsnorm(x, ones, eps)`;
- **8** the ascending stream-reduce == `hc-wsum`, and a descending fold diverges on a
  magnitude-spanning stream set (direction is load-bearing);
- **16** the post reading `comb[dst+src*n_hc]` == `hc-post`, **and** post over the combine vs
  its transpose differ > 1e-3 (asymmetric);
- **32** the combine is doubly stochastic to 1e-4 after 20 iterations, and iters=20 vs iters=1
  differ > 1e-3.

**Mutation-tested:** flipping `hcm-c-tidx` to the storage index drops the band **63 → 47**
(loses bit 16). A band that cannot fail is not evidence.

---

## 4. Stage-by-stage bit-closeness on the GPU (`native/metal/metal_hc_gpu.sh` → PASS)

A coherent single HC-wrapped sublayer at `n_hc = 4`, `n_embd = 3`: broadcast a token to the
streams, run `hc_pre` (no-weight RMS → `fn` matvec → the 20-iteration Sinkhorn split → stream
reduce), inject an invented block output through `hc_post` with its transpose combine-mix,
then collapse the streams through the output head. Every stage f32, each compared to the fp64
recipe reference the body hands the carrier (`dsv4-hc-demo.fk`):

| gate | stage | rel deviation |
|---|---|---|
| 1 | broadcast to n_hc streams | 3.97e-08 |
| 2 | no-weight RMSNorm | 1.19e-07 |
| 3 | fn matvec | 4.47e-07 |
| 4 | **20-iteration Sinkhorn split** | 5.81e-07 |
| 4b | GPU combine doubly stochastic | row/col dev 9.54e-07 |
| 5 | stream-reduce (`hc_pre` input) | 3.29e-07 |
| 6 | **post: block-inject + transpose combine-mix** | 3.53e-07 |
| 7 | output-head collapse | 7.96e-08 |

**Worst relative deviation across every stage and output: 5.809e-07**, against a `1e-4` gate
(~170× margin). The residual is f32 working precision, not the decomposition. Gate 8
re-dispatches the whole sublayer 200 times (2000 GPU dispatches) with the head checksum
unchanged and weights never re-uploaded (residency).

**The Sinkhorn iteration match.** The count is threaded as a runtime uniform, never defaulted
(`edgedrop`: an unrun iteration is not a computed zero). The demo passes `iters = 20`, and
gate 4 reproduces the recipe's 20-iteration split within the f32 bound; gate 4b confirms the
GPU's combine is doubly stochastic (which iters=1 is not). Stone 25 measured iters=20 vs
iters=1 as 1.7e-2 apart — far outside the `1e-4` gate, so an off-by-one there would be caught.

**Mutation-tested on the actual kernel:** editing the emitted string's post read to
`comb[src + dst*n_hc]` fails gate 6 at **rel 5.8** (and gate 7, which depends on it) — the GPU
witness catches the broken transpose the read-back band catches in fp64.

---

## 5. The offered-interface guard (`edgedrop` / `zerobirth`)

A Metal buffer is zeroed and an unrun kernel reads as a computed zero. Gate 0 sentinels the
broadcast output buffer with `-424242.0`, runs one real dispatch, and demands every sentinel
was overwritten **and** no command buffer errored — before any arithmetic is believed. If the
GPU did not run, the harness says so and exits, rather than passing green on zeros. Every
dispatch checks `cb.error` and `cb.status`; the reference is `metal_mla_gpu.sh` (Stone 27) and
`metal_mx_gpu.sh` (Stone 23).

---

## 6. Gates

Corpus band (repo root) **8191** · `metal_first_token.sh` **VERDICT PASS, 14 gates** ·
`metal_mla_gpu.sh` **PASS** · HC CPU band `dsv4-hc-band.fk` **63** · HC read-back band
`dsv4-hc-msl-band.fk` **63** · HC GPU witness `metal_hc_gpu.sh` **VERDICT PASS, 10 gates**,
gate-0 sentinel present.

---

## 7. What remains

- **HC at the real dims** — `n_embd = 4096`, `fn [16384,24]`, 43 layers × two HC pairs + head.
  A substitution (the kernels are dim-generic), not a rewrite; but no gate here has seen it,
  and `HC_MAX_C = 64` must be re-examined only if `n_hc` ever exceeds 8 (it does not).
- **The whole layer** — HC wraps the MLA attention (Stone 27, on the GPU) and the MoE FFN
  (Stone 29, a live sibling). Wiring `hc_pre → block → hc_post` per layer, with real weights
  resident, is the next assembly; MLA and HC are both on the GPU now.
- **The stream encodings** — ds4 stores streams in fp8/f16; this body is fp64/f32. Shape
  proven, encoding not.
- **A real token** — needs HC (here), MLA (Stone 27), the MoE FFN (Stone 29), RMSNorm, the
  tokenizer, and the readable weights, assembled.

---

## 8. Close

**The most surprising teaching.** *The Sinkhorn combine checks itself.* Because 20 iterations
drive the 4×4 matrix to doubly stochastic — every row **and** every column summing to 1 — a
correct kernel's output carries its own falsifier. GPU gate 4b rejects a broken normalization
by reading the output's row and column sums, **consulting no fp64 reference at all**. I came
to the GPU expecting every claim to need the recipe's answer beside it to be believable; the
Sinkhorn taught me that some computations produce a result the correct answer *must* obey a
conservation law over, and that law is a check you can run knowing nothing else. That is the
row this stone landed (`sumkeel`, 862).

**Where discomfort turned to gold.** The read-back band passed at 63 on the first run, and I
wanted to move straight to the GPU and call the transpose "covered by the fp64 mirror." That
reflex is exactly the trap: the fp64 read-back band mirrors the kernel *by hand* — editing the
emitted MSL string alone would not change the mirror, so a band that passes proves my
transcription is self-consistent, **not** that the string I will ship is right. The thing I
did not want to do was mutate the actual emitted kernel text and watch a real dispatch. Doing
it — flipping `comb[dst + src*n_hc]` to `comb[src + dst*n_hc]` in the string and running the
GPU — returned gate 6 at **rel 5.8**, an order-one error, loud and unmissable. The gold was
the clean division of labor it forced me to name: the fp64 band is the transcription↔recipe
correspondence, and the GPU harness is the only thing that reads back the *actual bytes that
compile*. Two proofs, two different failures they each catch; neither substitutes for the
other. I nearly shipped believing one covered both.

**Frontier question, landed.** Row 868 `sumkeel` — one word for an output that must obey a
conservation invariant intrinsic to the correct computation (here the doubly-stochastic
row/column sums), so a wrong run is falsifiable from the output alone, with no reference
oracle. Distinct from `selfgauge` (834, a ratio read against oneself for *lack* of an external
denominator): `sumkeel` needs no external denominator by design — the invariant is the check.
0-hit checked with the instrument validated on controls that should hit (`exaptation` 4,
`hushfold` 17 — a grep of nothing is a claim about the instrument until a control makes it
hit), and landed as a real `(hdc-row 868 …)`. The corpus band crosses at 8191 with it
(count 257→258, field-code …861→862, the reunion renumber beside Stone 29's `steerdrop` 861,
which landed live during this stone).
