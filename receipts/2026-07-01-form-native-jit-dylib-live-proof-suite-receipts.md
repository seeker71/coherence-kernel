# Form-native JIT dylib live proof-suite receipt tightening

Date: 2026-07-01

This receipt tightens the Form-only `.dylib` live proof suite so every upstream
receipt it consumes has a stale-receipt rejection in the witness surface.

Commands:

```sh
( cat observe/jit-dylib-live-proof-suite.fk observe/tests/jit-dylib-live-proof-suite-band.fk ) > /tmp/jdlps.fk
./fkwu --src /tmp/jdlps.fk
# 134217727

( cat observe/jit-live-execution-evidence.fk observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk
# 33554431

( cat observe/jit-dylib-live-runtime-proof.fk observe/tests/jit-dylib-live-runtime-proof-band.fk ) > /tmp/jdlrp.fk
./fkwu --src /tmp/jdlrp.fk
# 1073741823
```

The live proof suite already required carrier evidence, invoke/return evidence,
call-result evidence, host-exception bridging, and runtime stack attribution.
This change adds explicit stale call-result and runtime-stack rejection bits,
raising `dylib-live-proof-suite` from `33554431` to `134217727`.

`observe/jit-live-execution-evidence.fk` now requires that stronger
`dylib-live-proof-suite = 134217727` receipt and rejects the stale old
`33554431` suite total, raising `live-execution-evidence` from `16777215` to
`33554431`.
