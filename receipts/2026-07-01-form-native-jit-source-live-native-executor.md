# Form-native JIT source live native executor

Date: 2026-07-01

## Witness

```sh
( cat model/form-asm-x64.fk \
      model/jit-self-host-compiler.fk \
      model/jit-source-live-native-executor.fk \
      model/tests/jit-source-live-native-executor-band.fk ) > /tmp/jslne.fk
./fkwu --src /tmp/jslne.fk
# 2097151
```

## Receipt

Added `model/jit-source-live-native-executor.fk` and its band test. The cell is
a compact Form-native executor gate between source-runtime selection and live
native route status.

It validates real Form-emitted x64 args-vector bytes from
`model/jit-self-host-compiler.fk`, requires the source-runtime orchestrator,
self-host host-membrane runtime, and self-host native-call runtime receipts, and
then routes native/deopt/exception/rewalk/melt only when source state, host
readiness, carrier status, parity, and runtime flags agree.

Missing byte ingress or W^X state stays pending, unavailable host status deopts,
carrier mismatch rejects, stale state melts, invalidation rewalks, and parity
failure deopts. No C or Rust runtime work was added.
