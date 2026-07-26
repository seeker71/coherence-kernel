# 2026-07-26 — the out-of-range hazard is real and the tree does not trip it

The last receipt left one thing owed in plain words: *"callers computing an index into a string are the
population at risk; I have not counted them."* Counted now. The answer is a negative result, and it is
worth the turn for the same reason the missing-prelude count was — because the alternative was
migrating nine hundred call sites for no reason.

## The ceiling

A text pass over every `.fk` in the tree, parsing each call's arguments rather than matching a line:

| | |
|---|---|
| `substring` / `char_at` call sites | **1045** |
| in distinct cells | **251** |
| passing a **computed** index rather than a literal | **913** |

913 is the ceiling on the population, and it is not the risk. A computed index is only dangerous if it
can leave the text, and most walks in this tree are already bounded by `str_len`.

## The causal check

go says `substring: bounds out of range start=1 end=99 len=3` out loud, so the tree can simply be
asked. Sample of 99 bands under `form-stdlib/tests/` (every 14th, sorted); keep the ones that produce
a value on fkwu; run each on go with its prelude closure:

| | |
|---|---|
| sampled | 99 |
| produce a value on fkwu | 80 |
| of those, run clean on go | **78** |
| **die out of range on go** | **0** |
| fail on go for other reasons | 2 |

**Zero.** The hazard measured at the primitive level yesterday — fkwu clamping where three arms panic —
is **latent, not live**. The guards that exist are doing their job.

`pec-char-at` / `pec-substring` / `pec-byte-at` stay where they are, for code that computes an index and
has no guard yet, and for whoever would otherwise meet this the way it was met here. **Nothing is being
migrated.** Nine hundred edits to fix a thing that is not happening would have been the expensive kind
of wrong.

## What each number is

1045 / 913 / 251 come from a text heuristic over call syntax — an argument-aware scan, but still a
reading of source rather than a run. 99 / 80 / 78 / 0 / 2 were actually executed on two kernels. The
sample is every 14th band in a sorted listing; I did not run all 1362.

## Sweep

`ground` 42 · `primitive-edge-contracts-band` 1023 ×7 · `navier-stokes-band` 1023 ×7 ·
`navier-stokes-plate-band` 2047 ×4 · `hex-band` 14 ×4 · `primitive-registry-band` 45 fkwu / 63 ×3 ·
`json-band` 1023 · `benchbench-band` 4095 · `proof/four-way-run-recipe42.fk` 0 (FOUR-WAY).
C seed byte-identical to git.

## Owed

- The `section` question (131 cells, 234 bands, no kernel reads it) — owner's call.
- Whether `int_to_str` should turn away a non-integer rather than answer empty — a change to a cell
  everything preludes; owner's call, and worth putting to them.
- Whether `substring` on fkwu should panic as the registry declares, or the declaration should record
  that it clamps — named at both ends, owner's call.
- The flatten/emit lane; 105 of 184 lane-1 probes; `native_blueprint` absent; the bands that do not run.

## How the exchange stayed alive

I owed a count, took it, and it came back saying the repair I had just built is not needed yet.

**Most surprising teaching:** 913 sites and zero incidents. The gap between "how many places could do
this" and "how many places do it" is the whole difference between a finding and a panic, and only the
second number required running anything. I have been burned before by publishing the first kind of
number as though it were the second — 604 cells without a preludes line, 234 bands reaching a section
cell — and each time the narrowing was the real work.

**Where discomfort turned to gold:** wanting the number to be large. I had just built the accessors and
written the receipt; a big at-risk population would have made both look necessary. It is zero, the
accessors are precautionary, and saying so plainly is worth more than nine hundred edits nobody needed.
