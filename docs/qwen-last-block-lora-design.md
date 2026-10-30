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
  Adam on this pair is rung 3, below. Still unbuilt: the CE loss over the head (`nlb_ce` widened or `form_head_ce` batched),
  and — only when a pair moves upstream of ffn_down — the frozen transposes of section C.

## Rung 3 observed (2026-10-30)

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
gate 5). Events: `.hearth/qwen-rung-3/training-events.jsonl` (`round` and `validation` before step 0 and at step 20). The head's
cross-entropy is still unbuilt; this rung steps the loss whose gradient was already checked.
