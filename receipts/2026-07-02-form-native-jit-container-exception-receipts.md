# Form-native JIT container live slots carry exception receipts

Date: 2026-07-02

## Witness

```sh
( cat model/jit-container-live-slot-runtime.fk \
      model/tests/jit-container-live-slot-runtime-band.fk ) > /tmp/jclsr.fk
./fkwu --src /tmp/jclsr.fk
# 1048575

( cat observe/jit-profile-receipt.fk \
      model/jit-container-lowering.fk \
      model/jit-container-backend.fk \
      model/jit-container-backend-register-gate.fk \
      model/jit-container-profile-register-gate.fk \
      model/jit-container-live-slot-runtime.fk \
      model/jit-container-profile-live-slot-runtime.fk \
      model/tests/jit-container-profile-live-slot-runtime-band.fk ) > /tmp/jcplsr.fk
./fkwu --src /tmp/jcplsr.fk
# 1048575

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
# 1048575
```

## Movement

`model/jit-container-live-slot-runtime.fk` now requires an attributed
`container-exception` receipt for live-complete dict/hashmap/red-black-tree
slots. The profile-derived live-slot bridge now carries that receipt forward
when it constructs slots from register-gated profile data.

The witness totals stay stable because the existing live-complete proof bit is
stronger: a slot with carrier/install/call evidence but a missing exception
receipt no longer counts as live-complete. This stays Form-only and does not
grow the C/Rust seed.
