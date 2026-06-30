# Receipt - Form-native JIT host exception bridge (2026-06-30)

## What landed

Added:

- `observe/jit-host-exception-bridge.fk`
- `observe/tests/jit-host-exception-bridge-band.fk`

This composes host ingress readiness with full-stack runtime fault attribution.
A future arbitrary-byte host ingress path may route a runtime failure to
`exception` only when the exception receipt is complete, every stack frame is
source-attributed, and native/walker parity compares the full stack.

## Witness

Run:

```sh
( cat observe/jit-runtime-fault.fk \
      observe/jit-runtime-stack-attribution.fk \
      observe/jit-host-ingress-readiness.fk \
      observe/jit-host-exception-bridge.fk \
      observe/tests/jit-host-exception-bridge-band.fk ) > /tmp/jheb.fk
./fkwu --src /tmp/jheb.fk
```

Observed:

```text
524287
```

Meaning:

- `1`: host-ingress, runtime-stack, runtime-fault, and checked-access receipts compose.
- `2`: live carrier status is admissible: correct or unavailable.
- `4`: a future matching-architecture bridge packet is complete.
- `8`: future matching-architecture runtime failure routes to exception.
- `16`: bounds exception has complete full-stack attribution.
- `32`: identical native/walker exceptions pass full-stack parity.
- `64`: parity rejects changed caller source attribution.
- `128`: exception source/throwing-frame mismatch rejects bridge completion.
- `256`: missing full-stack requirement rejects bridge completion.
- `512`: unavailable architecture deopts before claiming exception/native.
- `1024`: guard failure deopts.
- `2048`: invalidation rewalks.
- `4096`: stale cache melts.
- `8192`: parity failure deopts.
- `16384`: bad host-ingress receipt rejects.
- `32768`: bad runtime-stack receipt rejects.
- `65536`: missing source map rejects.
- `131072`: missing exception map rejects.
- `262144`: generation zero rejects.

## Honest boundary

This is still a Form receipt model. It does not expose arbitrary Form byte lists
to `fk_native_call_args` and it does not implement kernel-level structured
throwing from installed machine code. It closes a contract gap: once host byte
ingress arrives, exception routing is not complete unless full stack/source
attribution is present and parity-checkable.
