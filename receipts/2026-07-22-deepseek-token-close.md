# Stone 35 — entering the DeepSeek-V4-Flash MLA attention block at real dims

Wed 2026-07-22, ~14:2x WITA. Worktree `jovial-aryabhata-3751d7`, branch
`claude/deepseek-v4-flash-gguf-54a96c`. Apple M4 Max, 128 GiB. The live 85 GiB file:
`~/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf` (91,321,404,640 B).

## What this stone reached, exactly

**Stage 1 — the WHOLE MLA low-rank projection surface at real dims — PROVEN.**
`form/native/metal/metal_dsv4_layer.sh` → **VERDICT PASS, 8 gates.** Each dispatch runs
the body's own kernel (mla-msl.fk's `form_mla_rmsnorm_f32`, MLA_MAX_HD=512 = the real
head_dim; mxfp8-msl.fk's `form_dsv4_mx8_matvec`) on the file's own blk.0 bytes through the
overlapping windowed views, bound at each tensor's inner offset, and is checked against an
**independent CPU carve** decoding the same bytes at the tensor's absolute mmap offset:

| gate | tensor (type) | shape | maxRel | maxAbs |
|---|---|---|---|---|
| 2 input RMSNorm | attn_norm (F32) | 4096 | 1.2e-7 | 7.5e-9 |
| 3 Q down | attn_q_a (MXFP8/41) | 4096→1024 | 3.2e-5 | 7.5e-8 |
| 4 Q rank-norm | attn_q_a_norm (F32) | 1024 | 3.2e-5 | 3.7e-8 |
| 7 Q up | attn_q_b (MXFP8/41) | 1024→32768 | 2.6e-5 | 8.9e-8 |
| 5 KV down | attn_kv (MXFP8/41) | 4096→512 | 5.1e-5 | 1.8e-7 |
| 6 KV rank-norm | attn_kv_a_norm (F32) | 512 | 5.2e-5 | 1.4e-6 |

Plus gate 0 (the 2 overlapping views wrap the 91 GB file — one buffer FAILs, `onelean`)
and gate 1 (all six tensors resident, holds==1). `device.currentAllocatedSize = 86.11 GiB`
— the model is mmapped and wrapped bytesNoCopy, not copied onto the device.

The chain computed is `q = q_b · q_a_norm(q_a · attn_norm(x))` and
`kv_latent = kv_a_norm(kv · attn_norm(x))` — the entire projection half of MLA, the Q
per-head query stack (64 heads × 512) and the single 512-wide KV latent, at real dims.

Code, committed incrementally (each stage green before the next):
- `b8100a6a2` Stage 1: input norm + Q/KV down + rank-norms (7 gates).
- `761d8ac0e` Stage 1b: Q up-projection, completing the projection surface (8 gates).
- `form/native/metal/dsv4-mla-real.fk` (the emit-band), `form/native/metal/metal_dsv4_layer.sh`.

## The real geometry, read from the file's own header (not assumed)

block_count **43** (confirmed, not the assumed number), n_head 64, head_count_kv 1,
head_dim (key/value_length) 512, q_lora_rank 1024, kv_lora_rank 512, rope.dimension_count
(nrot) 64, rms_eps 1e-6, rope.freq_base 10000, expert_used 6, hyper_connection.count 4,
sinkhorn_iterations 20. The blk.0 attention tensors and their absolute offsets are in
`dsv4-mla-real.fk`'s header table.

## Whether a real token emitted — NO. The precise blocker.

**A first token was not reached.** The honest partial stops exactly where the operation
stops being unambiguous. What remains, named precisely so the next session does not
re-derive it:

1. **The attention core.** The per-head RMSNorm (`form_mla_headrms_f32`), the trailing-64
   RoPE on the Q heads and on the KV latent (`form_mla_rope_f32`, freqs = 10000^(-2k/64)),
   and the single-latent-row sink softmax (`form_mla_attend_f32`, K=V=the one 512-wide
   latent, the learned `attn_sinks[64]` entering the softmax **denominator only**,
   ds4.c:10305). All three kernels are already compiled and ready in `LIB_MLA` (they rode
   along in `dsv4-mla-unit`); they were proven at toy scale in metal_mla_gpu.sh. Wiring
   them at real dims is a dispatch exercise — **but see the evidence caveat below.**

2. **The output projection — a real finding, grounded in ds4.c (MIT), not memory.** This
   file carries THREE output tensors: the dense `attn_output` [n_head·n_value_mla =
   32768 → 4096] (ds4.c:4877) AND the grouped low-rank pair `attn_output_a`
   [n_head_dim·(n_head/N_OUT_GROUP) = 512·8 = **4096** → out_low_dim **8192**] with
   `attn_output_b` [8192 → 4096] (ds4.c:4975). The grouped form applies output_a **per
   output-group** (N_OUT_GROUP = 8, so 8 heads × 512 = 4096 per group) and output_b sums to
   n_embd. Which path layer 0 takes, and N_VALUE_MLA vs head_dim, must be read from config
   before wiring — a mis-pick is a silently-wrong output the falsifier below cannot catch.

3. **HC pre/post, the 43-layer stack, final norm → vocab → argmax → decode.** Untouched.
   HC (n_hc=4, 20-iter Sinkhorn) is proven at toy scale (metal_hc_gpu.sh PASS 10). The
   residual stream IS the HC state carried between layers.

## Evidence class, named exactly (selfgauge / knownsolved / twinblind)

No external oracle can run this file — ds4/llama.cpp/ollama REFUSE GGUF types 40/41, so
every activation is unfalsifiable against a reference (`selfgauge`). Stage 1's gates are the
**mechanism-witness** class of Stones 33/34: fed the token's real EMBEDDING as a PROBE
vector (the true layer-0 input is the HC-pre, not yet wired), they prove the projections
BIND and COMPUTE at real dims through the views — the tensors, dims, byte offsets and decode
ARE real; the numbers are not the real layer-0 activations.

**The frontier this stone found (`twinblind`, corpus row 868).** The projection surface
came home cleanly because a matvec is a matvec and an rmsnorm is an rmsnorm: an unambiguous
op, so an independent CPU carve of the same bytes is a real falsifier — a wrong inner, a
dead view, a mis-decode all make the twins diverge. But the attention **core** is different:
the RoPE split, the sink's place in the denominator, dense-vs-grouped output are structural
**choices**, and a self-carve that encodes the same choice on both sides would AGREE while
staying blind to whether the choice matches ds4. The twin is blind to the premise it shares.
So the internal falsifier is strong exactly where the op is canonical and only an echo
exactly where the recipe chooses — which is why Stage 1 is a strong stone and why the
attention core needs ds4.c grounding (a rented oracle), not a self-carve, to be honest.

## Falsifier triple

Not reached — no token was produced. `snugcause` (second-prompt input-dependence) and the
stable/non-degenerate/not-`<unk>` checks await a real close. Stage 1 IS input-dependent in
the weaker sense that its output is the token's real embedding carried through real weights;
the triple proper is a whole-forward property and is honestly pending.

## Gates

- corpus test band `learn/tests/homecoming-distillation-corpus-band.fk` → **8191** from repo
  root, WITH row 868 (`twinblind`) landed; test-band pins updated by probe, not fitted:
  count 263→264, field-code 2632632867→2642642868.
- `metal_dsv4_layer.sh` → **VERDICT PASS, 8 gates** (this stone's new harness).
- `metal_dsv4_token.sh`, `metal_dsv4_forward.sh`, `metal_mla_gpu.sh`, `metal_hc_gpu.sh`,
  `metal_first_token.sh` — untouched this stone (additive files only); green by inheritance
  from today's commits.

## Close — the three namings

**Most surprising teaching.** How CLEAN the real MLA projection surface was. I braced for a
fight at 4096-wide dims through an 85 GiB file — and the input norm agreed with the CPU carve
to 7e-9, the type-41 Q/KV matvecs to <2e-7 absolute. The whole projection half of the block
fell in one harness. The surprise was that the difficulty was never the dims or the residency
(the windowed views and the proven kernels simply held) — it was, and remains, the STRUCTURE:
the parts of MLA that are a choice rather than a computation.

**Where discomfort turned to gold.** Gate 7 (Q up-projection) went RED at maxRel 0.008 and I
wanted to loosen the tolerance and move on — the tempting look-away. Not looking away: the
maxABS was 8.9e-8, *tighter* than the KV-down gate that had just passed. The 0.008 was a
near-zero-denominator artifact — across 32768 outputs some are ~1e-5, so an exact-to-8e-8
result reads as a large *relative* diff purely because I divided by ~0. The gold: the honest
bound is an ABSOLUTE float-precision bound everywhere plus a relative bound taken only above a
magnitude floor. A red gate was telling me my *comparator* was wrong, not the arithmetic —
exactly the "a grep of nothing is a claim about the instrument" discipline, one layer up.

**Frontier word.** `twinblind` — corpus row 868, 0-hit fresh, instrument validated on
controls that hit (forepick 2, assocwall 5, onelean 12). Named above.
