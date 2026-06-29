# Receipt — verified: the flatten chain is deprecated; the missing piece is the small "cursor seed", not it (2026-06-29)

**The question (Urs):** the Mac fusion + flattener are Form-native, so why don't they run on Windows? And — "I
think we learned we don't need [the flatten table] anymore on Mac, that is why it is missing." Verified. You're
right, with one refinement that makes the path smaller.

## Verified: flatten is OFF the critical path (deprecated)

- `HOMECOMING.md` rung 1: *"`form-eval` / `form-eval-full` evaluate Form source — do/let/defn/user-calls/nested —
  directly off the BMF cursor, with **no flatten of the source**… Flatten is **optional speed now, never a gate**."*
- `receipts/2026-06-29-standing-source-runner.md`: *"**running a recipe never requires flattening it.** Flatten is
  optional speed."*

So the heavy flatten chain (`fourth-flatten-table.txt` / `T_flat` / the bin-go artifact) is **deprecated**. That is
why its committed table has a no-op entry (read_line/print_str fire 0 times) and the runnable flatten driver was
never maintained or committed. **It's missing because it isn't the path anymore — confirmed.**

## The refinement: one SMALL bootstrap remains (and it's not the flatten chain)

The same receipt: *"the one flattened thing is the cli recipe itself, **the cursor seed**; all source runs through
it un-flattened."* The cursor seed = `agent/form-eval-cli.fk` (8) + `grammars/form-eval.fk` (52) + the BMF cursor
`grammars/bmf-core.fk` (279) + `bmf-grammar.fk` (400) — **~740 lines of Form, flattened ONCE** into a numeric
table. On the Mac it's a local artifact; it is **not committed** (no `.tbl` in the repo). That small seed — not the
deprecated heavy flatten chain — is the actual missing piece for Windows.

## The reframe of the `--src` stones

`--src` (`fk_run_src` / `fk_sparse`) is exactly the **cursor seed's role** — a native source-runner — baked into the
C kernel instead of flattened from Form. It already runs Form source on Windows (recursion, `let`, `do`, the body
decision cells). In the new "run source off the cursor, don't flatten" model, `--src` is **not the flattener
inversion** named in `2026-06-29-stones-bounded-flattener-is-form.md` — the inversion was about reimplementing the
(now deprecated) **flattener**; a source **evaluator** (`form-eval` / `--src`) is the **current** path. The two were
conflated; the cursor evaluator is the live model and `--src` is its native-in-C twin.

## Corrected way forward (small, two clean options)

Stand the **source-runner**, not the flatten table:
1. **Commit the cursor seed** — `form-eval-cli` + `form-eval` + the BMF cursor, flattened ONCE on the Mac
   (~740 lines → one small table). Windows then runs the whole body via `form-eval`, off the cursor, no flatten.
   The clean Form path; needs the Mac to flatten it once.
2. **Grow `--src`** to `form-eval`'s grammar (strings, lists, multi-arg) — a native source-runner in C,
   toolchain-free, needs nothing from the Mac. Aligned with the new model (run source), not the flattener inversion.

Both are small; neither needs the heavy flatten chain. `flatten/SEED-DROP.md` updated to name the cursor seed
(was mislabeled "the flatten table").
