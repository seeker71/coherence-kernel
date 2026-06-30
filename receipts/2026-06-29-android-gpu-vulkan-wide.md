# Receipt — full-width, file-loaded matvec on a real Adreno, bit-exact (2026-06-29 20:20 MDT)

**Closes Gap 2 (full width / multi-workgroup) and the Gap-3 weight-loading piece** for the linear kernel, on the
attached **Galaxy S23 Ultra (SM-S918U1), Adreno 740**, driven entirely from Form (no clang in the path).

## What ran

A **192×192** matvec `y = W·x` (vs the earlier 4×5 toy): real weights **loaded from a file into GPU memory**,
dispatched across **3 workgroups**, every output row checked against an independent in-Form reference.

- **Weight loading (Gap 3).** A new seed carrier `c_memcpy(dst, src, n)` copies bytes. The recipe reads
  `W.bin` (147 456 B = 192×192 f32) and `x.bin` (768 B) with `read_file`, takes their address with
  `c_str_addr`, and `c_memcpy`s them straight into the mapped GPU buffers. Real data movement, file → GPU, from
  Form — not hand-typed `i2f` constants.
- **Multi-workgroup dispatch (Gap 2).** `vkCmdDispatch(3,1,1)` = `ceil(192/64)` workgroups (the shader's
  `local_size_x=64`, `if (i>=rows) return`). Buffer sizes and push-constants (`rows=192, cols=192`) are the real
  shape.
- **Verification.** The weights are integer-valued (`W[i][j]=(i%7)+(j%3)`, `x[j]=1+(j%2)`) so the matvec is
  f32-exact, and **row-varying** (7 distinct row sums: 288, 576, 864, …, 2016 — not a degenerate constant). The
  recipe decodes each GPU output (`f2i`) and compares it to the same matvec **recomputed fresh in Form**
  (`ry`/`term`). Result: **192 / 192 rows match.**

## Falsifiable, not vacuous

```
3 workgroups (ceil(192/64)) -> 192 rows match     (every row, all 3 groups ran)
1 workgroup  (forced)       ->  64 rows match     (only rows 0-63 computed; 64-191 unwritten -> fail)
```
The drop from 192→64 when the dispatch is throttled to one workgroup proves the multi-workgroup dispatch is
doing real work across all three groups, and that the check genuinely depends on the GPU output.

## What this is

The **general unlock**: arbitrary-width linear layers can now run on the Adreno — weights streamed from disk,
dispatched at real width, verified bit-exact. The shader is unchanged (`fglsl-matvec`, `rows`/`cols` push
constants); only the dispatch (group count) and data path (file `c_memcpy`) scaled. Still integer-valued data
for *bit-exact* checking; arbitrary-f32 weights work through the identical path but would need a named f32
tolerance (or in-Form f32 software arithmetic) to verify — named, not yet built.

Artifacts: [`model/form-vulkan-wide.fk`](../model/form-vulkan-wide.fk),
[`native/vulkan/gen-wide-weights.py`](../native/vulkan/gen-wide-weights.py), carrier `c_memcpy` (tag 251) in the
`flt-ops` manifest + regenerated optable. Run: `python3 native/vulkan/gen-wide-weights.py` then push `W.bin`/
`x.bin`/`matvec.spv` + `fkwu` to the device and `./fkwu --src form-vulkan-wide.fk` → `192`.
