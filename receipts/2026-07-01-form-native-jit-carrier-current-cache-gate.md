# Form-native JIT carrier current-cache gate

Commands:

```sh
( cat observe/jit-carrier-current-cache-gate.fk \
      observe/tests/jit-carrier-current-cache-gate-band.fk ) > /tmp/jcccg.fk
./fkwu --src /tmp/jcccg.fk

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
8191
134217727
16777215
536870911
```

`observe/jit-carrier-install-call-evidence.fk` is already at its saturated
`4294967295` witness band. `observe/jit-carrier-current-cache-gate.fk` carries
the current carrier cache and pressure-memory receipts forward as a companion
gate so live-execution evidence can reject stale `dylib-memory-envelope =
1048575` and stale `dylib-cache-lifecycle = 8388607` receipts without growing
the saturated carrier ledger.

Direct consumers now require `carrier-current-cache-gate = 8191` through
`observe/jit-live-execution-evidence.fk`, whose witness moves to `134217727`.

No C/Rust runtime code changed.
