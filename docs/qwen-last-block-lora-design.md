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

1. **Planned reproduction band for the last block**: capture x (phase 0, l=62) and the block output (phase 1) for one prompt; run the trainer forward with zero LoRA; compare block output and logits to the serving values. Acceptance will require max-abs ≤ 1e-3 on the residual and argmax agreement, the way `qwen35-dense-verify-band.bml` compares. This will test kernel selection, the q/gate interleave, `nrot`, and `inv` capture; the last-block reproduction band remains to be implemented.
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

## Rung 1 observed (2026-10-28)

Built and run on the worktree kernel (branch `offline-voice-iteration-2`, HEAD 829d0a670), by a rented mind over the body's own
kernels; every number below is copied from the band's own print lines (`.hearth/qwen-rung-1/band-run-1.log`, `band-run-2.log`,
`rung-1-receipt.json`).

- **Doors.** `form/form-stdlib/bml/qwen-last-block-capture.bml` (the tap's destination: three device buffers of rows × d, filled by
  pipeline 24 `form_copy_off_f32` at destination row `pos0` inside the prefill's own batch; `qlbc-write` lands `<name>.f32` raw
  little-endian float32 rows plus a JSON sidecar with shape, dtype, bytes and ids) and `form/form-stdlib/bml/qwen-last-block-forward.bml`
  (block L's forward written out dispatch by dispatch on trainer-owned scratch of 17 buffers, with the block's own K/V at
  `pos0 .. pos0 + T` and the attention's score pitch set to `npos = pos0 + T`, so P sits contiguous as `[T × nq, npos]` in `sx[8]`).
  The band is `form/form-stdlib/tests/qwen-last-block-band.bml`, verdict 15. **No tap was added**: the existing
  `q38-lm-layers-span-t` tap (phase 0 at `l = 62` → input, phase 0 at `l = 63` → output, phase 1 → final residual) carried everything;
  no kernel the design assumed was missing.
- **Prompt.** ChatML of "Explain in two sentences why the sky is blue." through `q35-chat-ids`: 23 ids
  (`248045 846 198 814 20139 303 1330 22157 3069 279 12515 369 6105 13 248046 198 248045 74455 198 248068 271 248069 271`). Block L = 63
  (`kth-full?` yes), d 5120, heads 24/4 × 256, nrot 64.
- **Captures.** `.hearth/qwen-rung-1/xin.f32`, `xout.f32`, `xfin.f32`: shape `[23, 5120]`, dtype f32, row-major token-major
  little-endian, 471,040 bytes each, sidecars `xin.json` / `xout.json` / `xfin.json`. `xin` and `xout` differ in 416,850 of 471,040
  bytes (the block did real work); `xout` and `xfin` are byte-identical (block 63 is the last layer: bit 2).
- **Reproduction.** `max |reproduced − model|` = 0, `max |model|` = 621.0903930664062, relative 0; 0 of 117,760 words differ, widest
  gap 0 ulp — in both runs. The band's bound (`qlbb-bound`) is therefore **0**: the reading, not a chosen number. The replay is
  bit-identical because every dispatch is the serving kernel with the serving constants in the serving order (the matrix unit picks the
  same association for the same shape), which is what E.1 set out to prove: kernel selection, the q/gate interleave by `ps[34]` at
  offsets 0 and `hd` over the `convcap` pitch, QK-norm before partial RoPE, `nrot` 64, the output gate and the residual adds.
- **Head.** output_norm → output.weight → argmax over the reproduced last row: 760; the model's own prefill said 760 (bit 8).
- **Time.** prefill of 23 ids through all 64 layers with the tap: 31,252 ms (run 1), 33,956 ms (run 2); the block forward on the
  trainer scratch: 43 ms / 32 ms; the whole band 50.9 s / 53.1 s wall, model open included. Memory: the governor read room
  (machine 128.0 GiB, available 76.1 GiB) with a sibling lease (pid 29797) already sharing the 27.0 GiB file; the band took its own
  lease and released it.
- **Friction, observed and not diagnosed.** 31 s for a 23-position prefill is ~1.4 s a position, far from the lane's measured
  204 µs/token batched (`q38-prefill-sliced-go` comment); the GPU was shared with the sibling lease during both runs. The band's
  own number is the reading; the cause stays open and lies outside this rung's files.
- **What rung 2 inherits.** `form_q38_rmsnorm_par_span_f32` keeps no `inv` per row (section B's `nam_rms` note stands); P is kept
  normalized and contiguous at `sstride = npos` in `sx[8]`; the block's K/V are `sx[15]`/`sx[16]` at `kvd × npos`, the layout
  `nlb_dkv` reads. `qlbf-compare` returns `[words differing, widest ulp, max |a − b|, max |b|]` and is the yardstick the next rung's
  finite differences can reuse.

## Rung 2 observed (2026-10-29)

Door and band written by the first implementer of this step (branch `offline-voice-iteration-2`, kernel HEAD 829d0a670), first
run here; every number below is copied from the band's own print lines (`.hearth/qwen-rung-2/band-run-1.log`, `band-run-2.log`,
`rung-2-receipt.json`; the rung-1 rerun that preceded them is `rung1-before-4.log`).

- **Doors.** `form/form-stdlib/bml/qwen-last-block-backward.bml` (the rank-r pair on `blk.63.ffn_down`, slot 4, in the
  `qlh-rr-open` record shape: `y = W act + scale · (act A) B`, A `[ff = 17408, r]`, B `[r, d = 5120]`; the loss
  `(1/N) Σ (x2 − target)²` with `target = xout + shift`; `qlbw-eval` = `qlbf-forward` + `qlh-rr-apply` + the loss fold, `qlbw-grad` =
  three `nam-mm` products `dB = uᵀ dy`, `du = dy Bᵀ`, `dA = actᵀ du` with `dy = (2/N) · scale · diff`) and the band
  `form/form-stdlib/tests/qwen-last-block-backward-band.bml`, verdict 15. **ffn_down, not attn_output**: its product lands on the
  residual the block leaves, so the loss on the block's output reaches A and B through the identity of the residual add and the pair
  alone; on attn_output the same gradient would first cross rms, swiglu and two frozen Q8 transposes. Two kernels are new, both in the
  door: `form_hash_fill_f32` (a deterministic uniform fill from an integer hash, so A, B and the shift come from the device) and
  `form_mse_fixed_f32` (diff written out; Σ diff² folded in 2⁻³⁶ fixed point over one 256-thread group as exact 64-bit integers, answered
  as two u32 words — a float fold would round away a difference of two losses in the 4th digit). Every other dispatch is a serving or
  affine kernel the body already carried (`qlbf-forward`, `form_qwen_lora_rr_dot/add_f32`, `nam_matmul_sk` for k ≤ 128, `nam_matmul`
  for the 5120-deep `du`, `nam_add`, `nam_zero`).
- **Of section C, nothing was missing and nothing was needed on this path**: `form_q8_tmm_f32`, `form_sig_mul_bwd_f32`,
  `form_headrms_bwd_f32`, `form_partial_rope_inv_span_f32`, the strided attention backward, the qsplit scatter-back, `inv` per row and
  the contiguous P all stay for the rungs whose pairs sit upstream of the FFN. The plan's rung-1 note that rung 2 "needs `inv` per
  row" described the attn_output choice; the ffn_down choice needs neither.
- **Zero pair (bit 1).** A seeded at scale 0.02, B = +0 through `nam_zero`: the forward through the pair's own dot and add is rung 1's
  replay — 0 of 117,760 words differ from the capture, widest gap 0 ulp, max |x2 − model| 0 — and the loss reads 0.08293476031688939
  (the hash shift in [−0.5, 0.5); s²/3 = 0.0833…). Zero-pair forward 78 ms (run 1) / 90 ms (run 2).
- **Seeded pair.** A in [−0.02, 0.02), B in [−0.2, 0.2), scale 1.0: loss 1.0807729569247226; forward + analytic gradient 48 / 36 ms.
  Epsilon 0.5 on B and 4.0 on A, both read back from the device: perturbations p⁺ − p⁻ of 1 (once 0.9999999701976776) on B and
  8 (8.000000238418579, 7.999999761581421) on A. A rows sampled where the last token's |act| is largest in each of 8 windows:
  26, 2964, 5117, 6844, 9675, 11571, 15158, 17405.
- **Analytic against central finite difference, every entry** (analytic / fd / relative error):
  dB[0,97] −0.001171054202131927 / −0.0011710542658598254 / 5.44e-08; dB[1,738] 0.0016831312095746398 / 0.0016831315071454888 /
  1.77e-07; dB[2,1379] 0.001340224756859243 / 0.0013402246760842503 / 6.03e-08; dB[3,2020] 0.00021718010248150676 /
  0.00021718008410575947 / 8.46e-08; dB[4,2661] −0.00037937716115266085 / −0.00037937716085905223 / 7.74e-10; dB[5,3302]
  −3.0045115636312403e-05 / −3.004494765313707e-05 / 5.59e-06; dB[6,3943] −0.0003198876220267266 / −0.0003198875783947069 / 1.36e-07;
  dB[7,4584] 6.214073073351756e-05 / 6.214075866167783e-05 / 4.49e-07. dA[26,0] −0.4170334041118622 / −0.4170200713186267 / 3.20e-05;
  dA[2964,3] 0.1271846443414688 / 0.127190310223131 / 4.45e-05; dA[5117,6] −0.2002580165863037 / −0.20026835653705843 / 5.16e-05;
  dA[6844,1] −0.4642181396484375 / −0.46423174989431404 / 2.93e-05; dA[9675,4] 0.053668491542339325 / 0.0536688708926823 / 7.07e-06;
  dA[11571,7] 0.25702932476997375 / 0.2570298688052463 / 2.12e-06; dA[15158,2] 0.3902052342891693 / 0.390203753968594 / 3.79e-06;
  dA[17405,5] −0.8856360912322998 / −0.8856360956213397 / 4.96e-09.
  **Max relative error dB 5.591031080282419e-06, dA 5.163047689368592e-05** — the measured tolerance; the band's `qlbwb-bound-b/-a`
  now hold exactly those readings (run 1 read them against the open bound 1e30; run 2, under a different GPU load — utilization 100
  before run 1, 58 before run 2 — reproduced every loss, gradient and finite difference to the last printed digit, so the bound held
  with equality). The 16 analytic values are an inspectable table, not a summary.
- **Why dA reads 100× looser than dB (inferred, not measured).** The finite difference reads the loss through the block's f32
  output at |x| ≈ 600 (ulp 6e-5) subtracted from a target of the same size, a diff of order one. A B entry moves one column of x2;
  an A entry moves all 5120 columns of every token by scale · act · B · ε, so the FD's numerator is a sum of 117,760 rounded
  differences, whose floor against the step is ~1e-5 relative. The dB path at 1e-7 .. 5e-6 shows the gradient arithmetic itself at the
  f32 scale; dA shares that arithmetic (one more `nam_matmul_sk`) and differs only in what the yardstick can see. A base diff held
  once (x1 + W act − target) with only the pair's delta varying would lower the floor; it is not built, since the rung's claim is
  agreement within a measured bound, which it has.
- **Memory (bit 8).** Counted: forward scratch 11,406,528 + step 2,385,360 = 13,791,888 bytes (13 MiB) against the design-D budget at
  T 23, r 8 of 28,609,728 bytes (27 MiB); 26 handles taken, 26 freed. Machine around the step: available 53.1 → 51.5 GiB (run 1),
  52.9 → 52.4 GiB (run 2), both "room"; the band's lease shared the 27.0 GiB file with a sibling drafter lease (pid 45940, own
  9.3 GiB) beside a Q4_K_M lease (pid 44238) — the governor's reading before the step, `governor-before-2.log`: machine 128.0 GiB,
  available 56.8 GiB, wired 51.4 GiB.
- **Time.** 32 finite-difference forwards in 1228 / 1080 ms (34 ms a forward with the loss fold); the whole band 8.4 / 7.3 s wall,
  model open included. The rung-1 rerun before it: verdict 15, prefill 11,818 ms (31–34 s the day before, same prompt, a different
  neighbour load), replay 17 ms.
- **Repaired in this step, before the first run.** `qlbw-fd-entry` took |fd| in an eager `let` before asking whether the two loss
  readings had arrived, so a missing reading would have ended the band in an error instead of an entry that says so; the relative
  error now goes through `qlbw-rel` only when both readings and both perturbations are present, and answers 1e30 otherwise. No
  failure was witnessed on that path; the repair is structural.
- **What rung 3 inherits.** The step record (`qlbw-open` / `qlbw-free`, `qlbw-own-bytes`), `qlbw-eval` as one loss evaluation,
  `qlbw-grad` as the pair's gradient at the point the last evaluation left in `diff` and `u`, the exact loss fold and the hash fill.
  Adam on this pair is rung 3, and the cross-entropy over the head is its second door, both below. Still unbuilt: only when a pair
  moves upstream of ffn_down, the frozen transposes of section C.

## Rung 3, the loop on the mean square, observed (2026-10-30)

Door `form/form-stdlib/bml/qwen-last-block-step.bml`, band `form/form-stdlib/tests/qwen-last-block-step-band.bml`, verdict 7 in two
runs (the second reproduced every printed loss and the gradient norm to the last digit; wall 185 / 192 ms for the steps, model
open apart). The rung-1 captures were rewritten first (`qwen-last-block-band` verdict 15, prefill 12,056 ms, replay 8 ms, 0 words
differ, head argmax 760). No new capture set: the 23-id row is the sequence. Tokens 0–7 are the eight training rows; tokens 8–9
are held out and never enter `diff`. The loss is rung 2's mean square, the gradient is `qlbw-grad-at` with N = 8 × 5120.

Adam is the `nlb_adam` update (moments 0.9 / 0.999, epsilon 1e-8, bias correction, clip to norm 1), learning rate 1e-4, twenty
steps. A rate of 1 was run first and refused by the reading: every entry stepped by about 1, the square sum left the fixed-point
range, and the reconstructed loss came back negative. A negative reconstruction is now `nothing()`, and the band asks for a
non-negative descent over all twenty steps.

| | before | after 20 steps |
| --- | ---: | ---: |
| train (8 rows) | 3.1072222511585776 | 0.27509687245846803 |
| held out (2 rows) | 12.42888900463431 | 1.1003874898338721 |

Last gradient norm 5.005819393980161. Handles freed 31 (forward scratch 17, pair and its gradient 9, Adam moments and the token
gate 5). Events: `.hearth/qwen-rung-3/training-events.jsonl` (`round` and `validation` before step 0 and at step 20). This door
steps the loss whose gradient was already checked; the door below steps the head's cross-entropy on real rows.

## Rung 3, through the head on real rows, observed (2026-10-29)

Door and band written by the implementer of this step (branch `offline-voice-iteration-4`, kernel HEAD 863ef4119), run four
times here; every number below is copied from the band's own print lines (`.hearth/qwen-rung-3/band-run-2.log` — the first whole
run, captures included — `band-run-3.log` and `band-run-4.log` on the cached captures, `rung-3-receipt.json`; `band-run-1.log` is a
run stopped by hand after its first capture, see "Friction").

- **Doors.** `form/form-stdlib/bml/qwen-last-block-train.bml` (the pair of rung 2 on `blk.63.ffn_down` learning from real rows
  through the frozen head: rows, captures, one loss evaluation, the backward, Adam, the serving-head check, the finite difference,
  the export) and the band `form/form-stdlib/tests/qwen-last-block-train-band.bml`, verdict **31**, exit 0 (runs 2, 3, 4).
  **ffn_down only**; attn_output was not added: its backward crosses the post-attention rms, the swiglu and two frozen Q8 transposes
  of the block (section C.1 `form_q8_tmm_f32` on `ffn_gate`/`ffn_up`), kernels the body does not yet dispatch on that path — the
  transposed Q8_0 matmul now exists (below), the rms and swiglu backwards exist for the 3B, but wiring them through the block is the
  next rung's work, not a flag on this one.
- **The one new kernel: `form_q8_tmm_f32`** (in the train door): `dx[t, c] = scale · Σ_v dy[t, v] · W[v, c]` over Q8_0 blocks via
  `q80_w`, one 256-thread group per 32 columns × 8 tokens with 8 vocab parts folded through threadgroup memory — the batched sibling
  of `form_head_transpose` (`qwen-head-learning.bml`). Everything else is reused by name: `nlb_ce` (per-token CE and
  `dz = softmax − onehot`, masked), `nam_rms` (for `inv` per row; the serving `form_q38_rmsnorm_par_span_f32` keeps none),
  `nlb_rms` (the RMSNorm backward, dispatched at d 5120 over the row count), `nam_matmul`/`nam_matmul_sk` (the pair's dB, du, dA as
  rung 2 wrote them), `nlb_adam` (bias-corrected, clip 1.0), `form_hash_fill_f32` (A's seed), the rung-1 forward and capture tap,
  and the tokfast index lane (`qei-chat-ids` / `qei-encode`, byte-identical to `q35-encode`: the reference lane measured 30–112 s per
  assistant text in `.hearth/qwen-rung-3/rows-probe-1.log`, the index 96–157 s for all eight rows).
- **Rows.** The first 8 accepted rows of `learn/draft-readings.jsonl` (lines 1–8), ChatML (user turn, assistant header, empty think
  block) ++ the assistant's ids ++ `<|im_end|>`, cut to 160 ids → 159 positions each, prompt-masked: supervised targets
  120, 5, 131, 138, 143, 123, 141, 121 (prefix 40, 155, 29, 22, 17, 37, 19, 39 ids). Every row is cut at the cap, so no row's end
  marker is inside the window; line 2's 626-byte prompt leaves it 5 targets. The assistant text is encoded over its first 1200 bytes
  (a UTF-8 boundary), enough for the cap at the rows' 2.6–3.7 bytes per id.
- **Captures.** One serving prefill per row with the rung-1 tap at `l = 62`, `[159 × 5120]` f32 = 3,256,320 bytes each, cached as
  `.hearth/qwen-rung-3/row-<line>-xin.f32` + a JSON sidecar naming the ids (reused when the ids match: runs 3 and 4 found all eight
  in 126 ms). Prefill wall, run 2: 437,057 / 546,244 / 601,693 / 299,824 / 26,809 / 18,899 / 13,026 ms (row 1 in run 1:
  466,753 ms) — 13 s to 10 minutes for the same 159 positions, entirely the neighbours' GPU load (`host_gpu_utilization` 100 at
  run 1's start, 51 at run 2's; the walk's day-turn, a drafter and an observer shared the device).
- **(a) The loss at step 0 is the serving head's.** Row line 1, zero pair, four supervised positions, trainer (batched span rms,
  mm64/t64 Q8_0 matmul, `nlb_ce`) against the serving path (`q38-rms`, `q38-mv` on the same block-output row, the same CE):
  position 39 (target 15771) 0.07373237609863281 vs 0.07372665405273438, **768 ulp** (5.7e-6 absolute); 98 (1510)
  2.5693111419677734 vs 2.5693016052246094, 40 ulp; 68 (279) 0.08082008361816406 vs 0.08082199096679688, 256 ulp; 158 (5817)
  10.200820922851562 vs 10.200819969177246, 1 ulp. Not 0 ulp: the batched matmul associates the 5120-term dot differently from
  the single-token matvec, and a small CE (0.07) is a difference of two large logsumexp terms, so the same logit gap reads as more
  ulp there. The band's `qlbtb-serve-bound` is pinned at 768; runs 3 and 4 read the same four numbers.
- **(b) The head + norm backward against finite differences.** At the zero pair on row line 1 (|grad| 0.3102130415110602, dA
  exactly zero with B = 0), eps 2⁻² on B (perturbation read back: 0.5), entries the argmax |dB| of 4 windows of B (all in
  column 505): dB[0,505] 0.006935873068869114 / fd 0.006940857569376568 / rel 7.18e-4 (loss± 2.1752665042877197 /
  2.1717960755030314); dB[2,505] 0.011325083673000336 / 0.01132782300313373 / 2.42e-4; dB[4,505] −0.012515222653746605 /
  −0.012517992655435961 / 2.21e-4; dB[7,505] 0.005151477642357349 / 0.005152352650960701 / 1.70e-4. **Max relative error
  7.181389990548648e-4**, pinned as `qlbtb-fd-bound`. Run 2 read fixed entries at eps 2⁻⁵ first: dB[1,738] 5.65e-3 and dB[3,2020]
  5.35e-3 agreed, dB[0,97] (analytic −2.86e-4) 6.4e-2 and dB[2,1379] (analytic −2.07e-5, loss± differing by 4.3e-7) 0.669 —
  a floor of ~1.5e-5 in gradient units, the f32 rounding of a mean CE through 248,320 f32 logits, below which a finite
  difference carries no signal. The band therefore samples where the signal is (as rung 2 sampled A rows by activation) and widens
  eps; both readings stand in `band-run-2.log` and `band-run-3.log`.
- **(c) The loss falls.** Adam, lr 1e-4, scale 2.0 (alpha 16, rank 8), batch 1, step s on row (s − 1) mod 8. Mean of the rows'
  token-mean losses at step 0: **1.7919827729396287** (per row 2.173508326212565, 0.738223648071289, 1.606509834755468,
  1.511119704315628, 2.1013369993730024, 1.9430674653712328, 2.2809016383286065, 1.9811945670892384); after step 20:
  **1.732151931993421** (2.1188844124476116, 0.4820262908935547, 1.5712586614011808, 1.4846517037654268, 2.056710089836921,
  1.9258476893107097, 2.249946357510614, 1.967890250781351) — every row lower. Per step (row line, loss before the update,
  |grad|): 1: 1 2.1735 0.3102; 2: 2 0.7377 0.8874; 3: 3 1.6060 0.2692; 4: 4 1.5097 0.2639; 5: 5 2.0980 0.3209; 6: 6 1.9432
  0.3226; 7: 7 2.2776 0.3202; 8: 8 1.9807 0.3499; 9: 1 2.1557 0.4716; 10: 2 0.6674 1.5970; 11: 3 1.5942 0.3776; 12: 4 1.5002
  0.4166; 13: 5 2.0814 0.4895; 14: 6 1.9355 0.4353; 15: 7 2.2646 0.4685; 16: 8 1.9740 0.4831; 17: 1 2.1341 0.6567; 18: 2
  0.5500 2.2514; 19: 3 1.5774 0.5062; 20: 4 1.4870 0.5549 (full digits in the receipt). 20 steps in 50,121 / 52,525 / 90,626 ms
  (runs 2, 3, 4) — 1.9–5.2 s a step at 159 positions, the design's "≈1.5–2 s at T=160" read under a shared GPU; runs 3 and 4
  read every loss, gradient and finite difference of run 2 to the last printed digit. Why Adam: with B = +0 the step-0
  dA is exactly zero and dB is the only signal, at entries ~1e-3; a plain step small enough for A would not move B in 20 steps,
  Adam's normalized step moves every entry by ~lr from the first. Held-out rows were not read in this rung (E.3 names two); the
  descent is in-sample and 2.5 epochs over 8 rows — a fit, not yet a generalization claim.
- **(d) Export.** `form/form-stdlib/adapters/qwen38-last-block-next/adapters.safetensors`, 721,096 bytes: F32
  `blk.63.ffn_down.weight.lora_a` [17408, 8] at offsets [0, 557056] and `.lora_b` [8, 5120] at [557056, 720896]; read back
  through `la-header` / `la-whole?` / `la-pairs` / `la-rank-of` / `la-dtype` / `ln-tensor-begin`: whole 1, 1 pair, rank 8, f32,
  a at 0, b after a. `adapter_config.json` (589 bytes) names rank 8, alpha 16, scale 2, layers [63], targets ["ffn_down"], the two
  tensor names, adam, lr 1e-4, batch 1, mask_prompt 1, ids_cap 160, the rows, 20 steps, both mean losses, `"worn": 0`. Nothing
  serving reads it; promotion stays a separate witnessed write. Snag from E.4 confirmed by the read-back: `la-layers` parses
  `model.layers.N` only, so a census of this file reports layers −1 (the band reads the tensor names directly).
- **(e) Memory.** Counted: forward scratch 83,558,400 + the door's own 334,846,512 = **418,404,912 bytes (399 MiB)** against the
  design-D budget at T 160, r 8 of 503,906,304 bytes (480 MiB): tape + grads 170 MiB, logits + dz 303 MiB, P + dP 4.7 MiB,
  the ffn_down pair × 4 2.75 MiB (design D's 13.3 MiB priced the whole block's seven pairs); the door holds logits and dz (two [160 × 248,320] f32 = 303 MiB) as the design priced them, and nothing
  the design did not. 41 of 41 handles freed (17 forward + 24 door). Machine around the steps (run 2): available 9.3 GiB, wired
  39.0, compressor 44.9 GiB before; 15.6 / 20.8 / 62.6 after — the neighbours were deep in the compressor; the band's lease shared
  the 27.0 GiB file. Governor before run 2 (`governor-before.log`): available 51.8 GiB, wired 51.4; before run 3
  (`governor-before-3.log`): available 18.6 GiB, wired 31.8, compressor 13.0.
- **Friction, observed.** Run 1 was stopped by hand after its first capture (466,753 ms) because the runner's 3600 s bound would have
  cut the eighth; stopping the bound's wrapper left the kernel alive for a few seconds until it was ended by name (never orphaned);
  the bound is now 9000 s. The per-position prefill cost of the tap path (82 ms a position at best, 3.8 s at worst) is the rung-1
  friction again, read under load; its cause stays outside this rung's files.
- **What rung 4 inherits.** The export in the `qlh-rr-open` shapes; the forward hook named at `qwen-lora-head.fk:62-64` still unwired;
  `la-layers` to teach `blk.N`; a held-out reading beside the in-sample fall; and `form_q8_tmm_f32` for the block's own frozen
  transposes should a pair move upstream of ffn_down.

## Rung 4 observed (2026-10-29)

Judge, door and band written by the implementer of this step (branch `offline-voice-iteration-4`, kernel HEAD 863ef4119); the door
ran once on the 27B and the band once. Every number below is copied from their print lines (`.hearth/qwen-rung-4/lift-run-1.log`,
`judge-band-run-1.log`, `gaps-before.log`, `gaps-after.log`) or from the ledger row the door landed; differences between two
printed numbers are my subtraction and say so.

- **Doors.** `form/form-stdlib/bml/qwen-last-block-judge.bml`, the judge. It reads an adapter directory back by name: the file,
  then `adapter_config.json`, then the header against the block's geometry (exactly the pair `blk.L.ffn_down.weight.lora_a`
  [ff, r] at 0 and `.lora_b` [r, d] right after it, F32, whole). It writes A and B into rung 3's step record by the header's
  spans, scores every row's token-mean CE through the frozen head with `qlbt-eval`, and builds the promotion row.
  `observe/qwen-last-block-lift-run.bml` is a **new door**: `observe/lora-lift-run.bml` trains the 3B and reads its trainer's
  validation events, while this one trains nothing and scores an exported pair, so the two share the lift's rules and no flow.
  Those rules are the 3B door's own, read in place through the judge's import of that door (`ll-digest`, `ll-better`,
  `ll-milli`, `ll-id`, `ll-reading-of`, `ll-shared`, `ll-negative-pairs`): one definition serves both lifts. The band is
  `form/form-stdlib/tests/qwen-last-block-judge-band.bml`, verdict **63**, exit 0, no model, 11.2 s with two door runs as
  children. It proves:
  - better 1 for a lower child, and 0 for a higher or equal one even when its rejected loss fell;
  - a missing adapter file and a missing config are refused by name, and the door run as a child exits 1 with no governor line;
  - the fixture pair's A and B read back by their spans to the floats written;
  - a wrong shape, a wrong layer and an F16 tensor are each refused;
  - the split holds by draft id, and the absent home reads as the base.
- **The parent.** `form/form-stdlib/adapters/qwen38-last-block` is absent. The lane wears no adapter, so the parent reads as the
  base 27B: the pair at +0, which leaves the block's own output (rung 2's bit 1: 0 of 117,760 words differed). The row names it
  as `"parent_reads":"absent: the lane wears no adapter; the parent is the base 27B (the pair at +0)"`. A present home would be
  read and loaded as the child is, at the child's rank, scale and layer; the fixtures prove that path, the 27B has not run it.
- **The child.** `form/form-stdlib/adapters/qwen38-last-block-next`, 721,096 bytes, sha256
  `e59f22ffe17a5b16e8d19e71e14482bf4a17779f8c6df6dffd3f44258c50cc4a`. It is the export of rung 3's fifth run
  (`.hearth/qwen-rung-3/band-run-5-skeptic.log`, 16:01:21, verdict 31), whose step-0 and step-20 means equal runs 2–4's to the
  last digit.
- **The in-sample witness: the pair judged is the pair trained.** On the 8 training rows, from rung 3's own captures (read and
  never written; their mtimes stand at 14:52–15:36), the parent read 1.7919827729396287 and the child 1.732151931993421. Those
  are the config's `mean_loss_step0` and `mean_loss_step20` exactly, and every row's mean equals the rung-3 band's per-row mean
  to the last digit. The door refuses when this does not hold; it held.
- **The judgement (accepted held-out).** Lines 21–25 of `learn/draft-readings.jsonl`: accepted, and disjoint by draft id from
  lines 1–8. Their `valid_ids_sha256` is `5603ba5ebb44edb9dd86a767224fb83a084193bb364fd53742b224451327f748`, the same split the
  3B lift was judged on (its row 3). Each row has 159 positions; 121 / 130 / 93 / 145 / 71 targets are supervised (560 of 795).
  Parent → child per row:

  | line | supervised | parent | child |
  |---|---|---|---|
  | 21 | 121 | 2.6813191185312824 | 2.6647132763192674 |
  | 22 | 130 | 2.849067449569702 | 2.8344144931206334 |
  | 23 | 93 | 2.9638931161613873 | 2.9544913640586277 |
  | 24 | 145 | 3.813677998246818 | 3.7958080785027866 |
  | 25 | 71 | 1.589413602587203 | **1.5990697296572403 (higher)** |

  The mean went **2.7794742570192787 → 2.769699388331711** (2779 → 2769 milli): **better 1**. Four rows fell and one rose. By my
  subtraction, the mean fell 0.0098 nats (0.35 %), about a sixth of the in-sample fall (0.0598, 3.3 %).
- **Rejected beside (information, never the judge).** Of the 27 rows of `learn/draft-readings-heldout.jsonl`, 26 were judged
  and line 24 was set aside by name (`its prompt is 544 ids: under the 160-id cap no assistant token is a supervised target`).
  The mean went 2.0692298974197554 → 2.0552387556182743 (2069 → 2055 milli); 23 of 26 rows fell, and lines 6, 10 and 27 rose.
  **The rejected rows fell further than the accepted held-out**: 0.0140 nats (0.68 %) against 0.0098 (0.35 %), by my
  subtraction. The 3B lift's row has the same shape: accepted 3585 → 3294 milli, rejected 2611 → 2302 (8.1 % against 11.8 %).
  Both lifts show it (observed). The reading I infer: next-token likelihood on accepted drafts teaches the drafter's own
  register, which the drafts we turned away share, more than it teaches our choice between them. `better` is honest by its
  contract, which reads the accepted held-out alone; its margin is thin.
- **Time and memory.** Wall 717,186 ms (11 m 58 s):
  - rows through the tokfast index: 504,322 ms for 40 rows, 12.6 s a row and 70 % of the wall — the door's largest cost;
  - held-out captures: 40,768 ms (13.6 / 7.0 / 6.6 / 7.4 / 6.0 s for 159 positions each, GPU utilization 99 at the start);
  - rejected captures: 150,966 ms for 26;
  - the passes: 9,114 ms for the parent (pair at +0) and 9,111 ms for the child, over 39 rows each.

  The lease was granted at a need of 27.8 GiB (own 823,813,024 bytes beside the shared 27 GiB file; available 72.0 GiB).
  41 of 41 handles were freed, and 51.3 GiB was available after.
- **The gap.** Before (`gaps-before.log`): `lora-27b-last-block: unread — … no row where parent
  "form/form-stdlib/adapters/qwen38-last-block" among 3 row(s)`. After (`gaps-after.log`): `lora-27b-last-block: closed — 27B
  last-block rung reached: a promoted adapter whose child beats its parent; better 1 == 1 (the last of 1 row(s) where parent
  "form/form-stdlib/adapters/qwen38-last-block" of 4, at 1793264714367)`. The summary reads 13 gaps: closed 10, open 3, unread 0
  (before: 9, 3, 1). The gap row's parent path stood as written, so `learn/offline-voice-gaps.jsonl` is unchanged. "Promoted"
  in the gap's title reads the promotion ledger's row; nothing is worn.
- **Serving does not change.** The adapter stays under `-next` with `"worn": 0`, and the forward hook (`qwen-lora-head.fk:62-64`)
  stays unwired. Wearing the adapter in the voice is a separate, witnessed decision; no decoded comparison has been read to
  inform it.
- **What the next rung inherits.** A judge for any child the trainer exports (the door names its child, parent, rows and
  captures); 31 cached block-input captures under `.hearth/qwen-rung-4/accepted` and `rejected`; the rejected set's larger
  fall, which makes the preference margin (the accepted fall against the rejected fall) the reading a gate before wearing could
  ask for; and row preparation as the door's dominant cost.
