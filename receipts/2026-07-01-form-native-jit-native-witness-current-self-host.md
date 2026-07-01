# Form-native JIT native witness current self-host gate

Date: 2026-07-01

## Commands

```sh
( cat observe/jit-native-witness-sweep.fk \
      observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk

( cat observe/jit-replacement-runtime-integration.fk \
      observe/tests/jit-replacement-runtime-integration-band.fk ) > /tmp/jrri.fk
./fkwu --src /tmp/jrri.fk

( cat observe/jit-final-native-gate.fk \
      observe/tests/jit-final-native-gate-band.fk ) > /tmp/jfng.fk
./fkwu --src /tmp/jfng.fk

( cat observe/jit-live-runtime-integration.fk \
      observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk

( cat observe/jit-rung20-readiness.fk \
      observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
```

## Witness

```text
33554431
134217727
4194303
16777215
536870911
```

## Movement

`observe/jit-native-witness-sweep.fk` now rejects the stale
`self-host-completion-sweep = 16777215` receipt after self-host completion moved
to `33554431`. The native witness total moves to `33554431`, so downstream
runtime gates cannot continue to rely on the older self-host completion ledger.

Direct native-witness consumers were updated to require the stronger total:

- `observe/jit-replacement-runtime-integration.fk`
- `observe/jit-final-native-gate.fk`
- `observe/jit-source-replacement-runtime.fk`
- `observe/jit-live-runtime-integration.fk`
- `observe/jit-rung20-dylib-runtime-audit.fk`
- `observe/jit-rung20-readiness.fk`

No C or Rust runtime work was added. This is a receipt-currentness hardening:
the profile/register-derived container live-slot requirement now reaches the
native witness instead of stopping at self-host completion.
