# Receipt - Form-native JIT runtime stack attribution (2026-06-30)

## What landed

Added:

- `observe/jit-runtime-stack-attribution.fk`
- `observe/tests/jit-runtime-stack-attribution-band.fk`

This tightens the runtime-fault contract for native JIT paths. A native runtime
fault is only complete when every stack frame carries full source attribution,
the exception source matches the throwing frame, and native/walker parity
compares the full stack frame-by-frame.

## Witness

Run:

```sh
( cat observe/jit-runtime-fault.fk \
      observe/jit-runtime-stack-attribution.fk \
      observe/tests/jit-runtime-stack-attribution-band.fk ) > /tmp/jrsa.fk
./fkwu --src /tmp/jrsa.fk
```

Observed:

```text
1023
```

Meaning:

- `1`: a bounds exception with a three-frame attributed stack is complete.
- `2`: every frame in the good stack carries source attribution.
- `4`: an empty stack is rejected.
- `8`: a stack with a missing caller source is rejected.
- `16`: an exception whose source differs from the throwing frame is rejected.
- `32`: native/walker full-stack parity passes for identical exceptions.
- `64`: parity rejects changed caller source attribution.
- `128`: parity rejects changed call-site metadata.
- `256`: parity rejects changed operation kind.
- `512`: div-by-zero exceptions use the same full-stack attribution contract.

## Honest boundary

This is still a Form receipt model, not kernel-level throwing from installed
machine code. It removes an ambiguity in the previous runtime-fault receipt:
native completion must preserve full stack/source attribution, not only the top
frame and stack depth.
