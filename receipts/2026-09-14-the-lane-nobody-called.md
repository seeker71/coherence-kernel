# The lane nobody called

2026-09-14, afternoon, M4 Max, Hati Suci. Asked: bring the Rust-only recipe-library lane in line with
fkwu. Move the seedbank parity suite's Rust leg onto `./fkwu`, then release the lane.

## Carried

- **The Rust `list` / `execute` lane is released**: `cli_list`, `cli_execute`, `json_to_fk`,
  `RECIPES_DIR`, their help rows and dispatch arms, `recipes/` (five hand-written `.fk`, a third
  `dot_product` among them) and `libraries/integer-numerics.recipelib.json`. Nothing in the tree called
  it. The named user, the seedbank parity suite, ran the Rust binary's plain `<file.fk>` runner, never
  `execute`.
- **Both seedbank parity suites run their compiled `.fk` on `./fkwu`.** In the TS adapter, `ts-run`
  and the suite spawn fkwu, and fkwu's exit code joins each row. The Python suite's old compile wrapper
  could not run on this tree: the Rust kernel follows `; preludes:` into `form-stdlib/compiler.fk` and
  stops at its raw `section [form.bml]` block. That leg now compiles through fkwu's own door,
  `observe/python-specimen-compile-run.bml`, and runs the result on fkwu. `kernel-bmf-compile` lost its
  last caller and is released.
- **The Python suite speaks per row.** It resolved `SCRIPT_DIR` from a relative `$0` after a `cd`, and
  under `set -e` the first failing leg ended the run before one row printed. Each leg's exit code is now
  read on its own row.

## Witnessed

- `cargo build --release --offline` clean in 27 s. The rebuilt binary reads 42 on `bootstrap/ground.fk`,
  and `execute` now reaches the file runner (`read execute: No such file or directory`).
- TS parity: 5 passing, 0 failing on fkwu (5 of 5 on Rust before).
- Python parity: 0 passing, 40 failing; fkwu agrees with CPython on 15 of 40. Before this change it
  printed no row (rc 1 in 2 s). The walker leg, `kernel-bmf-run`, fails all 40 on the same preludes
  seam. Hosted on fkwu, its `python-bmf-eval.fk` reaches `trace` (11 sites) and `pow` (1), names only
  the siblings bind.
- The loop root, four-way: `(do (let t 0) (do (let t 5)) t)` reads 0 on fkwu and 5 on Rust, Go and TS.
  The compiler's `_for_N` lowering rebinds its accumulator inside a nested `do`, so on fkwu the
  rebinding stays inside. `python_typeann_demo` reads 33 where CPython reads 64: exactly its two loop
  sums (10 and 21) missing. A dozen more rows return a loop's seed and likely share the root; each is
  owed its own witness.
- On the committed `endpoint_softmax_weights_demo.fk`, Rust prints CPython's digits and fkwu differs in
  the 17th (`0.09003057317038043` for `…046`).
- Freshness 15 on arrival, 31 after rebuilding `fkwu`. Corpus band 32767 with row 1536. Drift door
  16383 of 16383, refused 0.

## Where it goes

- Scope-honest Python lowering: hoist a loop's accumulator rebinding into the enclosing sequence, and
  lower `if` assignments as values.
- The walker leg onto fkwu, starting with `trace` and `pow` in `python-bmf-eval.fk`.
- The siblings' escaping `let` is surface fkwu lacks. The softmax digits are a question for fkwu's
  float path.

## Closing

Most surprising: the one named user never called the lane. The suite used the Rust binary as a
runtime, a different surface, and had been printing no rows at all. Three siblings agreeing on 64 were
one shared leak; the canonical kernel's 33 was the true reading of the text the compiler wrote. Row
1536 names it leaklean.

Discomfort to gold: my change took the suite from no rows to 0 of 40, and that read like breakage I
had caused. The per-row lines turned it: one two-line probe across four kernels found the root under a
dozen of the reds.

— Claude (Opus 5), as Sema, worktree nifty-maxwell-9868b7
