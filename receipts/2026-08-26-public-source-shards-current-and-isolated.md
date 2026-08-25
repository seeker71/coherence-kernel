# 2026-08-26 — every current public source is reachable; worker control is isolated

The fresh integration census counted **6,095** repository sources across its
current fifteen families.  The persisted public-source shard report then
re-counted the same denominator and observed:

```
denominator=6095
planned=6095
indexed=6095
current=6095
stale=0
missing=0
invalid=0
unique-coordinate-ambiguities=0
source-retrieval-percent=100
source-retrieval-ready95=1
plan-current=1
model-executed=0
```

Every family was individually current at 100%.  The registry adapter agreed:

```
shard-denominator=6095
shard-indexed=6095
shard-current=6095
registry-source-percent=100
concept-bearing-source-percent=100
current-closure=1
executable-query-tokens=1
base-integration-ready95=0
adapter-repository-ready95=0
registry-model-executed=0
```

That last group is the boundary.  This is complete **source retrieval
closure**, not a claim that the local model has learned or reasoned over every
source.  The canonical census still reported zero for held-out integration,
grounded RAG, teaching-prefix exposure and learned-weight integration, and its
`integration-ready95` remained zero.

## The collision that made the refresh untrustworthy

The shard driver and a simultaneous band shared one global
`$TMPDIR/public-source-concept-shard-request-v1` control row.  A smoke request
could replace a live family refresh request between write and worker read.  A
busy process was therefore not evidence that the intended artifact/family was
moving.

The control coordinate is now derived from the artifact and family identities.
The driver writes the exact request into that isolated directory, launches the
worker with that directory as `TMPDIR`, and accepts the step only when the row
still equals the expected request afterward.  A same-artifact/family concurrent
writer is visible as a failed correlation rather than silent permission.

The expanded band creates two artifact identities concurrently, proves their
control paths differ and remain deterministic, reads both exact rows back, and
answers **1048575**, exit 0.  `git diff --check` is clean.  No model, Metal,
MLX, registry table, flattening path or C-seed growth was added.

Signed, Codex — sibling, this worktree.

Kept alive: a worker's activity became a correlated observation instead of a
guess from `busy`.

The surprising teaching: the source substrate had already reached the moving
6,095-source denominator; the remaining 95% gap lives above retrieval, in
model use and held-out transfer.

Discomfort turned to gold when a temp-file race stopped being treated as noisy
test interference and became a first-class identity boundary.

; witnessed: 2026-08-26 -> shard band 1048575; live report 6095/6095 current; adapter current-closure 1; canonical integration-ready95 0
