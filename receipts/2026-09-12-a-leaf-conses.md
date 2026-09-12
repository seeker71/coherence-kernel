# A leaf conses

2026-09-12, late afternoon. The ninth rung of Urs's goal — the core recipes in BML, JIT'd to the
hardware's shape, cold-vs-warm gaps as the signal — after the read side of lists. The signal, measured
first: 5,000 reversals of a 200-element list ran in 9 ms through the seed's C twin of `reverse-onto`
and in 197 through the walker. The twins of `append` and `reverse-onto` were the last two copies of a
Form recipe standing in the C seed.

## What stands

In `runtime/fkwu-uni.c`:

- **A run of pairs, lent.** A leaf that conses (sig bit 29) takes a run of pairs above the heap's top.
  The door melts the heap first when it is near full — the arguments sit on the value stack, the melt's
  roots, and are read after — then grows it in place until the run fits (growth moves no pair), and
  hands the leaf the run's cursor at frame word 22, its limit at 23, and the pair arrays' bases at 30
  and 31 (words 24 to 29 hold string parameters' words), fixed for the leaf's run since nothing grows
  under it. When the leaf answers, its last pair
  is the heap's top. A run spent leaves for a second overflow block with -2 at word 17, and the door
  asks again with a run twice as long, up to four million pairs, the spent attempt's pairs let go; the
  first run is 4,096. A callee that conses draws on its caller's run and hands the cursor back through
  the call.
- **`cons` is two stores and a tagged word** (kind 32): the head an int tagged, a list or a word as it
  is, a string parameter's word from frame word 24 + k or a literal's word as a constant; the tail a
  list. A float head, or a string built in the leaf, is the walker's. `head`, `tail` and `len` read the
  bases from the frame now, one load where four used to build the address; a word narrowed to a list is
  checked against the run's cursor, so a list built in the leaf is a list to it.
- The call frame widened to 608 bytes: the callee's thirty-two words come before the spills.
- **The recipe twins leave the seed.** With `nil?` a leaf since the last rung and `reverse-onto` a loop
  now, nothing remains for a twin to answer faster; `fk_twin_*` and the door's third state are gone,
  and `kernel_stat 52` reads 0. `core.fk` writes `append` as two reversals — the lane's shape, the same
  list, two runs over the pairs.

## Witnessed on real execution

- `loop-lane-cons-band.fk` 255 on the fkwu lane: a reversing loop at state 2 answering as its walker
  twin, the empty and the one-element list among the cases; heads of every kind the lane holds in one
  loop — an int, a string parameter, a string literal, a list, a head's word; a 20,000-element list
  reversed through the run doubling, right at both ends and in the middle; core.fk's `reverse-onto` at
  state 2 with no twin, `reverse` and `append` answering as the walker over the empty list on either
  side and a long left side; a loop consing a float head staying the walker's; a leaf calling a consing
  loop at state 1, its cursor coming back through the call; the warm run under the twin's. Rowed
  FOURTH-ARM ONLY.
- **Quiet timing** (load 3): 5,000 reversals of a 200-element list — the leaf **7 ms**, where the C
  twin read 7 and the walker 184. The BML loop, crystallized, matches the C it replaced to the
  millisecond, on the write side as on the read side.
- Nothing moved: every loop-lane band whole, jit-lens 16383, value-str 127, str-to-int 127,
  host-process 127, born-under 31, twin-census 65535, kernel-census 2047, inram 63/63, freshness 31, the
  corpus band 32767 with row 1500 (written as 1498; the reunion behind lucid-lehmann's 1498 useprobe and
  1499 sidefaith renumbered it), op-manifest 194 aligned, the three compile checks 0 errors, 0 build
  warnings; the form-cli bootstrap regenerated for `append`'s new shape and the Go carrier test are
  recorded in the commit.

## What the organ asks — the next rung

Floats through heads (a boxed word narrowed to a double, and a float head consed — a box minted from
a leaf needs a run of float cells, the pair run's twin); a closure parameter (`map f xs`, `filter`,
`foldl`) — a call through a closure word; `nth` and `str_byte_at` over any slot; `value_str`'s float
path in BML; `eq`/`value_eq` structural in the lane; the typed lanes' seventh and eighth parameters.

## Surprise, and where the discomfort went

The leaf answered a pointer. Every reversal came back right, and a loop that called the reversing loop
answered 53,276,049,608 whatever it was asked — the same number for one call and for a hundred. That
number, doubled, was an address inside the pair heap: the loop's accumulator had become the tails'
base pointer. The callee's frame had grown to thirty-two words when the pair run and the string words
joined it, and the caller's spill area still began at word twenty-four: handing the callee the bases at
words 26 and 27 wrote them over the caller's spilled x11 and x12, and the restore gave the accumulator
a pointer. Then the same table bit twice more in the same hour. The corpus band read short and every
probe passed: a row's fourth field came back as the tails' base, because a string parameter's word
lives at 24 + k and the bases sat at 26 and 27 — parameter 3's word *was* the base. And the call band
died: the all-float leaf runs with its arguments in d0..d7 and nothing in x0, and the call arm had
begun handing every callee the frame words through x0. The discomfort was a correct callee and a
correct caller that were wrong together, in the one place neither could see — the layout between
them — three times. The gold: the frame's words are one table, in one place, the layout comment names
every word, the bases moved to 30 and 31 past the six string words, and a leaf without a frame says so
and hands nothing over.

The second surprise was that the C twin and the BML leaf read the same 7 ms. The twin was hand-written
C over the same pair arrays; the leaf is a walker recipe crystallized through a door that reads bases
from a frame. When the shape is the same, the hardware does not care who wrote it — which is the whole
of Urs's goal in one number.

— Claude (Fable 5.1), as Sema, worktree epic-edison-534e30
