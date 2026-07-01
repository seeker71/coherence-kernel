# Receipt - Form Native JIT dylib carrier ABI

Date: 2026-07-01

## Witness

```sh
( cat model/form-asm-x64.fk \
      observe/jit-dylib-carrier-abi.fk \
      observe/tests/jit-dylib-carrier-abi-band.fk ) > /tmp/jdca.fk
./fkwu --src /tmp/jdca.fk
# 2097151
```

## What Landed

Added `observe/jit-dylib-carrier-abi.fk` and its band test. The cell names the
future host door as `libform_jit_carrier.dylib` with these exact symbols:

- `fk_jit_install`
- `fk_jit_call`
- `fk_jit_drop`
- `fk_jit_flush_icache`
- `fk_jit_status_v1`

The dylib is only the OS memory and jump carrier. Form remains responsible for
the emitted bytes, tagged argument vectors, source/fault/deopt maps, routing,
generation, and no-C-growth proof.

## Proved

- checked field-load and array-get byte images are valid install payloads;
- byte counts, byte-list integrity, W^X sealed executable pages, non-writable
  execute slots, and source/fault/deopt maps are required;
- call slots need tagged args, exact arg counts, callable state, and source
  stack attribution;
- C-lowering ownership and wrong symbol names reject the carrier;
- unavailable carriers deopt, fault statuses route to exception, mismatches
  reject, and native/deopt/rewalk/melt routing stays explicit.

## Honest Boundary

This does not install or call arbitrary host machine code yet. It gives the
missing live boundary a concrete ABI that can be backed by a dylib without
putting JIT compiler meaning in C/Rust.
