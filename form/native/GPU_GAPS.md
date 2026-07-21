# Form-native GPU / ML enablement — gap registry

Living tracker for "any ML layer / diffusion / transformer / attention fully enabled on Mac, Android, RTX."
Status: ✅ done+proven · 🟡 in progress · ⬜ not started · ⛔ blocked.
Backends: **Metal** (Mac, MSL) · **PTX** (RTX/NVIDIA, driver JIT) · **Vulkan** (Android+desktop, SPIR-V).
"Proven" = bit-exact (or named-epsilon) on real hardware vs the CPU recipe oracle.

## A. Layer carriers (the math exists as portable Form recipes; this tracks the GPU CARRIERS)

| Layer | Recipe (CPU) | Metal | PTX (RTX) | Vulkan (Android) |
|---|---|---|---|---|
| matvec / matmul | ✅ | ✅ f32/f16/bf16 | ✅ f32/f16/bf16 | ✅ f32 **bit-exact on RTX Vulkan, Form-emitted, Android-portable** |
| affine SGD train | ✅ | ✅ | ✅ f32 | ⬜ |
| gelu (Taylor) | ✅ | ✅ (in FFN) | ✅ f32 *(verdict 127)* | ⬜ |
| exp / softmax | ✅ | ✅ (in attn) | ✅ softmax f32 *(verdict 511, Taylor exp)* | ⬜ |
| FFN/MLP fwd | ✅ | ✅ | ✅ f32 *(verdict 255, bar.sync)* | ⬜ |
| FFN/MLP backprop | ✅ | ✅ | ⬜ | ⬜ |
| attention (single-head SDPA) | ✅ | ✅ f32 *(verdict 1023, fused dot·scale→softmax→·V)* | ⬜ | ⬜ |
| attention (MHA/causal/KV) | ✅ | 🟡 (single-head done; MHA/causal next) | ⬜ | ⬜ |
| layernorm / rmsnorm | ✅ | ✅ f32 *(verdict 8191, Newton-50 sqrt)* | ⬜ | ⬜ |
| residual (vec-add) | ✅ | ✅ f32 *(verdict 8191)* | ⬜ | ⬜ |
| transformer block fwd | ✅ | ✅ f32 **end-to-end on GPU, bit-exact** (12-launch kernel-graph, pre-LN self-attn) | ⬜ | ⬜ |
| llama block (fwd/causal/decode) | ✅ | ✅ | ⬜ | ⬜ |
| conv2d / groupnorm (diffusion) | ✅ recipe *(verdict 15, 3-way)* | ⬜ | ⬜ | ⬜ |

## B. Precision coverage
- ⬜ f16/bf16 across the **block-level** PTX+Metal kernels (only matvec has all three on PTX/Metal).
- ⬜ affine-train + FFN + attention in f16/bf16 (PTX).
- ⬜ generic fp8 / fp4 (only GGUF Q4_K/Q6_K + int8/NF4 exist as dequant). Q4_K and Q6_K now have **GPU dequant + fused-matvec kernels on Metal** (`q4k-msl.fk`, `q6k-msl.fk`); PTX/Vulkan twins are ⬜.

## C. Cross-cutting runtime (needed to go from "a kernel" to "a model")
- 🟡 **Kernel-graph / scheduler**: DEMONSTRATED — `form_cuda_ptx_block_host.c` chains 12 launches through resident intermediate device buffers (a full block). Generalize to many layers + persistent weights/KV-cache next.
- ✅ **Weight load → device**: **WHOLE-MODEL QUANTIZED RESIDENCY, PROVEN ON METAL** — `form/native/metal/metal_whole_tensor_residency_audit.sh` (2026-07-21, Stone 3). The entire llama3.2:3b blob (2 019 377 376 B; 3 212 749 888 weights; 2.01 GB of tensor data = 0.65 GB Q6_K + 1.36 GB Q4_K) is `mmap`ed and handed to one `MTLBuffer` with `bytesNoCopy` — **zero copies, 0.0001 s** — and every tensor is addressed inside it by offset. Dequant happens **on the GPU**, in kernels the body emits (`form-stdlib/q6k-msl.fk`, `form-stdlib/q4k-msl.fk`; no `#include <metal_stdlib>`, the f16 super-scale decoded by `fd-value`'s own arithmetic). Measured on M4 Max: a whole 25 165 824-weight tensor dequantized in **one dispatch in 0.0064 s (3.90G weights/s)** for Q6_K and **0.0032 s (7.87G w/s)** for Q4_K — against **108.0 s** for the same tensor by Form's fold on the CPU (measured, 2 runs), i.e. **~16 900×**. Gates: head tile AND **tail tile** (superblock 98 288 of 98 304 — the aporon discipline, corpus row 811) bit-exact vs Form for Q6_K and within the derived one-rounding `u*|w|` bound for Q4_K; whole-tensor dequant re-verified at both ends; the **fused** quantized-resident matvec (dequant inside the dot, 4× less device memory, no f32 tensor materialized anywhere) bit-exact vs an f32 right-fold over the dequantized buffer at the tensor's REAL width; vs Form's fp64 dot within `cols·2⁻²⁴·Σ|term|`; **all 28 `blk.N.ffn_down` tensors across BOTH quants dispatched from the one resident buffer with zero per-layer uploads**; and the emitted MSL compiled to a **`.metallib` cached across runs** by its own sha256. The tensor table for all 255 tensors comes from **one** header walk (15 s) instead of ~10 s per tensor. Still ⬜: the 58 F32 tensors are resident but no kernel consumes them yet; attention/RoPE/RMSNorm kernels still read f32 weights rather than the resident quantized bytes; `token_embd` (323 MB Q6_K) is resident but no gather kernel reads it.
- ⬜ **Parallel reductions** (perf path) — **now the measured bottleneck, and the next stone.** The fused quantized matvec is one thread per ROW with a serial right-fold down the row, which is what makes it bit-exact: **2.94 GMAC/s** (Q6_K, 8192-wide rows) and **10.14 GMAC/s** (Q4_K, 3072-wide). A whole llama3.2:3b token is ~3.21 GMAC, so this dispatch shape PROJECTS ~1–3 tok/s for the matvec work alone. A threadgroup-per-row reduction would lift it by ~2 orders of magnitude but reassociates the sum, so it needs the named-epsilon gate — that is the whole content of this row.
- 🟡 **Memory model**: KV-cache device buffers and workspace/scratch pooling are **allocated once and reused**, at real llama3.2:3b decode geometry (28 layers × 8 KV heads × 128 head-dim × 2048 seq × K/V, f32 = 470 MB) plus a scratch pool, driven across many steps by a real kernel with zero reallocation and every cached slot re-verified against a replay (gate 9 of the whole-tensor audit). ⬜ **No attention kernel consumes that cache yet** — it is a proven pool, not yet a proven cache.

## D. Algorithm layer (Form recipes, CPU-provable — smaller, mostly diffusion + serving)
- ✅ **conv2d + GroupNorm** (diffusion prerequisite) — `conv2d.fk` + `tests/conv2d-band.fk`, **verdict 15 three-way** (CPU recipe; GPU carriers next).
- 🟡 UNet (diffusion): `unet.fk` (upsample2x, downsample2x, resblock) + band **verdict 127 three-way**. VAE/full-UNet still ⬜.
- ⬜ Flash-attention (current attention materializes O(n²) scores).
- ✅ Sampling: temperature / softmax / top-k / top-p / min-p / seeded draw — `sampling.fk` + band **verdict 2097151 four-way** (in-recipe selection; beam still ⬜).
- ✅ Loss: MAE, **natural-log** (atanh series, 1e-9 vs libm), softmax **cross-entropy** — `loss.fk` + band **verdict 63 three-way**. Batch training still ⬜.
- ⬜ GroupNorm / BatchNorm; rsqrt as an explicit recipe.

## E. Backend infra
- ✅ **PTX (RTX)**: `form-ptx.fk` lane, driver-JIT -O0, gcc driver-only hosts. Four-way (verdict 127).
- ✅ **Vulkan (Android+desktop)**: matvec **bit-exact on RTX Vulkan ICD** (`native/vulkan/matvec_vk.c`, driver-only `dlopen(vulkan-1.dll)`); Form-emitted (`form-glsl.fk` → glslang `.spv`, **verdict 7 three-way**); `precise`→NoContraction keeps it unfused (do NOT run spirv-opt). Same `.spv` runs on Adreno/Mali (NDK arm64 + `libvulkan.so`; risks: FMA re-fusion, RelaxedPrecision, subnormal FTZ — all controlled). **arm64-android build PROVEN**: the exact carrier cross-compiles with NDK r27c → `matvec_vk_android` = `ELF aarch64, /system/bin/linker64, Android 24`, NEEDED libdl/libc (bionic), references `libvulkan.so` (not vulkan-1.dll). **Remaining gap: on-device RUN** — needs an actual Android device/emulator (none on this host; if connected via adb: install platform-tools, push `matvec_vk_android`+`.spv` to /data/local/tmp, run). Next: f16/bf16 GLSL, the bigger layers as compute shaders.
- ✅ **Metal (Mac)**: most complete (matvec, affine, mlp, attn, block, llama) — but Mac-only proof (off-Mac the audits SKIP). **+ real-weight residency** (`native/metal/metal_weight_residency_audit.sh`, one f32 tile) and **+ whole-MODEL quantized residency** (`native/metal/metal_whole_tensor_residency_audit.sh`: the 2 GB blob mapped zero-copy, Q6_K/Q4_K dequant on the GPU, all 28 ffn_down layers dispatched from one buffer, `.metallib` cached across runs) — see §C.
- ⬜ **gcc-clean fkwu** on Windows (emitter emits socket shims before def → gcc rejects; clang-built today). Upstream `hati-os-kernel-emit.fk` fix.
- ⬜ `hati-os-targets.fk` rows for `windows-x64-cuda` and `android-vulkan` (each needs the targets-band extension: count + verdict bit + artifact row).

## Active lanes (who's on what)
- **RTX climb**: ✅ 11 kernels (verdict 8191, four-way) + **a FULL transformer block end-to-end on the GPU, bit-exact** (kernel-graph). NEXT: stack N blocks (the kernel-graph generalizes) → a whole tiny model forward; add projections/gamma-beta for the exact tb-block; MHA/causal/KV.
- **Android/Vulkan**: ✅ matvec proven (RTX Vulkan) + Form-emitted + arm64-android cross-compiled. NEXT: on-device run (needs device); f16/bf16 GLSL; FFN/attention compute shaders.
- **Mac/quantized residency**: ✅ whole-model quantized residency + GPU dequant (Q6_K and Q4_K), multi-layer dispatch, pooled KV/workspace, cached `.metallib`. NEXT: a threadgroup-per-row reduction under a named-epsilon gate (the matvec is currently serial per row); attention/RoPE/RMSNorm reading the resident quantized bytes; a gather kernel for `token_embd`.
- **Diffusion**: ✅ conv2d/groupnorm recipe. NEXT: GPU carriers (PTX/MSL/GLSL) for conv2d.
- **Serving/Training**: ✅ sampling (top-k/p, temperature). NEXT: loss functions (cross-entropy + log) — agent.

## Proven milestones (RTX/PTX lane)
- **11 kernels** bit-exact on RTX 4070, driver-only, `form-ptx` band **verdict 8191 PASS-4WAY**: matvec f32/f16/bf16, affine-train, gelu(Taylor), FFN, softmax, attention, layernorm, rmsnorm, residual.
- **Full transformer block** end-to-end on GPU, bit-exact (`form_cuda_ptx_block_host.c`, 12-launch kernel-graph).
- **Tiny transformer FORWARD → logits** end-to-end on GPU, bit-exact (`form_cuda_ptx_model_host.c`: embed → N×block → final-ln → logits; 3 layers/144, 4 layers/576).
- **AUTOREGRESSIVE GENERATION** on GPU, bit-exact (`form_cuda_ptx_generate_host.c`: greedy loop, growing seq; prompt [1,2,3] → `13 21 16 12 12 9 13 20`, token-id seq matches CPU oracle, final logits bit-exact). A form-native transformer GENERATES.
- **+2 kernels for the EXACT tb-block**: projection (matvec+bias) + gamma/beta affine — `form-ptx-block` band **verdict 3 PASS-4WAY** (13 kernels total now). Compose the exact block next.
- Ideas: `04d35058-...` (lane), `0702a906-...` (carrier).
