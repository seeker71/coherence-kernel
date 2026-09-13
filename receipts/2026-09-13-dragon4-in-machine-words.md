# Dragon4 in machine words

2026-09-13, afternoon, M4 Max, Hati Suci. After the inliner fix (receipt 57), the 64-bit path for float-repr
could run hot. This rung puts it in core.fk.

## Carried

- **float-repr runs Dragon4 in machine words while its numbers fit.** The d4w-* functions keep R, S and the
  margins as plain integers under 2^59, so every product of ten and every sum stays under 2^63.
  - The digits gather in one integer, so the last digit's carry is plain addition.
  - `fl` packs two flags, whether m is even and whether m sits at its binade's boundary. That keeps the digit loop
    within six slots.
  - A scaling step that would pass the bound answers the empty list, and the value goes to the bignums (d4-from).
- **The mantissa is found once** (fr-me). fr-route sends m·2^e to the words when d4w-range? holds (e between
  bdy + 1 − 58 and 3, which is about 0.03 to 2^56), and to d4-from otherwise.
- **float-repr-edges-band gains bit 32 and reads 63, registered.** Bit 32: after 2000 renderings of the tie,
  d4w-run and d4w-scale read state 2.
- **The form-cli bootstrap follows.**
- **wordfit is row 1520.**

Witnessed:
- **Correctness:** 0 mismatches against value_str on the families (every 2^k, the odd multiples, the subnormals,
  10^k, 1,200 LCG doubles), 40,002 whole numbers and 4,000 random ones. float-repr-band reads 127 on all four
  kernels.
- **Timing:** old and new core.fk run back to back, second run of each, host load about 8.5:

  | float-repr of | bignum path | machine words |
  |---|---|---|
  | the tie (5000) | 0.16 s | 0.02 s |
  | 123456.789 (5000) | 0.10 s | 0.03 s |
  | 3.14159 (5000) | 0.08 s | 0.03 s |
  | 1e-300 (200) | 0.17–0.18 s | 0.17 s (still bignums) |
  | 5e-324 (200) | 0.18–0.19 s | 0.17–0.19 s (still bignums) |

  value_str's native takes 0.01 to 0.02 s for the same 5000. A 0.01 s run is mostly process start.
- **Bands:**
  - Every lane band reads as registered.
  - value-str 1023, str-to-int-reading 127, once-hold 15.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **The tie now costs about 2 µs per rendering, within reach of the native.** Whether value_str's float arm moves
  to the recipe is its own rung: a four-way timing receipt against the native, then the switch.
- **Doubles below about 0.03 and above about 2^56 still take the bignums.** Widening the window needs S in two
  words, or scaling R down first.
- epic-edison's guards 2 and 3 are under way.
- value_kind_code waits on Urs.
- The open items of receipts 12 to 57 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the algorithm did not change at all, only where its numbers live. For every double between
about 0.03 and 2^56, Dragon4's state fits in a machine word with room for a product of ten, and the same digits
came eight times faster.

The discomfort was that this path was written an hour earlier and hung once it ran hot, and the pull was to route
around the hang with an operand swap. Following the hang led to an old wrong answer in the inliner (receipt 57).
The gold is that the word path landed on a lane that answers right.

Frontier word, row 1520: **wordfit**, a value whose algorithm state fits in machine words with room for the next
step, so the same exact computation runs in plain integers.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
