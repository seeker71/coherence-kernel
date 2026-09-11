# The file only one kernel could find

2026-09-11, night, M4 Max, Hati Suci. Two consumers of the shards cell split the siblings, Go on
one side and Rust and TS on the other; fkwu, asked for a fourth reading, sided with Go. This piece
finds why and heals the bands.

## Carried

- **Two capability bands read their cell from the directory that holds it** (6720db27).
  runtime-program-image-fkb-current-source-capability-band and
  runtime-program-image-fkb-current-answer-capability-band read their own cell, and sized it for
  one-home, by the root-relative path form/form-stdlib/.... A probe asked every kernel the size of
  core.fk by that path and by form-stdlib/core.fk: run from form/, Go answered 35359 for both, and
  Rust, TS and fkwu -1 for the root-relative path; fkwu run from the body root answered the reverse.
  Go's resolveKernelHostPath walks up the parent directories and tries form/ beneath each, so Go
  found the cell; the others resolve from where they run. So Rust and TS lost the claims that stand
  on one-home: 2047 against 4095, and 16646143 against 16777215. Each band now picks its cell's
  directory by fs_list membership, the way the directory bands pick theirs. Go, Rust and TS now agree
  on 4095 and 16777215, fkwu answers the same, and each band takes a fourth-arm row.
- **Row 1451, rootcrawl** (1ced3b13): a kernel crawling up toward the root for a file the others look
  for only where they stand.

Witnessed on 1ced3b13, every band fresh: freshness 31, corpus 32767, drift pass 8191 of 8191.
current-source-capability reads 4095 and current-answer-capability 16777215, each four-way on its
new row.

## Still open

- **Go's resolveKernelHostPath searches for a path the other three kernels look for only where they
  run.** A band that reads a root-relative path passes on Go and fkwu and fails on Rust and TS, the
  shape this piece healed twice; how many more bands stand on the search is not yet counted.
- **form-knowledge-query-memory-shard-exec** stops on all three siblings at node_eq over the `live`
  record, and fkwu answers 65535 of its 262143.
- The list in receipts/2026-09-11-the-four-names-no-registry-held.md stands, less float-mint's c4
  and public-source-concept-shards.

## Surprise, and where the discomfort went

My first guess was that Rust and TS read a missing file as an empty string; the probe said every
kernel reads it as null, and the difference was elsewhere, in how Go finds a path at all. The band
that lost to it even states the rule it was breaking: its own manifest claims "no ambient
filesystem authority", and its answer depended on one.

The discomfort was guessing before probing, twice in this stretch, and both guesses wrong. The gold:
ask every kernel the same small question side by side before naming which one is out of step.

Frontier word, row 1451: **rootcrawl**, a kernel crawling up toward the root for a file the others
look for only where they stand.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
