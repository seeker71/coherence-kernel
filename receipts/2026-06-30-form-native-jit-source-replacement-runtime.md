# Form Native JIT Source Replacement Runtime

Date: 2026-06-30

## Witness

```sh
( cat observe/jit-source-replacement-runtime.fk \
      observe/tests/jit-source-replacement-runtime-band.fk ) > /tmp/jsrr.fk
./fkwu --src /tmp/jsrr.fk
# 134217727
```

## What Landed

`observe/jit-source-replacement-runtime.fk` connects a hot source recipe to the
replacement runtime handoff. It proves the source recipe must carry static
analysis, profile/category/tier, inline/frame/access/register facts, lower
through Form IR/backend bytes, and enter a replacement runtime with source
attribution, checked exceptions, deopt, melt, and parity.
It now also requires the source-to-dylib runtime executor receipt before the
source replacement contract can pass.

## Proved

- native-witness, replacement-runtime, static-analyzer, source-byte,
  backend-bytes, runtime-stack, source-runtime orchestrator, and source
  live-native executor receipts compose;
- source-to-dylib runtime executor receipts compose into the source replacement
  contract before the source runtime can feed the final native gates;
- a hot source recipe requires source attribution, static clean status,
  profile receipt, numeric category collapse, tiering, inline/frame/access
  facts, register facts, and positive generation;
- Form lowering requires Form ownership, IR, backend selection, exact byte
  count, valid bytes, source/maps, payload readiness, and positive generation;
- replacement runtime requires replacement/no-C-growth state, W^X, frame,
  stack, exception, deopt, melt, parity, and positive generation;
- current non-live runtime stays pending;
- future live replacement runtime routes native, guard deopt, runtime exception,
  invalidation rewalk, parity deopt, stale melt, unavailable-architecture deopt,
  and mismatch reject;
- C-bootstrap growth rejects;
- missing source attribution, bad static state, missing category collapse,
  missing backend, bad byte count, foreign lowering owner, missing stack,
  missing exception maps, bad source-runtime orchestrator/executor receipts,
  missing source-dylib executor receipts, and bad replacement-runtime receipts
  reject readiness.

## Honest Boundary

This still does not mark rung 20 complete. It proves the source-to-runtime
contract that a real replacement carrier must satisfy when hot recipes execute
as installed Form-emitted bytes.
