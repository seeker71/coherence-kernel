# Form Native JIT Live Execution Evidence

Date: 2026-06-30

## Witness

```sh
( cat observe/jit-live-execution-evidence.fk \
      observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk
# 8388607
```

## What Landed

`observe/jit-live-execution-evidence.fk` splits the final native gate's live
execution flag into a proof suite. A final claim now needs separate evidence
for replacement carrier availability, byte install, native call result,
source-attributed exception result, guarded deopt, stale melt, parity,
source/no-C-growth, and positive generation.

## Proved

- final-native-gate, source-replacement-runtime, and replacement-runtime
  receipts compose;
- current evidence is contract-ready but still pending because live carrier,
  install, call, exception, deopt, and melt proofs are absent;
- a complete live proof suite routes native, guard deopt, runtime exception,
  invalidation rewalk, parity deopt, and stale melt;
- missing carrier, install, call, exception, deopt, melt, or parity proof is not
  live completion;
- C growth, zero generation, bad final-gate receipts, missing source maps, and
  missing metadata maps reject readiness.

## Honest Boundary

This still does not complete rung 20. It narrows the remaining work to the
specific live proof suite that a replacement carrier must produce.
