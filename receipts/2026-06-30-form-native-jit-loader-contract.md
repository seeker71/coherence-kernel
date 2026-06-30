# Receipt - Form-owned JIT loader/executor contract (2026-06-30)

## What landed

Added:

- `observe/jit-loader-contract.fk`
- `observe/tests/jit-loader-contract-band.fk`

This cell models the final runtime gate before real host execution. A payload
can be loaded only when it is Form-owned and carries source, guard, fault,
deopt, root, ABI, relocation, safepoint, and exception maps. A loaded slot must
be executable, sealed, non-writable, and generation-stamped.

## Witness

Run:

```sh
( cat observe/jit-loader-contract.fk observe/tests/jit-loader-contract-band.fk ) > /tmp/jlc.fk
./fkwu --src /tmp/jlc.fk
```

Observed:

```text
16383
```

Meaning:

- `1`: safe Form-owned payload loads into an executable sealed slot.
- `2`: foreign `c-lowering` payload is rejected.
- `4`: missing deopt metadata rejects load.
- `8`: missing source metadata rejects load.
- `16`: writable/unsealed loaded slot is unsafe.
- `32`: guard/runtime/parity passing execution selects native.
- `64`: native execution returns the native value.
- `128`: guard failure selects deopt.
- `256`: deopt returns walker fallback.
- `512`: runtime failure selects exception.
- `1024`: exception returns attributed exception kind.
- `2048`: invalidation rewalks.
- `4096`: parity failure deopts.
- `8192`: malformed loaded slot is rejected.

## Honest boundary

This is a Form-owned loader/executor contract, not itself an `mmap`/W^X host
primitive and not itself a function pointer call. The runtime already has a
narrow host install/call membrane (`fk_native_call`, `fk_native_call_args`,
`fk_nat_install`); this receipt names the gates that membrane must satisfy before
payload bytes can be treated as completed native execution. The remaining lift is
to feed Form-emitted payloads through that membrane while preserving this
contract and without moving optimizer decisions into C.
