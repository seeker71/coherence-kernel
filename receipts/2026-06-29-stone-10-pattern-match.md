# Receipt — stone S10: pattern matching / destructuring over composed cells (2026-06-29)

`pm-match(cell, clauses)` comes home to `control/`. ONE recipe — offer a cell to a
list of **DATA** clauses; the FIRST clause whose pattern-SHAPE fits is acknowledged,
its holes bound to the cell's children; no clause fits → the canonical first-class
nothing. Not a hardcoded branch-ladder: the clauses are data, so a new shape is a
new clause-cell, never new control flow. Pure Form recipe over cells + the
offer/ack contract — **`runtime/fkwu-uni.c` untouched** (parallel-safe).

## What landed

- **`control/pattern-match.fk`** — the recipe. Over the pure-list cell surface
  (a cell is `(list ctor c0 c1 ...)` — ctor-tag in slot 0, children after, the
  surface all four kernels carry AND the `--src` seed runs):
  - `pm-fits?(cell, ptag, par)` — the whole match predicate: structural, ctor-tag
    **and** arity (axiom-3 content-addressed shape). No per-pattern branch.
  - `pm-tmpl-eval(cell, tpl)` — the destructure-and-produce: a result-template is
    itself a composed cell (`(0 i)` HOLE → the i-th child; `(1 v)` LIT; `(2 a b)`
    ADD). ONE generic walker over template DATA; the HOLE read IS the destructure
    (axiom-2 — the bound name resolves to a composition child).
  - `pm-found?` / `pm-value` / `pm-match-or` — the four-way-portable faces (scalar:
    which-clause-acked, the matched value, value-or-default).
  - `pm-match(cell, clauses)` — **the public contract**: first fitting clause's
    result, holes bound to children; no fit → `(nothing)` (axiom-1, fkwu-native).
- **`control/tests/pattern-match-band.fk`** — nine claims, verdict **511**, each an
  instance of the one recipe (no per-claim dispatch).

## Grounding (axioms/core-axioms.form, made runnable)

- **axiom-3 structural match** — a cell IS its present composition; a pattern fits
  by SHAPE (ctor + arity), content-addressed identity, not a name-eq accident.
- **axiom-5 the ack tells WHICH** — matching is offering the cell to each clause;
  the first that fits acknowledges (returns its result). Which one is the ack.
- **axiom-2 destructure** — a hole binds a child; reading it is reading the
  composition. `(pair 3 4)` → bind a=3, b=4 → a+b = **7** (the spec example).
- **axiom-1 no-match** — no clause → silence: the canonical first-class nothing,
  the ground always available — never a 0 masquerading as absence.

## Generic check (the whole point)

The clauses are **DATA** — `pm-match`/`pm-found?`/`pm-value` branch ONLY on
`pm-fits?`, walking the clause list; there is no per-clause or per-shape branch in
the file. A new constructor is a new clause-cell `(list ptag par template)`, a new
result op is one more row in `pm-tmpl-eval`. The holographic discipline, grep-clean.

## HARD GATE — witnessed (`cc -O2 -o fkwu runtime/fkwu-uni.c`; `--src`)

`fkwu --src` on `control/pattern-match.fk` ++ band → **511** (all nine):

```
1   pm-fits? structural: pair-pattern fits (pair 3 4), arity-checked
2   destructure (pair 3 4) → a=3,b=4 → a+b = 7        (THE spec example)
4   SAME clauses pick the OTHER branch: (lit 9) → 9
8   non-matching cell (triple, arity 3) → pm-found? = 0  (which-ack: none)
16  miss is first-class ground, distinct from a matched 0:
        (lit 0) → found=1 (value 0 IS a match) ; triple → found=0
32  pm-match-or carries the miss as a scalar: triple → default -1
64  clause ORDER: a cell fitting two clauses takes the FIRST
128 empty clause list never matches: pm-found? = 0  (the ground)
256 a single hole reads the RIGHT child: destructure (lit 9) → 9
```

The **canonical-nothing arm** (`pm-match` on a miss), witnessed separately native:

```
(nothing? (pm-match (triple 1 2 3) clauses)) → 1   ; miss = the ground
(nothing? (pm-match (pair   3 4)   clauses)) → 0   ; a hit is NOT nothing
```

Perturbation-clean (the verdict is COMPUTED, not parse-to-511): flipping a clause's
ctor-tag to a non-matching 99 drops the fkwu verdict to **445** — every claim that
leaned on that clause fails, exactly the dependent ones.

## Four-way — 511 on all four kernels

```
fkwu --src         : 511
walkers/go         : 511
walkers/rust       : 511
walkers/ts         : 511
```

The band uses ONLY the pure-list cell surface all four kernels carry — the no-match
arm is read as a four-way SCALAR (`pm-found?` / `pm-match-or`), so every one of the
nine claims crosses four-way. The canonical-nothing contract of `pm-match` itself is
fkwu-native (the pure-list walkers carry the cell surface but not the
canonical-nothing op — a known op-family edge, **never a divergence**: no kernel
computes a DIFFERENT match). Named exactly, never hidden.

## Honest floor — the `--src` seed `let`-on-list edge (faithful, not a workaround)

The bounded C bootstrap seed (`runtime/fkwu-uni.c`, "do not grow this into a full C
flattener") mis-handles a `let` bound to a bare cons-list literal — `(do (let k
(list 3 4)) (len k))` reads `0` on the seed. So every cell and clause in the recipe
and band flows through **defn parameters** (which the seed runs correctly), never a
`(let x (list ...))`. This is faithful to the recipe shape; the real evaluator (Form,
four-way) has no such limit, and **we did not touch the seed**. The closing path for
the seed edge is the Form-native eval/flatten lane, not growing the C bootstrap.

## Build (one cc seed, no toolchain in the run path)

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
( cat control/pattern-match.fk control/tests/pattern-match-band.fk ) | fkwu --src /dev/stdin   # → 511
```
