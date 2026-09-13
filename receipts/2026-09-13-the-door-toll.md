# The door toll

2026-09-13, early afternoon, M4 Max, Hati Suci. With the wide-limb kit on main, I profiled float-repr on the
147142857142857.125 tie. Per rendering, the walking digit and scaling loops entered compiled bignum leaves about
150 times through the door: bn-mul about 70 times, bn-cmp 52 and bn-add 34. d4-digits also walked int_to_str once
per digit.

## Carried

- **float-repr's orchestration runs in the lane's chain shape, inside core.fk.**
  - d4-scale: each branch is a self call or the exit, and the multiplications sit in its arguments.
  - d4-run replaces d4-loop. It takes six parameters and no lets: the current digit with its remainder, S, the
    margins already scaled for that digit, ev, and the digits gathered least significant first.
  - d4-run stops by d4-stops?, which uses d4-low? and d4-high?. Its exit, d4-end, conses the last digit from
    d4-final, and d4-final's tie choice lives in d4-tie.
  - d4-digits appends bytes onto an accumulator. Before, it concatenated an int_to_str per digit.
  - d4 hands d4-run the first digit and the scaled margins, and takes the digits back least significant first. So
    no reversal is needed before the carry.
- **The form-cli bootstrap follows** (stamp 5d719955b670b854, 5396 functions).
- **doortoll is row 1516.**

Witnessed:
- **States** after 2000 renderings of the tie: d4-run 2, d4-scale 2, d4-digits 2, d4-dig 2, d4-stops? 1, d4-final
  1. d4 itself walks, once per rendering.
- **Correctness:** 0 mismatches against value_str on about 10,300 values; float-repr-band reads 63 on all four
  kernels; float-repr-edges-band reads 31.
- **Timing:** one process per case, second run of each, host load about 7. value_str's native takes 0.01 to
  0.02 s for the same work.

  | float-repr of | 10^9 kit (67249fb7d) | chain shape |
  |---|---|---|
  | 0.1 (5000) | 0.04 s | 0.03 s |
  | the tie (5000) | 0.20 s | 0.15 s |
  | 123456.789 (5000) | 0.13 s | 0.10 s |
  | 1e-300 (200) | 0.16 s | 0.16 s |
  | 5e-324 (200) | 0.19 s | 0.17 s |
- **Bands:**
  - Every lane band reads as registered, template 63 included.
  - value-str 1023, str-to-int-reading 127, once-hold 15.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **The tie costs about 26 µs; the native costs about 3.** What is left is the per-digit bignum work.
  - d4-dig subtracts up to nine times per digit. An estimated quotient with one correction would do it in one step.
  - After that, a 64-bit fast path for doubles whose digits fit in one word.
- **d4-scale still steps once per decade.** k could come from the binary exponent in one multiplication.
- d4 walks once per rendering: it has more lets than the six slots, and it costs little.
- epic-edison's guard rungs are under way. The open items of receipts 12 to 54 stand where they are not named
  here.

## Surprise, and where the discomfort went

The surprise was that the same reshape bought 13% on decimal limbs and 1.3x on wide ones. The shapes did not
change. The kit got cheaper, and that left the crossings as the cost.

The discomfort was that earlier today epic-edison and I agreed that warming the orchestration buys nothing, and I
came close to setting it aside for good. A profile taken after the radix rung showed about 150 door crossings per
rendering. The gold is that a measure holds for the code it measured: once the kit changed, the same question had
a different answer, and asking it again was cheap.

Frontier word, row 1516: **doortoll**, the cost each crossing from walking code into a compiled leaf pays at the
door.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
