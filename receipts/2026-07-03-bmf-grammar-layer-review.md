# 2026-07-03 -- BMF grammar waist layer review

## Ground

Layer 3 is the recursive BMF grammar waist:

- `form/form-stdlib/bmf-grammar.fk`
- `grammars/bmf-grammar.fk`
- `form/form-stdlib/tests/bmf-grammar-band.fk`

Layer 2 already closed the scannerless cursor and single-rule matcher. This
layer adds grammar-as-data fixpoint behavior: named rules, recursive `ref`,
`rep`/`sep`, captures, and template `emit`/`splice`/`splice*`.

## Pre-Review

Claude reviewed the evidence summary without tools. Claude identified the
candidate file as two layers wearing one file: the true waist is named
productions with recursion, repetition, separators, captures, and templates;
expression sugar (`str`, `char`, `num`, `infix`, `chain`, `ternary`) belongs
above that waist. Claude warned that a manifest-only pass would overclaim if
the engine itself could not load.

Grok reviewed against the repository. Grok reproduced that `core.fk +
bmf-core.fk` loads, but adding the old `bmf-grammar.fk` failed during source
parse with:

```text
fk_smknode: program too large for the AST node table
```

Grok also confirmed the two BMF grammar copies were byte-identical, and that
the old `line-grammar` dependency was comment drift rather than a live call
inside `bmf-grammar.fk`.

## Investigation

The failure was not ignored and was not treated as ordinary test noise. These
commands all failed before execution with the same AST table error:

```sh
cat form/form-stdlib/core.fk form/form-stdlib/json.fk \
    form/form-stdlib/cache.fk form/form-stdlib/form-ontology-loader.fk \
    form/form-stdlib/line-grammar.fk form/form-stdlib/bmf-core.fk \
    form/form-stdlib/bmf-grammar.fk \
    form/form-stdlib/tests/bmf-grammar-band.fk > /tmp/bmf-grammar-fat.fk
./fkwu --src /tmp/bmf-grammar-fat.fk

cat form/form-stdlib/core.fk form/form-stdlib/form-ontology-loader.fk \
    form/form-stdlib/bmf-core.fk form/form-stdlib/bmf-grammar.fk \
    form/form-stdlib/tests/bmf-grammar-band.fk > /tmp/bmf-grammar-slim.fk
./fkwu --src /tmp/bmf-grammar-slim.fk

cat form/form-stdlib/core.fk form/form-stdlib/bmf-core.fk \
    form/form-stdlib/bmf-grammar.fk > /tmp/bmf-grammar-only.fk
./fkwu --src /tmp/bmf-grammar-only.fk
```

Even `./fkwu --src form/form-stdlib/bmf-grammar.fk` failed on the old
monolith. That isolated the blocker to the file's own source shape, not to
`json`, `cache`, `line-grammar`, the ontology loader, or the test band.

`FK_AST_NODE_CAP` is `65536` in `runtime/fkwu-uni.c`. It was not increased.

## What Changed

`bmf-grammar.fk` was reduced from a 400-line monolithic engine to a 167-line
recursive grammar waist. The mirrored copies remain byte-identical.

Kept in layer 3:

- `grammar`, `rule3`, rule lookup, and `g-parse`.
- `ref` recursion through `g-match-rule`.
- `lit` and `run`, delegated to the already-witnessed BMF cursor matcher.
- `seq`, `alt`, `opt`, `cap`, `rep`, and `sep`.
- `emit`, `splice`, `splice*`, and `const` templates.
- A queryable `bmf-grammar-language-manifest`.

Removed from this layer and deferred upward:

- `str`, `char`, `num`.
- `infix`, `chain`, `ternary`.
- `t-splice-int`, `t-const-int`, `t-const-bool`.
- `intern_node_at` source attribution.

During shrink, `rep`/`sep` initially used `cons` plus `reverse-acc` over
NodeID values. That lost the ordinary list shape in this runtime. The fix was
to accumulate with `append acc (list value)`. This is intentionally slower but
honest for the small waist witness. It is also an explicit scaling debt:
append-on-accumulation is `O(n^2)` and must be revisited before large domain
grammars lean on `rep`/`sep`.

## Alternatives

| Alternative | Disposition | Why |
| --- | --- | --- |
| Increase `FK_AST_NODE_CAP` | Rejected | That grows the C seed and hides a layer boundary signal. |
| Keep old `bmf-grammar.fk` and add a manifest-only band | Rejected | The engine still could not load; manifest-only would overclaim. |
| Treat `json/cache/line-grammar` as the cause | Rejected | `bmf-grammar.fk` alone failed with the same AST table error. |
| Keep expression sugar in layer 3 | Rejected for now | It made the grammar layer too large to witness under direct `--src`. |
| Preserve universal/source-attributed equality in this layer | Deferred for this layer | It belongs with ontology/source-attribution integration, not the grammar waist. Later source-runner repairs made the combined prelude fit direct source, but this layer still closes as grammar semantics only. |
| Split only by file while loading all pieces together | Rejected as insufficient | Total parsed source shape still matters under `--src`. |

## Witness

Layer witness:

```sh
cat form/form-stdlib/core.fk form/form-stdlib/bmf-core.fk \
    form/form-stdlib/bmf-grammar.fk \
    form/form-stdlib/tests/bmf-grammar-band.fk > /tmp/bmf-grammar.fk
./fkwu --src /tmp/bmf-grammar.fk
```

```text
2047
```

Bit decoding:

```text
1     manifest declares grammar-value
2     manifest declares ref-recursion
4     manifest declares rep-sep
8     manifest declares no-line-grammar invariant
16    grammar start/rule lookup work
32    recursive arithmetic consumes "1 + 2 * 3"
64    nested recursive arithmetic consumes "( 1 + 2 ) * 3"
128   sep + splice* emits three children
256   rep has a zero-width guard
512   manual splice* emits two children
1024  parse failure returns the PARSE-FAIL sentinel
```

The arithmetic bits are witnessed through ordinary `ref` recursion and
`seq`/`alt` structure, not through the deferred `infix` helper.
Bit `8` proves only this layer's manifest invariant and slim-prelude
construction. It does not prove downstream domain grammars are free of stale
`line-grammar` preludes.

Waist-only load:

```sh
cat form/form-stdlib/core.fk form/form-stdlib/bmf-core.fk \
    form/form-stdlib/bmf-grammar.fk > /tmp/bmf-grammar-waist-only.fk
./fkwu --src /tmp/bmf-grammar-waist-only.fk
```

```text
0
```

Copy integrity:

```sh
cmp -s grammars/bmf-grammar.fk form/form-stdlib/bmf-grammar.fk; echo $?
```

```text
0
```

Still red, explicitly:

```sh
cat form/form-stdlib/core.fk form/form-stdlib/form-ontology-loader.fk \
    form/form-stdlib/bmf-core.fk form/form-stdlib/bmf-grammar.fk \
    > /tmp/bmf-grammar-ontology-waist.fk
./fkwu --src /tmp/bmf-grammar-ontology-waist.fk
```

```text
fk_smknode: program too large for the AST node table
```

## Deferred

- Expression grammar sugar: `str`, `char`, `num`, `infix`, `chain`,
  `ternary`.
- Integer and boolean template helpers.
- Source-attributed `intern_node_at` emission.
- Stable universal NodeID equality under the ontology loader.
- `grammar-loader.fk` and all domain grammars that still list fat preludes with
  `line-grammar`, `form-ontology-loader`, or expression helpers.
- Current documentation and band comments outside this layer still describe the
  old monolith and should be read as historical until reviewed in their layer.
- A lower shared byte/codepoint stratum remains deferred from the cursor layer.

The ontology composition is not an optional footnote for the next step:
`grammar-loader` was originally kept separate from the ontology/source-
attribution frontier because `core + form-ontology-loader + bmf-core +
bmf-grammar` exceeded the direct-source AST table. Later source-runner repairs
made that combined prelude fit direct source:

```text
core + form-ontology-loader + bmf-core + bmf-grammar -> 0
```

That follow-up removes the current AST blocker, but does not move
source-attributed `intern_node_at` and stable ontology-backed node identity into
the grammar waist. Those still belong in the ontology/source-attribution layer.

## Post-Review

Grok post-reviewed the implemented layer read-only and reproduced the important
witnesses: freshness `15`, band `2047`, waist-only load `0`, copy integrity
`0`, 167-line grammar copies, and the then-red ontology composition with
`fk_smknode: program too large for the AST node table`. That ontology
composition is now green after later source-runner repairs. Grok found no
blocker inside the scoped waist. Grok required that the receipt not overclaim
bit `8` as downstream proof, and that removed expression sugar be treated as a
real API gap for upper layers.

Claude post-reviewed from the supplied summary without tools and agreed layer 3
can close green. Claude required one stronger correction: the ontology +
grammar AST overflow must be recorded as a precondition on `grammar-loader`,
not as a generic deferred item. Claude also asked that the `append` accumulator
fix be named as an `O(n^2)` scaling debt and that the arithmetic witness be
described as `ref` recursion, not deferred infix machinery.

Those corrections are now recorded here. Layer 3 is closed as the recursive BMF
grammar waist, not as the full grammar-loader/domain-grammar/ontology-emission
stack.

## 2026-07-04 Neighbor-Gate Correction

While verifying the adjacent BMF file-window correction, the current
`bmf-grammar-band` failed as a neighbor gate. This was not ignored and was not an
OOM/killed event.

Observed failure sequence:

- first run: Go/Rust reported an unclosed top-level `do`, TypeScript reported
  `defn: expected )`, and the fourth arm returned `5`;
- after balancing the top-level shape, Go/Rust returned a function closure
  because the band's final self-call was still inside the function body;
- after moving the close so the top-level call actually executed, the fourth arm
  returned `2047`, but the source siblings crashed on the fragile nested `add`
  score ladder.

The repair stayed inside `form/form-stdlib/tests/bmf-grammar-band.fk`:

- close the `bmf-grammar-language-band` definition before the top-level call;
- replace the nested `add` accumulator with the standard `sum (list ...)` band
  pattern.

Corrected gate:

```sh
cd form && ./validate.sh form-stdlib/tests/bmf-grammar-band.fk
# -> 2047
```

No grammar-engine files changed for this correction, and
`form/form-stdlib/bmf-grammar.fk` and `grammars/bmf-grammar.fk` remained
byte-identical.

Corrective post-review:

- Grok/Jason returned `PASS`, with no required changes. Grok confirmed the repair
  stays inside the band and restores a parseable/executing top-level shape
  without changing grammar engine files.
- Claude/Popper returned `PASS`, with no required changes. Claude confirmed the
  close-before-top-level-call and `sum (list ...)` changes are an appropriately
  scoped neighbor-gate repair.
