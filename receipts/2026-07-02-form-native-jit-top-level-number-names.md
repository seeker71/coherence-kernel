# Form-native JIT top-level number naming cleanup

Date: 2026-07-02

## Witness

```sh
( cat observe/jit-rung20-readiness.fk \
      observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
# 536870911

( cat observe/jit-rung20-dylib-runtime-audit.fk \
      observe/tests/jit-rung20-dylib-runtime-audit-band.fk ) > /tmp/jrdra.fk
./fkwu --src /tmp/jrdra.fk
# 268435455

( cat observe/jit-native-witness-sweep.fk \
      observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk
# 67108863

( cat observe/jit-self-host-completion-sweep.fk \
      observe/tests/jit-self-host-completion-sweep-band.fk ) > /tmp/jshcs.fk
./fkwu --src /tmp/jshcs.fk
# 134217727
```

## Movement

The top-level Form-native JIT readiness and sweep cells now name the receipt
totals they compare. The self-host completion sweep also names its proof-bit
weights, so the witness reads as named evidence such as
`jshcs-bit-runtime-exception` instead of anonymous powers of two.

This is a readability cleanup only. The observed witness totals are unchanged,
and the patch stays in Form observe cells with no C/Rust/runtime seed growth.
