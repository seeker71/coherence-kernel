# Form-native JIT dylib load/bind runtime

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-dylib-load-bind-runtime.fk \
      observe/tests/jit-dylib-load-bind-runtime-band.fk ) > /tmp/jdlb.fk
./fkwu --src /tmp/jdlb.fk
# 268435455
```

## Receipt

Added `observe/jit-dylib-load-bind-runtime.fk` and its band test. The cell
bridges the dylib image manifest receipt into compact Form-owned runtime state:
open handle, bound carrier symbols, W^X, sealing, source/maps, no-C-growth,
generation, and install/call tickets.

The load/bind runtime keeps current live execution honest. A current ticket is
ready but pending until both install and call are present. A complete ticket
routes native, guard-deopt, runtime-exception, invalidation-rewalk, stale-melt,
and parity-deopt. Unavailable loads deopt; mismatched loads reject; faulted
loads route exception; missing symbols, bad addresses, C-owner images, zero
generation, missing install/call, missing parity, and stale manifest receipts
reject before live completion.

No C or Rust runtime work was added. The `.dylib` remains the carrier artifact;
the JIT meaning and load/call evidence are still described and gated by Form.
