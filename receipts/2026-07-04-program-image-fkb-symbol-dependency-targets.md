# 2026-07-04 -- program-image fkb symbol dependency targets

## Trigger

User correction:

> the .fkb should have not just with node has with sym, it also needs to have dependencies of sym in nodes

The previous 8h shape carried `node-id`, `defined-symbol-id`, and a list of
dependency symbol ids. That was not enough. It said "this node depends on
symbol N" but left the concrete target node implicit. A future loader would
have to rediscover target nodes by scanning definitions, which makes executable
dependency truth partly outside the `.fkb`.

## Review

Hilbert/Grok verdict: `PASS_WITH_CHANGES`.

- Correct 8h/8h4/8h6 in place.
- Use dependency target rows rather than a bare symbol-id list.
- Bump the byte grammar version.
- Prove target-node out of range, wrong target definition, old row shape, and
  forged target drift cases.

Parfit/Claude verdict: `PASS_WITH_CHANGES`.

- Use paired dependency target rows, not parallel lists.
- Require local dependency targets to resolve to node rows whose
  `defined-symbol-id` equals the dependency symbol id.
- Require non-anonymous defined symbols to be unique.
- Keep `.sym` as a locale/domain lens only.
- Bump `pifbc-byte-version`; keeping v1 would make incompatible byte meanings
  share one wire version.

## Implemented Shape

Layer 8h now has:

```text
(pif-symbol-dependency-target symbol-id target-node-id)

(pif-node-symbol-row node-id defined-symbol-id dependency-targets)
```

`dependency-targets` is a canonical ordered list of paired rows. Validation
requires:

- symbol ids reference embedded `.fkb` symbols;
- target node ids reference table nodes;
- dependency target rows are ordered and duplicate-free;
- target node rows exist in the node-symbol image;
- target node rows define the referenced symbol;
- defined symbols are unique across node-symbol rows, except `-1` anonymous
  rows;
- old symbol-id-only node-symbol rows are invalid.

The program-image envelope version is now `program-image-fkb-v2`.

## Byte Grammar

Layer 8h4 is now `program-image-fkb-byte-container-v2` with
`pifbc-byte-version = 2`.

Node-symbol rows encode as:

```text
node-id defined-symbol-id dep-count (dep-symbol-id target-node-id)*
```

The symbol dependency target rows are hash-covered by the canonical `.fkb`
payload bytes. Layer 8h6 is now `program-image-fkb-byte-decode-v2` and decodes
the same dependency target rows. Old v1 bytes refuse with `bad-version`.

## Carrier Lift

The recipe carrier was also lifted because it still treated a program-image
envelope as table-only. It now preserves the symbol image under registered
NodeID categories:

- `PROGRAM-IMAGE-RECIPE-SYMBOL-IMAGE`
- `PROGRAM-IMAGE-RECIPE-SYMBOL-ROWS`
- `PROGRAM-IMAGE-RECIPE-SYMBOL-ROW`
- `PROGRAM-IMAGE-RECIPE-NODE-SYMBOL-ROWS`
- `PROGRAM-IMAGE-RECIPE-NODE-SYMBOL-ROW`
- `PROGRAM-IMAGE-RECIPE-SYMBOL-DEPENDENCY-TARGETS`
- `PROGRAM-IMAGE-RECIPE-SYMBOL-DEPENDENCY-TARGET`

The registry, direct-source ontology loader, and proof-sibling bp tables were
updated so all validation kernels resolve those names. This slice includes the
PIRC symbol-image carrier lift and the typed-carrier sync needed by the
dependent bands, so the generated lookup rows include both program-image recipe
carrier names and the adjacent typed program-image/typed literal names already
used by those bands.

Why proof-sibling bp tables were touched: these are generated blueprint-name
tables for the Go/Rust/TypeScript validation siblings, not runtime ownership.
`validate.sh` cross-checks bands through those siblings; missing names would
make the sibling kernels diverge on `(bp ...)` resolution. The TypeScript arm
failed first on the new `PROGRAM-IMAGE-RECIPE-SYMBOL-IMAGE` name, exposing the
missing offline table rows. The normal generator path
`../scripts/gen_bp_table.py` is absent in this worktree, and `validate.sh` only
runs it when present, so the generated tables were updated narrowly by hand.
This does not add Go/Rust/TS runtime meaning and does not touch the C seed.

Typed carrier still accepts the table-oriented typed input surface and
normalizes to a raw 8h envelope with an empty symbol image. Full typed-symbol
input remains a separate layer lift.

## Verification

Required startup floor was already green in this turn:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
native-vs-rented-check -> 11111
```

Focused and dependent bands:

```text
program-image-fkb-band                  -> 2147483647
program-image-fkb-byte-container-band   -> 2147483647
program-image-fkb-byte-decode-band      -> 536870911
program-image-fkb-byte-file-witness-band -> 2147483647
program-image-recipe-carrier-band       -> 2147483647
program-image-typed-carrier-band        -> 16777215
source-compiler-fkb-file-emission-band  -> 2147483647
```

Static checks:

```text
cmp form/form-stdlib/program-image-fkb.fk grammars/program-image-fkb.fk -> 0
cmp form/form-stdlib/program-image-fkb-byte-container.fk grammars/program-image-fkb-byte-container.fk -> 0
cmp form/form-stdlib/program-image-fkb-byte-decode.fk grammars/program-image-fkb-byte-decode.fk -> 0
cmp form/form-stdlib/program-image-recipe-carrier.fk grammars/program-image-recipe-carrier.fk -> 0

old v1/parallel-list scan over 8h/8h4/8h6 mirrors -> no hits
forbidden bridge/runtime scan over 8h/8h4/8h6/PIRC mirrors -> no hits
```

Post-review follow-up from Hilbert/Grok and Parfit/Claude tightened this slice:

- stale byte-format prose now names byte version `2` and dependency target pairs;
- 8h now proves duplicate dependency-target rows, out-of-order target rows, and
  in-range target nodes with no node-symbol row are refused;
- 8h6 now mutates a target-node byte in the v2 payload and proves decode refuses
  the reconstructed image as `invalid-symbol-image`;
- PIRC now witnesses a non-empty symbol image and asserts the dependency target
  recipe node carries both dependency symbol id and concrete target node id.

## Investigations

No OOM or `Killed` process occurred.

Three source-shape/score issues were investigated and fixed:

- `program-image-fkb-band` initially had an unclosed strengthened assertion.
  Parenthesis depth located the exact block; the rerun then exposed a
  `pifb-score` arity shape error in the same block, which was fixed.
- `program-image-recipe-carrier-band` initially had missing closes after the
  new blueprint and envelope assertions. Parenthesis depth located both blocks.
- `program-image-typed-carrier-band` returned a green sibling check with
  `16777199`, missing bit `16`. That was not accepted as success; the missing
  bit was the recipe-envelope child count after the symbol-image carrier lift,
  and the band was corrected back to `16777215`.

## Deferred

- Full typed-symbol input for `program-image-typed-carrier.fk`.
- Runtime load/walk/call from `.fkb`.
- Cache freshness admission based on `.fk` vs `.fkb`.
- Source compiler persistence handoff from ready 8j1 `.fkb` witness through
  the 8h6 admission row.
- `.sym` locale/domain grammar and rendering.
- Cross-module symbol resolution policy.
- Any C-seed growth.

The broader worktree currently contains tracked runtime/C-seed edits outside
this `.fkb` symbol-target correction. They were not made or reverted by this
slice, and this receipt does not bless them. Before merging the whole worktree,
that separate C-seed layer must be reviewed against the shrink-to-zero rule or
split out.

## Boundary

This change makes `.fkb` own the executable symbol dependency graph at node
resolution level. `.sym` can still localize, alias, or present symbols, but it
cannot be the only place where a loader learns which node realizes a dependency.
