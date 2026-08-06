# Form-native JIT dylib cache lifecycle

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-dylib-cache-lifecycle.fk \
      observe/tests/jit-dylib-cache-lifecycle-band.fk ) > /tmp/jdcl.fk
./fkwu --src /tmp/jdcl.fk
# 8388607

( cat observe/jit-carrier-install-call-evidence.fk \
      observe/tests/jit-carrier-install-call-evidence-band.fk ) > /tmp/jcice.fk
./fkwu --src /tmp/jcice.fk
# 4294967295
```

## Receipt

Added `observe/jit-dylib-cache-lifecycle.fk`, a Form-side cache lifecycle for
generationed dylib carrier slots. The lifecycle binds:

- source signature and expected source signature;
- slot generation and cache generation;
- invalidation epoch;
- stale/cold melt policy;
- guard fallback;
- result status;
- memory-envelope readiness;
- source/maps and no C-bootstrap growth;
- deopt-cache, source-cache, slot-runtime, memory-envelope, and invoke-return
  receipts.

`observe/jit-carrier-install-call-evidence.fk` now requires the dylib cache
lifecycle before carrier/install/call evidence can pass.

## Proved

- matching source/generation/current-epoch/memory-ready slots route native;
- unavailable carriers deopt;
- guard and parity failures deopt through fallback;
- runtime faults throw;
- invalidated or epoch-stale entries rewalk;
- cold stale entries melt;
- mismatched status rejects;
- missing fallback, source, generation, memory, maps, no-C-growth, or stale
  cache receipts reject before the carrier ledger can pass.

## Honest Boundary

This does not execute arbitrary host bytes. It makes the dylib carrier cache
rules explicit as Form data so a later live install/call proof cannot bypass
source signature, generation, invalidation, stale-melt, or memory-envelope
checks.
