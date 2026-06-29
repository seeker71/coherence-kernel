# proof/ — the kernel proves its own four-way

The clean kernel no longer borrows the origin's `validate.sh`. It crosses its own bands:

- `four-way-run.fk` — fkwu `host-exec`s the three minimal walkers (`../walkers/{go,rust,ts}`) on a recipe,
  parses their values, and diagnoses with `four-way-verdict`. No bash, no origin.
- `four-way-verdict.fk` — the diagnosis: FOUR-WAY (all agree) / FKWU-SUSPECT (walkers agree, fkwu odd — rare,
  investigate the native) / WALKER-SUSPECT (one walker odd — common, a proof-note). Encodes that the native
  walker is rarely the wrong one.

Witnessed 2026-06-29: a pure recipe (=42) → all three walkers returned 42, the runner returned 0 (four-way).
