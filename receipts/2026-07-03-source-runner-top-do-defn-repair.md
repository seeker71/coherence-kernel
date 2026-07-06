# 2026-07-03 -- source-runner top-level do defn-after-let repair

## Ground

The form-definition language layer exposed a source-runner red signal:

```text
(do (let rows (list 8 13 21)) (defn bump (x) (add x 1)) (bump 1)) -> 8
```

The expected value is `2`. The same function works when it appears before the
`let`, and it works when the `let` and `defn` are real top-level forms:

```text
(do (defn bump (x) (add x 1)) (let rows (list 8 13 21)) (bump 1)) -> 2
(let rows (list 8 13 21))
(defn bump (x) (add x 1))
(bump 1)
-> 2
```

This was not an OOM. No process was killed. It was a semantic source-runner
bug and was investigated instead of being ignored.

## Pre-Review

Grok reviewed the supplied evidence and accepted the repair direction as
coherent and proportionate. Conditions:

- pin the exact failing shape;
- prove defn-before-let still works;
- prove binding survives a defn interposition;
- record this as a bounded C-seed checkout-witness repair, not new runtime
  destination work.

Claude reviewed the supplied evidence and accepted the repair direction, with
the same core conditions plus:

- prove multiple defns after a value form;
- pin the nested boundary: transparent top-level nested `do` remains possible,
  while ordinary nested `do` through the value parser does not leak local
  `defn` into top-level definitions.

## Cause

`fk_parse_top` treats a top-level `(do ...)` as transparent until the first
value-bearing form. After that first value form, it previously routed the rest
of the sequence through ordinary `fk_parse_do`.

That was wrong for this shape:

```text
(do (let rows ...)
    (defn bump ...)
    (bump 1))
```

After the `let`, `fk_parse_do` parsed the later `(defn ...)` through the
ordinary value parser (`fk_sparse`) instead of the top-level definition path.
The prescanned function name and arity existed, but the function body was never
filled. The first repair draft also showed why saving only `fk_bd_top` was not
enough: parsing the interposed `defn` overwrote the binding stack entries
themselves. Restoring stack depth without restoring the live entries left
`rows` unavailable after the interposed definition.

## Implementation

`runtime/fkwu-uni.c` now has a top-level-do value parser used only from
`fk_parse_top`'s top-level `(do ...)` route:

- ordinary nested value-position `(do ...)` still uses `fk_parse_do`;
- top-level-do `let` still binds for the rest of that top-level sequence;
- a later top-level-do `defn` calls `fk_parse_top` so its prescanned function
  body is filled;
- while doing so, the repair snapshots and restores the live binding stack
  entries and `fk_maxslot`, so do-local bindings survive across definition
  interposition.

This is a C-seed checkout-witness repair. It does not add a language feature,
does not grow the primitive set, and remains shrink debt until the source
front door moves into the Form/native-walker compiler path.

## Witness

Build:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
```

The build succeeded with the existing `fread` header warning and
`getsockname` pointer-sign warning.

Required checkout witnesses:

```text
ground.fk                -> 42
ground-recursive.fk 10   -> 55
binary-freshness-band.fk -> 15
native-vs-rented-check   -> 11111
```

Focused band:

```text
source-runner-do-defn-band -> 127
```

Band bits:

```text
1   defn after a value let fills and calls: after(1) -> 2
2   a second defn after the value let fills: two(1) -> 2
4   the let binding survives interposed defns: head(rows) -> 8
8   defn before let in the same top-level do still works
16  multiple defns after the value form continue the sequence
32  leading nested top-level do remains transparent for definitions
64  ordinary nested value-position do does not overwrite a same-named global
    defn when parsed through the value parser
```

Targeted repros after repair:

```text
(do (let rows (list 8 13 21)) (defn bump (x) (add x 1)) (bump 1)) -> 2
(do (let rows (list 8 13 21)) (defn two (x) 2) (two 1))           -> 2
(do (defn bump (x) (add x 1)) (let rows (list 8 13 21)) (bump 1)) -> 2
```

Adjacent witnesses:

```text
source-runner-root-do-band       -> 31
source-runner-admission-band     -> 2097151
form-definition-language-band    -> 65535
form-definition-language-floor   -> 10
defdata-language-band            -> 8191
defdata-band                     -> 2047
bmf-grammar-band                 -> 2047
grammar-loader-band              -> 65535
source-artifact-cache-band       -> 1048575 (2026-07-03 witness; not
                                    re-claimed by the 2026-07-04 boundary pass
                                    because the current untracked band has a
                                    malformed prelude path)
bmf-core integration band        -> 600
form-definition copy cmp         -> 0
defdata-language copy cmp        -> 0
bmf-core copy cmp                -> 0
bmf-grammar copy cmp             -> 0
git diff --check                 -> clean
```

No OOM-killed process occurred during this repair pass.

## Deferred

- Full migration of source parsing out of the C seed.
- Closure semantics for functions defined after a do-local `let`. This repair
  fills the function body and preserves later do-local value bindings; it does
  not make a function body close over a do-local binding.
- Main source-compiler integration for the higher module language.
- Program-image `.fkb` and native `.dylib` artifact selectors.

## Post-Review

Grok post-review accepted the repair from supplied evidence. It found no
blocker and called the change a scoped checkout-witness repair, with the ongoing
C-seed shrink debt still named.

Claude post-review also accepted from supplied evidence. Claude noted that it
did not independently read or rerun the repo, and asked that the seven band
cases stay enumerated in the receipt. The band cases are decoded above.

2026-07-04 re-review found the original bit-64 witness depended on `nothing?`,
which direct `fkwu` can observe but the sibling validation kernels do not carry.
The band now defines a global `hidden`, keeps a nested same-named `hidden`
inside `local-probe`, and proves `(local-probe) -> 5` while the global
`(hidden 1)` still returns `101`. `./validate.sh
form-stdlib/tests/source-runner-do-defn-band.fk` now returns `127` across the
validation kernels.
