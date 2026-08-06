# Form-native JIT checked modulo payload

Date: 2026-07-02

The checked-access payload bridge already emitted Form-native x64 byte fragments
for array/list indexed load, field load, and signed division. Runtime fault
receipts already named `mod-zero`, but the checked-access payload layer did not
yet carry a modulo byte path.

This patch keeps the work Form-native and does not touch C or Rust. It adds:

- `x64-mod-checked` to `model/form-asm-x64.fk`
- source-attributed modulo fault receipts through the existing runtime-fault
  receipt shape
- access kind `4` as the checked modulo payload kind
- checked modulo payload, byte-equality, idiv marker, and attributed mod-zero
  witness bits

The checked-access witness moves from `2097151` to `33554431`. Dependent ledgers
that consume the checked-access receipt now expect the stronger total:

- `observe/jit-native-witness-sweep.fk`
- `observe/jit-full-track-sweep.fk`
- `observe/jit-host-exception-bridge.fk`
- `observe/jit-bootstrap-exit-carrier.fk`

Witness:

```sh
( cat model/form-asm-x64.fk \
      observe/jit-runtime-fault.fk \
      observe/jit-loader-contract.fk \
      observe/jit-checked-access-payload.fk \
      observe/tests/jit-checked-access-payload-band.fk ) > /tmp/jcap.fk
./fkwu --src /tmp/jcap.fk
# 33554431
```
