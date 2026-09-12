# A string is a value in a leaf

2026-09-12, afternoon. The seventh rung of Urs's goal — the core recipes in BML, JIT'd to the hardware's
shape, cold-vs-warm gaps as the signal — after the emit side of strings. After rung six a loop could
build a string and hand it back, but a leaf could not hold a string literal, could not answer a string an
`if` chose, could not call a leaf that builds one; and a leaf whose callee sat on a branch never taken
stayed cold for the life of the process. `int_to_str`'s cost was its walker wrapper.

## What stands

In `runtime/fkwu-uni.c`:

- **Every string value in a leaf rides a slot** — its pointer in x(10+s), its length at the slot's frame
  word. A parameter's slot the door fills; a **literal's** the leaf's own prologue fills, reading the pool's
  base when the leaf runs (the pool may have moved) and adding the offset a live string keeps (the melt
  holds the AST's literals live, and live strings never move); a **call's** or an **if's** string answer
  rides a hidden slot, taken from slot 5 downward past every parameter and let, that each arm or the call
  moves into. `str_len` reads any slot's length, after the expression that fills it.
- **A callee that builds or answers a string works in its caller's scratch.** The caller hands the callee
  its scratch words and a zero for the answer's length; copies a seeded accumulator's bytes to the
  scratch's end or base first (a seed that does not fit closes the frame and leaves for the overflow
  block); takes the answer back — pointer from the answer word, length from the callee's word — and
  narrows the scratch past the bytes when they lie inside it, from whichever end they lie nearer. A loop
  renews its scratch at each pass, since every string a callee builds within a pass is consumed within
  it. The door checks the overflow word for every leaf that carries scratch, not only for one that
  answers a string. The call frame widened to 544 bytes so the callee's twenty-four words fit before the
  spills.
- **`nothing?` of a parameter folds away** — as an `if`'s condition in an expression, or as a step in a
  loop's chain — because the door admits numbers and strings only.
- **The cold callee is warmed.** A call to a callee still at state 0 records the callee and the types its
  arguments carry; the pulse wrapper crystallizes those callees for those types (a pulse with no frame
  reads the forced types instead) and asks the caller again at once, four levels deep at most.
- The typed lanes keep to six slots, x10..x15: x16 and x17 are their scratch registers.

`core.fk` writes `fstr-int-str` — `(if (lt n 0) (fstr-neg-digits n "") (fstr-digits n ""))` — one leaf:
two literals, two builder calls, one string `if`. `int_to_str` calls it behind `nothing?` and the float
test.

## Witnessed on real execution

- `loop-lane-string-value-band.fk` 255 on the fkwu lane: a leaf answering a literal and an `if` between
  a literal and a parameter, at state 1, as its twins; a leaf asking `nothing?` first, state 1, and
  nothing still the walker's answer; `fstr-int-str` at state 1 agreeing with the C writer's walker twin
  over 28 edges, both tagged limits among them; a seeded builder copying its seed, and a 600-byte seed
  answered through the walker byte-equal; a loop calling a builder every pass, state 2, equal to its
  twin over 50,000 passes; `str_len` of a literal and of an answer inside a leaf; a leaf whose callee is
  on a branch never taken at state 1 with that callee in no hot row; the warm run under the twin's.
  Rowed FOURTH-ARM ONLY.
- **Quiet timing** (load 3, 200k six-digit words): `fstr-int-str` as one leaf **5 ms**, where C's
  `value_str` reads 20 and the walker twin 144 — the BML leaf renders integers four times faster than
  the C it replaced. `int_to_str` itself reads 31: its wrapper still walks, because its float branch
  calls `value_str`, a native the lane cannot take — the float writer in BML is the next rung.
- Nothing moved among the bands the field still lets run: every loop-lane band (string-build 255,
  string-door 2047, if-chain, continue-chain, let, call 255), jit-leaf-inram 63 and its multiarg 63,
  freshness 31, the corpus band 32767 with row 1496 (written as 1494; renumbered at the reunion), op-manifest 194 aligned, the three compile checks 0
  errors, 0 build warnings.
- Pending, honestly: the shared field's node columns filled during this rung (2^26 cells), so every BML
  lowering child fails until `observe/field-reset-run.fk` runs with no kernel alive — two kernels not
  mine are alive. value-str, str-to-int-reading, host-process, born-under, jit-lens, the census bands,
  the form-cli regen and its Go carrier test wait on that reset; they read whole an hour earlier on the
  rung-six binary, and the changes since touch the lanes only.

## What the organ asks — the next rung

`value_str`'s float path in BML: the shortest round-trip digits (`fk_fmt_float_js`) as a loop in the
lane's shape, so `int_to_str` becomes one leaf and `value_str`'s int and float paths leave the seed. Also
named: a string argument to a self call that is not the parameter itself (a literal or an answer moved
into the slot, both pointer and length); `str_byte_at` over any slot; a second builder in one leaf
(a partitioned scratch); the typed lanes' seventh and eighth parameters.

## Surprise, and where the discomfort went

Every probe that answered an int passed; every probe that answered a string a callee had built died
with a signal, and the difference was not in the string path at all. Hidden slots are taken from the top
of the frame down, and slot 6 rides x16 — the register the emitter has used as scratch since the fifth
rung, for the callee's address, for a limit, for a literal's offset. No leaf had ever had a seventh
parameter, so the collision had never been reached; the first hidden slot reached it. The discomfort was
reading a clean design crash for a reason the design could not show. The gold: a register is one
thing. The typed lanes now keep to six slots and x16 and x17 are scratch, said once, where the slots are
allotted.

The second teaching was quieter. With the leaf built, `fstr-int-str` read state 0 whenever a driver
passed only positive numbers: its call to the negative loop was not ready, and would never be, because
no negative number ever came. A leaf was held cold by a branch never taken. The call knows the types its
arguments wear; that is enough to crystallize the callee before it is ever called — impliedheat, row
1496 — and the leaf that had waited on nothing crystallized at once.

— Claude (Fable 5.1), as Sema, worktree epic-edison-534e30
