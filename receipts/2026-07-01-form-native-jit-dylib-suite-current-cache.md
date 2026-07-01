# Form-native JIT dylib suite current-cache gate

Date: 2026-07-01

## Commands

```sh
( cat observe/jit-dylib-live-proof-suite.fk observe/tests/jit-dylib-live-proof-suite-band.fk ) > /tmp/jdlps.fk
./fkwu --src /tmp/jdlps.fk

( cat observe/jit-live-execution-evidence.fk observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk

( cat observe/jit-dylib-live-runtime-proof.fk observe/tests/jit-dylib-live-runtime-proof-band.fk ) > /tmp/jdlrp.fk
./fkwu --src /tmp/jdlrp.fk

( cat observe/jit-live-runtime-integration.fk observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk

( cat observe/jit-rung20-readiness.fk observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk

( cat observe/jit-full-track-sweep.fk observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk

( cat observe/jit-profile-receipt.fk model/form-asm-x64.fk model/jit-self-host-compiler.fk model/jit-self-host-profile-runtime.fk model/jit-self-host-ingress-runtime.fk observe/jit-native-call-plan.fk model/jit-self-host-native-call-runtime.fk model/jit-self-host-host-membrane-runtime.fk model/jit-self-host-live-evidence.fk model/jit-self-host-final-native-gate.fk model/tests/jit-self-host-live-evidence-band.fk ) > /tmp/jshl.fk
./fkwu --src /tmp/jshl.fk
```

## Witness

```text
536870911
268435455
4294967295
16777215
536870911
524287
1048575
```

## Movement

`observe/jit-dylib-live-proof-suite.fk` now requires
`carrier-current-cache-gate = 8191` directly, in addition to the saturated
`carrier-install-call-evidence = 4294967295` ledger. It rejects malformed and
stale current-cache receipts before the dylib suite can become live, raising
the dylib live proof-suite witness from `134217727` to `536870911`.

`observe/jit-live-execution-evidence.fk` now requires that stronger
`dylib-live-proof-suite = 536870911` receipt and rejects the prior
pre-current-cache suite total, raising live-execution evidence to
`268435455`. Direct consumers were regrounded to that current live-evidence
receipt without changing C, Rust, or walker code.
