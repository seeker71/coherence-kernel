# Form-native JIT self-host field compiler parity

Date: 2026-07-01

## Commands

```sh
( cat model/form-asm-x64.fk \
      model/jit-self-host-compiler.fk \
      model/tests/jit-self-host-compiler-band.fk ) > /tmp/jsh.fk
./fkwu --src /tmp/jsh.fk

( cat observe/jit-self-host-completion-sweep.fk \
      observe/tests/jit-self-host-completion-sweep-band.fk ) > /tmp/jshcs.fk
./fkwu --src /tmp/jshcs.fk

( cat observe/jit-native-witness-sweep.fk \
      observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk

( cat observe/jit-live-runtime-integration.fk \
      observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk

( cat observe/jit-rung20-dylib-runtime-audit.fk \
      observe/tests/jit-rung20-dylib-runtime-audit-band.fk ) > /tmp/jrdra.fk
./fkwu --src /tmp/jrdra.fk

( cat observe/jit-rung20-readiness.fk \
      observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
```

## Witness

```text
16383
67108863
33554431
16777215
134217727
536870911
```

## Movement

`model/jit-self-host-compiler.fk` now compiles its own checked field-load path
and proves the stage2 compiler bytes match the stage1 field-load bytes. The
self-host compiler witness moves from `8191` to `16383`, covering array, field,
and div self-compiled paths.

`observe/jit-self-host-completion-sweep.fk` now requires
`self-host-compiler = 16383` and rejects the previous `8191` compiler receipt
as stale. Its witness moves from `33554431` to `67108863`.

The direct dependent gates now require the stronger self-host completion
receipt while preserving their established totals. No C, Rust, TypeScript, or
bootstrap carrier code was changed.
