# Form Native JIT Replacement Runtime Integration

Date: 2026-06-30

## Witness

```sh
( cat observe/jit-replacement-runtime-integration.fk \
      observe/tests/jit-replacement-runtime-integration-band.fk ) > /tmp/jrri.fk
./fkwu --src /tmp/jrri.fk
# 134217727
```

## What Landed

`observe/jit-replacement-runtime-integration.fk` defines the runtime handoff
contract between the hot-recipe native sweep and final rung 20 completion. It
does not install or call host machine code. It proves what a replacement
carrier must provide before Form-emitted bytes can be treated as native.

## Proved

- native-witness, bootstrap-exit, host-exception, runtime-stack, byte-ingress,
  and loader receipts compose;
- a Form-owned byte payload must have exact byte count, valid bytes, known ABI,
  tagged arguments, source/guard/fault/deopt/root maps, and positive generation;
- a replacement slot must be non-C-growth, W^X, executable, non-writable,
  sealed, installed, valid, and generationed;
- a call frame must carry rooted args, full stack attribution, source maps,
  exception maps, and parity;
- a not-yet-installed replacement slot stays pending;
- a complete replacement runtime routes native, guard deopt, runtime exception,
  invalidation rewalk, parity deopt, stale melt, unavailable-architecture deopt,
  and carrier mismatch reject;
- C-bootstrap growth rejects;
- malformed byte counts, invalid bytes, foreign owners, missing fault maps,
  untagged args, writable slots, stale slots, missing stack, missing exception
  maps, bad receipt totals, missing source maps, and missing metadata maps
  reject readiness.

## Honest Boundary

This tightens the runtime integration contract without pretending the live
replacement carrier exists yet. Rung 20 still requires actual runtime
integration: hot recipes must lower through Form IR/backend bytes into a
replacement install/call carrier and execute with source-attributed exceptions,
deopt, melt, and parity.
