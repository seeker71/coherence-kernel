# Receipt - Form exact byte-list membrane (2026-06-30)

## What landed

Added:

- `observe/jit-byte-list-membrane.fk`
- `observe/tests/jit-byte-list-membrane-band.fk`

This closes a Form-side gap between exact byte emitters and the loader contract.
Earlier loader receipts carried byte counts after emission. This cell keeps the
actual Form-emitted byte list inside the admitted/loadable payload, validates
each byte, preserves the exact list through load, and only then adapts the
payload into `observe/jit-loader-contract.fk` for native/deopt/exception/rewalk
action routing.

The membrane admits:

- Form-emitted Win64 and SysV `arg+1` carrier byte lists
- Form-emitted checked array/list-slot, field-slot, and div payload byte lists
- an arbitrary valid Form byte list with full metadata

It rejects foreign owners, empty byte lists, out-of-range bytes, negative bytes,
missing source metadata, missing deopt metadata, missing exception metadata,
unsafe generation-zero loads, and writable/unsealed loaded slots.

## Witness

Run:

```sh
( cat model/form-asm-x64.fk \
      observe/jit-runtime-fault.fk \
      observe/jit-loader-contract.fk \
      observe/jit-byte-list-membrane.fk \
      observe/tests/jit-byte-list-membrane-band.fk ) > /tmp/jblm.fk
./fkwu --src /tmp/jblm.fk
```

Observed:

```text
134217727
```

Meaning:

- `1`: Win64 carrier byte list is safe.
- `2`: SysV carrier byte list is safe.
- `4`: checked array/list byte list is safe.
- `8`: checked field byte list is safe.
- `16`: checked div byte list is safe.
- `32`: arbitrary valid Form byte list is safe.
- `64`: loaded Win64 payload preserves the exact byte list.
- `128`: loaded checked-array payload preserves the exact byte list.
- `256`: arbitrary valid byte list adapts into the loader contract.
- `512`: arbitrary valid byte list loads into a safe slot.
- `1024`: generation-zero load is unsafe.
- `2048`: writable/unsealed loaded slot is unsafe.
- `4096`: passing guard/runtime/parity selects native.
- `8192`: guard failure selects deopt.
- `16384`: runtime failure selects exception.
- `32768`: invalidation selects rewalk.
- `65536`: parity failure selects deopt.
- `131072`: out-of-range byte execution is rejected.
- `262144`: foreign `c-lowering` execution is rejected.
- `524288`: empty byte list is rejected.
- `1048576`: byte `256` is rejected.
- `2097152`: negative byte is rejected.
- `4194304`: missing source metadata is rejected.
- `8388608`: missing deopt metadata is rejected.
- `16777216`: missing exception metadata is rejected.
- `33554432`: checked array/list bytes retain their bounds-fault branch.
- `67108864`: runtime fault receipts remain source-attributed.

## Honest boundary

This still does not call `fk_nat_install`, `fk_native_call`, or a host function
pointer. It makes the next host membrane step stricter: the thing to pass across
is now an exact validated Form byte list with full source/fault/deopt metadata,
not a summary count or a C-lowered blob.
