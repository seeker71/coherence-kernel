# Form-native JIT replacement runtime dylib lifecycle

Date: 2026-07-02

## Movement

This pass tightens `observe/jit-replacement-runtime-integration.fk` so the
replacement runtime cannot feed final/source/live gates from a generic loader
receipt alone:

- The receipt set now requires the current `dylib-carrier-abi = 4194303`
  contract.
- The receipt set now requires the current
  `dylib-load-bind-runtime = 268435455` lifecycle.
- Bad carrier ABI, bad load/bind, and stale load/bind receipts are folded into
  the existing rejection lane without increasing the witness total.

This keeps the route aligned with the Form-owned `.dylib` carrier path while
still avoiding host machine-code execution claims.

## Witness

```sh
( cat observe/jit-replacement-runtime-integration.fk \
      observe/tests/jit-replacement-runtime-integration-band.fk ) > /tmp/jrri.fk
./fkwu --src /tmp/jrri.fk
# 268435455
```

## Boundary

- No C/Rust changes.
- No arbitrary host byte execution claim.
- Replacement runtime remains Form-only, but now requires the concrete `.dylib`
  ABI and load/bind receipts before it can be treated as current.
