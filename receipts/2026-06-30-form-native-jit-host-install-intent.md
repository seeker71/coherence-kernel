# Receipt - Form host install intent contract (2026-06-30)

## What landed

Added:

- `observe/jit-host-install-intent.fk`
- `observe/tests/jit-host-install-intent-band.fk`

This defines the Form-owned envelope that a future `fk_nat_install` /
`fk_native_call` bridge must consume. It combines exact byte receipt status,
native dispatch receipt status, source/maps, W^X sealing, executable/non-writable
slot state, argument maps, cache state, carrier status, and generation.

## Witness

Run:

```sh
( cat observe/jit-host-install-intent.fk \
      observe/tests/jit-host-install-intent-band.fk ) > /tmp/jhii.fk
./fkwu --src /tmp/jhii.fk
```

Observed:

```text
131071
```

Meaning:

- `1`: fully mapped install intent is safe.
- `2`: passing guard/runtime/parity/carrier/non-stale path selects native.
- `4`: guard failure selects deopt.
- `8`: runtime failure selects exception.
- `16`: invalidation selects rewalk.
- `32`: parity failure selects deopt.
- `64`: stale cache selects melt.
- `128`: unavailable carrier selects deopt.
- `256`: missing exact-byte receipt rejects.
- `512`: missing dispatch receipt rejects.
- `1024`: missing source metadata rejects.
- `2048`: missing map metadata rejects.
- `4096`: writable executable slot rejects.
- `8192`: unsealed slot rejects.
- `16384`: missing argument map rejects.
- `32768`: missing cache state rejects.
- `65536`: generation-zero install rejects.

## Honest boundary

This still does not call `fk_nat_install`, `fk_native_call`, or a host function
pointer. It makes the future host bridge narrower: native completion may only be
claimed for an exact-byte, dispatch-witnessed, source-attributed, sealed,
non-writable executable intent with arguments, cache state, carrier status, and
positive generation.
