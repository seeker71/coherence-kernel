# Receipt - name the dylib container live-runtime gate numbers (2026-07-02)

## Movement

`observe/jit-dylib-live-runtime-container-gate.fk` now names the numeric
structure it uses at the live `.dylib` container bridge:

- receipt length and receipt slots
- current receipt totals
- stale receipt totals
- gate slots
- source attribution coordinates
- proof bit weights

The public witness remains unchanged.

## Why

The gate is one of the active live-execution path ledgers. It carries
`source-dylib-container-executor` into the live runtime path, but the previous
form repeated totals such as `4294967295`, `4194303`, `2047`, stale sentinels,
and bit weights directly at call sites.

Those numbers are not gone; they are now named at the boundary where they
become evidence, so a reader can follow the contract without reverse-engineering
the ledger arithmetic.

## Witness

```sh
( cat observe/jit-dylib-live-runtime-container-gate.fk \
      observe/tests/jit-dylib-live-runtime-container-gate-band.fk ) > /tmp/jdlrcg.fk
./fkwu --src /tmp/jdlrcg.fk
# -> 134217727
```
