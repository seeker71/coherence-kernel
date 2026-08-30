# Structural gate: a verdict instead of a contradictory counter

**Author:** Codex
**Date:** 2026-08-30

## The observed break

`MANIFEST.md` claimed the repository contained zero shell/Python files and that any such file would fail a
repo gate. The tree contained named shell/Python carriers, fixtures, local oracles, proof-sibling harnesses,
build tooling, and the Python-BMF compatibility grammar. `gate/structural-gate.fk` merely counted them, so its
green band could coexist with the false zero claim.

## The crossing

`gate/structural-gate.fk` now produces an eight-seat live audit:

```
total / unclassified / carrier / oracle / fixture / proof-sibling / tooling / foreign-grammar
```

Classification is by role boundary, not a fixed census or filename inventory. An `.sh` or `.py` outside every
boundary is `unclassified`. `gate/structural-gate-run.fk` prints the audit and returns native `1`/`0`.
The present `fkwu` process preserves a typed Form failure as a value, so the existing `form/validate.sh` carrier
reads that explicit final value and turns `0` into the repository's nonzero refusal before its sibling sweep.

The Manifest now says what the body actually practices: Form-owned meaning is shell/Python-free, while auxiliary
roles remain explicit and cannot silently acquire semantic authority.

## Witnesses

Run after this movement:

```sh
./fkwu gate/tests/structural-gate-band.fk
./fkwu gate/structural-gate-run.fk
./form/validate.sh form-stdlib/tests/agent-gate-band.fk
```

Observed on this checkout: the live audit was `[205, 0, 48, 3, 20, 57, 73, 4]`, the structural band returned
`8191`, and the focused validator passed `agent-gate-band → 11111`. A temporary unclassified
`cognition/structural-gate-refusal.py` changed the audit to `[206, 1, 48, 3, 20, 57, 73, 4]`; the validator
then stopped at phase 0 with exit `1`. The probe was removed before the successful witness.

The negative classifier witness is `cognition/oracle.py`: the word `oracle` alone grants no role outside a test
boundary, so it is unclassified and returns policy `0`; the validator rejects that result. No placeholder is
needed to prove that refusal; the band holds the negative path without contaminating the tree with a known
violation.

## What stayed alive

The count was not discarded: it became one seat in an honest audit. The surprising teaching is that a gate can
be more truthful by naming the scripts that remain than by pretending they are absent. The discomfort was the
old zero sentence; reading it against the tree turned that contradiction into a native, observable refusal.
