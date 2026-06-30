# Receipt - Form-native JIT merge checkpoint (2026-06-30)

## What this checkpoint makes mergeable

This checkpoint adds a Form-owned JIT optimization track without growing the C
seed:

- profile receipts and numeric category collapse,
- runtime-fault receipts with source/stack attribution,
- tiering, inlining, stack/frame collapse, representation specialization,
- CPU/GPU register-lowering policy,
- deopt/cache policy,
- static analyzer gate for pre-execution/pre-JIT facts,
- compact emitter, IR, host-dispatch, backend-emission, backend-byte, and
  loader/executor contracts.

The core stdlib body was also moved out of BML into direct Form so high-semantic
source can concatenate and run through the direct source runner.

## Witness sweep

Required bootstrap:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk                 -> 42
./fkwu --src bootstrap/ground-recursive.fk 10    -> 55
./fkwu --src /tmp/nvr.fk                         -> 11111
```

Core and JIT track:

```text
core-band                         -> 255
jit-profile-receipt-band          -> 127
jit-runtime-fault-band            -> 255
jit-tier-policy-band              -> 1023
jit-inline-policy-band            -> 1023
form-static-analyzer-band         -> 1023
jit-stack-frame-band              -> 255
jit-representation-specialization -> 1023
jit-register-lowering-band        -> 511
jit-deopt-cache-band              -> 511
jit-policy-front-sweep            -> 31
jit-policy-access-sweep           -> 7
jit-policy-cache-sweep            -> 15
jit-emitter-bundle-band           -> 8191
jit-native-ir-band                -> 16383
jit-host-dispatch-band            -> 8191
jit-backend-emission-band         -> 8191
jit-backend-bytes-band            -> 16383
jit-loader-contract-band          -> 16383
```

## Existing runtime door

The runtime already contains the narrow host install/call door:
`fk_native_call`, `fk_native_call_args`, and `fk_nat_install`. Earlier receipts
name it as the host-side membrane for Form-emitted native bytes. This checkpoint
does not modify that door.

## Honest boundary

The opt-in in-process `FK_JIT=1` path in `runtime/fkwu-uni.c` still contains a C
proof-of-concept lowerer. It is not counted as the completion witness for this
track.

On this Mac checkout, a fresh optional probe crystallized but did not return the
walker value:

```text
./fkwu --src /tmp/jit-inc.fk
42

FK_JIT=1 FK_JIT_WITNESS=1 ./fkwu --src /tmp/jit-inc.fk
[jit] fn1 crystallized in-process: 118 bytes, njit=1 (native dispatch)
nothing

./fkwu --src /tmp/jit-sum.fk
500500

FK_JIT=1 FK_JIT_WITNESS=1 ./fkwu --src /tmp/jit-sum.fk
[jit] fn1 crystallized in-process: 398 bytes, njit=1 (native dispatch)
nothing
```

So the next sibling should not treat the C proof-of-concept lowerer as complete
on this host. The completion path is to feed Form-owned IR/backend/byte payloads
through the existing host install/call membrane, keep the optimizer decisions in
Form, and satisfy the loader/executor contract with source-attributed faults,
deopt, melt, and walker parity.
