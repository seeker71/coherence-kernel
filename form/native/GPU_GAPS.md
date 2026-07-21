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
- ✅ **Weight load → device**: **WHOLE-MODEL QUANTIZED RESIDENCY, PROVEN ON METAL** — `form/native/metal/metal_whole_tensor_residency_audit.sh` (2026-07-21, Stone 3). The entire llama3.2:3b blob (2 019 377 376 B; 3 212 749 888 weights; 2.01 GB of tensor data = 0.65 GB Q6_K + 1.36 GB Q4_K) is `mmap`ed and handed to one `MTLBuffer` with `bytesNoCopy` — **zero copies, 0.0001 s** — and every tensor is addressed inside it by offset. Dequant happens **on the GPU**, in kernels the body emits (`form-stdlib/q6k-msl.fk`, `form-stdlib/q4k-msl.fk`; no `#include <metal_stdlib>`, the f16 super-scale decoded by `fd-value`'s own arithmetic). Measured on M4 Max: a whole 25 165 824-weight tensor dequantized in **one dispatch in 0.0064 s (3.90G weights/s)** for Q6_K and **0.0032 s (7.87G w/s)** for Q4_K — against **108.0 s** for the same tensor by Form's fold on the CPU (measured, 2 runs), i.e. **~16 900×**. Gates: head tile AND **tail tile** (superblock 98 288 of 98 304 — the aporon discipline, corpus row 811) bit-exact vs Form for Q6_K and within the derived one-rounding `u*|w|` bound for Q4_K; whole-tensor dequant re-verified at both ends; the **fused** quantized-resident matvec (dequant inside the dot, 4× less device memory, no f32 tensor materialized anywhere) bit-exact vs an f32 right-fold over the dequantized buffer at the tensor's REAL width; vs Form's fp64 dot within `cols·2⁻²⁴·Σ|term|`; **all 28 `blk.N.ffn_down` tensors across BOTH quants dispatched from the one resident buffer with zero per-layer uploads**; and the emitted MSL compiled to a **`.metallib` cached across runs** by its own sha256. The tensor table for all 255 tensors comes from **one** header walk (15 s) instead of ~10 s per tensor. Still ⬜: the 58 F32 tensors are resident but no kernel consumes them yet; attention/RoPE/RMSNorm read f32 state and F32 gains IN PLACE from the resident buffer (Stone 4) — no quantized weights feed them because llama3.2:3b stores its norm gains unquantized; ~~`token_embd` (323 MB Q6_K) is resident but no gather kernel reads it~~ — **closed by Stone 4**: the embedding gather is `form_q6k_dequant_f32` at flat offset `id·3072` (bit-exact vs Form on all 3072 weights, gate 2), and the same tensor is the tied UNEMBEDDING via the fused matvec at 128 256 × 3072.
- 🟡 **Parallel reductions** (perf path) — **the diagnosis in this row was INCOMPLETE, and Stone 4 corrected it by measuring.** It read: the fused matvec is one thread per ROW with a serial right-fold, that is what makes it bit-exact, and lifting it needs a named-epsilon reassociation. Profiling a real full-width llama3.2:3b token PER TENSOR SHAPE (M4 Max, `FORM_PROFILE=1`, one command buffer per op) says the serial fold was not the wall:

  | shape (rows × cols) | MACs | ms/dispatch | implied rate |
  |---|---|---|---|
  | Q4_K 1024 × 3072 | 3.1 M | 2.618 | 1.2 GMAC/s |
  | Q4_K 3072 × 3072 | 9.4 M | 2.648 | 3.6 GMAC/s |
  | Q4_K 8192 × 3072 | 25.2 M | 2.607 | 9.7 GMAC/s |
  | Q4_K 3072 × 8192 | 25.2 M | 6.657 | 3.8 GMAC/s |
  | Q6_K 128256 × 3072 | 394.0 M | 8.576 | 45.9 GMAC/s |

  Eight times the work in the same time (rows 1–3); the same work in 2.6× the time when rows and columns swap (rows 3–4). Cost tracks the COLUMN count — one thread's serial depth — and barely notices the ROW count. The dispatch was **latency-bound with the machine idle**, not throughput-bound, and the single largest recoverable share needed **no epsilon at all**: a `.concurrent` compute encoder that stops making q/k/v (and gate/up) wait for each other bought **1.41× end-to-end with the generated token ids BIT-IDENTICAL**. The reassociating half is now done too and gated: `form-stdlib/qk-matvec-split.fk` splits each row into `parts` contiguous chunks (rows×parts threads) and folds the partials downward, under the derived bound **|split − attestant| ≤ (cols + ⌈cols/parts⌉ + parts)·u·Σ|term|**. At parts=1 the split kernel **IS** the attestant bit for bit (gate 8); at parts=16 on a real 3072×8192 Q6_K tensor the measured deviation was **0.000e+00** (gate 9), and the split path generates **the same token ids** as the attestant (gate 10) for **2.03× end-to-end**. The attestant is not deleted and is re-run every time (corpus row 810).

  **STONE 5 (2026-07-21) closed the `simd` half of this row and REFUTED the access hypothesis that was going to be the next stone.** The standing plan was a bit-exact structure-of-arrays re-layout, on the theory that Q6_K's 210-byte superblock stride (210 = 2·3·5·7, not a power of two) rotates every superblock through a different alignment and destroys coalescing. llama3.2:3b carries the **same two shapes in both quants**, which makes alignment a free controlled experiment — Q4_K's block is 144 B and 16-aligned, Q6_K's is 210 B and aligned to nothing. Best of 5, one thread per row, serial fold:

  | rows × cols | Q4_K (144 B, aligned) | Q6_K (210 B, misaligned) | |
  |---|---|---|---|
  | 1024 × 3072 | 3.848 ms | 0.938 ms | Q6_K **4.10× faster** |
  | 3072 × 8192 | 6.442 ms | 3.466 ms | Q6_K **1.86× faster** |

  The misaligned quant is the faster one at both sizes, so alignment is not the cost — and a lane-interleaved partition (perfect coalescing) measured only 0–18% against the contiguous one, which is what an ALU-bound dispatch looks like. The cost is per-weight arithmetic: `q4k_w` decodes an f16 super-scale **twice** per weight and `q6k_w` **once**, and that decode is a LOOP of float multiplications. Two things followed, and they have different epsilon status:

  - **The hoist — free, and bit-exact.** `d` (and Q4_K's `dmin`) are constants of the 256-weight superblock; computing them once per crossing instead of once per weight gives the identical f32 and touches no association. Serial fold, hoisted vs not: 1024×3072 Q4_K **3.848 → 1.103 ms (3.49×)**, 3072×8192 Q4_K **6.442 → 2.389 ms (2.70×)**, 3072×8192 Q6_K **3.466 → 2.041 ms (1.70×)**. Gate 8b of `metal_first_token.sh` demands the hoisted kernel at parts=1 be the untouched attestant **bit for bit on all 3072 rows** — and it is.
  - **The SIMD fold — reassociates, and the existing bound already covers it.** `form-stdlib/qk-matvec-lane.fk` gives one row to one SIMD group (32 lanes, lane-interleaved, hoisted) reduced by `metal::simd_sum`, in ONE dispatch with **no shared scratch** — so `q/k/v` and gate/up become concurrent again, which the `part`-buffer split path had forfeited. **No new derivation was needed**: Metal does not specify simd_sum's tree, but any association of 32 terms has depth ≤ 31 < 32, so `(cols + ⌈cols/32⌉ + 32)·u·Σ|term|` — qk-matvec-split's bound at parts=32 — bounds every tree it could be using. Measured deviation on a real 3072×8192 Q6_K tensor: **0.000e+00** (gate 9b), same token ids as the attestant (gate 11).

  **End-to-end 4.647 → 8.317 tok/s** (12 tokens, prefill included); decode-only 6.892 → 12.227; **4.77× vs the attestant end-to-end**, 1.79× vs Stone 4's split path. Against the external denominator (ollama on the same machine/model/blob, quoted, 150-token sample): decode went from **22.9× behind to 12.9× behind** 157.83 tok/s.

  **Measured and REJECTED, with its number:** `nr0` register blocking (one SIMD group folding several rows so the activation load amortizes) — the shape llama.cpp's `kernel_mul_mv_q6_K_f32_impl` uses, read for shape only (MIT). Built on the lane kernel it is **slower at every shape and monotonically worse in nr0** (3072×8192 Q6_K: simd+hoist 0.532 ms, nr0=2 0.555, nr0=4 0.683, nr0=8 0.865). It amortizes an activation load this kernel was never bound by. A shape borrowed from a kernel with a different binding constraint transfers nothing.

  Still ⬜: **threadgroup memory** is still untouched (this row's reduction is cross-lane, not cross-threadgroup); the remaining ~10× to llama.cpp's inner loop, which factors `d·sc` OUT of the sum and accumulates in QUANTIZED space, decodes with bitmasks instead of division, and vectorizes as `float4` — all three change the association materially and need an epsilon strictly larger than the one above; the `brimwidth` question (corpus row 814) is still open, though 3072×8192 now runs at 47 GMAC/s against 25 GB/s of a ~400 GB/s machine, so this shape is still nowhere near either wall.
- ✅ **Memory model**: KV-cache device buffers and workspace/scratch pooling are **allocated once and reused** — and as of Stone 4 the cache is a CACHE, not only a pool: `form_gqa_decode_f32` (`form-stdlib/llama-decode-msl.fk`) attends the current query over the cached prefix, and the k/v projections write their own slot (the matvec's output buffer is bound at `(layer·maxpos + pos)·KVD`), so no position's k/v is ever recomputed. 56.7 MB of activation + KV state for a 256-position window, allocated once for a whole run, zero reallocation; a second run from a freshly zeroed pool reproduces the same ids (gate 5 of `metal_first_token.sh`).

## D. Algorithm layer (Form recipes, CPU-provable — smaller, mostly diffusion + serving)
- ✅ **conv2d + GroupNorm** (diffusion prerequisite) — `conv2d.fk` + `tests/conv2d-band.fk`, **verdict 15 three-way** (CPU recipe; GPU carriers next).
- 🟡 UNet (diffusion): `unet.fk` (upsample2x, downsample2x, resblock) + band **verdict 127 three-way**. VAE/full-UNet still ⬜.
- ⬜ Flash-attention. The DECODE path no longer materializes O(n²): `form_gqa_decode_f32` scores one query against the cached prefix, O(n) per token, one thread per query head. The O(n²) shape remains in the whole-sequence kernels (`jte-gqa-blk-attn-causal`) and in prefill, which Stone 4 runs as n decode steps rather than a batched pass.
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
- **Mac/quantized residency**: ✅ **A REAL llama3.2:3b TOKEN, form-native** — `form/native/metal/metal_first_token.sh`, VERDICT PASS 10 gates: full width (28 layers, d=3072, 24/8 GQA, dff=8192, vocab 128 256), real tokenizer ids in and out, every op a body-emitted kernel off the one resident quantized buffer, no f32 tensor materialized anywhere. `"The capital of France is"` → `" Paris. The capital of Italy is Rome. The capital of"`. **3.53 tok/s end-to-end** (12 tokens incl. prefill), 4.78 tok/s decode-only, split path; 1.74 / 2.60 on the bit-exact attestant. NEXT: prefill as a batched pass instead of n decode steps; a true threadgroup reduction; the `brimwidth` measurement (corpus row 814).
- **Diffusion**: ✅ conv2d/groupnorm recipe. NEXT: GPU carriers (PTX/MSL/GLSL) for conv2d.
- **Serving/Training**: ✅ sampling (top-k/p, temperature). NEXT: loss functions (cross-entropy + log) — agent.

## Proven milestones (RTX/PTX lane)
- **11 kernels** bit-exact on RTX 4070, driver-only, `form-ptx` band **verdict 8191 PASS-4WAY**: matvec f32/f16/bf16, affine-train, gelu(Taylor), FFN, softmax, attention, layernorm, rmsnorm, residual.
- **Full transformer block** end-to-end on GPU, bit-exact (`form_cuda_ptx_block_host.c`, 12-launch kernel-graph).
- **Tiny transformer FORWARD → logits** end-to-end on GPU, bit-exact (`form_cuda_ptx_model_host.c`: embed → N×block → final-ln → logits; 3 layers/144, 4 layers/576).
- **AUTOREGRESSIVE GENERATION** on GPU, bit-exact (`form_cuda_ptx_generate_host.c`: greedy loop, growing seq; prompt [1,2,3] → `13 21 16 12 12 9 13 20`, token-id seq matches CPU oracle, final logits bit-exact). A form-native transformer GENERATES.
- **+2 kernels for the EXACT tb-block**: projection (matvec+bias) + gamma/beta affine — `form-ptx-block` band **verdict 3 PASS-4WAY** (13 kernels total now). Compose the exact block next.
- Ideas: `04d35058-...` (lane), `0702a906-...` (carrier).
