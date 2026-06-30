# Receipt - Form byte ingress and native args bridge (2026-06-30)

## What landed

Added:

- `observe/jit-byte-ingress-args.fk`
- `observe/tests/jit-byte-ingress-args-band.fk`

This defines the Form-owned packet that must feed the existing host
`fk_native_call_args` / `fk_nat_install` door: exact byte list, byte count, ABI,
tagged argument vector, rooted frame, source/exception maps, deopt metadata, and
W^X/sealed install state.

## Witness

Run:

```sh
( cat observe/jit-byte-ingress-args.fk \
      observe/tests/jit-byte-ingress-args-band.fk ) > /tmp/jbia.fk
./fkwu --src /tmp/jbia.fk
```

Observed:

```text
2097151
```

Meaning:

- `1`: byte membrane, native-call-plan, and host-membrane receipts compose.
- `2`: one-argument ingress packet is safe.
- `4`: two-argument ingress packet is safe.
- `8`: exact byte image is preserved.
- `16`: without host byte ingress, safe packet remains pending.
- `32`: with host ingress, passing path selects native.
- `64`: guard failure deopts.
- `128`: runtime failure selects exception.
- `256`: invalidation rewalks.
- `512`: parity failure deopts.
- `1024`: stale cache melts.
- `2048`: byte-count mismatch rejects.
- `4096`: invalid byte rejects.
- `8192`: untagged argument rejects.
- `16384`: too many arguments reject.
- `32768`: frame smaller than arg vector rejects.
- `65536`: foreign `c-lowering` owner rejects.
- `131072`: unknown ABI rejects.
- `262144`: missing source map rejects.
- `524288`: missing W^X/sealed state rejects.
- `1048576`: bad byte-membrane receipt rejects.

## Honest boundary

This still does not expose arbitrary Form byte lists to the runtime host
primitive. It makes the ingress contract precise enough to wire: the host door
may only consume Form-owned valid bytes with a matching byte count, tagged
arguments, a rooted frame, source and exception maps, deopt metadata, ABI
agreement, W^X/sealed state, and positive generation.
