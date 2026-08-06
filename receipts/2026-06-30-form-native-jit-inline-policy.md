# Receipt - Form-native JIT call tracking and inlining policy lands (2026-06-30)

## What landed

The JIT track now has an executable Form policy for call-site inlining. It does
not emit an inline body yet; it defines the conditions the emitter must satisfy
before it may erase a call boundary.

New files:

- `observe/jit-inline-policy.fk`
- `observe/tests/jit-inline-policy-band.fk`

The inline actions are numeric and stable:

```text
0 no-inline
1 inline
2 guarded-inline
3 fallback-call
4 deopt
```

The policy requires:

- a category-fed tier action that is at least crystallize/specialize;
- monomorphic target for direct inline, or low-polymorphic target for guarded
  inline;
- inline budget and depth limit;
- target/source guard;
- fallback call path;
- matching source/category signature;
- deopt when the call receipt is already marked guard-failed.

## Witness

Run:

```sh
( cat observe/jit-decision.fk observe/jit-profile-receipt.fk observe/jit-runtime-fault.fk observe/jit-tier-policy.fk observe/jit-inline-policy.fk observe/tests/jit-inline-policy-band.fk ) > /tmp/jip.fk
./fkwu --src /tmp/jip.fk
```

Observed:

```text
1023
```

Meaning:

- `1`: monomorphic hot safe call inlines.
- `2`: low-polymorphic hot safe call guarded-inlines.
- `4`: megamorphic call falls back.
- `8`: over-budget call falls back.
- `16`: missing target guard falls back.
- `32`: missing fallback call path falls back.
- `64`: source/category signature mismatch falls back.
- `128`: deopt-marked call deopts.
- `256`: warm/non-tiered profile prevents inline.
- `512`: per-instance call receipts collapse to one call category.

## Honest scope

This is policy and witness, not native code emission. The next inlining rung is
to make the Form emitter consume this policy, inline a small monomorphic call,
and prove walker/native parity plus correct fallback/deopt behavior.
