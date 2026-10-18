# flatten/ — the op manifest

`form-flatten.fk` contains the BML `flt-ops` table: native operation names,
arities and bootstrap tags. Run `./fkwu flatten/gen-source-walker-table.fk`
to import that table and the native rewrite rules and emit
`runtime/fkwu-optable.h` directly.

Adding a value operation updates its manifest row, this table and the matching
serializer arm in `form/form-stdlib/fkc-table-serialize.fk`, then regenerates
the header. The `op-manifest` drift check reads the compiler-normalized source.
Executable flattening lives in `form/form-stdlib/form-flatten.fk`; host-effect
meaning lives in `form/form-stdlib/host-effect-grammar.fk`.

An op family the seed carries with no caller outside `flt-ops` wants a caller or a release.
