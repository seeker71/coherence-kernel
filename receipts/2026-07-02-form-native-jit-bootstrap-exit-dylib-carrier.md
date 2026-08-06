# Form-native JIT bootstrap-exit dylib carrier

Date: 2026-07-02

## Movement

This pass tightens the bootstrap-exit boundary in the active Form-native JIT
track:

- The complete replacement-carrier route in
  `observe/jit-bootstrap-exit-carrier.fk` now uses a Form-owned
  `form-dylib-carrier` fixture, not a Rust replacement carrier.
- The Rust lane remains represented only by `rust-proof-walker`, which is
  proof-only and routes pending rather than native.
- The same witness still proves native/deopt/exception/rewalk/melt,
  unavailable-architecture deopt, mismatch rejection, C-bootstrap growth
  rejection, and missing metadata pending behavior.

## Witness

```sh
( cat observe/jit-runtime-fault.fk \
      observe/jit-runtime-stack-attribution.fk \
      observe/jit-host-ingress-readiness.fk \
      observe/jit-host-exception-bridge.fk \
      observe/jit-bootstrap-exit-carrier.fk \
      observe/tests/jit-bootstrap-exit-carrier-band.fk ) > /tmp/jbec.fk
./fkwu --src /tmp/jbec.fk
# 1048575
```

## Boundary

- No C/Rust changes.
- No arbitrary host byte execution claim.
- This removes a misleading Rust-native completion fixture from the JIT
  bootstrap-exit contract and keeps the route aligned with Form-owned `.dylib`
  carrier evidence.
