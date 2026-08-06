# Form-native JIT full-track source-dylib gate

## Witness

```sh
( cat observe/jit-full-track-sweep.fk \
      observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
# 134217727
```

## What changed

`observe/jit-full-track-sweep.fk` now requires the current
`source-dylib-runtime-executor = 4294967295` receipt in addition to the dylib
carrier, install/call, image, slot, carrier ledger, and live proof suite
receipts.

The full-track witness moves from `67108863` to `134217727`, and the stale
`source-dylib-runtime-executor = 33554431` receipt rejects before full-track can
feed post-ingress, host-membrane, or native-witness gates.

Direct full-track consumers were regrounded to the stronger receipt:

- `observe/jit-post-ingress-sweep.fk`
- `observe/jit-host-membrane-readiness.fk`
- `observe/jit-native-witness-sweep.fk`

No C or Rust runtime work was added. This carries the source-selected
array/field/checked-div `.dylib` executor into the compact full JIT ledger while
still leaving actual arbitrary host byte execution pending until carrier,
install, and call evidence is live.
