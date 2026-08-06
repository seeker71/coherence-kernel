# Form-native JIT dylib call dispatch evidence

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-dylib-call-dispatch-evidence.fk \
      observe/tests/jit-dylib-call-dispatch-evidence-band.fk ) > /tmp/jdcde.fk
./fkwu --src /tmp/jdcde.fk
# 134217727
```

## Receipt

Added `observe/jit-dylib-call-dispatch-evidence.fk` and its band test. The
cell bridges the prior dylib load/bind ticket into callable dispatch evidence:
ticket readiness, installed/callable state, rooted args, positive arg count,
runtime stack attribution, source/maps, no-C-growth, parity, exception/deopt/
melt routes, and positive generation.

This keeps the `.dylib` path honest. Load/bind proves an open handle and bound
symbols; this dispatch bridge proves the next contract before a native call may
be claimed. Current dispatch remains pending until installed and callable facts
are present. Complete dispatch routes native, guard-deopt, runtime-exception,
invalidation-rewalk, stale-melt, and parity-deopt. Unavailable dispatch deopts;
mismatched dispatch rejects; faulted dispatch routes exception; missing tickets,
args, stack, source/maps, parity, no-C-growth, generation, install/callable
facts, and stale load/dispatch receipts reject before completion.

No C or Rust runtime work was added. The carrier remains `.dylib` shaped, and
the call-dispatch evidence is Form-owned.
