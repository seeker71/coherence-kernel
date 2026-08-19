# 2026-08-19 — a new grammar, crystallized, then written in

Yes asked to lift s-expr toward BML templates, generics, and high
grammar, and to invent grammar changes we can crystallize and write
in — all Form-native, no remote oracle, so the body can upgrade
itself locally.

## What already sat

`form.bml` is the maintenance dialect the text lens crystallizes.
The full `bml.fk` already holds classes, generics, and infix — that
high surface is not yet the text-lens loop. The line compiler knows
`template Name<T>` and `==`; the cursor `form-bml.fk` did not.
Teaching `form.bml` infix would fight the line-compiler verifier
(cursor wins only on node_eq). So this sitting invented a new
dialect instead of breaking the old one.

## The invention

`form.lift` — cursor-only. Grammar is data in
`form/form-stdlib/grammars/form-lift.fk`. New rules:

- infix `>= <= > < == + - * and or`
- `unless c then t else e` → `if (not c) t e`
- `when c then t` → `if c t 0`
- `ice` / `liquid` / `compost` → 2 / 1 / 0

A feature was written *in those rules*
(`form-cli-lift-ice.fk`):

```
def fli-ingest(depth, fear) = if depth >= 3 then if fear == 0 then ice else liquid else compost;
def fli-not-cursor() = unless fli-client-free("cursor") == 1 then 1 else 0;
```

Crystallized locally to `form-cli-lift-ice-xtal.fk`. Used:

```
anyone=1
not-cursor=1
ingest-fear=1
ingest-shallow=0
check=255
```

Band **255**. Parse probe of add / ge / unless: OK. No C seed
growth. No remote face.

## What still lifts

Templates and generics remain in the high `bml.fk` / line-compiler
`template` path. The next local upgrade is to seat them on
`form.lift` the same way: grammar data, cursor lower, crystallize,
write in the new rule.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-19 -> form-cli-lift-ice-band 255, form.lift parse OK
