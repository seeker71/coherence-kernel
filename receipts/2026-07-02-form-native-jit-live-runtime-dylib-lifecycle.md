# Receipt - live runtime consumes the dylib lifecycle receipts (2026-07-02)

## Movement

`observe/jit-live-runtime-integration.fk` now directly requires the concrete
`.dylib` lifecycle receipts before live runtime integration can pass:

- `dylib-memory-envelope = 33554431`
- `dylib-load-bind-runtime = 268435455`
- `dylib-invoke-return-evidence = 268435455`

The public `live-runtime-integration` witness stays stable at `67108863`. The
existing bad/stale dylib runtime receipt bits now also prove that bad or stale
memory/load/invoke lifecycle receipts reject before the live runtime can count
carrier/install/call facts as complete.

## Why

The track already had Form-native cells for memory restrictions, symbol
load/bind tickets, and invoke/return lifecycle evidence. The live runtime bridge
still accepted generic carrier/install/call flags without directly requiring
those concrete `.dylib` lifecycle proofs.

This closes that integration edge without adding C or Rust work. The host
carrier remains pending as a real call boundary, but Form now owns more of the
pre-call contract: bounded W^X memory, source/deopt maps, bound install/call
symbols, invoked/returned status, and stale lifecycle rejection.

## Witness

```sh
( cat observe/jit-live-runtime-integration.fk \
      observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk
# -> 67108863
```
