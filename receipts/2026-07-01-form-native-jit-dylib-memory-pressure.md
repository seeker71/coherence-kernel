# Form-native JIT dylib memory pressure

Commands:

```sh
( cat observe/jit-dylib-memory-envelope.fk \
      observe/tests/jit-dylib-memory-envelope-band.fk ) > /tmp/jdme.fk
./fkwu --src /tmp/jdme.fk

( cat observe/jit-dylib-cache-lifecycle.fk \
      observe/tests/jit-dylib-cache-lifecycle-band.fk ) > /tmp/jdcl.fk
./fkwu --src /tmp/jdcl.fk

( cat observe/jit-carrier-install-call-evidence.fk \
      observe/tests/jit-carrier-install-call-evidence-band.fk ) > /tmp/jcice.fk
./fkwu --src /tmp/jcice.fk
```

Outputs:

```txt
33554431
8388607
4294967295
```

`observe/jit-dylib-memory-envelope.fk` now witnesses the normal JIT memory
pressure behavior around the dylib carrier: bounded page quotas, live/dirty/free
page accounting, protected live slots, high-water melt through an evictable
fallback path, and rejection of over-budget, non-evictable, or no-fallback
pressure states.

Direct consumers now require `dylib-memory-envelope = 33554431`:

- `observe/jit-dylib-cache-lifecycle.fk`
- `observe/jit-carrier-install-call-evidence.fk`

No C/Rust runtime code changed.
