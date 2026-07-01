# Form-native JIT Form-emitted carrier probe

Date: 2026-07-01

## Witness

```sh
( cat model/form-asm-x64.fk \
      observe/jit-form-emitted-carrier-probe.fk \
      observe/tests/jit-form-emitted-carrier-probe-band.fk ) > /tmp/jfecp.fk
./fkwu --src /tmp/jfecp.fk
# 1048575
```

## Receipt

Added `observe/jit-form-emitted-carrier-probe.fk` and its band test. The cell
binds the existing `native_call_test` host carrier to the exact direct-call
add1 byte image emitted by `model/form-asm-x64.fk`.

It proves the SysV and Win64 one-argument add1 payload bytes are produced by
Form and byte-identical to the carrier's probe images. The live carrier status
is accepted only when it either executes correctly or returns honest
unavailability on a non-matching architecture. Mismatched execution rejects.

This does not expose arbitrary byte ingress. It tightens the live carrier probe
so the observed host install/call door is tied to Form-emitted bytes instead of
standing as an unrelated black-box status check.
