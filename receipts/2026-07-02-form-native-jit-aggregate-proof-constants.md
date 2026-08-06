# Form-native JIT aggregate proof constants

Date: 2026-07-02

`observe/jit-native-witness-sweep.fk` is the compact aggregate gate for the
Form-native JIT track. It used to expose receipt totals, bad totals, stale
totals, tuple slots, and witness bit masks as raw numbers inside the proof.

This patch keeps the public witness total stable while naming the numeric
contract:

- receipt, recipe, and carrier tuple positions now have named slot accessors
- receipt totals are derived from named bit counts with `jnws-mask`
- bad receipt totals use `jnws-one-short`
- stale receipt totals use `jnws-stale-half`
- witness assertion bits derive from the previous named bit with
  `jnws-next-bit`

The result is still Form-native and does not touch C or Rust. The checksum stays
compatible with downstream JIT gates, but the proof now reads as named optimizer
coverage instead of a field of magic numbers.

Witness:

```sh
( cat observe/jit-native-witness-sweep.fk observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk
# 67108863
```
