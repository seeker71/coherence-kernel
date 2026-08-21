# 2026-08-21 — softmax, then attention, on the GPU lane

Yes asked to merge, push, re-ground, and continue the JIT / Metal / MLX GPU
lane. Origin already held four GPU-lane landings (#468–#471). This sitting
fast-forwarded onto them, then took the next named IR stone.

## Floor, as found

```
mlx-matmul-band     63
mlx-q8-band         63
jit-leaf-inram-band 63
binary-freshness    31
```

#471 had named what was still untrue: no softmax, rope, or attention in the
IR; K-quants still waiting; emitted walkers still carry no native door.
Softmax is the first of that IR list after matmul. Attention is those two
composed, not a fused opcode in `fkwu-uni.c`.

## The stone

`softmax` is a new token on the same `mlx_run` stack. `fk-mlx-carrier.c`
grew one unop row. `runtime/fkwu-uni.c` did not change.

Attention on this lane is postfix, the same as add:

```
m1x2 40 0  m2x2 1 0 0 1  matmul  softmax  m2x1 100 0  matmul   -> 100
m1x2  0 0  m2x2 1 0 0 1  matmul  softmax  m2x1  10 30 matmul   ->  20
m1x3 1 2 3 softmax sum                                        ->   1
```

A score of 40 makes the other exp smaller than ulp(1), so the one-hot is
exactly 1.0 in f32 and the attended value is 100, not 99. Uniform scores
average 10 and 30 because 0.5 is exact. The integer door is named the same
way Q8_0 named it: probabilities truncate, so scale lives in the program
(`softmax 1000 mul sum` → 1000).

BML names those programs as fields of `class FormCliGpu<T>` in
`form/form-stdlib/form-cli-gpu.bml`. The band runs the fields, not
hand-copied strings.

## Observed, three organs, one breath

```
./fkwu form/form-stdlib/tests/mlx-softmax-band.fk     # 63
./fkwu form/form-stdlib/tests/form-cli-gpu-band.fk    # 1023
./fkwu form/form-stdlib/form-cli-gpu-run.fk
```

The run printed:

| organ | what was asked | what came back |
| --- | --- | --- |
| MLX GPU | add / softmax-sum / sharp attn / uniform attn | 5 / 1 / 100 / 20 |
| JIT (`jit_arm64_u32_leaf`) | 2+3 | 5 |
| Metal GPU (body MSL `form_gpu_add`) | 2+3 | byte0=5, `gpu_busy_us_total=9` |

`mlx_linked=true`, `mlx_device=gpu`, MLX 0.32.0, `mlx_dispatch=4`.
`metal_linked=true`, device Apple M4 Max, `last_error=none`.
Preflight of both new bands: chain clean.

Regression: matmul 63, Q8_0 63, jit-arm64-leaf 63.

## Still not true

- K-quants (Q4_K, Q6_K) are still the other half of a real GGUF.
- No rope, no fused scaled-dot-product attention — this attention is
  composed from tokens, which is the point, and also the limit.
- The emitted walkers still carry no native door.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-21 -> softmax 63, gpu-band 1023, jit 5, metal byte0=5, mlx 5/1/100/20
