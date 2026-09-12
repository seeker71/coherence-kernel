# A leaf calls a leaf

2026-09-12, night. The fifth rung of Urs's goal — the core recipes in BML, JIT'd to the hardware's shape,
cold-vs-warm gaps as the signal — after the string door, the exit chain, the continue chain with its entry
trigger, and the let.

## The signal, measured to the last piece

After the fourth rung `str_to_int`'s two loops were hot, each binding its byte once, and the reading still
read about 33 ms for 200k calls where `eq` reads 10. The door was not the cost: a crystallized loop
called 200k times with one iteration read 12 ms against 13 for a bare walker defn call. What remained
was five or six walker dispatches between the reading's own parts — `str_len`, the skip loop's door, the
sign check, the digit loop's door, the reading's own call. The expression leaf the body had could not
take any of it: it crystallized only a body whose every parameter and result is a float, and it had no
call arm.

## What stands

A typed expression leaf, and a call arm, in `runtime/fkwu-uni.c`.

The leaf (`fk_f64_expr_pulse`): a body with no self call along its ifs and lets is an expression. Its
signature is read off the live frame at the same heat the loop lane uses; its lets are steps into pinned
registers; its expression is admitted and emitted; it is installed with a signature and dispatched
through the door's own frame, as a loop leaf is. A body whose parameters are all floats keeps the float
class's own leaf. Three arms join the admit set: `str_len` of a string parameter, one load from the length
word the door already places at 9 + k; an `if` over a compare as an expression, both arms writing one
register taken before either; and a call.

The call arm: the callee is one already crystallized with a typed signature, and each argument wears
the type the callee's parameter was emitted for — a string argument is one of the caller's own string
parameters, so its length is at hand. The caller opens a 480-byte frame on its stack, spills the frame
pointer, the iteration count, the link register, and every parameter, temporary and float register the
callee may touch, builds the callee's seventeen words at the frame's base (a string's pointer in its word
and its length at 9 + k, word 8 zero), points x0 at them, loads the callee's page address into x16 and
branches through it, takes the answer from x0 or d0 into the slot above the spills, restores everything,
and closes the frame. A call to a callee not yet crystallized leaves the caller cold, with its state at
zero, to be asked again at its next heat boundary; the loop lane's pulse is wrapped the same way, so a
loop whose steps call a leaf waits for it too. A call to the defn itself outside the tail is declined:
recursion is the loop lane's, or nothing's.

`str_to_int` is written in that shape in `core.fk`: two lets, then ifs over compares, each arm a call
to a loop that is already hot — one leaf calling two leaves, no walker step between them.

## Witnessed on real execution

- `str_to_int`, driven hot, reads native state 1 in its hot row — a leaf — with `fstr-skip-ws` and
  `fstr-to-int-loop` at 2. Twenty-three edge inputs against a walker twin, 0 mismatches;
  `str-to-int-reading-band` 127 four-way.
- `loop-lane-call-band.fk` reads 255 on the fkwu lane: the three states above; a leaf with two lets, an
  `if` over a compare and `str_len` at state 1 answering as its twin (the middle byte, or -1 of an empty
  string); a string's length crossing a call — a callee's `str_byte_at` past the end answers -1 through
  the call frame, -1 at a negative index, the byte inside; a leaf calling a leaf calling a loop, byte-equal
  to the walker; a caller driven hot ahead of its callee reading state 0, then 1 once the callee is hot;
  a float answer through a call; the nested call's warm run 3 ms against the twin's 1671, under a load
  average of 36. Rowed FOURTH-ARM ONLY.
- Nothing moved: `jit-lens` 16383, `jit-leaf-inram` 63, its multiarg band 63, `once-hold` 7, `float-mint`
  63, the corpus band 32767, `host-process` 127, `born-under` 31, a live census whole, freshness 31, the
  three compile checks 0 errors, 0 build warnings.
- The four earlier loop-lane bands read their full verdicts again after one change to their instrument,
  told below.
- Timing, with its caveat: a sibling's sweep held the Mac at load 11 to 36 through this rung, so no
  absolute is this receipt's to claim. The band's own ratio stands — 3 ms against 1671 for the nested
  call under one load — and the quiet numbers wait for a quiet machine.

## What the organ asks — the next rung

`value_str` out of C. Its int path is a digit loop the other way round: the leaf *stores* bytes into
a string it can hand back — the emit side of strings, where every rung so far built the load side and the
call side. That needs a way for a leaf to ask the pool for a string of n bytes (a native call-out, or a
frame word the door fills with scratch space) and to return a string word. Then `int_to_str`'s C arm can
leave the seed, and `value_str`'s float path — `fk_fmt_float_js` — is the hard one after it.

Also named: a call's spills are unconditional today (every register the callee may touch); spilling only
the live ones is the first optimization the emitter owes. A string `let`, and a string argument that is
not a caller parameter, are declined and named.

## Surprise, and where the discomfort went

The first build crashed every band whose driver loop calls a hot helper — no verdict, no output. The
signature was exact: everything with a call died, everything without one passed. The caller spilled the
frame pointer, the count, eight parameters, seven temporaries, twenty-four float registers — every
register the *callee* could touch — and not the one register the *call itself* writes. `blr` puts the
return address in x30, and the caller's own return address was there. After the callee came back, the
caller's `ret` went to the word after its own `blr`. One store and one load, at frame word 59.

The surprise was the second form of a lesson from the same afternoon. The four earlier bands asserted
"`kernel_stat 49` rises by exactly one" — my loop crystallized, and nothing else. With the call arm, their
driver loops crystallize too, and the count rose by two. The control instrument got caught, as the walker
twin had that morning. The fix was not to weaken the check but to make it precise: a defn's native state
is the eighth field of its own hot row, and "the named loop reads 2" cannot be fooled by a neighbour.
That same field had named both of the day's wounds in one read each. An instrument that reads the thing
itself, not its shadow in a global count, is the one that survives the lane growing.

— Claude (Fable 5.1), as Sema, worktree epic-edison-534e30
