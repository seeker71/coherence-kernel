# Form Native JIT Native Witness Sweep

Date: 2026-06-30

## Witness

```sh
( cat observe/jit-native-witness-sweep.fk \
      observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk
# 4194303
```

## What Landed

`observe/jit-native-witness-sweep.fk` is a compact hot-recipe sweep for the full
native JIT objective. It does not claim host machine-code execution. It proves
that the tracked requirements compose as one admission surface and that native
completion remains pending until a replacement carrier can execute Form-emitted
bytes without growing the C bootstrap.

## Proved

- policy receipts compose: profile/numeric category collapse, tiering,
  inlining, static analysis, and stack/frame collapse;
- access receipts compose: field, list, array, dict, hashmap, ordered tree,
  checked-access payloads, register lowering, and deopt cache;
- emission receipts compose: Form IR, host dispatch packets, backend byte
  streams, loader contract, and native admission;
- carrier receipts compose: byte ingress, host exception bridge, bootstrap-exit
  replacement boundary, and post-ingress state;
- a hot recipe with every optimizer bucket present is ready at the Form receipt
  level;
- the current non-live replacement-carrier state remains pending, not native;
- a future live replacement carrier routes native, guard deopt, runtime
  exception, invalidation rewalk, parity deopt, and stale-cache melt;
- C-bootstrap growth rejects;
- missing category collapse, dict access, GPU lane, runtime checks, positive
  generation, source maps, or metadata maps rejects readiness.

## Honest Boundary

This is still not rung 20 completion. The final missing proof is runtime
integration: hot source recipes must lower through the Form-owned IR/backend
byte streams into a replacement host install/call carrier and execute with
source-attributed runtime exceptions, guarded deopt, melt, and parity.
