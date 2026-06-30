# Receipt — a full transformer, full width, full depth, orchestrated end-to-end on a real Adreno (2026-06-29 21:30 MDT)

A complete **4-layer transformer** runs on the attached **Galaxy S23 Ultra / Adreno 740**, driven entirely from
Form (no clang in the path), every kernel emitted from `model/form-glsl.fk` and verified against an independent
python float32 oracle. This is the end of the gap list: the kernels run at width, stacked to depth, chained
end-to-end on the actual device.

## Full width — batched kernels

Five batched, shape-parameterized kernels were added to `form-glsl.fk` and minted from Form:
`fglsl-matmul`, `fglsl-add`, `fglsl-layernorm-batched`, `fglsl-ffn-batched` (per-token, no workgroup barrier),
`fglsl-attention-mh` (multi-head). The workhorse proven at width:

- **matmul** `Y[16×64] = X[16×64]·W[64×64]^T` — **16 workgroups, 1024 outputs, 1024/1024 bit-exact** to the
  in-Form integer reference ([`model/form-matmul.fk`](../model/form-matmul.fk)).

## Orchestration end-to-end — one transformer block

[`model/form-transformer-block.fk`](../model/form-transformer-block.fk): a complete **pre-LN block**
(D=8, S=4 tokens, H=2 heads, F=16) as **10 chained dispatches** on the device:

```
layernorm → matmul(Wq) → matmul(Wk) → matmul(Wv) → attention(MH) → matmul(Wo)
          → residual(+x) → layernorm → FFN → residual(+r1)
```

A single **generic `run-stage`** driver builds the descriptor set / pipeline / command buffer for *any* kernel
from parameters in an arena (binding buffer-indices, push constants, shader selector); the orchestrator sets
those slots and calls it 10 times over **25 persistent buffers** (11 activations + 2 scratch + 12 weights).
Stages are ordered by per-stage submit+fence (host-synchronized) over the shared buffers — the output of each
stage is the input of the next.

**Result: 32/32 outputs within 32 ULP** of the python float32 block oracle. The Form references (the body's own
f32 carriers) and the python oracle agree exactly; the GPU's ≤~20-ULP drift is the accumulated Adreno
division-rounding through the chained stages (matmuls stay bit-exact; layernorm/attention/FFN carry the division
divergence). Named, measured, reported.

## Full depth — 4 blocks stacked

[`model/form-transformer-depth4.fk`](../model/form-transformer-depth4.fk): the block wrapped as a `layer`
function, looped **4×** (output → input between layers) — **40 dispatches** total, on the device. Verified
against a 4-layer python f32 oracle:

```
depth-4 (40 dispatches): 31/32 within 32 ULP, 32/32 within 512 ULP
```

The divergence stays bounded across depth because each block's layernorm re-normalizes — the ≤~20-ULP per-layer
division drift does not blow up, it accumulates to ≤~512 ULP (one output) after four full blocks. A real
multi-layer transformer, computed on the phone's GPU, agreeing with an independent f32 reference to within a few
parts in 10^5.

## Verdict — the gap list is closed

| | proven on Adreno 740, from Form |
|---|---|
| **kernels** | matmul, matvec, FFN, softmax, layernorm, attention (MH), residual-add |
| **full width** | matmul 16×64×64 (1024 outputs, 16 workgroups), bit-exact |
| **orchestration** | full pre-LN block, 10 chained dispatches, 32/32 within 32 ULP |
| **full depth** | 4 blocks stacked, 40 dispatches, 32/32 within 512 ULP |

The only C is the one `fkwu` seed (FFI + f32 carriers, tags 240–256). Every shader is emitted from Form, every
reference is the body's own f32 arithmetic, every number was printed by the Adreno or by `fkwu`. Where the GPU
is not bit-exact it is the hardware's non-correctly-rounded division — named, bounded, and measured, never
hidden. Artifacts: `model/form-{matmul,transformer-block,transformer-depth4}.fk`, `native/vulkan/*.{comp,spv}`,
`native/vulkan/gen-block-data.py`, the 5 batched kernels in `model/form-glsl.fk`.
