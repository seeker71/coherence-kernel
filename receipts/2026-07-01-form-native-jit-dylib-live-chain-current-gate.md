# Form-native JIT dylib live-chain current gate

Date: 2026-07-01

## Commands

```sh
( cat observe/jit-final-native-gate.fk observe/tests/jit-final-native-gate-band.fk ) > /tmp/jfng.fk
./fkwu --src /tmp/jfng.fk

( cat observe/jit-live-execution-evidence.fk observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk

( cat observe/jit-dylib-live-runtime-proof.fk observe/tests/jit-dylib-live-runtime-proof-band.fk ) > /tmp/jdlrp.fk
./fkwu --src /tmp/jdlrp.fk

( cat observe/jit-live-runtime-integration.fk observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk

( cat observe/jit-rung20-dylib-runtime-audit.fk observe/tests/jit-rung20-dylib-runtime-audit-band.fk ) > /tmp/jrdra.fk
./fkwu --src /tmp/jrdra.fk

( cat observe/jit-rung20-readiness.fk observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk

( cat observe/jit-profile-receipt.fk model/form-asm-x64.fk model/jit-self-host-compiler.fk model/jit-self-host-profile-runtime.fk model/jit-self-host-ingress-runtime.fk observe/jit-native-call-plan.fk model/jit-self-host-native-call-runtime.fk model/jit-self-host-host-membrane-runtime.fk model/jit-self-host-live-evidence.fk model/jit-self-host-final-native-gate.fk model/tests/jit-self-host-live-evidence-band.fk ) > /tmp/jshl.fk
./fkwu --src /tmp/jshl.fk

./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

## Witness

```text
4194303
67108863
4294967295
16777215
134217727
536870911
1048575
42
55
11111
```

## Movement

`observe/jit-final-native-gate.fk` now rejects bad native-witness and bad
replacement-runtime receipts. `observe/jit-live-execution-evidence.fk` now
requires that stronger final-native gate and rejects the previous final-gate
total. The dylib live-runtime proof, live-runtime integration, rung-20 dylib
runtime audit, and rung-20 readiness gates now consume the current live evidence
and live runtime receipt totals, rejecting stale totals before completion.

This stays Form-native: no C seed growth, no Rust changes, and no walker changes.
