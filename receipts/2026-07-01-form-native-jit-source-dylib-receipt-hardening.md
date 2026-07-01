# Form-native JIT source dylib receipt hardening

## Witness

```sh
( cat model/form-asm-x64.fk \
      model/jit-self-host-compiler.fk \
      model/jit-source-dylib-runtime-executor.fk \
      model/tests/jit-source-dylib-runtime-executor-band.fk ) > /tmp/jsdre.fk
./fkwu --src /tmp/jsdre.fk
# 4294967295
```

## What changed

`model/jit-source-dylib-runtime-executor.fk` now rejects every direct dylib
lifecycle receipt that the source-to-dylib route depends on:

- profile-derived dylib slot runtime, including stale pre-current totals
- install/call attempt
- Form-emitted carrier probe
- dylib call dispatch evidence
- dylib call result evidence
- dylib invoke/return evidence

The witness moves from `33554431` to `4294967295`. Direct consumers now require
`source-dylib-runtime-executor = 4294967295`:

- `observe/jit-source-replacement-runtime.fk`
- `observe/jit-dylib-live-runtime-proof.fk`

No C or Rust runtime work was added. This keeps the live `.dylib` path
Form-owned: source-selected array, field, and checked-div byte images must agree
with current profile slot, install/call, carrier, dispatch, result, and
invoke/return receipts before the runtime proof can count the route.
