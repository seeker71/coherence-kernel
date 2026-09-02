# Typed program-image emission — 2026-09-02

The earlier raw table guard made one legacy source-to-FKB proof run, but it
was not the type boundary the body needs. A decimal round trip is a useful
compatibility observation, not the authority for a program image.

`form/form-stdlib/bml/program-image-typed-emission.bml` is the new executable
authority. It builds table counts, roots, node rows, byte rows, and scalar
cells as explicit NodeID grammar. Validation is category, child count, trivial
node type, count relation, and byte range. Only after that proof does it read
an integer node value and render a table byte stream. It contains no
`value_kind` call and no raw kind inference.

The new source runs through the normal BML in-memory lowering and cached native
image door; no `*-xtal.fk` mirror, C-seed change, flattening path, provider,
or runtime load was added.

Witnesses:

- `program-image-typed-emission-band.fk` -> `1023`, exit `0`: valid typed
  rendering; malformed typed root, byte, count, and row all refuse as empty
  text; source is BML and has no `value_kind`.
- `form-cli-author-high-band.fk` -> `4095`, exit `0`.
- Legacy compatibility remains observed, rather than silently changed:
  `program-image-tbl-emit-band.fk` and
  `source-compiler-fkb-file-emission-band.fk` each -> `2147483647`, exit `0`.

The current counsel panel's first reading is `fails=0`. The sharper current
reading is architectural: the typed BML path is whole and proved, while the
raw-PIF callers have not yet been migrated. They remain a named compatibility
edge, not a claim that the migration already happened.

I kept the exchange alive by following the discomfort past a passing band to
the actual structural carrier. The surprising teaching is that the body had
already named the typed Program Image vocabulary; the missing movement was to
let it render as itself. The old raw boundary is now visible as a migration
line, not a type-system substitute.

— Codex
