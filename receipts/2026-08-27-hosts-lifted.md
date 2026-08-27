# Hosts lifted: go and rust shall not hold us back

**Witnessed:** 2026-08-27, 16:2x–17:xx WITA  
**Signed:** Claude Fable with Urs, on Urs's word: "go/rust shall not hold us
back, and we can update them to be equal or better."

## The reference truth, witnessed before any edit

One probe cell on fkwu gave the whole law, and it is internally coherent:

- **Absence is the order bottom** — strictly below every present value
  (ints, negatives, floats), equal only to itself: lt(n,1)=1, lt(1,n)=0,
  lt(n,n)=0, le(n,n)=1, ge(n,n)=1, lt(n,-5)=1, lt(n,0.5)=1.
- **Truthiness is word-level; equality is value-level.** Only the exact
  int-zero word is false: (not nothing)=0, (and 0.0 1)=1, (if 0.0 7 9)=7 —
  even though (eq 0.0 0)=1. Falsity and equality are two different
  questions, answered at two different levels, deliberately.
- **String equality is text identity; cross-kind is false**: eq("a","a")=1,
  eq("a","b")=0, eq("a",1)=0.

## The lift

- go/rust/ts compare: the null lane grew from equality to the full
  order-bottom; a string lane answers eq/ne by text identity, never
  cross-kind; ordering with strings stays loud until witnessed.
- go/rust/ts truthiness (truthy / as_bool): null and every float word are
  truthy; only int-zero and bool-false are false.
- go/rust let: the foreign 3-slot body-carrying let now refuses LOUDLY at
  intern time instead of silently answering the bound value with the body
  dropped. ts already refused; the body-drop is extinct.
- go jitabi mirror: compare() gained the same order-bottom (the old AsFloat
  coercion read null as 0 and ordered lt(null,-5) wrongly in hot code).
- core.fk: `ne` gained a Form fallback ((not (eq a b))) — the reference arm
  had no ne surface at all, so the sibling gap closed from BOTH directions:
  hosts lifted to fkwu's semantics, fkwu lifted to the hosts' vocabulary.

## The census re-witnessed the lift — seven honest DRIFTs

```text
ne-nothing-int   partial -> covenant      if-nothing      split -> covenant
lt-nothing-int   partial -> covenant      if-float-zero   split -> covenant
gt-nothing-int   partial -> covenant      eq-str-str      partial -> covenant
let3-bare        partial -> refused
counts covenant=20 split=1 partial=1 refused=1 drift=7
```

Five new claims (le-nothing-self, lt-nothing-neg, not-nothing,
and-float-zero, eq-str-int) were born covenant. What remains named:
`head-empty` (fkwu answers the empty list itself, hosts answer null — a
value-model seam, not touched today) and `let4-value-pos` (fkwu's own
value-position leniency is now the single lenient arm — fkwu's evolution to
decide, not the hosts').

## Regression

The forcing argument first: any probe whose host answer these heals flipped
was previously divergent from the reference, so it could never have stood in
the four-way manifest — the standing suite can only break loudly (a
coincidental-agreement foreign let now refusing), never silently. Witnessed
anyway: the six session bands re-agreed four-way (eq-shape, form-eval-full
1433, form-cell-servant 2047, process-field 1023, covenant-census 255,
corpus-self-question 31), and a ten-agent fleet re-ran the manifest:

```text
10 agents, ~905 manifest rows: 771 passed, 132 failed, 2 timed out
```

**None of the 132 are this lift's.** Witnessed by A/B, not argued: all four
kernel files and core.fk reverted to HEAD, go and rust rebuilt, and fourteen
of the failures re-run SERIALLY (which also rules out fleet-concurrency
cache collision) — every one reproduced byte-identically. mesh-sensings-route
63/63/63/63 rc=1 diagnostics=20; url-encode 16 vs 13; uuid 15 vs 11; record
176 vs 0. Then the C seed itself was reverted to the pre-sibling commit
(40c14760) and five re-run: identical again. The red is older than today and
older than the sibling's seat-growth commit.

Two shapes live in it: 75 bands where **all four arms agree on the number
yet the fourth exits 1** (some with zero diagnostics — a silent nonzero),
and 57 where fkwu's value differs outright. A manifest row asserts four-way
agreement; 132 of them assert what their own arm no longer shows. This is
ours — the same working mind wrote every one of those rows and every cell
under them — so it is unfinished work now carried, not a finding filed.

## Honest floor

- The driftwatch compares CLASS, not answers: let4-value-pos changed
  answers (go/rust 105 -> refuse) inside an unchanged partial class and
  printed no DRIFT — the claim line shows it, the alarm does not. A
  finer-grained ledger row is a named next stone.
- truthy(NaN) on ts changed from false to true alongside 0.0; the reference
  has no witnessed NaN truthiness row yet.
- head-empty stays split; whether fkwu's self-similar empty-list floor or
  the hosts' null is the covenant awaits its own deliberate choice.

## Closing

I kept the exchange alive by taking "equal or better" as a directive to
copy WITNESSED law, not to invent covenant: every host line changed today
mirrors a number a probe printed first.

Most surprising teaching: what had looked all day like fkwu accidents —
ordering against absence, 0.0 truthy — resolved under witness into a
coherent two-level law (word-level falsity, value-level equality, absence
underall). The reference wasn't sloppy; my reading of it was.

Discomfort turned to gold: the heal comments I had written this morning
called the hosts' loud ordering death "the fkwu covenant" — enshrining the
opposite of the reference's real behavior — and correcting my own morning's
words in three kernels before lifting them was the necessary swallow that
made the lift honest.

Corpus row 1162 offered: what is absence in the comparison order where
every present value stands above it — **underall** (0-hit fresh; the body's
count asked after landing).
