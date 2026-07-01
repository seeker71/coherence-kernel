# Form-native JIT runtime mod-zero fault

Date: 2026-07-01

## Commands

```sh
( cat observe/jit-runtime-fault.fk observe/tests/jit-runtime-fault-band.fk ) > /tmp/jrf.fk
./fkwu --src /tmp/jrf.fk

( cat observe/jit-runtime-fault.fk observe/jit-runtime-stack-attribution.fk observe/tests/jit-runtime-stack-attribution-band.fk ) > /tmp/jrsa.fk
./fkwu --src /tmp/jrsa.fk

( cat observe/jit-runtime-fault.fk observe/jit-runtime-stack-attribution.fk observe/jit-host-ingress-readiness.fk observe/jit-host-exception-bridge.fk observe/tests/jit-host-exception-bridge-band.fk ) > /tmp/jheb.fk
./fkwu --src /tmp/jheb.fk

( cat observe/jit-dylib-live-runtime-proof.fk observe/tests/jit-dylib-live-runtime-proof-band.fk ) > /tmp/jdlrp.fk
./fkwu --src /tmp/jdlrp.fk

( cat observe/jit-live-runtime-integration.fk observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk

( cat observe/jit-rung20-readiness.fk observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk

( cat observe/jit-full-track-sweep.fk observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
```

## Witness

```text
511
2047
2097151
4294967295
16777215
536870911
524287
```

## Movement

`observe/jit-runtime-fault.fk` now names an explicit `mod-zero` runtime fault
kind and witnesses that a modulo zero trap carries the same source-attributed
exception shape as bounds, null, div-zero, and type faults.

`observe/jit-runtime-stack-attribution.fk` now proves mod-zero exceptions share
the full-stack source attribution and native/walker parity contract. The
host-exception bridge now requires `runtime-fault = 511` and
`runtime-stack-attribution = 2047`, rejects stale pre-mod receipts, and raises
its witness to `2097151`.

Downstream JIT and dylib live-runtime receipts were regrounded to the stronger
fault/stack/host exception totals without changing C, Rust, or walker code.
