# Authoring a Form stdlib recipe

A guide for any cell — agent or human — writing a new `.fk` recipe + proof band. It carries the
conventions and the hard-won traps so you don't rediscover them; `validate.sh` is the check that
reads the body, this is the guide that names the way. The recipes here are the body's logic, proven
by `fkwu` bands — **a band's observed verdict, held to the pin its head declares, is the proof; there
is no trusted prover.**

## Before you write — don't duplicate

Grep first. Much already exists (`substrate-phase.fk` is a whole phase metabolism; `nearest-shape.fk`,
`feature-vector.fk` and `classifier-eval.fk` are the perception toolkit). A recipe that already lives
wants your extension, not a sibling:

```
git grep -a -l "<the-thing>" form/form-stdlib/ docs/
```

## Blueprint names come from the registry

Do not put raw `(make_nodeid 1 2 99 N)` coordinates in executable stdlib logic. Load
`form-stdlib/form-ontology-loader.fk` as a prelude and ask by name, binding the answer once at
the top of the consumer:

```lisp
(let JSON-OBJECT (bp "JSON-OBJECT"))

(intern_node JSON-OBJECT children)
```

This keeps cell/blueprint/recipe names swappable and prevents quiet compile failures when a
name changes in one place. A new name is a row in `form-stdlib/blueprint-registry.json`; see
[`../user-blueprint-registry.md`](../user-blueprint-registry.md).

## New meaning — floor is BML + cached native

This is the guide, executable: `form-cli-author-high`. **Floor:** high-grammar
BML (`.bml`, `section [form.bml]`, `section [form.lift]`, BMF, field `.form`)
**with** the optimal cached native speed compiler (fresh `.fkb` / `.dylib`).
See `grammars/bml-native-north-star.form` at the repo root.

The native `.bml.fkb` / `.bml.sym` pair is the cache; no source-shaped
companion is authored or committed. Running the BML lane or
`form-cli-bml-cache-run.fk` reaches that cache on demand, scoped to the lane
being run rather than by an ambient repository sweep.
Existing `.fk` organs are welcome; `*-band.fk` witnesses; the C seed
shrinks gladly.

```
printf "author-high\nquit\n" | ./fkwu form/form-stdlib/form-cli-repl.fk
```

High-grammar authority and executable surface live together at
`form/form-stdlib/bml/<name>.bml` as `section [form.bml] { class Foo<T> { … } }`.
Run it for its **native cache**, not as a destination in generated Form source.
For infix, unless/when, and ice/liquid/compost, use `section [form.lift]`
(a `.bml` carrying it lowers in memory). Edit `form/form-stdlib/grammars/form-lift.fk`
to grow that dialect, then write in the new rules. No remote oracle.

A **recipe** `form/form-stdlib/<name>.fk` is a welcome organ when the native
BML lane cannot yet carry the meaning — name that door in the receipt:

```lisp
; <name>.fk — one-line purpose. (the comment block is the human-facing teaching)
(do
    (defn foo (a)   (add a 1))
    (defn bar (a b) (if (gt a b) a b))
    0)
```

A **band** `form/form-stdlib/tests/<name>-band.fk` — proves it, returning a **bit-sum verdict**
(each bit = one falsifiable claim). The first line names the recipe it loads:

```lisp
; preludes: form-stdlib/<name>.fk
(do
    (let c0 (if (eq (foo 4) 5) 1 0))
    (let c1 (if (eq (bar 7 3) 7) 2 0))
    (add c0 c1))            ; verdict 3 when both claims land
```

Keep the band **self-contained** — prelude only your own recipe (+ `core.fk`); a recipe that
composes others preludes them in its own header. `core.fk` is plain Form, so its helpers (`nil? map
filter foldl reverse range take drop any? all? …`) are bound wherever a unit's closure reaches it.
fkwu resolves and lowers that closure; `./fkwu --closure <band> <out>`, run from the repo root,
writes it as one plain-Form file.

## The primitive set — these and no others

`eq · gt · ge · add · and · not · nth · head · tail · len · list · cons · if · empty · str_eq`
plus `defn · let · do`. (Read `form/form-stdlib/core.fk` — it is the whole vocabulary.)

- `eq` compares **integers and nodes**; `str_eq` compares **strings**. Don't cross them.
- `cons` prepends: `(cons x xs)` → a list with `x` at the head. Build lists with `cons` + recursion
  (see `feature-vector.fk`'s `fv-hist-loop`).
- `empty` **constructs** the empty list (`(empty)`, no args) — it is the absence value, **not a
  predicate**. Test emptiness with `(eq (len x) 0)`. See trap 6.
- This is the **curated band-verdict subset, not the kernel's limit.** `mul`/`sub`/`div` and full IEEE
  floats all work and **compute deterministically on fkwu** (proven: integer `mul`, `0.1+0.2`, and a
  float matvec all repeat bit for bit). The kernel is a full numeric engine (the format recipes,
  e.g. `format-arith.fk`, carry bf16/fp8/nf4/int8/bitnet-158).
  These are kept out of *bands* only because verdicts stay integer for a clean `eq`. For numeric/ML
  recipes use the full engine, and reduce a float result to an integer verdict with `round`/`floor`/`ceil`/`trunc`.

## The traps (each one cost a real debugging cycle)

1. **`and` and `or` are BINARY. Never write `(and a b c)`.** A third argument is not a fold. Nest:
   `(and (and a b) c)`, the one shape every band uses.

2. **No `sub`, `mul`, `div`, `lt`, `le`.** Express everything with `add` + comparisons + recursion:
   - `a < b` → `(gt b a)` · `a <= b` → `(ge b a)`
   - "decrease / difference" → **count with recursion**, don't subtract.
   - a mean/ratio that needs division → redesign as a **proven-count gate** (`(ge correct min)`),
     the way `classifier-eval.fk` does. Most perception logic is
     counting, selection, and gating — which the primitive set covers exactly.

3. **Float COMPUTE is deterministic; raw-float EQ is the trap.** A fractional float result repeats
   bit for bit (proven); what's unreliable is `eq` on raw floats — and whole-number floats
   still display inconsistently (`3.0` vs `3`). So compute in float, then reduce to an INTEGER verdict —
   `(eq (round (mul r 100.0)) 40)` — and `eq` the integer. `round` is half-AWAY-from-zero;
   see `tests/rounding-ops-band.fk`. Band SCORES still default to integers `0..100`; floats are the
   numeric/ML payload, not the verdict.

4. **No `let` inside a `defn` body.** Use nested `defn`s or extra parameters. (`let` is fine only at
   the top level of the band's `(do ...)`.)

5. **Loop via recursion** — there is no loop form. The max-select shape (pick the best candidate over
   a list) is in `nearest-shape.fk`'s `ns-best-loop`; copy it.

6. **`(empty x)` is NOT "is x empty?".** `empty` constructs the absence value; `(empty anything)`
   returns `[]`, which `if` treats as **truthy** — so `(if (empty xs) A B)` **always** takes branch A.
   The failure is silent: no error, just a wrong verdict (a recursion that never recurses, a guard
   that never guards). Test emptiness with `(eq (len x) 0)` — the idiom every recipe uses
   (`nearest-shape.fk`). It cost a cycle once (verdict 88, not 127).

## Prove it on fkwu

From the repo's `form/` directory, name the band alone; its `; preludes:` header carries the rest:

```
cd form
./validate.sh form-stdlib/tests/<name>-band.fk
```

Success is `✓ ... → <verdict> (its pin)` **and** `1 ok, 0 failed`. Iterate until you see your
intended verdict. The band's head pins it (`; Verdict <n>`); a stem listed in
`form/band-verdicts.txt` is held to that row as well. A band that needs a host carrier (a live
Metal device) says so with `; PROOF LEVEL: FKWU-STAGED` and `; STAGED CARRIER:` and reads pending
until the carrier stands.

- `unresolved-call` → a misspelled name, a primitive that isn't in `core.fk`, or a prelude the
  unit's header does not name; `observe/preflight.fk` says which.
- wrong verdict → a band claim is false; fix the recipe or the claim. **Never weaken a
  claim to make it pass** — the band is the truth, not the obstacle.

## Honest bands

Each bit asserts something that could be false and would matter. Prove both the positive (it
recognizes) and the negative (it stays silent / flags novel / refuses below the floor). A band that
only checks `1 == 1` is theatre. Pick the simplest, strangest edge that pins the boundary — a tie, a
just-below-threshold value, an empty input — one expression each.

## When it proves

The recipe's leading comment block is its teaching (Lisp-comment voice, like `nearest-shape.fk`).
Add the band to `form/band-verdicts.txt` when its verdict should be registered, and ship in one commit — edges
land with the content. If you're a subagent in a workflow, return the contents instead and let the
parent integrate.

---
*Examples worth reading whole: `nearest-shape.fk` (a classifier from primitives), `feature-vector.fk`
(binning and recursion over lists), `classifier-eval.fk` (a proven-count gate), `substrate-phase.fk`
(state without mutation). The whole `form/form-stdlib/tests/` directory is worked bands.*
