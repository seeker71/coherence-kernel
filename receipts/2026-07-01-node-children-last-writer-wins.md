# Receipt — a live bug: `node_children` only holds for the most-recently-interned node (2026-07-01)

While live-testing `control/choice-lane-core.fk` against the c-bootstrap `fkwu` (see
`receipts/2026-07-01-choice-lane-control-invites.md`), a reproducible correctness bug surfaced in the runtime
itself — `runtime/fkwu-uni.c`, not in any `.fk` recipe. Naming it here, on its own, because it is a floor any
future recipe that interns more than one node-with-children in a scope will hit, not something specific to
this pass's work.

## The bug, minimally

No preludes needed — this reproduces against `fkwu` alone:

```form
(do
    (let a (intern_node (bp "A") (list (intern_trivial_int 1))))
    (let b (intern_node (bp "B") (list (intern_trivial_int 2))))
    (len (node_children a)))
```

Live result: **`0`**. Expected: `1` — `a` was interned with exactly one child (the int `1`), and nothing in
this recipe ever touches `a` again after `b` is built.

Extending to three:

```form
(do
    (let a (intern_node (bp "A") (list (intern_trivial_int 1))))
    (let b (intern_node (bp "B") (list (intern_trivial_int 2))))
    (let c (intern_node (bp "C") (list (intern_trivial_int 3))))
    (len (node_children a)))   ; -> 0, WRONG (expected 1)
    (len (node_children b)))   ; -> 0, WRONG (expected 1)
    (len (node_children c)))   ; -> 1, correct
```

The pattern is exact and repeatable: **only the most-recently-interned node with children reports its
children; every earlier one reads as childless once a later one is built.** Order, not content, decides —
swap which node is built last and the previously-correct one breaks instead:

```form
(do (let z (oac-zero)) (let o (oac-one (intern_trivial_int 5))) ...)
    (len (node_children z))  ; -> 0, WRONG
    (len (node_children o))  ; -> 1, correct

(do (let o (oac-one (intern_trivial_int 5))) (let z (oac-zero)) ...)
    (len (node_children z))  ; -> 1, correct (now built last)
    (len (node_children o))  ; -> 0, WRONG (now built first)
```

This reads like a single reused "last children" buffer in the runtime rather than per-node persistent
storage — building a second node-with-children appears to invalidate the first's, whatever its content.

## Why this matters beyond this pass

This is the actual root of the `control/choice-lane-core.fk` / `control/offer-ack-core.fk` live-testing gap
named in `receipts/2026-07-01-choice-lane-control-invites.md`. That receipt's first pass guessed a `bp`
blueprint-table capacity/collision; live investigation (gdb on `fk_sintern`, `FK_OBSERVE=1` tracing
`fk_offer_ack`, ruling the JIT in/out via `FK_JIT`/`FK_JIT_HOT`/`FK_JIT_WITNESS`) ruled that out step by step
and landed here instead: `OAC-ZERO` and `OAC-ONE` (`control/offer-ack-core.fk`) are both correctly-assigned,
genuinely distinct blueprint coordinates carrying one child each — the failure is not in blueprint identity,
it is that reading a node's children after a sibling node is built can return the wrong (empty) answer. Any
recipe that builds more than one non-leaf node in a scope and later inspects an earlier one's children is
exposed to this, not just `oac-kind`.

## What this pass did and did not do about it

- **Did**: isolate the minimal repro above (no prelude), confirm it is order-dependent and content-independent,
  and correct `receipts/2026-07-01-choice-lane-control-invites.md`'s honest-floor section to point here instead
  of the earlier, less precise guess.
- **Did not**: patch `runtime/fkwu-uni.c`. That file is a single machine-emitted ~60,000-character line (not
  hand-authorable source — confirmed via `objdump`/`nm`, not assumed), and `AGENTS.md` is explicit that the C
  seed is a shrink target: a patch here would need to be either a short-lived, precisely-scoped repair with its
  own shrink receipt, or land through the Form-native eval/flatten lane instead of growing the seed further. A
  blind edit to an opaque, generated blob, unverified against whatever emitted it, is worse than naming the gap
  plainly.
- **Did not** fabricate a passing band to route around it. `control/tests/choice-lane-core-band.fk` and the
  pre-existing `control/tests/offer-ack-core-band.fk` both still report their true (currently non-1023) result
  on this build; each primitive they exercise was instead verified correct in isolated, single-node live runs
  (see the choice-lane receipt) — none of which alone triggers this last-writer-wins pattern.

## Build (reproduce this receipt directly)

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
cat > /tmp/node-children-repro.fk <<'EOF'
(do
    (let a (intern_node (bp "A") (list (intern_trivial_int 1))))
    (let b (intern_node (bp "B") (list (intern_trivial_int 2))))
    (len (node_children a)))
EOF
./fkwu --src /tmp/node-children-repro.fk   # -> 0 (wrong; expected 1)
```
