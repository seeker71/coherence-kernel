# Receipt — FFN forward (the transformer MLP) on a real Adreno, bit-exact, no clang (2026-06-29 20:35 MDT)

**Closes Gap 1 for the FFN kernel** on the attached **Galaxy S23 Ultra / Adreno 740**: `y = W2·gelu(W1·x + b1) + b2`
— the MLP of a transformer block, gelu nonlinearity and all — dispatched on the GPU from Form and verified
**bit-exact** against an independent Form reference *and* a python float32 oracle.

## The keystone: f32 arithmetic as Form

Floating kernels can't be checked with integer math, and fkwu can't even parse f32 literals. So the seed gained
four tiny carriers — `c_fadd`/`c_fsub`/`c_fmul`/`c_fdiv` — operating on **u32 bit patterns** (IEEE-754 mandates
correctly-rounded non-fused +/−/×/÷, identical on host CPU and Adreno). Validated bit-exact vs a float32 oracle
(120/120 add/sub/mul cases; `1.0/3.0 = 0x3eaaaaab`). On top of them, the **gelu** is rebuilt *in Form* — the
shader's own 14-term Taylor `fexp` + `fgelu`, same op order, constants as f32 bit patterns — and validated
bit-exact across 10 values (`fgelu(1.0)=0x3f54...`, negatives, large magnitudes). The reference is the body's
own arithmetic, not a baked table.

## What ran

FFN with `indim=4, hid=8, outd=3`, fractional weights loaded from files via `c_memcpy`. The shader
([`native/vulkan/ffn.spv`](../native/vulkan/ffn.spv), emitted from `fglsl-ffn-fwd`, `NoContraction` preserved)
runs both matvecs + the barrier'd gelu in one workgroup. The Form recipe ([`model/form-ffn.fk`](../model/form-ffn.fk))
computes the same FFN — downward fold, `fgelu`, downward fold — and compares.

```
GPU Adreno 740 :  Y = [3209012134, 1068375604, 1072776150]   (u32 f32 bits)
Form reference :  Y = [3209012134, 1068375604, 1072776150]   -> 3/3 match
python f32 oracle: Y = [3209012134, 1068375604, 1072776150]
```

**Three independent implementations of the FFN agree to the bit.** The Form reference and the GPU shader are
separate codepaths (Form carriers vs Adreno SPIR-V) implementing the identical sequence of correctly-rounded f32
ops; the python oracle is a third. Bit-exact, `|Δ|=0` on every output.

## Meaning

The transformer MLP — its nonlinear heart — now runs on the phone's GPU under Form's orchestration and agrees
bit-for-bit with the body's own f32 reference. The gelu is not approximated against a tolerance; it is reproduced
exactly, because the body can now do f32 arithmetic itself. This is the template for the remaining floating
kernels (softmax, layernorm, attention): emit GLSL → mint → dispatch → reference in Form via the f32 carriers →
compare bit-exact.

Artifacts: [`model/form-ffn.fk`](../model/form-ffn.fk), [`native/vulkan/gen-ffn-weights.py`](../native/vulkan/gen-ffn-weights.py),
`native/vulkan/ffn.spv`/`.comp`, carriers `c_fadd`/`c_fsub`/`c_fmul`/`c_fdiv` (tags 252–255) + `c_memcpy` (251)
in the `flt-ops` manifest. Only C is the one `fkwu` seed; the FFN math and its reference are Form.
