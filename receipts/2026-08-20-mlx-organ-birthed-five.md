# 2026-08-20 — MLX is an organ of this fkwu, and it wrote 5

Yes asked to fully enable and witness MLX. It was not a missing
library. `libmlxc.dylib` 0.32.0 was already on this machine. Python
already added 2+3. form-cli had no primitive, so Allowance named the
door and never walked through — the same shape as yesterday's quiet
JIT counter.

## What was enabled

MLX sits in **this** binary, not a second one.

- carrier: `form/native/mlx/fk-mlx-carrier.c` (strong symbols)
- weak stubs in `runtime/fkwu-uni.c` speak `mlx_linked=false` when
  the carrier is not linked
- ops: `mlx_status` (tag 143), `mlx_add` (tag 144)
- Darwin + `libmlxc` links the carrier; otherwise the stubs hold

The C eval arms are checkout-witness, the same seat as Metal. The
seed is not the home. Meaning is the Form cells. Shrink target is
the walker owning the call.

BML still names the add (`alb-lhs` 2, `alb-rhs` 3).

## What ran

Door: `./fkwu form/form-stdlib/form-cli-mlx-run.fk`

```
bml lhs=2 rhs=3 bml=5
mlx-add=5
mlx_linked=true
mlx_metal_available=true
mlx_gpu_available=true
mlx_device=gpu
mlx_version=0.32.0
mlx_dispatch=1
last_error=none
```

Band **63**. JIT birth band still **255**. Freshness **31**. Ground **42**.

Rebuild:

```
cc -O2 -o fkwu runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m \
  form/native/mlx/fk-mlx-carrier.c \
  -framework Metal -framework Foundation -fobjc-arc \
  -I/opt/homebrew/include -L/opt/homebrew/lib -lmlxc -Wl,-rpath,/opt/homebrew/lib
```

Without `libmlxc`, omit the carrier; `mlx_status` says unlinked.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-20 -> form-cli-mlx-band 63, mlx-add=5, gpu, dispatch=1
