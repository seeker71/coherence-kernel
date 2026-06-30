# Rung 3 handoff — continue here

**Goal:** close NL→form-cli→native-oracle on the Galaxy S23 / Adreno 740, witnessed by Rung 9's capstone
receipt (`receipts/2026-06-29-android-native-oracle-e2e.md`). Plan: `receipts/2026-06-29-android-native-oracle-PLAN.md`.
Oracle: llama.cpp/ollama. No rung marked done on toy inputs or on the Mac.

## Verified state
- **Rungs 1–2 witnessed** on device: `receipts/2026-06-29-android-gguf-{layer,forward}.md`. Rung 2 = full
  16-layer llama3.2:1b forward, GPU argmax **16309 == oracle**.
- **Rung 3 generation reference validated bit-exact to ollama** (`native/vulkan/gen-llama-generate-reference.py`):
  `"The capital of France is"` → `" Paris. The Eiffel Tower is located in Paris."`; prompt ids `[128000,791,6864,315,9822,374]`.
- Recipe: `model/form-llama-generate-rung3-WIP.fk`. Kernels: `model/form-glsl.fk`. Device:
  `/data/local/tmp/formvk` (serial `R5CW20DK17A`; weights `r2/`+`r3/`; run with `FORM_KERNEL_STACK_MB=2048`;
  mint SPIR-V with `glslangValidator --target-env vulkan1.1` → **SPIR-V 1.3**, must match the working kernels).

## Fixed this session
The run-stage **Vulkan object leak**: pools/cmdbuf/fence created once in setup, reset/reused per dispatch;
6 pipelines cached by `sel` (`mkpipe`, cache at arena `8000+sel*32`: +0 setLayout, +8 pipeLayout, +16 module,
+24 pipeline; run-stage selects via scratch `8200`). **sel 0/1/3/4 (matmul/rmsnorm/add/rope) dispatch a full
forward.** Also a latent robustness fix for Rung 2.

## THE root cause to fix (one bug, two symptoms)
The two `fexp`-bearing kernels — **sel-2 (silu-mul)** and **sel-5 (decode-attn)** — **do not produce correct
output through the cached-pipeline run-stage**, while the four non-`fexp` kernels do. Evidence:
- sel-5 decode-attn: dispatch is a silent no-op (sentinel survives); yet all 4 `vkCreate*` for sel 5 return
  **VK_SUCCESS**, and the *matmul shader placed at sel-5's slot also fails* → it's the cache slot / reused
  pipeline, not the shader.
- sel-2 silu: in isolation it **completes** (no hang) but `h[0]=0` for `silu(1.0)*1.0` (should be 0.731) → it
  also doesn't write correctly. In the full forward block it appears to hang (GPU never signals the fence).

**Strong hypothesis:** a **cached/reused compute pipeline that contains the `fexp` function (loops + a large
private array) faults or mis-binds on the Adreno**, while the simple kernels are fine. rung2 ran these same
shaders fine when each dispatch created a *fresh* pipeline (the old leaking run-stage).

## CORRECTION (further debugging — read this)
- Recreating the sel-2/sel-5 pipelines **per-dispatch did NOT fix them** (the recipe currently has that change
  at the top of `run-stage`; it can be reverted — it neither helped nor hurt). So "cached vs fresh pipeline" is
  **not** the root.
- The "silu writes 0" finding was from a **coherency-confounded isolation test**: I set `gate[0]` with a host
  `c_u32_set` on the mapped buffer and silu read 0 → `silu(0)*u = 0`. Host writes to the mapped SSBO may not be
  GPU-visible the way I assumed, so that test is **unsound**. In the *real* forward `gate` is GPU-written by the
  Wg matmul, so silu's real-input behavior is **unverified**. Do NOT trust the "silu broken" conclusion.
- What IS solid: decode-attn (sel 5) sentinel-survives (no-op) with VK_SUCCESS on all creates; block L0 with the
  `gqacp` V-copy attention still hung (~84s) — but the hung stage was inferred from an earlier decode-attn-garbage
  run, not re-confirmed with gqacp. **Re-bisect block L0 cleanly with the gqacp pre** (which gives correct
  `gate`) before blaming silu.

## Next moves (in order)
1. **Minimal fix to try first:** in run-stage, **do NOT cache** the sel-2 and sel-5 pipelines — create those
   two per-dispatch (as the old run-stage did) while keeping sel 0/1/3/4 cached. If that makes silu write 0.731
   and decode-attn write V, the whole forward unblocks. (Watch the leak: only 2 kernels × dispatches/forward —
   may still fit, or destroy just those two per-dispatch.)
2. **Name the fault:** host build with the Vulkan **validation layer** (desktop libvulkan, same recipe shape) —
   it will say exactly why the cached `fexp` pipeline's dispatch is invalid on the Adreno.
3. **Reliable fallback:** in-Form `fexp` — add a `c_fexp` carrier to `runtime/fkwu-uni.c` (recompile host +
   cross-compile `aarch64-linux-android34-clang -ldl`), do silu and the attention softmax in Form via the f32
   carriers, leaving the matmuls on GPU. A GQA V-copy (`gqacp`, already in the recipe) handles pos-0 attention
   with no softmax — witness argmax 16309 first, then multi-token.

## Then
Witness device greedy stream == ollama, ship `receipts/2026-06-29-android-native-generate.md` (Rung 3), continue
Rungs 4–9 per the PLAN. Discipline: pending is honest; verify the whole vector, not index 0; name the tolerance.
