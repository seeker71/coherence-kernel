# Form-native JIT dylib memory envelope

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-dylib-memory-envelope.fk \
      observe/tests/jit-dylib-memory-envelope-band.fk ) > /tmp/jdme.fk
./fkwu --src /tmp/jdme.fk
# 1048575

( cat observe/jit-carrier-install-call-evidence.fk \
      observe/tests/jit-carrier-install-call-evidence-band.fk ) > /tmp/jcice.fk
./fkwu --src /tmp/jcice.fk
# 4294967295
```

## Receipt

Added `observe/jit-dylib-memory-envelope.fk`, a Form-side memory restriction
contract for the dylib JIT carrier. The envelope requires:

- page quota and reserved byte bounds;
- final executable/non-writable W^X state;
- sealed slot state;
- guard pages on both sides of the slot;
- rooted argument lifetime;
- source, exception, and deopt maps;
- no C-bootstrap growth;
- positive generation.

`observe/jit-carrier-install-call-evidence.fk` now requires the memory-envelope
receipt before carrier/install/call evidence can pass. A later cache-lifecycle
gate raised this carrier total again; the command above records the current
carrier ledger total.

## Proved

- current memory-safe but not installed/callable state remains pending;
- complete memory state routes native, guard-deopt, runtime-exception,
  invalidation-rewalk, parity-deopt, and stale-melt;
- over-budget, RWX, unsealed, unguarded, unrooted, missing source/maps, missing
  exception/deopt maps, or stale W^X receipts reject before the carrier ledger
  can pass.

## Honest Boundary

This is not arbitrary host byte execution. It is the Form-native memory floor
that the dylib carrier must satisfy before a later live install/call proof can
claim native execution.
