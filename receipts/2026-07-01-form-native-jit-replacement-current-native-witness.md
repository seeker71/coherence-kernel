# Form-native JIT replacement current native-witness gate

Date: 2026-07-01

## Commands

```sh
( cat observe/jit-replacement-runtime-integration.fk \
      observe/tests/jit-replacement-runtime-integration-band.fk ) > /tmp/jrri.fk
./fkwu --src /tmp/jrri.fk

( cat observe/jit-final-native-gate.fk \
      observe/tests/jit-final-native-gate-band.fk ) > /tmp/jfng.fk
./fkwu --src /tmp/jfng.fk

( cat observe/jit-source-replacement-runtime.fk \
      observe/tests/jit-source-replacement-runtime-band.fk ) > /tmp/jsrr.fk
./fkwu --src /tmp/jsrr.fk

( cat observe/jit-live-execution-evidence.fk \
      observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk

( cat observe/jit-rung20-readiness.fk \
      observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
```

## Witness

```text
268435455
4194303
134217727
67108863
536870911
```

## Movement

`observe/jit-replacement-runtime-integration.fk` now rejects the stale
`native-witness-sweep = 16777215` receipt after the native witness moved to
`33554431`. The replacement runtime integration total moves from `134217727` to
`268435455`.

Direct replacement-runtime consumers were updated to require the stronger total:

- `observe/jit-final-native-gate.fk`
- `observe/jit-source-replacement-runtime.fk`
- `observe/jit-live-execution-evidence.fk`
- `observe/jit-live-runtime-integration.fk`
- `observe/jit-rung20-readiness.fk`

No C or Rust runtime work was added. This carries current native-witness
requirements into the replacement runtime boundary instead of letting stale
native-witness receipts satisfy later runtime gates.
