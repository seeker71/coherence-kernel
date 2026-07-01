# Form-native JIT dylib invoke/return lifecycle evidence

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-dylib-invoke-return-evidence.fk \
      observe/tests/jit-dylib-invoke-return-evidence-band.fk ) > /tmp/jdire.fk
./fkwu --src /tmp/jdire.fk
# 268435455
```

## Receipt

Added `observe/jit-dylib-invoke-return-evidence.fk` and its band test. The cell
bridges the call-result shape into the lifecycle facts required before a dylib
result can count toward carrier/install/call completion: installed slot,
callable slot, rooted args, W^X, invoked state, returned state, known status,
stack attribution, source/maps, exception/deopt/melt support, parity,
no-C-growth, and positive generation.

This keeps the `.dylib` path explicit. Load/bind proves handles and symbols;
dispatch proves a callable ticket may be entered; result evidence proves the
return/fault shape; invoke/return evidence proves the lifecycle facts the
carrier ledger requires. Current lifecycle state remains pending until invoked
and returned facts are present. Complete lifecycle evidence routes native,
guard-deopt, runtime-exception, invalidation-rewalk, stale-melt, and
parity-deopt. Unavailable lifecycles deopt; mismatched lifecycles reject;
fault/trap lifecycles route exception; missing dispatch, installed slot,
callability, args, W^X, stack, source/maps, no-C-growth, generation,
invoke/return facts, and stale result/install/stack receipts reject before
completion.

No C or Rust runtime work was added. The invoke/return lifecycle is Form-owned
and composes into carrier/install/call evidence by receipt.
