# Form-native JIT slot runtime fault bridge

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-runtime-fault.fk \
      observe/jit-runtime-stack-attribution.fk \
      observe/jit-slot-runtime-fault-bridge.fk \
      observe/tests/jit-slot-runtime-fault-bridge-band.fk ) > /tmp/jsrf.fk
./fkwu --src /tmp/jsrf.fk
# 1048575
```

## Receipt

Added `observe/jit-slot-runtime-fault-bridge.fk` and its band test. The cell
ties dylib, profile-derived dylib, and container live slots to the existing
runtime-fault, full-stack-attribution, host-exception, slot-runtime, and
container-runtime receipts.

The bridge keeps the `.dylib` path honest: a native slot may route as native
only when its source, stack, exception, deopt, melt, parity, generation, and
upstream slot receipts are present. Guard failure routes deopt, runtime failure
and faulted slots route source-attributed exception, invalidated slots rewalk,
stale slots melt, unavailable carriers deopt, and malformed or mismatched slot
packets reject.

No C or Rust runtime work was added. The `.dylib` remains a carrier/output path,
not a hand-written runtime home.
