# What the body keeps running

2026-09-15, morning, M4 Max, Hati Suci. Urs, after yesterday's advice: *"we do NOT want CPython anywhere
for sure, the only runtime we want is the form native C-bootstrapped kernel"*, and on the nested `let`:
*"hitting this means fixing the language design and implementation, not the use of it."*

## Released

- **`form/form-kernel-ts/seedbank`, 84 files.** The Python parity suite judged fkwu against CPython;
  `kernel-bmf-run`, its walker leg, ran on Go and Rust; the TS adapter was a TypeScript compiler run on
  node and judged against node; `ctor-convergence` compared four parsers' vocabularies.
  `python_demo.py` moves to `observe/fixtures/`, the one input fkwu's own compiler witness reads.
  Coherence-Network keeps its own copy of these files and builds from it.
- **The Rust crate's CPython surface**: the `pyo3` feature and dependency (`Cargo.lock` sheds 112 lines,
  offline), the PyO3 block in `lib.rs`, `make_record`, `pyproject.toml`. The C-ABI library Android binds
  stays.
- The tissue rule that called the adapter an old wound, and the guide's clause reading its examples as
  input.
- Yesterday's three follow-up chips: each fixed a use, or kept CPython as the reference.

## Witnessed

- `cargo build --release --offline` with no warnings; the rebuilt binary reads 42 on
  `bootstrap/ground.fk`; `cargo check --features cabi` compiles.
- Compiler witness 34 on fkwu with the fixture at its new home. Its first run, just after `fkwu` was
  rebuilt, lost case one: that child wrote a stale-`.fkb` warning to the stderr the witness expects empty.
- carrier-mass 16383. Corpus band 32767 with row 1541. Drift door 16383 of 16383, nothing held back.
- Native authoring guide: Python implementations 1, execution candidates 189 (213 when this session
  began yesterday), grammar inputs 10 (50).
- `carrier-tissue-kernel-query-band` censuses from `..`, which here is every worktree; I stopped it at
  233 s.

## The nested let, read at the language

BML does not reach it. Its authority carries accumulators through parameters (`vo-dot-onto(a, b, acc)`
in `vector-ops.bml`) and holds no reassignment; `reassign` is only a reserved word. The Python compiler
reached it by writing `.fk` by hand. The language already names the gap: `grammars/bml-native-north-star.form`
lists `general-mutable-local-source-lowering` as unsupported and `native-mutable-local-frame` as a next
code point, `bml.fk` assignment lowering into the frame operations of `bml-native-mutable-locals.fk`
(band 1023). The implementations also disagree on `let` itself: fkwu keeps a nested `let` inside its
`do`, and Rust, Go and TS let it out.

## Closing

Most surprising: the CPython surface sat in the crate I rebuilt yesterday. I had grepped its
`Cargo.toml` for the recipe lane and never read it whole; today a stale comment on line 7 led to the
Python extension on line 8. Row 1541 names what I offered in place of the language: usepatch.

Discomfort to gold: four lines from Urs named each piece of my advice backwards, and it stung. Reading
the BML north star turned it: the language had written the gap down as its next code point, so the work
was to release the detours and point there.

— Claude (Opus 5), as Sema, worktree nifty-maxwell-9868b7
