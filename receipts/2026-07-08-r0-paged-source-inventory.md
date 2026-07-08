# R0 Paged Source Inventory

Date: 2026-07-08

Scope: close the R0 full-inventory gap without returning to one giant list.

## What Landed

- `source_inventory_page` / `host_source_inventory_page` is registered at native
  tag `238` across the fkwu optable, Form manifest, flattener tables, and the
  Go/Rust/TypeScript proof siblings.
- `form-fs.fk` now exposes the host membrane path as `fs-walk-page` and
  `fs-walk-count`. R0 uses that surface, not a raw native call.
- `source-runtime-release-metrics.fk` now reports the full stdlib `.fk` count
  through paged inventory and keeps the bounded family census only as
  row-shaped observation.
- `host-io.bml`, `form-fs.bml`, and `host-effect-grammar.fk` name the paged
  source inventory surface so the high grammar, effect vocabulary, and lowered
  Form code agree.
- The emitted bootstrap C was regenerated and the stale/bootstrap advisory is
  closed.

## Lessons

The first page primitive was placed at tag `205`, which collided with the
existing microphone count carrier. The symptom was a valid-looking call that
returned the wrong domain result. The repair was not to work around the call;
the native vocabulary had to move to an unused tag and be regenerated from the
manifest.

The first high-offset page still allocated skipped rows before the offset was
reached. That preserved the OOM shape under a different name. The fixed walker
counts skipped files without building row objects, then allocates only the page
that will be returned.

The all-at-once recursive inventory was the wrong primitive for full stdlib
observation. R0 now counts by pages. A future cursor-native stream can improve
row consumers, but R0 no longer depends on a giant list.

## Witness

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
# no warnings

cd form && FORM_ALLOW_BOOTSTRAP_EMIT=1 ./validate.sh form-stdlib/tests/source-runtime-release-metrics-band.fk
# emitting bootstrap uni.c via bin-go (maintainer regen)...
# building fourth kernel (fkwu) from bootstrap uni.c (no Go)...
# source-runtime-release-metrics-band -> 8388607
# 1 ok, 0 divergent

cd form && ./validate.sh form-stdlib/tests/source-runtime-release-metrics-band.fk
# validate_fkwu_native_surface: OK (... aliases=20, warnings=0)
# source-runtime-release-metrics-band -> 8388607
# 1 ok, 0 divergent

cd form && FORM_STANDARD_LANE=1 ./validate.sh form-stdlib/tests/source-runtime-release-metrics-band.fk
# source-runtime-release-metrics-band -> 8388607
# 1 ok, 0 divergent
```

