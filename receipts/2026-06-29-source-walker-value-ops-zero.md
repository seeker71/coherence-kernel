# Receipt — the source-walker's hand-written VALUE-op count is TRUE ZERO (2026-06-29)

**The ask (Urs):** drive the cc-seed source-walker's hardcoded VALUE-op count to true zero.
The prior data-driven refactor (`166697a`) got it to the last hand-written value forms:
`(empty)` and `(list ..)`. Fold those into the data path too — including a generic
variadic mechanism for `list`, so it is a DATA row, not a hand-cased C branch.

## What was hand-written before, and why it's gone now

Two value forms still lived as `if (fk_sym_eq(s, hn, ...))` cases in `fk_sparse`:

- **`(empty)`** — emitted `fk_smknode(18, 0, 0, 0)` (the nil value). It was *not even
  in the optable* (only `head` was). Now it is a manifest row `(list "empty" 0 18)` in
  `flt-ops`; the generic arity-0 dispatch emits the identical node. The hand case is deleted.
- **`(list a b ..)`** — a hand-written right-fold `fk_parse_list` building nested cons.
  This needed a real mechanism: **the arity `-1` VARIADIC sentinel.** A row
  `(list "list" -1 19)` in `flt-ops` means *parse operands until the close paren and
  fold them right via the row's tag (cons/19), terminating in nil (tag 18)*. The generic
  dispatch detects `arity < 0` and calls `fk_parse_variadic(tag)`. `list` is now DATA;
  any future variadic structural form is another `(name -1 tag)` row, never a C edit.

`head` was already data (`(list "head" 1 20)`); the only remaining `"head"` reference was
a stale doc comment, now retuned so the gate grep is clean.

## The single source held

`flt-ops` (in `flatten/form-flatten.fk`) is the ONE table. The flattener special-cases
`(empty)`/`(list ..)` in `flt-form2` *before* `flt-op`, so these new rows never gate the
flatten path — they exist purely so the GENERATED `runtime/fkwu-optable.h` carries them as
data. `flatten/gen-source-walker-table.fk` (a Form recipe) emits the header; the
temporary capture splices flt-ops (now lines 50–105) into it. The C dispatch reads the row's arity:
`-1` → variadic fold, `0` → bare node, `1/2/3` → fixed-arity emit.

## Gate (cc -O2 -o /tmp/fkwu runtime/fkwu-uni.c; all via --src)

**The hand-written op list is now EXACTLY the four control forms — true zero value-ops:**

```
grep -oE 'fk_sym_eq\(s, hn, "[a-z_]+"' runtime/fkwu-uni.c | grep -oE '"[a-z_]+"' | sort -u
=> "defn" "do" "if" "let"
```

`defn`/`do`/`let`/`if` are CONTROL forms (special eval semantics) — they legitimately stay.
No value form remains hand-cased.

**No regression:**
- `(mul 6 7)` → 42
- `(head (list 11 22 33))` → 11
- `(empty)` → 1 (the nil value)
- `(nth (list 5 6 7) 1)` → 6
- `(abs (sub 3 7))` → 4
- `(and 1 1)` → 1
- `native-vs-rented.fk` → 11111 (real server cell, no flatten, no bin-go)

**Variadic `list` is data-driven across 0/1/N args:**
- `(len (list))` → 0, `(len (list 9))` → 1, `(len (list 1 2 3 4 5))` → 5
- `(head (list 9))` → 9, `(head (list 1 2 3 4 5))` → 1

**Full data loop closed:** the new kernel (with `list`/`empty` as data) regenerates the
byte-identical `fkwu-optable.h` via `flatten/gen-source-walker-table.fk` plus a temporary
host capture — proof that the generator recipe (which itself uses `(list ...)` heavily)
runs on the very kernel whose `list` is now data, not a hand-written case.

## The honest floor

This is the c-bootstrap fkwu source-walker reading its op vocabulary — control forms apart —
entirely as data. Observed on **mac** via `cc -O2` + `--src`. Windows/Android platform rows
remain pending (the per-platform receipt is unchanged by this fold). The carrier-last smell
in the seed is now fully composted for value forms: adding ANY value op (fixed or variadic)
is a manifest row + regen, never a C edit.
