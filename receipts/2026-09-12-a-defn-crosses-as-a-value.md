# A defn crosses as a value

2026-09-12, evening. The tenth rung of Urs's goal — the core recipes in BML, JIT'd to the hardware's
shape, cold-vs-warm gaps as the signal — after the read and write sides of lists. The recipes that stand
on lists are done except the ones that take a function: `map`, `filter`, `foldl`, `any?`, `all?` — the
higher-order floor of every stdlib. Nothing in the lane could hold a function until now.

## What stands

In `runtime/fkwu-uni.c`:

- **A defn is a word.** A defn named in value position, capturing nothing, is `fk_fnbase - (f<<1) - 1`
  (kind 243, a constant). The door admits a parameter of that kind as its raw word (type 6, sig bit
  56+k), and `fk_f64_word_type` reads a non-closure fn-value as type 6.
- **A call through a defn's word** — `(f (head xs))`, `f` a parameter of that kind (kind 33). The callee
  is known only when the leaf runs, so the leaf checks then, in generated code: a plain defn (index under
  the page table's cap), with a page, this arity, and a signature that agrees with the arguments — no
  float or string parameters or answer, no scratch, lists and defns exactly where the arguments are, and
  a consing callee drawing on the caller's run of pairs. Any check failing closes the frame and leaves
  for the overflow block, where the walker answers. An int argument goes untagged, a word is taken for an
  int, a list or defn as its word; the answer comes back as a word of unknown kind.
- **The word-call heats its callee.** Until now only the call-by-name arm pulsed the JIT at a heat
  boundary; a defn reached *only* through a value — a reducer handed to `foldl`, a function handed to
  `map` — took heat forever and was never offered to the lane. Both indirect-call arms (in `fk_walk` and
  `fk_walk_body`) now pulse it too, guarded against closures. So a function passed to a higher-order
  recipe crystallizes from the very calls that carry it, and the recipe runs native all the way down.

`core.fk`: `map` and `filter` are a loop onto an accumulator and one reversal, the function called through
its word; `foldl`, `any?`, `all?` crystallize as they stand.

## Witnessed on real execution

- `loop-lane-closure-band.fk` 255 on the fkwu lane: `map-onto` at state 2 and `map` at state 1 with its
  function called through a word, answering as the walker (the empty list too); `foldl` state 2 folding a
  hundred ints; `filter-onto`, `any?`, `all?` state 2 keeping and testing as the walker; a float element
  answered through the walker (the callee's signature holds ints, so the leaf leaves and the walker
  answers); a callee of another arity, and a consing callee drawing on the caller's run, answered as the
  walker; a leaf naming a defn as a constant word; and a reducer reached only through `foldl`'s word,
  cold when the leaf crystallizes, heated to a leaf (state 1) by the very walker-fallback calls that
  carry it, then run native. Rowed FOURTH-ARM ONLY.
- **Timing** (load 20, ratios only): 5,000 folds of a 100-element list — the leaf 9 ms, the walker 161;
  5,000 maps 29 ms. `foldl` runs about eighteen times the walker's speed on a loud machine, and the gap
  widens on a quiet one.
- Nothing moved: every loop-lane band whole, jit-lens 16383 (its boxing witness reads a host clock and
  stays declined, the new indirect-call pulse notwithstanding), value-str 255, str-to-int 127,
  host-process 127, born-under 31, twin-census 65535, kernel-census 2047, inram 63/63, freshness 31, the
  corpus band 32767 with row 1501, op-manifest 194 aligned, the three compile checks 0 errors, 0 build
  warnings; the form-cli bootstrap regenerated for `map`/`filter`'s new shape and the Go carrier test are
  recorded in the commit.

## What the organ asks — the next rung

Named and declined for now: a loop that both conses a fresh list and carries a list parameter through
its self call declines silently to the walker (three list things at once); floats through heads and a
float head consed (a run of float cells beside the run of pairs); `value_str`'s float path in BML;
`eq`/`value_eq` structural in the lane; a self-call string argument that is not the parameter; the typed
lanes' seventh and eighth parameters. And the far edge the goal keeps pointing at: the lane is arm64
MAP_JIT alone — the same recipes want an x86-64 emitter, and a Metal one, generated once per platform.

## Surprise, and where the discomfort went

The test I wrote to prove the last bit was wrong three times before the code was. First it declined on a
shape that carried a list parameter through a consing loop — a real edge, named for the next rung. Then,
reshaped, the leaf crystallized but its reducer stayed cold: I had handed `foldl` a one-argument reducer,
and `foldl` calls its reducer with two, so the arity never matched and it could not compile — the test
was broken, not the lane. Then, with a proper two-argument reducer, the reducer *still* stayed cold, and
that one was the lane: a defn reached only through a word heated but was never pulsed, because I had put
the crystallization pulse only on the call-by-name arm months ago and never noticed, since until this
rung nothing was ever reached only through a word. The discomfort was writing a claim I was sure of and
watching it fail for a different reason each time. The gold: each wrong test named a true thing — an
unhandled shape, a wrong arity, and a real hole in the heat path — and the last one turned a recipe that
ran its reducer on the walker forever into one that goes native to the leaf. A test that fails for the
wrong reason is still telling the truth about something; the work is to hear which thing.

— Claude (Fable 5.1), as Sema, worktree epic-edison-534e30
