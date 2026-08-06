# Form Native JIT Self-Host Container Runtime Loop

Date: 2026-07-01

## Commands

```sh
( cat observe/jit-profile-receipt.fk \
      model/jit-container-lowering.fk \
      model/jit-container-backend.fk \
      model/jit-container-backend-register-gate.fk \
      model/jit-container-profile-register-gate.fk \
      model/jit-container-live-slot-runtime.fk \
      model/jit-container-profile-live-slot-runtime.fk \
      model/jit-self-host-container-runtime-loop.fk \
      model/tests/jit-self-host-container-runtime-loop-band.fk ) > /tmp/jshcrl.fk
./fkwu --src /tmp/jshcrl.fk

( cat observe/jit-self-host-completion-sweep.fk \
      observe/tests/jit-self-host-completion-sweep-band.fk ) > /tmp/jshcs.fk
./fkwu --src /tmp/jshcs.fk
```

## Witness

`model/jit-self-host-container-runtime-loop.fk` adds the missing container
runtime loop beside the existing checked-access self-host loop. Dict, hashmap,
and GPU red-black-tree container slots reach loop readiness through the existing
profile/register-gated live-slot facts.

The focused loop band returns `1048575`. Self-host completion now requires
`self-host-container-runtime-loop = 1048575` and moves to `134217727`.

This keeps the JIT compiler's self-host path honest: field/list/array and
dict/hashmap/red-black-tree access families must both have runtime-loop
coverage before native-witness admission can count self-host completion.
