# Receipt - compact Form-native JIT emitter bundle (2026-06-30)

## What landed

Added:

- `observe/jit-emitter-bundle.fk`
- `observe/tests/jit-emitter-bundle-band.fk`

The bundle consumes already witnessed JIT policy results as compact numeric
receipts and emits a `native-block` receipt with guard, fallback, deopt-cache,
runtime-fault-map, source-map, and policy summary. It also models guarded
dispatch outcomes:

- native dispatch when guard and runtime checks pass;
- deopt fallback when the guard fails;
- source-attributed exception when the runtime check fails;
- rewalk when the block is invalidated;
- no-emit when policy, source attribution, or fault maps are incomplete.

## Witness

Run:

```sh
( cat observe/jit-runtime-fault.fk observe/jit-emitter-bundle.fk observe/tests/jit-emitter-bundle-band.fk ) > /tmp/jeb.fk
./fkwu --src /tmp/jeb.fk
```

Observed:

```text
8191
```

Meaning:

- `1`: complete policy receipts admit emission.
- `2`: emitted block carries guard/fallback/deopt/fault/source maps.
- `4`: guard-passing dispatch selects native.
- `8`: native dispatch returns the native value.
- `16`: guard failure selects deopt.
- `32`: deopt returns the walker fallback value.
- `64`: runtime failure selects exception.
- `128`: exception dispatch returns the source-attributed fault kind.
- `256`: incomplete policy blocks emission.
- `512`: incomplete source attribution blocks emission.
- `1024`: missing fault map blocks emission.
- `2048`: unattributed runtime exception deopts instead of being claimed.
- `4096`: invalidated block rewalks.

## Honest boundary

This is a Form-native emitter and dispatch receipt model. It is not executable
machine code and does not claim that runtime dispatch has been wired into the
kernel. Follow-up receipt `2026-06-30-form-native-jit-ir.md` adds the executable
Form IR interpreter beneath it. The remaining required lift is host-native
dispatch generated from that Form-owned IR, preserving these guard/deopt/
exception outcomes and walker parity.
