# flatten/ — the op manifest

`form-flatten.fk` owns `flt-ops` — the hand-maintained single source of truth for
the native op rows. `gen-source-walker-table.fk` and `gen-source-walker.fk`
generate `runtime/fkwu-optable.h` from it. Adding a value op is a `flt-ops` row,
then a regen, then its serialize arm in `form/form-stdlib/fkc-table-serialize.fk`.
`host-effect-root-projection.fk` projects the host-effect grammar
(`form/form-stdlib/host-effect-grammar.fk`, from `grammars/host-effect-vocabulary.bmf`) onto the same rows. The drift gate `op-manifest` (`gate/op-manifest.bml`) reads the rows.

An op family the seed carries with no caller outside `flt-ops` wants a caller or a release.
