# Form-native JIT rung 20 dylib runtime audit

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-rung20-dylib-runtime-audit.fk \
      observe/tests/jit-rung20-dylib-runtime-audit-band.fk ) > /tmp/jrdra.fk
./fkwu --src /tmp/jrdra.fk
# 33554431
```

## Receipt

Added `observe/jit-rung20-dylib-runtime-audit.fk` and its band test. The cell
carries dylib live-runtime proof into the final rung-20 proof shape: source,
lowering, runtime, safety, self-host, live, no-C-growth, carrier, install, call,
exception, deopt, melt, parity, and positive generation.

The audit is intentionally not a live-host-execution claim. It proves that final
readiness now directly requires native witness, self-host completion,
live-runtime integration, dylib live-runtime proof, carrier/install/call
evidence, and source runtime receipts. Current missing carrier/install/call
facts remain pending. Complete audit routes native, guard-deopt,
runtime-exception, invalidation-rewalk, stale-melt, and parity-deopt. Missing
source/lowering/runtime/safety/self-host/live facts, C-growth,
carrier/install/call, generation, and stale live-runtime/dylib/carrier receipts
reject before final readiness.

No C or Rust runtime work was added. The audit composes into
`observe/jit-rung20-readiness.fk` by receipt.
