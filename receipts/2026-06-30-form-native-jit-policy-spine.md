# Receipt - Form-native JIT policy spine rungs (2026-06-30)

## What landed

This receipt extends the Form-native JIT track beyond profile/tiering/runtime
faults/inlining/static analysis with four additional policy/receipt cells:

- `observe/jit-stack-frame.fk`
- `observe/jit-representation-specialization.fk`
- `observe/jit-register-lowering.fk`
- `observe/jit-deopt-cache.fk`

It also adds grouped policy sweeps:

- `observe/tests/jit-policy-front-sweep-band.fk`
- `observe/tests/jit-policy-access-sweep-band.fk`
- `observe/tests/jit-policy-cache-sweep-band.fk`

## Witnesses

Run:

```sh
( cat observe/jit-profile-receipt.fk observe/jit-stack-frame.fk observe/tests/jit-stack-frame-band.fk ) > /tmp/jsf.fk
./fkwu --src /tmp/jsf.fk

( cat observe/jit-profile-receipt.fk observe/jit-runtime-fault.fk observe/jit-representation-specialization.fk observe/tests/jit-representation-specialization-band.fk ) > /tmp/jrs.fk
./fkwu --src /tmp/jrs.fk

( cat observe/jit-profile-receipt.fk observe/jit-runtime-fault.fk observe/jit-register-lowering.fk observe/tests/jit-register-lowering-band.fk ) > /tmp/jrl.fk
./fkwu --src /tmp/jrl.fk

( cat observe/jit-decision.fk observe/jit-profile-receipt.fk observe/jit-runtime-fault.fk observe/jit-tier-policy.fk observe/jit-deopt-cache.fk observe/tests/jit-deopt-cache-band.fk ) > /tmp/jdc.fk
./fkwu --src /tmp/jdc.fk

( cat observe/jit-decision.fk observe/jit-profile-receipt.fk observe/jit-runtime-fault.fk observe/jit-tier-policy.fk observe/jit-inline-policy.fk observe/form-static-analyzer.fk observe/tests/jit-policy-front-sweep-band.fk ) > /tmp/jpfs.fk
./fkwu --src /tmp/jpfs.fk

( cat observe/jit-profile-receipt.fk observe/jit-runtime-fault.fk observe/jit-stack-frame.fk observe/jit-representation-specialization.fk observe/jit-register-lowering.fk observe/tests/jit-policy-access-sweep-band.fk ) > /tmp/jpas.fk
./fkwu --src /tmp/jpas.fk

( cat observe/jit-decision.fk observe/jit-profile-receipt.fk observe/jit-runtime-fault.fk observe/jit-tier-policy.fk observe/jit-deopt-cache.fk observe/tests/jit-policy-cache-sweep-band.fk ) > /tmp/jpcs.fk
./fkwu --src /tmp/jpcs.fk
```

Observed:

```text
255
1023
511
511
31
7
15
```

## Coverage

`jit-stack-frame.fk` proves the frame-collapse contract: source signature match,
root map requirement, tail-looping, elided frame collapse, spill fallback, deopt
on guarded profile, and parity receipt shape.

`jit-representation-specialization.fk` proves the direct-access admission rules:
field offset, cons-list head/tail, array/buffer index, dict lookup, hashmap
bucket, red-black-tree ordered path, collision fallback, source-signature guard,
and source-attributed runtime fault requirement.

`jit-register-lowering.fk` proves the register-lowering contract: CPU scalar,
CPU SIMD, GPU warp, spill/root-map handling, fault-map requirement, source
signature guard, deopt, and parity receipt shape.

`jit-deopt-cache.fk` proves guarded deopt/melt/cache policy: install, keep,
invalidate, attributed deopt, invalidation-to-rewalk, stale melt, and fallback
requirement.

The grouped sweeps return `31`, `7`, and `15`, covering all nine currently
landed JIT policy bands while staying under the current direct-source symbol
table.

## Honest boundary

This is still policy and receipt tissue. It does not claim that native machine
code emission, dispatch, or runtime exception throwing is complete.

There is also a current `fkwu --src` composition floor: the source symbol table
has 256 entries (`fk_fnsym_s[256]` in the C seed). A single mega-concatenation
of every JIT policy file exceeds that envelope and later function names degrade
to `nothing`. The grouped sweeps avoid growing the C seed and record the floor
honestly. Follow-up receipt `2026-06-30-form-native-jit-emitter-bundle.md`
lands the compact Form-native emitter/dispatch receipt model that consumes
these witnessed summaries. The still-pending track work is executable native
dispatch/runtime integration without moving optimizer meaning into C.
