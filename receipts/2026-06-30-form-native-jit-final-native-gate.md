# Form Native JIT Final Native Gate

Date: 2026-06-30

## Witness

```sh
( cat observe/jit-final-native-gate.fk \
      observe/tests/jit-final-native-gate-band.fk ) > /tmp/jfng.fk
./fkwu --src /tmp/jfng.fk
# 1048575
```

## What Landed

`observe/jit-final-native-gate.fk` is a completion audit gate for rung 20. It
composes the full Form-native optimizer ladder through the source-to-replacement
runtime contract and explicitly separates contract readiness from live native
execution.

## Proved

- native-witness, replacement-runtime, source-replacement-runtime,
  host-exception, and bootstrap-exit receipts compose;
- the current track is contract-ready but not complete because live replacement
  carrier execution of Form-emitted bytes is still absent;
- a future live-native evidence row can route native, guard deopt, runtime
  exception, invalidation rewalk, parity deopt, and stale melt;
- live-but-no-native evidence is not completion;
- C-growth, missing lowering, missing runtime, missing source-attributed
  exceptions, missing generation, bad receipt totals, missing source maps, and
  missing metadata maps reject readiness.

## Honest Boundary

This does not mark the goal complete. It prevents premature completion by naming
the final proof still needed: live replacement-carrier execution, witnessed by
the repo, of Form-emitted bytes from a hot source recipe with the same
source-attributed runtime exits and deopt/melt behavior.
