# Receipt - Form native-call plan contract (2026-06-30)

## What landed

Added:

- `observe/jit-native-call-plan.fk`
- `observe/tests/jit-native-call-plan-band.fk`

This is a compact Form contract for the future `fk_native_call` /
`fk_native_call_args` bridge. It composes the prior static analyzer, runtime
fault, source-byte pipeline, native dispatch sweep, and host-install intent
receipts before a native call plan can be considered ready.

## Witness

Run:

```sh
( cat observe/jit-native-call-plan.fk \
      observe/tests/jit-native-call-plan-band.fk ) > /tmp/jncp.fk
./fkwu --src /tmp/jncp.fk
```

Observed:

```text
2097151
```

Meaning:

- `1`: static analyzer receipt total is exact.
- `2`: runtime fault receipt total is exact.
- `4`: source-byte pipeline receipt total is exact.
- `8`: native dispatch sweep receipt total is exact.
- `16`: host-install intent receipt total is exact.
- `32`: all receipt rows compose.
- `64`: call plan is ready only with args, source stack, exception map, ABI,
  callee slot, and positive generation.
- `128`: passing guard/runtime/parity/carrier/non-stale path selects native.
- `256`: guard failure selects deopt.
- `512`: runtime failure selects exception.
- `1024`: invalidation selects rewalk.
- `2048`: parity failure selects deopt.
- `4096`: unavailable carrier selects deopt.
- `8192`: stale cache selects melt.
- `16384`: missing argument map rejects.
- `32768`: missing source stack rejects.
- `65536`: missing exception map rejects.
- `131072`: wrong ABI rejects.
- `262144`: missing callee slot rejects.
- `524288`: bad source-byte receipt rejects.
- `1048576`: bad host-install intent receipt rejects.

## Honest boundary

This still does not call `fk_native_call`, `fk_native_call_args`,
`fk_nat_install`, or a host function pointer. It narrows the next bridge: no
native-call completion may be claimed unless the previous Form receipts compose
and the call plan carries argument maps, source stack attribution, exception
maps, ABI agreement, callee slot state, carrier status, and positive generation.
