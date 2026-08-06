# Form-native JIT receipt total naming cleanup

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-host-membrane-current-gate.fk \
      observe/tests/jit-host-membrane-current-gate-band.fk ) > /tmp/jhmcg.fk
./fkwu --src /tmp/jhmcg.fk
# 33554431

( cat observe/jit-carrier-current-cache-gate.fk \
      observe/tests/jit-carrier-current-cache-gate-band.fk ) > /tmp/jcccg.fk
./fkwu --src /tmp/jcccg.fk
# 32767

( cat observe/jit-live-execution-evidence.fk \
      observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk
# 536870911

( cat observe/jit-dylib-live-proof-suite.fk \
      observe/tests/jit-dylib-live-proof-suite-band.fk ) > /tmp/jdlps.fk
./fkwu --src /tmp/jdlps.fk
# 536870911

( cat observe/jit-dylib-live-runtime-proof.fk \
      observe/tests/jit-dylib-live-runtime-proof-band.fk ) > /tmp/jdlrp.fk
./fkwu --src /tmp/jdlrp.fk
# 4294967295

( cat observe/jit-live-runtime-integration.fk \
      observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk
# 67108863
```

## Movement

The active live-host and dylib JIT path now names receipt totals before using
them in receipt constructors. This changes direct numeric tuples like
`("live-execution-evidence" 536870911 536870911 ...)` into named helpers such as
`jlee-total-live-evidence`, `jcccg-total-host-current`, and
`jhmcg-total-install-call`.

This is a readability cleanup only. It keeps the witness totals unchanged while
making stale/bad receipt cases legible by name instead of by anonymous numeric
offsets.
