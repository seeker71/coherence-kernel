# A failure travels to the top: the program ends where it stood

2026-09-15, M4 Max, Hati Suci. After a2c456349 the coordinator named the two state items I had left
open: a method whose choose fails, called outside any choose, kept running on Hati while BMA ended
the program; and a control body stopped by a failed choose answered 0, not the value it stood on.

## Carried

- **A failure no choose catches ends the program on both lanes.** A unit that holds a `fail`
  anywhere carries a `__bml_fallible` mark in its Hati environments. In such a unit a statement that
  calls a method or fails reads FAILED before the next one runs, its body keeps a frame and the value
  it stands on, and a statement's value becomes the standing value only if it ran to the end. The
  failure travels up every caller to the top, and the program answers the value it stood on: BMA's
  stack top at that statement's edge. A bare `fail;` outside any choose used to be withheld; it now
  ends the program the same way, and the floor proof's clause reads it admitted (26 kept). A unit
  with no fail lowers as it did.
- **A control body keeps the value it stood on.** A control function that keeps a standing value
  writes it at every plain statement, at any depth of its loops and branches, and its epilogue
  answers it when FAILED is set. Class member bodies keep it too.

Canonical sources: `form/form-stdlib/tests/fixtures/bml-choose-uncaught.bml` (both lanes answer 13: a
failure caught inside Safe's branch, then one no choose catches, ending the program on b) and
`form/form-stdlib/tests/fixtures/bml-control-standing.bml` (Hati answers 7; declared as a lane gap
until BMA carries if and while). `bml-branch-assign.bml` now declares its Hati answer, 51, so
`observe/bml-native-run.bml` reads it. The mutable-locals carrier's next code point moves on (band
1023).

## Witnessed

Live on fkwu: choose-uncaught 13 on both lanes, no split; a bare fail in a method called outside any
choose 3 on both, and inside a branch 7 on both; a fail-free unit 3 on both; control-standing 7 on
Hati (BMA 3, the answer of a lane with no if or while); choose-state 12, choose-fields 8,
branch-assign 51 as before; floor proof 26. Every bml.fk consumer on fkwu reads the verdict it read
before this round, organ lines included. Drift door: pass 16383 of full 16383, refused 0. Four-way
through `form/validate.sh` on the tree rebased onto the live-run door: the thirty-three rows of
`form/fourth-arm-bands.txt` that prelude grammars/bml.fk or the mutable-locals carrier each read
`1 band(s) four-way`, `1 ok, 0 divergent`. `observe/bml-native-run.bml` over the five state fixtures:
choose-uncaught and choose-state ok, choose-fields, control-standing and branch-assign lane gaps at
their declared Hati answers, none differing.

## Still open, with the reason

- The control-standing pair on BMA waits for the control lane's BMA if and while (agent
  ab29e129bffc7c2f4). When it lands on origin I run the pair on both lanes.
- A failure in the middle of an expression: BMA answers the operand it had already pushed
  (`return x + Bump(b);` reads x), Hati answers the value the statement list stood on. The lanes
  agree at a statement's edge, which is where every fixture fails.

## Closing

Most surprising: BMA already held the rule. Its stack top at a failure is the value the program
stood on, exactly the standing value the save snapshot introduced, as long as the failure lands at a
statement's edge.

Discomfort to gold: guarding every call in every body was the simple rule, and it would have
reshaped the table of every proof in the tree. Tying it to a unit that holds a fail kept every
fail-free table as it was.

Frontier word: **midfail** (0 hits in the tree). When a failure lands inside an expression, which
value did the program stand on: the last statement's, or the operand one lane had already pushed?

— Claude (Opus 5), as Sema, worktree agent-a69badcddce1f9c16
