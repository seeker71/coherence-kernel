# One snapshot, two lanes: element sets, save and restore, branch assignments, the ontology in Form

2026-09-15, M4 Max, Hati Suci. The coordinator carried Urs's ask from the morning: close every known
gap rather than name it. Four were left from the first round of the BML native lane.

## Carried

- **Element assignment**, `form/form-stdlib/tests/bml-hati-native-element-assign-proof.fk`, 29 of 29,
  commit 001fda5d3. `a[i] = v;` and `a[i][j] = v;` read as the whole-array assignment
  `a = set(a, i, v)`, so declare, assign and the frame keep one home. Hati calls one list-set function
  that `bml-hati-image` links last into a table that needs it; BMA has a list-set op. An index already
  seen past the end, a const target and a scalar target are named before emission.
- **The ontology generator in Form**, `form/form-stdlib/bml/ontology-emit.bml`, door
  `form/form-stdlib/ontology-emit-run.fk`, band `form/form-stdlib/tests/ontology-emit-band.fk` 31 of 31,
  commit b8b38ff3e. The door's output is cmp-identical to `dialect-categories.fk`; the engine rows it
  emits are cmp-identical to `engine-constants.fk` lines 7-11. All 196 bindings of the hand-kept mirror
  stand; `natural.bmf` joined the JSON at the registry's 99/760-763. `scripts/generate_ontology.py` is
  absent from the tree and its history; the notes that named it now name the Form unit. Release-ledger
  R85 released (35000106).
- **save, restore, discard as one state snapshot**, `form/form-stdlib/tests/bml-hati-native-state-snapshot-proof.fk`,
  25 of 25, commit 2a8cf44a5. Every source runs to the same value on BMA and Hati. BMA's locals now ride
  the floor of its value stack, so its stack snapshot carries them; Hati keeps the standing value in the
  frame's `__top` slot and a snapshot stack under `__marks`. `discard;` and `save;` read from source; a
  restore with no snapshot is named. The restore proof now runs the pair on Hati to 2 (17 of 17).
- **An assignment carried out of its branch**, canonical sources in
  `form/form-stdlib/tests/fixtures/bml-branch-assign.bml` (the Hati lane answers 51 in a live fkwu run;
  no new proof file, the expectation moves into the compiler's own voice next), this commit. A while body's local and parameter cross the back-edge; a try body's and a
  catch's assignments stand after the try; an if branch's parameter stands after the join (the control
  landing's promotion and frame ferry). `choose { ... } , { ... }` reads from source and runs over the
  snapshot save takes, with a `__failed` flag in it: a failed branch returns every slot, the next one
  runs, and the branch that succeeds carries its assignment out, a parameter's included. BMA and Hati
  read 2 for a branch that fails after two assignments. The carrier's next code point now names what
  is left: record-backed state set inside a failing branch.

## Witnessed

On the tree rebased onto the control-flow landing (19944ae23 and f602b5580): every BML native proof at
its mark (class-dispatch 17, class-field 21, class-template-overload 23, compiler 39, floor 26, locals
20, method-calls 19, multi-arg 20, overload-resolution 19, general-calls 22, import-symbol 36,
many-arg-methods 23, package-namespace 31, unresolved-names 20, import-consts 13, lane-parity 14,
path-words 11, control-block 24, while 19, for 22, switch 20, break-continue 25, try-throw 21,
mutable-locals 34, arrays 28, restore 17, element-assign 29, state-snapshot 25);
`bml-native-mutable-locals-band` 1023, `bml-native-source-control-band` 2047, ontology-emit 31,
compiler-runtime 131012, action-runtime 8388680, thesis 131071 / 262143 / 2097151. Preflight clean on
the new proofs. Drift door: pass 16383 of full 16383, no gate held back.

## Still open, with the reason

- A field set (`this.f = v`) inside a failing choose branch is not in the frame snapshot, so Hati keeps
  it; the carrier names it as the next code point.
- Hati runs a branch's statements after its `fail;` and then rolls the frame back, where BMA stops at
  the fail. Frame slots agree; an effect outside the frame between the fail and the branch end is not
  undone. A choose whose every branch fails sets the flag and its body goes on; BMA ends in fail mode.
- `form/scripts/build_form_compiler_artifact.sh --categories` still generates `python-bmf-categories.fk`
  with an inline python3 heredoc. It is a separate generator from the one this round named; it did not
  come home here.
- The new proofs and the ontology band are not in `form/fourth-arm-bands.txt`; I witnessed fkwu only.

## Closing

Most surprising: after the names landing, BMA programs lost their floor frame and their callee stores
at once. One missing op read as three verdicts off: lane-parity 12, mutable-locals 33, element-assign
exiting 1. The op stream printed `[lget, return]` with value 0, and that was the whole diagnosis. And
the control landing had already ferried the bnml frame across loop re-entry, so the snapshot stack rode
into loops without a line from me.

Discomfort to gold: the order asked me to wait for the control-flow landing, and it kept not coming.
The pull was to build a second backtracking mechanism beside it. Building the save/restore snapshot
first and waiting meant choose needed nothing new: the same snapshot and one flag.

Frontier word: **floorframe** (0 hits in the tree). When a VM's locals ride the floor of the stack it
snapshots, which state still sits outside every snapshot, and who names it?

— Claude (Opus 5), as Sema, worktree agent-a69badcddce1f9c16
