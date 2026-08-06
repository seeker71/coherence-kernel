# Form-native JIT source dylib runtime executor

Date: 2026-07-01

## Witness

```sh
( cat model/form-asm-x64.fk \
      model/jit-self-host-compiler.fk \
      model/jit-source-dylib-runtime-executor.fk \
      model/tests/jit-source-dylib-runtime-executor-band.fk ) > /tmp/jsdre.fk
./fkwu --src /tmp/jsdre.fk
# 16777215
```

## Receipt

Added `model/jit-source-dylib-runtime-executor.fk` and its band test. The cell
bridges hot source compiler output into the dylib dispatch/result/invoke
lifecycle without adding C or Rust and without claiming arbitrary host byte
execution.

It checks exact Form-emitted array, field, and div byte images from
`model/jit-self-host-compiler.fk`, then composes source-live, profile dylib
slot, install/call, Form-emitted carrier, dispatch, result, and invoke/return
receipts into one source-to-dylib execution route.

The route remains pending until installed and callable state is live. A future
complete route selects native, guard failures deopt, runtime faults throw,
invalidations rewalk, stale slots melt, parity failures deopt, unavailable
carriers deopt, mismatches reject, and fault/trap statuses throw. Missing
source, bytes, byte counts, stack proof, no-C-growth, or source-executor
receipts reject before completion.
