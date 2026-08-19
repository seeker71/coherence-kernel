# Core lexicon vitality: the word moves only when its ground improves

**Date:** 2026-08-13
**Status:** witnessed four-way
**Band:** `form/form-stdlib/tests/core-lexicon-vitality-band.fk` → **2047**

## The movement

The closed 64-word dictionary remains authoritative and unchanged. The new
`ingest/core-lexicon-vitality.fk` is an overlay: it accepts a caller-observed
before/after profile, acknowledges the offered successor through the existing
satsang terminal kinds, and serves the successor only when the evidence earns
it. Every receipt retains the original, so rollback is a read rather than a
reconstruction.

Admission requires all of these at once:

- vitality rises;
- trust rises;
- sovereignty does not fall;
- coverage rises;
- proof count rises;
- residual count does not rise.

An invalid or worsening offer is declined with `0`. A valid but unmeasured
offer rests as `nothing`. An earned offer is admitted with `1`. Zero candidates
or competing candidates leave the original word served; ambiguity never elects
a winner. The tended surface is still exactly 64 core words.

The profile numbers and source label are caller-offered observations. This cell
checks their shape and compares them; it does not infer that one word is
intrinsically more vital than another, and it does not yet verify the named
source. The band’s word pairs are mechanics fixtures, not semantic rankings.

## Witness

The repository bootstrap was rebuilt before belief. The live freshness canary
returns **31**, the value encoded by the current five-reading band. Several
current-facing instructions still said **15** after later history carried the
older text forward, so those instructions and the source-runner admission mocks
were brought back to 31; historical receipts and historical examples were left
untouched.

The vitality band then passed:

```text
preflight: balanced; 0 errors; 0 warnings; 0 unresolved
repo fkwu source runner: 2047, exit 0
Go + Rust + TypeScript + emitted fkwu: 2047
focused validator: 1 four-way, 1 ok, 0 divergent
```

Neighboring witnesses remained whole:

```text
core-lexicon-band                 262143
satsang-transmute-band               127
source-runner-admission-band     2097151
binary-freshness-band                  31
```

The first draft of the band did not close. `observe/tree-heal.fk` tested its
candidate repair and refused it, reverting byte-for-byte; the missing closer
was then found manually and re-observed. A second preflight exposed that a
multi-line prelude declaration hid dependencies from the source door. Making
the declaration one complete line brought the chain to zero unresolved calls.
Finally, the first sibling run was honestly only three-way: the test lived
outside the canonical fourth-arm lane. Moving the band into
`form/form-stdlib/tests/` and registering it made the fourth witness real.

## What the work taught

The surprising teaching was that sovereignty is not a score to maximize here;
it is a floor that may never be spent to buy apparent vitality. Trust and
vitality must rise, while sovereignty holds the boundary from underneath.

The discomfort was the sequence of almost-green results: a balanced-looking
band with a missing closer, a clean-looking source with invisible preludes, and
a three-way check beside a built fourth kernel. It turned to gold when each
near-pass became a smaller interface: one complete prelude line, one canonical
proof location, one explicit registry row, and no claim larger than the witness.

## Edges

- `form/form-stdlib/core-lexicon.fk` names the overlay while remaining the
  untouched dictionary.
- `ingest/satsang-transmute.fk` names the composition that gives the overlay its
  hold / decline / admit acknowledgement kinds.
- `form/fourth-arm-bands.txt` registers the focused band at verdict 2047.
