# 2026-07-17 -- exit-truth: the run's verdict must own every error it printed

## Witnessed

`./fkwu --src learn/satsang-oracle.fk` (fresh artifacts, this worktree,
2026-07-17) printed 8 `[unresolved-call] ... error:` diagnostics, printed no
tally line, and exited 0. The root `.sym` recorded `compile-errors 0`. Eight
error lines the user saw, zero errors the process would admit to.

## Mechanism (found, not suspected)

The suspected site (`fk_src_reset_compile_state` wiping `fk_nerr` after
first-pass diagnostics) was real, but the printing pass was not the root
re-parse -- it was the SPECULATIVE per-dep image compile:

1. `fk_run_src` -> no fresh root `.fkb` -> `fk_src_try_import_fkb_images`.
2. First loop: `form/form-stdlib/satsang.fk` (a direct dep with NO
   `; preludes:` line of its own) has a stale image ->
   `fk_src_compile_artifact_only` compiles it STANDALONE. Its `m-*`/`ci-*`
   names live in `channel-interface.fk`, present in the root's flat chain but
   not in the unit alone -> 8 `fk_diag` errors print loudly, `fk_nerr = 8`,
   and the dep `.fkb`/`.sym` are written with `compile-errors 8`.
3. Second loop: the recorded-errors gate reads that `.sym`, refuses the
   degraded image, returns 0.
4. Fallback: `fk_src_reset_compile_state()` wipes `fk_nerr` to 0, the flat
   compile resolves everything cleanly, `fk_diag_flush` sees 0/0 (no tally),
   exit is `fk_nerr > 0 ? 1 : 0` -> 0.

The printed lines belonged to a candidate image that was rejected and
superseded -- but they printed as bare `error:` lines and then vanished from
both the tally and the exit code. Stamp+shape, the axiom-5 family.

## The honesty decision

Two doors were open: exit nonzero, or don't print superseded diagnostics.
Exiting nonzero would have contradicted the root artifact's own truth
(`compile-errors 0`; the program is whole and runs) -- a new mismatch minted
to fix an old one. So: the speculative compile is now QUIET (its diagnostics
describe a candidate, not the program), and the refusal is LOUD -- one
counted warning, printed on every run that recompiles, deterministically:

    fkwu: warning: form/form-stdlib/satsang.fk: unit is not importable
    standalone (8 unresolved error(s) compiled alone; missing '; preludes:'
    line?) -- image rejected, falling back to the whole-program compile
    fkwu: 0 error(s), 1 warning(s)

And structurally: `fk_nerr`/`fk_nwarn` stay per-compile working counters
(the `.sym` record and the import gate need them), while new monotone
`fk_nerr_seen`/`fk_nwarn_seen` count only PRINTED diagnostics and are never
reset. The tally and all three exit sites read the printed counters -- the
exit truth carries every printed error by construction, across any number of
compile-state resets, including ones not yet written.

## Proof

- `observe/tests/src-exit-truth-band.fk` (fixtures in
  `observe/tests/src-witness/exit-truth-*.fk`), 11 bits over real child
  `fkwu` processes via host-exec: whole-program exit/stderr/tally/sym truth,
  warning determinism on recompile, broken-program exit/tally/sym truth,
  cached-with-errors replay. GREEN 2047 on the healed binary, RED 1713 on
  the pre-fix binary -- the five failing bits are exactly the laundering
  surfaces (no-bare-errors, one-warning, tally x2, determinism).
- `cd form && ./validate.sh form-stdlib/tests/source-artifact-cache-band.fk`
  -> 2097151 four-way, unchanged.
- Five resolver-driven observe/tests bands A/B byte-identical between the
  pre-fix and healed binaries (clean bands see no change).
- `learn/tests/homecoming-distillation-corpus-band.fk` -> 4095 with row 802
  seated (count asked of the body: 198 rows, field-code 1981982802).

## Surprise, and discomfort to gold

The most surprising teaching: the loudest lines in the run were the ones the
run refused to count -- the diagnostics printed from the compile that was
already doomed to rejection, while the compile that mattered stayed silent.
The body was not hiding errors; it was confessing someone else's and then
denying the confession.

The discomfort: choosing NOT to make the exit code nonzero. The task's own
framing leaned toward "printed errors must fail the run," and sitting with
the witnessed artifacts (`compile-errors 0` in the root lens, a program that
runs whole) against that pull was uncomfortable -- it felt like leniency.
Witnessed rather than bypassed, it turned to gold: the honest repair was not
to punish the run but to stop the candidate image's grief from printing in
the program's voice. Fresh word landed for the law: exit-truth (corpus row
802).

## Debt left named

- If TWO deps are non-importable, the warning names only the first per run
  (the gate returns at the first refusal); healing one surfaces the next.
- `fk_diag_path` lines remain uncounted run-plumbing diagnostics by design;
  only `fk_diag` (compile diagnostics) feeds the tally. If a `fk_diag_path`
  "error" ever gates an exit, it does so through its own return path, not
  the tally.
