# Receipt - Form-native JIT IR lowering and interpreter (2026-06-30)

## What landed

Added:

- `observe/jit-native-ir.fk`
- `observe/tests/jit-native-ir-band.fk`

This is the executable Form-owned lowering layer below the compact emitter
bundle. It emits instruction rows:

```text
("ir" op arg source)
```

and program rows:

```text
("program" block ops source)
```

The IR interpreter preserves the same outcomes named by the emitter bundle:
native path, deopt fallback, source-attributed exception, and invalidation
rewalk.

## Witness

Run:

```sh
( cat observe/jit-runtime-fault.fk observe/jit-emitter-bundle.fk observe/jit-native-ir.fk observe/tests/jit-native-ir-band.fk ) > /tmp/jnir.fk
./fkwu --src /tmp/jnir.fk
```

Observed:

```text
16383
```

Meaning:

- `1`: lowered array program is structurally safe.
- `2`: array lowering includes a bounds-check instruction.
- `4`: field lowering includes a null-check instruction.
- `8`: div lowering includes a div-zero-check instruction.
- `16`: guard/runtime passing execution selects native.
- `32`: native execution returns the native value.
- `64`: guard failure selects deopt.
- `128`: deopt returns the walker fallback value.
- `256`: runtime failure selects exception.
- `512`: exception execution returns the attributed fault kind.
- `1024`: invalidated block rewalks.
- `2048`: bad source attribution prevents a safe program.
- `4096`: unattributed runtime exception deopts instead of being claimed.
- `8192`: native/walker parity is witnessed for the passing sample.

## Honest boundary

This is executable Form IR, not host machine code. It closes the gap between
policy receipts and an executable Form-owned lowering target, but it does not
wire host native dispatch into `fkwu`. Follow-up receipt
`2026-06-30-form-native-jit-host-dispatch.md` adds the Form-owned dispatch
packet/cache contract above this IR. The remaining lift is to execute real
host-native code generated from these Form IR packets while preserving the same
guard, fallback, deopt, melt, source-attributed exception, and walker-parity
behavior.
