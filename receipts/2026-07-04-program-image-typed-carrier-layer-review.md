# 2026-07-04 -- Program-image typed carrier layer review

## Layer

Layer 8h3: `program-image-typed-carrier.fk`.

This layer consumes a program-image envelope/table expressed as registered
NodeID cells whose scalar values are typed-literal wrappers. It validates that
typed input, normalizes valid input into the raw Layer 8h PIF envelope, and
returns the existing PIRC carrier row shape with a directly built PIRC recipe.

It exists because Layer 8h1 honestly exposed a native floor: current direct
`fkwu` cannot reliably classify raw positive integers after source loading, so
raw PIRC admission must remain `investigate/native-raw-kind-floor`. Layer 8h2
added typed literal wrappers; Layer 8h3 is the first program-image consumer of
that typed data language.

## Review Before Build

Two existing local reviewer agents were reused because literal Grok/Claude
endpoints are not exposed in this Codex thread:

- Jason, acting as the Grok reviewer: `PASS_WITH_CHANGES`
- Popper, acting as the Claude reviewer: `PASS_WITH_CHANGES`

The combined gate required:

- valid output must return the existing `pirc-carrier-row` shape;
- the carrier envelope slot must hold the normalized raw 8h PIF envelope, not
  the typed input node;
- the layer must not call `pirc-carrier-from-envelope`;
- raw PIRC admission must remain the named native floor;
- sibling kernels must guard with exact NodeID kind before `node_*` access;
- reason partitioning must be explicit:
  `refused/malformed-pif-envelope`,
  `investigate/invalid-pif-envelope`, and
  `investigate/non-carrierable-table`;
- registry, FOL, Go, Rust, and TypeScript bp rows must exist for
  `(8,45,6,60..66)`;
- the architecture map must name 8h2 and 8h3 explicitly;
- tests must include direct `fkwu` probes for typed carrier readiness and raw
  PIRC non-readiness.

## Implemented

Files:

- `form/form-stdlib/program-image-typed-carrier.fk`
- `grammars/program-image-typed-carrier.fk`
- `form/form-stdlib/tests/program-image-typed-carrier-band.fk`
- `receipts/2026-07-03-core-layer-architecture-map.md`
- registry/FOL/bp-table rows in the Form, Go, Rust, and TypeScript surfaces

Registered categories:

```text
PROGRAM-IMAGE-TYPED-ENVELOPE     (8,45,6,60)
PROGRAM-IMAGE-TYPED-TABLE        (8,45,6,61)
PROGRAM-IMAGE-TYPED-FN-ROOTS     (8,45,6,62)
PROGRAM-IMAGE-TYPED-NODE-ROWS    (8,45,6,63)
PROGRAM-IMAGE-TYPED-NODE-ROW     (8,45,6,64)
PROGRAM-IMAGE-TYPED-STRING-ROWS  (8,45,6,65)
PROGRAM-IMAGE-TYPED-STRING-ROW   (8,45,6,66)
```

Main constructors:

- `pitc-fn-roots`
- `pitc-node-row`
- `pitc-string-row`
- `pitc-node-rows`
- `pitc-string-rows`
- `pitc-table-node`
- `pitc-table-from-sections`
- `pitc-envelope-node`

Main admission function:

- `pitc-carrier-from-typed-envelope`

Valid input produces:

```text
pirc-carrier-row
  normalized raw PIF envelope
  pirc-envelope-recipe(normalized raw PIF envelope)
  carrier-ready
  carrier-ready
```

Invalid input is partitioned:

- malformed top-level typed envelope node/category/arity:
  `refused/malformed-pif-envelope`;
- metadata typed-kind failures, bad version/path/hash/mtime/seal, count
  mismatch, row arity, and byte range:
  `investigate/invalid-pif-envelope`;
- table collection/category or typed-cell carrierability failures:
  `investigate/non-carrierable-table`.

## Investigation Trail

The first 8h3 band attempt had an unclosed `do` form:

```text
cd form && ./validate.sh form-stdlib/tests/program-image-typed-carrier-band.fk
-> parse error: unclosed `(` opened at line 1 col 1
```

A local balance check showed `balance 1`. The missing close was repaired.

The next failure was a test bug, not a layer bug:

```text
node_eq: expected NodeID args
```

The band compared a normalized raw PIF list with the typed input NodeID using
`node_eq`. The assertion now proves the intended boundary by checking the typed
input category and the raw output PIF envelope shape.

The next direct `fkwu` investigation found a current file/kind floor:

```text
direct fkwu value_kind(read_file(...)) == "string" -> 0
```

Direct `fkwu` can read the file text, but does not report that text as
`value_kind == "string"` from the repo-root static helper. Sibling kernels
return `Null` for a missing `read_file`, so `str_len` is not safe as a
cross-kernel missing-file guard. The band keeps the null-safe `value_kind`
path choice and the full direct-native band is run from `form/`, where the
fallback path is the valid one.

This is recorded as a current observability quirk, not a runtime claim.

## Verification

Bootstrap:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
# existing fread and getsockname warnings only
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
native-vs-rented-check -> 11111
```

Focused 8h3 band:

```text
cd form && ./validate.sh form-stdlib/tests/program-image-typed-carrier-band.fk
-> 16777215
```

Direct native full band:

```text
cd form && ../fkwu --src /tmp/pitcb-full-direct.fk
-> 16777215
```

Direct native behavior probes:

```text
typed program-image input -> carrier-ready raw PIF envelope + recipe -> 127
raw PIRC envelope -> investigate/native-raw-kind-floor + no recipe -> 63
```

Neighbor checks:

```text
typed-literal-carrier-band -> 8388607
program-image-recipe-carrier-band -> 2147483647
program-image-fkb direct concat band -> 2147483647
form-ontology-parity-band -> 1497
```

Follow-up repair:
`receipts/2026-07-04-program-image-band-path-hygiene-repair.md` closed the
older program-image band prelude-path caveat. The standard validator path now
returns full masks:

```text
program-image-fkb-band -> 2147483647
program-image-tbl-emit-band -> 2147483647
program-image-table-text-witness-band -> 2147483647
```

This was a band-path hygiene repair only; it did not change the 8h3 layer or
the 8h/8i/8i1 implementation files.

Static and mirror checks:

```text
cmp form/form-stdlib/program-image-typed-carrier.fk grammars/program-image-typed-carrier.fk -> 0
cmp form/form-stdlib/typed-literal-carrier.fk grammars/typed-literal-carrier.fk -> 0
cmp form/form-stdlib/program-image-recipe-carrier.fk grammars/program-image-recipe-carrier.fk -> 0
forbidden 8h3 source/mirror scan -> no hits
```

Forbidden scan covered:

```text
pirc-carrier-from-envelope
runtime-artifact-attempt
runtime-table-text-attempt
source-artifact-probe-observation
sap-observation
rao-obs
runtime-artifact-selector
ras-selection
line-grammar
g-match-rule
cursor-str
write_form_binary
read_form_binary
recipe_to_bytes
bytes_to_recipe
walk_recipe
fk_run
dlopen
dlsym
```

## Deferred

- Raw PIRC admission changes. Raw Layer 8h envelopes still return
  `investigate/native-raw-kind-floor` on direct `fkwu`.
- Binary `.fkb` write/read and freshness-based runtime selection.
- `.tbl` text parsing or table execution.
- Program-image load/walk/call.
- Runtime attempts or observations.
- Selector installation.
- Any C-seed growth.
- Turning typed literals into source syntax. This layer is a NodeID data
  language, not a parser or line grammar.

## Post-Review

Jason, acting as Grok reviewer: `PASS`.

Jason found no blocking issues. He checked that valid input returns the existing
PIRC row with the normalized raw PIF envelope in the envelope slot, that the
recipe is built directly with `pirc-envelope-recipe raw`, that
`pirc-carrier-from-envelope` is not called, that reason partitioning matches
the requested split, that `(8,45,6,60..66)` registry/FOL/sibling rows are
present, and that the architecture map now names both 8h2 and 8h3. He reran:

```text
program-image-typed-carrier-band -> 16777215
typed-literal-carrier-band -> 8388607
program-image-recipe-carrier-band -> 2147483647
```

Popper, acting as Claude reviewer: `PASS`.

Popper found no blocking issues. He checked the normalized raw PIF envelope
boundary, absence of `pirc-carrier-from-envelope`, malformed/invalid/
non-carrierable partitioning, registry/FOL/sibling rows, and the architecture
map ownership boundaries. He reran:

```text
program-image-typed-carrier-band -> 16777215
typed-literal-carrier-band -> 8388607
program-image-recipe-carrier-band -> 2147483647
form-ontology-parity-band -> 1497
direct full native band -> 16777215
direct typed probe -> 127
direct raw PIRC probe -> 63
```

Both reviewers scoped their PASS to the 8h3 files and registration surfaces;
the broader worktree remains dirty with unrelated work.
