# Receipt — the transformer kernels on a real Adreno: softmax, layernorm, attention, kernel-chaining (2026-06-29 20:55 MDT)

**Closes the remaining Gap-1 kernels and the Gap-3 chaining primitive** on the attached **Galaxy S23 Ultra /
Adreno 740**, all driven from Form (no clang in the path), each verified against an independent Form f32
reference *and* a python float32 oracle. Every kernel's reference is the body's own f32 arithmetic
(`c_fadd/fsub/fmul/fdiv/fsqrt` carriers), not a baked table.

## The kernels, witnessed on metal

| kernel | shape | result vs Form reference | divergence (named) |
|---|---|---|---|
| **matvec** (linear) | 192×192, 3 workgroups, file-loaded | **192/192 bit-exact** | none (integer-exact) |
| **FFN** `W2·gelu(W1·x+b1)+b2` | 4→8→3 | **3/3 bit-exact** | none |
| **softmax** | n=8 | 5/8 bit-exact, **8/8 ≤1 ULP** | Adreno division rounding |
| **layernorm** | n=8 | **8/8 bit-exact** | none here |
| **attention** (1 head, QK^T·softmax·V) | S=4, D=4 | 14/16 bit-exact, **16/16 ≤4 ULP** | div rounding compounded through softmax-weighted AV |

In every case the **Form reference matched the python float32 oracle exactly** — the algorithm is reproduced
bit-for-bit in Form. Where the GPU diverges it is by a small, *named, measured* amount (≤1 ULP softmax, ≤4 ULP
attention), the Adreno's non-correctly-rounded division — exactly the driver-opt tolerance flagged from the
start, reported not hidden. The bit-exact kernels (matvec, FFN, layernorm here) confirm `precise`/NoContraction
on mul+add is honored; only division drifts.

## Kernel chaining with barriers (Gap 3)

`model/form-chain.fk`: **two compute dispatches in one command buffer**, with `vkCmdPipelineBarrier` between
them (compute→compute, SHADER_WRITE→SHADER_READ), dispatch 2 consuming dispatch 1's output buffer `h`:
`y = W2·(W1·x)` over two descriptor sets and one shared pipeline. Result `[0, 42, 42]` — **3/3 bit-exact** to
the integer Form reference. This is the missing multi-dispatch primitive: the barrier makes stage N+1 see
stage N's writes. (Calling the 10-arg `vkCmdPipelineBarrier` required widening the `c_call` carrier from 8 to
11 args — done, all prior kernels still pass.)

## New seed carriers (this receipt)

All in the `flt-ops` manifest → regenerated optable, bodies in the one `fkwu` seed:
`c_memcpy` (251, bulk file→GPU weight loading), `c_fadd`/`c_fsub`/`c_fmul`/`c_fdiv` (252–255, IEEE f32 on bit
patterns), `c_fsqrt` (232, correctly-rounded sqrt). Validated bit-exact vs a float32 oracle.

## Full transformer block — status

Every component of a pre-LN transformer block now runs on the Adreno from Form, verified:
**layernorm ✓ · attention ✓ · residual (= elementwise `c_fadd`) ✓ · layernorm ✓ · FFN ✓ · residual ✓**, and the
**chaining glue** (multi-dispatch + barrier) ✓. A full block is the *composition* of these proven dispatches —
each stage's output buffer barrier'd into the next — with no new mechanism required. That single assembled
6-stage recipe is the one honest "not-yet-run-as-one-file" item; its every piece and its glue are witnessed
above. The wide matvec (Gap 2) shows the same kernels run at real width with multi-workgroup dispatch.

## Verdict on the three gaps

- **Gap 1 (kernel coverage):** matvec, FFN, softmax, layernorm, attention — all dispatched and verified on the
  Adreno. The transformer kernel set is covered.
- **Gap 2 (full width):** 192×192 matvec, weights from disk, multi-workgroup dispatch, bit-exact, falsifiable
  (1 workgroup → 64 rows). See [`2026-06-29-android-gpu-vulkan-wide.md`](2026-06-29-android-gpu-vulkan-wide.md).
- **Gap 3 (full layer):** real-weight loading (`c_memcpy`) and kernel-chaining-with-barriers both proven; a full
  block is their composition.

Artifacts: `model/form-{ffn,softmax,layernorm,attention,chain,vulkan-wide}.fk`, `native/vulkan/*.{comp,spv}` +
`gen-*` data scripts, carriers 251–255 + 232. The only C is the one `fkwu` seed; every kernel and every reference is
Form. Nothing faked — every number was printed by the Adreno or by `fkwu`'s own f32 arithmetic.
