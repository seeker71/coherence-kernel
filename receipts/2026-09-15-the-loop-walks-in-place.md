# The loop walks in place: select, with's receiver, a switch per call, the value it stood on

2026-09-15, M4 Max, Hati Suci. The control-flow landing (7d4c6ccf2, 19944ae23) left five items
open: a loop re-entered its own function on every pass, select was not read, with was named and
not lowered, a switch kept its value in memory cells every call shared, and the control lowering
had not yet carried the standing value the state landings gave it. They land on the native path
in 785fe1326.

## Carried

- **A loop is one LOOP row the walker iterates in place.** `fk-loop` in
  `form/form-stdlib/hati-os-kernel.fk` is tag 257, the first number above the byte tag space
  (`native-op-manifest.fk`: nothing left to spend there), and only the recipe walker reads it:
  `fkm-loop` asks the test and walks the step in one self-tail-recursive arm, so a pass costs no
  depth. `form/form-stdlib/grammars/bml.fk` lowers while, for and loop to
  `init; LOOP (cond) (body; step); exit`, with the modes `bnsc-loop-step` reads, as before. The
  dispatch cell, the frame ferry and the function index a loop used to re-enter its own function
  are gone, and so is the classes member env's function-index binding.
- **select** reads from the thesis (a `jmp breakLabel` after each matched statement): only the
  clause that matched runs, over value lists and `lo .. hi` ranges. switch keeps C fall-through.
- **Per-call switch state.** switch and select read as statements over two bnml locals the switch
  node declares, the stored value and the chosen clause, so the frame each call opens owns them.
- **with's implicit receiver**, recovered from `WithWith ::= "with" "(" Expression ")" Statement;`:
  the base is read once into a bnml local, `.self` reaches it, `.x` reads its field, and `.m(...)`
  is `__bml_with.m(...)` for the classes landing's receiver dispatch. A leading dot outside any
  with is named before a table exists (`with/no-base`).
- **Standing values** on the rewritten lowering: a body that keeps one writes it at each plain
  statement, a statement that may fail stops the ones after it, and a failure that stopped the body
  answers the value it stood on with MODE cleared. These came from 5c34e25fa and c56bdf7a5, which
  had changed the functions I was replacing; I carried each change into the new ones.
- Fixtures: `form/form-stdlib/tests/fixtures/bml-control-flow.bml` (`Answer: 1460`) and
  `bml-control-loop.bml` (`Answer: 1000000`). The registered while, for and control-block proofs
  read the LOOP row and the with statement where they read the retired cells.

## What came out with BMA, and why

I had also built a BMA control lane: if, while, try and switch as ctl-* ops over the VM's own
modes, a callee carrying control run on a floor frame of its own, BMA promotions, a standing value
on the stack, a loop-budget reading. It ran: eight functions answered the same on both lanes, the
control-flow fixture 1460 on both, a million passes on BMA in 11 s with one loop-budget reading.
Urs at 15:31: BMA was the thesis's assembly; our path is Form kernel primitives and the JIT, and
the two are not mixed. All of it came out. The three wrappers (`bml-compile-node-in`,
`bma-exec-op`, `bml-bma-op-refusals`) are back to their own names, and the thesis's own BMA proofs
read their full marks (262143, 2097151). The control-effect and with-receiver readings I had added
came out too: the compiler already speaks where the lowering names what it cannot lower and where
it withholds a table, and a second reading only doubled that voice.

## Witnessed

On fkwu, native path:
- `bml-control-loop.bml` answers 1000000 in 95 s, peak 57 MB, with the LOOP row in its table.
  `int i = 0; while (1) { i = i + 1; }` ran until the watchdog stopped it at 40 s, peak 56 MB.
- `G(2) * 10 + G(1)`, G recursing from inside its own switch, answers 12. select over a range 3,
  over a value list 1; switch fall-through 11; `with (6 * 7) { return .self; }` 42; with over a
  class value (`.size`, `.Area()` through receiver dispatch, `with (this)`) 3096; a try across a
  call 321; `bml-control-standing.bml` 7.
- Every BML native proof at its manifest mark (control-block 24, while 19, for 22, switch 20,
  break-continue 25 and try-throw 21 among the 28), the thesis control proofs 262143 and 2097151,
  the carriers 2047, 65535 and 1023. Every bml.fk consumer reads the verdict it reads on origin.
  Preflight clean on the edited proofs; `./fkwu --check` compiles bml.fk clean.
- Four-way through `form/validate.sh`: the 33 rows that prelude grammars/bml.fk each read
  `1 band(s) four-way`, `1 ok, 0 divergent`. Drift door: pass 16383 of full 16383, refused 0.
- `observe/bml-native-run.bml` on main at 785fe1326: 70 ok of 70, control-flow 1460, control-loop
  1000000 and control-standing 7 among them.

## Still open, with the reason

- A loop past a budget has no reading on the native path. The LOOP row is walked inside the
  kernel's recipe walker, which holds no organ surface; the reading that did this lived on BMA and
  left with it.

## Closing

Most surprising: the rebase knew what I had to carry. My new control section kept a few comment
lines word for word from the one it replaced; git anchored on them, and the only conflicts left
were the old functions other hands had changed that day: the standing value, the stop guard, the
edge check, the loop's failure guard. The conflict list was the list of meanings to bring across.

Discomfort to gold: the first live probe held 5.5 GB at 27% CPU after ten minutes. I read its
resident size before touching anything, stopped that one pid, and bisected under a watchdog of
resident and time caps. Load, a Hati while and a BMA while each answered in two seconds; the
growth was BMA inlining the recursive G without end, because the lookup meant to send a callee
carrying control to its own frame answered "none". Printed one at a time, every piece of that
lookup answered true, and the whole answered empty: `nil?` gave 1 for the METHOD node it was
handed, so "found a callee" read as "found nothing". Carrying the found node inside a list healed
it. That lane has left with BMA; the watchdog runner, and the habit of printing each piece alone,
stayed and carried every probe after.

Frontier word: **atomnil** (0 hits in the tree). When a predicate asks whether nothing is here,
what should it say of a value that is not a list at all? My answer: not "empty". A found node is a
thing. The body names absence for itself, `nothing`, and an empty list for an empty collection, so
a result that may be missing should travel in a shape the question can read; a predicate that reads
an atom as empty turns a found thing into a lost one.

— Claude (Opus 5), as Sema, worktree agent-ab29e129bffc7c2f4
