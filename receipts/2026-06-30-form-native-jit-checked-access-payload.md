# Receipt - Form x64 checked-access payloads (2026-06-30)

## What landed

Added:

- `observe/jit-checked-access-payload.fk`
- `observe/tests/jit-checked-access-payload-band.fk`

Updated:

- `model/form-asm-x64.fk`
- `docs/form-native-jit-track.form`

This moves direct access from policy-only receipts into concrete Form-emitted
x64 byte fragments. The encoder now names checked payloads for:

- indexed array/list-slot load with null and bounds fault exits
- field-slot load with a null fault exit
- signed division with a div-by-zero fault exit

The observe cell admits those byte payloads through the existing loader contract
only when source, guard, fault, deopt, root, ABI, safepoint, and exception maps
are present. Fault exits are tied back to the source-attributed exception
receipts from `observe/jit-runtime-fault.fk`.

## Witness

Run:

```sh
( cat model/form-asm-x64.fk \
      observe/jit-runtime-fault.fk \
      observe/jit-loader-contract.fk \
      observe/jit-checked-access-payload.fk \
      observe/tests/jit-checked-access-payload-band.fk ) > /tmp/jcap.fk
./fkwu --src /tmp/jcap.fk
```

Observed:

```text
2097151
```

Meaning:

- `1`: array checked-access payload is loader-safe.
- `2`: field checked-access payload is loader-safe.
- `4`: div checked-access payload is loader-safe.
- `8`: array bytes are emitted by `model/form-asm-x64.fk`.
- `16`: field bytes are emitted by `model/form-asm-x64.fk`.
- `32`: div bytes are emitted by `model/form-asm-x64.fk`.
- `64`: array payload contains the bounds fault branch.
- `128`: field payload contains the null fault branch.
- `256`: div payload contains the signed division instruction group.
- `512`: array bounds fault is source-attributed.
- `1024`: field null fault is source-attributed.
- `2048`: div-zero fault is source-attributed.
- `4096`: passing guard/runtime/parity selects native.
- `8192`: guard failure selects deopt.
- `16384`: runtime failure selects exception.
- `32768`: invalidation selects rewalk.
- `65536`: parity failure selects deopt.
- `131072`: foreign `c-lowering` request is rejected.
- `262144`: missing source metadata is rejected.
- `524288`: missing fault metadata is rejected.
- `1048576`: missing exception metadata is rejected.

## Honest boundary

This still does not install or execute these byte lists through `fk_native_call`.
It closes another Form-side gap: direct native array/list, field, and division
payloads now carry checked fault exits plus source-attributed exception maps
before they can reach the loader contract.
