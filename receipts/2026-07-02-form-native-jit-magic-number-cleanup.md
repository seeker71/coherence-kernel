# Form-native JIT magic-number cleanup

Date: 2026-07-02

## Movement

This pass removes repeated raw numeric tags from the active Form-native JIT
container path without touching C or Rust:

- Profile samples now use `jpr-receipt`, so receipt rows read as a named JIT
  profile shape instead of anonymous positional lists.
- Container operation tags are owned by `model/jit-container-lowering.fk` as
  `jcl-op-list`, `jcl-op-array`, `jcl-op-dict`, `jcl-op-hashmap`, and
  `jcl-op-rbtree`.
- The backend aliases those operation names instead of re-declaring raw op
  numbers.
- Byte-plan word sizes and payload encoding bases are named as Form definitions
  before use.
- The JIT track document now names the convention: shared numeric tags should
  have one Form owner at their first semantic boundary.

This is intentionally not a blanket replacement of every integer. Proof-bit
weights, sample evidence values, tuple slot indices, and emitted byte/op stream
values still need to remain compact or receive targeted names in later slices.
The current `fkwu --src` composition envelope still makes huge accessor-only
cleanup risky.

## Witness

```sh
( cat observe/jit-profile-receipt.fk observe/tests/jit-profile-receipt-band.fk ) > /tmp/jpr.fk
./fkwu --src /tmp/jpr.fk
# 127

( cat model/jit-container-lowering.fk model/tests/jit-container-lowering-band.fk ) > /tmp/jcl.fk
./fkwu --src /tmp/jcl.fk
# 16383

( cat model/jit-container-lowering.fk model/jit-container-backend.fk model/tests/jit-container-backend-band.fk ) > /tmp/jcb.fk
./fkwu --src /tmp/jcb.fk
# 65535

( cat model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk model/tests/jit-container-byteplan-band.fk ) > /tmp/jcp.fk
./fkwu --src /tmp/jcp.fk
# 65535

( cat model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/tests/jit-container-loader-band.fk ) > /tmp/jcld.fk
./fkwu --src /tmp/jcld.fk
# 65535

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-backend-register-gate.fk model/jit-container-profile-register-gate.fk model/tests/jit-container-profile-register-gate-band.fk ) > /tmp/jcprg.fk
./fkwu --src /tmp/jcprg.fk
# 8191

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/tests/jit-container-profile-runtime-band.fk ) > /tmp/jcpr.fk
./fkwu --src /tmp/jcpr.fk
# 65535

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/jit-container-replacement-runtime.fk model/tests/jit-container-replacement-runtime-band.fk ) > /tmp/jcrr.fk
./fkwu --src /tmp/jcrr.fk
# 255
```
