# Situated lens conclusion — observed relation, witnessed

Date: 2026-07-26

## The named gap

`core-word-inquiry.fk` made timeless identity invariant and situated identity
dependent on time plus the complete lens. It deliberately stopped before
claiming that a lens changes truth. The next enquiry asked what witnessed
relation could let point, direction, intensity, and focus alter a conclusion
rather than only its node address.

The attempt is
`form/form-stdlib/situated-lens-conclusion.fk`.

## The relation

An observer offers a sparse field of rows:

```
[time, [point, direction, intensity, focus], candidate, push, source-node]
```

Time and all four lens coordinates select one exact row. The row carries both
the candidate conclusion and its measured or analytically supplied push. The
cell does not derive either from the lens ordinals.

The crossing law is attributed to `observe/jacobian-lens.fk`:

```
push > margin   -> candidate may replace the current conclusion
push == margin  -> tie; current conclusion remains
push < margin   -> current conclusion remains
```

Exactly one valid row must match. No row returns `nothing`; duplicate rows at
the same coordinate also return `nothing` because choosing between conflicting
receipts would fabricate priority.

The selected row’s source node remains in the outcome. The relation therefore
answers not only “what changed?” but “which observation was allowed to change
it?”

## Timeless and situated

- Timeless settlement carries the proposition and inner-knowing bit but ignores
  time and lens. Its node and conclusion remain invariant when both change.
- Situated settlement constructs the contextual node and selects evidence by
  time and the whole lens. The witness changes time, point, direction,
  intensity, and focus independently and observes a different conclusion at
  every coordinate.
- Exclusive scope and inner decline still settle to `0` before lens evidence
  can override them.

## Honest floor

- The ordinals do not claim physical units, mental states, or universal
  semantics.
- The observation field is caller-supplied and source-attributed. This build
  proves deterministic selection and margin crossing, not that the supplied
  evidence is true.
- A source node is traceability, not proof of semantic truth.
- Missing and ambiguous relations stay `nothing`; they are not silently
  converted to the current conclusion.
- The relation is synchronous Form data. It does not yet learn a lens field or
  actuate an external model.

## Witness

Direct source:

```
./fkwu --src form/form-stdlib/tests/situated-lens-conclusion-band.fk
-> 2097151

./fkwu --src form/form-stdlib/situated-lens-conclusion-actual-witness.fk
-> 7
```

Four-way:

```
cd form
./validate.sh form-stdlib/tests/situated-lens-conclusion-band.fk
-> 2097151
-> 1 ok, 0 divergent
```

The existing checkout notices for same-arity compatibility aliases remain
classified by `validate_fkwu_native_surface.py`; no new native operation or C
surface was added.
