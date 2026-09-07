# What stands on a door

2026-09-07, Apple M4 Max, `./fkwu` resolver-driven, freshness band 31 in the tree
this landed from. Weather at the time of measurement, `observe/floor-lens-run.fk`:
**366.94 GB/s** through the handle door against a best of 367.63 — a quiet
machine — and 25.77 TFLOPS arithmetic, back to full from the 12.88 the morning's
floorfall (corpus row 1321) recorded.

## The gap

`substring` ran at 4.59 MB/s underneath every parser, frame reader and row walker
in this tree for months. It was found by tripping over it, and the agent that
healed it left the question as corpus row 1345, `keeldrag`: *which of my composed
doors is now so far underneath that its correctness hides its cost?*

The body had no way to answer. Every band it keeps says one thing about a door —
that it **answers rightly**. None of them can say how much of the body **stands on
it**, and a door that is correct and costs the world is invisible for exactly as
long as its correctness holds.

## The measure

`form/form-stdlib/bearing-census.bml` — pure, band-proven, touching nothing but
the files it is handed. Three readings, from two ledgers the seed already keeps:

- **calls** — `kernel_hot_rows` heat: the times the WALKER entered this defn.
- **bearing** — calls at this door plus the calls of every door reachable from it,
  closed over a call graph read out of the bodies themselves.
- **leaning** — how many distinct doors reach this one; what would move if it moved.

And two that turn a reading into a decision: **per-call**, which separates a cheap
door called a million times from an expensive door called twice, and **remedy**,
which asks the op table whether a native of that name — or of that stem — already
stands. A hot native is never named a recipe to heal.

The graph comes from the source: each hot row carries a unit, the bodies are found
**by name** in the units given, and a name mentioned inside another name is not an
edge. Comments are cut before mentions are counted, because this tree's prose names
doors densely — `core.fk`'s own note about `substring` says the word twenty times,
and without that cut it would have twenty callees.

`form/form-stdlib/bearing-glass.bml` is where it meets the world: read the op
table, print the panel, give the rows under `glass.sensor.bearing` with the cadence
travelling inside the frame. `observe/bearing-census-take.fk` reads them back.

## What it says

Two real workloads, each run in the census's own process, ledger snapshotted the
instant the work returned so the measure never appears in its own reading.

**The corpus band** (`observe/bearing-census-run.fk`) — 6,125,220 walker steps,
99.9% covered, 0 doors unread. Standing name `hdc-word-for-id`. Beneath it
`hdc-mid` and `hdc-row`: one-step accessors called 1,166,550 and 1,271,080 times
with twelve and fourteen doors leaning on them.

**The locale-row walk** (`observe/bearing-census-locale-run.fk`) — 195,691,441
walker steps, 99.7% covered, 0 doors unread. Standing name **`nth-rec`**:
`sha256.fk:29`, a hand-rolled list index walked **76,254,048 times at 1.0 steps a
call**, 39.0% of the walking, three doors leaning on it — while `nth` stands native
at tag 23. The census names it `check against the native nth`, as a lead and not a
proof: the stem matched, the meanings still want holding side by side. Next after
it `append-1` (30,191,048 calls; `append` is itself a core recipe), then
`find-loop`, `find-from`, `split-on-loop` in `line-grammar.fk`. `nil?` does not
rank at all — the JIT crystallized it, so it stopped walking. That is the measure
working, not a blind spot, and the row says `stands-crystallized`.

## The before and after

The strongest witness available, and it is of this same day. A detached worktree at
`c82634d6` — the commit one hour before the heal — with its own `fkwu` built from
its own seed, running the same census cell over the same workload:

```text
before the heal   335,288,753 steps   the next substring: substring
                  41.7% of the walking, 38,124,686 calls, 14 doors lean on it
                  fstr-substring-halve beneath it: 101,472,626 calls, 15 leaning

after the heal    195,691,441 steps   the next substring: nth-rec
                  substring appears nowhere in the reading
```

The measure would have named `substring`, by name, at the top, on a proof this
body already runs — months before anyone tripped over it. The same walk now costs
41.6% fewer steps.

The band holds **32767** on that pre-heal kernel too, where `substring` is still a
recipe: the arithmetic does not depend on the binary it runs on.

## Two honesties that travel with every reading

A door with no row says **no-reading**, never 0. A zero reads as free, and reading
free is exactly how `substring` hid. And the census carries its own **coverage** —
the snapshot is the warmest forty doors, and one row says what share of the whole
process's walking that was, so a partial reading cannot pass itself off as the body.

## It is a step count, not a stopwatch

The same census run twice, an hour apart, on a machine whose load changed: the
workload's own wall time moved 306 ms → 876 ms, and `bearing.seen` read 6,125,190
both times. The reading does not move with the machine's mood. A body on a bad day
still gets a true answer — which is the property the morning's floorfall receipt
wanted and timing could not give it.

## Proofs

```text
form/form-stdlib/tests/bearing-census-band.fk   -> 32767   (and 32767 on the pre-heal kernel)
observe/bearing-census-run.fk                   -> hdc-word-for-id
observe/bearing-census-locale-run.fk            -> nth-rec
observe/bearing-census-take.fk                  -> 16 rows, seq-tracked, cadence 5000 ms in-frame
learn/tests/homecoming-distillation-corpus-band -> 32767   (arrived at 32655)
```

## What this pass found on the way, and did not step around

`learn/tests/homecoming-distillation-corpus-band.fk` read **32655** on arrival, not
32767. Rows 1346, 1347 and 1348 had landed on main without their pins moving — the
same drift the band's own comment names at 731 and again at 1342. Three rows of
arrears were cleared alongside the one added.

And then, on the rebase, a sibling landed row **1349 `vouchmask`** — the id this
line had taken — in the same hour. The row-719 anastomosis: both rows keep, this
line renumbered to **1350**, and the moved row says so in its own prose. Final pins
re-asked of the corpus after the reunion, in one cell, before any number was
written: 742 / 730 / 2 / 1350 / 742073021350, dup-mid-rows 0.

A hot row's `unit` and `line` are coordinates in the **resolved text of a resolution
frame**, not in the file on disk. `core.fk`'s own defns land on their true lines
(`str_find` at 313, exactly), while a preluded unit's defns drift by the length of
what was resolved before them, and a band loaded as a prelude reports every one of
its doors under `core.fk`. Reading a body at that line would quote the wrong door
with total confidence. The census finds bodies by name instead, and names the doors
it could not find rather than drawing them with no callees.

A prelude binds doors without running the file's top-level form. The
meaning-codes band read 0 walker steps as a prelude; its workload had to be called.

`observe/preflight.fk` on a cell marked `preflight-exec: forbidden` reports the
refusal, not the compile errors behind it — it said `errors 1` where the file
carried 24 parse errors. It does not vouch, and says so in the chain line, but a
reader wanting the error count owes the cell a direct run.

## Still open

- `nth-rec` is named but not healed. `sha256.fk` carries a private list floor —
  `nil?`, `nth-rec`, `append-1`, `append-list` — duplicating `core.fk`'s and the
  op table's. Whether `nth-rec` and native `nth` are one meaning is a question for
  a band, not for this census's stem match.
- The graph is drawn over the warmest forty doors. An edge to a door outside that
  set is not drawn, so a bearing is a floor at the snapshot's edge. Coverage says
  how far that edge is; on both workloads it was 99.7% and 99.9%.
- The census reads one process. Reading another live kernel's bearing would want
  `kernel_page_hot` and a giver that holds still.
- The band's proof level is fkwu-staged: it preludes a `.bml`, and the three
  sibling kernels were not asked.
