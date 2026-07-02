# Form-native JIT live evidence dylib lifecycle

Date: 2026-07-02

## Movement

This pass tightens `observe/jit-live-execution-evidence.fk` so live execution
evidence cannot rely only on aggregate dylib proof-suite receipts:

- Live evidence now directly requires `dylib-memory-envelope = 33554431`.
- Live evidence now directly requires `dylib-load-bind-runtime = 268435455`.
- Live evidence now directly requires `dylib-invoke-return-evidence =
  268435455`.
- Bad and stale lifecycle receipts are folded into the existing saturated
  rejection lane without increasing the witness total.

This keeps live execution evidence aligned with the concrete Form-owned `.dylib`
carrier lifecycle: bounded executable memory, opened/bound carrier symbols, and
invoke/return facts all have to be current before the proof can pass.

## Witness

```sh
( cat observe/jit-live-execution-evidence.fk \
      observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk
# 536870911
```

## Boundary

- No C/Rust changes.
- No arbitrary host byte execution claim.
- This is a Form-only contract hardening of the live `.dylib` evidence path.
