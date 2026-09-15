# A let answers its value, a closure keeps its binding, a tail call keeps no frame

2026-09-15, afternoon, this Mac, Hati Suci. The three items the last receipt left open, closed
after Urs asked that every known gap close and that a language surprise be fixed in the language;
then, mid-movement, that checks live in the code that does the work and speak when it departs.

## Carried

- **A let's value is the value it binds, on fkwu too.** A let that ends its do reads its own slot
  (tag 110) in `fk_parse_do`; a bare let in value position (an if arm, a defn body) reads its slot
  instead of lit 0 in `fk_sparse`; a let that ends the unit's top-level do answers its hold in
  `fk_parse_top_do_value`. The column-0 let already answered its hold. It still binds nothing past
  itself. Go, Rust and TS already answered the value.
- **A closure sees the bindings in scope where it was defined, on Go, Rust and TS.** Inside a do,
  a let binds in a fresh frame when a closure was made since the scope last opened, so
  `(defn f (x) (do (defn g () x) (let x 2) (g)))` answers 1. At the unit level a closure carries
  its own empty frame stamped with the unit version, and a later unit let that rebinds a name keeps
  a history, so a defn made between two unit lets of one name reads the first and later code reads
  the second, as fkwu's hold per reference reads it. The unit frame keeps its identity for route
  startup and name-check, and the collectors mark the history.
- **A tail call keeps no frame alive, on TS and Rust.** The TS walker takes a conditional's arm, a
  do's last form and a closure's body in place and replaces its form-stack slot, as Go and Rust
  do. The Rust walker drops the frames its loop made at each tail call while no closure was made
  since the loop began.
- **A kernel says it when it departs.** TS voices one organ-health-v1 reading on stderr when
  `cons` and `tail` pass 2^28 copied list elements (`list` / `copy-budget`, need
  `shared-tail-list` left open); no remedy exists in-process, so the need stays open for care to
  route. It rides stderr, the line the native process runner reads live; the voice that landed
  beside it (531449344) speaks a BML organ's rows on stdout through `print_str`. Two paths, one
  schema, and the same reader takes both. A unit let that rebinds a name inside a function's own
  frame had a reading of its own while `walk_recipe_here` could walk a recipe there; that door left
  the siblings with 2622299f6, the shape went with it, and so did the reading.
- **let-scope-band stays as it was**, nine bits at 511; its cases grew for a moment and came back
  out when the checks moved into the code.

## Witnessed

Probes, fkwu / Go / Rust / TS, after the last rebase:

| probe | reads |
| --- | --- |
| `(do (let x 5))` | 5 / 5 / 5 / 5 (fkwu read 0 before) |
| `(defn f () (let x 3))` `(f)` | 3 / 3 / 3 / 3 (fkwu read 0 before) |
| `(do (let y 0) (if 1 (let y 9) 0))` | 9 / 9 / 9 / 9 (fkwu read 0 before) |
| `(defn f (x) (do (defn g () x) (let x 2) (g)))` `(f 1)` | 1 / 1 / 1 / 1 (siblings read 2 before) |
| `(do (let x 1) (defn g () x) (let x 2) (g))` | 1 / 1 / 1 / 1 (siblings read 2 before) |
| `(let x 1)` `(defn g () x)` `(let x 2)` `(g)` | 1 / 1 / 1 / 1 (siblings read 2 before) |
| `(defn f (x) (do (defn g () x) (let x 2) (add (g) x)))` `(f 1)` | 3 / 3 / 3 / 3 (siblings read 4 before) |
| `(do (let t 0) (do (let t 5)) t)` | 0 / 0 / 0 / 0 |

- let-scope-band reads 511 on fkwu, Go, Rust and TS.
- string-join, run alone on validate's captured inputs: TS 255 in 31 s at a 380 MB peak (it passed
  48 GB and never finished before); Rust 255 in 82 s at a 130 MB peak (the origin/main Rust passed
  22 GB within 3 s on the same inputs).
- TS on string-join voices `list` / `copy-budget` at 268,449,175 copied elements, and
  `observe/organ-health-run.bml` holds the reading in its current map (rc 0). The rebind reading
  Go, Rust and TS gave for a recipe walked inside a function left with `walk_recipe_here`.
- All 1049 fourth-arm bands on fkwu before and after the let change: the only differences are
  let-scope (its cases were changing then), loop-lane-char-at (a timing bit; 15 run alone), the
  q8-0 matmul pair and gpu-lower (the baseline binary's place; equal beside `./fkwu`: 15, 5,
  2047), and host-process (the baseline binary's name; bits 1 and 2 ask for a binary named fkwu).
  No band's value came from a let answering 0.
- Old against new on a let-heavy loop: Go 0.17-0.21 s and 0.18-0.19 s, Rust 0.26 s and 0.26-0.28 s,
  TS 0.44-0.51 s and 0.50-0.51 s, the same answer throughout.
- The five backend test files pass after the TS walk rework; tsc clean. Freshness 31.

Through ./validate.sh on the tree rebased onto 4bde1c4d9, each line "1 band(s) four-way, 1 ok, 0
divergent": let-scope-band 511; string-join 255 (99 s); all 28 bml-hati-native proofs, each read on
fkwu at its manifest mark (my 18, origin's six control-flow proofs, and origin's four newer ones,
import-consts 13, lane-parity 14, path-words 11 and state-snapshot 25); let-effect-once 111,
recursive-let-clobber 1, call-head-reading 255, vector-ops 511, rounding-ops 255, record-band 176,
record-blueprint-band 7, method-band 116, bml-native-mutable-locals-band 1023,
bml-native-interface-package-import-band 255, both class-model proofs and glass-sensor-body-scope.
primitive-registry agrees three-way at 47 (exits go=0 rust=0 typescript=0; it has no manifest
row). The drift door reads `pass=16383 full=16383 refused=0`. gpu-lower and intern-content-address, fourth-arm only by their own headers, read
1048575 and 127 on fkwu at that level; gpu-lower's bit 65536 is a let after the exit riding the
kernel loop. homecoming-distillation-corpus reads 32767 on fkwu at its mark. On the twelve bases before, 853331098, 785fe1326, bbfefcdb4, 1c33f84db, d15962210,
5724bb4f7, 5c34e25fa, 694633b20, 0a95061ee, 34d6c967a, 449628840 and 6b1e17ea8 (there less
intern-content-address and primitive-registry), the same set ran the same way, the drift door
reading `pass=16383 full=16383 refused=0` each time, except gpu-lower on 5724bb4f7 and d15962210:
there it answered 903167 when let-scope-band ran first through validate.sh on a fresh build, on
origin's own binary too, and 1048575 otherwise. 1c33f84db, from a sibling session, healed that
(`kernel_ast` answers the process's own defn body live); the same sequence now reads 1048575.
On 2ffd5f9a5, whose one change these bands read is vector-ops.bml (0fb54dcae), let-scope-band,
vector-ops and rounding-ops ran again four-way, gpu-lower at its fkwu-only level and
primitive-registry three-way, each "1 ok, 0 divergent". 4bde1c4d9 carries 2622299f6, which took
`walk_recipe` and its kin out of the siblings. The siblings commit was rebased over it, the natives
it had touched leaving with the rest, and the 30 rows that commit added read four-way against these
siblings, each "1 ok, 0 divergent".

## Still open, with the reason

- **The voiced need stays open.** TS lists copy on `cons` and `tail`; a shared-tail list would
  answer it.
- **q8-0-matmul-mma reads 5 on fkwu against the manifest's 15**, before and after this change.
- **Go and Rust read past a stray `)` where fkwu and TS stop**; a band typo of mine showed it.
- **fkwu captures at most eight names per function** (`FK_CLOSURE_CAP_MAX`, `fkwu-uni.c:632`). A
  ninth is declined with `[closure-scope]` and rc 1, where Go, Rust and TS answer 45. Since
  0a95061ee the decline also arrives as organ readings, a `compile-error` asking for
  `source-diagnostics` and a `binding-missing` for the name, so care can see it; the cap itself is
  the open part.

## Closing

Most surprising: string-join was never healthy on two of the four kernels. Rust answered 255 for
as long as the band has stood, by spending tens of gigabytes, and nobody watched what it spent; TS
and Rust shared one fault, a tail call keeping its caller's frame alive.

Discomfort to gold: a 104 GB Rust leg appeared right after my closure changes, and the pull was to
blame my own frames. The origin/main Rust at 22 GB in 3 s showed it older, and the repair went to
the root rather than back through my commit. The sweep's six differences went the same way: each
looked like my let change until a baseline built beside `./fkwu` showed a timing bit, a place and
a binary's name. And the band cases I grew to 4095 came back out when the checks moved into the
code; that felt like undoing work, and the gold is kernels that now say it themselves.

Frontier word: **dearright** (0 hits in the tree). Its question: when a kernel answers right by
spending what it should not, is the answer witnessed, or does a verdict need its cost beside it?

— Claude (Opus 5), as Sema, worktree agent-a4e17ec567397b8e7
