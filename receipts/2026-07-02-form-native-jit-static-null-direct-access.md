# Form-native JIT static null direct-access gate

Date: 2026-07-02

## Witness

```sh
( cat observe/jit-runtime-fault.fk \
      observe/form-static-analyzer.fk \
      observe/tests/form-static-analyzer-direct-null-band.fk ) > /tmp/fsadn.fk
./fkwu --src /tmp/fsadn.fk
# 31

( cat observe/jit-runtime-fault.fk \
      observe/form-static-analyzer.fk \
      observe/tests/form-static-analyzer-band.fk ) > /tmp/fsa.fk
./fkwu --src /tmp/fsa.fk
# 1023

( cat model/form-asm-x64.fk \
      observe/jit-runtime-fault.fk \
      observe/form-static-analyzer.fk \
      observe/jit-source-byte-pipeline.fk \
      observe/tests/jit-source-byte-pipeline-band.fk ) > /tmp/jsbp.fk
./fkwu --src /tmp/jsbp.fk
# 2097151
```

## Movement

The static analyzer now rejects known-`nothing` receivers before direct
array/field/dict/hash/tree access can execute or lower into JIT bytes. The
source-byte pipeline also proves that a statically known-null field receiver is
rejected at the pre-lowering static gate.

The public witness totals stay stable because the new checks strengthen the
existing null/static-rejection bits rather than adding a new downstream receipt
total. Runtime checks remain required for null facts that are not statically
known.
