# 2026-09-06 — the JIT on the glass

Urs, midday: "see when the JIT is boxing and unboxing and loop unrolling."

## What the seed had, and what it did not

Boxing was already charged per defn: every float result allocates a pool slot
and `fk_fn_fbox` remembers which recipe minted it — the exit report wrote it to
`.fkwu-boxing` as "the unboxing worklist". Unboxing was not counted anywhere.
And loop unrolling has no lane in this seed: the arm64 u32 leaf "intentionally
does not claim that loop" (`runtime/fkwu-uni.c:3349`); the tree walk runs every
iteration. That is a fact to show, not a feature to invent.

## What stands

The kernel now charges every float box it *reads* to the defn running
(`fk_fn_unbox`, in `fk_num`), counts native arm64 leaf calls at their one
door, and publishes three totals on its live page (words 24–26) and as
`kernel_stat 45/46/47`. `kernel_hot_rows n` answers each hot defn as
`(heat name unit line col boxes unboxes)`; `kernel_box_rows n` (tag 191 — 197,
the tag my memory called free, is `method_define`; the mirror gate said so
within a minute) is the unboxing worklist. On the glass, the `j` view carries
`jit-boxes`, `jit-unboxes`, `jit-native-calls`, the `box.<defn>` worklist rows,
the hot defns wearing both ledgers, and `jit-unroll` named absent by its door.

```
jit-lens  boxes 20002  unboxes 40000  native 0
          jlb-floaty [20001, 40000]   jlb-inty [0, 0]
```

Twenty thousand float adds: one box minted per result and two reads per add
(the accumulator and the constant); the int twin touches the pool never.
`jit-lens-band` 255; every gate and glass band as before.

Ledger R122 released. Corpus 1290 *unboxsight*.

## The most surprising teaching

Unboxing costs twice what boxing does, and nothing had ever counted it:
`(add acc 0.5)` mints one slot and dereferences two. The exit report's
"unboxing worklist" ranked recipes by mints alone; the read side, the larger
half, was invisible until a counter stood beside `fk_num`.

## Where discomfort turned to gold

"Loop unrolling" arrived as something to show, and the seed has none. The pull
was to build a lane so there would be something to see. The gold was a row
that says so — `jit-unroll: unavailable`, door `fkwu-uni.c:3349` — on the same
glass as the two ledgers that are real. What is absent is shown as absent, by
the line that decided it.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015, native-surface 1023, jit-lens-band 255
