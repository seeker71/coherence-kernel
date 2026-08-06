# Nothing at the public list boundary — temporary C-seed repair

Date: 2026-07-21

## Named gap

The canonical internal `nothing` sentinel is intentionally a tagged value. Scalar
transport already rendered it as `nothing`, but the source runner's list printer
sent list elements through its numeric-only inline renderer. Public lists therefore
leaked `-8999999999999999999`, including recursively nested lists.

## Smallest checkout-witness repair

`runtime/fkwu-uni.c` adds one renderer branch at the existing public list transport
boundary: test `fk_is_nothing(v)` before float/integer formatting and emit `nothing`.
The internal sentinel, reducer, equality, classifier, and evaluator are unchanged.

This is explicitly a short-lived C-seed repair under the repository's shrink rule.
Its shrink target is a Form-owned public value renderer used by the native walker;
when that renderer owns list transport, this branch must disappear with the C seed.
No model, scripture, or semantic layer is changed.

## Witness contract

- scalar: `nothing`
- mixed list: `nothing` remains distinct from `0`, `1`, negative integer, float,
  and a node handle
- nested lists: every contained sentinel renders as `nothing`
- bootstrap and native-body ground remain unchanged after rebuilding `fkwu`

## Live observation

After rebuilding `fkwu` from the changed seed:

```text
ground                         -> 42
ground-recursive 10            -> 55
binary-freshness-band          -> 15
ground-numeric-list            -> [1, 2.5, [3, 4]]
native-vs-rented-check         -> 11111
nothing-render-scalar-witness  -> nothing
nothing-render-list-witness    -> [nothing, 0, 1, -7, 2.5, -5]
nothing-render-nested-witness  -> [[nothing, 0], [1, [nothing, -3]]]
```

The final `-5` in the mixed list is the node handle. It remains visibly distinct
from `nothing`, zero, one, the negative integer, and the float. The repair changed
presentation only; all grounding values remained stable.
