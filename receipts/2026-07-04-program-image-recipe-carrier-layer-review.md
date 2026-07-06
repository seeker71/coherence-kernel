# 2026-07-04 -- Program-image recipe carrier layer review

## Layer

Layer 8h1: `program-image-recipe-carrier.fk`.

This layer sits between the 8h program-image `.fkb` envelope and later binary
cache/runtime admission work. It converts a valid 8h envelope into registered
NodeID recipe-carrier data that can be inspected structurally. It does not make
binary `.fkb` IO, disk cache loading, table execution, runtime walking, or
native calls true.

## Review Before Build

Two local reviewer agents were used because literal Grok/Claude endpoints were
not exposed in this Codex thread:

- Volta, acting as the Grok reviewer: `PASS_WITH_CHANGES`
- Lorentz, acting as the Claude reviewer: `PASS_WITH_CHANGES`

The combined gate required registered blueprint categories, explicit collection
nodes for table sections, trivial leaves only, a carrierable numeric gate,
malformed rows as `refused`, invalid/non-carrierable rows as `investigate`, no
runtime/table/load claims, a mirror, static boundary scans, and focused
negative tests.

## Implemented

- Added seven registered categories from `(8,45,6,40)` through `(8,45,6,46)`
  for the recipe envelope, table, function roots, node rows, node row, string
  rows, and string row.
- Added `form/form-stdlib/program-image-recipe-carrier.fk` and grammar mirror.
- Added `form/form-stdlib/tests/program-image-recipe-carrier-band.fk`.
- Valid carriers preserve source/artifact identity, seal bit, table counts,
  function roots, node rows, and string byte rows as NodeID children.
- Carrierability now uses exact `value_kind` names where the sibling kernels
  provide them and falls back to the current `fkwu` atom floor where they do
  not. The fallback accepts `len == 0` atoms and rejects non-empty lists, but it
  cannot honestly distinguish positive raw integers from string-table indices
  after strings are interned. Exact int-vs-string carrierability therefore
  remains a sibling-proven contract until the native body gets a real raw kind
  primitive or typed literal carrier.
- Added manual bp-table rows for Go/Rust/TypeScript because the referenced root
  generator/scanner scripts were not present in this worktree.
- Moved the PIRC coordinates after post-review: the first version occupied
  `(8,45,6,20)..(8,45,6,26)`, colliding with `runtime-grammar.fk` at
  `(8,45,6,20)`. PIRC now uses `(8,45,6,40)..(8,45,6,46)`.
- Added a PIRC-owned primitive-kind preflight before any `pif-envelope-valid?`
  call. This prevents wrong primitive kinds from crashing inside 8h typed
  validators; table kind failures now return `investigate` +
  `non-carrierable-table`, while semantic count/arity/range failures still
  return `invalid-pif-envelope`.

## Neighbor Repairs

The neighboring sweep exposed real pre-existing defects:

- `program-image-tbl-emit-band.fk` read a missing file as text; Go accepted it,
  while Rust/TypeScript correctly treated it as null/non-string. The band now
  checks missing-file status by stat size before reading content.
- `runtime-table-text-attempt.fk` had public functions nested under prior defns
  because closes were at the file end instead of defn boundaries. Source and
  grammar mirrors were repaired.
- `runtime-artifact-handoff.fk` had the same boundary-close issue. Source and
  grammar mirrors were repaired.
- `source-compiler-emission.fk` was short one real close. Source and grammar
  mirrors were repaired.
- `runtime-artifact-handoff-band.fk` had a nested mirror test and a deep
  static-boundary assertion that called `rahb-score` with one argument. It now
  uses an explicit forbidden-needle helper.

These were not caused by 8h1, but they are on the same route and blocked
meaningful neighboring verification.

Post-review blocker fixed:

- Claude review found that `pirc-carrier-from-envelope` still called
  `pif-envelope-valid?` before a PIRC type/carrierability preflight. A shaped
  table with string byte `"65"` could therefore crash in `pif-byte?` instead of
  returning a PIRC investigation row. PIRC now preflights envelope metadata and
  table primitive kinds first. The band now covers a non-int string-byte cell
  and a non-string source-path field.
- Claude follow-up found that even `pif-envelope?` and `pif-table?` are not
  kind-safe when their tag cell is an integer. PIRC now has its own tag-safe
  `pirc-envelope-shape?` and `pirc-table-shape?` guards before any 8h tag
  predicate is reached. The band covers a non-string envelope tag as
  `refused/malformed-pif-envelope` and a non-string table tag as
  `investigate/non-carrierable-table`.
- Grok follow-up found that a direct `fkwu` valid-table probe still could not
  prove carrier readiness: positive raw ints collide with list/string cells, so
  valid table counts such as `2` do not pass a trustworthy native int gate.
  PIRC now makes this executable. If exact raw `value_kind` names are absent,
  public carrier admission returns `investigate/native-raw-kind-floor` with
  `pirc-no-recipe` instead of attempting to classify or build the carrier.

## Deferred

- Real disk `.fkb` writing, reading, freshness selection, and runtime loading.
- `walk_recipe` or `walk_recipe_here` admission for the produced carrier.
- Table-text execution from this layer.
- Runtime attempt/observation production.
- Selector installation.
- C-seed growth.
- Full fourth-arm/fkwu registration for this band. The focused sibling
  validation crosses Go/Rust/TypeScript. `fkwu` can load the PIRC source and
  prove the current atom fallback, but full direct-source concatenation of core,
  ontology, 8h, 8h1, and reason coverage still hits the current source AST
  ceiling.
- A native exact raw-kind primitive or typed literal carrier. This is the
  reason `.tbl` is not yet honestly folded all the way into a reusable `.fkb`
  cache: `fkwu` raw positive integers and string-table indices are ambiguous
  after string interning.
- Restoring the missing blueprint generator/scanner workflow. This worktree has
  `form/scripts/*`, but not the root `scripts/gen_bp_table.py` or
  `scripts/scan_form_blueprints.py` paths referenced by the workflow messages.

## Stall/AST Lesson

A malformed carrier file with one extra `)` stalled `fkwu --src` instead of
reporting a clean parse error. A full direct-source concatenation of core,
ontology, 8h, and 8h1 also fails with:

```text
fk_smknode: program too large for the AST node table
```

That is the same architectural lesson as the earlier `head`/small-output stall:
do not treat short output or early rows as proof of completion. Reader stalls
and AST ceilings are runtime facts to investigate. The right direction remains
smaller compiled artifacts and cached recipe/data images, not larger
concatenated source bundles or a larger C seed.

The post-review type-gate investigation found a second core lesson:
`str_len` is not a stable fkwu raw-int predicate after source with string
literals is loaded. Positive integers can overlap string-table indices, so
`str_len 7` may observe a string cell instead of proving non-string-ness. The
PIRC fallback therefore uses only the honest current atom floor (`len == 0`) in
`fkwu`, while Go/Rust/TypeScript enforce exact `value_kind == "int"`.

## Verification

Focused:

```text
./validate.sh form-stdlib/tests/program-image-recipe-carrier-band.fk
-> 2147483647
```

Focused `fkwu` source-loaded atom-floor probe:

```text
PIRC source + pirc-int-carrierable? probes -> 15
```

This `15` means the current native fallback accepts integer atoms, also accepts
the string atom `"7"` because `fkwu` lacks exact raw kind names, and rejects a
non-empty list. That is evidence of the named floor, not evidence that exact
carrierability is solved in `fkwu`.

Focused `fkwu` public carrier-admission probe:

```text
PIRC source + valid-shaped envelope + malformed envelope -> 31
```

This proves the current direct-native behavior does not claim carrier-ready:
valid-shaped input returns `investigate/native-raw-kind-floor` with
`pirc-no-recipe`, and malformed input still returns
`refused/malformed-pif-envelope`, without the previous stall.

Neighbor route, run from repo root so existing `read_file "form/..."` checks
resolve as authored:

```text
program-image-fkb -> 2147483647
program-image-tbl-emit -> 2147483647
program-image-table-text-witness -> 2147483647
runtime-table-text-attempt -> 2147483647
runtime-artifact-handoff -> 2147483647
form-ontology-parity -> 1497
```

Final bootstrap rerun is recorded in the turn transcript.

Post-review final bootstrap:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
./fkwu --src /tmp/nvr.fk -> 11111
```
