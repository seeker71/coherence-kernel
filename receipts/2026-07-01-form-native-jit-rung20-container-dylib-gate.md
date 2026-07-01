# Form Native JIT Rung-20 Container Dylib Gate

Date: 2026-07-01

## Commands

```sh
( cat observe/jit-rung20-dylib-runtime-audit.fk \
      observe/tests/jit-rung20-dylib-runtime-audit-band.fk ) > /tmp/jrdra.fk
./fkwu --src /tmp/jrdra.fk

( cat observe/jit-rung20-readiness.fk \
      observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
```

## Witness

`observe/jit-rung20-dylib-runtime-audit.fk` now requires
`source-dylib-container-executor = 4194303` before the final dylib runtime audit
can compose.

The rung-20 dylib audit witness moves to `268435455`. The final rung-20
readiness witness remains `536870911`, but its dylib audit receipt is now the
current container-aware total and rejects the stale pre-container audit
`134217727`.

This keeps the direct-access surface honest at the final readiness boundary:
array/field/div and dict/hashmap/red-black-tree source-dylib bridges must both
be present before the track can say the dylib runtime path is ready for rung 20.
