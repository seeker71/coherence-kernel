# Form-native JIT self-host live-evidence carrier gate

Date: 2026-07-01

This receipt tightens the Form-only self-host live-evidence band so it consumes
the current `.dylib` carrier ledger and live-execution evidence totals without
adding C or Rust runtime code.

Commands:

```sh
( cat observe/jit-profile-receipt.fk model/form-asm-x64.fk model/jit-self-host-compiler.fk model/jit-self-host-profile-runtime.fk model/jit-self-host-ingress-runtime.fk observe/jit-native-call-plan.fk model/jit-self-host-native-call-runtime.fk model/jit-self-host-host-membrane-runtime.fk model/jit-self-host-live-evidence.fk model/jit-self-host-final-native-gate.fk model/tests/jit-self-host-live-evidence-band.fk ) > /tmp/jshl.fk
./fkwu --src /tmp/jshl.fk
# 1048575

( cat observe/jit-self-host-completion-sweep.fk observe/tests/jit-self-host-completion-sweep-band.fk ) > /tmp/jshcs.fk
./fkwu --src /tmp/jshcs.fk
# 33554431
```

The self-host live-evidence band now validates receipt tuples for:

- `carrier-install-call-evidence = 4294967295`
- `live-execution-evidence = 16777215`
- stale carrier evidence rejection

The self-host completion sweep now requires `self-host-live-evidence = 1048575`
and, after the later profile container slot gate, also requires
`container-profile-live-slot-runtime = 1048575`. It rejects the stale `262143`
live-evidence receipt before downstream native witness, live-runtime, and
rung-20 readiness receipts can pass.
