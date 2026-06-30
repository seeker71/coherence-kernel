# Receipt - Form-emitted carrier ABI bytes (2026-06-30)

## What landed

Added:

- `observe/jit-carrier-abi.fk`
- `observe/tests/jit-carrier-abi-band.fk`

Updated:

- `model/form-asm-x64.fk`

The x64 Form encoder now emits the compact `add rax, imm8` instruction and the
SysV `mov rax, rdi` argument move. The new carrier ABI witness proves Form emits
the exact Win64 and SysV byte images for the host carrier's `f(a)=a+1` payload:

```text
Win64: 48 89 C8 48 83 C0 01 C3
SysV : 48 89 F8 48 83 C0 01 C3
```

Those are the two byte images `fk_native_call_test` uses around the existing
host install/call membrane.

## Witness

Run:

```sh
( cat model/form-asm-x64.fk \
      observe/jit-carrier-abi.fk \
      observe/tests/jit-carrier-abi-band.fk ) > /tmp/jca.fk
./fkwu --src /tmp/jca.fk
```

Observed on this Mac checkout:

```text
32767
```

Meaning:

- `1`: Form emits the exact Win64 carrier byte image.
- `2`: Form emits the exact SysV carrier byte image.
- `4`: Win64 payload length is eight bytes.
- `8`: SysV payload length is eight bytes.
- `16`: the compact `83 /0 imm8` add opcode is present.
- `32`: the payload returns with `ret`.
- `64`: Win64 image carries source/map metadata.
- `128`: SysV image carries source/map metadata.
- `256`: a foreign `c-lowering` ABI label is visible as foreign, not normalized
  into Form ownership.
- `512`: missing source metadata is rejected.
- `1024`: live `native_call_test 41` is either correct or honestly unavailable.
- `2048`: live `native_call_test 99` is either correct or honestly unavailable.
- `4096`: an incorrect live result is classified as mismatch.
- `8192`: a successful carrier status counts as complete.
- `16384`: an unavailable carrier status does not count as complete.

## Honest boundary

On this Mac checkout, `native_call_test` currently returns `-1`; that is
classified as host-carrier unavailable, not as a native execution success. This
receipt closes the byte/ABI equivalence gap between Form emission and the
existing carrier test payload. It does not yet provide the general source path
that passes arbitrary Form-emitted byte lists into `fk_native_call` or
`fk_nat_install`.
