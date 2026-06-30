# Receipt - Form-native JIT host W^X audit (2026-06-30)

## What landed

Added:

- `observe/jit-host-wx-audit.fk`
- `observe/tests/jit-host-wx-audit-band.fk`

This audits the host install/call membrane against the JIT track's W^X
requirement. The current Windows door writes bytes into RW memory and flips the
page to RX. The current POSIX door still maps RWX, so native completion remains
pending there until the carrier changes to write-then-execute sealing.

## Witness

Run:

```sh
( cat observe/jit-host-wx-audit.fk \
      observe/tests/jit-host-wx-audit-band.fk ) > /tmp/jhwa.fk
./fkwu --src /tmp/jhwa.fk
```

Observed:

```text
1048575
```

Meaning:

- `1`: byte-ingress, args-vector, host-membrane, and post-ingress receipts
  compose.
- `2`: current Win64 host memory policy is W^X-safe.
- `4`: current POSIX host memory policy is not W^X-safe.
- `8`: future POSIX write-then-execute policy is W^X-safe.
- `16`: current POSIX install is not ready.
- `32`: current POSIX install remains pending, not native.
- `64`: future POSIX install is ready.
- `128`: current Win64 install is ready.
- `256`: future ready install selects native on a passing path.
- `512`: guard failure deopts.
- `1024`: runtime failure selects exception.
- `2048`: invalidation rewalks.
- `4096`: parity failure deopts.
- `8192`: stale cache melts.
- `16384`: missing executable stage rejects memory safety.
- `32768`: unknown platform rejects memory safety.
- `65536`: missing byte ingress, args vector, source, exception, deopt, parity,
  or positive generation rejects readiness.
- `131072`: bad byte-ingress receipt rejects.
- `262144`: byte-ingress receipt is exact.
- `524288`: args-vector receipt is exact.

## Honest boundary

This still does not change `runtime/fkwu-uni.c` or expose arbitrary Form byte
lists to the host function pointer. It prevents the track from overclaiming the
current POSIX install door: RWX memory is not a completed native JIT membrane.
