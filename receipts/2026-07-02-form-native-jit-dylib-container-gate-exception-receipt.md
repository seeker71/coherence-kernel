# Form-native JIT dylib container gate exception receipt

Date: 2026-07-02

The compact `dylib-live-runtime-container-gate` carries source-dylib container
execution into live runtime integration. Before this patch, its completion
contract still accepted a bare `1` in the `exception` proof slot.

This patch keeps the work Form-native and does not touch C or Rust. The gate now
requires an attributed exception receipt:

```fk
("dylib-container-exception" path line column span stack-depth)
```

`jdlrcg-complete?` accepts the gate only when that receipt has a non-empty
source path and positive source/stack coordinates. The existing zero-generation
rejection bit also proves a malformed exception receipt is rejected, so the
public witness total remains stable.

Witnesses:

```sh
( cat observe/jit-dylib-live-runtime-container-gate.fk observe/tests/jit-dylib-live-runtime-container-gate-band.fk ) > /tmp/jdlrcg.fk
./fkwu --src /tmp/jdlrcg.fk
# 134217727

( cat observe/jit-live-runtime-integration.fk observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk
# 67108863

( cat observe/jit-full-track-sweep.fk observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
# 536870911

( cat observe/jit-rung20-readiness.fk observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
# 536870911
```
