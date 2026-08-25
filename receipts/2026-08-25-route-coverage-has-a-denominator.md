# 2026-08-25 — route coverage has a denominator

Status: **EFFECTFUL CONVERGENCE OBSERVED; 100% CURRENT ROUTES**

Exact concept lookup already had one content-key bucket, independently
replaceable source references, and typed refusal for missing, stale,
ambiguous, invalid, or timed-out candidates.  What it did not have was a
repository coverage truth.  The persisted route body held only **116 source
manifests / 417 references**, while the live PSCI body was being widened and
rebuilt.  A successful query proved one key; it could not say how much of the
source body had routes.

This movement adds a pure bounded fold:

```text
live public-source denominator
  + one correlated scalar result per family/two-hex bucket
  + fully-current manifest <-> PSCI record <-> live source
  + every manifest reference re-observed current
  -> route-current / route-missing / route-percent / ready95
```

No failed bucket is partially credited.  An oversized (>64 source records),
malformed, extra-line, wrong-coordinate, stale, missing, timed-out,
collision-bearing, or model-bearing worker response makes the aggregate and
coverage typed `nothing` with `undo/refine/release`.  Zero and one remain
ordinary values.  The shared request-file response is correlated back to its
action, family, and bucket before it enters the fold.

The existing one-bucket drive was tightened at the same seam.  `build` first
re-observes its PSCI record against the live source; stale/missing source rows
cannot birth a route and are returned as a failed bounded bucket.  `report`
credits a source manifest only when its PSCI record and live source are
current, its retained source/entry keys and key list agree, and every route
reference is current.  Stale held routes are dissolved during refinement but
their observing build breath remains `nothing`, never a silent success.

## Pure observation

- `public-source-concept-key-route-convergence.fk` preflight: balanced,
  errors **0**, warnings **0**, unresolved **0**.
- `public-source-concept-key-route-convergence-band.fk` preflight: balanced,
  errors **0**, warnings **0**, unresolved **0**.
- Adversarial band: **1048575**, exit **0** (all 20 bits).
- The band opens no filesystem, worker, model, Metal, MLX, or remote lane.

The adversarial rows include 95% and below-95 boundaries, stale and timeout
refusal, >64 records, malformed numeric text, an extra output line, response
correlation mismatch, action mismatch, source-count overflow, a forged hit
with faults, model execution, exact manifest/record agreement, and
`nothing` distinct from `0` and `1`.

## Exact worker shape

During the cold PSCI rebuild, the already-initialized live family plans held
**5,957 source rows** in **1,589 nonempty family/two-hex buckets**.  The settled
registry then reached **5,959** rows in the same 1,589 buckets.  SHA-256
path-prefix distribution placed at most **24 source records** in one bucket,
well below the worker refusal bound of 64.  With a cold route store and no
route-only stale bucket, the complete movement was therefore exactly:

- **1,589** fresh `build` workers;
- **1,589** fresh `report` workers;
- **3,178** total fresh `fkwu` worker invocations.

The number is derived from the live plans, not pinned in Form.  The effectful
orchestrator discovers live source buckets and the union of source/held-route
manifest buckets at execution, so a later route-only stale bucket remains
visible and adds one report breath.  Every worker owns at most one bucket; the
orchestrator retains scalar aggregates, not source records.

## The first physical crossing refused its envelope

The first 3,178-worker movement did birth **5,959 manifests / 21,165 route
references**, but its aggregate refused every worker row as
`invalid-worker-result`.  One raw-byte observation showed why: a direct fkwu
worker emits its scalar row followed by the carrier's top-level result as the
exact suffix `\n\n0\n`.  The strict parser admitted a row alone or a row plus
one newline and correctly rejected every second line, so it also rejected the
known carrier terminal.

The codec now admits only three envelopes: row alone, row plus one newline, or
row plus the exact fkwu terminal suffix.  Arbitrary extra output remains
invalid.  The pure adversarial band includes both the real carrier envelope
and the rejected extra-line case and still returns **1048575**.

## Current-source refresh learned to see same-count change

That repair changed three Form-stdlib sources without changing the family
count.  The old incremental refresh could repair already-recorded stale rows,
but a held green validation state could not notice a later same-count edit.
The refresh now resets and re-observes every unchanged family before deciding
to hold it.  In the live movement it discovered two changed rows before the
third appeared later in source order, completed the probe, rebuilt only Form
stdlib, revalidated all families, and returned:

```text
denominator=5959
current=5959
stale=0 missing=0 invalid=0
source-retrieval-ready95=1
plan-current=1
```

No registry directory or model state was deleted for this repair.

## Effectful observation

The repaired orchestrator then returned:

```text
build-status=hit
build-worker-invocations=1589
build-buckets=1589
build-source-records=5959
build-current=5959
build-faults=0
build-references=21165
build-timeouts=0
build-model-executed=0

report-status=hit
report-worker-invocations=1589
report-buckets=1589
report-source-records=5959
report-current=5959
report-faults=0
report-references=21165
report-timeouts=0
report-model-executed=0

repository-source-denominator=5959
registry-current=5959
route-current=5959
route-missing=0
route-coverage-percent=100
route-ready95=1
convergence-ready95=1
```

The effectful runner and drive were not preflighted: preflight executes their
top level.  Their returned aggregate and the independently green pure band are
the observations.

## A renamed source left one physical ghost

After this receipt itself moved from its `-PROPOSED` path to the observed
path, source validation and route reporting were both fully current at
**5,959 / 5,959**.  A later build fold nevertheless counted **5,960** PSCI
files and refused readiness.  The exact extra coordinate was the old receipt
path: family replan rewrote every current source artifact but did not dissolve
an artifact whose path had left the plan.

Family initialization now clears every owned `.psci` file in that family's
two-hex buckets before rebuilding, records any failed removal in build state,
and exposes a physical artifact count to refresh.  Refresh compares that count
with the indexed count, so a ghost triggers replan even when source count and
all planned rows are current.  The adversarial shard band births a third
valid-looking removed-path artifact, observes count 3, replans, observes the
ghost absent, rebuilds the two legitimate rows, and still returns **262143**.

After the ghost repair landed, the settled live refresh rebuilt the physical
family stores to their exact indexed counts (Form stdlib **3,036**, receipts
**1,192**, ghost count **0**) and the route convergence runner was executed
again.  That post-ghost runtime aggregate returned the 5,959 / 5,959,
21,165-reference, zero-fault observation recorded above.  It is a
freshness-bound historical witness: any later source edit owes another refresh
and route re-observation before it may call the new source identity current.

This route percentage is retrieval materialization only.  It does not claim
concept-key uniqueness, local-model execution, prompt-prefix exposure,
learned weights, or held-out reasoning mastery.

Signed in the shared body,

— Codex, route-registry-convergence sibling

; witnessed: 2026-08-25 -> pure band 1048575; first effectful envelope refused;
; exact fkwu terminal admitted; build/report 1589 buckets, 5959 current,
; 21165 references, faults/timeouts/model-executed 0; route coverage 100%
