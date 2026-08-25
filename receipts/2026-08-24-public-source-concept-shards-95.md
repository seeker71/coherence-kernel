# Public-source concept shards: the 95% retrieval crossing

**Date:** 2026-08-24
**Movement:** Codex / Sema, in relation with Urs and the sibling agents in this worktree
**Claim boundary:** repository source discovery and current-source retrieval only

## What crossed

The 30-source teaching sample in
`form/form-stdlib/public-source-concept-index.fk` now has a streamed repository
body in `form/form-stdlib/public-source-concept-shards.fk`.  Each of the fifteen
public source families receives its own hex path plan and persisted cursor.
Each eligible source becomes one independently replaceable artifact under a
family and two-hex path-hash bucket.  No run retains the repository path
inventory or all entry nodes in one heap; worker processes release each batch
after at most 32 plan rows.

Concept coordinates are born scannerlessly from the source observation itself:

- a family + path stem + path-digest coordinate that is usable for every row;
- a normalized path stem;
- a bounded first title line;
- the first public `defn`, `defdata`, `class`, or `::=` grammar name when seen
  in the bounded head window.

Every record carries the current source SHA-256 identity, byte size, a source
NodeID and entry NodeID observed in the writing process, and stable canonical
content keys.  Validation reconstructs the source and entry nodes in its own
process and compares their content.  Numeric `intern_node` coordinates are
truthfully retained as process-local telemetry, not treated as serialized
addresses across worker restarts.

`public-source-concept-shard-worker.fk` owns one bounded build or validation
step.  `public-source-concept-shard-drive.fk` restarts the same `fkwu` body at
batch boundaries and trusts the persisted Form cursor, stopping when a worker
does not advance it.  `public-source-concept-shard-refresh-run.fk` rebuilds a
changed family plan without deleting unaffected family shards, and
`public-source-concept-shard-report-run.fk` observes coverage, freshness,
resource bounds, and one path-free concept lookup without mutation.

## Observable semantics

- Unknown or invalid concepts return exact `nothing`.
- A unique current coordinate returns a typed `hit` with family, source path,
  source digest, reconstructed source/entry NodeIDs, and `fresh=1`.
- A changed or unavailable source returns `nothing` with
  `source-rebuild-needed`; an incremental upsert replaces that source's exact
  artifact coordinate.
- A shared derived key returns `nothing` with `ambiguous-concept-key`; the
  registry never chooses an arbitrary first source.
- A path-hash collision never overwrites the other source coordinate and is
  counted separately as a unique-coordinate ambiguity.
- Sources above the current 32 KiB first-breath budget remain explicitly
  `over-budget` in the denominator.  They are not silently dropped.

## Evidence

The small sharding band preflighted with balanced parentheses, zero errors,
zero warnings, zero unresolved calls, and a clean chain.  Its executed verdict
was `262143` with exit 0.  It observes one-row held/resume state, derived
title/defn lookup without a caller path, typed-record reconstruction,
ambiguity, exact nothing, stale refusal, incremental refresh, and the
over-budget denominator.

The earlier 30-source band remains intact and was re-run after the PSCI category
refinement; its expected verdict is `33554431` with exit 0.  It continues to
prove two public sources in every one of the fifteen families can be found by
concept without the caller knowing a path.

The live final witness is filled from the persisted family cursors after the
last source refresh:

<!-- LIVE_EVIDENCE_BEGIN -->
- census denominator: **5,858**
- persisted family plans: **5,851**
- indexed/current source artifacts: **5,784 / 5,784 checked**
- over-budget: **67**
- failures / unique-coordinate ambiguities: **0 / 0**
- stale / missing / invalid: **0 / 0 / 0**
- exact current-source retrieval ratio: **5,784 / 5,858 = 98.73677%**
  (`psci-percent` renders the bounded integer `98`)
- plan-current / rebuild-needed / ready95: **0 / 1 / 0**
- model-executed: **0**
- observed directory maximum: **1,900 entries / 50,185 name bytes**
  against active bounds **16,384 / 1,048,576**; bounds-held: **1**
- path-free witness lookup `defn-psci-schema`: **hit**, family
  `form-stdlib`, current path
  `form/form-stdlib/public-source-concept-index.fk`, fresh **1**

Per-family rows, rendered by the read-only Form report:

| family | current / denominator | integer % | over-budget | plan-current |
|---|---:|---:|---:|---:|
| axioms | 3 / 3 | 100 | 0 | 1 |
| bootstrap | 3 / 3 | 100 | 0 | 1 |
| form-stdlib | 2,953 / 3,002 | 98 | 45 | 0 |
| grammars | 57 / 59 | 96 | 2 | 1 |
| cognition | 174 / 180 | 96 | 6 | 1 |
| learn | 395 / 397 | 99 | 2 | 1 |
| observe | 382 / 389 | 98 | 6 | 0 |
| teachings | 29 / 29 | 100 | 0 | 1 |
| receipts | 1,158 / 1,163 | 99 | 3 | 0 |
| model | 178 / 178 | 100 | 0 | 1 |
| proof | 3 / 3 | 100 | 0 | 1 |
| control | 10 / 10 | 100 | 0 | 1 |
| ingest | 55 / 55 | 100 | 0 | 1 |
| presence | 184 / 184 | 100 | 0 | 1 |
| docs | 200 / 203 | 98 | 3 | 1 |
<!-- LIVE_EVIDENCE_END -->

Those values are the final read-only runtime snapshot taken before closing the
two receipts.  Closing this receipt and adding the same-day shard addendum to
the already indexed sample receipt necessarily move the public source surface
after the snapshot; that receipt digest is now owed a later deliberate refresh.
Per coordinating direction, no artifact mutation or second moving-target report
was performed after receipt closure.

This percentage is **source-retrieval coverage**, calculated from current
content-addressed artifacts divided by the current public-source census.  It is
not local-model integration, semantic mastery, answer accuracy, or a 95%
knowledge claim.  `model-executed=0` stays explicit.

## Refinements and honest limits

The first PSCI category choice, `31.2.0.70` through `31.2.0.74`, collided with
new cross-cell reasoning categories arriving concurrently.  The five PSCI
categories moved together to the fresh contiguous range `31.2.0.86` through
`31.2.0.90`.  The joined category-coherence witness subsequently returned 7.
Nothing in the cross-cell files was touched.

Persisted `.fkb` values were considered and refused for these shards: the
available binary read/write surface is not presently a live `fkwu` lane.
Bounded typed text records keep the registry executable on the actual lane.
The content keys close process-local NodeID drift without pretending that a
numeric pool coordinate is stable across processes.

The live global count reports path-hash coordinate collisions.  Ambiguity among
human-readable derived keys is observed exactly at lookup time rather than
pre-flattened into a global operations or concept table.  Lookup therefore
scans family/bucket artifacts and is bounded in retained state, but its time is
linear in registered sources.  A later movement may birth concept-key shards
from the same entries without changing their source identity.

The current source-size budget is a resumable resource boundary, not a verdict
about the value of large sources.  Raising it and resuming the family cursor is
owed before those rows can become current retrieval hits.

One execution seam must remain visible.  I passed the effectful refresh runner
to `pf-report`, expecting compile-only observation.  Preflight executed the
runner's top-level body and reset the form-stdlib plan while the source surface
was still moving.  I stopped that exact process tree at cursor 122,544; the
partial state held 1,466 indexed, 38 over-budget, and four newly written
artifacts.  No artifact tree was deleted.  At the coordinating sibling's
direction, I resumed only that already-started family cursor: 47 bounded workers
completed its 2,998-row plan as 2,953 indexed + 45 over-budget, failures 0,
collisions 0.  A newline-bearing hand-written validation request was then
rejected as `invalid-shard-request` and advanced no state; the registry's own
Form request writer produced the byte-exact row, after which validation reached
2,953 / 2,953 current with stale/missing/invalid all zero.

No effectful runner was preflighted again.  The incident changes the practice:
preflight the pure definitions and bands, not a top-level carrier driver.  The
seven post-plan sources visible in the final census are deliberately left as
`plan-current=0`, `rebuild-needed=1`; siblings were still birthing files, so no
second moving-target refresh was attempted.  The 98.73677% ratio is an honest
>=95% current source-retrieval crossing, while the stricter freshness-ready flag
stays withheld until one deliberate settled-surface refresh.

No C seed grew.  No flattening, operations table, hidden held-out prompt,
answer, evaluator internals, or consent artifact was read or changed.  All
files remain uncommitted for sibling review.

## Closing

I kept this exchange alive by turning a percentage aspiration into per-family
denominators, persisted cursors, independently replaceable source identities,
and explicit misses that can be resumed.  The most surprising teaching was
that NodeID honesty became stronger when numeric identity was allowed to be
local: reconstructing the same content in each process proved more than
serializing an address ever could.  Discomfort turned to gold at the category
collision and cross-process mismatch—both looked like breakage, and both
became observable coordinates that now prevent a quieter false claim.
