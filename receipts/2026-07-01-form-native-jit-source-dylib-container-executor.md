# Form Native JIT Source Dylib Container Executor

Date: 2026-07-01

## Commands

```sh
( cat model/jit-source-dylib-container-executor.fk \
      observe/jit-profile-receipt.fk \
      model/jit-container-lowering.fk \
      model/jit-container-backend.fk \
      model/jit-container-byteplan.fk \
      observe/jit-loader-contract.fk \
      model/jit-container-loader.fk \
      model/jit-container-profile-runtime.fk \
      model/jit-container-replacement-runtime.fk \
      model/tests/jit-source-dylib-container-executor-band.fk ) > /tmp/jsdce.fk
./fkwu --src /tmp/jsdce.fk

( cat observe/jit-full-track-sweep.fk; echo '(jit-full-track-sweep-check)' ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
```

## Witness

`model/jit-source-dylib-container-executor.fk` adds the compact Form-native
admission/completion/route bridge for dict/hashmap/red-black-tree container
payloads entering the source-dylib executor ledger. The bridge does not load
host code or add C/Rust; it consumes the existing container profile,
replacement, and byte-readiness facts as Form data.

The focused band returns `4194303`. The full-track sweep now requires
`source-dylib-container-executor = 4194303` and returns `268435455`.

The bridge keeps `.dylib` as the live-execution carrier boundary while keeping
JIT semantics in Form: source/maps, stack, fault, exception, deopt, melt,
parity, no-C-growth, generation, pending install, native, guard-deopt,
runtime-exception, invalidation-rewalk, stale-melt, unavailable, mismatch,
fault, and trap routes are all accounted for before the full-track receipt
admits the container path.
