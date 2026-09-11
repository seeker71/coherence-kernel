# The slot spent before asking

2026-09-11, night, M4 Max, Hati Suci. The last receipt left float-mint one claim short, with its
cause read to the line. This piece carries that claim.

## Carried

- **fkwu interns a float the field already holds without boxing it again** (fcd1c6e3).
  `intern_trivial_float` dedups by canonical bits. With the field on, which is every run whose
  shared-memory store attaches, it boxed the float first and then asked the field, so each call spent
  a pool slot even when it returned the node the field already held. `fk_field_find_float` asks the
  field first, by value, and the box happens only on a miss. The same node comes back either way, so
  only readers of the pool count can tell: two interns of 2.5 now cost 0 slots where they cost 2,
  float-mint reads 63 where it read 47, and field-band (255) and kernel-census-band (2047), the other
  two bands that read the pool, stay where they stood. On the rebuilt ./fkwu freshness reads 31 and
  the capture probes from 9f99bc48 keep [9, 6000] and [6, 8, 2, 7, 6000].
- **Row 1449, prespend** (d84ca19f): to pay for a thing before asking whether it is already held.

Witnessed on d84ca19f, fresh: float-mint reads 63 on its fkwu lane, freshness 31, corpus 32767,
drift pass 8191 of 8191.

## Still open

The list in receipts/2026-09-11-the-four-names-no-registry-held.md stands, less float-mint's c4.

## Surprise, and where the discomfort went

I expected the probe to read one slot for two interns, the first paying and the second finding it.
It read none: the field already held 2.5 before the first call. The band's own claim, "at most one",
was written by someone who knew the pool does not start empty. My expectation was the thing out of
step, not the kernel. The gold: when a measurement lands under the number I predicted, state the
measured one and let the claim that was already right stand.

Frontier word, row 1449: **prespend**, to pay for a thing before asking whether it is already held.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
