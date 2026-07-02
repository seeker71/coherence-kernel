# Form-native JIT container lowering names

Date: 2026-07-02

The container lowering cell had repeated raw `1023` representation proof totals
and full ready-plan tuples like:

```fk
(jcl-plan 1 1 7 1 1 1 1023 1023 1 1 1 0 0)
```

That shape was difficult to read, especially for new contributors trying to
understand which numbers are semantic flags and which are proof-band totals.

This receipt keeps the cleanup native/Form-only:

- `jcl-repr-complete-total` names the `1023` representation-specialization
  proof total used by container direct-access lowering.
- `jcl-ready-plan` names the repeated happy-path plan shape for dict, hashmap,
  and red-black-tree lowering.
- `jit-container-lowering-metadata-band` now builds bad metadata cases through
  local helper constructors instead of repeating full positional tuples.

The cleanup is intentionally compact. The larger profile/replacement runtime
composition is already close to the current `fkwu --src` symbol ceiling, so a
first pass that added a full field-accessor family pushed composed witnesses
over the edge. This patch keeps the public lowering prelude to two added
definitions and leaves deeper schema cleanup for the next source-runner capacity
or parser/core refactor pass.

Witnesses:

```sh
( cat model/jit-container-lowering.fk model/tests/jit-container-lowering-band.fk ) > /tmp/jcl.fk
./fkwu --src /tmp/jcl.fk
# 2047

( cat model/jit-container-lowering.fk model/tests/jit-container-lowering-metadata-band.fk ) > /tmp/jclm.fk
./fkwu --src /tmp/jclm.fk
# 31

( cat observe/jit-profile-receipt.fk model/jit-container-lowering.fk model/jit-container-backend.fk model/jit-container-byteplan.fk observe/jit-loader-contract.fk model/jit-container-loader.fk model/jit-container-profile-runtime.fk model/jit-container-replacement-runtime.fk model/tests/jit-container-replacement-runtime-band.fk ) > /tmp/jcrr.fk
./fkwu --src /tmp/jcrr.fk
# 255
```
