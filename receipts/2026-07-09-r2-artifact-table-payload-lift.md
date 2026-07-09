# 2026-07-09 -- R2 artifact table-payload lift

## What moved

The artifact lifecycle layer no longer presents `.tbl` as a peer runtime
artifact in the active grammar. The BMF rule now says:

```text
artifact lifecycle ... source ... fkb ... sym ... table-payload ... dylib ... freshness ...
```

The Form use layer, cache row, descriptor row, and source compiler emission row
were lifted to the same vocabulary:

- `grammars/stdlib-uplift-missing-grammars.bmf`
- `form/form-stdlib/stdlib-uplift-bmf-use.fk`
- `form/form-stdlib/source-artifact-cache.fk`
- `form/form-stdlib/source-artifact-descriptor.fk`
- `form/form-stdlib/source-compiler-emission.fk`
- `grammars/source-compiler-emission.fk`

The semantic object is now an embedded `.fkb` table payload. Standalone `.tbl`
remains retired as a runtime input.

## Compatibility left on purpose

The old `includes-tbl` and `table-text` names remain as small aliases only where
older rows or tests may still call them. They delegate to:

- `sac-output-embeds-table-payload`
- `sad-artifact-embeds-table-payload`
- `sce-compile-output-embeds-table-payload?`
- `sce-table-payload-match?`
- `sce-emission-table-payload`

The lower `program-image-tbl-emit` module is not renamed in this slice. Its
helper still renders the textual table payload used as an observational witness.
That is the next focused layer, not hidden debt.

## Witness

Focused bands after the lift:

```text
cd form && ./validate.sh form-stdlib/tests/source-artifact-cache-band.fk
cd form && ./validate.sh form-stdlib/tests/source-artifact-descriptor-band.fk
cd form && ./validate.sh form-stdlib/tests/source-compiler-emission-band.fk
cd form && ./validate.sh form-stdlib/tests/stdlib-uplift-bmf-use-band.fk
```

Expected values remain:

```text
source-artifact-cache-band          -> 2097151
source-artifact-descriptor-band     -> 2147483647
source-compiler-emission-band       -> 2147483647
stdlib-uplift-bmf-use-band          -> 131071
```

No C seed growth is part of this change.
