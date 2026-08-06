# Program Image `.sym` Lens Layer Review

Date: 2026-07-04

Layer: 8h7, program-image `.sym` presentation lens.

## Question

The previous 8h layers made `.fkb` the target holder of executable table and
symbol/dependency truth. The user correction was that `.fkb` carries symbols and
their dependency target nodes, while `.sym` should be locale/domain specific.
This slice asks what `.sym` may own without becoming a second executable
authority.

## Pre-Review

Claude-lineage review: `PASS_WITH_CHANGES`.

- Attach the layer only to a validated PIF envelope or a symbol image proven
  against a valid PIF table.
- Do not silently fall back to canonical names when the lens is malformed,
  duplicate, out of range, or marked with executable dependency data.
- Add 8h7 to the architecture map and keep generated Go/Rust/TS tables out of
  the runtime authority story.
- Fix stale byte-container prose that still said byte version `1`.

Grok-style review: `PASS_WITH_CHANGES`.

- Keep 8h7 independent of byte-file/decode admission, selectors, attempts, and
  observations.
- Enforce fixed row arity and adversarial cases: `-1`, `count`, empty fields,
  duplicate keys, alias malformation, invalid PIF, executable-deps flags, extra
  fields, localized hit, canonical fallback, and static forbidden-name scans.
- Keep deferred `.sym` persistence, parser, locale fallback hierarchy, reverse
  alias policy, source-compiler emission, runtime load/walk/call, and C growth
  explicitly out of scope.

## Implemented

- Added `form/form-stdlib/program-image-sym-lens.fk`.
- Added the mirror `grammars/program-image-sym-lens.fk`.
- Added `form/form-stdlib/tests/program-image-sym-lens-band.fk`.
- Inserted `8h7. Program-image .sym lens` in the architecture map between 8h6
  and 8i.
- Corrected the 8h4 byte-container prose to byte version `2`.

The layer defines fixed tagged rows:

```text
("program-image-sym-row" locale domain symbol-id display aliases doc carries-executable-deps)
```

and fixed lens bundles:

```text
("program-image-sym-lens" "program-image-sym-lens-v1" rows)
```

A row is valid only over `pif-envelope-valid?`; `symbol-id` must be in the
embedded symbol image range; `locale`, `domain`, `display`, and `doc` must be
nonempty strings; aliases must be a nonempty list of nonempty strings; the
executable-deps flag must be exactly zero; and duplicate
`(locale domain symbol-id)` rows are rejected.

Rendering returns a status row, not a bare string. Missing localized rows may
fall back to the canonical PIF key only after the PIF envelope and lens are both
valid. Invalid lenses investigate; invalid envelopes and out-of-range render
queries refuse.

## Go Table Boundary

The "Go table" is not a core substrate. It remains only a generated
proof-sibling projection for offline Go/Rust/TS walkers. 8h7 does not consume
`bp_table`, `bpTable`, or `BP_TABLE`; the band statically checks this.

## Verification

```sh
cd form && ./validate.sh form-stdlib/tests/program-image-sym-lens-band.fk
```

Result:

```text
→ 1048575
1 ok, 0 divergent
```

## Post-Review

Claude-lineage post-review: `PASS`.

- Confirmed valid PIF envelope admission before row/lens/render use.
- Confirmed fixed arity rejects extra executable payload fields.
- Confirmed malformed/duplicate invalid lenses investigate/refuse and do not
  canonical-fallback.
- Confirmed adversarial fixtures, mirror exactness, static forbidden-name
  checks, architecture insertion, byte-container version `2`, and deferred items.

Grok-style post-review: `PASS`.

- Confirmed fallback happens only after PIF and full lens validation.
- Confirmed no forbidden file/runtime/Go-table symbols in the 8h7 source or
  grammar mirror.
- Confirmed the architecture map and receipt preserve the `.fkb` executable
  truth / `.sym` presentation lens boundary.

Both reviewers noted that the broader worktree still contains unrelated
`runtime/fkwu-uni.c`, `runtime/fkwu-optable.h`, and generated bp-table changes.
Those are outside this 8h7 slice and are not reviewed or blessed here.

## Deferred

- Actual `.sym` file grammar/parser/reader/writer.
- Locale fallback chains and domain inheritance.
- Alias reverse lookup and alias collision policy.
- Docs markup/normalization/rendering policy.
- Grammar-facing display integration.
- Cross-module symbol resolution.
- `.fkb` load/walk/call.
- Cache freshness admission.
- Selector install.
- Runtime attempts/observations.
- C-seed growth.
