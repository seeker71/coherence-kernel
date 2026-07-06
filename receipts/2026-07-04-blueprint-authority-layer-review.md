# 2026-07-04 -- Blueprint authority layer review

## Layer

Layer 1c: `blueprint-authority.fk`.

This layer answers the "Go table? why?" pressure by making the authority split
explicit in Form policy:

- reviewed bootstrap `bp` rows are current direct-source runtime authority only
  for the names admitted by `form-ontology-loader.fk`
- `blueprint-registry.json` is the current authoring/generator source
- generated Go/Rust/TS bp tables are proof-sibling projections, not runtime
  authority
- program-image `.fkb` is the target home for executable symbol/dependency truth,
  not a claim that a live `.fkb` loader is installed today
- `.sym` is a locale/domain presentation lens over stable symbols
- unknown names must refuse/investigate without a fake NodeID
- the C seed must not grow to hide the registry migration

This is not a loader. It does not parse JSON, import generated host tables, call
`bp`, load `.fkb`, parse `.sym`, or touch `runtime/fkwu-uni.c`.

## Pre-Review

Two local reviewer agents were used because literal Grok/Claude endpoints were
not exposed in this Codex thread.

Godel, acting as the Claude-lineage reviewer, returned a conditional pass. It
accepted the pure policy layer as the right shape and required:

- do not repeat the registry-doc overclaim that `form-ontology-loader.fk`
  runtime-reads the full registry
- do not claim generated Go/Rust/TS tables are runtime authority
- do not hide the fourth-arm residual bp-table parity wall
- use synthetic rows for unknown-name behavior because direct-source Form and
  sibling kernels do not currently expose the same unknown-`bp` behavior

Plato, acting as the Grok-style adversarial reviewer, returned a conditional go
with the same requirements and made the registry documentation correction a
required part of the slice.

## Implementation

Added:

- `form/form-stdlib/blueprint-authority.fk`
- `form/form-stdlib/tests/blueprint-authority-band.fk`

Updated:

- `form/user-blueprint-registry.md`
- `form/form-stdlib/blueprint-registry.json`
- `receipts/2026-07-03-core-layer-architecture-map.md`

The policy layer defines source rows for:

- `form-bootstrap`
- `registry-json`
- `generated-sibling-table`
- `program-image-fkb`
- `sym-lens`
- `c-seed-growth`

It then exposes predicates for runtime authority, authoring source,
projection-only source, presentation-lens-only source, executable dependency
authority, generated-table admission, `.sym` lens admission, C-seed-growth
refusal, and unknown-name refusal/investigation.

The registry doc now states the current honest split: the loader does not
runtime-parse the full JSON registry, generated bp tables are proof-sibling
projections, `.fkb` carries target executable symbol/dependency truth, and
`.sym` remains presentation.

Initial post-review found one remaining contradictory sentence in
`form/user-blueprint-registry.md`, which still said the registry file was
something `(bp ...)` could read at load time. That sentence is now removed. The
same follow-up clarified the policy row for program-image `.fkb`: it is target
executable dependency authority, not current direct-source runtime authority.

## Verification

Checkout floor:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
./fkwu --src /tmp/nvr.fk -> 11111
```

Layer band:

```text
cd form
./validate.sh form-stdlib/tests/blueprint-authority-band.fk
```

```text
core.fk+core.fk+blueprint-authority.fk+blueprint-authority-band.fk -> 65535
1 ok, 0 divergent
```

The edited JSON registry note also parsed successfully:

```text
json ok
```

No OOM, killed process, stall, or kernel divergence occurred.

## Achieved

- The Go/Rust/TS bp-table role is now executable policy: generated sibling
  projection only, not runtime authority.
- The registry documentation no longer claims the direct-source Form loader
  runtime-reads the full JSON registry.
- The architecture map now has Layer 1c for Blueprint authority.
- Unknown names are modeled as investigate/refuse with no fake NodeID, avoiding
  the historic `(1 2 0 0)` collapse.
- `.fkb` and `.sym` have a clearer boundary: executable symbol/dependency truth
  vs locale/domain presentation.
- Program-image `.fkb` target authority is separated from current loader
  authority, so the policy does not imply the runtime selector is installed.

## Deferred

- Full `blueprint-registry.json` as Form-owned runtime data: deferred because
  this layer is only policy and does not implement a registry parser/image.
- Loud Form-native diagnostic path for unknown `bp`: deferred because current
  direct-source Form returns absence for misses while sibling kernels throw.
- Full generated bp-table availability in the fourth-arm flattener: deferred
  because `fourth-arm-bands.txt` still names hand-coded bp-table parity as a
  wall.
- Real program-image `.fkb` writer/loader/selector: deferred to the existing
  program-image and runtime artifact layers.
- Formal `.sym` sidecar grammar and locale packs: deferred because this layer
  only states the lens boundary.
- C-seed growth: rejected, not deferred as implementation strategy.

## Post-Review

Initial post-review failed correctly. Tesla and Aquinas both found that
`form/user-blueprint-registry.md` still contained one stale sentence saying the
registry file was something `(bp ...)` could read at load time. Aquinas also
flagged a possible overread in the policy: program-image `.fkb` target
authority could look like current loader authority.

Corrections applied:

- removed the stale load-time registry sentence from
  `form/user-blueprint-registry.md`
- added `program-image-fkb-target-not-current-loader` to the manifest
- changed the program-image `.fkb` row to `current-runtime = 0` and
  `executable-deps = 1`
- updated the band to assert this target/current split and to check that the
  stale load-time sentence is absent

Recheck:

- McClintock, acting as the Claude-lineage reviewer: `PASS`
- Bernoulli, acting as the Grok-style adversarial reviewer: `PASS`

Both rechecks verified that the doc overclaim is gone, the `.fkb` authority
overread is corrected, the band returns `65535`, and the remaining risks are
honest deferred work rather than blockers.
