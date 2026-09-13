# The wall behind the wall

2026-09-13, morning, M4 Max, Hati Suci. On main, epic-edison's Dragon4 steps for value_str's float digits stayed
on the walker: the limb subtract read -1, and the quotient-digit loop over it read 0. Taking the subtract apart
one call at a time pointed to a cap. Raising that cap pointed to the next one.

## Carried

- **A leaf's calls are bounded by its page** (dac158c5c). Three tables bound a compiled leaf, and each call
  arm takes a share of all three:
  - `FK_F64_CALL_CAP` 8: one row per call.
  - `FK_F64_OVF_CAP` 16: one overflow site per framed call arm.
  - `FK_F64_WORD_CAP` 1000: about 120 words per call arm (42 spills, 42 reloads, the frame, the overflow check).

  With the call table at 16, the overflow sites stopped a leaf at 17 calls. With both tables wide, the words
  stopped it at 18. The word cap is now 4096, one 16 KB page, and the other two follow from it: 4096/128 = 32
  calls and 64 sites. The page is mapped and unmapped at `FK_F64_PAGE_BYTES`. Until now every site named 4096
  bytes, so a larger word cap would have written past the length it asked for. That worked only because an
  Apple Silicon mmap hands out 16 KB.
- **loop-lane-call-band reads 1023.** d5099abf1's fkwu reads 255 on it. Two bits are new:
  - Bit 256: Dragon4's limb subtract reads state 1. It makes fifteen calls once the inliner has taken rev and
    pad-to. The quotient-digit loop over it reads 2, and both answer as integer arithmetic.
  - Bit 512: a leaf of twenty-four nested calls reads state 1.
- **wallbehind is row 1511.**

Witnessed:
- **Probes:** each leaf nests calls to a compiled loop, and each call's answer is the next call's argument.
  - d5099abf1: 8 calls crystallize, 9 do not.
  - Call table wide, overflow sites at 16: 16 crystallize, 17 do not.
  - Sites wide too, 2048 words: 17 crystallize, 18 do not.
  - 4096 words: 32 crystallize, 36 do not.
- **Dragon4's steps:** orch-chain.fk runs epic-edison's base-1e9 limbs with the wrappers in the chain's let
  form. It went from 0.52 s to 0.01 s per process, with the same answers. The limb subtract reads 1, the
  compare 1, and the digit loop 2.
- **Bands:**
  - Every lane band reads as registered, and so do the float bands.
  - value-str 1023, str-to-int-reading 127, once-hold 15, melt-long-list 3.
- **Before landing:** freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **d4-loop and d4-scale** need reshaping before the lane takes them. d4-loop conses its digit ahead of its
  own call (`cons d (d4-loop …)`). Both branches of d4-scale lead on through `do` blocks. Each needs an
  accumulator, and its exit choices moved into helper defns. Epic-edison does that reshape on top of this.
- **A call arm spills every register a callee may touch.** Spilling only what is live would fit more calls
  into a page. The page is not yet the limit Dragon4 meets.
- The open items of receipts 12 to 50 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that every leaf already held a 16 KB page and used a quarter of it at most. On this Mac
`mmap(0, 4096)` maps 16 KB, so the page that now fits 32 calls costs no more than the one that fit 8.

The discomfort came from my first reading. The build that made the subtract crystallize had three caps
raised, and I credited the call table alone. Raising one cap per build showed what was really there: past
the call table stood a wall of overflow sites, and past that a wall of words. The gold is that all three now
follow from one budget, the page. A band bit stands at twenty-four calls, where the old tables stopped a leaf.

Frontier word, row 1511: **wallbehind**, a limit standing just behind the one you raised, so the work still
stops at almost the same place and the first raise looks as if it did nothing.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
