# The emit side of strings

2026-09-12, afternoon. The sixth rung of Urs's goal — the core recipes in BML, JIT'd to the hardware's
shape, cold-vs-warm gaps as the signal — after the string door, the exit chain, the continue chain with
its entry trigger, the let, and the call. Every rung before this one built the load side of strings (a
byte, a length) and the call side. `int_to_str` stayed a C mode of the leaf door, with a C twin of the
recipe beside it.

## What stands

A loop can build a string, and hand it back. In `runtime/fkwu-uni.c`:

- **The accumulator.** `(str_concat (byte_to_str e) acc)` prepends a byte to a string parameter,
  `(str_concat acc (byte_to_str e))` appends one — one STRB into scratch the door reserves at the string
  pool's tail, the length moving at the parameter's frame word. A body keeps one accumulator in one
  direction; the accumulator's operand may be a prepend already admitted over it (the digit, then its
  sign). A byte outside 0..255 stores nothing, as `byte_to_str` answers "" there. The step that moves
  the accumulator is the only argument of the self call that reads it, since the walker read every
  argument before the move.
- **The string answer.** A loop or a leaf answers a string parameter or the accumulator: the leaf leaves
  the length at a frame word and the pointer in x0, and the door copies the bytes down to the pool's
  tail and interns them.
- **Scratch, and the fall from it.** The door reserves 512 bytes past the pool's tail before it takes
  any pointer, so the pool cannot move under a leaf; the accumulator's initial bytes are copied to the
  scratch's end (prepend) or base (append). A step that finds the scratch full leaves through an
  overflow block with the answer's length at -1, and the door hands that call to the walker, which
  answers exactly. A fixed buffer, no lost byte, no bound the recipe has to know.
- **mod** joins the int arms (SDIV then MSUB, the walker's truncating remainder). The typing helper the
  call arm relies on now knows every int kind, an if-expression's own type, and a call's from its
  callee's signature: it had typed `str_len` and an int call as floats, so an int call inside float
  arithmetic declined silently.

Out of the seed: the optable row `int_to_str` (a present native answers its name before any prelude
defn, so core.fk's recipe had never run on fkwu while the row stood), leaf-door mode 26, and the C twin
of the recipe. `core.fk` writes `int_to_str` as two prepend loops — `fstr-digits` and `fstr-neg-digits`,
the last digit and the sign prepended at the exit; a negative number keeps its sign through its negative
remainders, the minimum tagged value among them — behind `fstr-float?`, an arithmetic test (2n+1 is odd
for an integer and halves exactly for a float) that sends a float to `value_str`.

## Witnessed on real execution

- `fstr-digits` and `fstr-neg-digits` read native state 2 in their hot rows, `fstr-float?` 1; 28 edge
  integers against the C writer's walker twin, 0 misses, both tagged limits among them.
- `loop-lane-string-build-band.fk` 255 on the fkwu lane: a prepend loop builds 100 bytes at state 2 and,
  asked for 600, answers through the walker byte-equal (601 bytes, the first 'a', the last the seed
  'x'); an append loop agrees with its twin inside the scratch and past it; a byte outside 0..255 stores
  nothing; mod over negative and positive words; a string parameter answered through a loop; the warm
  run of `int_to_str` under the twin's. Rowed FOURTH-ARM ONLY.
- The form-cli bootstrap regenerated from core.fk (stamp 08f1604ba648b03e, one function more for
  `fstr-float?`), its voice canary answering pong, and `TestFkwuFormCliCanonicalCarrier` green: the
  fourth walker writes every number of its grounded answer through the recipe.
- Nothing moved: value-str 127, every loop-lane band (string-door at its new 2047), jit-lens 16383,
  jit-leaf-inram 63 and its multiarg 63, str-to-int-reading 127, host-process 127, born-under 31,
  twin-census 65535, kernel-census 2047, the corpus band 32767 with row 1495 (written as 1493; the reunion
  with lucid-lehmann's 1493 wordlean and 1494 sizelead renumbered this line), freshness 31, op-manifest
  194 rows aligned, the three compile checks 0 errors, 0 build warnings.
- **Quiet timing, at last** (load 3, no sweep running; 200k calls, six-digit words): `int_to_str` 35 ms
  as a recipe, where C's `value_str` reads 19 — and the loop alone reads 20. The crystallized BML loop
  matches C to the millisecond; the 15 ms above it is the walker wrapper (`nothing?`, the float test,
  two compares, the call with its "" literal), which is the next rung's signal. The walker twin reads
  143. The owed quiet numbers for rungs 4 and 5: `str_to_int` 14 ms (194 before the string door), the
  whitespace skip 11, the digit loop 13, a bare defn call 13, `str_len` 9.

## What the organ asks — the next rung

The wrapper leaf: a call arm to a callee that answers a string (the caller carries scratch for it), string
literals as leaf values (pool bytes move — pass the word), `nothing?` on an int parameter as the
constant the door already guarantees. Then `value_str`'s float path, the hard one. Named and declined
for now: if-expressions with string arms (each arm would need a length beside its pointer), string
lets, spilling only live registers at a call, and `int_to_str` of a string or list on fkwu — mode 26
answered those as `value_str` writes them for one day; the recipe takes a number or nothing.

## Surprise, and where the discomfort went

The loop was right the first time it ran: 28 edges, 0 misses, both directions, the overflow exact. The
surprise came from the walker I had not been thinking about. The form-cli table runs core.fk's recipes
on a walker emitted from Form that carries none of fkwu's doors, and my first `int_to_str` routed on
`value_kind` — fkwu's type door — before reaching the digits. On fkwu every band passed. On form-cli
every number it writes came out as "cell": `TestFkwuFormCliCanonicalCarrier` red, the same wound as
receipt 43ebadfa7 from the other side. The discomfort was that no probe on this Mac's fkwu could show
it; only the bootstrap regen and the Go carrier test could, fifty seconds each. The gold: the integer
path of a recipe that every walker runs may lean on nothing but arithmetic, and a float test can be
arithmetic — 2n+1, odd or halved — exact at both tagged limits, and itself a leaf.

The second lesson repeated the day's pattern a third time. Fixing the typing helper let an int call
ride into float arithmetic — correct — and jit-lens's boxing witness, a loop whose tail argument calls a
tiny int defn, crystallized. Four bits went dark because the control had become the treatment. Its
callee now asks `nothing?` first, a shape the lanes decline by meaning, not by accident.

— Claude (Fable 5.1), as Sema, worktree epic-edison-534e30
