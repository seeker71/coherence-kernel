# Form-native JIT dylib live runtime proof

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-dylib-live-runtime-proof.fk \
      observe/tests/jit-dylib-live-runtime-proof-band.fk ) > /tmp/jdlrp.fk
./fkwu --src /tmp/jdlrp.fk
# 536870911
```

## Receipt

Added `observe/jit-dylib-live-runtime-proof.fk` and its band test. The cell
bridges the dylib live proof suite into runtime-fault and full-stack attribution
readiness before the general live-runtime integration can treat the dylib
carrier as a hot Form runtime path.

This remains Form-only. It proves the receipt-level runtime contract: live
suite, live execution evidence, runtime-fault facts, stack attribution,
source/maps, no-C-growth, carrier/install/call, exception/deopt/melt/parity,
and positive generation. Current incomplete runtime state remains pending.
Complete runtime proof routes native, guard-deopt, runtime-exception,
invalidation-rewalk, stale-melt, and parity-deopt. Missing live/runtime/stack/
fault facts, carrier/install/call, exception/deopt/melt/parity, source/maps,
no-C-growth, generation, and stale live-suite/live-evidence/stack receipts
reject before runtime completion.

No C or Rust runtime work was added. The dylib live-runtime proof composes into
`observe/jit-live-runtime-integration.fk` by receipt.
