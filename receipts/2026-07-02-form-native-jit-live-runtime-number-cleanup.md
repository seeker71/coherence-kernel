# Form-native JIT runtime number cleanup

Date: 2026-07-02

## Movement

`observe/jit-source-replacement-runtime.fk` and
`observe/jit-live-runtime-integration.fk` no longer leave active runtime
receipt/proof ledgers as fields of raw packed integers:

- receipt totals are derived from named bit-width masks with `jlri-mask`
- source-runtime totals are derived from named bit-width masks with `jsrr-mask`
- stale and bad totals now name the receipt they are stale/bad against
- witness bits now use names such as `jlri-bit-runtime-fault-excepts`,
  `jlri-bit-missing-carrier-rejects`, and
  `jlri-bit-stale-dylib-runtime-receipts-reject`

This is readability work on the active Form-native JIT live-runtime bridge. It
does not touch C or Rust and it does not claim arbitrary host byte execution.

## Witness

```sh
( cat observe/jit-source-replacement-runtime.fk \
      observe/tests/jit-source-replacement-runtime-band.fk ) > /tmp/jsrr.fk
./fkwu --src /tmp/jsrr.fk
# 536870911

( cat observe/jit-live-runtime-integration.fk \
      observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk
# 67108863
```
