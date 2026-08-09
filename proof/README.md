# proof/ — the kernel proves its own four-way

The clean kernel no longer borrows the origin's `validate.sh`. It crosses its own bands:

- `four-way-run.fk` — fkwu `host-exec`s the three minimal walkers (`../walkers/{go,rust,ts}`) on a recipe,
  parses their values, and diagnoses with `four-way-verdict`. No bash, no origin.
- `four-way-verdict.fk` — the diagnosis: FOUR-WAY (all agree) / FKWU-SUSPECT (walkers agree, fkwu odd — rare,
  investigate the native) / WALKER-SUSPECT (one walker odd — common, a proof-note). Encodes that the native
  walker is rarely the wrong one.

Run it, from the repository root:

```
./fkwu proof/four-way-run-recipe42.fk      ->  0   (FOUR-WAY)
```

This line used to read `fkwu proof/four-way-run.tbl`. That stopped working when `.tbl` execution
was retired — the seed now answers *".tbl execution has been retired; use .fk, .fkb, or .dylib"* —
so the proof had no runnable entry until `four-way-run-recipe42.fk` was written on 2026-07-25.

`host-exec` is a host PORT (`runtime/fkwu-uni.c` optag 136, the VIA-HOST family) and `str_to_int`
is optag 31; `fwv-verdict` computes 0=FOUR-WAY / 1=FKWU-SUSPECT / 2=WALKER-SUSPECT.

The three walkers build from source in one command each — see `walkers/README.md`; the TS one runs
under `node --experimental-strip-types`, no tsx needed. Nothing prebuilt is required.

Perturbation-verified 2026-06-29 and again live on 2026-07-25 (the verdict is COMPUTED, not
parse-to-zero): the three walkers each return 42 on `recipe42.fk` → verdict **0** (FOUR-WAY); point
ts at a recipe answering 99 → **2** (WALKER-SUSPECT); tell the runner fkwu=99 while the walkers agree
→ **1** (FKWU-SUSPECT). The verdict tracks actual agreement among the host-exec'd values, not the
literal. Full evidence: `receipts/2026-06-29-kernel-self-proves-four-way.md` and
`receipts/2026-07-25-all-four-arms-are-here.md`.

## Running an ordinary band on the other three arms

Two things fkwu does for you that no walker does. Both look like the band is broken when they bite,
so they are written down here rather than rediscovered.

**1. The walkers do not read `; preludes:`.** fkwu walks the closure itself; a walker takes the
closure explicitly, in dependency order, on the command line.

```
$ walker form-stdlib/tests/hex-band.fk                                  walk: unbound function "hex-encode"
$ walker core.fk form-ontology-loader.fk str-byte-at.fk hex.fk \
         form-stdlib/tests/hex-band.fk                                  14
```

**2. The walkers do not know `import`.** It is an fkwu statement; every other arm answers
`walk: unbound identifier "import"` and stops. To cross a band that uses it, strip the `import`
lines and hand the same files over as the closure.

Worked twice on 2026-07-26, against the two bands that had just landed on main claiming a four-way
witness — both claims hold, and neither band runs on a walker as written:

```
cognition/tests/identity-space-structure-four-way-band.fk        127  on fkwu, go, rust, ts
cognition/tests/family-constellation-findings-four-way-band.fk  4095  on fkwu, go, rust, ts
```

So a header that says *"witnessed on fkwu + Go + Rust + TypeScript"* is about the **recipe**, not
about the band file — the file itself only runs as written on fkwu. Worth saying plainly in a
header, so the next person reproducing it knows which of the two they are being told.
