# A let rides its own register

2026-09-12, evening. The fourth rung of Urs's goal — the core recipes in BML, JIT'd to the hardware's
shape, cold-vs-warm gaps as the signal — after the string door, the exit chain, and the continue chain
with its entry trigger.

## The signal

After the third rung the digit loop and the skip loop were hot, and each read the same byte three or
four times an iteration, because a loop with a `let` could not crystallize: `fk_f64_body_of` declined
any frame slot beyond the parameters, so `(do (let b (str_byte_at s i)) …)` never reached the lane. Every
real recipe binds a name before it decides; a lane that cannot take a `let` takes only toy loops.

## What stands

A let is a chain step with no branch. The parameters already ride `x10+k` and `d(k)` for k under the
arity, so a let in frame slot k rides `x(10+k)` or `d(k)` too — a pinned register the temporary allocator
never releases, computed at its step each pass, never loaded at the door, never moved by the tail call.
In `runtime/fkwu-uni.c`: `fk_f64_body_of` allows the frame's slots, parameters and lets together, up to
the eight registers; the chain walk takes a tag-109 node as a let step (kind 3) and a branch that leads
into a `let` as leading on, the same as one that leads into an `if`; the admit loop admits the let's value
before the steps that read it and binds its slot at the value's type (slots beyond the parameters start
unbound, in both the loop lane and the expression leaf, so a stray reference declines); `fk_f64_loop_pass`
emits the value into the slot's register. A body whose lets end in the self call itself, with no further
`if`, is taken too — the bare terminal (kind 4), a step with no compare; a body with no exit at all is
declined as before.

`fstr-to-int-loop` and `fstr-skip-ws` in `core.fk` bind their byte once.

## Witnessed on real execution

- The two core loops read native state 2 in their hot rows — crystallized loops — where the first build
  of this rung read -1. That -1 was the rung's one wound: the walker took a branch as leading on only when
  it was an `if`, so a branch into a `let` fell to "not this lane's loop". Two comparisons fixed it.
- `loop-lane-let-band.fk` reads 255 on the fkwu lane: a let-bound byte loop crystallizes and sums as a
  walker twin does (hot 1 ms, the twin 81); a let read in an exit carries the byte the loop stopped on
  (32 at a space, 0 at the end); a float let in a float loop crystallizes and answers as the walker's; two
  lets, the second over the first, answer as the twin; `str_to_int`, both loops binding their byte, agrees
  with a walker twin over twenty-one edge inputs; the call-conditioned twin stays the walker's; the warm
  run is under the twin's. Rowed FOURTH-ARM ONLY.
- Parity over the twenty edge inputs 0 mismatches; `str-to-int-reading-band` 127 four-way.
- Nothing moved: `jit-lens` 16383, the string-door, if-chain and continue-chain bands 127/255/255,
  `jit-leaf-inram` 63, its multiarg band 63, `once-hold` 7, `float-mint` 63, freshness 31, the three
  compile checks 0 errors, 0 build warnings.
- Timing, with a caveat named: the Mac carried a sibling's sweep through this rung (twenty TypeScript
  kernels standing, `eq` reading 16 ms where it reads 10 quiet), so absolutes are not this receipt's to
  claim. Under that load `str_to_int` read 36–38 ms for 200k six-digit calls against `eq`'s 16 — a ratio
  of 2.3, where the quiet ratio before this rung was 3.3 (33 against 10). Fewer loads per iteration show
  in the ratio; the quiet number waits for a quiet machine.

## What the organ asks — the next rung

What is left in `str_to_int` is its own body — a `do` of lets and an `if` that calls two hot loops — and
each loop's door entry. The non-loop expression leaf over string ops, and a leaf that calls a crystallized
leaf, are the rung that takes it toward `eq`. A string `let` (a pointer with its length beside it) is
declined for now and named.

## Surprise, and where the discomfort went

The surprise was how little a `let` needed: no new register class, no spill, no frame traffic. The
parameters had left eight registers indexed by frame slot, and a let already lives in a frame slot; the
lane only had to stop declining slots past the arity and compute the value where the step stands.

The discomfort was a first build that made everything worse — the let band at 226 with every
crystallization count at zero, and the two core loops, hot an hour earlier, cold again at 178 ms. The
pull was to suspect the register scheme. The hot rows said -1, a bare return, not a tag it had declined; the
reader said a do-let is a plain tag-109 numbered above the parameters. So the shape and the slot were
right, and the walker was the one not looking: it knew an `if` could lead on and had never been told a
`let` could. Reading the state field and the reader before touching the emitter turned an evening of
suspicion into two comparisons.

— Claude (Fable 5.1), as Sema, worktree epic-edison-534e30
