# The first read of a state never written

2026-09-11, night, M4 Max, Hati Suci. public-source-concept-shards-band answered nothing on fkwu,
the plainest miss left on the fkwu-only lane. This piece carries it.

## Carried

- **public-source-concept-shards reads a state file that is not there yet as empty** (39c3f3d4).
  A fresh family has no build or validate state file until its first batch writes one. The two
  state readers handed `fs-read-text`'s answer for a missing file straight to a parser that splits
  what it reads, and on fkwu that answer is no string: a probe read its value kind as null. So the
  band stopped at "str_len: nothing has no length". The readers now ask `fs-stat-size` first, as the
  cell's entry reads already did, read an absent file as "", and the parsers fall to their zero
  state. The band reads 1048575, its Verdict. public-source-concept-shards.fk is one of form-cli's
  sources, so its bootstrap regenerates with it (stamp 69a655cedf348313), and `go test -run
  '^TestFkwu'` passes on it.
- **Row 1450, firstread** (294072f5): a family's first read of a state it has not written yet.

Witnessed on 294072f5, every band fresh through validate.sh: freshness 31, corpus 32767, drift pass
8191 of 8191. Of the eight bands that load the cell, public-source-concept-shards reads 1048575 on
its fkwu lane, and key-route-convergence (1048575), key-routes (1073741823),
form-nodeid-knowledge-routed-query (67108863) and form-nodeid-knowledge-query (68719476735) pass on
Go, Rust and TS.

## Still open

- **Three consumers of the cell fail on the siblings, the same before and after this change** (an
  A/B with the cell set back to its version before the heal): form-knowledge-query-memory-shard-exec
  stops on all three at a null NodeID, runtime-program-image-fkb-current-source-capability reads
  4095 on Go and 2047 on Rust and TS, and runtime-program-image-fkb-current-answer-capability reads
  16777215 on Go and 16646143 on Rust and TS.
- The list in receipts/2026-09-11-the-four-names-no-registry-held.md stands, less float-mint's c4
  and public-source-concept-shards.

## Surprise, and where the discomfort went

The cell already knew how to ask before reading: its entry reads check `fs-stat-size` first. Only its
two state readers did not, and they are the reads a fresh family makes first. A cell can hold the
right habit and still skip it in the one place that runs before anything else exists.

The discomfort was that the band's own error names no place: fkwu says "nothing has no length" and
stops, with no stack. The cause came from asking the doors directly, a probe of what a missing
read answers, rather than from the band. The gold: when a kernel gives no stack, ask the doors the
band stands on one at a time.

Frontier word, row 1450: **firstread**, a family's first read of a state it has not written yet.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
