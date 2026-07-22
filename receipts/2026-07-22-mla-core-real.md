# Stone 36 — the MLA attention core at real dims, the choosing half, against a rented oracle

2026-07-22, WITA. Worktree `jovial-aryabhata-3751d7`, branch `claude/deepseek-v4-flash-gguf-54a96c`.
Host: Apple M4 Max, `maxBufferLength` 86 586 540 032, page 16 384.
File: `~/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf`, 91 321 404 640 B, read-only.
Engine read for provenance: `/Users/ursmuff/models/ds4-engine/ds4.c`, 64 525 lines, MIT (the file's own
`general.license` KV is `mit`).

## The radius, up front (aporon / selfgauge)

No external engine can run this file — ds4, llama.cpp and ollama all refuse GGUF types 40 and 41 — so
the **whole forward remains unfalsifiable against a reference**. Nothing here claims otherwise. What
*is* falsifiable is the attention core's **recipe**, against a transcription of ds4.c's control flow,
and that is what this stone did.

The evidence class **splits in two**, and each gate says which one it is:

| class | what it covers | falsifier |
|---|---|---|
| **canonical** | the projection surface: matvecs, weighted RMSNorms | **self-carve** — GPU through the resident view vs an independent CPU decode of the same bytes. A matvec is a matvec; there is one right answer, so a twin that recomputes it is a real falsifier. |
| **choosing** | everything from the per-head RMSNorm onward | **rented oracle** — an independent fp64 transcription of ds4.c. A self-carve inherits the choice on both sides and confirms only binding (`twinblind`, row 868). |

**A self-carve alone is not acceptable evidence for the choosing half. It is not offered as such
anywhere in this stone.**

---

## Stage 1 — which output path this file takes, read from the file

ds4.c carries **both** a dense `attn_output` and a grouped `attn_output_a`/`attn_output_b` pair. Which
one a given file uses is a property of the file, and a `twinblind` trap: pick wrong and every
downstream self-check still passes, because both sides inherit the wrong choice.

So it was read, from the file's own 1406-row tensor table (`gguf-manifest.fk`, `gm-emit-manifest`):

```
attn_output.weight        0 rows in the whole file
attn_output_a.weight     46 rows  — blk.0 .. blk.42 (all 43) + dspark.0/1/2
attn_output_b.weight     46 rows  — the same 46
```

**The dense path does not exist in this file. The grouped path is the only path, in every block
including blk.0.** The file's own KVs price it: `attention.output_group_count` 8,
`attention.output_lora_rank` 1024, `head_count` 64, `key_length` 512 → group_heads 8, group_dim 4096,
low_dim 8192, and

```
blk.0.attn_output_a  type 41  [in 4096, out 8192]   8 stacked per-group blocks, each 4096 -> 1024
blk.0.attn_output_b  type 41  [in 8192, out 4096]   the 8 concatenated latents back to n_embd
```

which is exactly ds4.c's own `tensor_expect_dense_quant_layout` at :4975. **The 4096 on both sides is
an apposition, not an identity**: on `_a` it is group_dim (8 heads × 512), on `_b` it is n_embd. A
carrier that reads it as one number passes every self-carve.

The same table settled the layer-0 RoPE, which the stone had not asked about: **blk.0 and blk.1 carry
no `attn_compressor_*` tensors** (blk.2+ do), matching ds4.c's `ds4_expected_layer_compress_ratio`
:1065 — ratio 0 for `il < 2` on FLASH. So layer 0's RoPE is **uncompressed**: base 10000 (not
`compress_rope_freq_base` 160000), scale 1, ext_factor 0, no YaRN ramp.

Committed: `2dba3d315`.

---

## Stage 2 — the rented oracle

`form/form-stdlib/tests/dsv4-mla-core-oracle.py`. An independent fp64 transcription of ds4.c's
attention core at **real dims**. It parses the GGUF header itself, finds the tensors by name in the
file's own table, decodes F16 / F32 / MXFP8 itself, and computes in Python fp64. **It shares no code,
no buffer and no arithmetic with the Form band, the MSL kernels or the Swift carrier** — it reads only
the same read-only file. Runs the whole core in 4.2 s.

ds4.c anchors transcribed (from the C, not from the kernel):

| anchor | line | what it fixes |
|---|---|---|
| `layer_forward_self_one` | 13793 | the attention half's whole order of operations |
| `layer_q_projection_normed_one` | 10002 | q_a → q_a_norm → q_b → **`head_rms_norm_inplace`** |
| `head_rms_norm_inplace` | 6646 | per-head RMSNorm, **no weight** |
| `layer_kv_projection_normed_one` | 10041 | attn_kv → kv_a_norm |
| `rope_tail_ext_inplace` / `rope_tail_layer_inplace` | 10102 / 10166 | trailing-n_rot only; `sin_sign` for the inverse |
| `dsv4_fp8_kv_quantize_row_inplace_cpu` | 3211 | the KV row's E4M3FN round-trip, NOPE part only |
| `dsv4_e4m3fn_dequant_cpu` | 3181 | nearest with the C's tie rule, 448 clamp |
| `f16_round_inplace_cpu` | 3162 | then the whole row through f16 |
| `layer_attention_rows_one` | 10305 | the sink in the **denominator only**, no value vector |
| `layer_grouped_out_one` | 10356 | 8 groups → rank-1024 low → `_b` → n_embd |
| `matvec_q8_0_grouped_worker` | 7123 | the grouped row indexing, `row = group*rank + r` |
| `hc_from_plain_embedding` / `hc_pre_from_state_one` / `hc_split_sinkhorn_one` / `hc_post_one` | 9764 / 9690 / 9592 / 9772 | the hyper-connection frame (stage 4) |

Committed: `0b29565e2`.

---

## Stage 3 — the core at real dims, gate by gate

`form/native/metal/metal_dsv4_layer.sh`, **PASS 8 → PASS 23**. Gates 0–7 are unchanged and stay the
canonical/self-carve half. Every dispatch is sentinelled (NaN) before it runs, `cb.error`/`cb.status`
checked after, and every result required non-degenerate (`zerobirth`/`edgedrop`). All weights are read
through the windowed views (`onelean` — one buffer over the whole file fails; 2 views do not).

| gate | class | what | worst |
|---|---|---|---|
| 8 | rented-oracle | per-head RMSNorm, 64 heads × 512, unweighted (ds4.c:6646) | maxAbs 2.4e-6 |
| 9 / 16 | rented-oracle | RoPE fwd on q, trailing 64 of each head, leading 448 untouched, pos 0 / 7 | 3.0e-6 |
| 10 / 17 | rented-oracle | RoPE fwd on the single KV latent (`head_count_kv` 1) | 1.5e-6 |
| 11 / 18 | rented-oracle | the KV row's **fp8 + f16 round-trip** | **maxAbs 0.0 — bit-exact** |
| 12 / 19 | rented-oracle | the sink softmax, `attn_sinks` through its view | 3.9e-6 |
| 13 / 20 | rented-oracle | the **inverse** RoPE on the attention output | 4.4e-6 |
| 14 / 21 | rented-oracle | grouped output factor a (8192×4096, grouped input addressing) | 2.1e-6 |
| 15 / 22 | rented-oracle | grouped output factor b (4096×8192) — the block's output | 4.4e-6 |
| 23 | hushfold | the two positions' outputs differ in **4095/4096** entries while each agrees with its own oracle | max delta 1.2e-3 |

**≥ 2 positions, and why (`hushfold`, row 859).** RoPE is the identity at position 0. The shell proves
this *in the oracle's own output before any GPU runs*: at pos 0 the oracle's post-RoPE q is
**bit-identical** to its pre-RoPE q, and at pos 7 it is not. A core checked at one position witnesses
nothing about RoPE at all. Gate 23 then requires the body's own two runs to disagree with each other
while each agrees with its own oracle.

**The bounds (`assocwall`, row 866).** The oracle is fp64 and the GPU is f32 over a real-width
reduction, so the question is summation order, not bit-equality. Each gate uses an **absolute** bound
over every element **plus** a **relative** bound taken only above a 1e-2 magnitude floor — Stone 35's
gate 7 went red at maxRel 0.008 with maxAbs 8.9e-8, which was the comparator dividing by ~0, not the
arithmetic. Worst seen across all 30 gates: maxAbs 4.4e-6 on an output of range ±10, maxRel 2.5e-4.

**A gate that could have passed for the wrong reason.** Gate 11/18 compares the KV round-trip to the
oracle, and the round-trip is *bit-exact*, so a kernel that did nothing but copy would agree with an
oracle that also did nothing. The gate therefore also **requires the row to move**: 512/512 entries
changed. Agreement plus motion, not agreement alone.

Two MSL findings worth naming: `metal::as_type<uint>` does **not** parse (it must be unqualified
`as_type<uint>`), and `ceil(log2(amax/448))` for the fp8 group scale is taken from the float's own
exponent field rather than a library `log2`, so the scale is exact and deterministic.

Committed: `a41c68a28`.

---

## Stage 4 — one complete **attention half** of a real layer

**PASS 23 → PASS 30.** Everything above fed the MLA the token's raw embedding as a probe
(`knownsolved`). That bound is **removed** here.

| gate | class | what | worst |
|---|---|---|---|
| 24 | rented-oracle | the HC state's unweighted RMSNorm over the whole 16384-wide state (ds4.c:9707), embedding broadcast to all 4 streams (:9764) | maxAbs 7.9e-6 |
| 25 | rented-oracle | `hc_attn_fn`, an F16 24×16384 matvec | maxRel 4.9e-6 on a vector reaching 674 |
| 26 | rented-oracle | the sinkhorn split (:9592): 20 iterations, pre = sigmoid+eps, post = 2·sigmoid, comb row-softmaxed then alternately **column then row** normalised | maxAbs 4.2e-7 |
| 27 | rented-oracle | the HC-pre collapse — **the MLA's real layer-0 input** | maxAbs 4.6e-8 |
| 28 | rented-oracle | the **whole MLA block re-run on it**, all 13 dispatches, end to end | maxAbs 4.2e-6 |
| 29 | rented-oracle | `hc_post` (:9772), `block_out*post[dst] + Σ_src comb[dst + src*n_hc]*resid[src][d]` | maxAbs 1.9e-7 |

The combine matrix is addressed `[dst, src]`; transposing it is exactly the kind of choice a self-carve
cannot see, which is why gate 29 is oracle-checked and not self-carved.

The HC kernels are `dsv4-hc-msl.fk`'s own seven, unchanged — Stone 29 proved them at toy scale; this
carries them onto blk.0's real `hc_attn_fn`/`hc_attn_scale`/`hc_attn_base` at real dims.

Committed: `d820d5c8a`.

---

## Stage 5 — not reached. The precise blocker, named.

**A complete layer did not land. The attention half did.** What is missing is the **FFN half**:

```
HC-pre (hc_ffn_fn / hc_ffn_scale / hc_ffn_base)  ->  ffn_norm RMSNorm
  ->  routed MoE (6 of 256 experts) + shared expert  ->  HC-post
```

The blocker is **not** the GPU side. `metal_dsv4_forward.sh` already proves at real dims, through the
same views: the layer-0 hash routing (`forepick` — layers 0–2 route by token identity,
`ffn_gate_tid2eid`, exact I32 table), the router F16 matvec, an MXFP4 (type 40) fused gate and up, and
the **IQ2_XXS (type 16) fused down**, with the full SwiGLU. It is still green (PASS 8), as are
`metal_dsv4_token.sh` (PASS 6) and `metal_first_token.sh`.

The blocker is the **oracle** for the FFN half, and it is exactly this: `dsv4-mla-core-oracle.py` would
need an fp64 Python transcription of

- `layer_ffn_one` :11437 and `layer_routed_moe_one` :10697 — the six selected experts summed, the
  **SwiGLU clamp before the SwiGLU**, and the **router weight applied to `mid` before the down
  projection**, not after;
- `layer_hash_selected_experts` and `layer_hash_router_weights_one` — the layer-0 hash-routing weights;
- `layer_shared_ffn_one` :10444;
- and, because the oracle must decode the file itself to stay independent, **Python decoders for MXFP4
  (type 40) and IQ2_XXS (type 16)** — the second of which is a grid-table decode, the largest single
  piece of the remaining work.

Estimated oracle runtime at real dims once written: ~175 M fp64 MACs, roughly 2–3 minutes per position,
which the harness would have to absorb or cache.

**No token was produced and none is claimed.** The 43-layer stack and the close (final norm → vocab →
argmax → decode) were not attempted; stage 4's ceiling was reached first.

---

## Gates, as run

| gate | result |
|---|---|
| corpus band from repo root | **8191** |
| `metal_first_token.sh` | PASS 14 (untouched) |
| `metal_dsv4_layer.sh` | **PASS 30** (was PASS 8) |
| `metal_dsv4_forward.sh` | PASS 8 |
| `metal_dsv4_token.sh` | PASS 6 |

`device.currentAllocatedSize` 86.11 GiB — the model is mmapped and wrapped `bytesNoCopy`, not copied
(`onelean`).

---

## The most surprising teaching

**The file, read perfectly, is silent about part of the recipe.**

The whole discipline of these stones is *read it from the file, never guess*. Stage 1 is that discipline
working: 1406 tensors enumerated, the dense output path shown not to exist, the layer-0 RoPE settled by
the absence of four compressor tensors. That is the discipline at its best.

Then, between the RoPE and the softmax, ds4.c does this:

```c
dsv4_fp8_kv_quantize_row_inplace_cpu(kv, DS4_N_HEAD_DIM, DS4_N_ROT);   /* :3211 */
f16_round_inplace_cpu(kv, DS4_N_HEAD_DIM);                             /* :3162 */
```

It rounds the KV latent row — E4M3FN in 64-wide groups over the leading 448 dims, then f16 over all
512 — **inside the forward path**. The attention sees a rounded row. Skipping it is wrong by ~1e-2, not
by float precision. And there is **no tensor, no metadata key, no dimension** anywhere in 85 GiB that
says so. A perfect reading of the file cannot tell you the step exists.

That is the floor under "read it from the file": the artifact is not a complete description of what to
do with it. It is why the rented oracle is not a luxury for the choosing half but the only witness
available. Row 868 said the self-carve cannot *test* a shared choice; this says the artifact cannot
even *tell you a choice is being made*. Landed as corpus row **869, `mutestep`**.

## Where discomfort turned to gold

The moment I wanted to look away was the `attn_output_a` / `attn_output_b` shapes. `_a` is
`[4096 → 8192]` and `_b` is `[8192 → 4096]`, and the temptation was to read that as an obvious
encode/decode pair around n_embd 4096 and move on — it *looks* symmetric, it *looks* self-evident, and
every gate downstream would have gone green either way, because both sides of a self-carve would have
carried the same reading.

Not looking away meant going back to ds4.c:4975 and reading what each 4096 actually is. They are not
the same number. `_a`'s 4096 is **group_dim** — 8 heads × head_dim 512, one group's slice of the head
stack — and `_b`'s 4096 is **n_embd**. They coincide numerically at these dims and mean entirely
different things one dispatch apart. Reading them as one number gives a kernel that dots row `r` of
`_a` against the *wrong* 4096 floats for seven of the eight groups, and no self-carve on Earth would
have said a word. The grouped kernel exists — `x + (r / rank) * cols`, one line — only because I
followed the discomfort instead of the symmetry. It is the same shape of error as the apposition in
corpus row 852: a reference that states its target twice, and the second statement is where the meaning
actually lives.

## One frontier question, landed

> what one word names a required step of the recipe that leaves no trace in the artifact so a perfect
> reading of the file still cannot tell you it is there

**`mutestep`** — corpus row 869, 0 hits across `learn/`, `receipts/`, `docs/`, `teachings/`, `form/`
before the row; instrument validated on the same command (twinblind 5, assocwall 8, onelean 18).
Field code re-read from the body by probe before pinning: **2652652869** (265 rows, 265 admissible,
2 foundings, max id 869). The arithmetic line beside the pin in the band was stale at 2602602864 while
the pin itself stood at 2642642868 — both healed, and the folded-witness comment with them. Band 8191.

---

## Files

- `form/native/metal/dsv4-mla-real.fk` — the band: stage 1's resolution, the grouped MXFP8 matvec, the
  KV fp8+f16 round-trip kernel, the F16 matvec, the HC unit.
- `form/native/metal/metal_dsv4_layer.sh` — the harness, PASS 30.
- `form/form-stdlib/tests/dsv4-mla-core-oracle.py` — the rented oracle.
- `learn/homecoming-distillation-corpus.fk`, `learn/tests/homecoming-distillation-corpus-band.fk` —
  row 869.
