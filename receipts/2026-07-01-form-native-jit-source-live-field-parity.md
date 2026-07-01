# Form-native JIT source-live field parity

Date: 2026-07-01

## Commands

```sh
( cat model/form-asm-x64.fk model/jit-self-host-compiler.fk model/jit-source-live-native-executor.fk model/tests/jit-source-live-native-executor-band.fk ) > /tmp/jslne.fk
./fkwu --src /tmp/jslne.fk

( cat model/form-asm-x64.fk model/jit-self-host-compiler.fk model/jit-source-live-native-executor.fk model/jit-source-dylib-runtime-executor.fk model/tests/jit-source-dylib-runtime-executor-band.fk ) > /tmp/jsdre.fk
./fkwu --src /tmp/jsdre.fk

( cat observe/jit-source-replacement-runtime.fk observe/tests/jit-source-replacement-runtime-band.fk ) > /tmp/jsrr.fk
./fkwu --src /tmp/jsrr.fk

( cat observe/jit-dylib-live-runtime-proof.fk observe/tests/jit-dylib-live-runtime-proof-band.fk ) > /tmp/jdlrp.fk
./fkwu --src /tmp/jdlrp.fk

( cat observe/jit-rung20-readiness.fk observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
```

## Witness

```text
4194303
33554431
536870911
4294967295
536870911
```

## Movement

`model/jit-source-live-native-executor.fk` now proves the self-host compiler's
field-load bytes match the direct Form x64 field byte stream, alongside the
existing array and div byte parity checks. Its witness moves from `2097151` to
`4194303`.

`model/jit-source-dylib-runtime-executor.fk` now requires that stronger
source-live receipt and rejects stale pre-field source-live evidence, moving to
`33554431`.

`observe/jit-source-replacement-runtime.fk` now requires both current
source-live and current source-dylib executor receipts, rejects stale pre-field
totals for each, and moves to `536870911`. Downstream live/runtime/rung gates
were regrounded to those stronger source receipts without C, Rust, or walker
changes.
