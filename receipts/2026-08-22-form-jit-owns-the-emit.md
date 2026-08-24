# 2026-08-22 — the JIT is Form; C only installs

Yes named the law. We generate code on demand. We do not bake low-level
code into C. The Form-native JIT — authored here as BML class `FormJit<T>`
— emits onto CPU, Metal GPU, and MLX from one thought. Hardware doors
install and call. They do not own the algorithm.

That is also a correction of the sitting that walked K-quants and fused
attention home by writing dequant and SDPA loops in `fk-mlx-carrier.c`.
Those loops already live in Form (`q4k-dequant.fk`, `q4k-msl.fk`,
tensor-ir). New tokens in C were the costume. This organ is the door.

## The thought

```
f(n) = n * Mul + Add
Mul = 3, Add = 7, sample 5 → 22
```

Named fields in `form/form-stdlib/form-cli-jit.bml`. The class emits:

- MLX postfix: `5 3 mul 7 add`
- MSL: `a[0] = a[0] * 3 + 7` with 3 and 7 from those fields
- AArch64: `form-lower` of the SSA tree, and `fa-image` of load/mul/add/store

## Observed

```
./fkwu form/form-stdlib/tests/form-cli-jit-band.fk   # 1023
./fkwu form/form-stdlib/form-cli-jit-run.fk
```

| organ | door | result |
| --- | --- | --- |
| MLX GPU | `mlx_run` of Form postfix | 22 |
| Form CPU | `lo-compile-fn` 20 bytes, `jit_leaf_inram` | 22 |
| Metal GPU | `metal_pipeline` of Form MSL | byte0=22, `gpu_busy_us=417` |
| CPU via Metal door | `form_cpu_jit` of Form AArch64 | byte0=22, `cpu_jit_dispatch=1` |

`fkwu-uni.c` did not change. Preflight clean.

## Still s-expr, named

`form-lower.fk` and `form-asm.fk` are still s-expression compilers. The
organ that *chooses* the emission is BML. Lifting the encoder itself to
BML is the next honest climb, not this hour.

C-baked `q4k` / `q6k` / `rope` / `attn` still sit in the MLX carrier.
They are compost: Form already emits those as MSL and as recipes. This
sitting did not delete them. It stopped growing them.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-22 -> jit-band 1023, mlx 22, form-lower 22, metal 22, form_cpu_jit 22
