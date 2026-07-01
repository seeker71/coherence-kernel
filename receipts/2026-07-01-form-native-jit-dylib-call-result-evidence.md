# Form-native JIT dylib call result evidence

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-dylib-call-result-evidence.fk \
      observe/tests/jit-dylib-call-result-evidence-band.fk ) > /tmp/jdcre.fk
./fkwu --src /tmp/jdcre.fk
# 134217727
```

## Receipt

Added `observe/jit-dylib-call-result-evidence.fk` and its band test. The cell
bridges callable dylib dispatch into the post-call result contract: invoked and
returned state, known status, tagged return value, stack attribution,
source/maps, exception/deopt/melt support, parity, no-C-growth, and positive
generation.

This keeps the `.dylib` path precise. Load/bind proves handles and symbols;
dispatch proves a callable ticket may be entered; result evidence proves the
return/fault boundary before live execution may claim a Form-owned result.
Current result state remains pending until invoked and returned facts are live.
Complete result evidence routes native, guard-deopt, runtime-exception,
invalidation-rewalk, stale-melt, and parity-deopt. Unavailable results deopt;
mismatched results reject; fault/trap results route exception; missing dispatch,
value tag, stack, source/maps, parity, no-C-growth, generation, invoke/return
facts, and stale dispatch/stack/exception receipts reject before completion.

No C or Rust runtime work was added. The result contract is Form-owned and
composes into carrier/install/call evidence by receipt.
