# Form-native JIT container profile live-slot runtime

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
      model/tests/jit-container-profile-live-slot-runtime-band.fk ) > /tmp/jcplsr.fk
./fkwu --src /tmp/jcplsr.fk

( cat model/jit-container-live-slot-runtime.fk \
      model/tests/jit-container-live-slot-runtime-band.fk ) > /tmp/jclsr.fk
./fkwu --src /tmp/jclsr.fk

( cat observe/jit-profile-receipt.fk \
      model/jit-container-lowering.fk \
      model/jit-container-backend.fk \
      model/jit-container-backend-register-gate.fk \
      model/jit-container-profile-register-gate.fk \
      model/tests/jit-container-profile-register-gate-band.fk ) > /tmp/jcprg.fk
./fkwu --src /tmp/jcprg.fk
```

## Witness

```text
1048575
1048575
4095
```

## Movement

`model/jit-container-profile-live-slot-runtime.fk` derives compact live-slot
contracts for dict, hashmap, and red-black-tree container replacements from
profile receipts that already passed the register-aware backend schedule gate.
The bridge maps profile-derived container kinds to the existing live-slot byte
shapes and target facts, then reuses the live-slot route matrix for native,
guard-deopt, runtime-exception, invalidation-rewalk, parity-deopt, and
stale-melt behavior.

Array and warm profile rows do not derive slots, stale register receipts reject,
and current missing carrier/install/call evidence remains pending. This moves
the container path closer to the live carrier: live slots are no longer only
hand-modeled dict/hash/tree rows; they now have a witnessed profile/register
admission path before the later dylib/install/call ledger consumes them by
receipt.
