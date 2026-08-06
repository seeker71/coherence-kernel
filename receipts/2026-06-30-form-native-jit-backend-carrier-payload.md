# Receipt - Form backend emits carrier-compatible payloads (2026-06-30)

## What landed

Added:

- `observe/jit-backend-carrier-payload.fk`
- `observe/tests/jit-backend-carrier-payload-band.fk`

This cell composes `model/form-asm-x64.fk`, `observe/jit-backend-bytes.fk`,
`observe/jit-carrier-abi.fk`, and `observe/jit-loader-contract.fk`.

It preserves the existing generic backend-byte witness and adds a stricter
carrier-compatible path: Win64 and SysV `f(a)=a+1` payloads are generated through
the Form x64 encoder, wrapped as backend byte outputs, admitted as loader
payloads, and routed through the native/deopt/exception/rewalk action contract.

## Witness

Run:

```sh
( cat model/form-asm-x64.fk \
      observe/jit-backend-bytes.fk \
      observe/jit-loader-contract.fk \
      observe/jit-carrier-abi.fk \
      observe/jit-backend-carrier-payload.fk \
      observe/tests/jit-backend-carrier-payload-band.fk ) > /tmp/jbcp.fk
./fkwu --src /tmp/jbcp.fk
```

Observed:

```text
32767
```

Meaning:

- `1`: Win64 carrier-compatible backend output is safe.
- `2`: SysV carrier-compatible backend output is safe.
- `4`: Win64 bytes match the Form-emitted carrier ABI image.
- `8`: SysV bytes match the Form-emitted carrier ABI image.
- `16`: loader payload is safe.
- `32`: loaded slot is safe.
- `64`: guard/runtime/parity passing selects native.
- `128`: guard failure selects deopt.
- `256`: runtime failure selects exception.
- `512`: invalidation selects rewalk.
- `1024`: parity failure selects deopt.
- `2048`: foreign `c-lowering` request is rejected.
- `4096`: missing source metadata is rejected.
- `8192`: missing deopt metadata is rejected.
- `16384`: generation-zero load is unsafe.

## Honest boundary

This still does not pass arbitrary byte lists into `fk_native_call` at runtime.
It closes the stronger Form-side connection between the backend payload layer and
the carrier ABI bytes already witnessed by `observe/jit-carrier-abi.fk`.
