# A whole double's own digits

2026-09-13, early afternoon, M4 Max, Hati Suci. For common values the profiles all pointed at Dragon4's per-digit
bignum work. One common class never needed that work: doubles that are whole numbers.

## Carried

- **float-repr takes a whole double below 2^53 straight from its digits.**
  - fr-whole? asks whether the value is under 2^53 and equal to its own integer.
  - fr-cut takes int_to_str's digits, strips the trailing zeros, and turns the length into the exponent.
  - Every other double goes to d4 as before.
- **Why it is exact.** Every integer below 2^53 is a double. Any shorter decimal differs from it by at least one
  unit, which is more than the half unit that still rounds back.
- **float-repr-band gains bit 64 and reads 127 four-way, registered.** Bit 64 covers 0 to 2000 and their
  negatives, plus 100 whole numbers below 2^53, each alone and with its last three digits zeroed.
- **The form-cli bootstrap follows.**
- **exactwhole is row 1518.** Row 1517 is left for epic-edison's str_eq guard.

Witnessed:
- **Correctness:** 0 mismatches against value_str.
  - The families: every 2^k, the odd multiples, the subnormals, 10^k, and 1,200 LCG doubles.
  - 40,002 whole numbers from -20000 to 20000, and 4,000 random whole numbers below 2^53.
  - 2^53 − 1 renders from its digits. 2^53 and 2^53 + 2 go to Dragon4 and print 9.007199254740992e+15 and
    9.007199254740994e+15.
- **Timing,** per 5000 renderings: 3.0 and 100000.0 went from 0.04 s to 0.01 s, the process start alone. The tie
  stays at 0.15 s.
- **Bands:**
  - float-repr 127 on all four kernels; float-repr-edges 31.
  - Every lane band reads as registered; value-str 1023, str-to-int-reading 127, once-hold 15.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **Fractional doubles still take Dragon4.** The tie costs about 26 µs; the native costs about 3. A 64-bit fast
  path for doubles whose digits fit in one word is that rung.
- **d4-scale still steps once per decade.**
- **epic-edison's str_eq guard (guard 1)** waits on a call-arm fix. A kind-35 leaf reached through the call arm
  read stale string words ("abc" against "abd" answered equal); the fix gives such leaves a signature bit that the
  call arm honors, as it does for consing callees.
- **value_kind_code** waits on Urs.
- The open items of receipts 12 to 55 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the cheapest win was a class of values that never needed the algorithm. Below 2^53 a whole
double's digits are its shortest rendering. Proving it takes no bignum, only the width of a unit.

The discomfort was the hour before this one, spent on an estimated quotient digit that came out correct and
slower. Stepping back to ask which values need Dragon4 at all found a class that does not. The gold is a four-line
path that is exact by a one-sentence argument, with a band bit on all four kernels.

Frontier word, row 1518: **exactwhole**, a double that is a whole number below 2^53, whose own digits are already
its shortest rendering.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
