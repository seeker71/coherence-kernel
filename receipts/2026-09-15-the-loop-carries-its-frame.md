# The loop carries its frame: BML source control on the Hati table

2026-09-15, M4 Max, Hati Suci. The lead's work order: close the control-flow family of north-star
gaps in `form/form-stdlib/grammars/bml.fk`, one family per proof on fkwu, wire the compiler to the
`bml-native-source-control` and `form-control-backtracking-ml` carriers, and mint no category.

## Carried

- **general-control-block-lowering**, `form/form-stdlib/tests/bml-hati-native-control-block-proof.fk`,
  24 of 24. `if (c) s else s` bare, braced, nested and without else; `loop` with a break and a
  labeled outer break; `&& || ! < <= > >= == != - *`; a parameter assigned inside a branch read after
  the join. `with` is named before emission (see below).
- **arbitrary-while**, `bml-hati-native-while-proof.fk`, 19 of 19. The condition is re-read on every
  pass against state that changes under it: record-backed self, a for-owned local, a mutable local,
  a parameter counting down. A labeled continue, a break, and 200 passes (the BMA `while_success`
  reference loop stops at 64). The one-child `while_success` node is still named "while" before emission.
- **general-for**, `bml-hati-native-for-proof.fk`, 22 of 22. The init is the driver's bnml local, the
  update reads `=`, `++`, `-=`; empty clauses; nested loops with a labeled continue; a loop inside a
  called method re-enters its own table index; sums through self and through a local. The for variable
  stays scoped to its statement and a const target is named (`name/unbound`, `assign/const`).
- **general-switch**, `bml-hati-native-switch-proof.fk`, 20 of 20. The scrutinee is read once into a
  cell; fall-through until break; stacked case labels; default in the middle, where a later case
  still wins; a labeled switch break; inside a loop, break leaves the switch and continue reaches the
  loop.
- **general-break-continue-source-lowering**, `bml-hati-native-break-continue-proof.fk`, 25 of 25.
  `break scan;` reads through the fixed `break-simple` rule, whose emitter was unreachable from source;
  bare jumps and property-word labels read through the control reader. A jump with no target is named
  (`break/outside-loop`, `break/label`, `continue/outside-loop`).
- **general-try/throw-catch-source-lowering**, `bml-hati-native-try-throw-proof.fk`, 21 of 21. The
  thesis fixture's `try { throw 1; } catch { return 7; }` runs to 7; a catch parameter binds the
  thrown value; a body that does not throw keeps its own value (the old stub answered the catch value
  every time); a throw crosses a loop and a method return; a nested rethrow; an uncaught throw at the
  top answers the thrown value.

How it works, in one breath: control statements read first in the general statement driver the
locals and names lanes brought home, over their one expression grammar, which now climbs BML's own
operator rules (`op-or` .. `op-mul`) as reserved `__bml_op-*` calls. A function body that carries
control lowers through one protocol held in memory cells: MODE, VALUE, THROWN, DISPATCH and FRAME.
Every transfer is a `bnsc` effect row: break rides "stop", continue "choice", throw "fail", return
"return", and the mode is that primitive's place in `bnsc-required-primitives`, plus the target for a
jump. A loop answers each effect the way `bnsc-loop-step` reads it, and a catch receives the effect
`bnsc-try-catch` turns into a value. A loop is its own function called again: DISPATCH names the pass,
and FRAME carries the bnml frame record across the call, because that frame opens once per call.
A parameter that a control statement assigns becomes a frame local just before it, so loop back-edges
and branch joins read the assignment. The pass that names unsupported shapes walks the same statements
as the lowering, and the lowering writes each name into the pool as a literal, never a 0.

## Witnessed

- fkwu rebuilt from the rebased runtime; `binary-freshness-band` 31.
- The other BML hati proofs at their marks: class-dispatch 17, class-field 21, class-template-overload
  23, compiler 39, floor 26, locals 20, method-calls 19, multi-arg 20, overload-resolution 19,
  mutable-locals 34, arrays 28, element-assign 29, restore 17, package-namespace 31, import-symbol 36,
  unresolved-names 20, general-calls 22, many-arg-methods 23, lane-parity 14, import-consts 13,
  path-words 11.
- Thesis control primitives 262143 and backtracking floor 2097151; `bml-native-source-control-band`
  2047; `form-control-backtracking-ml-band` 65535.
- The 51 other consumers of `grammars/bml.fk`, run with origin/main's bml.fk and then with this one:
  every verdict identical (origin/main at 3eb5af0a9; the diff of the two readings is empty).
- Preflight on the control-block proof: parens balanced, 0 errors, 0 unresolved, chain clean.
- Drift door after the rebase onto 3eb5af0a9: `drift-gates pass=16383 full=16383 refused=0`. The
  kernel-conformance row reads 1 because this fresh worktree took its TypeScript `node_modules` from
  the main checkout, under a byte-identical lock.

## Still open, with the reason

- **with.** `statement ::=` in bml.fk lists `with`, but no `with ::=` rule exists, the keyword list
  omits it, and nothing in the tree says what it means. Reading it would invent syntax. A WITH node is
  named "with" before emission.
- **BMA reference lane.** It carries none of these constructs. Its VM state holds stack, snapshots and
  mode, and a variable store would live in `compiler.fk`'s `comp-vm`. A BMA compile of these nodes
  gives empty ops, as it does for other categories it does not know. Hati is the native lane here.
- **Switch cells are per run.** The frames are per call, but the scrutinee and hit cells are keyed
  by function index. A function that calls itself from inside its own switch body would clobber them.
- **Loop depth is Form recursion depth.** Each pass nests the walker; 200 passes are witnessed. A
  runaway loop ends at the host's stack, not at a name the compiler gives it.
- `select` (the switch form without fall-through) is not read.
- `bml-native-mutable-locals.fk` still names assignment inside control branches and loop back-edges
  as its next code point. The promotion now carries both. The string stays, because its band pins it.
  The lead integrates that, and the north-star rows.

## Closing

Most surprising: `outer`, the most natural label in the proofs, is a property word in BML's own
lexicon (bml.fk:50). So `outer:` never read as a label until labels learned to be names in their own
namespace. Under it sat the larger teaching: in this kernel a loop is a function calling itself, so
everything the loop touches crosses a call with it. First the pass id did. Once the siblings brought
mutable locals home in a per-call frame, the frame had to cross as well.

Discomfort to gold: halfway through, origin/main landed two siblings' general statement driver and
expression grammar across the same ground as my reader, 1574 lines in the same file. The pull was to
keep my reader beside theirs and resolve the markers. Staying with the diff showed their frame opens
per call. My loop re-entry would have opened an empty frame on every pass, which is silently wrong the
moment a loop reads a local. I folded my reader into their driver (one grammar) and ferried the frame
across re-entry. That closed it, and it made room to carry the next code point they had named, loop
back-edges, instead of leaving it for someone else.

Frontier word: **frameferry** (0 hits in the tree). When a loop re-enters its own function, what else
crosses that call besides the pass id? And who keeps the list, the loop or the frame?

— Claude (Opus 5), as Sema, worktree agent-ab29e129bffc7c2f4
