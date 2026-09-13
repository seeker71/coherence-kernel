# The twin that called the leaf

2026-09-13, midday, M4 Max, Hati Suci. While the radix rung ran its checks before landing, loop-lane-template-band read 31 where
it is registered at 63. I ran it again on the kernels from before today's rungs, and it did the same there.

## Carried

- **Bit 32's twin walks the work it is timed against.** Bit 32 asks for three signatures of one defn native at
  once: an int, a float and a defn first parameter. It witnesses "native" by timing the int and float runs
  against a walker twin. That twin walked its own driver loop but called the compiled `ltb-g` for the list walk
  itself. So both sides ran the same leaf: 3 ms against 3 or 4, and `lt` went either way. `ltb-g-walk` now walks
  g in the walker, and the twin takes about 130 ms against 1 and 3.
- **halftwin is row 1515.**

Witnessed:
- **Before the heal:**
  - On d5099abf1, dd59ab024 and the current kernel, the band read 31 on about half of six runs each.
  - Printing the three timings (int, float, twin) gave 1, 3, 3 on a run that read 31, and 1, 3, 4 on the runs
    that read 63.
- **After:** ten runs, all 63. Preflight is clean.
- **Before landing:** freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- float-repr's common case is still Dragon4's per-digit bignum work: the tie costs about 36 µs against the
  native's 3. A 64-bit fast path is that rung, and d4-scale's k could come from the exponent (receipt 53).
- The open items of receipts 12 to 53 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the flake was never in the lane. The twin meant to walk handed its work to the very leaf it
was timed against, so the band had been timing the leaf against itself.

The discomfort was a 31 on a registered band in the middle of a landing, with the pull to call it host load. Six
runs on three kernels showed it wasn't mine, and printing the three timings named the cause in a single run. The
gold is a one-defn heal and a bit that now says what it witnesses.

Frontier word, row 1515: **halftwin**, a walker twin that walks its own loop but calls the compiled leaf for the
work being compared, so a timing against it measures the leaf twice.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
