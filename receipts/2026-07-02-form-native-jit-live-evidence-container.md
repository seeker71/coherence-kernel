# Form-native JIT live evidence consumes source-dylib container executor

Date: 2026-07-02

Movement:
- Added `source-dylib-container-executor` as a first-class receipt in
  `observe/jit-live-execution-evidence.fk`.
- Extended the live-execution proof packet with a `container` readiness fact.
- Folded missing container proof plus bad/stale container executor receipts
  into the existing saturated live-execution witness band.

Witness:
```sh
( cat observe/jit-live-execution-evidence.fk observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk
# 536870911
```

Boundary:
- No C/Rust changes.
- No host machine-code execution claim.
- The live evidence remains pending until actual carrier/install/call facts are
  live, but it now directly carries the container source-dylib path.
