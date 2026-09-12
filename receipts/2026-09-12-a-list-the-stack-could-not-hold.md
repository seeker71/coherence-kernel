# A list the stack could not hold

2026-09-12, evening, M4 Max, Hati Suci. Epic-edison's line comes home on top of this one, and a crash every kernel
carried surfaces once the shared field is reset.

## Carried

- **Rungs 10 and 11a and the lane-shaped core recipes come home**, cherry-picked as epic-edison wrote them:
  - 7c9b82fa7, a defn crosses as a value: map, filter and foldl crystallize, reducer and all;
  - c9ea30cc8, a float rides out of a head;
  - cb1d269ca, a float head compared everywhere;
  - 0c708262b, range and take turn to the lane's shape;
  - 9768cd58b, sum, product, maximum and minimum turn to the lane's shape.

  Their rows move past this line's two, each keeping a note of its first number: wordheat 1501 to 1503,
  floatlend 1502 to 1504, shapeturn 1503 to 1505. Rung 10's per-defn run memory and this line's hand-back at
  half a run merged without a conflict, and they compose.
- **The form-cli bootstrap carries them** (2284a48f7, stamp 62a20dcb28b63763). TestFkwuFormCliCanonicalCarrier
  passes with range, take and the four reductions in the lane's shape. This is the carrier test the last two
  commits were held for until the field was reset.
- **The melt walks a list's spine in a loop** (961a3fa41). fk_mlive and fk_mcopy recursed along a list's tail,
  one stack frame per cell. After the reset the field held 185,187 nodes where it had held 41M, and the melt's
  headroom follows that count, so melts came often. A live list of about four million cells then ran the
  evaluation thread's 256 MB stack out in the middle of a melt: rc 138, on e86210dc0's kernel as on this line.
  Both walks now loop along the tail and recurse into heads, as fk_smark already did. fk_mcopy places each cell
  and sets its forward before it copies the head. melt-long-list-band (FOURTH-ARM ONLY) holds a
  5,000,000-cell list through the melts its growth brings and through a second 5M build. It reads 3 here in
  0.34 s and stops with rc 138 on the walk it replaces. It peaks near 500 MB.
- **The form-cli bootstrap follows** (8642f8047).
- **cellframe is row 1506** (71bfc70d5).

Witnessed at 71bfc70d5:
- **Bands:** loop-lane-cons 511, loop-lane-closure 255, loop-lane-float-head 31, loop-lane-string-door 2047,
  and 255 on every other loop-lane band. arena-melt 63, closure-capture-melt 31, hati-os-heap 7. Every JIT band
  reads as rung 9 reads it. value-str 255, str-to-int-reading 127, once-hold 15, jit-lens 16383.
- **Answers:** list-build-diff-probe answers as e86210dc0's kernel does, all 21 lines. A loop carrying an int, a
  float, a string and a list answers as main did before the reset; main now stops on it with rc 138.
- **Wall clock, one process each against e86210dc0,** on the reunion's kernel before the spine loop, at load 37
  to 42: the four list shapes took 0.32 s against 0.56, and the edge probe 0.26 s against 0.29.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **Epic-edison's line keeps growing** past bb76c4a41; the commits after it come home in the next reunion.
  Their review of 0f47d16c6, the hand-back, is still owed.
- **A loop that both conses a fresh list and carries a list parameter** through its self call declines to the
  walker (named by epic-edison for the next rung).
- **A single pass that needs more than 4,194,304 pairs** still falls to the walker (receipt 46).
- **Lowering native-table-compile.bml interns more than 2 GiB of strings** (receipt 47). Its busiest recipes are
  flt-sidx (43.8M calls) and flt-assoc (12.7M), both string-keyed linear lookups the lane declines at str_eq.
- **value_str is still leaf mode 25 in C** (receipt 45).
- The open items of receipts 12 to 47 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that a cleanup of the whole host set how deep one program's stack goes. Resetting the
shared field shrank its node count, the melt's headroom shrank with it, and melts came more often. A recursion
that had never met a long list at melt time now met one, on every kernel at once.

The discomfort came right after five of epic-edison's commits arrived: the mixed probe's answers "differed", and
the pull was to bisect their rungs for what had broken the float lane. The first backtrace put the crash in
fk_mlive, on main's own kernel as much as on this line's; it was neither line's doing. The gold is a crash that
any long list could hit, closed at its walk, with a band that fails on the walk it replaced.

Frontier word, row 1506: **cellframe**, a walk that spends one stack frame per list cell, so the list's length,
not its nesting, sets how deep the stack goes.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
