# 2026-07-09 -- R2 artifact table-payload lift

## What moved

The artifact lifecycle layer no longer presents `.tbl` as a peer runtime
artifact in the active grammar. The BMF rule now says:

```text
artifact lifecycle ... source ... fkb ... sym ... table-payload ... dylib ... freshness ...
```

The Form use layer, cache row, descriptor row, source compiler emission row, and
payload witness chain use the same vocabulary:

- `grammars/stdlib-uplift-missing-grammars.bmf`
- `form/form-stdlib/stdlib-uplift-bmf-use.fk`
- `form/form-stdlib/source-artifact-cache.fk`
- `form/form-stdlib/source-artifact-descriptor.fk`
- `form/form-stdlib/source-compiler-emission.fk`
- `grammars/source-compiler-emission.fk`
- `form/form-stdlib/program-image-table-payload-emit.fk`
- `form/form-stdlib/program-image-table-payload-witness.fk`
- `form/form-stdlib/runtime-table-payload-attempt.fk`
- `grammars/program-image-table-payload-emit.fk`
- `grammars/program-image-table-payload-witness.fk`
- `grammars/runtime-table-payload-attempt.fk`

The semantic object is now an embedded `.fkb` table payload. Standalone `.tbl`
remains retired as a runtime input.

## Released names

The active source surface has no compatibility aliases for retired artifact
names. The table-payload chain is named directly:

- `program-image-table-payload-emit`
- `program-image-table-payload-witness`
- `runtime-table-payload-attempt`
- `sac-output-embeds-table-payload`
- `sad-artifact-embeds-table-payload`
- `sce-compile-output-embeds-table-payload?`
- `sce-table-payload-match?`
- `sce-emission-table-payload`

The tracked retired proof table artifact was removed. Active proof documentation
now points at the current `.fk/.fkb` runtime surface instead of a table file.

## Witness

Focused bands after the release:

```text
cd form && ./validate.sh form-stdlib/tests/source-artifact-cache-band.fk
cd form && ./validate.sh form-stdlib/tests/source-artifact-descriptor-band.fk
cd form && ./validate.sh form-stdlib/tests/source-compiler-emission-band.fk
cd form && ./validate.sh form-stdlib/tests/stdlib-uplift-bmf-use-band.fk
cd form && ./validate.sh form-stdlib/tests/program-image-table-payload-emit-band.fk
cd form && ./validate.sh form-stdlib/tests/program-image-table-payload-witness-band.fk
cd form && ./validate.sh form-stdlib/tests/runtime-table-payload-attempt-band.fk
cd form && ./validate.sh form-stdlib/tests/reason-coverage-band.fk
cd form && ./validate.sh form-stdlib/tests/source-artifact-file-probe-band.fk
cd form && ./validate.sh form-stdlib/tests/source-compiler-fkb-file-emission-band.fk
cd form && ./validate.sh form-stdlib/tests/source-compiler-file-persistence-band.fk
cd form && ./validate.sh form-stdlib/tests/source-compiler-persistence-band.fk
cd form && ./validate.sh form-stdlib/tests/runtime-artifact-handoff-band.fk
cd form && ./validate.sh form-stdlib/tests/source-runtime-release-metrics-band.fk
```

Observed values:

```text
source-artifact-cache-band          -> 2097151
source-artifact-descriptor-band     -> 2147483647
source-compiler-emission-band       -> 2147483647
stdlib-uplift-bmf-use-band          -> 131071
program-image-table-payload-emit    -> 2147483647
program-image-table-payload-witness -> 2147483647
runtime-table-payload-attempt       -> 2147483647
reason-coverage-band                -> 2147483647
source-artifact-file-probe-band     -> 2147483647
source-compiler-fkb-file-emission   -> 2147483647
source-compiler-file-persistence    -> 2147483647
source-compiler-persistence         -> 2147483647
runtime-artifact-handoff            -> 2147483647
source-runtime-release-metrics      -> 8388607
```

Validation also exposed and closed two test-health gaps on the touched path:

- `source-artifact-file-probe-band.fk` had malformed assertion nesting and
  direct static-read paths that returned `Null` outside the Go walker.
- `reason-coverage-band.fk` had a multi-line prelude shape the validator did not
  load and the same null static-read pattern.

The stale bootstrap advisory was handled by regenerating the committed bootstrap
stamp through `FORM_ALLOW_BOOTSTRAP_EMIT=1 ./validate.sh ...`, then rerunning a
plain validation without the advisory.
