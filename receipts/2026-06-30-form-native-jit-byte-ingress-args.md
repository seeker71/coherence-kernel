# Receipt - Form byte ingress and native args bridge (2026-06-30)

## What landed

Added:

- `observe/jit-byte-ingress-args.fk`
- `observe/tests/jit-byte-ingress-args-band.fk`

This defines the Form-owned packet that must feed the existing host
`fk_native_call_args` / `fk_nat_install` door: exact byte list, byte count, ABI,
tagged argument vector, rooted frame, source/exception maps, deopt metadata, and
W^X/sealed install state.

Amended after the args-vector ABI witness: the admitted byte image now loads
from the `long long *args` pointer (`[rdi]` on SysV, `[rcx]` on Win64). The
older one-register carrier payload is rejected for this membrane.

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

- `1`: byte membrane, native-call-plan, host-membrane, and args-vector payload
  receipts compose.
- `2`: SysV one-argument arg-vector ingress packet is safe.
- `4`: Win64 one-argument arg-vector ingress packet is safe.
- `8`: SysV two-argument arg-vector ingress packet is safe.
- `16`: the old register-argument carrier payload rejects.
- `32`: exact SysV arg-vector byte image is preserved.
- `64`: without host byte ingress, safe packet remains pending.
- `128`: with host ingress, passing path selects native.
- `256`: guard failure deopts.
- `512`: runtime failure selects exception.
- `1024`: invalidation rewalks.
- `2048`: parity failure deopts.
- `4096`: stale cache melts.
- `8192`: byte-count mismatch rejects.
- `16384`: invalid byte rejects.
- `32768`: untagged argument rejects.
- `65536`: too many arguments reject.
- `131072`: frame smaller than arg vector rejects.
- `262144`: foreign `c-lowering` owner rejects.
- `524288`: unknown ABI rejects.
- `1048576`: missing source, missing W^X/sealed state, and bad byte-membrane receipt reject.

## Honest boundary

This still does not expose arbitrary Form byte lists to the runtime host
primitive. It makes the ingress contract precise enough to wire: the host door
may only consume Form-owned valid args-vector bytes with a matching byte count,
tagged arguments, a rooted frame, source and exception maps, deopt metadata, ABI
agreement, W^X/sealed state, and positive generation.
