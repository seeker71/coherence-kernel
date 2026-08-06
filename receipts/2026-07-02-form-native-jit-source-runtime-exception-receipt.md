# Form-native JIT source runtime exception receipt

Date: 2026-07-02

The source replacement runtime bridge connects a hot source recipe to the
replacement runtime path. Before this patch, its runtime record still accepted a
bare `1` for the exception proof slot.

This patch keeps the work Form-native and does not touch C or Rust. The runtime
record now requires an attributed exception receipt:

```fk
("source-runtime-exception" path line column span stack-depth)
```

`jsrr-runtime-ready?` accepts runtime completion only when that receipt has a
non-empty source path, positive source coordinates, and a positive stack depth.
The public witness total remains stable because the existing
`jsrr-no-exception-runtime` rejection now proves malformed receipt rejection
instead of boolean absence.

Witnesses:

```sh
( cat observe/jit-source-replacement-runtime.fk observe/tests/jit-source-replacement-runtime-band.fk ) > /tmp/jsrr.fk
./fkwu --src /tmp/jsrr.fk
# 536870911

( cat observe/jit-final-native-gate.fk observe/tests/jit-final-native-gate-band.fk ) > /tmp/jfng.fk
./fkwu --src /tmp/jfng.fk
# 8388607

( cat observe/jit-live-execution-evidence.fk observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk
# 536870911

( cat observe/jit-dylib-live-runtime-proof.fk observe/tests/jit-dylib-live-runtime-proof-band.fk ) > /tmp/jdlrp.fk
./fkwu --src /tmp/jdlrp.fk
# 4294967295
```
