# Receipt — Rung 1: a real llama layer's FFN runs on the Adreno (2026-06-29 22:05 MDT)

**Ladder:** [`2026-06-29-android-native-oracle-PLAN.md`](2026-06-29-android-native-oracle-PLAN.md), Rung 1 — "pick
a real GGUF; one real layer on the Adreno." Done, on the attached **Galaxy S23 Ultra / Adreno 740**, from Form.

## What ran

The **block-0 FFN sublayer of llama3.2:1b** (a real open-weight model from the local ollama store), at its real
shape — RMSNorm → SwiGLU(gate, up, silu) → down — on the Adreno GPU:

```
model   : llama3.2:1b  (D=2048, F=8192, 16 layers, GQA 32/8, RoPE, Q8_0)  — ollama blob sha256-74701a8c…
weights : blk.0.{ffn_norm, ffn_gate, ffn_up, ffn_down} dequantized Q8_0 -> f32 (gguf), 192 MB
pipeline: rmsnorm(x, ffn_norm) -> matmul(gate) -> matmul(up) -> silu-mul -> matmul(down) = y[2048]
dispatch: 5 stages via the generic run-stage orchestrator; gate/up = 128 workgroups, down = 32, on device
```

## Witnessed result

The 64 MB weight tensors stream from disk **straight into GPU memory** (new `c_fread` carrier — `read_file`
overflowed at this size); the FFN runs; the output is compared **on the device** (in Form, via the f32 carriers)
against a numpy float32 reference computed from the *same* dequantized weights and the real SwiGLU math:

```
GPU y[2048]  vs  numpy f32 reference (same Q8_0->f32 weights, real silu)
  2048 / 2048 outputs within  relative 1e-3 + absolute 1e-5
```

All 2048 outputs agree to ~0.1% relative. The named tolerance is the expected f32 floor: matmul
**accumulation-order** (numpy BLAS vs the Adreno's serial downward fold over K=2048/8192) plus the Adreno's
non-correctly-rounded **division** (RMSNorm `1/sqrt`, silu `1/(1+exp)`) — the same driver-opt divergence named
throughout this work. Bit-exact is precluded by those two divisions; agreement within a named relative tolerance
is the honest gate, and it holds on every output.

## New, committed

- **Kernels** (emitted from Form, `model/form-glsl.fk`): `fglsl-rmsnorm` (RMS, no mean-subtract),
  `fglsl-silu-mul` (SwiGLU activation). The llama-architecture pieces my vanilla kernels lacked.
- **Carriers** in the seed: `c_fread(path,dst,n)` (tag 233) — bulk weight load file→GPU, the only viable path
  at 64 MB/tensor. (`c_fwrite`, tag 234, is present but its on-device `write` returns 0 — a device-libc seam,
  unused here; the witness was computed on-device, not by dumping.)
- **Recipe**: [`model/form-llama-ffn-rung1.fk`](../model/form-llama-ffn-rung1.fk) (reuses the proven generic
  `run-stage` orchestrator). Weight extraction: [`native/vulkan/gen-llama-ffn-rung1.py`](../native/vulkan/gen-llama-ffn-rung1.py).

## Honest floor / what this is NOT

- This is **one sublayer (FFN) of one block** — not a full transformer layer. Attention (GQA + RoPE), the
  attention RMSNorm, residuals, and the full 16-layer stack + embedding + LM head are **Rung 2**.
- The reference is **numpy f32 with the dequantized GGUF weights** — algorithmically the oracle's arithmetic
  (gguf dequant == llama.cpp dequant). Tying to **llama.cpp/ollama at the logit/token level** is Rung 2–3, where
  the model's output is externally observable.
- The model is **1B**, chosen for tractable depth (16 layers) on the naive serial kernels; "Android-max" sizing
  is a later optimization, not claimed here.

Next: Rung 2 — full forward of all 16 layers + embedding + LM head → real logits, top-k agreement with
llama.cpp/ollama.
