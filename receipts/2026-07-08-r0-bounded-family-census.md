# R0 Bounded Family Census

Date: 2026-07-08

Scope: R0 measurement for the source runtime release path.

## What Landed

- `source_inventory` in the checkout witness now initializes the Form heap before
  constructing inventory rows. Without that, direct `source_inventory` calls
  could hit `fk_list_push` heap exhaustion before any healthy bounded census was
  possible.
- `form/form-stdlib/source-runtime-release-metrics.fk` no longer asks for one
  recursive all-stdlib list. It measures root `.fk` files with known stdlib
  family directories skipped, then measures each family separately.
- `form/form-stdlib/tests/source-runtime-release-metrics-band.fk` now witnesses
  the R0 shape with score `8388607`.
- The current bounded family census reports `2,297` stdlib `.fk` files.
- `grammars/source-runtime-release-metrics.bmf` now carries both aggregate
  `metric` rows and `family` census rows, and both are exercised through
  bidirectional runtime BMF rules.

## What Remains Open

The all-at-once recursive `source_inventory "form/form-stdlib"` traversal still
overfills the current fkwu list heap. That is not ignored and not treated as a
release blocker for R0. It is the next design instruction: full inventory should
be streaming or paged through the BMF cursor / host membrane path, not built as
one giant list.

## Witness

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
# no warnings

cd form && ./validate.sh form-stdlib/tests/source-runtime-release-metrics-band.fk
# validate_fkwu_native_surface: OK (... aliases=19, warnings=0)
# source-runtime-release-metrics-band -> 8388607
# 1 ok, 0 divergent
```

This is still a guide, not a gate. R0 now gives repeatable counts and visible
pressure; the release path continues through `.fk -> .fkb/.sym -> runtime
selector`, with `.tbl` retired and full inventory streaming still owed.
