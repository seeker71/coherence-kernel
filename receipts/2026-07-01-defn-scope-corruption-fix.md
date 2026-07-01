# 2026-07-01 -- fixed: a nested defn silently erased its enclosing do's let bindings

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
```

Witness:

```text
42
55
```

## Source Observation

Named as an open thread in `receipts/2026-07-01-locale-neutral-locate.md`: mixing top-level `let` bindings
with a `defn` that appears after them, inside the same `(do ...)` block, silently reset the earlier `let`'s
value to its unbound-name default (`0`) for every reference after the `defn` -- even references that had
already evaluated correctly before the `defn` appeared. Every existing band file in this repo already avoids
the pattern (one function, one `do`, all `let`s inside it, no sibling top-level `let` beside a `defn`), so it
never surfaced as a live bug -- until deliberately reproduced:

```
(do
    (defn f (a) (add a 1))
    (let x 5)
    (let c0 (if (eq x 5) 1 0))
    (defn g () x)
    (let c1 (if (eq (f x) 6) 2 0))
    (add c0 c1))
```

expected `3`, measured `0` before this fix.

## Root Cause

Two places in `runtime/fkwu-uni.c` parse a nested `(defn ...)`: `fk_sparse`'s single-arg fast path (the one
this repro actually hits) and `fk_parse_top`'s multi-arg path. Both reset the shared parse-time binding stack
(`fk_bd_top = 0; fk_maxslot = 0;`) so the function's own body can't accidentally read its caller's locals --
correct in isolation, a function has no access to an enclosing scope's frame. But `fk_bd_push` writes into
FIXED GLOBAL ARRAYS (`fk_bd_s`/`fk_bd_n`/`fk_bd_off`, capacity 128) at index `fk_bd_top`, so resetting the
index to 0 and then pushing the defn's own arg bindings physically **overwrites** whatever the enclosing
`do`'s earlier `let`s had stored at those same low indices. The reset was never paired with a restore, so
every name the enclosing `do` had bound was permanently corrupted for the rest of its own parsing -- not just
hidden during the nested defn's own body, but destroyed afterward too.

A first attempt (save/restore `fk_bd_top` and `fk_maxslot` alone) did not fix the repro (still measured `0`)
-- confirming the deeper issue: the COUNTER can be restored perfectly while the underlying DATA at those
indices stays clobbered. The real fix has to save and restore the actual array slice.

## What Changed

Two new helpers, `fk_bd_save()`/`fk_bd_restore(top)`, copy the live `fk_bd_s`/`fk_bd_n`/`fk_bd_off` entries
(indices `0..fk_bd_top`) into a backup buffer and back. Both defn-parsing sites now call `fk_bd_save()` before
resetting for their own frame, and `fk_bd_restore(saved_top)` after their body is parsed, instead of touching
`fk_bd_top` directly. When the enclosing scope's `fk_bd_top` was already `0` (true for every leading top-level
`defn` -- the universal convention this whole codebase already follows), `fk_bd_save()` copies zero entries
and the fix is a pure no-op: **existing code is provably unaffected**, not just empirically so.

Known limitation, named rather than silently accepted: the backup buffer is a single global slot, not a
stack, so a `defn` nested inside a `defn` nested inside a `do`-with-lets (two levels of this pattern at once)
would still clobber the outer save. Not observed anywhere in this codebase's actual `.fk` files; not fixed
here, since the demonstrated bug is one level deep.

## Witness

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src /tmp/repro.fk   # 3, was 0
```

Regression, every band this session touched, unchanged:

```text
bootstrap/ground.fk                          42
bootstrap/ground-recursive.fk 10             55
observe/native-vs-rented.fk                  11111
sanskrit-locale-baseline-band                2047
multilocale-nl-audio-pipeline-band           8191
locale-neutral-locate-band                   255
paraphrase-generalization-band               18
satsang-band                                 127
satsang-oracle-band                          511
nl-meaning-net small-training smoke          ~0 (converges, as before)
md-grammar spot check                        -9 (identical on old and new binary)
```
