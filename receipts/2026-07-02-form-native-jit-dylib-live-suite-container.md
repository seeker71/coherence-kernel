# Form-native JIT dylib live suite consumes source-dylib container executor

Date: 2026-07-02

Movement:
- Added `source-dylib-container-executor` as a first-class receipt in
  `observe/jit-dylib-live-proof-suite.fk`.
- Extended the dylib live proof-suite packet with a `container` readiness fact.
- Folded missing container proof plus bad/stale container executor receipts
  into the existing saturated dylib live proof-suite witness band.

Witness:
```sh
( cat observe/jit-dylib-live-proof-suite.fk observe/tests/jit-dylib-live-proof-suite-band.fk ) > /tmp/jdlps.fk
./fkwu --src /tmp/jdlps.fk
# 536870911
```

Boundary:
- No C/Rust changes.
- No host machine-code execution claim.
- The suite still models Form-owned readiness and routing before arbitrary
  Form-emitted byte install/call can be claimed live.
