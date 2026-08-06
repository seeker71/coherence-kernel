# Form-native JIT native admission carries exception receipts

Date: 2026-07-02

## Witness

```sh
( cat observe/jit-runtime-fault.fk \
      observe/form-static-analyzer.fk \
      observe/jit-backend-bytes.fk \
      observe/jit-loader-contract.fk \
      observe/jit-native-admission.fk \
      observe/tests/jit-native-admission-band.fk ) > /tmp/jna.fk
./fkwu --src /tmp/jna.fk
# 131071

( cat observe/jit-runtime-fault.fk \
      observe/jit-runtime-stack-attribution.fk \
      observe/jit-host-ingress-readiness.fk \
      observe/jit-host-exception-bridge.fk \
      observe/tests/jit-host-exception-bridge-band.fk ) > /tmp/jheb.fk
./fkwu --src /tmp/jheb.fk
# 2097151

( cat observe/jit-native-witness-sweep.fk \
      observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk
# 67108863
```

## Movement

`observe/jit-native-admission.fk` no longer collapses a runtime failure to a
bare exception kind. A runtime-failed admitted native intent now returns the
source-attributed `jit-runtime-fault` exception receipt through `jna-value`, and
the admission witness checks that the returned exception is attributed and has
the expected `div-zero` kind.

The public native-admission total stays `131071`; the existing runtime-failure
bit is stronger. Full-stack attribution remains guarded by
`jit-host-exception-bridge`, which still witnesses `2097151`.
