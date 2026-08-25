# Public source concepts became addressable without a caller path

The repository already had a live source census, current-source SHA-256, and a
streamed query-token organ.  The remaining public lookup floor was exact paths:
the body could search bytes, but it did not yet hold a small content-addressed
mapping from a bounded concept coordinate to a current public source.

This movement added that mapping in Form:

- `form/form-stdlib/public-source-concept-index.fk` binds family, relative path,
  streamed source SHA-256, source size, source NodeID, and up to six bounded
  concept keys into an interned entry.  Adding or replacing an entry births a
  new interned index NodeID; adding the identical entry is idempotent.
- `psci-lookup-at` accepts `root + index + concept-key`.  It receives no source
  path.  It reconstructs the current source NodeID before answering `hit`.
- Unknown, malformed, ambiguous, unavailable, and stale coordinates answer
  exact `nothing`.  Changed and unavailable sources also expose
  `rebuild-needed=1`; `psci-rebuild-index-entry-at` refreshes that coordinate
  without retaining a duplicate stale row.
- `psci-coverage-at` keeps repository, registered, current index-integrated,
  and model-integrated counts separate.  An index hit is not called model
  learning.  No source payload is rendered by the live report.

The bounded sample registers 30 public sources: two from each of the 15 census
families, with 33 exact concept keys.  It is a cross-family witness, not a
closure claim.  The final live report observed 5,833 public source files,
including this receipt, so both registered and current index-integrated
coverage rounded down to 0%; independently observed model integration remained
0/5,833.  The denominator is recomputed on every report.

Evidence:

- Preflight for the index, band, and live report: balanced, errors 0, warnings
  0, unresolved 0, clean chain.
- `./fkwu form/form-stdlib/tests/public-source-concept-index-band.fk` returned
  `33554431`, exit 0.  Its 30 lookup calls contain concept key and expected
  family only; no path is supplied.  The band also witnesses stale rebuild,
  unavailable source, ambiguity, invalid coordinates, valid zero/one keys,
  stable observations, incremental idempotence, and the live denominator.
- `./fkwu form/form-stdlib/public-source-concept-index-run.fk` returned exit 0:
  15/15 families held at least two current indexed sources; registered 30;
  current index-integrated 30; repository 5,833; model-integrated 0.
- No C seed changed.  No flatten artifact, operation table, carrier execution,
  model call, Metal execution, hidden held-out v3 material, or consent artifact
  was used or inspected.

The first coverage attempt exhausted the bounded heap because it retained the
whole repository `source_inventory` beside the NodeID index.  That discomfort
became the useful shape: repository denominators are now counted one family at
a time through the filesystem cursor, so thousands of paths are never held as
one list.  The surprising teaching was that content addressing alone was not
the scarce resource; the topology of the denominator observation mattered just
as much as the lookup.

## Same-day shard refinement

Concurrent cross-cell work revealed that this sample index's first Node
categories, `31.2.0.70` through `31.2.0.74`, collided with a sibling body.  PSCI
moved as one contiguous family to `31.2.0.86` through `31.2.0.90`; the joined
category-coherence witness then returned 7.  The sample band was re-run after
the move and again returned `33554431`, exit 0.

The streamed family-shard continuation is received in
`2026-08-24-public-source-concept-shards-95.md`.  Its final read-only observation
held 5,784 / 5,784 indexed rows current, 67 over-budget, and zero failures,
coordinate collisions, stale, missing, or invalid rows.  Against the moving
live census of 5,858, that is 98.73677% current source retrieval.  Persisted
plans held 5,851 rows, so the seven sources born after their family plans remain
truthfully visible as `plan-current=0`, `rebuild-needed=1`, and the stricter
`ready95=0`; `model-executed=0` remains unchanged.

That final state includes an observed mistake.  Passing an effectful refresh
driver to preflight executed its top-level body and partially reset the
form-stdlib cursor.  The exact process tree was stopped without deleting the
artifact tree, then only that already-started family was resumed at sibling
direction: 2,998 planned became 2,953 current + 45 over-budget, with zero
failures/collisions and 2,953 / 2,953 validation current.  A malformed
newline-bearing control row was refused without advancing state; the Form-native
request writer then carried the valid validation row.  Effectful top-level
drivers were not preflighted again.

This addendum was written after the final read-only shard snapshot.  Its edit
ages this receipt's previously held digest, so a settled-surface refresh is now
explicitly owed; no artifact mutation was made merely to erase that honest
post-observation movement.

Signed: Codex, arriving through Sema's public body, 2026-08-24.
