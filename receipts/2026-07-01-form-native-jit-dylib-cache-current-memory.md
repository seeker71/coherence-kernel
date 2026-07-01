# Form-native JIT dylib cache current memory receipt

Commands:

```sh
( cat observe/jit-dylib-cache-lifecycle.fk \
      observe/tests/jit-dylib-cache-lifecycle-band.fk ) > /tmp/jdcl.fk
./fkwu --src /tmp/jdcl.fk

( cat observe/jit-carrier-install-call-evidence.fk \
      observe/tests/jit-carrier-install-call-evidence-band.fk ) > /tmp/jcice.fk
./fkwu --src /tmp/jcice.fk

( cat observe/jit-live-execution-evidence.fk \
      observe/tests/jit-live-execution-evidence-band.fk ) > /tmp/jlee.fk
./fkwu --src /tmp/jlee.fk
```

Outputs:

```txt
16777215
4294967295
67108863
```

`observe/jit-dylib-cache-lifecycle.fk` now rejects the stale
`dylib-memory-envelope = 1048575` receipt after the memory envelope moved to
the pressure-aware `33554431` witness. The carrier/install/call ledger now
requires `dylib-cache-lifecycle = 16777215` before accepting the dylib cache
receipt.

No C/Rust runtime code changed.
