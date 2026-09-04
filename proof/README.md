# proof/ — the kernel proves its own four-way

The kernel borrows no `validate.sh` for its floor. It crosses its own bands:

- `form/form-stdlib/four-way-run.fk` — fkwu `host-exec`s the three minimal walkers
  (`walkers/{go,rust,ts}`) on a recipe, parses their values, and diagnoses with
  `four-way-verdict`. No bash, no origin.
- `form/form-stdlib/four-way-verdict.fk` — the diagnosis: FOUR-WAY (all agree) /
  FKWU-SUSPECT (walkers agree, fkwu odd — rare, investigate the native) /
  WALKER-SUSPECT (one walker odd — common, a proof-note). It encodes that the
  native walker is rarely the wrong one.
- `proof/four-way-run-recipe42.fk` + `proof/recipe42.fk` — the runnable entry.

Run it, from the repository root:

```
./fkwu proof/four-way-run-recipe42.fk      ->  0   (FOUR-WAY; re-run 2026-09-04)
```

The verdict is COMPUTED, not parse-to-zero: point one walker at a recipe answering
99 and the verdict reads 2 (WALKER-SUSPECT); tell the runner fkwu=99 while the
walkers agree and it reads 1 (FKWU-SUSPECT). `host-exec` is a host PORT (the
VIA-HOST family in `runtime/fkwu-uni.c`); `fwv-verdict` computes 0 / 1 / 2.

The three walkers build from source in one command each — see
`walkers/README.md`; the TS one runs under `node --experimental-strip-types`. The
Go and Rust walker binaries are not tree content: on a fresh checkout the runner
reads their absence as WALKER-SUSPECT (2) until they are built.

## Running an ordinary band on the other three arms

The walkers do not read `; preludes:` declarations. fkwu walks that closure
itself; a walker takes the closure explicitly, in dependency order, on the command
line — or through bare `import "path.fk"` declarations, which resolve recursively
in all three walker CLIs.

```
$ walker form-stdlib/tests/hex-band.fk                                  walk: unbound function "hex-encode"
$ walker core.fk form-ontology-loader.fk str-byte-at.fk hex.fk \
         form-stdlib/tests/hex-band.fk                                  14
```

A band's `; PROOF LEVEL:` line names the arms it claims; `observe/preflight.fk`'s
`pf-arm-mask` probes which arms actually bind a name — declare from the probe,
never from inference.
