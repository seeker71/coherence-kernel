# Form-native JIT source orchestrator current container receipt

Date: 2026-07-02

## Movement

The source runtime orchestrator still accepted the older
`container-profile-runtime = 32767` receipt total. The current container profile
runtime witness is `65535` and now includes list and array container profiles
alongside dict/hash/tree.

This patch updates `model/jit-source-runtime-orchestrator.fk` so source runtime
admission requires the current `container-profile-runtime = 65535` receipt. The
existing public orchestrator total stays `1048575`; one reject bit is
strengthened so stale container-profile and stale self-host receipts both reject.

No C or Rust changed.

## Witness

```sh
( cat model/jit-source-runtime-orchestrator.fk model/tests/jit-source-runtime-orchestrator-band.fk ) > /tmp/jsro.fk
./fkwu --src /tmp/jsro.fk
# 1048575

( cat model/form-asm-x64.fk model/jit-self-host-compiler.fk model/jit-source-live-native-executor.fk model/tests/jit-source-live-native-executor-band.fk ) > /tmp/jslne.fk
./fkwu --src /tmp/jslne.fk
# 4194303
```
