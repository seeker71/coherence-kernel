# Receipt - rung 20 audit consumes the dylib lifecycle receipts (2026-07-02)

## Movement

`observe/jit-rung20-dylib-runtime-audit.fk` now directly requires the same
concrete `.dylib` lifecycle receipts that feed live runtime integration:

- `dylib-memory-envelope = 33554431`
- `dylib-load-bind-runtime = 268435455`
- `dylib-invoke-return-evidence = 268435455`

It already required `source-dylib-container-executor = 4194303`. The audit now
composes the source-container bridge with the memory/load/invoke lifecycle at
the rung-20 boundary as well.

## Why

`live-runtime-integration` was hardened first, but the final rung-20 dylib audit
still accepted the summarized live-runtime receipt plus the saturated dylib
proof. That left the last audit edge less explicit than the live-runtime bridge
itself.

This round carries the lifecycle receipts into the rung-20 audit and strengthens
the existing bad/stale lifecycle rejection bit without changing the public
witness total.

## Witness

```sh
( cat observe/jit-rung20-dylib-runtime-audit.fk \
      observe/tests/jit-rung20-dylib-runtime-audit-band.fk ) > /tmp/jrdra.fk
./fkwu --src /tmp/jrdra.fk
# -> 268435455
```
