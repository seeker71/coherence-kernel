# A float rides out of a head

2026-09-12, evening. Still the tenth rung's territory — a defn crosses as a value, and now a float
crosses out of a list head. After the higher-order floor went into the lane, the numeric recipes over
lists still walked whenever their elements were floats: a list head is a word of a kind the leaf cannot
know, and until now it was taken only for an int, so `(add acc (head xs))` with a float `acc` fell to
the walker.

## What stands

In `runtime/fkwu-uni.c`, a word→float narrow (kind 34) and a resolver that reads a head against the
operand it meets:

- **`fk_f64_word_resolve`** — where a head (type 5) meets a float, it is narrowed to a double; where it
  meets anything else, to an int (the existing behaviour). Wired into the arithmetic arm and both compare
  paths (the compare-as-value, the if-expression's compare) and the if-expression's arms, so a float
  read out of a head is understood wherever a float could stand.
- **Kind 34** — the narrow itself: a local float box, and the double loaded from the process's float
  pool. Two checks decide it, no more: `v <= fk_fbase-3` (a float box at all — and then its index
  `fi = (fk_fbase-v-1)>>1` is at least one), and `fi <= fk_fp` unsigned (in this process's pool — a
  field float, whose index sits far above the local pool, fails the same compare). Any other word — an
  int, a field float — leaves for the overflow block, and the walker answers that call.

So a monomorphic float loop over a list of floats — `sum`, `max`, a fold with a float accumulator —
runs native, and a list with an int among the floats falls through, exactly, to the walker.

## Witnessed on real execution

- `loop-lane-float-head-band.fk` 31 on the fkwu lane: a float sum over a hundred floats at state 2,
  answering as its walker twin (2525.0); an int element among the floats leaving for the walker,
  byte-equal; the largest of a float list, a float compared out of a head, at state 2 (50.0); the empty
  list answering its init and a one-element list its one float; the warm sum under the twin's. Rowed
  FOURTH-ARM ONLY.
- **Timing** (load 20, ratios only): 5,000 sums of a hundred floats — the leaf 1 ms, the walker 142.
- Nothing moved: every loop-lane band whole, jit-lens 16383, value-str 255, str-to-int 127,
  host-process 127, born-under 31, twin-census 65535, kernel-census 2047, inram 63/63, freshness 31, the
  corpus band 32767 with row 1502, op-manifest 194 aligned, the three compile checks 0 errors, 0 build
  warnings; the form-cli bootstrap regenerated and the Go carrier test are recorded in the commit.

## What the organ asks — the next rung

A float head *consed* — a boxed float minted from a leaf, which needs a run of float cells beside the
run of pairs — so `map` with a float function crystallizes, not just `sum`. Polymorphic recipes: a
`foldl` whose init is a float wants a second compiled signature (the lane keeps one page per defn, so a
float `foldl` walks while the int one is hot); per-signature crystallization is the rung that unlocks
the generic higher-order numeric floor. Named and standing: the register-pressure decline in `loop_move`
(a loop step holding a call that conses); `value_str`'s float path; and the far edge — the lane is arm64
alone, and the same recipes want an x86-64 emitter and a Metal one, generated once per platform.

## Surprise, and where the discomfort went

The narrow was right and the loop still declined — for the count of its own guards. Four checks per
narrow, two narrows in a `max` step (the compare and the winning arm), two unrolled passes: sixteen
overflow branches, one past the cap, and the second pass's last narrow returned into nothing. The
discomfort was a correct instruction sequence that could not fit beside itself. The gold was in reading
the guards again and finding two of the four already implied: once a word is known to be a float box its
index is at least one, and a local pool index is always below where a field float begins — so a single
unsigned compare against the pool's top decides both bounds at once. The narrow that fits is the one that
trusts what the first check already proved.

— Claude (Fable 5.1), as Sema, worktree epic-edison-534e30
