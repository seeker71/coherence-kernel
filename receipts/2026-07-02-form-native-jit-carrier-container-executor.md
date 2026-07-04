# Form-native JIT carrier evidence consumes source-dylib container executor

Date: 2026-07-02

Movement:
- Added `source-dylib-container-executor` as a first-class receipt in
  `observe/jit-carrier-install-call-evidence.fk`.
- Extended the carrier/install/call proof packet with a `container` readiness
  fact so the host membrane ledger cannot stand up without the container
  source-dylib path.
- Folded missing container proof plus bad/stale container executor receipts
  into the existing saturated carrier evidence witness band.

Witness:
```sh
( cat observe/jit-carrier-install-call-evidence.fk observe/tests/jit-carrier-install-call-evidence-band.fk ) > /tmp/jcice.fk
./fkwu --src /tmp/jcice.fk
# 4294967295
```

Boundary:
- No C/Rust changes.
- No host machine-code execution claim.
- The carrier ledger remains pending for actual arbitrary Form-emitted byte
  install/call until live install/call facts are present.
