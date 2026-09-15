# The value it stood on: a program a fail ends answers its last completed statement

2026-09-15, M4 Max, Hati Suci. After 5c34e25fa one split stayed: when a failure no choose held
stopped the program in the middle of an expression, BMA answered the operand it had already pushed
(`return x + Bump(b);` read x) and the native lane answered the value the program stood on. Urs at
15:31: BMA was the thesis's assembly; our path is Form kernel primitives and the JIT, and the two are
not mixed. BMA is not required. So the split closes as a question of the language's meaning, on the
native path.

## What the thesis says

The backtracking model language text (Coherence-Network
`docs/field/urs/artifacts/master-thesis-2000/backtracking-model-languages.txt`) defines fail at
line 634: it forces the virtual machine into its fail state, which backtracks until another path is
found, typically a choice. Line 932 lowers a choice to one fail-jump target per alternative. The
only unhandled ending the text defines is for throw, at line 632: with no catch "the program will
terminate with the specified exception". What a program answers when a fail finds no path left,
the text does not say. The result register a return fills (624, 956) is overwritten by every if and
while test (885, 907), so its content at a failure is a machine artifact, not a meaning.

## The meaning

A program a fail ends with no choice left answers the value of its last completed statement, the
value it stood on, even when the fail lands in the middle of an expression. It is written where the
language describes its own package, `form/form-stdlib/bml/bml-lang.bml`, and at the fail lowering in
`form/form-stdlib/grammars/bml.fk`.

## Carried

- **The native lane holds it.** A fail statement answers its body's standing value (0 with no body
  frame), so a program that ends on its own `fail;` answers what it stood on. A failure that stops a
  control body clears the RETURN mode a failed return expression left behind.
- **It is heard.** The Hati run keeps its own memory and reads the FAILED cell after it ends. A run
  that ended on a fail no choose held voices one reading through `bml-compiler-health.bml`, naming
  the unit, the lane and the value it stood on; a run whose every fail was held is silent.
  `bml-hati-run-unit-value-at` lets a door name the unit.
- **Fixture:** `form/form-stdlib/tests/fixtures/bml-choose-midfail.bml`, the `return x + Bump(b);`
  shape, `Answer: 6`.

On the way I first had BMA keep a standing value of its own, a floor-frame `__top` written at each
top-level statement. Two thesis proofs that pin BMA's op stream told me the frame opened too
broadly, and then the direction took BMA out of the judgment. I discarded all of it before landing;
BMA is as it was.

## Witnessed

On fkwu: `return x + Bump(b);` 6, `int y = 1 + Fails();` 4 (x's value, not the 1 already evaluated),
a lone `fail;` 0, `int x = 5; fail;` 5, each with one organ line naming its unit; a held fail and a
fail-free unit silent. Every bml.fk consumer on fkwu reads the verdict it read before this round,
the two thesis proofs that pin BMA's op stream at full marks (control primitives 262143, exit
1073741840). Drift door: pass 16383 of full 16383, refused 0. Four-way through `form/validate.sh`:
the thirty-three rows of `form/fourth-arm-bands.txt` that prelude grammars/bml.fk or the
mutable-locals carrier each read `1 band(s) four-way`, `1 ok, 0 divergent`. `observe/bml-native-run.bml`,
the native-only door, over every fixture that declares an answer: 68 ok of 68, midfail 6 among
them. It voices exactly four runs, one for each source that ends on a fail no choose holds
(choose-midfail standing on 6, choose-state 12, choose-uncaught 13, control-standing 7), each
naming its path; every other run is silent. The door passes the path through
`bml-run-unit-value-at`.

## Closing

Most surprising: the split was never in the language. It came from reading BMA as a judge; the
thesis names a terminating answer only for throw, and for fail says where the machine goes next,
never what a program answers when nothing is left.

Discomfort to gold: I had built BMA's standing value, frame rule and all, and it passed. Dropping it
was the right cut: the meaning lives in the language's own description and on the native path, and
the thesis proofs keep their full marks without a line of BMA changed.

Frontier word: **lastwhole** (0 hits in the tree). When a failure has no choice left, is the last
whole statement's value an answer, or only the place the program stopped?

— Claude (Opus 5), as Sema, worktree agent-a69badcddce1f9c16
