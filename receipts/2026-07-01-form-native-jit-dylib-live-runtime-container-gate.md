# Form-native JIT dylib live-runtime container gate

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-dylib-live-runtime-container-gate.fk \
      observe/tests/jit-dylib-live-runtime-container-gate-band.fk ) > /tmp/jdlrcg.fk
./fkwu --src /tmp/jdlrcg.fk
# 134217727

( cat observe/jit-live-runtime-integration.fk \
      observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk
# 67108863

( cat observe/jit-full-track-sweep.fk observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
# 536870911
```

## Movement

`observe/jit-dylib-live-runtime-container-gate.fk` adds a compact companion
receipt for the saturated `dylib-live-runtime-proof` ledger. The gate requires
`dylib-live-runtime-proof = 4294967295`,
`source-dylib-container-executor = 4194303`, runtime-fault, and stack
attribution before live-runtime integration can treat the `.dylib` route as
current.

`observe/jit-live-runtime-integration.fk` now consumes
`dylib-live-runtime-container-gate = 134217727` and returns `67108863`. Rung-20
dylib audit and readiness receipts were updated to require that new live-runtime
total. The full-track sweep now carries the same gate and returns `536870911`.
Post-ingress and host-membrane receipt consumers now require that current
full-track total.

This does not grow `runtime/fkwu-uni.c`, Rust, or any proof sibling. The host
carrier remains a loader/call membrane; the JIT semantics stay Form-owned.
