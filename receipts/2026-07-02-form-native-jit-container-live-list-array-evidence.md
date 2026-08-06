# Form-native JIT container live list/array evidence

Date: 2026-07-02

## Movement

List and array container plans now reach profile runtime, replacement runtime,
source runtime, live slots, and self-host container runtime. The downstream
live-evidence and source-dylib container bands were still centered on dict,
hashmap, and red-black tree.

This patch strengthens those repository-native witnesses:

- `jit-self-host-container-live-evidence-band` now requires list and array
  replacement readiness and byte shapes alongside dict/hash/tree.
- The high witness bit now proves list and array are accepted container profiles
  instead of expecting array to reject.
- `jit-source-dylib-container-executor-band` now admits list and array plans and
  checks their replacement byte lengths before the source-dylib executor can
  claim its stable total.
- The track document now describes list/array in the container live-evidence and
  source-dylib executor rungs.

No C or Rust changed.

## Witness

```sh
( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/jit-container-replacement-runtime.fk model/jit-self-host-container-live-evidence.fk model/tests/jit-self-host-container-live-evidence-band.fk ) > /tmp/jshcl.fk
./fkwu --src /tmp/jshcl.fk
# 4194303

( cat model/jit-source-dylib-container-executor.fk observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/jit-container-replacement-runtime.fk model/tests/jit-source-dylib-container-executor-band.fk ) > /tmp/jsdce.fk
./fkwu --src /tmp/jsdce.fk
# 4194303
```
