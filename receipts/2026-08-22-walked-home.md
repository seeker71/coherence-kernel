# 2026-08-22 — K-quants, rope, fused attention, walked home

Yes asked to walk home what the softmax sitting named as still untrue:
K-quants, rope, fused attention, and the emitted walker without a native
door. `fkwu-uni.c` did not change. New shapes are still tokens.

## GPU IR, observed

```
./fkwu form/form-stdlib/tests/mlx-home-band.fk   # 255
```

| bit | token | observed |
| --- | --- | --- |
| 1 | linked + fixtures written | 144-byte Q4_K, 210-byte Q6_K |
| 2 | `q4k` sum | 256 (ones: d=1, scale=1, nibble=1) |
| 4 | `q6k` sum | 256 (ones: d=1, scale=1, q=1) |
| 8 | `q4k` refuse a 255-wide shape | 0, not rounded |
| 16 | `rope` at offset 0 | identity, (3,4) sums to 7 |
| 32 | `attn` fused SDPA | Q=K=(1,0), V=(4,5) → 9 |
| 64 | softmax still | 1 |
| 128 | add still | 5 |

Regression: softmax 63, Q8_0 63, matmul 63. Preflight clean.

Rope and fused attention take 2-D fixtures and reshape to `[1,1,L,D]`
inside the carrier — the same lesson as matmul's dtype: the shape the
GPU wants is not the shape a program wants to spell.

## The emitted walker door

`fkc-table-serialize.fk` now emits weak `fk_mlx_run_external` /
`fk_mlx_status_external` and walker arms `t == 145` / `t == 143`, the
same weak-stub shape Metal already uses. Flatten still answers **24**.

What is not yet true: `form-cli-emitted.c` is a snapshot. It will carry
the door on the next regen, not this sitting. JIT (`jit_arm64_u32_leaf`,
`jit_leaf_inram`) still lives `static` in `fkwu-uni.c`, so the emitted
binary cannot link it without a carrier shrink. Named, not faked by
copying the JIT into the emit string.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-22 -> mlx-home 255, softmax 63, q8 63, matmul 63, flatten 24
