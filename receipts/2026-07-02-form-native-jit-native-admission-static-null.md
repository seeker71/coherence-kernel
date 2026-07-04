# Form-native JIT native admission rejects static null direct access

Date: 2026-07-02

## Movement

`observe/jit-native-admission.fk` now proves that a known-null direct access
cannot pass the final Form-owned native admission gate. The static analyzer
already rejected known-null array/field/dict/hash/tree receivers; this patch
carries that fact into native admission with a concrete `array-get nothing`
admission case.

The native-admission witness grows from `131071` to `262143`. Downstream
aggregate receipt widths were updated in:

- `observe/jit-full-track-sweep.fk`
- `observe/jit-native-witness-sweep.fk`

No C or Rust runtime work was added. This does not claim arbitrary host byte
execution; it prevents a statically known null direct access from reaching that
future boundary.

## Witness

```sh
( cat observe/jit-runtime-fault.fk \
      observe/form-static-analyzer.fk \
      observe/jit-backend-bytes.fk \
      observe/jit-loader-contract.fk \
      observe/jit-native-admission.fk \
      observe/tests/jit-native-admission-band.fk ) > /tmp/jna.fk
./fkwu --src /tmp/jna.fk
# 262143

( cat observe/jit-full-track-sweep.fk \
      observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
# 536870911

( cat observe/jit-native-witness-sweep.fk \
      observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk
# 67108863
```
