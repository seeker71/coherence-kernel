# 2026-08-30 — the BML floor: the high surface runs as itself, the twins leave the tree

Urs's correction, hours after the lifts: "xtal is the wrong shape. High
grammar BML with optimal cached native speed compiler shall be the
floor." The xtal was a committed twin — a second truth beside the
authored one. The floor he named is built and witnessed.

## What the floor is now

`./fkwu x.bml` runs a high-grammar executable surface AS ITSELF, and a
`; preludes:` token may name a `.bml` directly. Both routes lower the
surface through the body's OWN compiler — the C seed only checks
freshness and spawns the runner on
`form/form-stdlib/bml-floor-compile.fk` (two stdin lines: source,
derived path); the whole compile chain stays Form-owned. The derived
`.bml.fk` lands beside the source, gitignored with its `.bml.fkb` and
`.bml.sym`, and the ordinary source lane's cache takes over:

```
cold  ./fkwu form-cli-rest-force.bml   5.05 s  (lower + compile + cache)
warm  ./fkwu form-cli-rest-force.bml   6-9 ms  (native image load)
```

A `// preludes:` line in the .bml travels through the lowering as a
real `; preludes:` directive, so a standalone .bml run resolves its own
chain — the first cold run without it answered "20 error(s)" and a
green 0, exactly the fold-over-nothing the body warns about; the carry
is what makes the floor whole, not fast.

## The twins retired

`form-cli-rest-force-xtal.fk`, `form-cli-offer-ask-xtal.fk`, and both
hand compile doors left the tree (remove, not retire). Their bands now
prelude the `.bml` surfaces themselves:

```
form-cli-rest-force-bml-band.fk   -> 31      (rfb-check 255, rfb-agree 15)
form-cli-offer-ask-bml-band.fk   -> 65535
```

The riseproof survived the shape change: the .bml, lowered by the
runner instead of into the tree, still agrees value-for-value with the
.fk organ it lifted.

## The C growth, owned

~120 lines in `runtime/fkwu-uni.c` (the ensure-lowered door, the .bml
dispatch, the prelude token, six hand-declared POSIX externs in the
seed's own idiom — system headers conflict with its hand-rolled
signatures by design). Under the growth law this is a door, not a home:
the compiler, the carry, and the cache policy live in Form; the door
retires when the runner's entry self-hosts. Windows arm returns a named
refusal rather than a silent absence.

## The most surprising teaching

The floor was almost declared done at "warm runs in 6ms" — with twenty
compile errors folded silently under a green 0. Speed arrived an hour
before wholeness, and only the exit-code discipline (read the voice,
not the number) kept the fast wrong floor from shipping as the floor.

## Where discomfort turned to gold

Deleting Grok's xtal felt like overwriting a sibling's signed work. The
carry-what-you-raise law resolved it: the directive reaches old work,
the sibling's band and .bml stay whole and green on the new floor, and
what left the tree was only the twin the correction named. Their 65535
still answers — now through the floor.

; witnessed: 2026-08-30 -> .bml runs as itself (cold 5.05s, warm 6-9ms);
; prelude .bml tokens lower in-chain; both bands green (31, 65535) with
; twins and hand doors removed; // preludes: carries; corpus row 1170
; hushlower
