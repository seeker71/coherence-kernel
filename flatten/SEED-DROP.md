# SEED-DROP - a flatten/`form-eval` door (no longer a gate; the body already runs via `--src`)

> **P1 WITNESSED (2026-06-29).** The cursor seed is committed — `flatten/form-eval-cli-loop.tbl`
> (flattened `form-eval-cli-loop`, `nf=153`) — and a recipe reduces through `form-eval` (Form) on the
> C **table-executor**, NOT the `--src` parser (`fk_sparse`): `fkwu flatten/form-eval-cli-loop.tbl 0 t.fk`
> with `(add 40 2)` → 42, `(do (let x 40) (add x 2))` → 42 (the same `let` via `--src` returns 2 — a
> `fk_sparse` bug, so the seed path is a demonstrably different, correct reducer). The seed is produced by
> bin-go (bootstrap flattener, named honestly — not the runtime) because both fkwu-native flatten paths are
> still blocked. Coverage + the Phase-2 gap + the broken-circle detail:
> `receipts/2026-06-29-cursor-seed-p1-form-eval-reducer.md`.

> **SUPERSEDED for the run path (2026-06-29).** The Form body already runs on `fkwu` **without any seed**: the
> kernel's own C-bootstrap source-runner, `fkwu --src file.fk`, runs real multi-function/list/recursion body cells
> (`observe/native-vs-rented.fk` -> `11111`, bit-identical to the proof walkers; no Go, no flatten, no T_flat). See
> `receipts/2026-06-29-src-stone5-real-cell-on-fkwu.md`. So "standing the body on Windows" is **done** via `--src`.
>
> This doc is retained only for the *other* door: running source through `form-eval` (the Form meta-evaluator)
> instead of the C source-runner. That door wants the small **cursor seed** (`form-eval-cli` + `form-eval` + the
> BMF cursor, ~740 lines, flattened ONCE) - a flatten *cache*, never a gate (`T_flat`/bin-go heavy chain stays
> deprecated; "flatten is optional speed" - HOMECOMING). It is optional: a speed/parity convenience, not a
> requirement for the body to run.

The kernel runs the Form body as Form today via `--src`. The seed below is the optional `form-eval` door - a
flattened `form-eval-cli-loop` table (platform-neutral numeric data) that lets `form-eval` itself run on a kernel
without exercising the C front-end. Useful, not required.

## What The Seed Is

The Form **source-runner**, flattened once. `agent/form-eval-cli-loop.fk` reads `.fk` source off the staged input
(`input_byte`) and evaluates it through the BMF cursor (`grammars/form-eval.fk` / `form-eval-full`) with **no
per-recipe flatten**. Its flattened table - the `fk_next` numeric stream (`nf, fn[], nr, nodesx4, ns, strings`),
the same format as `fourth-flatten-table.txt` - is the optional seed.

Preludes to flatten together (the cursor seed):
`core.fk` + `input-stream.fk` + `form-eval-full.fk` + `agent/form-eval-cli-loop.fk`
(or the minimal single-expression variant: `core.fk` + `form-eval.fk` + `agent/form-eval-cli.fk`).

It is **platform-neutral data** - flattened on any kernel, it runs on all of them (content-addressed; the same
table interns the same NodeIDs on mac, android, windows).

## The Drop

1. On the Mac (where the flattener stands): flatten the modules above into a table.
2. Commit it: **`flatten/form-eval-cli-loop.tbl`** (the numeric stream).

That's it. No Windows-specific work - the table is data.

## What Runs Without It

The body already runs through the direct source path:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk        # -> 11111
```

No committed flattened source-runner table is required for that witness. If a cell does not fit the current
`--src` surface, name the actual source/lowering coverage gap; do not call it a missing table seed.
