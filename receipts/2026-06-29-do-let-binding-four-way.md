# Receipt — do-let binding crosses four-way: the `--src` top-level `(do (let …) …)` fix (2026-06-29)

**A four-way DIVERGENCE, root-caused and resolved** (CLAUDE.md: a divergence is a hard
gate, never a named gap). On the `--src` source-runner a top-level `(do (let NAME VAL) …
BODY)` lost the binding — `fkwu` computed a different answer than the three walkers:

```
(do (let p (list 10 20)) (len p))   fkwu OLD = 0   go = rust = ts = 2
(do (let p 5)            (add p 1))  fkwu OLD = 1   go = rust = ts = 6
```

This is the pre-existing shape the stone-S13 receipt named — *"a composed cell stored
through `let` degenerates on the `--src` seed"* — and worked around by binding cells as
nullary defns. This receipt fixes the root cause; the workaround is no longer required.

## Canonical decision — `let` is TWO-ARG

`let` is a **two-arg do-statement**: `(let name val)` binds `name` for the REST of its
`do` and evaluates to `val`. This is unanimous across Go, Rust, TS **and already documented
in the body** (`observe/wav-sense.fk`: *"let is two-arg only; binds for the rest of its do;
a three-arg (let n v body) drops body"*). The disagreeing side was `fkwu`, not the walkers.
A 3-arg `(let n v body)` is malformed — TS rejects it outright (`let: expected )`).

## Root cause — `fk_parse_top` lost the binding; `fk_parse_do` always had it

`fkwu` has two do parsers. `fk_parse_do` (nested dos, defn bodies) binds a do-let correctly
(tag 109 = store-then-rest). `fk_parse_top` (the source root) handled a top-level `(do …)`
by re-entering `fk_parse_top` per inner form — and `fk_parse_top` has **no let-binding** and
overwrote `fk_root` each iteration. So at the top level the binding was dropped and BODY read
an unbound name (a list let → `len` 0; an int let → 0). Masked everywhere a do reached the
runtime through `fk_parse_do` (any non-root position); exposed only at the absolute root.

## The fix — `runtime/fkwu-uni.c`, `fk_parse_top` top-level `(do …)`

Leading `(defn …)` and a leading nested `(do …)` stay TRANSPARENT through `fk_parse_top`
(defn registration + the optable generator's nested-do are unchanged). The FIRST value-bearing
inner form hands the REST of the do to **`fk_parse_do`** — the parser that binds do-lets and
sequences forms. Routing only the value part through `fk_parse_do` is the whole change; a
do of only defns still leaves `fk_root` unset so the last defn becomes the root. The 2-arg
canonical decision is recorded in the `fk_sparse` let comment; that bare-value-position path
is left byte-identical so every prelude library is untouched.

## Gate — `cc -O2 -o fkwu runtime/fkwu-uni.c`, run on `--src` (no Go in the runtime)

**The new band — `observe/tests/let-binding-band.fk` — FOUR-WAY:**
```
observe/tests/let-binding-band.fk  ->  31   on  fkwu = go = rust = ts   (OLD fkwu = 0)
   1   a LIST let survives:  (len p)  == 2
 + 2   the list is intact at head:  (head p) == 10
 + 4   ...and at an interior index: (nth p 1) == 20
 + 8   an INT let survives:  (add q 1) == 6
 +16   a let bound FROM an earlier let chains: r=(add q 1), s=(add r 1), s == 7
```

**Reproduction cases — now FOUR-WAY:**
```
(do (let p (list 10 20)) (len p))        fkwu = go = rust = ts = 2
(do (let p 5) (add p 1))                 fkwu = go = rust = ts = 6
(do (do (let p (list 10 20)) (len p)))   fkwu = go = rust = ts = 2   (nested-do, also fixed)
```

**No regression — real known-good bands unchanged (patched == HEAD):**
```
model/tests/tensor-ir-band.fk     -> 15    fkwu = go = rust = ts   (the Stone 7 band)
substrate/tests/cell-type-band.fk -> 1013  fkwu = go = rust = ts
proof/four-way-run.tbl            -> 0  (FOUR-WAY)  · recipe42 -> 42 · four-way-verdict -> 11111
```

**Blast radius — a sweep of all 287 `.fk` on `--src`, patched vs HEAD:** only 6 files differ,
every one a top-level `(do …)` carrying lets that previously dropped to a wrong value and now
computes (`offer-ack-core`, `wav-sense`, `world-build`, `cross-cell-interface`, the new band,
`fourth-flatten-driver`). Zero both-nonzero regressions; prelude libraries (e.g.
`grammars/form-ontology-loader.fk`) and the optable generator's nested-do are byte-identical.

**Honest floor:** four-way on Mac via `--src` today (self-contained s-expr bands run directly;
the BML-core bands prove through the existing flatten harness). Platform rows (Windows /
Android) and the standard sovereignty receipt remain pending, as for every band.
