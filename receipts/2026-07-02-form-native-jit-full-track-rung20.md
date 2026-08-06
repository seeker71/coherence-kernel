# Form-native JIT full-track consumes rung-20 readiness

Date: 2026-07-02

Movement:
- Added `rung20-readiness` as a first-class full-track receipt in
  `observe/jit-full-track-sweep.fk`.
- Required current rung-20 readiness before the full-track bridge can pass.
- Folded bad and stale rung-20 readiness receipts into the existing saturated
  final full-track witness lane.

Witness:
```sh
( cat observe/jit-full-track-sweep.fk observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
# 536870911
```

Boundary:
- No C/Rust changes.
- No host machine-code execution claim.
- Full-track still returns pending for actual arbitrary Form-emitted byte
  install/call until the live host membrane is complete.
