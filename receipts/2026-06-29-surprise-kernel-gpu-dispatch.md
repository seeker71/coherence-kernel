# Receipt — surprise-kernel runs on the M4 Max GPU, bit-exact (2026-06-29)

The active-inference attention gate now runs on real GPU silicon, not just emits a shader.

## What was observed

`surprise-kernel`'s `sk-error-sum` (the per-frame predict→surprise computation, four-way 511) was dispatched on
the **Apple M4 Max GPU (Metal)** via an MSL twin (the same carrier pattern as `gpu-ffn-forward`'s `jte-mlp-fwd-msl`),
and compared to the CPU / four-way-recipe result:

```
GPU: Apple M4 Max
  quiet [102,50,800]:  GPU=8   CPU=8   BIT-EXACT  -> ATTEND=0   (GPU idle, ride the prediction)
  spike [101,900,803]: GPU=854 CPU=854 BIT-EXACT  -> ATTEND=1   (wake full perception)
ALL BIT-EXACT
```

The serial-downward |actual−expected| reduce (no FMA) matches the recipe exactly, so GPU == CPU == the four-way
verdict, and the salience gate (threshold 50) fires correctly on both frames. The per-frame surprise that keeps the
continuous loop cheap-and-awake is now a **real GPU dispatch**.

## Honest floor

- The MSL kernel was **hand-ported** from `sk-error-sum` for this witness (the recipe is the body; Metal is the
  carrier, dropped by the byte-gate exactly as clang is). The promotable next step is an `sk-msl` *emitter* — the
  Metal sibling of the already-four-way `sk-glsl`/`sk-ptx` — so the recipe emits its own MSL, no hand port.
- Integer vectors → exact on both lanes by construction. The float lane (the matvec/softmax kernels) crystallizes
  by the same door; its bit-exact GPU run at width is the FFN-forward witness's territory.

This closes the "emits a shader but no GPU run" gap for `surprise-kernel`: it emits GLSL+PTX (four-way) AND runs
bit-exact on this M4 Max GPU.
