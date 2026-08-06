# Form-native JIT container lowering metadata gate

Date: 2026-07-01

## Commands

```sh
( cat model/jit-container-lowering.fk model/tests/jit-container-lowering-band.fk ) > /tmp/jcl.fk
./fkwu --src /tmp/jcl.fk

( cat model/jit-container-lowering.fk model/tests/jit-container-lowering-metadata-band.fk ) > /tmp/jclm.fk
./fkwu --src /tmp/jclm.fk

( cat model/jit-container-lowering.fk model/jit-container-backend.fk model/tests/jit-container-backend-band.fk ) > /tmp/jcb.fk
./fkwu --src /tmp/jcb.fk

( cat model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk model/tests/jit-container-byteplan-band.fk ) > /tmp/jcp.fk
./fkwu --src /tmp/jcp.fk

( cat model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/tests/jit-container-loader-band.fk ) > /tmp/jcld.fk
./fkwu --src /tmp/jcld.fk

( cat model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-source-runtime.fk model/tests/jit-container-source-runtime-band.fk ) > /tmp/jcsr.fk
./fkwu --src /tmp/jcsr.fk

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/tests/jit-container-profile-runtime-band.fk ) > /tmp/jcpr.fk
./fkwu --src /tmp/jcpr.fk

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/jit-container-replacement-runtime.fk model/tests/jit-container-replacement-runtime-band.fk ) > /tmp/jcrr.fk
./fkwu --src /tmp/jcrr.fk

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/jit-container-replacement-runtime.fk model/jit-self-host-container-live-evidence.fk model/tests/jit-self-host-container-live-evidence-band.fk ) > /tmp/jshcl.fk
./fkwu --src /tmp/jshcl.fk
```

## Witness

```text
2047
31
4095
4095
8191
65535
32767
255
4194303
```

## Movement

`model/jit-container-lowering.fk` now carries the representation-specialization
receipt total, source-attributed fault proof, fallback proof, key category,
collision status, and ordered-tree proof in the Form-native container lowering
plan. The new metadata band rejects stale representation receipts, missing
fault/fallback proof, hash collision plans, and unordered red-black-tree plans
before backend byteplan expansion.

This keeps the larger profile/runtime path inside the current `fkwu --src`
source-runner envelope by leaving the original stream-shape band compact and
placing the added metadata assertions in a focused band.
