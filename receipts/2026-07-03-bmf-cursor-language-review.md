# 2026-07-03 -- BMF cursor language layer review

## Ground

The second bottom-up stdlib layer is the BMF cursor core:

- `form/form-stdlib/bmf-core.fk`
- `grammars/bmf-core.fk`

Those two files were byte-identical before this layer and remain
byte-identical after it. Standing gate for this layer: any edit to one copy
must touch the other copy and re-run:

```sh
cmp -s grammars/bmf-core.fk form/form-stdlib/bmf-core.fk; echo $?
```

Before implementation, the base witnesses held:

```text
bootstrap/freshness/native: 42, 55, 15, 11111
core-waist-language-band: 255
semantic-stdlib-band: 524287
defdata-band: 2047
```

## Pre-Review

Urs named the rule for language layers: avoid line grammars and tokenizers; the
body has the BMF cursor, and streaming cursor grammars are preferred.

Claude reviewed the layer from the supplied evidence and agreed that BMF cursor
core is the right next layer after `core.fk`: core names the string waist, and
the cursor is the first streaming surface reader above that waist. Claude called
the `line-grammar.fk` dependency an inverted layer: BMF cursor was borrowing
codepoint predicates from a line-oriented module. Claude preferred a small
codepoint-predicate stratum below both BMF and line grammar, or an interim
internalization with duplication risk named.

Grok grounded against the files and receipts. It also agreed that BMF cursor is
the correct layer two for the grammar/cursor spine. Grok recommended the interim
choice used here: internalize the four base codepoint predicates in `bmf-core`
itself, keep `core.fk` free of grammar-class predicates, add a queryable
manifest, and add a slim band that runs with `core.fk + bmf-core.fk` only.

## Investigation

The older BMF integration band listed a fat prelude:

```sh
cat form/form-stdlib/core.fk form/form-stdlib/json.fk \
    form/form-stdlib/cache.fk form/form-stdlib/form-ontology-loader.fk \
    grammars/line-grammar.fk form/form-stdlib/bmf-core.fk \
    form/form-stdlib/tests/bmf-core-band.fk > /tmp/bmf-core-fat.fk
./fkwu --src /tmp/bmf-core-fat.fk
```

Witness:

```text
fk_smknode: program too large for the AST node table
```

Removing `json/cache` was not enough. The ontology-emission and recursive
grammar witnesses still hit the same AST ceiling. This is a red, pre-existing
integration witness, not a passing deferred item. It is recorded here as an
ontology/prelude sizing failure, while the cursor layer gets its own slim
witness that does not load that path.

## What Changed

- `form/form-stdlib/bmf-core.fk`
- `grammars/bmf-core.fk`

Both gained:

- a queryable BMF cursor language manifest:
  - `scannerless`
  - `streaming-cursor`
  - `immutable-cursor`
  - `checkpoint-restore`
  - `pattern-data`
  - `template-data`
  - `no-token-stream`
  - `no-line-grammar`
- base codepoint predicates local to BMF cursor core:
  - `is-ws-cp`
  - `is-digit-cp`
  - `is-alpha-cp`
  - `is-ident-cont-cp`

`form/form-stdlib/tests/bmf-cursor-language-band.fk` was added as the focused
layer witness. It runs without `line-grammar`, tokenizers, `json`, `cache`, or
`form-ontology-loader`.

The manifest is declared in full, but the slim band only queries four manifest
entries directly. The other manifest entries are covered only where there is a
matching behavior bit (`immutable-cursor`, `checkpoint-restore`,
`pattern-data`) or remain declared/deferred for this layer (`template-data`).

## Alternatives

| Alternative | Disposition | Why |
| --- | --- | --- |
| Keep `line-grammar.fk` as a BMF cursor dependency | Rejected | It inverts the layer: cursor core should not depend on line parsing for base codepoint facts. |
| Add a tokenizer or line-token stream before BMF | Rejected | The desired abstraction is streaming cursor over a surface, not tokenized lines. |
| Move character predicates into `core.fk` | Rejected for now | Core just established itself as vocabulary/waist, not a grammar-class layer. |
| Add a separate `byte-class.fk` / codepoint stratum | Deferred | Cleaner long-term deduplication for BMF and line grammars, but one more prelude file now. |
| Keep two divergent BMF core copies | Rejected | The `form/form-stdlib` and `grammars` copies remain byte-identical in this layer. |
| Treat the fat ontology band as the layer-2 witness | Rejected for this layer | It mixed cursor semantics with ontology emission. A later source-runner repair made the integration band runnable again, but the narrow cursor witness remains the correct layer-2 proof. |
| Enlarge the C seed AST table | Rejected here | New runtime meaning belongs in Form/native walker cells; sizing needs a separate shrink/ontology strategy, not an ad hoc C growth. |

## Witness

```sh
cat form/form-stdlib/core.fk form/form-stdlib/bmf-core.fk \
    form/form-stdlib/tests/bmf-cursor-language-band.fk > /tmp/bmf-cursor.fk
./fkwu --src /tmp/bmf-cursor.fk
```

```text
1023
```

Bit decoding:

```text
1     manifest declares scannerless
2     manifest declares streaming-cursor
4     manifest declares no-token-stream invariant
8     manifest declares no-line-grammar invariant
16    cursor peeks the first byte/codepoint of the surface
32    advance returns a new cursor and leaves the old cursor at pos 0
64    digit run capture consumes "12" and ends at pos 2
128   alternation backtracks with no sediment on the source cursor
256   checkpoint/restore returns to the saved coordinate
512   base codepoint predicates are live in BMF core
```

Bits `4` and `8` are architecture invariants plus slim-prelude construction:
the band proves this witness path runs without importing a token stream or line
grammar. It does not prove global absence of line grammar files elsewhere in
the repository.

Copy integrity:

```sh
cmp -s grammars/bmf-core.fk form/form-stdlib/bmf-core.fk; echo $?
```

```text
0
```

## Deferred

- Universal NodeID emission through `bp` and `intern_node`.
- Recursive grammar engine features in `bmf-grammar.fk`: `ref`, `rep`, `sep`,
  node captures, precedence, and `g-parse`.
- Ontology/prelude sizing for the old integration bands was later repaired in
  the source runner; keep it out of the layer-2 witness anyway because it mixes
  cursor semantics with ontology emission.
- A shared lower `byte-class.fk` or equivalent codepoint-predicate stratum that
  removes duplication between BMF cursor and legacy line grammar.
- Until that stratum exists, the duplicated codepoint predicates in BMF and
  legacy line grammar are an explicit drift debt.
- Migration of legacy line grammars to streaming BMF cursor grammars.
- Semantic-stdlib observation surfaces wired through BMF cursor grammar.

## Post-Review

Grok post-reviewed the implemented layer read-only and reproduced the important
witnesses: copy integrity remained `0`, the slim cursor witness returned
`1023`, the fat ontology band still failed with `fk_smknode: program too large
for the AST node table`, and no BMF core import of `line-grammar` was present.
Grok found no blocker bug, but asked for the manifest proof boundary and the
template-emission wording to be tightened.

Claude post-reviewed from the supplied summary without tools. Claude agreed the
layer can close before `bmf-grammar`, with no code blocker. Claude required
three receipt corrections: call the fat ontology band red rather than merely
deferred, name the two-file sync gate, and flag predicate duplication until the
shared byte-class stratum exists.

Those corrections are now recorded here. The layer is closed as a cursor-layer
witness, not as a universal grammar/emission witness.
