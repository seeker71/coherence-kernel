# Receipt - Form-native JIT host ingress readiness (2026-06-30)

## What landed

Added:

- `observe/jit-host-ingress-readiness.fk`
- `observe/tests/jit-host-ingress-readiness-band.fk`

This composes the post-W^X host membrane state. The POSIX x64 carrier now has a
write-then-execute page policy, but native completion still requires arbitrary
Form byte-list ingress and a matching executable architecture. On this non-x64
POSIX checkout, the live x64 carrier is unavailable and must deopt rather than
claim native execution.

## Witness

Run:

```sh
( cat observe/jit-host-ingress-readiness.fk \
      observe/tests/jit-host-ingress-readiness-band.fk ) > /tmp/jhir.fk
./fkwu --src /tmp/jhir.fk
```

Observed:

```text
1048575
```

Meaning:

- `1`: byte-ingress, args-vector, host-W^X, host-membrane, and post-ingress receipts compose.
- `2`: host-W^X audit receipt is exact.
- `4`: live `native_call_test 41` is admissible: correct or unavailable.
- `8`: live `native_call_test 99` is admissible: correct or unavailable.
- `16`: without arbitrary byte ingress, the membrane is not ready.
- `32`: without arbitrary byte ingress, the current path stays pending.
- `64`: a fully-described ingress packet is ready at the Form metadata level.
- `128`: unavailable executable architecture deopts.
- `256`: this live checkout is not a complete native carrier.
- `512`: future matching-architecture ingress selects native.
- `1024`: guard failure deopts.
- `2048`: runtime failure selects exception.
- `4096`: invalidation rewalks.
- `8192`: parity failure deopts.
- `16384`: stale cache melts.
- `32768`: carrier mismatch rejects.
- `65536`: missing byte ingress, args, W^X, source, or positive generation rejects readiness.
- `131072`: bad host-W^X receipt rejects.
- `262144`: byte-ingress receipt is exact.
- `524288`: args-vector receipt is exact.

## Honest boundary

This still does not expose arbitrary Form byte lists to `fk_native_call_args`.
It narrows the remaining bridge after the W^X repair: native completion now
depends on arbitrary Form byte ingress and a matching executable architecture.
