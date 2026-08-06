# Receipt - Form-native JIT host W^X audit (2026-06-30)

## What landed

Added:

- `observe/jit-host-wx-audit.fk`
- `observe/tests/jit-host-wx-audit-band.fk`

This audits the host install/call membrane against the JIT track's W^X
requirement. Windows and POSIX x64 now both write bytes into RW memory and flip
the page to RX before calling or caching the native slot. Non-x64 POSIX hosts
return unavailable for these x64 payloads instead of executing them. The old
POSIX RWX policy is retained in the witness only as a rejected historical shape.

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
- `4`: current POSIX x64 host memory policy is W^X-safe.
- `8`: old POSIX RWX memory policy is not W^X-safe.
- `16`: current POSIX x64 install is ready.
- `32`: current non-x64 POSIX install remains pending/unavailable.
- `64`: old POSIX RWX install is not ready.
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

This still does not expose arbitrary Form byte lists to the host function
pointer. The C seed change is carrier-only: it repairs the host executable-page
membrane, guards x64 payload execution by architecture, and adds no optimizer
meaning or lowering rule to C.
