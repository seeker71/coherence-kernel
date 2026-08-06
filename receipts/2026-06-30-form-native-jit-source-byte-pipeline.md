# Receipt - Form source to exact byte pipeline (2026-06-30)

## What landed

Added:

- `observe/jit-source-byte-pipeline.fk`
- `observe/tests/jit-source-byte-pipeline-band.fk`

This composes the static analyzer with a compact exact-byte payload membrane in
one source-runner pass. A hot Form source plan must:

- pass `observe/form-static-analyzer.fk`
- select a known direct lowering
- emit exact Form-owned x64 bytes
- validate every byte as `0..255`
- preserve the exact bytes through a sealed executable slot
- route native/deopt/exception/rewalk from the admitted payload

The fuller exact byte-list membrane remains separately witnessed in
`observe/jit-byte-list-membrane.fk`. This cell stays compact so it can run with
the static analyzer in one `fkwu --src` envelope.

## Witness

Run:

```sh
( cat model/form-asm-x64.fk \
      observe/jit-runtime-fault.fk \
      observe/form-static-analyzer.fk \
      observe/jit-source-byte-pipeline.fk \
      observe/tests/jit-source-byte-pipeline-band.fk ) > /tmp/jsbp.fk
./fkwu --src /tmp/jsbp.fk
```

Observed:

```text
2097151
```

Meaning:

- `1`: hot source plan passes the static analyzer.
- `2`: `arg+1` source plan admits to exact bytes.
- `4`: checked array/list source plan admits.
- `8`: checked field source plan admits.
- `16`: checked div source plan admits.
- `32`: `arg+1` loaded slot preserves exact bytes.
- `64`: checked array/list loaded slot preserves exact bytes.
- `128`: checked div bytes are all valid bytes.
- `256`: passing guard/runtime/parity selects native.
- `512`: guard failure selects deopt.
- `1024`: runtime failure selects exception.
- `2048`: invalidation selects rewalk.
- `4096`: parity failure selects deopt.
- `8192`: static div-zero source rejects before byte admission.
- `16384`: cold source plan rejects.
- `32768`: foreign `c-lowering` owner rejects.
- `65536`: missing source metadata rejects.
- `131072`: missing deopt metadata rejects.
- `262144`: generation-zero load rejects.
- `524288`: checked array/list bytes retain their bounds-fault branch.
- `1048576`: runtime fault receipts remain source-attributed.

## Honest boundary

This still does not install the byte list with `fk_nat_install` or execute it
through `fk_native_call`. It closes the next Form-side pipe: source must clear
static analysis and hot admission before exact bytes can reach a sealed
load/action contract.
