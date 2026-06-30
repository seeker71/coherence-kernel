# Receipt - Form-native JIT post-ingress sweep (2026-06-30)

## What landed

Added:

- `observe/jit-post-ingress-sweep.fk`
- `observe/tests/jit-post-ingress-sweep-band.fk`

This folds the byte-ingress/native-args bridge into the compact full-track
ledger. It keeps completion honest: the current state has Form-owned byte and
argument ingress receipts, but host arbitrary-byte ingress is still not wired.

## Witness

Run:

```sh
( cat observe/jit-post-ingress-sweep.fk \
      observe/tests/jit-post-ingress-sweep-band.fk ) > /tmp/jpis.fk
./fkwu --src /tmp/jpis.fk
```

Observed:

```text
1048575
```

Meaning:

- `1`: full-track, host-membrane, byte-ingress, call-plan, byte-membrane, and
  host-handoff receipts compose.
- `2`: current post-ingress state is ready on the Form side.
- `4`: current post-ingress state is still not complete.
- `8`: current passing native path remains pending without host ingress.
- `16`: future host-ingress path selects native.
- `32`: guard failure deopts.
- `64`: runtime failure selects exception.
- `128`: invalidation rewalks.
- `256`: parity failure deopts.
- `512`: stale cache melts.
- `1024`: carrier unavailable deopts.
- `2048`: bad byte-ingress receipt rejects.
- `4096`: bad full-track receipt rejects.
- `8192`: missing source metadata rejects.
- `16384`: missing map metadata rejects.
- `32768`: native-call-plan receipt is exact.
- `65536`: byte-ingress receipt is exact.
- `131072`: host-membrane readiness receipt is exact.
- `262144`: host-handoff receipt is exact.
- `524288`: missing byte-ingress state is not ready.

## Honest boundary

This still does not expose arbitrary byte-list host execution. It proves the
latest Form-side state is ready up to the host-ingress flag, and that the
native/deopt/exception/rewalk/melt routes are already specified for the future
host bridge.
