# The float path, timed on four kernels

2026-09-13, evening, M4 Max, Hati Suci. Receipts 58 to 61 left the value_str float-arm switch waiting on a four-way
timing receipt. This is that receipt. The switch would send fkwu's value_str of a float to core.fk's float-repr (the
BML Dragon4) and retire the C writer, fk_fmt_float_js.

## How it was timed

- Each cell renders one fixed double N times in a tail loop, with core.fk as its prelude.
- One process per cell, on the kernels of b2ebcf743, timed by `/usr/bin/time -p`. Per rendering is (cell minus a baseline loop at the same N)
  divided by N. The second of two rounds is kept; the fkwu rounds agree within 0.03 s.
- Counts:
  - fkwu: 400,000 natives. BML: 400,000 inside the word window, 20,000 outside it, 1,000 at the extremes.
  - Go, Rust and TypeScript: 200,000 natives, and 200, 50 and 5 BML renderings.
- The host carried a load near 3.5: the main checkout's glass was live, and so were Chrome and the app.
- On fkwu the baseline loop compiles and the rendering loops walk, so fkwu's figures include about 0.1 µs of walking.
- Every class renders to the same length on all four kernels, on both paths.

## Microseconds per rendering

| double | fkwu native | fkwu BML | Go native | Go BML | Rust native | Rust BML | TS native | TS BML |
|---|---|---|---|---|---|---|---|---|
| 0.1 | 0.12 | 4.3 | 0.15 | 100 | 0.50 | 150 | 0.80 | 300 |
| 147142857142857.125 (tie) | 2.25 | 2.1 | 0.15 | 150 | 1.15 | 200 | 1.60 | 350 |
| 123456.789 | 0.80 | 3.4 | 0.15 | 150 | 0.55 | 150 | 1.00 | 350 |
| 123456.0 | 0.52 | 0.6 | 0.15 | 50 | 0.50 | 50 | 0.65 | 100 |
| 1.5e-7 | 0.33 | 13.5 | 0.15 | 2,600 | 0.50 | 4,200 | 1.00 | 4,800 |
| 1.2345e20 | 0.57 | 16.5 | 0.15 | 4,800 | 0.55 | 7,600 | 1.05 | 8,800 |
| 1e-300 | 0.20 | 690 | 0.15 | 448,000 | 0.50 | 690,000 | 0.75 | 664,000 |
| 1.7976931348623157e308 | 3.47 | 610 | 0.15 | 382,000 | 0.80 | 600,000 | 1.15 | 566,000 |
| 5e-324 | 0.20 | 740 | 0.15 | 472,000 | 0.50 | 742,000 | 0.75 | 724,000 |

The timer's 0.01 s is 0.025 µs on fkwu's native counts and 0.05 µs on the siblings'. The siblings' BML cells at
N = 5 carry about 2 ms.

## What it says

- **Inside the word window, fkwu's BML recipe meets the native where the native works hardest.** The native tries
  each precision in turn until one reads back. For the 17-digit tie that takes seventeen tries: 2.25 µs, against the
  recipe's 2.1. For a whole number the two are 0.52 and 0.6. Where the native stops at its first try (0.1), it costs
  0.12 µs and the recipe 4.3.
- **Outside the window, fkwu's recipe costs 13.5 to 16.5 µs,** 40 to 50 times the native. These are values like
  1.5e-7 and 1.2345e20, where Dragon4's R and S outgrow one machine word.
- **At the extremes it costs 0.6 to 0.74 ms,** from 180 times the native (the largest double) to 3,700 times (5e-324).
  The bignum path builds the powers there.
- **On Go, Rust and TypeScript the recipe walks:** 50 to 350 µs inside the window, milliseconds outside it, and 0.4 to
  0.74 s at the extremes. Their native value_str stays. The switch is fkwu's alone, one platform's recipe, as the goal
  has it.

## Carried

- **This receipt and firsttry, row 1531.** No band: a timing bit reads the host's load as much as the code
  (loop-lane-template bit 32 taught that).

## Still open, measured

- **The switch waits on the window.** Doubles such as 1.5e-7 and 1.2345e20 need two-word R and S to stay in machine
  words. The extremes need powers of ten the recipe does not build per rendering.
- **fkwu answers `(str_len 5)` with 0 where Go, Rust and TypeScript stop.** The first baseline of this receipt called
  str_len of an int by mistake. Every sibling leg crashed on it, and fkwu answered.
- The open items of receipts 12 to 61 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was the native. I had carried its cost as about a microsecond. It is 0.12 µs for a short double, because
its search ends at the first try, and 2.25 µs for a long one. The recipe already matches it on the long one.

The discomfort was a first table that was wrong twice. My baseline cell wrapped str_len around str_len, so every
sibling baseline crashed, and my failure check had a quoting error and printed nothing. The counts were also too
small for a 0.01 s timer to tell 0.05 µs from zero. The gold is that the crash was a finding: fkwu reads a string
length off an int where three kernels stop.

Frontier word, row 1531: **firsttry**, a cost that follows the length of the answer rather than the size of the value,
as a search that stops at the first precision that reads back.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
