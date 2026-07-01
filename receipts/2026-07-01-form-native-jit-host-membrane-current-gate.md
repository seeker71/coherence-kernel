# Form-native JIT host membrane current gate

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
```

## Movement

`observe/jit-host-membrane-current-gate.fk` adds an acyclic companion receipt
for the host membrane path. It requires `host-membrane-readiness = 524287`,
`host-ingress-readiness = 1048575`, `host-wx-audit = 1048575`,
`install-call-attempt = 67108863`, byte-ingress, and args-vector receipts before
carrier freshness may count the host membrane as current.

`observe/jit-carrier-current-cache-gate.fk` now consumes
`host-membrane-current-gate = 33554431` and returns `32767`.
`observe/jit-live-execution-evidence.fk` now consumes the updated carrier
freshness gate and returns `536870911`.

This keeps install/call live completion honest: current install and call facts
remain pending, but the live evidence path can no longer bypass ingress, W^X,
or install-call attempt freshness. No C or Rust seed work was added.
