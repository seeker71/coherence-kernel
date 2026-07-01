# Receipt - Form Native JIT dylib slot runtime

Date: 2026-07-01

## Witness

```sh
( cat model/form-asm-x64.fk \
      observe/jit-install-call-attempt.fk \
      observe/jit-dylib-carrier-abi.fk \
      observe/jit-dylib-slot-runtime.fk \
      observe/tests/jit-dylib-slot-runtime-band.fk ) > /tmp/jdsl.fk
./fkwu --src /tmp/jdsl.fk
# 262143
```

## What Landed

Added `observe/jit-dylib-slot-runtime.fk` and its band test. The cell composes
the concrete install/call attempt packet with the dylib carrier ABI:

- converts Form-owned install/call attempts into carrier install records;
- binds slots to generationed signatures;
- checks cache hits before any native route;
- validates callable/source/mapped slot state;
- routes carrier status through native, deopt, exception, rewalk, and melt.

## Proved

- checked field and array attempts become safe dylib slots;
- signature misses, invalid slots, non-callable slots, and C-lowering carriers
  do not route native;
- successful status routes native;
- guard failure deopts, runtime fault throws, invalidation rewalks, stale slots
  melt, parity failure deopts, unavailable carriers deopt, carrier faults throw,
  and carrier mismatches reject;
- install safety still depends on exact Form-emitted bytes, W^X, source maps,
  fault maps, deopt maps, tagged args, and positive generation.

## Honest Boundary

This is still not host machine-code execution. It is the Form-owned slot table
and call-routing layer that a future `libform_jit_carrier.dylib` must back with
actual executable memory and function-pointer calls.
