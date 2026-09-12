# Every string loop has more than one way out

2026-09-12, afternoon. The second rung of Urs's goal (`2026-09-12-a-string-crosses-into-the-loop-lane.md`
is the first): the core recipes in BML, not the C kernel, JIT'd to the hardware's shape, with cold-vs-warm
gaps as the signal.

## The signal the first rung left standing

After the string door, `str_to_int` still read 194 ms for 200k calls — unchanged — while its siblings in
C read 10 to 20. Its digit loop, `fstr-to-int-loop`, had every op the lane now took, and did not
crystallize, because its *shape* was not the lane's: `(if (ge i len) acc (if (digit? b) self acc))` — a
nested `if`, two exits, and a call to `fstr-digit-cp?` in the condition. The lane took one shape only,
`(if <compare> <exit> <self call>)`, one exit. Every real string loop has more than one way out: the end
of the text, a byte under '0', a byte over '9'.

## What stands

Two changes, each in the goal's grain.

The lane learns if-chains (`runtime/fkwu-uni.c`): `fk_f64_loop_pulse` walks the body as a chain. At each
`if` the condition is a compare (`le` 5, `eq` 102, `lt` 103), one branch is an exit expression, and the
other is the self tail call or the next `if`; the branches may be swapped at any step; up to four steps.
Each step's compare and exit are admitted, every exit answering the one return type. `fk_f64_loop_pass`
emits one compare and one conditional branch per step, then the parallel move and the iteration count;
two unrolled passes as before. After the loop, one exit block per step — its expression, the count into
the ninth frame word, the answer into x0 or d0, and a return — and every branch is patched to its own
block. A one-step chain is exactly the old shape, and `jit-lens` reads 16383 through the new code.

The recipe is written in the lane's shape (`form/form-stdlib/core.fk`): `fstr-to-int-loop` is now
`(if (le len i) acc (if (lt b 48) acc (if (lt 57 b) acc self)))` — three exits, one continuation, the
same meaning as `fstr-digit-cp?` with only the shape moved. Authoring the recipe well is the BML-first
path the goal names; inlining a small non-self defn at the lane is a later class. `ge` would have served
too (a rewrite to `le` with its children swapped), but `gt` rewrites into an `if`, not a compare, so the
recipe uses `le` and `lt` alone. `fstr-to-int-loop`'s only caller is `str_to_int`.

## Witnessed on real execution

- `str_to_int`, 200k six-digit calls: **194 ms → 52 ms**, `kernel_stat 49` 0 → 1. Three and a half
  times, not a hundred, and the reason is the next signal: on six digits the crystallized loop runs six
  iterations, and the *wrapper* — `str_len`, `fstr-skip-ws`, the sign check, the door's entry — is now the
  cost, all of it still walker dispatch.
- `str_to_int` on a forty-digit input, 20k calls: **97 ms on the walker twin → 6 ms hot**, sixteen
  times, answers equal. The ratio grows with the loop, as it should when the fixed cost is the wrapper.
- Parity over twenty edge inputs, hot against a walker-shaped twin (a call in its condition, which the
  lane declines): `""`, `0`, `-42`, `"  42"`, `"\t-7x"`, `123abc`, `abc`, `-`, `--5`, `+5`, `00012`,
  the int64 bounds, `12 34`, `"\n99\n"`, `0x10`, `1e3`, `4.5` — 0 mismatches.
- `str-to-int-reading-band` reads 127 four-way through `validate.sh`: the reshaped recipe reads as the
  siblings' natives do.
- `loop-lane-if-chain-band.fk` reads 255 on the fkwu lane: a three-exit chain crystallizes; each exit
  path — the end of the text, a byte under '0', a byte over '9' — answers as the twin; the sweep agrees;
  the chain with every step's branches swapped crystallizes too; a chain whose condition is a call stays
  the walker's and still answers right; the warm run is under the twin's. Rowed FOURTH-ARM ONLY.
- Nothing moved: `jit-lens` 16383, `jit-leaf-inram` 63, its multiarg band 63, `once-hold` 7,
  `float-mint` 63, `loop-lane-string-door-band` 127 (see below), freshness 31, the three compile checks
  0 errors, the build 0 warnings.

## What the organ asks — the next rungs

- **The wrapper.** `str_to_int`'s own body is a `do` of `let`s and an `if`, not a loop; `fstr-skip-ws`
  is a loop with a non-self call in its condition. The classes named before this goal — `let` (tag 109)
  into a register, a non-loop expression leaf over string ops, and the inlining of a small non-self defn
  — are what take the 52 ms down. Also the three `str_byte_at s i` loads per iteration, which one `let`
  would bind once.
- **`value_str` out of C into BML.** Its int path is a digit loop the other way round: it needs a byte
  *store* into a string the leaf can hand back. The string door built the load side; this is the emit side.
- The emitter's own home, and per-platform emission, as the first receipt names.

## Surprise, and where the discomfort went

The string-door band read 63 after this rung, not 127, and the bit that dropped was the one that says the
warm run beats the walker's. Its walker twin was a nested `if` — a shape the lane declined that morning
and took by the afternoon. The control had been caught by the treatment it was built to escape: both runs
were hot, and equal. The lane had grown past the band's assumption in the same session that wrote the
assumption down.

The pull was toward the chain code, the newest thing, as the thing that broke. The band's own line showed
the two timings side by side, equal, and `kernel_stat 49` two higher than expected: the twin had
crystallized. The lane was right on both counts. The fix was the twin — a call in its condition, which no
compare arm reads — and a comment saying why, so the next lane that grows finds the reason where it
looks. Reading the evidence line before the code, for the third time in two days, turned a suspected
regression into the proof that the rung had worked.

— Claude (Fable 5.1), as Sema, worktree epic-edison-534e30
