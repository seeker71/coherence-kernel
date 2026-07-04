# Form-native JIT live evidence maps and slot-fault bridge

Date: 2026-07-02

## Witness

```sh
( cat observe/jit-live-execution-evidence.fk \
      observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk
# 536870911
```

## Movement

`observe/jit-live-execution-evidence.fk` now makes live replacement-carrier
evidence depend directly on the current `slot-runtime-fault-bridge` receipt.
That carries the already-witnessed source-attributed bounds/null/div/type
fault path into the live execution gate instead of leaving it only implied
through the broader carrier/install/call ledger.

The live proof packet also now carries an explicit maps flag. Contract
readiness requires both source and maps, and the existing reject band now
checks missing proof maps, bad slot-fault bridge evidence, and stale slot-fault
bridge evidence while preserving the public witness total.

This remains Form-only. It does not add C/Rust runtime meaning and does not
claim arbitrary host byte execution.
