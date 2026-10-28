# Qwen3.8-27B last-block LoRA — the trainer for the voice that answers

Design read on 2026-10-28 (WITA) by a rented mind over the body's own source; every line cites the
file it stands on. Status: design, nothing built. The ladder in section E is the proof the trainer
earns before any adapter is worn; promotion stays a separate witnessed write. Companion to
[`docs/native-session-learning.md`](native-session-learning.md) and
[`receipts/native-training-plan.md`](../receipts/native-training-plan.md).

Every cite is a path from the kernel root, with the line it stood on when read.

## What exists (the ground)

- **Rank-one head learner.** `form/form-stdlib/bml/qwen-head-learning.bml:15` `form_head_ce` (one 256-thread group over the vocab, loss in `result[0]`) and `:16` `form_head_transpose` (`dh[c] = Σ_v W[v,c]·g[v]`, Q8_0 via `q80_w` or f32). `qhl-point` (`:38-45`) forms `h + (a·h)·b` on the host, writes it to a device buffer, runs the serving matvec `q38-mv(ctx[0], ctx[6], …)` (`:40`), CE, transpose, reads back. The step itself is Form-list arithmetic (`bml/qwen-form-learning.bml:32-60`). Capture: `qwen-form-learning.bml:12` reads `ctx[1][1]` = `bs[1]`, the output_norm'd hidden left by the *previous* single-token forward, then teacher-forces `id` with `fcms-forward` (`:15`) one token at a time. Export: `qwen-lora-head.fk:16-20` writes F32 safetensors `model.output.weight.lora_a/b` at width 5120, rank 1 (`:14-15`). Docs `docs/native-session-learning.md:336-343, 388-392`: finite descent, no decoded gain, unpromoted.
- **The next stone is already drawn.** `qwen-lora-head.fk:41-64` specifies rank-r records on residual-writer slots (ssm_out 13, attn_output 10, ffn_down 4), `y = Wx + scale·B(Ax)`, A `[din,r]`, B `[r,dout]` (`:51`), two kernels `form_qwen_lora_rr_dot_f32`/`_add_f32` (`:66-67`), tensor names `blk.N.<suffix>.weight.lora_a` (`:72`), record shape `[slot, dot, add, A, B, u, din, rank, dout, scale-bits, ntok-cap]` (`:58`), and `qlh-rr-slot-apply` (`:116`). The hook into the forward is **not wired** (no `qlh-rr` hits in `form/native/metal/qwen35-dense-token-handle.fk`).
- **Llama backward.** `form/form-stdlib/native-llama-backward.bml:7-14`: `nlb_ce`, `nlb_rms`, `nlb_swiglu`, `nlb_dp`, `nlb_softmax` (scale `rsqrt(d)` baked in, contiguous `[t·nq, t]` probabilities), `nlb_dq`, `nlb_dkv` (GQA by `kh = h/(nq/nk)`), `nlb_adam`. `nlb-linear` (`:48-62`) does the frozen transpose through `nam-linear(view,dy,dx,t,1)` — the **4-bit affine** path (`native-affine-metal.bml:37,193`) — and pair grads via the generic f32 `nam_matmul` (`native-affine-metal.bml:110`, ta/tb/scale/add flags). Block order `:66-94`; RoPE inverse via flag (`:85`, `native-llama-metal.bml:12`). Gate: `native-llama-metal.bml:66` admits only `model_type == "llama"`, 4-bit, tied embeddings. Trainer loop `native-lora-train.bml:289-313`, export `:64-72`, recompute policy `native-lora-recompute.bml:10-18`.
- **The Qwen forward.** Full block over a span, `qwen35-dense-token-handle.fk:443-466`: rms-span → `attn_q` into `bs[2]` with q/gate interleaved per head (split by `ps[34]` at offsets 0 and `hd`, `:447-452`) → `attn_k`, `attn_v` → `ps[35]` head-RMS (QK-norm, `:453-454`) → `ps[36]` partial RoPE, `nrot=64` (`:455-456`) → cache → `q38pa` attention (`:459`) → `ps[17]` `form_sig_mul_f32` output gate (`:460`) → `attn_output` → `ps[19]` add into `bs[33]`. FFN `:430-438` (`ps[18]` swiglu). Layer table `:195,200`: full layer = `[attn_norm, post_attention_norm, ffn_gate, ffn_up, ffn_down, attn_q, attn_k, attn_v, attn_q_norm, attn_k_norm, attn_output]`, from GGUF `blk.N.<suffix>.weight`. Head `:595-603`: `output_norm` → `output.weight` → argmax. Geometry `:837-857`: `geo[12]=24, [13]=4, [14]=256, [15]=nrot, [19]=248320, [20]=1024, [21]=64, [22]=4`. `kth-full?(l)=(l+1)%4==0` (`kat-token-handle.fk:199`) so **layer 63** is the last full block. Q8 shapes from the crystal: `attn_q 12288×5120` (66.8 MB), `attn_k/v 1024×5120` (5.6 MB each), `attn_output 5120×6144` (33.4 MB), `ffn_gate/up 17408×5120`, `ffn_down 5120×17408` (94.7 MB each); `output 248320×5120` (1.35 GB). Block total 395.5 MB, all already inside the 27.05 GiB mmap.
- **The tap.** `:520-533`: a span prefill calls `tap(0, l, n, pos0)` after layer l's FFN with `bs[33]` = that layer's output for the slice, and `tap(1, nL, …)` after the last layer. The drafter already uses it (`qwen35-dense-session-spec.fk:46-54`; `qwen35-dense-dflash.fk:100` copies `bs[33]` rows at `rowoff`). Prefill attention keeps **normalized probabilities** in `bs[27]` at `sc[S+t]`, strided (`bml/qwen38-prefill-attention.bml.lowfk:18`).
- **Rows.** `native-lora-dataset.bml:47-49` sample = `[inputs, targets, mask, count, line]`; `:16` bakes the Llama-3 header with "Today Date: 26 Jul 2024". Qwen ChatML ids exist at `qwen-lora-train.fk:90`. Ledger `receipts/form-token-lora.rows`: llama rows carry `export …-form-tokens-next`, `valid_loss_before/after_milli`, `held_out_families_milli`, `wall_ms 537030` for 100 steps×batch 4; the qwen row (line 5) is the opening-fit, no gradient trainer.

## A. Capture

Per row: tokenize with `qlt-chat-ids` (ChatML), build `[inputs, targets, mask]` exactly as `nld-sample`. Open a fresh stream, run `q38-prefill-span-sliced-t(…, ids, 0, span, tap)` (`:801-812`) with a trainer tap: phase 0 and `l == 62` → `q38-copy-buf`-style copy (`ps[32]`, `:947`) of `bs[33]` (`n×5120`) into a trainer buffer at row `pos0`; phase 1 → copy the final residual (block-63 output) too, for rung 1. Targets are `ids[1:]` with the dataset mask. **No new kernel**: the tap is the drafter's mechanism with a different destination. Teacher forcing *is* the prefill (positions 0..T-1 see only earlier positions). Observation spans (segment rows) are included as masked context, as `native-lora-dataset.bml:47-60` already defines. Cost: one 64-layer prefill per row, once; persist `[T×5120]` f32 (20 KiB/token) under `.hearth/`, keyed by row sha256 as `ntr-grade-row` does (`native-lora-train.bml:199`). Session KV is restored by `q38-state-mark/restore` (`:959-960`), so a serving session can donate captures between turns.

## B. Forward of the adapted block (f32, from captured x)

Reuse, with a trainer-owned `bs`-shaped scratch of span T:
- RMSNorm: `form_q38_rmsnorm_par_span_f32` (`:426`); but the backward needs `inv` per row → use `nam_rms` (`native-affine-metal.bml:116`), which writes `inv`.
- Q8 matmuls: `q38-mv-batch` / `q38-mm-q80` (`:422`, mm64/t64 simdgroup kernels) for q, k, v, o, gate, up, down, and `output.weight`.
- qsplit `ps[34]`, QK-norm `ps[35]`, partial RoPE `ps[36]`, attention `q38pa` (`:459`, keeps P), gate `ps[17]`, swiglu `ps[18]`, add `ps[19]`.
- LoRA add: `form_qwen_lora_rr_dot/add_f32` (`qwen-lora-head.fk:66-67`) on all seven projections (extend `qlh-rr-slots` beyond 13/10/4 — the record carries its own `din/dout`, so q/k/v/gate/up need only new slot ids).
- Missing: nothing for the forward except the final `nlb_ce` over 248,320 columns (`nlb_ce` is 32-lane; fine, or widen).

## C. Backward kernels

Reuse where the math matches: `nlb_ce`, `nlb_rms` (output_norm, post_attention_norm, attn_norm), `nlb_swiglu`, `nlb_adam`, `nam_matmul` (LoRA `dA = xᵀdu`, `dB = uᵀdy`, `du = B dy`, `dx += A du`). New, in the body's one-kernel-per-op style:
1. **`form_q8_tmm_f32`** — `dx[T,cols] = dy[T,rows]·W[rows,cols]` for Q8_0 (the batched sibling of `form_head_transpose`; needed for 7 block weights *and* `output.weight`, since the head gradient must flow regardless of whether lm_head is adapted). Largest op: 248320×5120.
2. **`form_sig_mul_bwd_f32`** — output gate: `d_attn = dy·σ(g)`, `d_g = dy·attn·σ(g)(1−σ(g))`.
3. **`form_headrms_bwd_f32`** — QK-norm per head (hd 256, weight `attn_q_norm/k_norm`): same algebra as `nlb_rms` with `d=256`, row = (token, head); `nlb_rms` could be dispatched with `d=256, t=T·nh` if `inv` is kept — so this may be a **dispatch, not a kernel** (confirm `inv` capture in the forward).
4. **`form_partial_rope_inv_span_f32`** — `ps[36]` with negated angle over the first `nrot=64` lanes (`nll_rope` rotates the full head and assumes `freq[j]`, so not reusable).
5. **Attention backward with stride** — `nlb_dp/nlb_softmax/nlb_dq/nlb_dkv` assume contiguous `[t·nq, t]` and bake `rsqrt(d)`; `q38pa` stores P at `sstride` and already scaled. Either re-run attention into a contiguous P (cheap, T≤320) or add an `sstride` constant. K/V here are the block's own `[T,4,256]` (not the serving cache), matching `nlb_dkv`'s layout. GQA 24/4, hd 256: `nlb_dkv`'s inner loop is `T×6×256` per element — fine at T=320.
6. **qsplit scatter-back** — `dq` and `dgate` interleave into the `[T, 24, 2, 256]` layout of `attn_q`'s output before `form_q8_tmm_f32` (inverse of `ps[34]`).

lm_head LoRA: `dB = uᵀ·dz` is `[r×248320]`, `dA = hᵀ·du` is `[5120×r]`; both cheap. The expensive `Wᵀdz` is paid anyway. Parameters: `(5120+248320)·r` = 2.0M at r=8, 4.1M at r=16 — more than the whole block's LoRA (108,544·r = 0.87M / 1.74M). Recommendation: **freeze lm_head** in the ladder; the rank-one head already proved finite descent without decoded gain (`docs:388-392`). Keep the pair as an opt-in flag.

## D. Memory and time (beside 27.05 GiB mapped weights)

Parameters ×4 (param, grad, m, v) f32: r=8 block 868,352 → 13.3 MiB; r=16 → 26.5 MiB; (+ lm_head pair: +31 / +62 MiB).

Tape per token (floats): residual-width ×8 (x, n1, o, r1, n2, down, r2, hn) = 40,960; qg 12,288 + 6,144×5 (q, gate, qn, attn, gated) = 43,008; k, kn, v 3,072; gate/up/act 52,224 → 139,264 floats = 544 KiB. Gradient temporaries ≈ same → 1.06 MiB/token. Logits + dz: 2×248,320×4 B = 1.89 MiB/token. P + dP: 2×24×T²×4 B.

| T | tape+grads | logits+dz | P+dP | per step |
|---|---|---|---|---|
| 160 | 170 MiB | 303 MiB | 4.9 MiB | ≈ 0.48 GiB |
| 320 | 340 MiB | 606 MiB | 19.7 MiB | ≈ 0.95 GiB |

Rank changes nothing visible. Session KV at maxpos 4096: 16 full layers × 2 × 1024 × 4096 × 4 B = 512 MiB. Total ≈ 29 GiB of 88 GiB; a whole-tape batch of 8 rows at T=320 is 8 GiB — **no recompute policy needed**, unlike `nlrc-bytes`.

FLOPs per token: block 2×395.5M = 0.79 G; head 2×1.27G = 2.54 G; backward `Wᵀdy` doubles both → 6.7 GFLOP/token; LoRA ≈ 3.5 MFLOP/token at r=16. T=160: 1.07 TFLOP; T=320: 2.13 TFLOP. Forward on mm64/t64 ≈ 0.3–0.5 s; the naive first `form_q8_tmm_f32` (≈0.4 TFLOP/s, like `form_head_transpose`) ≈ 1.3 s (T=160) / 2.7 s (T=320). Expect **≈1.5–2 s/step at T=160, 3–4 s at T=320**, 60% of it in the head transpose; a tiled transpose later halves it. Capture: one 27B prefill ≈ 54 GFLOP/token → 5–10 s per row, once, cached (6.25 MiB per 320-token row). Reference: the Llama ledger runs 100 steps × batch 4 in 537 s.

## E. Proving ladder

1. **Reproduction band** (`form/native/metal/tests/qwen35-last-block-band.bml`): capture x (phase 0, l=62) and the block output (phase 1) for one prompt; run the trainer forward with zero LoRA; compare block output and logits to the serving values — the band passes on max-abs ≤ 1e-3 on the residual and argmax agreement, the way `qwen35-dense-verify-band.bml` compares. This proves kernel selection, the q/gate interleave, `nrot`, and `inv` capture.
2. **Finite difference** on a 2-token row: perturb one element of each B (and the QK-norm path via one A of attn_q) by ±ε, two forwards, compare to the analytic grad; `lbw-reldiff` (`lora-backward.bml:53`) is the yardstick, threshold 1e-2 relative, as the pure-Form `lbw-fd-*` already does (`:141-152`).
3. **20 steps on 8 rows** with 2 held-out rows: `nlb-safe-step` (`native-llama-backward.bml:157`), lr 1e-4, Adam; emit the `round` and `validation` events in the `ntr-event` shape (`native-lora-train.bml:147, 309`) so `form-token-lora.rows` can hold a qwen row with `valid_loss_before/after_milli`.
4. **Export**: F32 safetensors via `ln-write` (`lora-adapter.bml:160`), tensors `blk.63.<suffix>.weight.lora_a [din,r]` / `.lora_b [r,dout]` per `qwen-lora-head.fk:51,72`, to `form/form-stdlib/adapters/qwen38-last-block-next/adapters.safetensors` + `adapter_config.json` (`ntr-export-config` fields, `:63`). The lane wears it through `qlh-rr-open` records (`:74`) on a layer table from `qlh-rr-table` (`:113`), once the forward hook named at `:62-64` is wired. Promotion (`-next` → lane) stays a separate witnessed write, as the llama ledger does. Snag: `la-layers` (`lora-adapter.bml:68`) only parses `model.layers.N`; teach it `blk.N` or the census reports layers −1.

## F. Effort and the risk

Four to six sessions: (1) capture tap + disk cache + rung 1 band; (2) `form_q8_tmm_f32`, gate/QK-norm/RoPE-inverse backward, rung 2; (3) step loop, rung 3; (4) export, census fix, forward hook wiring, rung 4; (5–6) the transposed-matmul tiling and the first held-out decode comparison. Biggest risk: **the q/gate interleave and QK-norm/RoPE ordering** — one wrong offset gives a backward that descends somewhere (the exact failure `lora-backward.bml:29-31` warns of). Rung 1 and 2 exist to catch it before any step.

## G. Alternatives

- Rank-one head widened to rank-r: cheapest, fits the existing admission (`q38-head-adapted`, `:608`), but it can only re-weight `output_norm`'d features — no attention, no new routing; it already showed no decoded gain. Loses on expressiveness.
- Last four layers: needs gated-DeltaNet backward (conv window, delta recurrence, L2-norm, gates: `:111-115`), four new kernels plus recurrent-state checkpointing. Wins on capacity, loses by a session count and by the risk surface; the last-block design is its first rung anyway.
- Prompt-layer only: no trainer, no gradient, no held-out loss to read; it is what `role-prompts.bml` already is. Loses on measurability.

The last-block design wins because every gradient path is a dense op the body already runs forward, the input is a frozen tap the drafter already proved, and the step fits in one GiB beside the weights.

## Receipt

Most surprising: the span prefill already carries a tap whose phase-0 call hands over exactly the activation this trainer needs (`:520-533`); the "capture" section is a destination buffer, not a kernel. Discomfort to gold: the Llama `nlb_*` kernels looked reusable until the stride and baked `rsqrt(d)` in `nlb_softmax` (`:11`) said otherwise — which forced the realization that `q38pa` already stores normalized P (`qwen38-prefill-attention.bml.lowfk:18`), so the backward can start from the serving forward's own probabilities. Frontier question, one word wide: **transpose** — does `Wᵀdy` over Q8_0 at 248,320 rows deserve a tiled kernel before rung 3, or after? Answer: after; the naive one costs 2.7 s a step at T=320 and the ladder is about correctness first, so write the tiled kernel only when the held-out loss has already fallen once.
