# Form-native JIT dylib live-runtime stale gate

Date: 2026-07-01

This receipt carries the stronger `.dylib` live proof-suite and live-execution
evidence totals into the live-runtime proof, then propagates those stronger
totals through live-runtime integration and rung-20 readiness.

Commands:

```sh
( cat observe/jit-dylib-live-runtime-proof.fk observe/tests/jit-dylib-live-runtime-proof-band.fk ) > /tmp/jdlrp.fk
./fkwu --src /tmp/jdlrp.fk
# 4294967295

( cat observe/jit-live-runtime-integration.fk observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk
# 8388607

( cat observe/jit-rung20-dylib-runtime-audit.fk observe/tests/jit-rung20-dylib-runtime-audit-band.fk ) > /tmp/jrdra.fk
./fkwu --src /tmp/jrdra.fk
# 134217727

( cat observe/jit-rung20-readiness.fk observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
# 268435455
```

`observe/jit-dylib-live-runtime-proof.fk` now rejects stale
`dylib-live-proof-suite = 33554431` and stale `live-execution-evidence =
16777215` receipts while requiring the current stronger totals. Downstream
live-runtime integration and rung-20 readiness now reject their own stale
runtime/audit totals before the chain can pass.
