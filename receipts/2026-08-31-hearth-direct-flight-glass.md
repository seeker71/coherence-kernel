# Hearth direct-flight glass — 2026-08-31

## Observation

The exact local task at turn 9012 showed the remaining opaque span plainly:
the durable framebuffer emitted `direct-answer/observe begin`, then no phase
change for 33,938 ms, then `observe value`; generation followed. The existing
begin stamp was already a live fact. The glass simply did not project it while
the work was in flight.

## Movement

`form/form-stdlib/hearth-glass.bml` now owns the projection:

- finds the newest `direct-answer` framebuffer stage;
- treats `status=begin` as a present flight and derives its age from
  `stamp-ms` and the current clock;
- reports `none`, not numeric zero, once a value/failure end stage arrives.

`form/form-stdlib/lane-motion.bml` publishes `direct` and `directage` through
the existing snapshot. `observe/hearth-glass-live.fk` only renders the new
data lane. The model process, task bytes, KV state, and egress protocol stay
untouched.

## Evidence

```
./fkwu form/form-stdlib/hearth-glass.bml -> 0
./fkwu form/form-stdlib/lane-motion.bml -> 0
./fkwu form/form-stdlib/tests/hearth-glass-band.fk -> 8191
./fkwu form/form-stdlib/tests/lane-motion-band.fk -> 63
./fkwu observe/preflight-run.fk (hearth-glass-live) -> errors 0, unresolved 0
```

The `8191` band contains active observe phase, 2.5-second derived flight age,
latest-stage precedence, and the settled-to-`none` transition. It also retains
the older collector, percentile, waiting-set, phase, and ledger witnesses.

## Boundary

The user-visible glass currently running from the sibling checkout keeps its
already-loaded source until that checkout is consciously synchronized. The
active Form-cli resident is independent and continues to serve from its
current born image. This change needs no model restart: the next glass birth
alone reads the new BML projection.

The next live question is performance rather than visibility: direct
observation has a measured duration now, so repeated warm tasks can show
whether its carrier work shrinks or whether the prefill carrier needs a native
incremental event seam.
