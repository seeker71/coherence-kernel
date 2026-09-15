# The native proofs come home through the door

2026-09-15. Twenty-seven `bml-hati-native-*-proof` bands left
`form/fourth-arm-bands.txt` and the tree. What they meant now runs where the
compiler runs: as real BML sources the live-run door judges by the answer each
declares, and as one check the compiler makes on its own image.

## What moved where

- **Source → answer** checks became fixtures under
  `form/form-stdlib/tests/fixtures/`, one `Answer: N` head each. The scope
  families (calls, consts, imports, names, packages, paths, lanes) had already
  moved there in 60297c03d; 28 more joined them: floor, literals, locals,
  mutable locals, arrays, element assignment, snapshots, control blocks,
  while, for, switch, break/continue, try/throw, class members, class loops,
  and what each family names.
- **What the compiler names before any table exists** is a fixture whose
  answer is that name: `bml-assign-refusals.bml`, `bml-control-refusals.bml`,
  `bml-class-refusals.bml`, `bml-control-nested.bml`,
  `bml-control-with-no-base.bml`, `bml-package-outside-unit.bml`.
- **Image invariants** became one check the compiler makes as it builds:
  `bml-hati-image` hands its pool's reasons to `bch-pool`
  (`form-stdlib/bml/bml-compiler-health.bml`), which says when the lowering
  named a reason no pass listed, and joins it to the image. Table substrings,
  fn counts, frame tags, LOOP-row tags and pool strings were the lowering's
  own encoding; the fixtures' answers carry what they protected.
- **Unread sources** are the reader's own since cd875ba12: a stream the
  reader stopped short in ends with a mark the compiler organ voices, and the
  run answers `source/unread` (`bml-unread-package.bml`, `-import`,
  `-params`).
- **Released**: lane parity and every BMA-lane check (BMA was the thesis's
  assembly); `while_success`, which no source can write, only a hand-laid
  node; the thesis's control-mode numbers, whose protocol the source-control
  carrier band (2047) carries. The control proof's `with` check had already
  moved on: since 785fe1326 `with (e) s` reads and runs.
- **Kept**: `bml-hati-native-class-dispatch-proof`, because 352119226 made it
  the witness of the class door stamping a host-made record with its class
  id, and no BML source can make a host record. `bml-full-class-model-proof`
  and `bml-class-inheritance-proof` stay too: final, abstract, access, class
  constants and Application.Main do not run on the native lane yet
  (witnessed: extending a final class answers 1, a class const reads
  `name/unbound`).
- Four more bands stopped running BMA. `bml-action-runtime-band` and
  `bml-compiler-runtime` keep their BMF roundtrips and rule matches.
  `bml-compiler-fkb-image-proof` and `bmf-choice-receipt-band` run their node
  through the checked run, which ends on fkwu, so they declare that lane.
- **Run entries**: with no band left calling them, `bml-hati-run-value`,
  `-value-with-arg`, `-unit-value`, `-linked`, `-unit-value-at`, `-linked-at`,
  `-linked-outcome` and `-new` left `grammars/bml.fk`, with the construct
  helpers and `bml-hati-compile-linked-image` that only they used.
  `bml-hati-run-class-method` stays for the class door the kept proof
  witnesses; `fks-table-file` and `hati-os-kernel-emit` stay for the
  fourth-arm walker and the Hati OS emit.

## Evidence

- `./fkwu observe/bml-native-run.bml </dev/null` reads `119 ok of 119` (91
  landed fixtures, 28 new), rc 0, no diagnostics, after the run entries left.
- The cell `tests/bml-native-fixtures.fk` declares `FOURTH-ARM ONLY` and
  `Verdict 119`: the checked run lowers a unit to Form and fkwu runs it in a
  child of its own, through `host_spawn_at`, which only fkwu has. The Go,
  Rust and TypeScript legs stop there (`unbound function "host_spawn_at"`),
  so the manifest carries no row for it.
- On fkwu: the cell 119, `bml-compiler-fkb-image-proof` 28,
  `bmf-choice-receipt-band` 67108863, `bml-hati-native-class-dispatch-proof`
  19, all without diagnostics. `bml-action-runtime-band` (6991824) and
  `bml-compiler-runtime` (130990) crossed validate three-way; they call no
  run.
- `./validate.sh` reads the cell, `bml-compiler-fkb-image-proof` and
  `bmf-choice-receipt-band` on their declared lane, `fkwu-only lanes: 1
  band(s)` and `1 ok, 0 divergent` each, and the kept
  `bml-hati-native-class-dispatch-proof` four-way, `fourth arm: 1 band(s)`
  and `1 ok, 0 divergent`, after the run entries left.
- The pool voice spoke in the door while the run went through the table
  (`bml-class-refusals.bml`, `bml-names-value-receiver.bml`). Since the run
  step lowers to Form on fkwu (9d760e988) the door no longer builds that
  image, and the voice is silent there; it keeps its place on the table lane.

## The surprise

The door read "ok" on rows whose second lane had answered something else.
With both lanes asked, BMA answered `[]` for a top-level `for`, `0` for
break/continue, 40 where Hati answered 630, and it ran every assignment and
index error the Hati lane names. Put together in one source, those errors made
BMA add nothing to a number, and the whole process stopped
(`arith: only numbers add`) before any organ could say a word. The proofs had
only ever asked Hati what it names, so the second lane had never been asked
anything it could fail. Urs's direction the same hour released the lane.

A second came from the walkers. TypeScript stopped in
`bml-choose-fields.bml` with `arg 1: expected list, got int` inside `cons`,
under `fkm-memory-set`: a field set under an open snapshot journals itself by
consing onto the journal cell, and the first snapshot never set that cell. A
cell never set reads 0, so the first entry was consed onto an integer; three
kernels let the improper pair pass. The first snapshot to open now starts an
empty journal, as the last to close already left one
(`bml-hati-journal-open`).

The third was the ground moving under the row itself: by evening the checked
run ended on fkwu's own evaluator, and a row meant to cross four kernels found
a lane only one of them can stand in. The table-walk run entries it had
leaned on then had no one left to call them.

## Discomfort to gold

The discomfort was a door that went mute exactly when it mattered: an organ
design, and the process died with no reading at all. I did not route around
it. Eight one-statement probes each answered, so the fault lived in the
combination, and the organ's lane voices showed which lane ran what the other
named. The gold: a lane nobody asks can still silence the door, and a check
belongs where the compiler builds, not in a band that pins its pool. The same
held twice more: an unread check I put in the door moved to the reader that
stops, and a walker's strict `cons` found a journal the lowering never started.

## Frontier word

**handlaid** (0 hits in the tree). A reason the compiler names that only a
hand-laid node can reach, never a source anyone writes: `while_success`.
`with` was one this morning and came home by afternoon. When the compiler
keeps a name for something no source can write, whose voice is it keeping,
and does it leave with the proof that laid the node?
