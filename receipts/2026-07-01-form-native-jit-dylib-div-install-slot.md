# Form-native JIT dylib div install slot

Date: 2026-07-01

## Commands

```sh
( cat model/form-asm-x64.fk \
      observe/jit-dylib-carrier-abi.fk \
      observe/tests/jit-dylib-carrier-abi-band.fk ) > /tmp/jdca.fk
./fkwu --src /tmp/jdca.fk

( cat model/form-asm-x64.fk \
      observe/jit-install-call-attempt.fk \
      observe/tests/jit-install-call-attempt-band.fk ) > /tmp/jica.fk
./fkwu --src /tmp/jica.fk

( cat model/form-asm-x64.fk \
      observe/jit-dylib-carrier-abi.fk \
      observe/jit-dylib-image-manifest.fk \
      observe/tests/jit-dylib-image-manifest-band.fk ) > /tmp/jdimg.fk
./fkwu --src /tmp/jdimg.fk

( cat model/form-asm-x64.fk \
      observe/jit-install-call-attempt.fk \
      observe/jit-dylib-carrier-abi.fk \
      observe/jit-dylib-slot-runtime.fk \
      observe/tests/jit-dylib-slot-runtime-band.fk ) > /tmp/jdsl.fk
./fkwu --src /tmp/jdsl.fk

( cat observe/jit-carrier-install-call-evidence.fk \
      observe/tests/jit-carrier-install-call-evidence-band.fk ) > /tmp/jcice.fk
./fkwu --src /tmp/jcice.fk
```

## Witness

```text
4194303
67108863
33554431
524287
4294967295
```

## Movement

`observe/jit-dylib-carrier-abi.fk` now treats the checked div args-vector byte
image as a valid Form-owned install payload for the `.dylib` carrier contract.
The carrier ABI witness moves from `2097151` to `4194303`.

`observe/jit-install-call-attempt.fk` now admits two-argument checked div
attempt packets and proves the div payload is contract-ready beside add1,
field, and array. The install/call attempt witness moves from `33554431` to
`67108863`.

`observe/jit-dylib-image-manifest.fk` and
`observe/jit-dylib-slot-runtime.fk` now carry checked div through image
manifest install safety, slot signatures, cache lookup, and native route
selection. Their witnesses move to `33554431` and `524287`.

Direct receipt consumers now require the strengthened carrier, attempt, image,
and slot totals. No C, Rust, TypeScript, or bootstrap carrier code was changed.
