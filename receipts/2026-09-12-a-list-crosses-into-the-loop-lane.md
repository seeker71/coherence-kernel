# A list crosses into the loop lane

2026-09-12, late afternoon. The eighth rung of Urs's goal — the core recipes in BML, JIT'd to the
hardware's shape, cold-vs-warm gaps as the signal — after strings became values in a leaf. Every rung
before this one moved numbers and strings; the recipes that stand on lists — `nil?`, `drop`, every fold
and find — walked, and three of them had C twins in the seed to walk faster. The read side of lists is
now the lane's, and one of the twins is gone.

## What stands

In `runtime/fkwu-uni.c`:

- **A list is a word.** The empty list is 1; a list is an odd positive word whose pair index is under the
  heap's top — and every other tagged word (a float, a string, a closure, a record, a node id, nothing)
  is negative. So the door admits a list parameter as its raw word (sig bit 48+k), and answers a list
  (bit 25) or a head's word (bit 27) as the raw word back. `head` and `tail` are one load each from the
  pair arrays, their base read when the leaf runs: the heap grows in place, and a leaf never conses, so
  no melt moves a pair under it. `head` of the empty list is nothing — the walker's answer, through the
  overflow block; `tail` of the empty list is the empty list. `len` of a list walks its tails; `len` of
  a string is its length word; `(eq (len L) 0)` is L's word against 1, no walk.
- **Typefall.** A head is a word of a kind the leaf cannot know. Beside a number it is taken for an int
  (even, untagged), beside a list parameter for a list (odd, positive, under the heap's top); one test at
  the use tells, and a word not of that kind leaves for the overflow block, where the walker answers
  the call exactly. A float among the ints sums as a float; a string finds itself. The door now
  initializes and reads the overflow word for every leaf, and a caller reads its callee's after every
  call.
- **A compare is a value** (1 or 0, CSET), **any int expression is a condition** (tested against 0 as the
  walker tests it, a compare as a value keeping its own compare), and **a call to a small defn is
  inlined** — a body that is one expression over its parameters, no self call, no let, admitted with
  each parameter read as the caller's argument expression, two levels deep; a body that does not admit
  restores the admit's state and the call arm has its turn. So `(if (nil? xs) …)` in a loop is `xs`
  against 1, and core.fk's `nil?`, `drop` and any sum, count, find or nth written as a tail loop
  crystallize as they are. The C twin of `nil?` leaves the seed.

## Witnessed on real execution

- `loop-lane-list-door-band.fk` 255 on the fkwu lane: a sum with `nil?` as its condition at state 2 and
  `nil?` itself at 1; core.fk's own `drop` at state 2 answering the empty list past the end; an nth at
  state 2 handing back an int, a string, a list, and nothing for the empty list; typefall on a float and
  on a string; `len` in a leaf and a find over `eq`; a twin whose compare holds a host clock staying the
  walker's; identity over words in a list of lists; the warm sum under the twin's. Rowed FOURTH-ARM
  ONLY.
- **Quiet timing** (load 3): 20,000 sums over a 200-element list — the leaf **5 ms**, the walker twin
  898: a hundred and eighty times, on the read side alone.
- The field came back fresh mid-rung, so the whole sweep stands again: value-str 127, str-to-int 127,
  host-process 127, born-under 31, twin-census 65535, kernel-census 2047, jit-lens 16383, every
  loop-lane band whole (string-value and call 255 again), inram 63/63, freshness 31, the corpus band
  32767 with row 1495, op-manifest 194 aligned, the three compile checks 0 errors, 0 build warnings.
- The form-cli bootstrap regenerated from core.fk (stamp e4d634b1c6340d96, the voice canary answering
  pong) and `TestFkwuFormCliCanonicalCarrier` green — after the wound told below.

## What the organ asks — the next rung

The write side of lists: `cons` from a leaf, with a run of pairs the door reserves ahead (growth
relocates nothing; only the melt moves pairs, and the door can melt before the call), overflow to the
walker as the scratch does — so `reverse-onto`, `append`, `take`, `map` and `filter` over first-order
bodies crystallize and the last two twins leave the seed. Then floats through heads (a boxed word
narrowed to a double), `str_byte_at` and `nth` over any slot, and the closure parameter (`map f xs`).

## Surprise, and where the discomfort went

The lane took its own witnesses. Every band since the string door had used "a call in the condition" as
the shape the lane would never take; the inliner takes it, and eight bands read short in one sweep —
their walker twins had crystallized, their small leaves had vanished into hot drivers and read state 0.
Nothing was wrong except the instruments: a control is a shape, and a growing lane takes shapes. The
discomfort was watching a whole afternoon's bands go dark from one correct change. The gold is the rule
that came out of it: a twin holds a door the lanes decline *by meaning*, not by accident — a host clock
in its compare, `(lt (host_monotonic_ms) 0)`, which no lane will ever admit and which never changes the
answer — and a claim about a small leaf's own state drives it from a walker, since a hot driver would
inline it. Seven bands and jit-lens carry that rule now, in one line each.

The second surprise was the inliner's first hour. Every probe answered right, every .fk band read
whole, and the form-cli regen — the table compiler running on this binary over 185 sources — wrote a
table with the wrong string count. A bisect with the inliner off passed, so the inliner was wrong
somewhere no probe reached. It was this: `str_len` and `str_byte_at` read their string by *slot number*
— "parameter k" — straight from the AST, while every other arm went through the admit, where an inlined
body's parameter k is mapped to the caller's argument. Inside an inlined `(defn strlen2 (s) (str_len
s))`, slot 0 was the caller's slot 0: another string's length, a wrong count, silently. The discomfort
was a green sweep over a wrong compiler. The gold: every read of a parameter goes through one door, and
the door knows whose parameter it is. Both arms admit their operand now, and take its slot from the
admitted node.

The third teaching was the melt. Cons can move every pair; a leaf that reads pairs never allocates.
The read side is safe by that one abstinence, and the write side will be safe by one reservation.

— Claude (Fable 5.1), as Sema, worktree epic-edison-534e30
