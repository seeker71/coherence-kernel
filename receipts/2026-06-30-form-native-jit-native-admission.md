# Receipt - Form-owned native admission gate (2026-06-30)

## What landed

Added:

- `observe/jit-native-admission.fk`
- `observe/tests/jit-native-admission-band.fk`

This cell composes the static analyzer, backend byte emitter, and loader
contract into one executable Form admission gate. It refuses native execution
unless source analysis passes, bytes are Form-owned, backend maps are complete,
the loader slot is sealed/executable/non-writable, and guard/runtime/parity
state selects the correct native/deopt/exception/rewalk/reject action.

## Witness

Run:

```sh
( cat observe/jit-runtime-fault.fk \
      observe/form-static-analyzer.fk \
      observe/jit-backend-bytes.fk \
      observe/jit-loader-contract.fk \
      observe/jit-native-admission.fk \
      observe/tests/jit-native-admission-band.fk ) > /tmp/jna.fk
./fkwu --src /tmp/jna.fk
```

Observed:

```text
131071
```

Meaning:

- `1`: static analyzer admits the good source.
- `2`: backend bytes are Form-owned and match the request.
- `4`: the loader payload is safe.
- `8`: the loaded slot is safe.
- `16`: the complete admission chain admits native.
- `32`: guard/runtime/parity passing selects native.
- `64`: native path returns the native value.
- `128`: guard failure selects deopt.
- `256`: deopt returns the walker value.
- `512`: runtime failure selects exception.
- `1024`: exception path returns the attributed runtime kind.
- `2048`: invalidation selects rewalk.
- `4096`: parity failure selects deopt.
- `8192`: static div-zero source is rejected before native admission.
- `16384`: foreign `c-lowering` bytes are rejected.
- `32768`: missing source metadata is rejected.
- `65536`: unsafe generation `0` load is rejected.

## Honest boundary

This is still a Form-owned admission model, not a host function pointer call. It
makes the last contract before the host membrane executable and compositional,
so future runtime work has one Form witness to satisfy before dispatching real
payload bytes through `fk_native_call`/`fk_nat_install`.
