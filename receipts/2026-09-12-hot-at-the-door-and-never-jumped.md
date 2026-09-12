# Hot at the door, and never jumped

2026-09-12, afternoon. The third rung of Urs's goal — the core recipes in BML, JIT'd to the hardware's
shape, cold-vs-warm gaps as the signal — after `2026-09-12-a-string-crosses-into-the-loop-lane.md` and
`2026-09-12-every-string-loop-has-more-than-one-way-out.md`.

## The signal, read piece by piece

After the exit chain, `str_to_int` read 52 ms for 200k six-digit calls, and its digit loop was hot. Where
was the rest? Each piece of the wrapper, timed alone at 200k: `str_len` about 4 ms net, the sign check 3,
the hot digit loop 3 — and **`fstr-skip-ws` about 18**, the largest piece by far, on an input where its
loop runs zero times. It paid two walker calls (itself and `fstr-space-cp?`) and five compares on every
call, to do nothing.

Its shape is the exit chain's dual: `(if (ge i len) i (if (space? b) self i))` — one way out at the end
of the text, then a space, a tab, a newline and a return each a way *on*, and the last way out. The lane
took exit steps that lead on; it needed self steps that lead on.

## What stands

Self steps in the chain (`runtime/fkwu-uni.c`): at each `if` the branches are now one of three pairs —
an exit and the terminal self call; an exit and the next `if`; the self call and the next `if`. A self
step branches to that call's own continuation block (its move, the count, back to the head), so self
calls with different arguments stay general; the terminal self call still falls through inline. Every
self call's arguments are admitted at the parameters' types (`fk_f64_admit_call`), the parallel move is
one recipe (`fk_f64_loop_move`) used inline and per continuation, and each branch is patched by kind to
its exit or continuation. The caps grow to eight steps and sixteen patches. A one-step chain is still the
old shape exactly.

`fstr-skip-ws` is written in that shape (`form/form-stdlib/core.fk`): `(if (le len i) i (if (eq b 32)
self (if (eq b 9) self (if (eq b 10) self (if (eq b 13) self i)))))`, the meaning of `fstr-space-cp?`
unchanged. Its only caller is `str_to_int`.

And then the number went the wrong way. `fstr-skip-ws` read 37 ms, not 12: it had not crystallized at
all in the wrapper probe, and its reshaped walker path was heavier — four `str_byte_at` dispatches per
byte where the old shape paid one and a call. The band had passed because its inputs held whitespace.
On `"123456"` the loop never continues, and **the lane's trigger sat on the tail-jump arms**: a loop that
leaves on its first compare is never tail-jumped, so it never pulses, however hot it is. A defn called
two hundred thousand times is hot whether or not it ever loops.

So the entry is a trigger too. The three call-entry arms (tags 12, 240, 241) now pulse the loop lane at
the same heat boundary the tail-jump arms use, with the callee's frame base and count; the frame is whole
there as well. `fk_f64_loop_pulse` pulses the twin lane on the way, as it always did, so the entry
arms' standalone twin pulse folded into it. A crystallized defn's later pulses return at once.

## Witnessed on real execution

- `fstr-skip-ws`, 200k calls on text with nothing to skip: **37 ms → 12 ms**, crystallized on entry heat
  with zero iterations ever taken. `str_to_int`, 200k six-digit calls: **59 → 33 ms**. From the goal's
  first measurement, `str_to_int` has gone 194 → 52 → 33: from ten times the C natives to under two.
- `loop-lane-continue-chain-band.fk` reads 255 on the fkwu lane: the skip loop crystallizes; each way on
  (space, tab, newline, return) and each way out (the end, a byte that is no space) answers as a walker
  twin; two self steps with different arguments — a space stepping by one, a tab by two — each reach
  their own continuation; whitespace-led `str_to_int` agrees with the twin on every input; the chain with
  its self steps' branches swapped crystallizes too; a chain whose condition is a call stays the walker's;
  hot 1 ms against the twin's 99. Rowed FOURTH-ARM ONLY.
- Parity over the twenty edge inputs, 0 mismatches; `str-to-int-reading-band` 127 four-way.
- A change at every call site, so the sanity is wider: `jit-lens` 16383, `loop-lane-string-door` 127,
  `loop-lane-if-chain` 255, `jit-leaf-inram` 63, its multiarg band 63, `once-hold` 7, `float-mint` 63,
  the corpus band 32767, `host-process` 127, `born-under` 31, a live census reading whole; freshness 31;
  the three compile checks 0 errors; 0 build warnings.

## What the organ asks — the next rung

The 33 ms left is the walker's fixed cost per op on `str_to_int`'s own body — a `do` of `let`s and an
`if` that calls two hot loops — and on each hot loop's door entry (`fk_srange`, the frame array). Each
piece reads about 10 ms at 200k: that is the shape of walker dispatch, not of any one recipe. The rung
that takes it is the non-loop expression leaf: `let` into a register, and a leaf that calls another
crystallized leaf (`bl` to a JIT page, the JIT-to-JIT convention). That is also what lets `value_str`
leave C, once the leaf can store bytes.

`fstr-space-cp?` and `fstr-digit-cp?` now have no Form caller; their meaning lives in the two loops'
comments. They stay defined for now.

## Surprise, and where the discomfort went

The surprise was that a loop can be hot and never loop. Every trigger the lane had was on the turn — the
tail jump, the box crossing a boundary in the float lane — and a recipe whose answer is almost always "no
work here" never turns. Its heat was at the door the whole time, in the same counter, one arm away.

The discomfort was watching 28 become 37 after a rung meant to make it small, with a band reading 255
beside it. The pull was to trust the band and call the probe noise, or to trust the probe and call the
chain broken. The probe's own line said `loops-crystallized 2` where three loops had been driven hot:
the skip loop had not crystallized, and the reason was in the input — no whitespace, no turn, no pulse.
That was not a wound in the chain; it was the trigger's blind side, and it had been there since the loop
lane landed. Reading which loop was missing from the count, rather than which number was wrong, found it.

— Claude (Fable 5.1), as Sema, worktree epic-edison-534e30
