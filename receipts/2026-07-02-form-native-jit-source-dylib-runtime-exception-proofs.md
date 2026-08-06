# Form-native JIT source dylib runtime exception proofs

Date: 2026-07-02

The source-dylib runtime executor used to accept bare `1` values for its
`fault` and `exception` proof fields. That made the live execution bridge too
easy to satisfy: the executor could prove byte-image, dispatch, invoke, return,
deopt, melt, and parity shape while still treating runtime exception evidence
as an unnamed boolean.

This patch keeps the work Form-native and does not touch C or Rust. The executor
now carries compact attributed runtime proof receipts:

```fk
("jsdre-runtime-proof" kind op source-path line column span stack-depth)
```

`jsdre-ready?` requires both the fault proof and exception proof to be complete:
known runtime status, non-empty source path, positive source line/column/span,
and a non-empty stack depth. The existing no-C-growth rejection bit also checks
that a malformed exception proof is rejected, so the public witness total stays
stable at `4294967295`.

Witnesses:

```sh
( cat model/form-asm-x64.fk model/jit-self-host-compiler.fk model/jit-source-dylib-runtime-executor.fk model/tests/jit-source-dylib-runtime-executor-band.fk ) > /tmp/jsdre.fk
./fkwu --src /tmp/jsdre.fk
# 4294967295

( cat observe/jit-dylib-live-runtime-proof.fk observe/tests/jit-dylib-live-runtime-proof-band.fk ) > /tmp/jdlrp.fk
./fkwu --src /tmp/jdlrp.fk
# 4294967295

( cat observe/jit-full-track-sweep.fk observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
# 536870911

( cat observe/jit-rung20-readiness.fk observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
# 536870911

( cat observe/jit-native-witness-sweep.fk observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk
# 67108863
```
