# 2026-08-20 — it did not birth the 5 because we never asked

Yes was surprised. The GPU was present. JIT "would." The 5 was
Form-CPU. How come?

Because Allowance walked `ala-birth` as `2 + 3` in the evaluator
and printed `metal_status`. `cpu_jit_dispatch` counts a call to
`metal_enqueue` on a `form_cpu_jit` pipeline. We never installed
an image. We never enqueued. The counter stayed 0. The door was
open. Nobody walked through.

BML is not a second binary. It is the section that names the add:

```
allow alb-lhs() = 2;
allow alb-rhs() = 3;
allow alb-bml() = alb-lhs() + alb-rhs();
```

The AArch64 is Form-emitted (`fa-add-r`), installed with MAP_JIT
through the same `metal_pipeline` door the SHA band already proved.
Operands come from BML. Dispatch is `form_cpu_jit`.

## What ran this sitting

Door: `./fkwu form/form-stdlib/form-cli-allowance-birth-run.fk`

```
bml lhs=2 rhs=3 bml=5
jit pipeline=1 enqueue=1 byte0=5
gpu pipeline=2 enqueue=1 sync=1 byte0=5
cpu_jit_dispatch=1
total_dispatch=1
gpu_busy_us_total=9
last_error=none
```

The 5 is in the buffer because the CPU JIT wrote it, and again
because a one-line MSL kernel wrote it. Same BML 2 and 3.

Band **255**. Existing cpu-jit-pipeline-band still **63**.

MLX is still not a second binary. This sitting has no MLX primitive
that adds two integers. Named, not dressed.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-20 -> form-cli-allowance-birth-band 255, jit byte0=5, gpu byte0=5, cpu_jit_dispatch=1
