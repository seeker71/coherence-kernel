# Receipt - Form native JIT host membrane readiness (2026-06-30)

## What landed

Added:

- `observe/jit-host-membrane-readiness.fk`
- `observe/tests/jit-host-membrane-readiness-band.fk`

This audits the exact remaining host membrane condition without growing the C
seed. The current runtime exposes a hardcoded `native_call_test` carrier, but
the full JIT still needs arbitrary Form byte-list ingress, argument-vector
ingress, W^X/sealed install state, and source-attributed exception/deopt routing
through that membrane before native completion can be claimed.

## Witness

Run:

```sh
( cat observe/jit-host-membrane-readiness.fk \
      observe/tests/jit-host-membrane-readiness-band.fk ) > /tmp/jhm.fk
./fkwu --src /tmp/jhm.fk
```

Observed:

```text
524287
```

Meaning:

- `1`: full-track sweep receipt is exact.
- `2`: native-call-plan receipt is exact.
- `4`: byte-list membrane receipt is exact.
- `8`: live `native_call_test 41` is admissible: correct or unavailable.
- `16`: live `native_call_test 99` is admissible: correct or unavailable.
- `32`: current membrane is not complete.
- `64`: current membrane routes a passing native path to pending.
- `128`: future membrane with byte ingress, args, W^X, source exceptions,
  deopt/melt, parity, and positive generation is ready.
- `256`: future passing path selects native.
- `512`: guard failure deopts.
- `1024`: runtime failure selects exception.
- `2048`: invalidation rewalks.
- `4096`: parity failure deopts.
- `8192`: stale cache melts.
- `16384`: missing arbitrary byte ingress remains pending.
- `32768`: missing argument ingress remains pending.
- `65536`: missing W^X/sealed state remains pending.
- `131072`: carrier mismatch rejects.
- `262144`: bad full-track receipt rejects.

## Honest boundary

This still does not call arbitrary Form-emitted bytes through `fk_native_call` or
`fk_native_call_args`, and it does not change `runtime/fkwu-uni.c`. It narrows
the runtime bridge that remains: expose a Form-owned byte-list ingress into the
existing host install/call door, carry argument vectors, preserve W^X/sealing,
and return source-attributed exception/deopt/melt outcomes.
