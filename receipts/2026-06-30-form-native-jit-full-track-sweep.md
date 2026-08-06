# Receipt - Form-native JIT full track sweep (2026-06-30)

## What landed

Added:

- `observe/jit-full-track-sweep.fk`
- `observe/tests/jit-full-track-sweep-band.fk`

This is the compact full-track ledger for the Form-native JIT work. It composes
the profile/category, tiering, runtime fault, inlining, static analyzer,
stack/frame, representation specialization, register lowering, deopt/cache,
emitter, IR, backend, byte-list, source-byte, dispatch, install-intent, and
native-call-plan receipts.

## Witness

Run:

```sh
( cat observe/jit-full-track-sweep.fk \
      observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
```

Observed:

```text
524287
```

Meaning:

- `1`: policy/front-end receipts compose.
- `2`: access/register/cache receipts compose.
- `4`: emitter/IR/backend/loader/admission receipts compose.
- `8`: carrier/byte/source/cache/dispatch/install/call-plan bridge receipts
  compose.
- `16`: all Form-owned receipts compose.
- `32`: host arbitrary-byte execution is still not witnessed.
- `64`: full completion remains false.
- `128`: a passing native path stays pending rather than overclaiming host
  execution.
- `256`: guard failure deopts.
- `512`: runtime failure selects exception.
- `1024`: invalidation rewalks.
- `2048`: parity failure deopts.
- `4096`: unavailable carrier deopts.
- `8192`: stale cache melts.
- `16384`: bad representation receipt rejects.
- `32768`: missing source metadata rejects.
- `65536`: missing map metadata rejects.
- `131072`: native-call-plan receipt is exact.
- `262144`: host-install-intent receipt is exact.

## Honest boundary

This is not the completion receipt. It explicitly proves that the current
full-track state remains pending until arbitrary Form-emitted byte lists can be
installed and called through the host membrane with source-attributed runtime
exceptions, guarded deopt/melt behavior, and parity witnesses intact.
