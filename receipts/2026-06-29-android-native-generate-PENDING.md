# Receipt — Rung 3: KV-cache generation reference validated; on-device blocked on a run-stage leak (PENDING)

**Status: PENDING — honest, not faked.** The full greedy-generation **reference** is built and validated
**bit-exact against ollama** (the named oracle). The device generation recipe (real attention: Wq/Wk, RoPE,
causal softmax over a KV cache) is built and its non-attention path runs, but a **Vulkan object leak in the
generic `run-stage`** blocks the attention dispatches under Rung 3's memory footprint. Token agreement on-device
is therefore **not yet witnessed**. Pending is the truth.

## What IS validated (the Rung 3 gate, off the critical-path oracle)

A from-scratch generation pipeline — **byte-level BPE tokenizer built from the GGUF** (vocab + merges),
**interleaved RoPE** (base 500000, llama.cpp layout), **causal GQA** (32 q / 8 kv heads, kvh = h/4), and a
**KV cache** — reproduces ollama's greedy decode **exactly**:

```
prompt : "The capital of France is"   ->  ids [128000, 791, 6864, 315, 9822, 374]
numpy-on-GGUF greedy : " Paris. The Eiffel Tower is located in Paris."
ollama  greedy (temp 0, repeat_penalty 1.0, top_k 1) : " Paris. The Eiffel Tower is located in Paris."
                                                          ^ identical, token-for-token
```

This is the strong external witness the plan calls for: the same arithmetic as llama.cpp, agreeing with the
live oracle. Reference + tokenizer: [`native/vulkan/gen-llama-generate-reference.py`](../native/vulkan/gen-llama-generate-reference.py).

## What is BUILT for the device (and where it stops)

[`model/form-llama-generate-rung3-WIP.fk`](../model/form-llama-generate-rung3-WIP.fk) — the decode recipe:
per-token embedding lookup via `c_memcpy` from resident tied-embedding tiles, per-layer weight reload, the GPU
matmuls (Wq/Wk/Wv/Wo/Wg/Wu/Wd) from Rung 2, two new kernels (`fglsl-rope`, `fglsl-decode-attn` in
[`model/form-glsl.fk`](../model/form-glsl.fk); minted SPIR-V 1.3), a combined K+V cache, and the greedy decode
loop with a tiled LM head. The embedding lookup, weight load, and matmul path all run on-device.

## Root cause of the block (definitively isolated on-device)

Layer-by-layer probing established: the **5th GPU dispatch in the rung3 sequence silently fails to write its
output buffer**, and so do all after it — *regardless of shader, push constants, bindings, or `sel`* (the
matmul shader with proven bindings fails as the 5th dispatch just as the new kernels do). Rung 2's forward runs
**160** dispatches without issue. The difference is **headroom**: the generic `run-stage` **creates a fresh
descriptor pool, command pool, pipeline, shader module, pipeline/descriptor-set layout, and fence on every
dispatch and never destroys them**. Rung 2 (20 buffers) stays under the driver's budget; Rung 3 holds **~1.3 GB
resident** (8 × 131 MB tied-embedding tiles + per-layer weights + KV cache), so the leaked objects exhaust the
budget after the 4th allocation and subsequent `vkAllocate*`/pipeline creates fail (unchecked return → null →
dispatch is a silent no-op).

The fix is per-dispatch cleanup or pool reuse. Adding `vkDestroyDescriptorPool` &c. to `run-stage` hit a
**second seam**: passing a Vulkan handle back through the Form FFI to a `vkDestroy*` aborts with Android's
*"pointer tag … was truncated"* (tagged-pointer / TBI) check — even though the same handles pass into the
`vkCreate*`/`vkCmd*` calls fine. So both the leak and the destroy-FFI tagging need resolving.

## The two ways forward (named, not hand-waved)

1. **Reuse, don't recreate** — create descriptor pool + command pool once in setup; `vkResetDescriptorPool` /
   `vkResetCommandPool` per dispatch; cache the 6 pipelines/shader-modules/layouts by shader id. Eliminates the
   leak without any `vkDestroy*` (dodges the tagged-pointer seam). *Preferred.*
2. **Fix the destroy-FFI tagging** — pass handles to `vkDestroy*` without the Form int round-trip mangling the
   top-byte tag (e.g. a carrier that reads the handle straight from the arena slot and calls with TBI intact).

Either unblocks the attention dispatches; the validated reference above is then the gate the device output must
match (greedy token stream == ollama). This is also a latent robustness fix for **Rung 2** (it survives only by
staying under the leak budget).

## Honest floor

- Rungs 1 (FFN sublayer) and 2 (full forward → correct logits) **are** witnessed on the Adreno
  ([`…-gguf-layer.md`](2026-06-29-android-gguf-layer.md), [`…-gguf-forward.md`](2026-06-29-android-gguf-forward.md)).
- Rung 3 is **not** — the on-device greedy token stream has not been produced, so no token-agreement claim is
  made. The tokenizer + generation arithmetic are validated *off-device* against ollama; the on-device witness
  waits on the run-stage leak fix.
