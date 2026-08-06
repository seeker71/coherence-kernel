# Form-native JIT self-host live maps and slot-fault bridge

Date: 2026-07-02

## Witness

```sh
( cat observe/jit-profile-receipt.fk \
      model/form-asm-x64.fk \
      model/jit-self-host-compiler.fk \
      model/jit-self-host-profile-runtime.fk \
      model/jit-self-host-ingress-runtime.fk \
      observe/jit-native-call-plan.fk \
      model/jit-self-host-native-call-runtime.fk \
      model/jit-self-host-host-membrane-runtime.fk \
      model/jit-self-host-live-evidence.fk \
      model/jit-self-host-final-native-gate.fk \
      model/tests/jit-self-host-live-evidence-band.fk ) > /tmp/jshl.fk
./fkwu --src /tmp/jshl.fk
# 1048575
```

## Movement

`model/jit-self-host-live-evidence.fk` now mirrors the current live-execution
proof shape. The self-host bridge requires source, maps, slot-fault evidence,
no-C-growth, and positive generation before it can call a live proof complete.

The witness band now uses named receipt helpers for `carrier-install-call-
evidence`, `live-execution-evidence`, and `slot-runtime-fault-bridge` instead
of rebuilding anonymous receipt lists inline. It also rejects missing proof
maps, missing slot-fault evidence, bad receipt totals, missing receipt source,
and missing receipt maps while preserving the public witness total.

This stays Form-native and does not claim arbitrary host byte execution.
