# 2026-07-04 -- Typed literal carrier layer review

## Layer

Layer 8h2: `typed-literal-carrier.fk`.

This layer introduces explicit NodeID wrappers for literal data:

- `TYPED-LITERAL-INT` at `(8,45,6,50)`
- `TYPED-LITERAL-STRING` at `(8,45,6,51)`
- `TYPED-LITERAL-LIST` at `(8,45,6,52)`

The layer exists because 8h1/PIRC exposed a real native floor: current direct
`fkwu` cannot reliably distinguish positive raw integers from string/list cells
after source loading. Typed literal wrappers let later layers carry values by
wrapper category plus trivial child node type instead of raw kind guessing.

## Review Before Build

Two local reviewer agents were used because literal Grok/Claude endpoints were
not exposed in this Codex thread:

- Mill, acting as the Grok reviewer: `PASS_WITH_CHANGES`
- Socrates, acting as the Claude reviewer: `PASS_WITH_CHANGES`

The combined gate required:

- Do not claim PIRC admission is fixed in this layer.
- Use exact `value_kind == "node_id"` guards in sibling kernels before any
  `node_*` access.
- In direct `fkwu`, use category/shape evidence rather than raw-kind guessing,
  and name the remaining raw-negative alias floor.
- Admit both small and wide trivial int child node types (`1` and `5`).
- Keep malformed tests sibling-safe by using malformed NodeID structures, not
  raw children inside `intern_node`.
- Make accessors total or explicitly preconditioned.
- Add registry/FOL/Go/Rust/TS bp-table surfaces and mirror/static checks.

## Implemented

- Added `form/form-stdlib/typed-literal-carrier.fk` and exact grammar mirror.
- Added `form/form-stdlib/tests/typed-literal-carrier-band.fk`.
- Added registry rows and Form/sibling bp table rows for `(8,45,6,50..52)`.
- `tlit-int`, `tlit-string`, and `tlit-list` construct typed wrapper nodes.
- `tlit-int?`, `tlit-string?`, `tlit-list?`, and recursive `tlit?` validate by
  wrapper category and trivial child node type, not raw value kind.
- `tlit-int-child-type?` accepts both child node type `1` and `5`.
- Child type helpers are also guarded, so raw values passed directly to helper
  functions reject in strict siblings before `node_type` is called.
- `tlit-int-value`, `tlit-string-value`, and `tlit-list-items` are total
  sentinel accessors: invalid input returns `0`, `""`, or `(empty)`.
- The manifest names `direct-fkwu-raw-negative-nodeid-alias-floor` so direct
  native tests do not overclaim raw negative rejection.

## Deferred

- Integrating typed literals into `program-image-fkb.fk` or
  `program-image-recipe-carrier.fk`.
- Changing PIRC direct `fkwu` admission. 8h1 still returns
  `investigate/native-raw-kind-floor` until a later integration layer consumes
  typed literal data.
- Binary `.fkb` writing, reading, freshness selection, loading, walking, or
  selector installation.
- Parser/source-language syntax for typed literals.
- Any C-seed growth.

## Verification

Focused:

```text
cd form && ./validate.sh form-stdlib/tests/typed-literal-carrier-band.fk
-> 8388607
```

Direct `fkwu` source-loaded probe with a minimal `bp` mapping:

```text
typed literal int/string/list + malformed child-type probe -> 127
```

Mirror and registry:

```text
cmp form/form-stdlib/typed-literal-carrier.fk grammars/typed-literal-carrier.fk -> 0
TYPED-LITERAL-INT    8 45 6 50
TYPED-LITERAL-STRING 8 45 6 51
TYPED-LITERAL-LIST   8 45 6 52
duplicate-coordinate check -> no rows
```

Neighbor checks:

```text
program-image-recipe-carrier-band -> 2147483647
form-ontology-parity-band -> 1497
```

Checkout witness:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
./fkwu --src /tmp/nvr.fk -> 11111
```

The C compile still reports the existing `fread` declaration warning and
`getsockname` pointer-sign warning; this layer did not grow the C seed.

## Review After Build

- Mill, acting as Grok reviewer: `PASS`
- Socrates, acting as Claude reviewer: `PASS`

Post-review specifically checked the strict sibling guard paths, child helper
guarding before `node_type`, registry surfaces, mirror parity, direct `fkwu`
floor naming, and that this layer does not hide a PIRC readiness claim.
