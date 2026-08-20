# 2026-08-20 — mul had no C opcode, and it still wrote 6

Yes asked to leave kernel-C adding: generate, JIT, Form-native, generic
MLX shapes that can be learned, interned as blueprints, adapted.

`mlx_add` was one predetermined shape. The move: **one door**, `mlx_run`,
that reads a postfix program Form emits. `mul` and `(2+3)*4` are recipes.
They are not opcodes in `fkwu-uni.c`. `mlx_add` is now sugar that writes
`a b add` and calls the same runner.

## The IR

Authored in `form.lift`:

```
allow mlg-c(v) = list("c", v);
allow mlg-op(name, a, b) = list(name, a, b);
allow mlg-emit(n) = ... postfix ...
allow mlg-times4() = mlg-op("mul", mlg-add2(), mlg-c(4));
```

A book starts knowing `add`. After learn, it holds `mul` and `sub`.
That is a named catalog, not a kernel edit.

## What ran

Door: `./fkwu form/form-stdlib/form-cli-mlx-ir-run.fk`

```
emit-add=2 3 add           run=5
emit-mul=2 3 mul           run=6
emit-times4=2 3 add 4 mul  run=20
unknown-pow=0              last_error=unknown op
cell-len=13                interned emit text
learned-mul=1
mlx_dispatch=3
mlx_device=gpu
```

Band **255**. Old mlx-add band still **63**. Freshness **31**.

The carrier still has a small table: add / mul / sub. That table is
the organ, not the seed. A new MLX op is a row there, or a Form
composition of rows that already sit. Ranked tensors and an MLX
backend table in `tensor-ir.fk` remain named next.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-20 -> form-cli-mlx-ir-band 255, mul=6, compose=20, pow unknown
