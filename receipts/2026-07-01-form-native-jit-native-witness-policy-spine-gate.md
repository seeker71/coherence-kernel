# Form-native JIT native witness policy spine gate

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-native-witness-sweep.fk \
      observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk
# 33554431
```

## Receipt

Strengthened `observe/jit-native-witness-sweep.fk` so the hot-recipe native
witness requires the grouped policy spine receipts:

- `policy-front-sweep` = 31
- `policy-access-sweep` = 7
- `policy-cache-sweep` = 15

Those grouped sweeps prove the profile/category, tiering, inlining, static
analysis, stack/frame, representation, register, runtime-fault, and deopt/cache
policy families coexist under the current source-runner envelope. The native
witness already required the leaf receipts; this gate makes the composed policy
spine itself part of the higher native-readiness chain.

A later self-host completion hardening raised this same witness to `33554431`
by rejecting the stale `self-host-completion-sweep = 16777215` receipt.

Updated direct consumers of the native witness total:

- `observe/jit-replacement-runtime-integration.fk`
- `observe/jit-source-replacement-runtime.fk`
- `observe/jit-final-native-gate.fk`
- `observe/jit-live-runtime-integration.fk`
- `observe/jit-rung20-readiness.fk`
- `observe/jit-rung20-dylib-runtime-audit.fk`

No C or Rust runtime work was added. This keeps arbitrary host byte execution
pending while making the native witness depend on the full grouped Form policy
spine, not only isolated policy leaves.
