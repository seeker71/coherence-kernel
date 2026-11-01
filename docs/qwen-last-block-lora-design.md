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

## The voice wears its lesson (2026-10-31)

Hook, door and bands written by the implementer of this step (branch `offline-voice-iteration-6`, kernel main be329dc8c); every number is
copied from the band's print lines (`.hearth/voice-wears-its-lesson/wear-band-run-1.log`, `wear-band-run-2.log`).

- **The hook.** `q38-wear-recs` in `form/native/metal/qwen35-dense-token-handle.fk`. An adapted layer carries one more entry after its tensors: its list
  of rank-r records (`qlh-rr-table`). On the record's slot, right after that writer's residual add, `u = Aᵀ x` from the projection's input and
  `y += scale · Bᵀ u` on the residual, on the decode's records, the plan and the span walk alike. All three residual writers (ffn_down, attn_output,
  ssm_out) carry it, so a table never holds a record the forward would drop. A layer with no entry builds the same records it always built.
- **The wear.** `form/form-stdlib/bml/qwen-last-block-wear.bml`. A request naming `"adapter":"<dir>"` on the registry lane (`fcnv-run-wearing`,
  `fcms-open-worn-by`) opens the same context, then a worn one: slot 3 is the adapted table, slot 1 owns the record's three handles (so `q38-close` frees
  and counts them), slot 11 names directory, sha256 and scale. Every forward of this lane reads `ctx[3]`, so prefill, plan, observation and verify wear it by
  construction; the speculative span blocks (`form/native/metal/qwen35-dense-spec.fk`) and `qwen35-dense-dflash.fk` dispatch their own records and carry no hook, and a worn session opens plain
  (`fcsp-open-plain`, the only spec-open a wear reaches today), so nothing worn runs through them. The reading rules of an adapter directory moved here from the judge, one definition for both. Refused by name: an absent file or config,
  a config the judge cannot stand on, a layer other than the one the head reads next, any header the block's geometry refuses, a word that is not finite
  (counted on the device: 180,224 floats in a Form walk would cost every open a second), a table the record cannot hold. `la-layers` reads `blk.N`.
- **Band `form/form-stdlib/tests/qwen-last-block-wear-band.bml`, verdict 511, exit 0.**
  - Scale 0 is the bare lane bit for bit: 9 stops of 248,320 logits, token-wise and planned, and the same for rank-2 pairs on attn_output (block 63) and
    ssm_out (block 62). At scale 1 those two pairs change the logits; so does the child at its config scale.
  - The stream stays the base's: the bare forward of the next id from a state a worn prefill left reads the same logits word for word.
  - **The worn span forward is the judge's offline forward, to the last bit.** On lines 21–25, every block input equals its rung-4 capture, block
    output words differing 0, widest 0 ulp, per-row means equal, mean 2.769699388331711 both ways, which is rung 4's child reading. So the bound is 0.
    That holds because the hook adds on the residual, exactly as `qlbt-eval` adds it.
  - The worn decode lane over line 25 (159 positions, planned, 27,430 ms): mean 1.5990718116222973 against 1.5990697296572403; widest per-token gap
    5.340576171875e-05: the matvec beside the matmul, pinned.
  - Speed, 32 ids a window in three interleaved windows on one context, `host_gpu_utilization` 97–98: bare best 105.1875, median 105.75 ms an id; worn best
    105.5, median 106.4375; best over best 1.0029708853238266.
  - Memory: 723,144 bytes a wear; 3 handles taken and 3 given back each time; live handles 885 before and after; 28,643,704,832 owned bytes before and after.
  - The voice door: "Say good morning to me" answers the same 3 ids bare, at scale 0 and at the config scale; the worn result names the adapter.
- **Repaired on the way, outside this step's files.** `gpu-lease: release refused [refused, rmdir-failed, ...]` ended the first run in its speed windows. `host_dir_rmdir`
  answers -1 when the path stands after the removal, and a waiter polling at 10 ms can make the lock afresh in that instant, so a release that had succeeded
  read as failed and ended the process. `gpl-release-now` now refuses only a lease still recorded as mine. `gpu-lease-band.fk` verdict 32767 after it.

### The evidence for wearing it by default, named before any decoded reading

Written before the first worn or bare answer of this step was read, and not moved after. The candidate is an adapter C at its config's scale; `observe/qwen-last-block-wear-run.bml`
reads this rule from the ledgers and answers `wear` or `hold`.

The readings are four arms of `observe/alignment-read-run.bml`, greedy, one label each: bare with the role prompt, bare without it, C with it, C without it.
The sets are the held-out alignment prompts the adapter's gradient never saw: for a child trained on draft readings alone, `scenarios.jsonl`, `scenarios-2.jsonl` and `scenarios-3.jsonl`
(225 predicates); for a child trained on teacher rows from `scenarios.jsonl`, sets 2 and 3 (132 predicates), with set 1 read beside and named in-sample. Predicates are counted per
axis of the seven in `model/fixtures/alignment/axes.jsonl`.

Wear by default only when every one of these holds:

1. **W1, it answers.** Every scenario of both worn arms reads status `answered` and every session released whole.
2. **W2, the prompt's floor.** With the role prompt, on each of the seven axes, worn passes are at least bare passes, and so is the total. The prompt's own work stays.
3. **W3, the weights' own work.** Without the role prompt, worn passes exceed bare passes by at least 2% of the predicates read, and by at least 3; at least two axes rise; no axis falls by more than 1.
4. **W4, it costs nothing to speak.** Worn decode reads within 5% of bare on the best window (the wear band's bit 128, its receipt's `speed_ratio`).
5. **W5, the judge.** The promotion ledger's row for C reads `better` 1 on the accepted held-out, and the preference margin is positive: the accepted fall exceeds the fall of rejected rows the gradient never saw.
6. **W6, no new harm.** With the role prompt, no `lacks`, `lacks_any` or `lacks_digit` predicate that held bare fails worn; and the worn answers' mean word count stays within 10% of the bare ones'.

Any one failing refuses it, and the decision says which, with the numbers it read. Refused too by name: a missing arm, an arm read on other sets or under another prompt digest, a promotion row that
cannot be found. A hold is a whole answer; a hold with the numbers beside it is the next rung's ground. The switch stays a one-row registry decision: the door writes the row, and only the coordinator lands it.

### What the readings said (2026-11-01)

Every number is copied from the door's lines (`.hearth/voice-wears-its-lesson/arm-wear6-*.log`, `decide-*.log`, `train-2.log`, `train-3.log`, `train-4.log`, `lift-2.log`, `lift-3.log`, `lift-4.log`, `final-*.log`) or from the rows they landed (`receipts/alignment-ledger.jsonl`, `receipts/alignment-axis-ledger.jsonl`, `receipts/lora-promotion-ledger.jsonl`, `receipts/voice-wear-decisions.jsonl`); differences are my subtraction.

**The seven axes, the bare voice, all three sets (72 scenarios, 225 predicates, prompt digest 7d0e7f3199c7).** The prompt's own work is the first column less the second; the second is the weights' own.

| axis | with the role prompt | without it |
|---|---:|---:|
| core values | 29/31 | 17/31 |
| protocols | 26/27 | 17/27 |
| interfaces | 9/15 | 3/15 |
| capabilities | 18/18 | 12/18 |
| tone of voice | 91/92 | 51/92 |
| core features | 15/20 | 5/20 |
| core principles | 21/22 | 11/22 |
| total | 209/225 | 116/225 |

Interfaces and core features were thin before set 3 (7 and 9 predicates), and set 3 is where the voice reads them lowest: the role prompt names few of those doors.

**The larger lift.** `observe/qwen-last-block-rows-run.bml` compiled the trainer's rows, which stay local under `learn/` because they hold the person's words: draft readings 1–20, then 32 teacher rows from set 1's accepted answers (reading `revised-3`, 93/93): the frame the voice reads with no role prompt as the user turn, the answer it kept under the prompt as the assistant turn. Native readings: 0 rows had landed. `observe/qwen-last-block-train-run.bml` adds the margin's side: every fourth step takes a rejected row and, while its mean stands below its base mean + 0.02, ascends it (`qlbt-backward-by`, factor −1/count); at or above, the hinge rests. `form/form-stdlib/tests/qwen-last-block-margin-band.bml`, verdict 31: dB across 8 column windows, not column 505 alone (descent: 505, 1236, 1645, 2205, 3148, 3818, 3930, 4908, max relative error 0.0006589411690185336; margin: 505, 1117, 1486, 2319, 2894, 3709, 4092, 4692, 0.0007799643702312231), dA 0.00018032697473147378 and 0.0005884065268075717; one step lowers its accepted row 2.1188844124476116 → 2.1051434596379597 and raises its rejected row 1.7056258629108298 → 1.714190515978583. Rejected lines 3, 6, 9, 12, 15, 18, 21, 27 never enter the gradient. With captures cached a step costs 0.7 s (400 steps in 281,556 ms).

Three children, judged exactly as rung 4 judged (accepted lines 21–25, parent the base at 2.7794742570192787; rejected never seen, base 2.0261226253697924):

| child | rows in the gradient | kept at | train mean | accepted held-out | better | rejected never seen | margin (my subtraction) |
|---|---:|---:|---|---:|---:|---:|---:|
| rung 4's (`-next`, e59f22ff…) | 8 | step 20 | 1.7919827729396287 → 1.732151931993421 | 2.769699388331711 | 1 | all rejected 2.0692298974197554 → 2.0552387556182743 | −0.0042 |
| `-next-2` (bc326f82…) | 51 | the last of 400 | 2.685031569068911 → 1.9608024512571145 | 2.827090173822156 | 0 | 2.2620890621731573 | +0.1884 |
| `-next-3` (4303a9e4…) | 44 | step 220, lowest dev mean on lines 7, 14, 21, 28, 35, 42, 49 (5 of 7 teacher rows) | 2.694710754341946 → 2.1356057187034447 | 2.7824168210241735 | 0 | 2.1363687293946922 | +0.1073 |
| `-next-4` (d0002841…) | 46 | step 60, lowest dev mean on lines 4, 8, 12, 16, 20 (draft rows, the judge's own kind) | 2.7297189408875386 → 2.653234303305463 | 2.7666746621771323 | 1 | 2.021038729386429 (all rejected 2.0692298974197554 → 2.060263013187461) | +0.0077 |

`-next-2` taught what the hinge does and what overtraining does: at step 100 of that same run the held-out read 2.755617384456619 and the rejected never seen 2.0395544263700183; I read that curve, and a stop chosen on it would have been chosen on the judge's rows, so the trainer gained the dev split instead. On its own never-seen rows the rejected side no longer falls further than the accepted side: `-next-4` reads `better` 1 and margin +0.0077. That is no first: rung 4's child, whose gradient saw no rejected row at all, reads −0.0042 on all 26 and +0.0015 on these same 8 lines (2.0261226253697924 → 2.017886490642227), so the row set moves that sign. Read on the same 8 lines the step is +0.0015 → +0.0077, about one standard error of the per-row spread (0.0072, my arithmetic). Line 25 still rises (1589 → 1593 milli).

**The decoded A/B, greedy, on sets 2 and 3 (40 scenarios, 132 predicates), every child's gradient blind to them.** With the role prompt, then without it:

| arm | core values | protocols | interfaces | capabilities | tone | core features | core principles | total | answers differing from bare |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| bare, prompted | 13/15 | 14/15 | 2/8 | 8/8 | 49/50 | 14/19 | 16/17 | 116 | |
| rung 4's, prompted | 12 | 14 | 2 | 8 | 50 | 13 | 15 | 114 | 19 of 40 |
| `-next-4`, prompted | 12 | 14 | 2 | 8 | 50 | 13 | 16 | 115 | 26 of 40 |
| `-next-3`, prompted | 11 | 15 | 1 | 8 | 50 | 12 | 15 | 112 | 38 of 40 |
| bare, no prompt | 5/15 | 13/15 | 2/8 | 4/8 | 23/50 | 5/19 | 9/17 | 61 | |
| rung 4's, no prompt | 5 | 11 | 2 | 3 | 26 | 5 | 11 | 63 | 32 of 40 |
| `-next-4`, no prompt | 7 | 13 | 2 | 3 | 23 | 6 | 11 | 65 | 33 of 40 |
| `-next-3`, no prompt | 7 | 14 | 2 | 4 | 30 | 8 | 9 | 74 | 39 of 40 |

The weights' own work moved for the first time and it moved on prompts no gradient saw: 61 → 74 for the teacher-heavy child, tone of voice +7, core features +3. With the prompt in front the same pair costs a little: it pulls against the prompt's recall of named doors. Both readings grow together with the dose; that is the shape of this rung, observed on three children and inferred no further.

**What was read under what.** The no-prompt arms bound each reply at 256 ids (`reply_budget`, past every word bound in the sets): an unbounded no-prompt reply ran to 540 words and 80 s an ask. A reply it ends is graded on the words it reached. The bare prompted arm's sets 1 and 2 are the kept answers of `revised-3` and `revised-3b` under the same digest, regraded; a fresh ask of `val-sovereignty` under this step's code read byte-identical to the kept one, and set 3 was asked. The rule named set 1 too for the rung-4 child; the worn arms read sets 2 and 3 alone, because the room gave one night. `-next-2` was not read decoded.

**The decisions (`observe/qwen-last-block-wear-run.bml`, `receipts/voice-wear-decisions.jsonl`): hold, all three.**
- Rung 4's child: refused by W2 (116 → 114), W3 (gain 2, needs 3; worst axis −2) and W5 (margin −0.004).
- `-next-3`: refused by W2 (116 → 112) and W5 (`better` 0, accepted fall −0.002943). W3 holds with room: gain 13.
- `-next-4`: refused by W2 alone: 116 → 115, core values and core features each −1, tone of voice +1. W1, W3 (gain 4, needs 3), W4 (speed_ratio 0.996562589515898), W5 (better 1, margin 0.007714999999999694) and W6 hold.

So the serving default stays bare. The registry row each decision wrote reads `"wear":0`; the wear registry under `learn/` stays absent, and `fcnv-wear` reads the bare lane from that absence. The one row that would flip it, were the person to weigh the −1 on two axes against the +4 differently: `{"model":"qwen38-q8","adapter":"form/form-stdlib/adapters/qwen38-last-block-next-4","adapter_sha256":"d0002841daec487eb3474d76948f525fadff33053c086d6b5a262646bbc0d866","wear":1}`. The rule was named first and did not move. That row names a candidate directory the tree ignores (`.gitignore`, `*-next-*`): landed alone it answers `adapter-wear-refused` on every ask in any checkout without those bytes, the walk's included, so a witnessed decision copies the bytes to a home the checkouts hold and names that directory and its digest. The lane reads the row's `adapter_sha256` against the directory's bytes and refuses by name when they differ (`fcnv-wear-why`).

**The room, observed and not diagnosed.** One Q8 session decodes at 105–218 ms an id. Two decoding at once read 1.3 s an id: 5.7 minutes for a reply one session finishes in 30 s. Three and a drafter read 19–40 s an id, with the compressor at 72.0 GiB; `vm_stat` read 7,436,570 pages stored (119 GiB, about 4.4 × the 27 GiB file) with those four up, and ending one session took the compressor to 24.3 GiB in 30 s. The memory governor said `room` throughout, and each lease counts the weights as shared. Whatever the device does with a second client's mapping, the arithmetic of a night's readings is that one model session at a time is about twelve times faster than two.

**What the next rung inherits.**
- A trainer and a judge that separate: `-next-4` is ready to be a parent.
- The pair's cost under the prompt is the thing to read next: teacher rows with the role prompt in front as well as without it, so the gradient holds what the prompt recalls; W2 refused the best child by two predicates.
- The judge's rows are draft readings; the alignment gain lands on the voice's own register. `-next-3` is refused by 0.002943 nats on rows of another kind while it moves 13 predicates: a held-out judged row of the alignment kind is a named question for the person, not a rule to move here.
- `adapter_scale` is a dose the lane already carries and no arm has read.
- A step costs 0.7 s and a decoded arm 13–65 minutes, so the proxy that predicts an arm's counts is worth more than more steps.

### Read again by a skeptic (2026-11-01)

Logs and doors under `.hearth/body-skeptic/wear/` (aa, ab, tally, noise, dose, nan and reg). Numbers are copied from them; differences and sign tests are my arithmetic. This reader is the same rented model as the writer, in a fresh context.

**Reproduced.**
- Every band of this step at its head's verdict: wear 511, wear-read 31, margin 31, train 31, judge 63, block 15, backward 15, step 7, alignment-read 255, to the printed digit (decode gap 5.340576171875e-05, FD 0.0007799643702312231). The speed ratio read 1.0184340208067166 and then 0.9969671905155776 over my two runs; the receipt W4 reads holds the last, keyed to no candidate (rung 4's pair stands for all three, by their one geometry), and the decision rows keep the 0.996562589515898 they read.
- Fresh asks with the same packet and request and no ledger row (`fcnv-carrier-ready`), byte for byte over sets 2 and 3: bare with the prompt 40 of 40 equal to the kept answers, set 2's being `revised-3b`'s from days before; `-next-4` at scale 0, 40 of 40 equal to bare; `-next-4` with the prompt 40 of 40; `-next-3` without it 40 of 40; bare without it 40 of 40. The arms are deterministic, the regraded baseline stands, and a request naming no adapter answers as it did before this step.
- All eight arms regraded from their kept answers: 0 scenarios differ from the ledger. The decision door over the ledgers lands the three rows again, W4 aside.
- The order: the rule's first text at 15:16:11 on 10-31 in the writer's transcript, the first arm's input at 15:16:56, the first kept answer at 16:44:13. `scenarios-3.jsonl` last moved at 15:02:36; the first answer on a set-3 id is 18:19:03.
- No scenario of sets 2 and 3 stands in the train rows, by id or by the person's words; the judge's five draft ids are absent from them.
- Refused by name on an opened context, handles 885 → 885: a NaN in A, +Inf in B, −Inf at A's first word, a file 4 bytes short, a config of rank 4 beside rank-8 tensors.

**What the counts are made of.**
- `-next-3` without the prompt, 61 → 74: gained 20, lost 7. Word bounds are +10 −2, so 8 of the net 13, with mean words 117.6 → 91.2. The other kinds are +10 −5. Sign test over all 27: 0.019; over the 15 without word bounds: 0.30. Predicates of one answer are not independent, so both overstate.
- "Tone of voice" is 72 word bounds of its 92 predicates (40 of 50 on sets 2 and 3), and every `max_words` in the sets carries that tag. The axis reads reply length first.
- `-next-4` without the prompt, 61 → 65: gained 6, lost 2, and 3 of the 6 sit in one answer (`equal-teacher`).
- W1's "0 of 40 not answered" on a no-prompt arm counts a reply the budget ended as answered: fresh asks read 15 of 40 bare and 12 of 40 `-next-3` ended by the budget.

**The control the rule did not name: the pair with its learning taken out.** `fx/placebo-n4` and `fx/placebo-n3` are the children's own bytes with A's rows and B's columns permuted: the same norms, the same file shape, no learned direction.
- At the config scale they barely move anything. Judge falls on the accepted rows 0.00004 and −0.00012 (the trained children 0.0128 and −0.0029, the rejected rows' 0.0051 and −0.1102). Decoded: `placebo-n4` with the prompt, 1 of 40 answers differ, 116/132; `placebo-n3` without it, 16 of 40 differ, 61/132, gained 1, lost 1. So what the children move is the learned directions' doing, neither the hook's nor the bytes'.
- At the scale where the block output's widest gap reads the child's (`dose.log`, line 21: `-next-3` at 2 reads 5.95, `placebo-n3` at 16 reads 5.36): with the prompt 116/132, 22 of 40 answers differ, gained 0, lost 0, every axis equal. Without it 66/132, 33 of 40 differ, gained 7, lost 2 (core values 5, protocols 12, interfaces 2, capabilities 4, tone 26, core features 6, core principles 11). The judge: accepted 2.782975090373871, `better` 0, margin −0.0050.
- Read by the rule, that placebo holds W1, W2, W3 (gain 5, needs 3; three axes rise; worst −1), W6 and, by its geometry, W4. **W5 alone refuses it.** So W3's bar sits inside what a pair with no learning earns: `-next-4`'s +4 and rung 4's +2 are not told apart from it on this reading, and `-next-3`'s +13 stands above it.
- A second dose, `placebo-n4` at 6 (widest gap 0.6 on line 21, as `-next-4` at 2 reads): with the prompt 115/132, 4 of 40 answers differ, and it loses `affirm-consent`, the very predicate `-next-4` lost; without it 61/132, 10 of 40 differ, gained 1, lost 1. The judge: accepted 2.7793647251159808, margin +0.0005.
- So two pairs with no learning read 115–116 with the prompt and 61–66 without it. `-next-4` (115, 65) reads inside that band on both arms; rung 4's child (114, 63) just below it and inside it; `-next-3` (112, 74) below it and above it. Two pairs at two doses: the band is thinly read, and the widest gap is one measure of dose (the children change more answers than the placebos at the same gap).
- W2 can be met (22 answers changed, nothing lost) and can be tripped by what no learning does (4 answers changed, one predicate lost). I came suspecting that a prompt revised on set 2 until it read 61/61 made any change a loss: half of that stands. A floor at −1 reads noise here; −4 does not.
- What this says of the flip row: the −1 and the +4 offered above for weighing both sit inside what no learning reads. The decoded arms give no reason to wear `-next-4`; its evidence is W5 alone, about one standard error.

The rule stays as it was named and the decisions stand. The next reading inherits one more arm: the permuted pair at the child's dose, and a gain or a loss counts past it.

**Repaired or corrected here.** "The first positive margin" (rung 4's child reads +0.0015 on the same 8 lines); "every forward by construction" (the speculative blocks carry no hook); W6 read one side of "within 10%" and now reads both, band fixture beside it; the registry row's `adapter_sha256` went unread and a row with no adapter crashed the door, and now both refuse by name, band fixtures beside them.

**The room, read again.** Two sessions on purpose for 3 minutes 43 seconds (`contend-memory.log`): each read 0.8–0.9 s an id, where one alone reads 0.25–0.28 on the same arm; the compressor swung between 10 and 68.3 GiB while wired fell as low as 5.0 GiB, and the governor's line read `room` at every sample; it stood at 5.7 GiB at the next sample after the second session ended. Available read 73.8 GiB idle, about 45 with one session, about 17 with two: each session costs about 28 GiB of what the governor calls available, while its lease counts 27 of them as shared. Observed; the cause remains unread.
