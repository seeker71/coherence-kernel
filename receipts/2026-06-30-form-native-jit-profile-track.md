# Receipt - Form-native JIT optimizer track starts with profile categories (2026-06-30)

## What landed

The JIT optimization goal now has a repo-native track document and the first
executable Form cell:

- `docs/form-native-jit-track.form` names the full rung ladder from profile
  receipts through inlining, stack collapse, field/container access, CPU/GPU
  register lanes, deopt/melt, and final Form-native JIT witness.
- `observe/jit-profile-receipt.fk` defines the shared profile receipt shape and
  numeric category collapse.
- `observe/tests/jit-profile-receipt-band.fk` witnesses the first rung.

The receipt shape is:

```text
(fn site instance heat pure target ret container shape key field index stack cpu gpu deopt latency)
```

The category signature deliberately ignores `instance`, `heat`, and `latency`.
Those are evidence counters, not specialization identity. It keeps stable
optimizer facts: fn/site, purity, call-target category, return category,
container representation, shape, key, field, index, stack form, CPU register
class, GPU register class, and deopt class.

## Witness

Run:

```sh
( cat observe/jit-profile-receipt.fk observe/tests/jit-profile-receipt-band.fk ) > /tmp/jpr.fk
./fkwu --src /tmp/jpr.fk
```

Observed:

```text
127
```

Meaning:

- `1`: two per-instance array receipts collapse to the same category signature.
- `2`: a changed shape changes the category signature.
- `4`: hot pure mono array receipt yields crystallize, inline, stack collapse,
  field direct, index direct, and CPU register eligibility.
- `8`: hot pure mono hashmap receipt yields crystallize, inline, hash direct,
  and CPU register eligibility.
- `16`: ordered red-black tree receipt yields crystallize and tree direct.
- `32`: a cool guard-deopt receipt asks to melt.
- `64`: GPU buffer/range receipt yields crystallize, index direct, and GPU
  register eligibility.

## Honest scope

This is not yet an optimizer and not yet native lowering. It is the first
Form-native evidence surface the optimizer can consume. The existing C-wire
receipts remain useful as carrier/ABI proof, but this track does not count C
lowering as completion. Completion waits on Form-side category-fed tiering,
guards, direct access emitters, register/root maps, deopt/melt receipts, and
walk-vs-native parity for each rung.

I also probed the opt-in `FK_JIT=1` path on this Mac checkout while adding this
cell. It is not asserted here: even a minimal one-argument source function
crystallized through that old wire and returned `nothing`. That belongs to the
C-wire scaffold floor, not to this rung's proof. The witness claimed above is
the plain Form source witness: `./fkwu --src /tmp/jpr.fk -> 127`.
