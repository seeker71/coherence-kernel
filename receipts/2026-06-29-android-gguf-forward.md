# Receipt — Rung 2: the full 16-layer llama forward runs on the Adreno, logits agree (2026-06-29)

**Ladder:** [`2026-06-29-android-native-oracle-PLAN.md`](2026-06-29-android-native-oracle-PLAN.md), Rung 2 —
"full forward of all 16 layers + embedding + LM head → real logits, top-k agreement with the oracle." **Done**,
on the attached **Galaxy S23 Ultra / Adreno 740**, from Form.

## What ran

The **entire forward pass of llama3.2:1b** (real Q8_0 GGUF from the local ollama store), at full width and full
depth, on the Adreno GPU, driven from Form:

```
token embedding (tied)  →  16 × [ RMSNorm → Wv → GQA-expand(V) → Wo → +residual
                                   → RMSNorm → SwiGLU(Wg·,Wu·,silu,Wd·) → +residual ]
                        →  final RMSNorm  →  LM head (128256 vocab, tied embeddings)  →  argmax
```

Real model, real weights: 3.4 GB of per-layer tensors streamed from disk straight into reused GPU buffers via
`c_fread`; the 1 GB LM head tiled into 8 × 131 MB (the Adreno rejects a single 1 GB allocation despite an 11 GB
heap); the 16032-wide argmax recursion runs under `FORM_KERNEL_STACK_MB=2048`. Input: BOS (token 128000),
seq=1, pos=0 — where attention degenerates exactly to V (softmax of a single self-score = 1, RoPE at pos 0 =
identity), so Wq/Wk are correctly unused this step. Recipe:
[`model/form-llama-forward-rung2.fk`](../model/form-llama-forward-rung2.fk).

## Witnessed result

```
GPU greedy argmax over 128256 logits  =  16309   (reference argmax 16309)   ✓
GPU logit[16309] = 7.0514   (reference 7.051)
GPU logit[2]     = 7.0367   (reference 7.037)
```

The reference is numpy f32 on the *same* dequantized GGUF weights (gguf dequant == llama.cpp dequant); its top-2
are token 16309 (7.051) and token 2 (7.037) — **0.014 apart**. The GPU not only picks the same top token but
resolves that razor-thin gap in the **same order**, with both logits matching to 4 decimals. Matching a
1-in-128256 argmax *and* the sub-0.02 top-2 separation is not luck — the 16-layer forward is numerically faithful
to the oracle, within the named f32 floor (matmul accumulation-order + the Adreno's non-correctly-rounded
division, named throughout this work).

## The bug this rung caught (and why honesty found it)

The first full run gave argmax 55267 — wrong. Layer-by-layer on-device probing (not faking a pass) localized it:
every per-element [0] check matched, but the **full x-vector after block 0 was wrong past index 63**. The cause:
the **residual-`add` stages dispatched 1 workgroup instead of 32**. The add kernel is one-invocation-per-element
(64-wide local size), so 2048 elements need ⌈2048/64⌉ = 32 workgroups; with 1, only `x[0..63]` received the
residual and `x[64..]` silently kept the pre-add value. It matched at index 0 (the only index anyone spot-checks)
and corrupted everything downstream through the RMSNorm mean. Fix: groups 1 → 32 on both residual adds. This is
the exact failure mode the "no rung on toy inputs, verify the whole vector" discipline exists to catch.

## New / committed

- **Recipe**: [`model/form-llama-forward-rung2.fk`](../model/form-llama-forward-rung2.fk) — generic `run-stage`
  orchestrator, per-layer weight reload (`loadlayer`/`lpath`), GQA V-expand (`c_memcpy`), tiled LM head,
  deep-recursion argmax. Extraction: [`native/vulkan/gen-llama-forward-rung2.py`](../native/vulkan/gen-llama-forward-rung2.py).
- Builds on Rung 1's kernels (`fglsl-rmsnorm`, `fglsl-silu-mul`) and the `c_fread` carrier.

## Honest floor / what this is NOT

- Single forward step (seq=1, pos=0) → **next-token logits**. The KV-cached multi-token generation loop and the
  real tokenizer are **Rung 3**; this proves the forward arithmetic, not yet a decoded string.
- Config (D=2048, F=8192, 16 layers, GQA 32/8, head_dim 64) is still **hardcoded in the recipe**, not read from
  the GGUF header. Data-driven config (cf. `~/source/Coherence-Network/form/form-stdlib/gguf-read.fk`) is the
  next hardening step — a hardcoded dim is precisely the kind of latent bug that would pass one model and break
  the next.
- Reference is numpy-f32-on-GGUF-weights; tying to a live llama.cpp logit dump (vs. ollama serving) is a
  strengthening still owed at Rung 3.

Next: Rung 3 — tokenizer + KV-cache generation loop; agreement with ollama on a real decoded token sequence.
