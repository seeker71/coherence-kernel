# Form-native JIT array live slots

Date: 2026-07-02

This receipt carries the array container path from the profile/register gate
into the compact live-slot runtime and self-host container runtime loop. The
previous array bit in these bands proved rejection; after the array lowerer and
profile-register gate were promoted, that bit needed to prove readiness instead.

No C or Rust changed.

## Witnesses

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

## Notes

The totals remain `1048575`; the meaning changed. The old array placeholder
was a rejection proof. It is now a real array live-slot proof with bytes
`72 139 4 24 195`, source/maps, carrier/install/call facts, attributed
exception proof, deopt/melt/parity proof, and positive generation.

Warm profiles still reject before live-slot construction. Current missing
carrier/install/call evidence still routes pending rather than pretending a
host call happened.
