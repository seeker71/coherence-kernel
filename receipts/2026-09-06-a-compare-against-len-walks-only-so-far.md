# 2026-09-06 — a compare against len walks only so far

Urs, evening: "continue with optimizing."

## What the page said

The live glass loop, read from its own page after eight seconds without a
terminal (`kernel_page_hot pid 24` from another kernel): 219M dispatches,
64% of a core, and at the top of the heat `nil?` 2.6M and `append` 2.6M —
ahead of every glass recipe. `nil?` is `(eq (len xs) 0)`. `len` walks the
list. So `nil?` on a list of ten thousand costs ten thousand cells, and every
list recursion in the body — `append`, `map`, `take`, the grammar walkers,
the glass's row folds — was quadratic in the list it walked. A probe: `nil?`
one hundred thousand times on a 10K list, 300 ms; the same over
`(eq xs (list))`, 6 ms; `append` of one element onto that list two hundred
times, 4.7 s.

## What stands

**A compare against `len` walks only as far as it must.** When one side of
`eq`, `lt` or `le` is a `len` node and the other an int literal K — either
order, since `gt` and `ge` lower onto `le` with the literal on the left —
the arm walks at most K+1 cells (`fk_len_upto`, `fk_len_cmp` in
`runtime/fkwu-uni.c`). The child is evaluated exactly once; the answer is the
word `len` would have given, compared the same way. No new tag, no change to
`len`, no change to `nil?`, and the same answer over lists, strings, ints,
floats and the empty list — eight kinds probed against the old shape.

**Witnessed.**

| probe | before | after |
| --- | --- | --- |
| `nil?` ×100K on a 10K list | 300 ms | 6 ms |
| `append` of one element ×200 onto a 10K list | 4677 ms | 161 ms |
| `len` ×100K on a 10K list (not a compare) | 290 ms | 285 ms |
| corpus band, cold ice | 2.30 s | 1.87 s |
| live glass loop, 8 s, dispatches per CPU second | 31M | 98M |
| live glass loop CPU | 64% | 49% |

Quartet 42 / 31 / 1 / 2047. table 2047, float-natives 28, eq-shape 524287,
jit-lens 16383, persistence 7, channel-breath 500, glass observer / live /
live-ui / kernel-view / dashboard / events-channels / frame-work unchanged,
cell-store / field / node-gift unchanged, corpus 32767, mirror gates 1023 /
63 / 1023. `sha256-band` answers 2 on this seed and on the one before it.

## The most surprising teaching

The hottest recipe in the glass was not a glass recipe. The page ranks defns
by dispatch, and `nil?` sat first not because the glass calls it so often
but because everything does — and the cost it hid was not even in the
dispatch count: `len`'s walk is C time inside one arm. The loop's
dispatches per CPU second tripled while the dispatch count rose, which is
the signature of a cost that lived under the counter, not in it.

## Where discomfort turned to gold

The clean fix was one line in core.fk, `(eq xs (list))`, and it was wrong:
it changes what `nil?` answers for a string, an int and a float, in a body
where four kernels share that recipe and nobody knows every caller. Sitting
with that, the question moved from "what should `nil?` be" to "what does
the walker already know when it sees this shape" — and it knows the literal.
The peephole keeps every recipe as written and every answer as it was, and
the whole body gets the cut, not one file.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2047, nil?-100k 300->6 ms, append-200 4677->161 ms, corpus-cold 2.30->1.87 s
