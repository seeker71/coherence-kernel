# Receipt — source-runner constants, root `do`, and Form-owned bootstrap `bp` (2026-07-03)

## Why

The `make_nodeid` parser repair removed the AST-cap symptom from
`form-ontology-loader.fk`, but it exposed deeper source-runner issues:

- top-level `(let NAME VALUE)` did not bind across later top-level forms;
- `bp` in `fkwu --src` was a native identity stub, so `(bp "add")` returned the
  string `"add"`, and `node_type` / `node_inst` over that value returned `0`.
- after fat preludes, a value-bearing top-level `(do ...)` inherited parser
  binding state from earlier prelude `defn` / `do` forms. Root locals were then
  assigned/read through stale slots; the minimal cursor repro read `s-peek` back
  as `5` instead of `100`, and the older BMF integration band scored `2055`
  instead of its historical `600`.

That meant the ontology loader could parse while still failing semantically. A
green parse was not a live ontology, and a green narrow cursor band was not a
live integration witness.

## What changed

- `runtime/fkwu-uni.c` now carries a small module-constant table for direct
  source runs. Top-level `(let NAME VALUE)` records a module constant; later
  top-level forms and function bodies can resolve it. Lexical bindings still
  shadow module constants.
- `runtime/fkwu-uni.c` widens the top-level function-name lookup table from
  `256` entries to `FK_FN_CAP`. The old smaller table let fat preludes define
  function bodies past the lookup cap; later calls to those names became
  unresolved offers returning `nothing`.
- `runtime/fkwu-uni.c` resets the parser binding table before the root
  value-sequence inside a top-level `(do ...)`, then wraps the root in a reserve
  node when that sequence used local slots. Defn scanning remains transparent,
  but the first real value sequence now owns its own slots.
- `runtime/fkwu-optable.h` no longer registers native `bp` as tag `45`. That
  removes the silent identity behavior from the direct source runner.
- `form/form-stdlib/form-ontology-loader.fk` and
  `grammars/form-ontology-loader.fk` now define a Form-owned bootstrap `bp`
  floor for reviewed bootstrap rows. It includes the 23 kernel ontology rows
  from `form-ontology.json` plus the program-image recipe carrier,
  typed-literal, and typed program-image rows needed by current carrier bands.
  Unknown names return `nothing` for now instead of a string or a fake zero
  NodeID.
- Dialect/user blueprint binding materialization is deferred until the full
  blueprint registry has a Form-owned data path. Binding those names through
  the old identity stub would have materialized strings as if they were
  NodeIDs.
- `form/form-stdlib/tests/source-runner-root-do-band.fk` pins the root
  value-sequence slot bug with `core.fk`, `json.fk`, and `bmf-core.fk` loaded
  first.
- `form/form-stdlib/tests/form-ontology-parity-band.fk` is back to the generic
  `sum-checks table check` shape. It now witnesses ontology coordinates, the
  runner's indirect-call path, and representative non-core bootstrap rows.

## Proof

Fresh build:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
```

The build still emits the pre-existing `fread` header warning and
`getsockname` pointer-sign warning.

Required checkout witnesses:

```sh
./fkwu --src bootstrap/ground.fk                                      # 42
./fkwu --src bootstrap/ground-recursive.fk 10                         # 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk          # 15
./fkwu --src <(cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)') # 11111
```

Targeted witnesses:

```sh
printf '(let X 40)\n(add X 2)\n' | ./fkwu --src /dev/stdin            # 42
printf '(let X 40)\n(defn f () (add X 2))\n(f)\n' | ./fkwu --src /dev/stdin # 42
printf '(let X 40)\n(defn f () (do (let X 5) (add X 2)))\n(f)\n' | ./fkwu --src /dev/stdin # 7
```

Ontology witnesses:

```sh
cat form/form-stdlib/form-ontology-loader.fk; echo '(add (node_type (bp "add")) (node_inst (bp "add")))'
# through ./fkwu --src => 13

cat form/form-stdlib/form-ontology-loader.fk; echo '(nothing? (bp "OUTER"))'
# through ./fkwu --src => 1

cat form/form-stdlib/core.fk \
    form/form-stdlib/form-ontology-loader.fk \
    form/form-stdlib/source-compiler.fk \
    form/form-stdlib/tests/form-ontology-parity-band.fk \
    | ./fkwu --src /dev/stdin
# 1506
```

Layer witness still green:

```sh
cat form/form-stdlib/core.fk \
    form/form-stdlib/bmf-core.fk \
    form/form-stdlib/tests/bmf-cursor-language-band.fk \
    | ./fkwu --src /dev/stdin
# 1023
```

Root `do` and integration witnesses:

```sh
cat form/form-stdlib/core.fk \
    form/form-stdlib/json.fk \
    form/form-stdlib/bmf-core.fk \
    form/form-stdlib/tests/source-runner-root-do-band.fk \
    | ./fkwu --src /dev/stdin
# 31

cat form/form-stdlib/core.fk \
    form/form-stdlib/json.fk \
    form/form-stdlib/cache.fk \
    form/form-stdlib/form-ontology-loader.fk \
    form/form-stdlib/line-grammar.fk \
    form/form-stdlib/bmf-core.fk \
    form/form-stdlib/tests/bmf-core-band.fk \
    | ./fkwu --src /dev/stdin
# 600
```

`git diff --check` is clean.

## Reviews

Pre-review:

- Claude accepted the split with modifications: land module constants first,
  then phase `bp` so the tree can distinguish binding failure from registry
  failure.
- Grok pre-review stalled with no usable output and was interrupted. This is
  recorded as review-tool friction, not approval.

Post-review:

- Grok accepted this as a source-runner plus ontology-loader repair, not as
  BMF integration completion or source-runner-admission completion.
- Grok accepted the root-`do` repair as a root-aligned checkout-witness fix,
  with the requirement that it be recorded as shrink debt and pinned by a band.
- Claude post-review stalled with no usable output and was interrupted. This is
  recorded as review-tool friction, not approval.
- 2026-07-04 Grok/Huygens and Claude/Bacon re-reviewed the current guide and
  returned `PASS_WITH_CHANGES`. They required loud module-constant overflow,
  focused module-constant and `make_nodeid` bands, honest bootstrap `bp` naming,
  and closure receipts. The follow-up is recorded in
  `receipts/2026-07-04-source-runner-cseed-guide-review.md`.

## Deferred / not claimed

- Full blueprint registry: only reviewed bootstrap rows are Form-owned here.
  User/dialect blueprint names still need a Form data path sourced from
  `blueprint-registry.json` without adding a giant C table or another host
  generator dependency.
- Cross-kernel temporal semantics for a function compiled before a duplicate
  top-level constant overwrite are not claimed. The focused module-constant band
  pins the shared behavior for top-level constants, later function access, local
  shadowing, duplicate overwrite for later forms, and stability of already
  materialized top-level constants.
- Unknown `bp` names return `nothing`, not a loud `form_error`. The current
  `fkwu --src` optable does not expose `form_error`; loud failure needs a real
  Form-native error/diagnostic path, not a fake node.
- `form/form-stdlib/source-runner-admission.fk` was updated in a follow-up
  layer pass to record the repaired current snapshot: ontology, defdata module
  constants, root `do`, and BMF integration are green, and the current route is
  direct admission.
- A malformed extra `)` in the first draft of the Form-owned `bp` body stalled
  the direct-source runner instead of failing structurally. That is a parser
  observability gap.
- The low-level authoring surface is still too exposed. This repair makes
  `let`, `defn`, module constants, root `do`, and indirect calls honest enough
  for checkout witnessing; it does not yet provide the layer-appropriate
  semantic languages the stdlib should be written in.

## Shrink path

The C edits are checkout-witness repairs, not the destination. The constant
table, widened lookup, and root-`do` slot isolation make `--src` honest enough
to run the body while the real direction remains:

- constants/data literals become Form/source-compiler artifacts that can be
  cached as `.fkb`;
- blueprint registry data becomes Form-owned table/recipe data;
- root source parsing becomes a Form-owned compiler/load artifact rather than
  C parser state;
- high stdlib layers are authored in their own observable streaming grammars,
  lowering to the core waist instead of forcing every layer to write raw
  `let`/`defn` sequences;
- the C seed stops owning semantic registry meaning and continues shrinking.
