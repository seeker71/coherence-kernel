# Data-driven source-walker — the last C-work, made permanent

**Date:** 2026-06-29
**Touches:** `runtime/fkwu-uni.c`, `runtime/fkwu-optable.h` (new, GENERATED),
`flatten/gen-source-walker-table.fk` (new), `flatten/gen-source-walker.sh` (new)

## The smell removed

`runtime/fkwu-uni.c`'s `--src` source-walker (`fk_sparse`) carried a hand-written
`if (fk_sym_eq(s, hn, "head")) ...` line **per op** — `head`/`tail`/`cons`/`list`/
`nth`/`str_eq`/`gt`/`lt`/`ge`/… — plus two hand-written name→tag / tag→arity tables
(`fk_optag`, `fk_oparity`). Every new op was another C edit. That is the
carrier-last inversion: the body's vocabulary frozen into the seed.

## The fix — one generic engine, ops as data

The single source of truth for `(name arity tag)` is **`flt-ops`** in
`flatten/form-flatten.fk` (itself GENERATED from
`form/form-stdlib/native-op-manifest.fk`). The flattener already reads it
generically via `flt-assoc`. The source-walker now reads the **same** data.

1. **`flatten/gen-source-walker-table.fk`** — a Form recipe that reads `flt-ops`
   (spliced in by the wrapper) plus a small rewrite-rule table, and emits
   `runtime/fkwu-optable.h`. The recipe's value *is* the header text;
   `flatten/gen-source-walker.sh` runs it on the bootstrap kernel and redirects
   stdout to the header (the only non-Form glue — a trivial capture, never a
   generator in Go). Adding a value op = a manifest row → a `flt-ops` row → regen,
   **never a C edit**.

2. **`runtime/fkwu-optable.h`** (GENERATED, committed, do-not-hand-edit) — two tables:
   - `fk_optab[]` = `{name, arity, tag}` primitive rows (138 rows, straight from `flt-ops`).
   - `fk_rwtab[]` = `{name, arity, nprog, prog[]}` rewrite rows (`gt ge lt eq and or not abs`),
     each an RPN lowering template.

3. **`fk_sparse` (C)** — the per-op if-chains are gone. The dispatch is now:
   control forms `defn`/`do`/`let`/`if` and the two structural literals
   `(empty)` / `(list …)` keep their own handling (their **shape** is not a flat
   `(tag arity)` emit); everything else flows through, in order:
   - `fk_rwtab_find` → `fk_rw_build` — generic RPN instantiator. The rewrite
     program is postfix over `ARG i` / `LIT v` / `NODE tag nkids`; one
     left-to-right pass with a small stack materialises the lowered tree. This is
     the **exact** vocabulary the flattener's `flt-low` uses (`if`/`le`/`sub` on
     tags 6/5/4, `lt`/`eq` on 103/102), now read as data.
   - `fk_optab_find` — read `(arity, tag)`, parse `arity` args, emit
     `fk_smknode(tag, …)`.
   - else: a registered `defn` name → call (tags 12 / 240).

`append` and any Form-defined op need **no** C — they are `defn` + the function
table already, confirmed below.

## The generic dispatch design

```
fk_rw_build(row, args): RPN over prog[]:
  0 i    -> push args[i]              (the i-th parsed operand)
  1 v    -> push fk_smklit(v)
  2 t n  -> pop n kids, push fk_smknode(t, kids...)
return top of stack
```

The rewrite rows lower exactly as the flattener does:

| op | lowering (flattener `flt-low`) | rwtab prog |
|----|-------------------------------|-----------|
| `gt a b` | `if (le a b) 0 1` | `a b NODE(5,2) 0 1 NODE(6,3)` |
| `ge a b` | `le b a` | `b a NODE(5,2)` |
| `lt a b` | tag 103 | `a b NODE(103,2)` |
| `eq a b` | tag 102 | `a b NODE(102,2)` |
| `and a b`| `if a (if b 1 0) 0` | `a b 1 0 NODE(6,3) 0 NODE(6,3)` |
| `or a b` | `if a 1 (if b 1 0)` | `a 1 b 1 0 NODE(6,3) NODE(6,3)` |
| `not a`  | `if a 0 1` | `a 0 1 NODE(6,3)` |
| `abs a`  | `if (le 0 a) a (sub 0 a)` | `0 a NODE(5,2) a 0 a NODE(4,2) NODE(6,3)` |

`abs` is a NEW rewrite (not in the flattener's curated set); it is added in the
**data** (the rewrite table), not as a C case.

## Gate (cc -O2 -o /tmp/fkwu runtime/fkwu-uni.c; `--src`)

**No regression** (unchanged from before):
```
(mul 6 7)                => 42
(head (list 11 22 33))   => 11
nth via fn               => 6
(str_eq "yes" "yes")     => 1
(str_eq "a" "b")         => 0
```

**Now FROM DATA** (every one of these returned `0` before — they fell through to
the unbound-symbol path because no C case handled them):
```
(abs (sub 3 7)) => 4     (abs 5) => 5      (abs 0) => 0
(and 1 1) => 1           (and 1 0) => 0    (and 0 1) => 0
(or 1 0)  => 1           (or 0 0)  => 0
(not 0)   => 1           (not 5)   => 0
(eq 7 7)  => 1           (eq 7 8)  => 0
(gt 9 4)  => 1  (gt 4 9) => 0  (gt 5 5) => 0   ← were hardcoded, now data
(lt 4 9)  => 1  (lt 9 4) => 0
(ge 4 4)  => 1  (ge 3 4) => 0
```

**`append` recipe via `defn` — no C:**
```
(do (defn append (a b) (if (eq (len a) 0) b (cons (head a) (append (tail a) b))))
    (defn sum (xs) (if (eq (len xs) 0) 0 (add (head xs) (sum (tail xs)))))
    (sum (append (list 1 2) (list 3 4))))                 => 10
```

**A real body source-only — `observe/native-vs-rented.fk`** (the README smoke
cell; `gt`/`eq`/`add`/`head`/`tail`/`list`/`if`), NO flatten, NO bin-go:
```
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
/tmp/fkwu --src /tmp/nvr.fk            => 11111
```
`observe/surprise-receipt.fk` likewise runs source-only → 11111.

## Hardcoded-op-chain count

```
grep -cE 'fk_sym_eq\(s, hn, "(head|tail|nth|str_eq|cons|gt|lt|ge|abs|and|append|...)"'
   over CODE lines (excluding comments)  => 0
```
The only `if (fk_sym_eq(s, hn, …))` chains remaining in `fk_sparse` code are the
control forms `defn`/`do`/`let`/`if` and the two structural literals
`empty`/`list` — exactly the design's allowed set. **Zero value-op chains remain.**

## Named gap (not this change)

`observe/presence-feature.fk` and `observe/same-room.fk` do **not** run
source-only — not because of any op, but because the `--src` walker's
function-call path supports calls of arity ≤ 2 (tags 12 / 240) and those bodies
call helpers with up to 8 args (`pf-sad-row`). That is a **pre-existing
call-arity limitation of the source-walker**, orthogonal to op dispatch, and is
left untouched here. Bodies within the ≤2-arg envelope
(`native-vs-rented`, `surprise-receipt`) run source-only and exercise the
data-driven ops/rewrites end to end.

## Regenerating

Pure Form, zero bash (the `gen-source-walker.sh` wrapper was composted —
see `receipts/2026-06-29-optable-regen-is-form-no-bash.md`). From the repo root:

```sh
fkwu --src flatten/gen-source-walker.fk        # splice flt-ops -> /tmp/gen-source-walker-combined.fk (pure Form)
fkwu --src /tmp/gen-source-walker-combined.fk  # generator writes runtime/fkwu-optable.h via write_file_text
cc -O2 -o /tmp/fkwu runtime/fkwu-uni.c
```
