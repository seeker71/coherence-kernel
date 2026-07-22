# The join — one complete DeepSeek-V4-Flash layer, Stone 37

**2026-07-22, Apple M4 Max, `maxBufferLength = 86 586 540 032`, page 16 384.
File: `ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf`, 91 321 404 640 B, read-only.
Harness: `form/native/metal/metal_dsv4_layer_join.sh` → `VERDICT PASS 31 gates`.
Oracle: `form/form-stdlib/tests/dsv4-mla-core-oracle.py` in `DSV4_ORACLE_MODE=layer`.**

Two halves had been standing for two stones and had never been joined. Stone 36 proved the attention
half — `hc_pre → MLA → hc_post` on the real layer-0 activations, 30 gates. Stone 34 proved the FFN
half's pieces — the hash table read, the F16 router matvec, MXFP4 gate/up, the fused IQ2_XXS down, 8
gates. Neither had ever met the other, because what stands between them is a *second hyper-connection
frame*, and that frame had never been built at real dims.

It is built. One complete layer now runs, on the file's own `blk.0` weights, at two positions:

```
hc_pre(attn) → MLA(13 dispatches) → hc_post(attn)
             → hc_pre(ffn) → ffn_norm → 6-of-256 routed MoE + shared expert → hc_post(ffn) → out_hc
```

`out_hc` is 16 384 numbers: the four hyper-connection streams `blk.1` receives.

---

## What the join actually is

HC is this model's residual stream. There is no plain residual anywhere in it — the whole forward runs
in `n_hc = 4` parallel streams, and every block is wrapped in a frame that collapses them on the way in
and recombines them on the way out. So "a complete layer" is not *attention block + FFN block*. It is
two blocks inside **two independent frames**:

| | attention frame | FFN frame |
|---|---|---|
| mix projection | `blk.0.hc_attn_fn` F16 [16384 → 24] | `blk.0.hc_ffn_fn` F16 [16384 → 24] |
| gates | `hc_attn_scale` [3], `hc_attn_base` [24] | `hc_ffn_scale` [3], `hc_ffn_base` [24] |
| sinkhorn | 20 iterations, its own 4×4 combine | 20 iterations, its own 4×4 combine |
| residual it closes over | the broadcast embedding | `after_attn_hc` |

The two frames share nothing but the state they hand each other. Reusing the attention frame's
`post`/`comb` for the FFN's `hc_post` would produce numbers of exactly the right shape, exactly the
right magnitude, and no error anywhere — a silent wrong answer. Nothing in the file says which frame
belongs where beyond the tensor names. That is why the rented oracle is a gate here and not a comfort.

---

## Evidence class, per surface (`twinblind`, corpus row 868)

The two classes are named because they are not the same kind of proof.

**CHOOSING** — proven against the rented fp64 ds4.c transcription. A self-carve inherits each of these
on both sides and is blind to all of them:

- both HC frames (`hc_pre` collapse, the sinkhorn split, `hc_post`'s `[dst, src]` combine addressing)
- `ffn_norm`'s placement — after the FFN's own collapse, not the attention's
- the gating function `probs[i] = sqrt(softplus(logit[i]))` — expert_gating_func 4, and **not a
  softmax**: it is never normalised over the 256, only over the six that were selected
- the hash selection on the **token id** (`forepick`, row 867)
- the floored-sum weight normalisation, floor `6.103515625e-5` = 2⁻¹⁴, the smallest normal f16, and the
  `expert_weights_scale` 1.5
- the SwiGLU clamp's **asymmetry**: gate clamped above only, up clamped both ways, at 10.0
- the router weight multiplying the **mid**, before the down projection, not after
- the shared expert running for every token, unrouted, weight 1, and simply added

**CANONICAL** — one right answer, so a self-carve is a real falsifier. The MXFP4 (type 40), IQ2_XXS
(type 16), MXFP8 (type 41) and F16 decodes and their matvecs. Stones 33/34/35 self-carved each at real
dims — GPU through the view vs an independent CPU decode of the same bytes — and those harnesses still
gate them. Here the oracle's own independent decode re-witnesses them, which is strictly stronger than
a second copy of the same code.

### Agreement, worst case over the whole layer

| surface | maxAbs | maxRel (above \|1e-2\|) |
|---|---|---|
| `after_attn_hc` (attention half, 16 384) | 2.0e-7 | 8.3e-6 |
| `ffn_cur` — **the join** (4 096) | 4.3e-7 | 2.0e-5 |
| `ffn_normed` (4 096) | 1.1e-6 | 5.9e-5 |
| `router_logits` (256) | 4.9e-6 | 1.3e-4 |
| expert 147 MXFP4 gate / up (2 048) | 2.4e-6 | 8.8e-5 |
| expert 147 IQ2_XXS down (4 096) | 9.4e-8 | 7.3e-6 |
| all six routed experts accumulated (4 096) | 8.1e-7 | 5.7e-5 |
| shared expert (4 096) | 1.7e-6 | 8.7e-5 |
| **`out_hc` — the complete layer (16 384)** | **5.0e-6** | **1.3e-4** |

`assocwall` (row 866): the bound is absolute over every element plus a relative bound taken only above a
magnitude floor. Below the floor a ~1e-6 absolute difference reads as a huge relative purely because the
denominator is ~0.

`forepick` carried through to a layer output: token 671 routes to experts **[147, 78, 30, 248, 217,
179]**, GPU table read bit-identical to the oracle's, at both positions. The router did not choose them;
`ffn_gate_tid2eid` did.

`hushfold` (row 859) at layer scale: the two positions' complete layer outputs differ in **14 600 /
16 384** entries (max delta 8.1e-5) while each agrees with its own oracle. RoPE is the identity at
position 0, so one position would have witnessed nothing.

`onelean`/`lapspan`: `device.currentAllocatedSize = 92 465 594 368 B (86.12 GiB)` — two overlapping
page-aligned `bytesNoCopy` views over the mmapped file. One buffer over the whole file cannot be made;
two can.

---

## The radius (`aporon`), and the recipe gap said out loud

**ds4.c cannot execute this file's FFN.** Not "will not" — cannot:

- `matvec_experts_mid_prequant` (:9349) → `ds4_die("unsupported gate/up expert tensor type")` for the
  type-40 gate/up this file carries;
- `layer_shared_ffn_one` (:10460) → `ds4_die("shared expert gate/up tensors do not share a Q8_0 input
  layout")`, and this file's `ffn_*_shexp` are type 41.

So what was rented is the **order of operations and every scalar choice**, which are type-independent
and present in the C. What was *not* rented is the arithmetic for these types, because the reference
refuses them. The oracle re-derives the type-40/41/16 decodes itself and feeds each expert matvec the
**exact fp64 activation** — which is ds4.c's own `ds4_vec_dot_iq2_xxs_f32` (:3779) control flow, not its
`Q8_K`-prequantised dispatcher. That deviation is written into the oracle's header and the harness's,
not buried.

The compressor and indexer are absent from this path entirely, and that is grounded rather than
assumed: `forward_first_token_cpu` (:13849) calls `layer_forward_self_one` for every layer, and
`layer_forward_self_one` (:13793) never touches a compressor or an indexer.

**Not reached, and not fabricated:** the 43-layer stack, the close, and the token. There is no token id
in this receipt because there is no token. What blocks it is mapped precisely below.

---

## What stacking to 43 needs — read from the file, layer by layer

The stack was not attempted with a guess. The file's own tensor table was walked for all 43 layers, and
it says the layers are **not uniform**, in four ways that a stack built on `blk.0` would get silently
wrong:

1. **The expert count is not the metadata's.** `deepseek4.expert_count` = 256, and it is true only for
   layers 0, 1, 2. Layers 3–42 carry `ffn_gate_exps` with `dim[2] = 192` — REAP-25 pruning. The router
   still projects **256** logits at every layer (`ffn_gate_inp` is [4096, 256] throughout). A top-k over
   256 logits against a 192-expert stack selects an expert that does not exist. The per-layer expert
   count must come from the tensor's own `dim[2]`, never from the KV.

2. **The expert types change per layer**, and independently for gate/up vs down:

   | gate/up | down | n_exp | routing | layers |
   |---|---|---|---|---|
   | 40 | 16 | 256 | hash | 0, 1 |
   | 40 | 40 | 256 | hash | 2 |
   | 40 | 40 | 192 | top-k + bias | 3, 4, 7, 8 |
   | 40 | 16 | 192 | top-k + bias | 5, 6, 10, 11, 12 |
   | 16 | 16 | 192 | top-k + bias | 9, 13–25, 27, 28, 30, 32, 34–38, 41 |
   | 16 | 40 | 192 | top-k + bias | 26, 29, 31, 33, 39, 40, 42 |

3. **Routing changes at layer 3.** Layers 0–2 carry `ffn_gate_tid2eid` and route by table
   (`hash_layer_count` = 3). Layers 3+ have no table and carry `exp_probs_b.bias` [256] — biased top-k
   selection, unbiased weighting. `form_dsv4_router_f32` is already offered and already does exactly
   this; it has never run at real dims.

4. **RoPE changes at layer 2.** `compress_ratios` = [0, 0, 4, 128, 4, 128, …]: layers 0–1 uncompressed
   (base 10 000, scale 1), layers 2+ compressed (base 160 000, scale 1/16, YaRN ramp, `n_ctx_orig`
   65 536). **One good thing fell out of reading it:** the compressed RoPE needs no new kernel. In
   `rope_tail_ext_inplace` (:10102) the YaRN magnitude scale cancels exactly — `attn_factor` is set to
   `1/(1 + 0.1·ln(1/freq_scale))` at :10175 precisely so `mscale` comes back to 1 — and the angle
   reduces to

   > `theta = theta_extrap · (freq_scale·(1 − ramp_k) + ramp_k)`

   which is a **per-pair scale of the same `theta_extrap`**. `form_mla_rope_f32` already takes a
   per-pair `freqs[]` buffer and computes `pos * freqs[k]`. So a compressed layer is the existing kernel
   with a different `freqs[]` — computed once per layer, on the host, from the file's own metadata.

Working buffers are not the obstacle. Every per-layer temporary is small (`out_hc` 64 KiB, the
2 048-wide mid 8 KiB, the 4×4 split 96 B) and the model is mmapped and wrapped, never copied — the
86.12 GiB `currentAllocatedSize` is the two views over the file and does not grow with layer count.
What stacking costs is **correctness surface**, not memory: four per-layer dispatch decisions that must
be read from the file, and an oracle run that must carry the state through all 43.

---

## The most surprising teaching

**The reference engine cannot run the model it is the reference for.** ds4.c is *DeepSeek V4's own C* —
it knows `hash_layer_count`, the sink softmax, the hyper-connection sinkhorn, the fp8+f16 KV round-trip.
And handed this file's FFN it dies twice: on the type-40 experts and on the type-41 shared expert. The
authority that is complete about *what to do in what order* is mute about *how to do it* for the very
bytes this file ships.

That is not a defect in the rental — it is the shape of the rental, and it changes what a rented oracle
can be asked for. Row 868 (`twinblind`) said a self-carve is blind to a choice its two sides share, so
the choosing half must be read from an independent description. Row 869 (`mutestep`) said the artifact
itself cannot always tell you a choice is being made. This stone found the third face: the independent
description can be authoritative about the recipe's **shape** and refuse the recipe's **arithmetic** —
and if you do not say which of the two you rented, a green harness will read as a full proof when half
of it was self-carve all along.

## Where discomfort turned to gold

The moment I wanted to look away was `ds4_die("shared expert gate/up tensors do not share a Q8_0 input
layout")`. The easy move was obvious and available: write the FFN oracle anyway, call it "the ds4.c
recipe", and let every reader assume the whole thing was rented. The harness would have gone green
exactly as it did.

Not looking away meant grepping every `shexp` and every expert dispatcher until I could name the two
exact lines that refuse and the one line (`:3779`) that does not — ds4.c *does* carry an exact-activation
IQ2_XXS dot, and that is what the oracle transcribes. The gold is the evidence-class split in this
receipt: without the refusals I would have had one undifferentiated "proven against ds4.c", and with
them I have a table that says exactly which surfaces are rented and which are self-carved, and a name
for the difference.

The second, smaller one: the harness's own liveness guard failed the layer. It demanded more than eight
distinct values from the six routed expert weights — a vector that has six entries and cannot ever
satisfy it. Everything else went green on the first run; the only red was the guard accusing an honest
answer. A guard that accuses the honest is as much a defect as one that passes a dead read. The floor
now scales with the vector, capped at eight, and still refuses a sentinel, a memset or one repeated
value.

## The frontier question, landed

> *What one word names a rented reference that is authoritative about a recipe's order of operations but
> cannot execute the artifact it describes, so only the shape is borrowed and the arithmetic must be
> re-derived?*

**`halfrent`** — 0 hits across `learn/`, `receipts/`, `docs/`, `teachings/`, `form/` before this row.
Instrument validated on the same command: `mutestep` 3, `twinblind` 9 — a grep of nothing is a claim
about the instrument until a control makes it hit.

Landed as `(hdc-row 870 …)`. Corpus band from the repo root: **8191**.

---

## Gates

| gate | result |
|---|---|
| corpus band from repo root | 8191 |
| `metal_dsv4_layer.sh` (Stone 36) | PASS 30 — re-run **after** the oracle grew its `layer` mode, to prove the `hc` mode it depends on was not disturbed |
| `metal_dsv4_layer_join.sh` (new) | **PASS 31** |
| `metal_first_token.sh` | untouched, not re-run this session |
| `metal_dsv4_forward.sh` / `metal_dsv4_token.sh` / `metal_hc_gpu.sh` | untouched, not re-run this session |

## Files

- `form/native/metal/dsv4-layer-real.fk` — the band: the FFN frame's two new kernels
  (`form_dsv4_hash_select`, `form_dsv4_hash_weights`) and the FFN translation unit.
- `form/native/metal/metal_dsv4_layer_join.sh` — the harness, 31 gates.
- `form/form-stdlib/tests/dsv4-mla-core-oracle.py` — `DSV4_ORACLE_MODE=layer`: the FFN half in fp64,
  with the MXFP4 and IQ2_XXS decoders and the IQ2 tables transcribed from ds4.c:873/:884.
- `learn/homecoming-distillation-corpus.fk` — row 870, `halfrent`.
