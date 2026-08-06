# Receipt - Form-owned JIT backend byte/op streams (2026-06-30)

## What landed

Added:

- `observe/jit-backend-bytes.fk`
- `observe/tests/jit-backend-bytes-band.fk`

This cell emits concrete backend payloads as Form data:

- x64 byte stream;
- arm64 operation-word stream;
- PTX-like operation-code stream.

It only emits when the request is owned by `form-backend` and includes source,
guard, fault, deopt, root, and ABI maps.

## Witness

Run:

```sh
( cat observe/jit-backend-bytes.fk observe/tests/jit-backend-bytes-band.fk ) > /tmp/jbb.fk
./fkwu --src /tmp/jbb.fk
```

Observed:

```text
16383
```

Meaning:

- `1`: x64 byte output is safe.
- `2`: arm64 op-word output is safe.
- `4`: PTX-like op-code output is safe.
- `8`: x64 payload length is stable.
- `16`: x64 payload starts with the prologue byte.
- `32`: x64 payload ends with `ret`.
- `64`: x64 payload includes the guard branch byte.
- `128`: arm64 payload length is stable.
- `256`: arm64 payload ends with the return word.
- `512`: PTX-like payload length is stable.
- `1024`: PTX-like payload includes the branch op category.
- `2048`: foreign `c-lowering` byte requests are rejected.
- `4096`: missing deopt metadata rejects emission.
- `8192`: missing source metadata rejects emission.

## Honest boundary

This is concrete backend payload emission as Form data, not loaded or executed
host machine code. It closes the gap between backend metadata and deterministic
payload bytes/op streams. Follow-up receipt
`2026-06-30-form-native-jit-loader-contract.md` adds the Form-owned loader and
executor contract over these payloads. The remaining lift is to provide the
runtime primitive that actually loads and calls Form-emitted payloads while
preserving the witnessed maps, guards, deopt/melt behavior, source-attributed
exceptions, and walker parity.
