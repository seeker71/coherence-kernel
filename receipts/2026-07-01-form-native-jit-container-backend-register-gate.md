# Form-native JIT container backend register gate

Date: 2026-07-01

## Commands

```sh
( cat model/jit-container-lowering.fk \
      model/jit-container-backend.fk \
      model/jit-container-backend-register-gate.fk \
      model/tests/jit-container-backend-register-gate-band.fk ) > /tmp/jcbr.fk
./fkwu --src /tmp/jcbr.fk

( cat model/jit-container-lowering.fk \
      model/jit-container-backend.fk \
      model/tests/jit-container-backend-band.fk ) > /tmp/jcb.fk
./fkwu --src /tmp/jcb.fk
```

## Witness

```text
1023
4095
```

## Movement

`model/jit-container-backend-register-gate.fk` adds a compact Form-native gate
between container backend schedules and later live-code carriers such as the
dylib path. A schedule is treated as register-aware only when the current
`register-lowering` receipt is present at 511, the container backend receipt is
present at 4095, root/fault/parity/source-map metadata is present, and the
schedule shape matches CPU or GPU lane/register expectations.

The focused band rejects stale register receipts, missing root proof, missing
fault proof, missing parity proof, missing source/map proof, a CPU schedule with
GPU-style lanes, and a GPU schedule with scalar lanes. The existing backend
band remains 4095, so the hot backend path stayed stable while this admission
contract moved forward in Form.
