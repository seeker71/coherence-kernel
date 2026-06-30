# SEED-DROP — a flatten/`form-eval` door (no longer a gate; the body already runs via `--src`)

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
> (`observe/native-vs-rented.fk` → `11111`, bit-identical to the proof walkers; no Go, no flatten, no T_flat). See
> `receipts/2026-06-29-src-stone5-real-cell-on-fkwu.md`. So "standing the body on Windows" is **done** via `--src`.
>
> This doc is retained only for the *other* door: running source through `form-eval` (the Form meta-evaluator)
> instead of the C source-runner. That door wants the small **cursor seed** (`form-eval-cli` + `form-eval` + the
> BMF cursor, ~740 lines, flattened ONCE) — a flatten *cache*, never a gate (`T_flat`/bin-go heavy chain stays
> deprecated; "flatten is optional speed" — HOMECOMING). It is optional: a speed/parity convenience, not a
> requirement for the body to run.

The kernel runs the Form body as Form today via `--src`. The seed below is the optional `form-eval` door — a
flattened `form-eval-cli-loop` table (platform-neutral numeric data) that lets `form-eval` itself run on a kernel
without exercising the C front-end. Useful, not required.

## What the seed IS

The Form **source-runner**, flattened once. `agent/form-eval-cli-loop.fk` reads `.fk` source off the staged input
(`input_byte`) and evaluates it through the BMF cursor (`grammars/form-eval.fk` / `form-eval-full`) with **no
per-recipe flatten**. Its flattened table — the `fk_next` numeric stream (`nf, fn[], nr, nodes×4, ns, strings`),
the same format as `fourth-flatten-table.txt` — is the seed.

Preludes to flatten together (the cursor seed):
`core.fk` + `input-stream.fk` + `form-eval-full.fk` + `agent/form-eval-cli-loop.fk`
(or the minimal single-expression variant: `core.fk` + `form-eval.fk` + `agent/form-eval-cli.fk`).

It is **platform-neutral data** — flattened on any kernel, it runs on all of them (content-addressed; the same
table interns the same NodeIDs on mac, android, windows).

## The drop

1. On the Mac (where the flattener stands): flatten the modules above into a table.
2. Commit it: **`flatten/form-eval-cli-loop.tbl`** (the numeric stream).

That's it. No Windows-specific work — the table is data.

## What runs the instant it lands (mechanism already verified on Windows)

```
# the body runs as Form, off the cursor, no per-recipe flatten:
fkwu flatten/form-eval-cli-loop.tbl 0 <any-recipe.fk>

# verify:
printf '(add 40 2)' > t.fk
fkwu flatten/form-eval-cli-loop.tbl 0 t.fk        # -> 42

# then the real body cells run as Form on Windows:
fkwu flatten/form-eval-cli-loop.tbl 0 observe/sense-stream.fk
fkwu flatten/form-eval-cli-loop.tbl 0 observe/native-vs-rented.fk
...
```

The load path is witnessed ready: `fkwu <table> 0 <source>` loads the table and stages the source for
`input_byte` (confirmed on Windows 11, 2026-06-29 — `input_byte 0` over a staged file returned the first byte).

## What collapses the moment the seed is in

- Every `.fk` body cell runs as **Form** on Windows (sense-stream, presence-model, surprise-receipt,
  native-vs-rented, mesh-sense-7w, fused-observation, …).
- The hot ones **crystallize to native** via the wired JIT (`fk_native_call` dispatch).
- The pixel-walk lowers (`form-asm-x64`); the mesh fusion + oracle-economy go live.
- The **C scaffolds retire**: `fk_sense_stream`/`fk_frame_read` math, and the **bounded `--src` bootstrap parser**
  (stones 1–3) — their telos met, deleted in the same breath.

## Why this and not more C

The flattener and the source-runner ARE Form (`flatten/README.md`: "the real flatten body"). Standing them needs
their flattened *data*, not a C reimplementation. Growing the `--src` bootstrap to the full grammar would be the
carrier-last inversion (see `receipts/2026-06-29-stones-bounded-flattener-is-form.md`). The seed is the clean,
toolchain-free unlock — and it's one committed table away.
