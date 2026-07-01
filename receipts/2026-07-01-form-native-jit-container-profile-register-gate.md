# Form-native JIT container profile register gate

Date: 2026-07-01

## Commands

```sh
( cat observe/jit-profile-receipt.fk \
      model/jit-container-lowering.fk \
      model/jit-container-backend.fk \
      model/jit-container-backend-register-gate.fk \
      model/jit-container-profile-register-gate.fk \
      model/tests/jit-container-profile-register-gate-band.fk ) > /tmp/jcprg.fk
./fkwu --src /tmp/jcprg.fk

( cat model/jit-container-lowering.fk \
      model/jit-container-backend.fk \
      model/jit-container-backend-register-gate.fk \
      model/tests/jit-container-backend-register-gate-band.fk ) > /tmp/jcbr.fk
./fkwu --src /tmp/jcbr.fk

( cat model/jit-container-lowering.fk \
      model/jit-container-backend.fk \
      model/tests/jit-container-backend-band.fk ) > /tmp/jcb.fk
./fkwu --src /tmp/jcb.fk
```

## Witness

```text
4095
1023
4095
```

## Movement

`model/jit-container-profile-register-gate.fk` connects profile/category
receipts to the register-aware container backend gate without loading the full
replacement/live stack. Profile-derived dict and hashmap plans must prove CPU
register shape; the red-black-tree plan must prove GPU lane/register shape; all
three require the current `register-lowering=511` and `container-backend=4095`
receipts through `model/jit-container-backend-register-gate.fk`.

The gate rejects array and warm profile rows before they can count as container
register schedules, and it keeps the stale-register and missing source-map
receipt rejections visible at the profile-derived layer. This avoids widening
the hot backend or the oversized replacement composition while making the
profile-to-container path more true before the later dylib/live carrier work.
