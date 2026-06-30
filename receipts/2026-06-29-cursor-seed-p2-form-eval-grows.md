# Receipt — cursor-seed P2: form-eval grows, op-dispatch becomes DATA (2026-06-29)

**Claim.** The Form meta-evaluator `form-eval` (`form-eval-full.fk`) now covers the next chunk of
the reducer surface `fk_sparse` handles — nine new value ops (`div mod lt gt ge ne and or` + unary
`not`) and three literal keywords (`true false nothing`). The new constructs run THROUGH the
re-flattened cursor seed on fkwu's C **table-executor** (`fk_walk`), **not** through the C `--src`
source parser (`fk_sparse`). And the value-op dispatch is now **DATA** — a single op table — not a
hand-written `if`-chain. Witnessed on Mac metal, four-way proven (635), zero P1 regression.

This is the Phase-2 increment named at the end of P1
(`receipts/2026-06-29-cursor-seed-p1-form-eval-reducer.md`).

## Where the grammar was raised (the HIGH-GRAMMAR lift)

P1's `form-eval-full.fk` had the binop surface as **two parallel hand-written if-chains**:
`fef-isbinop` (a 5-deep `if (str_eq op "add") 1 …`) tested membership, and `fef-apply`
(another 5-deep chain) computed the result. Two places to edit per op; the op set was code.

P2 replaces both with **one table as DATA** — exactly the shape `flt-ops` is (a tag→action map):

```
(defn fef-optable ()
    (list (list "add" 1 2) (list "sub" 2 2) (list "mul" 3 2) (list "div" 4 2)
          (list "mod" 5 2) (list "le" 6 2) (list "lt" 7 2) (list "gt" 8 2)
          (list "ge" 9 2) (list "eq" 10 2) (list "ne" 11 2) (list "and" 12 2)
          (list "or" 13 2) (list "not" 14 1)))
```

Each row is `(name tag arity)`. **One** table now drives BOTH:
- the dispatch test — `fef-oparity name` walks the table; `=2` → binary, `=1` → unary, `0` → not an op
  (so `fef-bykey` routes by arity, no per-op branch);
- the application — `fef-apply` reads the row's `tag` and calls `fef-apply-tag`, the single primitive
  switch (one branch per primitive — the irreducible floor: a primitive's machine action cannot itself
  be data without an eval-native apply, which is a later stone).

Adding `div`/`mod`/`lt`/`gt`/`ge`/`ne`/`and`/`or`/`not` was **rows + one apply-tag branch each**, never
two parallel if-edits. Literals are a second `(name value)` table (`fef-littable`):
`(list (list "true" 1) (list "false" 0) (list "nothing" 0))` — a new literal is a row.

**What stayed a keyword cond, on purpose.** The CONTROL forms `if`/`do`/`let`/`defn` are NOT value ops:
they thread the environment and the source cursor position in ways a uniform `(eval-args, apply)` table
cannot express (an `if` must not evaluate both branches; `let` extends env; `defn` skips body source).
They remain a small explicit cond in `fef-bykey`. The value ops + literals — the part that IS uniform —
are the data-driven surface. That split is the honest line between grammar-as-data and grammar-as-control.

## The hard gate — witnessed on metal

Same fkwu binary (`cc -O2 -o fkwu runtime/fkwu-uni.c`), re-flattened cursor seed
(`flatten/form-eval-cli-loop.tbl`, now **nf=171**, +18 fns over P1's 153, 46,387 bytes):

```
# fkwu <seed> 0 <recipe>  — the C table-executor (fk_walk) path, NOT --src
(div 85 2)                 => 42      (mod 142 100)             => 42
(if (lt 3 5) 42 0)         => 42      (if (gt 5 3) 42 0)        => 42
(if (ge 5 5) 42 0)         => 42      (if (ne 1 2) 42 0)        => 42
(if (and 1 1) 42 0)        => 42      (if (or 0 1) 42 0)        => 42
(if (not 0) 42 0)          => 42      (if true 42 0)            => 42
(if false 0 42)            => 42      (if (eq nothing 0) 42 0)  => 42
```

**Zero P1 regression** — the P1 floor still reduces on the same seed:
`(add 40 2)`=42, `(sub 50 8)`=42, `(do (let x 40) (add x 2))`=42, `(do (defn dbl (n) (mul n 2)) (dbl 21))`=42.

## Proof it rode form-eval, not fk_sparse — a double witness

P1's `let`-divergence witness no longer holds: kernel commit #41 fixed the `fk_sparse` `let` bug, so
`(do (let x 40) (add x 2))` is 42 on both paths now. P2 supplies two fresh, independent witnesses:

1. **`ne` differs between the two reducers.** `(ne 1 2)` → **seed = 1** (correct), but
   `--src`/`fk_sparse` → **`nothing`** (the C parser has no `ne` op). The seed path computes a
   *different, correct* answer than `fk_sparse` — it cannot be riding `fk_sparse`.

2. **New seed ≠ old seed, same kernel.** `(div 85 2)` and `(if true 42 0)` → **42** on the P2 seed,
   but **EMPTY** (unhandled) on the committed **P1** seed — with the *same fkwu binary*. Only the table
   changed, so the new behavior is the re-flattened `form-eval-full.fk`, not anything in C.

**Structural proof (unchanged from P1, re-verified against current `runtime/fkwu-uni.c`).**
`fk_run` enters `fk_run_src` (the only caller of `fk_sparse`) **only** when
`argv[1][0]==45 && argv[1][1]==45` (i.e. `--`). Our invocation `fkwu <seed> 0 <recipe>` has
`argv[1] = "<seed>"` (not `--`), so `fk_run` takes the `open(argv[1])` table-loader branch and walks
`fk_fn[0]`. `fk_run_src`/`fk_sparse` are never entered; the recipe's bytes reach the program only
through `input_byte` (tag 17), read by the flattened `form-eval` cursor.

## Four-way

`tests/form-eval-full-band.fk` (sibling `form/form-stdlib/tests/`) grown to **635** =
131 (P1: let+mul, do+defn+call, nested call) + 12×42 (the nine new ops + three literals).
`./validate.sh core.fk form-eval-full.fk form-eval-full-band.fk` → **635, fkwu = Go = Rust = TS,
1 ok / 0 divergent**. (The first attempt showed a TS rparen + a 45 from fkwu — a paren miscount in MY
sources, not a kernel divergence; fixed, then 635 four-way clean.)

## What form-eval now covers vs the remaining fk_sparse surface

**Covered (P1 + P2):** non-negative int literals; symbol reference; control `if`/`do`/`let`/`defn` +
user calls (recursion); value ops `add sub mul div mod le lt gt ge eq ne and or` + unary `not`
(table-driven); literal keywords `true`/`false`/`nothing` (table-driven, `nothing` as the 0-floor).

**Remaining to retire `fk_sparse` (the next Phase-3 increment, named honestly):**
- string literals + the string op family (`str_concat`, `substring`, `str_eq`, …) in evaluated position
  — `form-eval` uses strings internally but does not yet *evaluate* string-valued recipes;
- negative / float literals;
- list / cell constructors as VALUES (`list`, `cons`, `head`, `tail`, `nth`) in evaluated position;
- the native op surface in evaluated position (`intern_node`/`node_value`/`bp`, host-io
  `read_file`/`print_str`, NodeID/figure ops) — `form-eval` dispatches a curated set, not the full optable;
- **first-class `nothing`** — P2 carries `nothing` as the 0-floor (a value the reducer can hold);
  the cell-level absence sentinel (`fk_nothing`, stone 2a) in evaluated position is the next step;
- multi-line / multi-defn programs as one expression beyond the line-REPL loop;
- tail-call / deep-recursion depth (stone S11 TCO closes this generically).

Each is a closing recipe — for value ops, add a table ROW; for control or new literal kinds, a keyword
form — then prove four-way and re-flatten the seed.

## Seed production — still bin-go (unchanged from P1)

The seed is flattened by **bin-go** (`form/form-kernel-go/bin-go`, the bootstrap flattener — not the
runtime) over `fourth-shim.fk` + `input-stream.fk` + `form-eval-full.fk` + band `form-eval-cli-loop.fk`,
via the Form flattener `fks-table-file`. The two fkwu-self-derivable flatten paths named in P1
(fkwu `--src` substrate-node ABI; re-emitting `T_flat` with a driver `fn[0]`) remain the unlock to drop
bin-go. Producing the seed is bootstrap; the **gate is the reduction**, which uses neither bin-go nor
`fk_sparse`.

## Honest floor (standard-receipt coordinates)

- **body**: ✓ the grown reducer is Form (`form-eval-full.fk`), four-way proven (635), op-dispatch is data.
- **c-bootstrap**: ✓ run on the cc-seed `fkwu` (`cc -O2 -o fkwu runtime/fkwu-uni.c`).
- **toolchain-free reduction**: ✓ the reduction uses neither Go nor `fk_sparse` — table-executor +
  `input_byte` + `form-eval` only.
- **toolchain-free seed-production**: ✗ pending — bin-go flattens the seed (bootstrap); fkwu-self-derivable
  flatten is the standing next step.
- **platforms**: mac **observed**; windows/android **pending** (the seed is platform-neutral data — it
  runs the committed seed on first pull).
