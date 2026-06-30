# Receipt - Form-native JIT runtime faults need source-attributed exceptions (2026-06-30)

## What landed

The JIT track now includes runtime safety as a first-class rung. Optimized
native code must not skip semantic checks just because it has a direct field,
array, dict, hash, tree, CPU, or GPU path.

New executable Form files:

- `observe/jit-runtime-fault.fk`
- `observe/tests/jit-runtime-fault-band.fk`

The exception receipt shape is:

```text
("exception" kind op source stack detail)
```

The source shape is:

```text
("source" language path line column span)
```

The stack frame shape is:

```text
("frame" fn site source)
```

## Witness

Run:

```sh
( cat observe/jit-runtime-fault.fk observe/tests/jit-runtime-fault-band.fk ) > /tmp/jrf.fk
./fkwu --src /tmp/jrf.fk
```

Observed:

```text
255
```

Meaning:

- `1`: bounds fault carries source + stack attribution.
- `2`: null-ref fault carries source + stack attribution.
- `4`: div-by-zero fault carries source + stack attribution.
- `8`: type/shape fault carries source + stack attribution.
- `16`: out-of-range index is rejected before direct access.
- `32`: nothing/null dereference is rejected before direct access.
- `64`: zero denominator is rejected before division/mod lowering.
- `128`: native and walker exception receipts can be parity-checked.

## Honest scope

This does not yet implement runtime throwing in the kernel or the Form emitter.
It is the executable Form contract the emitter must satisfy before direct access
specializations can count as complete. The current `core.fk` assertion floor
still uses `head(empty)` as a panic. The destination is stricter: bounds,
null-ref, div-by-zero, type/shape, overflow, and guard faults throw structured
exception receipts with stack trace and full source attribution, and native must
match walker attribution.
