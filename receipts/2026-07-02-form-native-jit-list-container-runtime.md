# Form-native JIT list container runtime

Date: 2026-07-02

## Movement

List container access was already represented in the lowering/backend/byteplan
lane, but the profile-derived container runtime path stopped at array, dict,
hashmap, and red-black tree.

This patch carries cons-list head access into the same Form-native runtime lane:

- `jpr-list-sample` gives the profile/category layer a reusable hot list
  receipt.
- `jit-container-profile-register-gate` maps list receipts to `jcb-list` and
  proves list schedules pass the register-aware CPU backend gate.
- `jit-container-profile-runtime` derives list byteplans from profile receipts.
- `jit-container-replacement-runtime` returns replacement-compatible list bytes.
- `jit-container-live-slot-runtime` adds a list slot with attributed exception,
  byte-count, source/maps, deopt/melt/parity, and live-complete checks.
- `jit-container-profile-live-slot-runtime` derives list live slots from the
  profile/register gate.
- `jit-self-host-container-runtime-loop` now requires list and array container
  slots together under the existing proof total.

The public totals stay stable by strengthening existing array witness bits into
array+list witness bits. This avoids downstream receipt churn while making the
container runtime claim more true.

No C or Rust changed.

## Witness

```sh
( cat observe/jit-profile-receipt.fk observe/tests/jit-profile-receipt-band.fk ) > /tmp/jpr.fk
./fkwu --src /tmp/jpr.fk
# 127

( cat model/jit-container-live-slot-runtime.fk model/tests/jit-container-live-slot-runtime-band.fk ) > /tmp/jclsr.fk
./fkwu --src /tmp/jclsr.fk
# 1048575

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-backend-register-gate.fk model/jit-container-profile-register-gate.fk model/tests/jit-container-profile-register-gate-band.fk ) > /tmp/jcprg.fk
./fkwu --src /tmp/jcprg.fk
# 8191

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/tests/jit-container-profile-runtime-band.fk ) > /tmp/jcpr.fk
./fkwu --src /tmp/jcpr.fk
# 65535

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/jit-container-replacement-runtime.fk model/tests/jit-container-replacement-runtime-band.fk ) > /tmp/jcrr.fk
./fkwu --src /tmp/jcrr.fk
# 255

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-backend-register-gate.fk model/jit-container-profile-register-gate.fk model/jit-container-live-slot-runtime.fk model/jit-container-profile-live-slot-runtime.fk model/tests/jit-container-profile-live-slot-runtime-band.fk ) > /tmp/jcplsr.fk
./fkwu --src /tmp/jcplsr.fk
# 1048575

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-backend-register-gate.fk model/jit-container-profile-register-gate.fk model/jit-container-live-slot-runtime.fk model/jit-container-profile-live-slot-runtime.fk model/jit-self-host-container-runtime-loop.fk model/tests/jit-self-host-container-runtime-loop-band.fk ) > /tmp/jshcrl.fk
./fkwu --src /tmp/jshcrl.fk
# 1048575
```
