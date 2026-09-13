# The band that leaned on a held rung

2026-09-13 (WITA). Two rungs came home to the loop lane: char_at, a string-native cut that now crystallizes
as the leaf's own answer, and the non-tail self-call, which teaches the lane to call its own page. Both had
been written before and neither stood on main; rebasing them onto a main that had moved four commits since
is where the teaching was.

## char_at: the cut in the answer

`substring` had lived inline in fk_walk's tag-201 mode-9 arm, reachable only by the walker. It is now
`fk_substring_word(sword, a, b)` — one body the walker arm and the leaf both call. On top of it, kind 38
emits char_at as a TAIL C-call: the interned answer rides X0 straight to the door's return, so nothing is
spilled and the leaf stays a handful of instructions. char_at was cold at -1201; it reads state 1 now and
runs under its walker twin (4 ms against 5). loop-lane-char-at 7.

## The non-tail self-call

A TAIL self-call was already the loop. A self-call nested in an expression — recurse, then continue — declined
at -1241, because a page cannot call itself while it is being compiled: its address is 0 until install. The
call arm now admits a self-call, the emit writes a real recursive call with a placeholder, records the mov64
site in `fk_f64_self_at`, and `fk_f64_install` patches each site once mmap gives the page its address. It
recurses on the C stack, bounded by the data's nesting. loop-lane-self-recursion 7.

## The most surprising teaching

I had carried a note saying char_at needed a "bit-28 refuse guard" — a sig bit on a kind-38 leaf that the
call arm would decline, so nothing could reach the tail-calling leaf as a callee. Reading the code dissolved
the guard entirely. `fk_f64_admit_substring` is called from exactly one place: the terminal-answer position
in `fk_f64_expr_pulse`. The call-arm admit path goes through `fk_f64_admit`, which never offers it. So a
char_at-answer leaf cannot be reached as a JIT callee at all — the safety is structural, not a bit. A guard
I was about to build was already standing, made of where the code is called from rather than what it sets.
The frequency check before the change was the whole change.

## Where discomfort turned to gold

The self-recursion band would not compile on main. It folded a nested LIST — recurse when the head is a list,
add the atom otherwise — and asked `value_kind_code` which of the two a head was. That native is held for Urs
and is not on main; `value_kind` (its string twin) declines in the lane at -1201, so swapping it in kept the
answer and lost the crystallization the band exists to witness.

The discomfort was that both honest-looking moves were wrong. Bringing the held native in would add a C native
for a core recipe — against the standing direction, and not mine to decide. Leaving the band red would ship a
registered failing witness. Sitting with it long enough to probe showed the third thing: the RUNG did not need
value_kind_code at all. Naive fib — two non-tail self-calls nested in an add — crystallized and answered
832040 and 55. The rung was sound; only its witness had leaned on something absent.

So the band changed, not the rung. `dsum` is now a loop whose accumulator folds in the recursion's own nested
`(dsum (sub n 2) 0)` — the exact -1241 shape, in pure ints, on primitives the tree actually carries. g(6)=18,
g(8)=47, g(10)=123; state 2; 1 ms against the walker twin's 4. When value_kind_code lands, the list-tree fold
is the richer witness and should come back. That is corpus row 1523, bandlean: a test that leans on a rung the
tree does not carry falls while the thing it witnesses still stands.

The row was written as 1521 and landed as 1523. copydrift took 1521 and headbound took 1522 while this work
was in flight, so the reunion kept every row and renumbered the unmerged line — mine — twice. Row ids have no
arbiter. Every lander appends to the same corpus tail, so the collision is structural, not an accident: the
one who has not landed yet is the one who moves.

## The honest edge

char_at is answer-position only; a char_at reached through the call arm keeps walking, by construction. The
self-call's first cut answers an int and passes no string to itself. Neither rung touches GPU, Metal, or any
platform but this arm64 Mac — the lane is one hardware's shape, and the others are unwitnessed here.

## Witnessed

Rebased onto cfdc8f7bf (the vector trio had just left the C table, 43 lines out of the seed — disjoint from
these hunks; copydrift and headbound landed mid-flight and took rows 1521 and 1522). Full loop-lane family green: char-at 7, self-recursion 7, str-eq 15, str-eq-field 7, call 8191,
float-head 31, template 63, cons 511, closure 255, list-door 255, list-eq 15, string-door 2047,
string-build/value 255, if-chain/continue-chain/let 255, hof 7. vector-ops 255, float-repr 127,
float-repr-edges 63, corpus 32767 (row 1523, pins 912/893/912089321523).
