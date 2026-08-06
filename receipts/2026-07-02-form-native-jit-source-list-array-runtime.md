# Form-native JIT source list/array runtime

Date: 2026-07-02

## Movement

`model/jit-container-source-runtime.fk` already accepted list and array
container op tags through the shared container backend, but its positive source
recipes only witnessed dict, hashmap, and GPU red-black-tree paths.

This patch adds source-level hot recipes for:

- `jcb-list` on the CPU container path
- `jcb-array` on the CPU container path

The existing public witness total stays `65535`; several bits are strengthened
so they now prove list/array alongside the older dict/hash/tree source runtime
facts:

- source recipes load for list plus dict
- source recipes load for array plus hashmap
- code-size facts cover list `16`, array `20`, and GPU tree `112`
- native routing covers list plus hashmap

No C or Rust changed.

## Witness

```sh
( cat model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-source-runtime.fk model/tests/jit-container-source-runtime-band.fk ) > /tmp/jcsr.fk
./fkwu --src /tmp/jcsr.fk
# 65535

( cat observe/jit-full-track-sweep.fk observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
# 536870911

( cat observe/jit-native-witness-sweep.fk observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk
# 67108863
```
