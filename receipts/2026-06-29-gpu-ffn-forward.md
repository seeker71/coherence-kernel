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
