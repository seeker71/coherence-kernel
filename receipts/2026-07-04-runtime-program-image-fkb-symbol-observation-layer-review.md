# Runtime Program Image `.fkb` Symbol Observation Layer Review

Date: 2026-07-04

Layer: 9h8, `form/form-stdlib/runtime-program-image-fkb-symbol-observation.fk`

## Question

9h7 capability-binds a computed symbol walk, but intentionally stops before
producing attempts or observations. This slice asks how a computed, capability-
bound trace becomes a 9c observation/outcome without smuggling in supplied row,
trace, observation, or outcome authority.

## Pre-Review

Claude/Sema review: `PASS_WITH_CHANGES`.

- Expose one authority entrypoint only:
  `rpswo-adapt envelope readiness admission symbol-request step-budget
  input-value`.
- Recompute `rpswc-bind` internally. Do not accept a supplied 9h7 join, trace,
  observation, or outcome.
- Derive a 9c observation only when the recomputed 9h7 row is
  `capability-bound-trace` and the bound trace is an `rpiwt-trace?`.
- Call `rao-run-observation` and `rao-outcome-from-selection-observation` only
  inside the recomputed-bound branch.
- Non-bound 9h7 states must expose no derived observation and no 9c outcome.
- Do not call 9f supplied-attempt helpers, 9h1/9h2/9h3/9h4 bridges, or
  `rpiwt-bridge-from-trace`.

Grok-style review: `PASS_WITH_CHANGES`.

- Public surface must use base inputs only and derive selection from
  `rale-envelope-selection envelope`.
- Mark the observation explicitly as a synthetic adapter observation from a
  computed trace, not as host execution.
- Check trace status/stop-reason coherence; hard statuses such as timeout,
  stalled, killed, and OOM must not normalize to success.
- Require the observation action to match the selected action before deriving
  the 9c outcome.
- Non-bound 9h7 results must not call `rao-outcome-from-selection` as fallback.
- Add static forbids for supplied attempts, 9h1-9h4 bridges, table text,
  presentation `.sym`, generated proof-sibling tables, artifact IO, selector
  mutation, and C-seed growth.

## Achieved

- Added `runtime-program-image-fkb-symbol-observation.fk` and byte-identical
  mirror `grammars/runtime-program-image-fkb-symbol-observation.fk`.
- Added one row-producing authority entrypoint, `rpswo-adapt`, which
  recomputes `rpswc-bind` from the base 9h7 inputs.
- The adapter emits `no-derived-observation` and `no-derived-outcome` for every
  non-bound 9h7 result.
- Bound traces are checked for trace shape, resource validity, and status /
  stop-reason coherence before any observation is derived.
- Observation detail is prefixed with
  `synthetic-computed-trace-observation:` so the 9c row is never confused with
  host execution.
- The 9c outcome is derived only after the envelope selection is readable,
  selected, and action-matched.
- Added `runtime-program-image-fkb-symbol-observation-band.fk`, covering happy
  completion, synthetic detail, timeout/hard investigation, error non-success,
  symbol refusal, unavailable readiness, malformed selection, action mismatch,
  selector-investigate, static authority forbids, mirror identity, architecture
  map, and no supplied observation or outcome authority.

## Deferred

- Real artifact load/walk/call remains deferred; 9h8 adapts computed in-memory
  9h7 traces only.
- 9f supplied-attempt production remains deferred and intentionally unused.
- OOM, killed, and stalled traces are preserved by the same hard-status path as
  timeout, but this band only has a current computed timeout fixture; broader
  hard-status production awaits richer computed walker coverage.
- Selector install, storage mutation, champion/corpus updates, and learning
  ingestion remain downstream work.
- Disk `.fkb` freshness selection and `.dylib` execution remain outside this
  layer.

## Failure / Stall Notes

- No OOM-killed process occurred during this layer.
- The earlier `head`-style stall concern is treated as a workflow constraint:
  this receipt was built from targeted `rg`/`sed` reads, not broad truncating
  scans.
- The layer deliberately keeps 9h8 from papering over non-bound 9h7 states with
  a generic `pending-observation` outcome.
- C-seed guide note: `runtime/fkwu-uni.c` and `runtime/fkwu-optable.h` are dirty
  work in this repo (`222 insertions / 18 deletions` across those files at
  review time). 9h8 did not edit or depend on those files, but they are still
  part of the shared work ledger. Their current purpose and shrink debt are
  recorded in
  `receipts/2026-07-04-source-runner-cseed-guide-review.md`.

## Validation

Bootstrap before edits:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
./fkwu --src /tmp/nvr.fk -> 11111
```

Focused validation:

```text
./validate.sh form-stdlib/core.fk ... form-stdlib/runtime-program-image-fkb-symbol-observation.fk form-stdlib/tests/runtime-program-image-fkb-symbol-observation-band.fk
-> 262143, 1 ok, 0 divergent
final focused rerun after receipt/post-review updates -> 262143, 1 ok, 0 divergent
```

Neighbor validation:

```text
runtime-artifact-outcome-band -> 2147483647, 1 ok, 0 divergent
runtime-program-image-fkb-symbol-capability-bound-band -> 262143, 1 ok, 0 divergent
runtime-artifact-handoff-band -> 2147483647, 1 ok, 0 divergent
```

## Post-Review

Claude/Sema post-review: `PASS_WITH_CHANGES`.

- No 9h8 source/test blocker found.
- `rpswo-adapt` is the only function constructing the tagged
  `runtime-program-image-fkb-symbol-observation` row.
- It recomputes `rpswc-bind` internally from base inputs only.
- The synthetic observation language is honest: detail is prefixed with
  `synthetic-computed-trace-observation:` and outcome derivation is guarded by
  envelope selection/action match.
- Required receipt updates: replace pending post-review and guide the C-seed
  claim against the tracked dirty C files.

Grok-style post-review: `BLOCK` before this C-seed guide update.

- The 9h8 Form layer authority surface passed review.
- The block was not in 9h8 source behavior; it was the strict review rule to
  block on any visible C-seed growth while the current worktree contains
  tracked `runtime/fkwu-uni.c` and `runtime/fkwu-optable.h` diffs.
- Response: this receipt now links the C-seed guide work explicitly instead of
  pretending the repo-wide worktree is C-clean.

Follow-up post-review after C-seed guide update:

- Claude/Sema: `PASS`.
- Grok-style: `PASS`.
- The pass is for the 9h8 layer. The tracked dirty C-seed work remains visible
  in the same repo ledger through the C-seed guide receipt.
