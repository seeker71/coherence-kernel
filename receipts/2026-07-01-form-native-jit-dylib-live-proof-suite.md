# Form-native JIT dylib live proof suite

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-dylib-live-proof-suite.fk \
      observe/tests/jit-dylib-live-proof-suite-band.fk ) > /tmp/jdlps.fk
./fkwu --src /tmp/jdlps.fk
# 33554431
```

## Receipt

Added `observe/jit-dylib-live-proof-suite.fk` and its band test. The cell
bridges carrier/install/call evidence and dylib invoke/return/result facts into
the live-execution proof-suite shape: carrier, install, call, exception, deopt,
melt, parity, source/maps, no-C-growth, and positive generation.

This does not claim arbitrary host machine-code execution. It proves the Form
contract that live-execution evidence must now require by receipt. Current
incomplete live proof remains pending; complete proof routes native,
guard-deopt, runtime-exception, invalidation-rewalk, stale-melt, and
parity-deopt. Missing carrier/install/call, exception/deopt/melt/parity,
source/maps, no-C-growth, generation, and stale carrier/invoke/exception
receipts reject before live completion.

No C or Rust runtime work was added. The live proof suite is Form-owned and
composes into `observe/jit-live-execution-evidence.fk` by receipt.
