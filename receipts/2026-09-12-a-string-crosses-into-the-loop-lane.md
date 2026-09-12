# A string crosses into the loop lane

2026-09-12, midday. Urs set a standing goal: the core recipes — `int_to_str`, `value_str`, `eq`,
`value_eq` and all the rest — belong in BML, not the C kernel, generated once per hardware platform to
the hardware's own shape and reused as O(1) blocks through the JIT, with every boxing, unrolling and
JIT practice, until the stdlib matches C++, Go and Rust in features and speed; the C bootstrap stays
minimal, BML grammar usage maximal, and any cold-vs-warm difference is a signal. The hardware is the
only limit.

The goal landed the morning `value_str` went the other way: ~75 lines of C squeezed into a leaf door
(mode 25) because the tag space is full — the very pressure the goal relieves. The body had already
chosen the goal's direction once, on 2026-07-01, when `substring` and `str_find` left the seed: "the
JIT takes the class, the seed stays its size." This receipt is the first rung of making that the rule
for every core recipe.

## The signal, measured first

Two hundred thousand calls each, on the walker, nothing crystallized (`kernel_stat 48/49/50` all 0):

| recipe | ms | where it lives |
|---|---|---|
| `eq` | 10 | C native |
| `value_str` | 17 | C native (that morning) |
| `int_to_str` | 20 | a thin wrapper over `value_str` |
| `value_eq` | 18 | C native |
| `str_to_int` | **194** | BML: `fstr-to-int-loop`, a per-byte loop over `str_byte_at` |

`str_to_int` is the one named recipe that lives in Form, and it is ten times slower than its siblings
for exactly one reason: its shape is the loop lane's — a self tail call over ints — but the lane's
admit set (tags 1, 53, 2, 110, 3, 4, 42, 10: literals, parameters, add, sub, mul, div) carries no
string op, so it never goes hot. That 10x is the cold-vs-warm gap between "in C" and "in BML", and it
is the number the whole program is anchored to: the goal wants `value_str` out of C, which is sound
only if the JIT makes the BML version as fast.

## What stands

The string door in the transparent loop lane, `runtime/fkwu-uni.c`:

- `fk_f64_loop_pulse` reads a string in a frame slot as type 3 (signature bit 16 + k); before, a
  string word stopped the lane with -2.
- `fk_f64_admit` takes tag 28, `str_byte_at`, over a string *parameter* only. Arithmetic, a compare,
  or an exit over a string pointer is declined, naming its tag, and the recipe stays the walker's: a
  pointer added to a number is no recipe's meaning.
- `fk_f64_emit` kind 14: load the string's length from frame word 9 + k into x9 (the literal scratch,
  dead between uses), compare the index unsigned, then `ldrb` the byte or answer -1 by `movn`. That is
  the walker's own arm (`t == 28`: `k < 0 || k >= len` answers -1), a negative index reading as past
  the end through the unsigned compare. Six instructions, no call-out.
- The tag-194 door widens its frame array from 9 to 17 words and places each string parameter's byte
  pointer (`fk_srange`) in its word and its length at 9 + k; word 8 stays the iteration count. The
  pointer is safe for the leaf's whole run for the reason `2026-09-03-runtime-string-argument.md`
  gave: a leaf never calls out, so nothing can intern a string and move the pool under it.

The recipe is untouched. A string rides the int register as a pointer through the parallel move, and
only `str_byte_at` reads it.

## Witnessed on real execution

- A loop-shaped byte sum, `(if (lt i len) (self s (add i 1) len (add acc (str_byte_at s i))) acc)`,
  over a 220-byte string: `kernel_stat 49` reads 0 before and 1 after — the loop crystallized. Twenty
  thousand calls, 4.4 million byte loads: **323 ms on the walker** (the same sum by a nested-if shape the
  lane declines), **3 ms hot** — about 108x. The two sums are equal, byte for byte.
- Every read past the end, hot, answers -220 over 220 reads: the leaf's -1 arm is the walker's.
  Every read at a negative index answers the same.
- `loop-lane-string-door-band.fk` reads 127 on the fkwu lane: crystallized, sums equal, -1 past the
  end, -1 at a negative index, the string passed through the tail call naming the same bytes each
  iteration, arithmetic over the string parameter declined with the count still right, and the warm
  run under the walker's. Rowed FOURTH-ARM ONLY: the lane is fkwu's MAP_JIT arm64 door.
- Nothing moved: `jit-lens` 16383 (the loop lane's own witness), `jit-leaf-inram` 63, its multiarg
  band 63, `once-hold` 7, `float-mint` 63; `jit-lower` 15, `jit-lower-emit` 63, `jit-native-span`
  127, `f64-wire` 2047 and `float-parity` 255 four-way through `validate.sh`. The walker baseline
  re-read 18 / 17 / 192 / 9 / 18. `binary-freshness-band` 31. The mac, Linux-shaped and Windows
  compile checks read 0 errors, the build 0 warnings.

## What the organ asks — the next rungs, named

- **The multi-exit loop.** `fstr-to-int-loop` is `(if (ge i len) acc (if (digit?) (self …) acc))`:
  two exits and a non-self call, and `ge` is not a lane compare (only `le` 5, `eq` 102, `lt` 103). The
  lane takes `(if <cmp> <exit> <self-call>)` alone. Needs a chain of compare-and-exit arms, and either
  the inlining of a small non-self defn (`fstr-digit-cp?`) or the admission of its bool shape. That is
  the rung that turns `str_to_int`'s 194 ms into the lane's floor — the 100x is waiting on it.
- **`value_str` out of C into BML.** Its int path is a digit loop, which needs a byte *store* into a
  string the leaf can hand back — the emit side of strings, where this rung built the load side. Its
  float path is `fk_fmt_float_js`, the shortest-'g' JS rendering, the hard one. Its list path is a
  recursive `str_concat`.
- **The emitter itself leaves C.** `fk_f64_*` is C; `form-lower.fk` is the Form-authored emitter,
  with `lo-streq` and `lo-strfind` already inlining byte loops, but only on the explicit path. "C
  minimal, BML maximal" reaches the transparent lane's arms in time.
- **Per-platform emission.** arm64 MAP_JIT only today. x86_64 for Windows and Linux, and "generated
  once per platform and reused" is the 2026-09-01 two-tier shape: RAM always, disk optional and
  byte-keyed.

## Surprise, and where the discomfort went

The string-family receipts of 2026-09-03 called the runtime-string door "an unbuilt one" — new C-side
argument marshalling — and I set out expecting to build one. The transparent lane's dispatch had
already built it without saying so: it untags every frame word into a plain array the leaf reads by
offset. The door was open; only the meaning of one word in it had to change, from an untagged int to a
byte pointer. And the walker's bound came along for the price of one unsigned compare, because the C
arm's -1 was already a single rule.

The band read 95, not 127. The pull was toward the newest, least-trusted code — the declining arm
behind bit 32 — and toward changing it. The band's own print line said `spaces 40` first. Ten tokens a
line, each followed by one space, four lines: forty. I had counted eleven. The arm had declined the
arithmetic and the walker had counted the spaces exactly; the miss was my expectation, as it was with
`unpaged` the day before. Reading the evidence line before touching a line of code turned a suspected
wound in the door into a wrong number in a comment.

— Claude (Fable 5.1), as Sema, worktree epic-edison-534e30
