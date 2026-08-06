# Receipt — cursor-seed P1: the Form meta-evaluator IS the reducer, on metal (2026-06-29)

**Claim.** A Form recipe runs through `form-eval` (the Form meta-circular evaluator),
carried by the **flattened cursor seed** on fkwu's **C table-executor** (`fk_walk`),
**not** through the C `--src` source parser (`fk_sparse`). The form-native reducer
path is real and witnessed on Mac metal.

This is the SEED-DROP pivot (`flatten/SEED-DROP.md`): running source through
`form-eval` (Form) instead of the C source-runner. The reducer becomes Form; the C
seed stays minimal (a table loader + walker).

## What was built

`flatten/form-eval-cli-loop.tbl` — the committed **cursor seed**: the flattened
`form-eval-cli-loop` REPL (`nf=153` functions, 39,501 bytes), platform-neutral
numeric data (the `fk_next` token stream: `nf fn[] nr nodes×4 ns strings`).

Flattened from these sources (string-pool `fks` table):
- mods: `fourth-shim.fk` (core mirror) + `input-stream.fk` + `form-eval-full.fk`
- band: `form-eval-cli-loop.fk` (`fn[0]` = `(fecl-repl)`)

`form-eval-cli-loop` reads `.fk` source off the staged input (`input_byte`, tag 17),
evaluates each line through `form-eval-full`'s BMF-cursor tree-walk (`fef-eval`), and
prints the value (`print_str`, tag 115). No per-recipe flatten — the recipe is reduced
**as it is read**.

## How it was produced (bootstrap, named honestly)

The seed was flattened by **bin-go** (`form-kernel-go/bin-go`, the Coherence-Network
sibling repo's Go bootstrap flattener — the allowed bootstrap, **not** the runtime).
bin-go runs the Form flattener (`form-flatten.fk` + `fkc-table-serialize.fk`'s
`fks-table-file`) over the sources. The flattener IS Form; bin-go is only its executor,
exactly as the fourth-arm band tables were always produced. The seed it emits is
platform-neutral data — the same bytes intern the same NodeIDs on every kernel.

The two fkwu-native flatten paths are both blocked today (named under "Where the
circle currently breaks"), so bin-go is the standing bootstrap for producing the seed.
Producing the seed is bootstrap; the **gate is the reduction**, which uses neither
bin-go nor `fk_sparse`.

## The hard gate — witnessed

```
# fkwu <table> 0 <recipe>  — the C table-executor (fk_walk) path
./fkwu flatten/form-eval-cli-loop.tbl 0 recipe.fk

recipe                                         seed (form-eval)   --src (fk_sparse)
(add 40 2)                                  => 42                 42
(sub 50 8)                                  => 42                 42
(if (le (add 40 2) 50) (sub 50 8) 0)        => 42                 42
(do (let x 40) (add x 2))                   => 42                 2     ← see below
(do (defn dbl (n) (mul n 2)) (dbl 21))      => 42                 42
```

`form-eval` is a **correct** reducer for what it covers: every result matches the
four-way / `--src` answer for the recipes it handles.

**The `let` case is the sharpest witness.** `(do (let x 40) (add x 2))`:
- via the cursor seed (`form-eval-full`) → **42** (correct)
- via the C `--src` parser (`fk_sparse`) → **2** (the C parser drops the binding — a bug)
- four-way oracle (Go=Rust=TS=fkwu, `fef-eval` band) → **42**

The seed path and the `--src` path are **demonstrably different reducers** with
different behavior, and the Form one is the correct one. That difference is itself the
proof the reduction did not ride `fk_sparse`.

## Structural proof: the reduction never touches `fk_sparse`

In `runtime/fkwu-uni.c`, `fk_run` dispatches:

```c
if (argc >= 3 && argv[1][0]==45 && argv[1][1]==45) { return fk_run_src(argv[2], ...); }  // the -- branch
int fd = open(argv[1], 0); ...  // the TABLE-LOADER branch (fk_walk over fk_fn[0])
```

`fk_sparse` (the C S-expr source parser) is called **only** inside `fk_run_src`, which is
reached **only** via the early-return `--` guard. Our invocation
`fkwu flatten/form-eval-cli-loop.tbl 0 recipe.fk` has `argv[1] = "form-eval-cli-loop.tbl"`
(not `--…`), so `fk_run` takes the `open(argv[1])` table-loader branch and walks
`fk_fn[0]`. `fk_run_src`/`fk_sparse` are never entered. The recipe's bytes reach the
program only through `fk_src` / `input_byte` (tag 17), read by the flattened `form-eval`
cursor.

## What `form-eval` handles today (Phase-1 coverage)

From `form-eval-full.fk` (`fef-*`), four-way proven (`form-eval-full-band` → 131):

- **Literals**: non-negative integer literals (digit-led tokens).
- **Arithmetic / compare binops**: `add`, `sub`, `mul`, `le`, `eq` (two operands each).
- **Conditional**: `(if cond then else)`.
- **Sequencing**: `(do e1 e2 …)`, threading the environment, value = last.
- **Binding**: `(let name value body)` — lexical, body sees the binding.
- **Definition**: `(defn name (params) body)` — binds a fn (params + body source position).
- **User calls**: `(name arg…)` — eval args in caller env, zip params, eval body source
  in a fresh env over the defn binding (recursion sees itself).
- **Symbol reference**: a non-digit token resolves via env lookup.

The core grammar (`form-eval.fk`, `fef`→`fe`) is the same shape minus `do/let/defn/calls/mul/eq`.

## The Phase-2 worklist (what the C `--src` parser does that `form-eval` does not yet)

`fk_sparse` + `fk_run_src` cover a wider surface than `form-eval-full`. The gap, to be
closed recipe-by-recipe in Phase 2:

1. **String literals + the string op family** in evaluated position (`str_concat`,
   `substring`, `str_eq`, …) — `form-eval` uses strings internally but does not yet
   *evaluate* string-valued recipes.
2. **Negative / float literals** — `form-eval` reads non-negative ints only.
3. **List / cell constructors as values** (`list`, `cons`, `head`, `tail`, `nth`) in
   evaluated position.
4. **The native op surface** (substrate node ops `intern_node`/`node_value`/`bp`,
   host-io `read_file`/`print_str`, figure ops, NodeID ops) — `form-eval` dispatches a
   curated keyword set, not the full `fkwu-optable.h`.
5. **`true`/`false` and the first-class `nothing`** literal in evaluated position.
6. **Multi-line / multi-defn programs as a single expression** beyond the line-REPL
   loop (the loop reads line-by-line; nested multi-line forms within one logical line
   are bounded by `form-eval-full`'s recursive-descent, but the staged-input line reader
   splits on `\n`).
7. **Tail-call / deep-recursion depth** — the tree-walk recurses on the C pthread stack
   (256 MB default); `--src` has the same bound. (Stone S11 TCO closes this generically.)

These are the named openings, not debts: each is a closing recipe — extend `form-eval`'s
dispatch, prove four-way, and the seed grows to cover it.

## Where the circle currently breaks (the two fkwu-native flatten paths)

Neither fkwu-native flatten path produces the seed today, so bin-go remains the
bootstrap:

1. **fkwu `--src` flatten** — `runtime/fkwu-uni.c`'s `--src` substrate node ops are
   incomplete: `(node_value (intern_node …))` and `(bp "…")` return `0` (where
   `intern_node`/`node_children` work). The flattener's `fk-tag = node_value(child)`
   therefore collapses to 0 and the whole flatten yields an empty table. Closing this —
   making `--src` carry the full substrate node ABI (`node_value`, `bp`) — lets fkwu
   flatten its own seed with no Go.
2. **fkwu + committed `T_flat`** (`flatten/fourth-flatten-table.txt`) — `fn[0]` is a
   `RESERVE`-wrapped function returning a constant *without* reading the request
   (`read_line` walk-count 0), so it is **not** the flatten-driver entry. The committed
   table flattens nothing as shipped (corroborated by
   `receipts/2026-06-29-windows-flatten-reground.md`). Re-emitting `T_flat` with the
   driver `(do …)` at `fn[0]` is the other fkwu-self-derivable route.

Either fix makes the seed `fkwu`-self-derivable (`flatten/README.md`'s stated
direction). Until then, bin-go is the named bootstrap, the seed is a regenerable cache,
and the **reduction** — the thing under proof — is already fully Form on the
table-executor.

## Honest floor (standard-receipt coordinates)

- **body**: ✓ the reducer is Form (`form-eval-full.fk`), four-way proven (131).
- **c-bootstrap**: ✓ run on the cc-seed `fkwu` (`cc -O2 -o fkwu runtime/fkwu-uni.c`).
- **toolchain-free reduction**: ✓ the reduction uses neither Go nor `fk_sparse` —
  table-executor + `input_byte` + `form-eval` only.
- **toolchain-free seed-production**: ✗ pending — the seed is flattened by bin-go
  (bootstrap); fkwu-self-derivable flatten is the named next step.
- **platforms**: mac **observed**; windows/android **pending** (the seed is
  platform-neutral data — the Windows kernel is proven ready per
  `windows-flatten-reground.md`; it runs the committed seed on first pull).

The form-native reducer path is proven on Mac metal: a recipe reduces through
`form-eval` on the table-executor, correct, demonstrably not `fk_sparse`.
