# 2026-08-19 — one flow, from BML to a cell we can inspect

Yes asked for choice/cut/stop/undo/restore/timeout/nothing, BML to
token, generated recipe, a valid cell on the substrate, framebuffer
with a visual event/trace path, and xpath to spot-inspect thoughts
that may live only as gas or water.

## What ran

Authored in `form.lift` (`form-cli-e2e-flow.fk`), crystallized, used.

Control ids: nothing 0, cut 1, stop 2, timeout 3, choice 4, undo 5,
restore 6. Choice executes a generated `2+3` branch → 5. Undo
returns prev. Restore returns orig. Timeout is 0. Nothing is gas.
Restore is ice.

xpath-shaped walk of the flow:

```
names:  bml, token, exec, cell
phases: 1,    0,     1,    2
        water gas    water ice
```

The token stage and the nested `("add" ("add" 1 1) 1)` IR stay gas
— generated, inspectable, not frozen.

A recipe NodeID was interned `(bp "add")` with two children and
executed as Form `fee-branch` → 5. `walk_recipe` is a sibling seam
and was not called.

Framebuffer (no prompt text): action 5 (rehearse ground), 2 events.

Metal door, this host: `metal_linked=true`, device Apple M4 Max,
`cpu_jit_dispatch=0`, `last_error=none`. The GPU is present. It did
not execute this recipe. MLX is not a second binary. JIT is a named
door, not this sitting's executor.

Band **1023**. Door: `./fkwu form/form-stdlib/form-cli-e2e-flow-run.fk`.

## Honest radius

xpath here is `fee-xpath-*` over the flow list, the same path idea
as `xpath.fk`, not the intern_node walker (1910-decade). That
walker stays available for NodeID trees. This flow's inspect is
the list lens, so fkwu can hold it.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-19 -> form-cli-e2e-flow-band 1023, metal door present, jit 0
