# Form-native JIT array/list container lowering

Date: 2026-07-02

This receipt extends the compact Form-native container JIT path beyond
dict/hashmap/red-black-tree lookup streams. The container lowerer, backend
scheduler, byteplan admission, loader bridge, profile runtime, register gates,
and replacement bridge now carry indexed array access; the lowerer/backend/
byteplan/loader path also carries list head/tail access.

No C or Rust changed.

## Witnesses

```sh
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

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/tests/jit-container-profile-runtime-band.fk ) > /tmp/jcpr.fk
./fkwu --src /tmp/jcpr.fk
# 65535

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-backend-register-gate.fk model/jit-container-profile-register-gate.fk model/tests/jit-container-profile-register-gate-band.fk ) > /tmp/jcprg.fk
./fkwu --src /tmp/jcprg.fk
# 8191

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/jit-container-replacement-runtime.fk model/tests/jit-container-replacement-runtime-band.fk ) > /tmp/jcrr.fk
./fkwu --src /tmp/jcrr.fk
# 255
```

## Notes

The direct-source composition envelope is still tight. A first pass named every
container op/register/key helper as a separate `defn`, but the full profile
runtime then lost late definitions under `fkwu --src`. The landed version keeps
the op table documented in the lowerer (`4=list`, `5=array`, `6=dict`,
`7=hashmap`, `8=red-black-tree`) and avoids helper-function growth in the hot
composed path.

This is not a retreat to C lowering. The streams, backend schedules, byteplans,
loader actions, register gates, profile runtime admission, and replacement
payload readiness are all Form data witnessed on the bootstrapped `fkwu`.
