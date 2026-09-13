# The slot the inline borrowed

2026-09-13, afternoon, M4 Max, Hati Suci. I was building float-repr's 64-bit fast path, which runs Dragon4 on plain
integers while they fit. Its scaling loop answered correctly when called once, and looped forever once it ran hot.
It did the same on every kernel back to d5099abf1.

## Carried

- **The inliner hands back the environment slot it borrows.** When an inlined body reads one of its parameters, the
  param arm admits the caller's argument one level down. If that argument is itself an inlinable call, the inner
  inline borrowed the same slot and left its own bindings in it. So the outer body's later parameter reads took the
  inner call's arguments. `fk_f64_inline_try` now saves the slot (its eight arguments and its arity) before writing
  it, and restores it after admitting the body, whether or not the body admits.
- **loop-lane-call-band reads 8191, registered.** Bit 4096 is new: `(lcb-ig 1 5 3)` answers 10001 hot, as the walker
  does. main's runtime without the fix reads 4095 on it.
- **borrowslot is row 1519.**

Witnessed:
- **The repro:** `(ec-g 1 5 3)` is `(ec-comb a (ec-pick b c))`, and ec-comb reads x after its inlined argument.
  - Before: on d5099abf1 and on main at ff35e2209 it answered 10001 cold and 10005 hot. x read b's 5 where a's 1
    belonged.
  - With the fix it answers 10001 hot, and 3000 driven calls sum to 30003000.
- **The fast path's scaling loop,** `(if (i4-up? R S (i4-mp mm fl)) …)`, has i4-up? read S after its inlined
  argument mp. It hung hot on three kernels; it now answers k = 15 hot.
  - Dropping the inlined argument cured it, and so did reading S first (`(lt S (add R mp))`). Changing `false` to
    `0` did not.
- **Bands:**
  - Every lane band reads as registered.
  - value-str 1023, str-to-int-reading 127, once-hold 15.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **float-repr's 64-bit fast path (scratchpad rp/fr6-defs.txt)** is next. It keeps R, S and the margins as integers
  under 2^59, and bails to the bignum path past that. It already sweeps with 0 mismatches cold. Hot timing waited on
  this fix.
- epic-edison's guards 2 (tree recursion with its depth wall) and 3 (kind-38 callees) are under way.
- value_kind_code waits on Urs.
- The open items of receipts 12 to 56 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that a hang in new code was an old wrong answer. The same slot left behind had been answering
wrong, silently, in any compiled leaf where an inlined helper reads a parameter after an inlined argument, since at
least d5099abf1. It took a loop to make the wrong value loud.

The discomfort was the pull to route around it. The bisect showed that reading S first cures the hang, and that one
operand swap would have made the fast path work and buried the miscompile under a lucky order. The gold is a fix of
a dozen lines in the inliner, and a band bit that main's kernel fails.

Frontier word, row 1519: **borrowslot**, the environment slot an inline borrows and hands back as it found it,
because an inline inside one of its arguments borrows the same slot.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
