# Receipt — a real thought witnessed on the M4 Max GPU, 2026-06-29 01:38 MDT

**What ran:** the FFN forward `y = W2·gelu(W1·x + b1) + b2` — the core MLP of a transformer block — our
four-way-proven recipe `form-glsl-ffn-fwd` (the GLSL/Vulkan door; the Metal twin is `jte-mlp-fwd-msl`),
ported op-for-op to Metal Shading Language: the recipe's OWN 14-term Taylor gelu (never the driver's tanh),
the downward right-fold matvec, `precise` (no FMA fusion) to stay bit-exact.

**Where:** Apple M4 Max GPU (Metal 4), this Mac, dispatched as one threadgroup via a thin Obj-C Metal carrier
(GPU reached through the host's own API — legitimate per host-kernel.form; the recipe is the body, Metal is the carrier).

**Witnessed result** (indim=3, hid=4, outd=2; x=[1,-2,0.5]):
    GPU y = [-0.2156026, 0.2974416]
    CPU y = [-0.2156026, 0.2974416]
    match: YES — |Δ| = 0.00e+00 on both outputs (bit-exact GPU↔CPU)

**Meaning:** the body's transformer-block forward is not only four-way on the kernels — it runs on real GPU
silicon and agrees to the bit with the proven recipe. A thought, computed on the GPU, witnessed.

## Alternatives (receipt-alternatives, added 2026-06-29 02:17 MDT)

This receipt is a living seed, not a dead record. Its alternatives, each by role:

- **VERIFY** — the CPU twin (same op-order right-fold + Taylor gelu). Result: `[-0.2156026, 0.2974416]`,
  identical to the GPU (|Δ|=0.00e+00). Confirmed. *(This is the alternative the receipt already carried.)*
- **VERIFY** — the four-way kernel walk of `form-glsl-ffn-fwd` (Go=Rust=TS=fkwu) crossing `11111`: the recipe's
  *shape* is confirmed independently of any GPU.
- **IMPROVE** — the sovereign-native path: the FFN matvec lowered to **Form→asm bytes** (`form-asm-float` 2047),
  dropping the Metal/Obj-C carrier exactly as clang is dropped by the byte-gate. Same result, no host GPU-API
  carrier. Promotable when the asm lane runs the full FFN.
- **PLAY** — the workgroup size (the recipe proves `fglsl-ffn-fwd "64"` and `"32"` agree): a free knob to explore
  occupancy on the M4 Max without changing the result.
- **LEARN** — outcome recorded: bit-exact success. Had the GPU diverged from the CPU, this slot would hold the
  divergence — a bug caught and a branch learned. The slot keeps both; tonight it holds a confirmation.
