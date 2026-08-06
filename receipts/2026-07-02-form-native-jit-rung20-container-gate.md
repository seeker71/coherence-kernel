# Form-native JIT rung-20 consumes container live-runtime gate

Date: 2026-07-02

Movement:
- Added `dylib-live-runtime-container-gate` as a first-class receipt in
  `observe/jit-rung20-readiness.fk`.
- Extended the rung-20 proof packet with a `container` readiness fact so final
  readiness cannot rely only on the aggregate live-runtime receipt.
- Folded missing container proof, bad container-gate receipts, and stale
  container-gate receipts into the existing saturated rung-20 witness band.
- Named the saturated receipt masks used by the rung-20 totals.

Witness:
```sh
( cat observe/jit-rung20-readiness.fk observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
# 536870911
```

Boundary:
- No C/Rust changes.
- No host machine-code execution claim.
- This hardens the Form-native readiness contract before the final live
  carrier/install/call membrane is claimed complete.
