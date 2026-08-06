# Receipt - Form source byte handoff to host carrier status (2026-06-30)

## What landed

Added:

- `observe/jit-host-handoff.fk`
- `observe/tests/jit-host-handoff-band.fk`

This composes the hot source to exact byte pipeline with the existing
`native_call_test` carrier status. It proves that an admitted source byte
payload only claims native execution when the carrier result is correct. If the
host carrier is unavailable, the handoff deopts to walker. If the carrier
returns a wrong value, the handoff rejects.

## Witness

Run:

```sh
( cat model/form-asm-x64.fk \
      observe/jit-runtime-fault.fk \
      observe/form-static-analyzer.fk \
      observe/jit-source-byte-pipeline.fk \
      observe/jit-host-handoff.fk \
      observe/tests/jit-host-handoff-band.fk ) > /tmp/jhh.fk
./fkwu --src /tmp/jhh.fk
```

Observed:

```text
524287
```

Meaning:

- `1`: hot `arg+1` source plan admits to exact bytes.
- `2`: source bytes match the SysV carrier ABI image.
- `4`: live `native_call_test 41` is either correct or honestly unavailable.
- `8`: live `native_call_test 99` is either correct or honestly unavailable.
- `16`: a correct carrier status is complete.
- `32`: an unavailable carrier status is not complete.
- `64`: unavailable carrier deopts to walker.
- `128`: mismatched carrier rejects.
- `256`: correct carrier plus passing guard/runtime/parity selects native.
- `512`: correct carrier plus guard failure selects deopt.
- `1024`: correct carrier plus runtime failure selects exception.
- `2048`: correct carrier plus invalidation selects rewalk.
- `4096`: correct carrier plus parity failure selects deopt.
- `8192`: checked-array bytes are not accepted as the `arg+1` host carrier image.
- `16384`: cold source rejects.
- `32768`: static div-zero source rejects before handoff.
- `65536`: admitted source bytes are preserved.
- `131072`: admitted source bytes are all valid bytes.
- `262144`: mismatched carrier status is not admissible.

## Honest boundary

This still does not pass arbitrary Form byte lists into `fk_native_call` or
`fk_nat_install`. It closes the host-status side of the handoff: the Form JIT
pipeline may only call a native result complete when the existing carrier proves
the expected value. On hosts where the carrier is unavailable, this receipt
demands deopt rather than overclaiming native success.
