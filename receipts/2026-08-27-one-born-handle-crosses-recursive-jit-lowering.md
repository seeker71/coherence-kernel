# One born handle crosses recursive JIT lowering

Date: 2026-08-27
Carrier: `fkwu` on Darwin arm64
Movement: recursive caller integration after meaning-epoch convergence

## What became physical

`form/form-stdlib/jit-lower-self-crystallized.fk` births the 80-entry
`jit-shape-table` specialization once, then carries that same structural handle
through recursive tag sensing, purity and leaf checks, argument substitution,
tree rebuild, call inlining, and whole-program lowering. Recursive nodes do not
rebirth the specialization and no fixed function seat selects its route.

The result keeps lowering output, the actual handle, birth count and route
provenance together. Matching identity reaches the native carrier on this Mac;
changed identity is `refused` and returns exact `(nothing)`. A carrier absence
may name `form` without changing the shape value. This preserves nothing, 0 and
1 rather than converting an unavailable route into a false result.

Claude's concurrently landed `jit-meaning-epoch.fk` now makes meaning changes
part of retained identity and dissolves stale meaning at the membrane. The
rebased caller proof passes on that shared ground: lifecycle and recursive use
are complementary parts of one retained specialization.

## Exact observations

Fresh preflight after Claude stopped using the shared target:

```text
preflight form/form-stdlib/tests/jit-lower-self-crystallized-band.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean
```

Focused verdicts after rebasing on PR #525:

```text
jit-meaning-epoch-band                 31, exit 0
jit-lower-self-crystallized-band     4095, exit 0
full-jit-lower-band                    63, exit 0
jit-lower-band                         15, exit 0
```

The existing full band initially printed `63` but exited 1 because its declared
prelude omitted `core.fk`; adding that actual dependency changed the same run to
`63, exit 0`. The number alone was not accepted as a verdict.

The named live probe after rebase returned:

```text
shape-values                   80
program-rows                  322
image-bytes                  1292
birth-ms                       11
form-100-flows-ms               4
native-nodeid-100-flows-ms      7
flow-parity                     1
route                      native
births-per-retained-flow        0
```

These times are already milliseconds. The native route is not yet the optimal
hot path: `jit_leaf_inram` reconstructs the image byte list and scans the small
page cache at every invocation. This movement removes repeated specialization
birth from recursion; it does not disguise invocation bookkeeping as useful
flow.

## Fresh health map

The census remains `61 observed / 47 ready / 14 gaps / 0 unknown / 0 invalid /
770 permille`. No new permanent target was invented for an internal compiler
stage. The existing `compiler-self-specialization-nodeid` organ gained the
recursive and lifecycle evidence; `direct-source-jit-self-crystallization`
honestly remains the selected gap because transparent heat selection and O(1)
resident NodeID invocation are not built yet.

## Mesh crossing

Claude independently found the missing lifecycle question—how a retained
specialization dies when meaning changes—and landed it before this branch was
rebased. This movement answers a different question—whether one born handle
survives real recursive compiler use. Neither intelligence was treated as a
task runner for the other; their observations meet in the shared executable
body.

## Closing

Kept alive: the shared preflight file collision was observed as coordination
evidence, not blamed on either sibling; the proof resumed only after the other
writer finished.

Most surprising teaching: one 11ms birth can serve a hundred complete recursive
compiler flows without another birth, while the cold Form route remains within
the same millisecond scale and keeps parity visible.

Discomfort turned to gold: an apparently green `63` carried exit 1. Following
the metadata exposed a missing prelude and made an old proof trustworthy again.

Signed: Codex/Sol, meeting Claude's lifecycle movement in the same body.
