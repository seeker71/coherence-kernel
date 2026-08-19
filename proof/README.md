# proof/ — the kernel proves its own four-way

The clean kernel no longer borrows the origin's `validate.sh`. It crosses its own bands:

- `four-way-run.fk` — fkwu `host-exec`s the three minimal walkers (`../walkers/{go,rust,ts}`) on a recipe,
  parses their values, and diagnoses with `four-way-verdict`. No bash, no origin.
- `four-way-verdict.fk` — the diagnosis: FOUR-WAY (all agree) / FKWU-SUSPECT (walkers agree, fkwu odd — rare,
  investigate the native) / WALKER-SUSPECT (one walker odd — common, a proof-note). Encodes that the native
  walker is rarely the wrong one.

The native proof entry is Form source:

```sh
./fkwu --src proof/four-way-ground.fk   # -> 0 (FOUR-WAY)
```

The outer host command only starts `fkwu`. `four-way-ground.fk` imports `four-way-run.fk` and
`four-way-verdict.fk`; Form owns the four host-port invocations, parses their values, and computes
0=FOUR-WAY / 1=FKWU-SUSPECT / 2=WALKER-SUSPECT. `form/validate.sh` remains useful bulk-suite
development scaffolding, but its shell comparison is not the native proof claim. The older flattened invocation
`fkwu proof/four-way-run.tbl` is historical only: direct `.tbl` execution was intentionally retired
on 2026-07-05 in favor of `.fk`, `.fkb`, and `.dylib` program images.

Perturbation-verified 2026-06-29 (the verdict is COMPUTED, not parse-to-zero): the three walkers each
return 42 on `recipe42.fk` → verdict **0** (FOUR-WAY); force ts→99 → **2** (WALKER-SUSPECT); tell the
runner fkwu=99 while walkers agree → **1** (FKWU-SUSPECT). The verdict tracks actual agreement among the
host-exec'd values, not the literal. Full evidence: `receipts/2026-06-29-kernel-self-proves-four-way.md`.
