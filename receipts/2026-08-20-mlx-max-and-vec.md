# 2026-08-20 — max is a row, v2 is a shape

Yes said yes to both next shapes: `max` as a carrier row Form can
compose, and a 1-d vector add.

No new kernel opcode. `mlx_run` is still the door.

```
2 9 max                     → 9
v2 10 20 v2 1 2 add sum     → 33
```

`[10,20]+[1,2]` lives as a vector on the GPU, then `sum` reduces it
so the scalar door can speak. The interned emit is 23 bytes.

Book: `mlg-learn` of `max` after `mul`/`sub` — learned-max=1.

Band **1023** (old 255 plus max and vec). Add band still **63**.
`mlx_dispatch=5`. `pow` still unknown.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-20 -> form-cli-mlx-ir-band 1023, max=9, vadd-sum=33
