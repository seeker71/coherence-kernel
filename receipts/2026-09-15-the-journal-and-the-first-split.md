# The journal and the first split: a branch stops at fail, fields roll back, the compiler speaks

2026-09-15, M4 Max, Hati Suci. The coordinator lifted the four-way hold and named the two state
leftovers the north star carries: record fields inside the save snapshot, so a field set in a failing
choose branch rolls back; and a Hati branch that stops at its fail. Urs's direction this morning holds:
no new band or proof file, the expectation lives in the compiler and speaks through the organ.

## Carried

- **A Hati branch stops at its fail.** The fail flag moved from a frame slot to the run's FAILED
  memory cell (4005), so a callee's failed choose reads the same cell as its caller. Inside a branch
  every statement guards the next on FAILED; outside one, a statement that holds a tracking choose
  does. A choose whose every branch fails leaves FAILED set and the body answers the value it stood
  on (`__top`, now kept by every body that takes a snapshot), which is what BMA's fail mode leaves on
  its stack. A choose whose every branch is a bare `fail;` used to be withheld
  (`choose/no-success-branch`); it now lowers the same way and agrees with BMA. In a body that
  carries control, the join after such a statement and the loop's re-entry read FAILED too.
- **Record fields inside the snapshot.** A field lives in its record, outside every frame. While a
  snapshot is open (DEPTH, cell 4008, above 0) each field set first journals the record, the key and
  the value it replaces (JOURNAL 4006, COUNT 4007). A choose's snapshot and a save's snapshot carry
  COUNT; a failed branch and a restore call one shared undo function, linked last into a table that
  journals, back to that count. It reaches every record: Go's failing branch sets another object's
  field through that object's own method, and the set is undone.
- **The compiler speaks at the snapshot.** grammars/bml.fk preludes
  form/form-stdlib/bml/bml-compiler-health.bml. A restore or discard with no open snapshot withholds
  its table through `bch-admit`. A choose whose every branch fails voices a reading, the remedy it
  offered (`fail-through`) and the result applied. A branch-bearing statement checks that no outer
  name it assigns still reads from the argument after its promotions (the branch edge), and voices
  the names if one does. `bml-run-lanes-unit-value` runs one source on both lanes: a withheld lane
  says where it ran (`bch-ran`), two answers that differ are a split (`bch-lanes`).
- **The split the organ found, healed.** BMA inlined a callee's choose statement through the generic
  child rule, so the chosen branch's values stayed on the stack under the callee's result.
  `Stop() + Nested()` read 14 on BMA and 9 on Hati. A choose statement in an inlined callee now keeps
  the callee's statement rule in every branch; both lanes read 9.

Canonical sources: `form/form-stdlib/tests/fixtures/bml-choose-state.bml` (both lanes answer 12)
and `form/form-stdlib/tests/fixtures/bml-choose-fields.bml` (Hati answers 8; BMA names
`bma/class-lane`). The carrier's next code point moves on:
`form/form-stdlib/bml-native-mutable-locals.fk`, band 1023.

## Witnessed

Live on fkwu through the both-lanes door: choose-state 12 with the two all-fail heals voiced and no
split; choose-fields 8 with BMA's `bma/class-lane` answer voiced; branch-assign 51, silent; a restore with
no snapshot withheld in the open. Every bml.fk consumer on fkwu reads the verdict it read before:
the twenty-eight BML native proofs at their marks (state-snapshot 25 now voices its
restore-with-no-snapshot case), `bml-native-mutable-locals-band` 1023 with the new next code point,
`bml-native-source-control-band` 2047, ontology-emit 31, full class model 1073741823, class
inheritance 8388607, compiler-runtime 131012, action-runtime 8388680, and every thesis band at its
own. Drift door: pass 16383 of full 16383, refused 0. Four-way through `form/validate.sh`: the
thirty-three rows of `form/fourth-arm-bands.txt` that prelude grammars/bml.fk or the mutable-locals
carrier each read `1 band(s) four-way`, `1 ok, 0 divergent`, the organ's lines included.

## Still open, with the reason

- A method whose choose failed, called outside any choose: its caller's statements go on, where BMA's
  fail mode stops the whole program. Only a branch, or a statement that holds a tracking choose,
  reads FAILED after itself today. The carrier names this as its next code point.
- A body that carries control and stops at a failed choose answers the control protocol's 0, not
  the value it stood on: the control lowering keeps no standing value, and BMA carries no if or while
  shape to compare against.
- A bare field assignment (`v = 5;` inside a method of a class with field `v`) reads
  `field/no-receiver` on Hati, inside a choose or not; `this.v = 5;` works. That belongs to the
  class lane and is left to it.
- `host-kernel-interface-proof` (34) and `kernel-core-image-compiler-proof` (30) exit 1 on fkwu with
  origin's bml.fk as well: `walk_recipe` errors on the fkwu lane, not this change.

## Closing

Most surprising: the organ's first live run found a split that twenty-eight proofs never saw. Each
method agreed with itself on both lanes; only their sum split, because BMA kept a chosen branch's
values between the two operands. A proof that runs one method at a time cannot see what sits under
a call's result.

Discomfort to gold: with the journal in place, a field set after a `fail;` is undone anyway, so
the stop at fail has no observable value of its own in that shape. The pull was to call it done
without it. Keeping it made the all-fail choose, the callee's failure and the loop's re-entry one
rule: FAILED stops what comes after, wherever it was set.

Frontier word: **pairsplit** (0 hits in the tree). Two parts that agree alone and split together:
which lane invariant does a single-method proof leave unasked, and who asks it at the join?

— Claude (Opus 5), as Sema, worktree agent-a69badcddce1f9c16
