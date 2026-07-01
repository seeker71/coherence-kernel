# Form-native JIT final gate current replacement receipt

Commands:

```sh
( cat observe/jit-final-native-gate.fk \
      observe/tests/jit-final-native-gate-band.fk ) > /tmp/jfng.fk
./fkwu --src /tmp/jfng.fk

( cat observe/jit-live-execution-evidence.fk \
      observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk

( cat observe/jit-live-runtime-integration.fk \
      observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk

( cat observe/jit-rung20-readiness.fk \
      observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
```

Outputs:

```txt
8388607
67108863
16777215
536870911
```

`observe/jit-final-native-gate.fk` now rejects the stale
`replacement-runtime-integration = 134217727` receipt after the replacement
runtime moved to `268435455`. Direct live consumers now require
`final-native-gate = 8388607` before accepting the final gate receipt.

Touched:

- `observe/jit-final-native-gate.fk`
- `observe/jit-live-execution-evidence.fk`
- `observe/jit-live-runtime-integration.fk`
- `observe/jit-rung20-readiness.fk`
- `docs/form-native-jit-track.form`
