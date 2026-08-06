# Receipt - Form-native JIT args-vector payload ABI (2026-06-30)

## What landed

Added:

- `observe/jit-args-vector-payload.fk`
- `observe/tests/jit-args-vector-payload-band.fk`

This narrows the native call membrane for `fk_native_call_args`. That host door
calls generated code as `fn(long long *args)`, so an accepted payload must load
from the argument-vector pointer. The older direct carrier payload
`mov rax, rdi; add rax, 1; ret` is deliberately rejected for this membrane.

## Witness

Run:

```sh
( cat observe/jit-args-vector-payload.fk \
      observe/tests/jit-args-vector-payload-band.fk ) > /tmp/javp.fk
./fkwu --src /tmp/javp.fk
```

Observed:

```text
2097151
```

Meaning:

- `1`: byte-ingress, post-ingress, host-membrane, and byte-list receipts compose.
- `2`: SysV one-arg arg-vector payload is safe.
- `4`: Win64 one-arg arg-vector payload is safe.
- `8`: SysV two-arg arg-vector payload is safe.
- `16`: Win64 two-arg arg-vector payload is safe.
- `32`: SysV one-arg bytes are exact: `mov rax, [rdi]; add rax, 2; ret`.
- `64`: Win64 one-arg bytes are exact: `mov rax, [rcx]; add rax, 2; ret`.
- `128`: SysV two-arg bytes are exact: `mov rax, [rdi]; add rax, [rdi+8]; ret`.
- `256`: the old register-argument carrier payload rejects for arg-vector use.
- `512`: without host ingress, safe payloads remain pending.
- `1024`: with host ingress, a passing path selects native.
- `2048`: guard failure deopts.
- `4096`: runtime failure selects exception.
- `8192`: invalidation rewalks.
- `16384`: parity failure deopts.
- `32768`: stale cache melts.
- `65536`: malformed byte lists reject.
- `131072`: bad ABI rejects.
- `262144`: missing source metadata rejects.
- `524288`: missing W^X state rejects.
- `1048576`: bad post-ingress receipt rejects.

## Honest boundary

This still does not expose arbitrary Form byte lists to the host function
pointer. It makes the next host bridge stricter: the bridge must accept exact
arg-vector payload bytes, preserve source/exception/deopt/W^X metadata, and
only then flip host ingress from pending to native.
