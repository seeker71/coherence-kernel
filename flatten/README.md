# flatten/ — the op manifest, and what flatten is not

`form-flatten.fk` owns `flt-ops` — the hand-maintained single source of truth for
the native op rows. `gen-source-walker-table.fk` and `gen-source-walker.fk`
generate `runtime/fkwu-optable.h` from it (195 rows on 2026-09-04). Adding a value
op is a `flt-ops` row, then a regen, then its serialize arm in
`form/form-stdlib/fkc-table-serialize.fk` — never a C edit. `host-effect-grammar.fk`
names the host-effect families the same rows carry.

Flatten is not a run lane. The body runs source (`fkwu file.fk`), lowers BML in
memory (`fkwu file.bml`), and caches program images (`.fkb`) beside their sources;
`.tbl` execution does not exist (`fkwu x.tbl` refuses by name; no `.tbl` is in the
tree). `fourth-flatten-table.txt` is a regenerable cache left from the flatten era,
not a foundation — a regen of the emitted artifacts still passes through this body,
and that regen owes a source-native path (`MANIFEST.md`, `release-ledger.bml`).

The op families the seed carries with no caller outside `flt-ops` are enumerated
in `docs/penumbra-map.md`: each row wants a caller or a release.
