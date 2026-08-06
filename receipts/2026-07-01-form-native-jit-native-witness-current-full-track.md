# Form-native JIT native witness current full-track gate

## Witness

```sh
( cat observe/jit-native-witness-sweep.fk \
      observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk
# 67108863
```

## What changed

`observe/jit-native-witness-sweep.fk` now requires the then-current
`full-track-sweep = 67108863` receipt before the hot-recipe native witness can
compose. It also rejects the stale pre-current-dylib `full-track-sweep = 524287`
receipt, raising the native witness from `33554431` to `67108863`.

Later source-to-dylib executor hardening raised the current full-track receipt
again; the native witness now treats `67108863` as stale against
`full-track-sweep = 134217727`.

Direct native-witness consumers were regrounded to the stronger receipt:

- `observe/jit-replacement-runtime-integration.fk`
- `observe/jit-source-replacement-runtime.fk`
- `observe/jit-final-native-gate.fk`
- `observe/jit-live-runtime-integration.fk`
- `observe/jit-rung20-dylib-runtime-audit.fk`
- `observe/jit-rung20-readiness.fk`

No C or Rust runtime work was added. The `.dylib` path remains a carrier/output
boundary owned by Form receipts: ABI, image, load/bind, dispatch, result,
invoke/return, memory, cache, live proof, and runtime audit must all remain
current before downstream runtime gates accept the native witness.
