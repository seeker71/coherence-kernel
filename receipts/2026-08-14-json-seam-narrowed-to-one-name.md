# 2026-08-14 — the json seam narrowed to one name, and both doors re-witnessed

Claude, witness seat, continuing from
[`2026-08-14-one-fkwu-form-cli-first.md`](2026-08-14-one-fkwu-form-cli-first.md)
without waiting for a bus.

## What the morning receipt left open

The full `form-cli-loop-band` printed 511 and exited 1 — "the prelude
chain still carries a json lane seam." That sentence was a name with no
attempt beside it yet. Item 6 says a named gap is a work order, so I
preflighted before believing anything:

```
preflight form/form-stdlib/tests/form-cli-loop-band.fk
  errors 14   unresolved 5
  json-array-length / json-array-elements / json-object-get /
  json-int-value / json-string-value  ->  LANE SEAM (go rust ts)
```

Five names, all defined in `form/form-stdlib/json.fk` (lines 400–434),
all called by `code-tool-learning.fk` — and the loop chain simply never
carried `json.fk`. The precedent was already committed:
`tests/code-tool-learning-band.fk` preludes `core.fk json.fk ...` in
exactly that order and is proven. So the attempt was one word in two
prelude lines: `form-stdlib/json.fk` after `core.fk` in
`form-cli-loop.fk` and its band.

## Fresh compile, after

```
  errors 1    unresolved 1
  value_kind  ->  LANE SEAM (go rust ts)
band: 511, exit 1
```

Fourteen errors fell to the one that is already receipted:
`value_kind` is a native on go/rust/ts and absent from the fkwu seed;
`json.fk:468` prices exactly what that costs (the never-witnessed null
case folds to false). The repair roads are both closed by evidence, not
by fear: a Form `defn` overrides the working native on go and rust
(`2026-07-25-the-shim-is-an-override.md` — the band went from four-way
agreement to a two-arm crash), and a `value_eq`-marker null test changes
fkwu behavior on a path **no band on any arm can observe today**
(json.fk header, 2026-07-26). An attempt that cannot be observed enters
as a lesson, not a claim — so the floor stays: every json-carrying
chain on fkwu answers its number and exits 1 through this single name.
`json-band` itself: 1023, exit 1. `code-tool-learning-band`: 1048575,
exit 1. The loop band now stands in that same, single-seam family
instead of carrying five extra foreign names.

## Both doors, re-witnessed by this hand

Offline law over core alone (bits 32..256 of the band, `core.fk` +
`fcl-route-net`/`fcl-step-lane` in one temp cell):

```
480, exit 0
```

A miss with no net is nothing, not Starlink. Edit / commit / review /
crystallize never wait on the satellite. Push offline is silence; push
with net is local host git.

One-fkwu Metal door (`form/form-stdlib/tests/metal-door-band.fk`):

```
PASS fkwu-form-cli-metal-matvec-f32
metal_owner=fkwu-form-cli  metal_linked=true  device=Apple M4 Max
y=30 70 110 150
15, exit 0
```

One binary. Metal is this host's organ, not a second executable.

`form-cli-loop.fk` now carries today's `; witnessed:` stamp naming all
of this, so the next reader does not have to re-derive which of the two
opposite repairs the red line wants.

## Most surprising teaching

Fourteen errors and one error produce the **same green number**. 511
printed before the fix and after it — the band's nine claims never
route through json, so the number could not feel the wall falling. Only
preflight's error count and the exit code carried the truth. The body's
"read the exit code, not the number" is not a caution about rare cases;
it is the only instrument that registered this entire move.

## Where discomfort turned to gold

I wanted exit 0. Sitting with preflight's last red line, the pull was
strong to define `value_kind` in Form and watch the band go clean here
— and the 2026-07-25 receipt shows exactly that pull, acted on, crashing
two sibling arms that were working. The discomfort of leaving an exit
at 1 turned into the actual deliverable: the seam is now **one**
receipted name instead of five unexamined ones, and the cell says so
where the next hand will look. A smaller honest floor beats a local
green that lies four-way.

## Proposed distillation row — not landed, the corpus is not mine to edit

Frontier question (smallest thing the body cannot yet answer natively):
*can a chain tell the difference between "this call resolves nowhere"
(typo) and "this call resolves on a sibling arm" (lane seam) at
runtime, without preflight?* Rented answer: no — fkwu's
`[unresolved-call]` recovery to nothing is uniform; only
`observe/preflight.fk` offering the name to all four kernels
distinguishes the two repairs. The native form of that question is an
arm-mask primitive the seed does not carry.

```
q: unresolved name — typo or lane seam?
a: indistinguishable inside one arm; witnessed only by offering the name to all four (pf-arm-mask)
evidence: form-cli-loop-band 14->1 errors, 2026-08-14
```
