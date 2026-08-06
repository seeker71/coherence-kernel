# Receipt - Form-native JIT category-fed tiering lands (2026-06-30)

## What landed

The optimizer track now has an executable Form policy that composes heat/purity,
numeric category receipts, and source-attributed runtime-fault receipts into one
tier action.

New files:

- `observe/jit-tier-policy.fk`
- `observe/tests/jit-tier-policy-band.fk`

The tier actions are numeric and stable:

```text
1 baseline/profile
2 hold
3 crystallize
4 specialize
5 deopt
6 melt
```

Direct specialization is safety-gated: if the category asks for direct
field/index/hash/tree access, the policy requires an attributed runtime fault
receipt before choosing `specialize`. Without that attribution, a hot pure
receipt may still crystallize, but it does not get the direct specialized path.

## Witness

Run:

```sh
( cat observe/jit-decision.fk observe/jit-profile-receipt.fk observe/jit-runtime-fault.fk observe/jit-tier-policy.fk observe/tests/jit-tier-policy-band.fk ) > /tmp/jtp.fk
./fkwu --src /tmp/jtp.fk
```

Observed:

```text
1023
```

Meaning:

- `1`: two per-instance receipts in one category choose the same tier action.
- `2`: a changed shape changes the category signature.
- `4`: hot pure basic receipt crystallizes.
- `8`: hot safe direct array receipt specializes.
- `16`: hot direct array without attributed fault receipt only crystallizes.
- `32`: warm receipt holds.
- `64`: cold uninstalled receipt stays baseline/profile.
- `128`: cold installed receipt melts.
- `256`: guard-deopt receipt deopts.
- `512`: GPU buffer/range category specializes when attribution exists.

## Honest scope

This is still policy, not native lowering. It proves that tier selection can now
be driven by Form-native receipt data and guarded by runtime-fault attribution.
The next rung is to use this tier action to drive inlining/call-site receipts
and then the direct-access emitters.
