# 2026-08-20 — one input, three organs, no flags

Yes asked to see it whole: not a toy band beside the flow, not a
flag, not a second binary. Recursive thoughts generate micro-kernels
from the input and translate the add on demand onto MLX, CPU JIT, and
Metal GPU.

Door, nothing else:

```
./fkwu form/form-stdlib/form-cli-live-run.fk
```

## The input

```
(add 2 3) (max 2 9) (vadd 10 20 1 2)
```

A recursive thought (`fle-progs`, `fle-pulse`) walks that list,
emits postfix, and `mlx_run`s each kernel as it recurses.

```
gen0=2 3 add
gen1=2 9 max
gen2=v2 10 20 v2 1 2 add sum
mlx-pulse=47   (5+9+33)
```

The same add row, on demand:

```
jit enqueue=1 byte0=5    cpu_jit_dispatch=1
gpu enqueue=1 sync=1 byte0=5   total_dispatch=1 gpu_busy_us=10
mlx device=gpu dispatch=10 last_error=none
unified_memory=1
fb-live=3  then bfc pair -> fb-events=5
```

Band **255** is `fle-check` — it *is* the pulse, not a string-eq
costume. Check **255**.

## Honest radius

This is a generic *flow* over input-shaped micro-kernels. It is not
a GGUF forward, and it is not every MLX op. `pow` still names a miss.
Bandwidth numbers are the organs' own counters, not a new tuner.
Rebuild fkwu with the Metal+MLX carriers so the doors are linked.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-20 -> form-cli-live-band 255, mlx 5/9/33, jit 5, gpu 5
