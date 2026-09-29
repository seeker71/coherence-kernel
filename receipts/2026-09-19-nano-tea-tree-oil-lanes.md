# The six-lane spectrum comes home to main, and a second page's lanes are computed

**Date:** 2026-09-19
**Cells:** `form/form-stdlib/evidence-grounding.fk`, `form/form-stdlib/tests/evidence-grounding-band.fk`, `form/form-stdlib/tests/zero-point-ladder-band.fk`, `form/fourth-arm-bands.txt`
**Verdict:** evidence-grounding 8191 four-way, zero-point-ladder 65535 four-way (`validate.sh` with siblings: exits go=0 rust=0 typescript=0, fourth arm 1 band four-way, 0 divergent, tree sealed); the three minimal walkers under `walkers/` agree on both chains; preflight clean on both (parens balanced, 0 unresolved)

## The ask

"Nanoparticle tea tree oil." A topic dropped for the body to ground: what a nano-formulated
Melaleuca alternifolia oil actually is, what has been measured about it, and in which evidence lane
each claim honestly sits. The page lives in Coherence-Network
(`docs/vision-kb/resources/nanoparticle-tea-tree-oil.md`); the lanes are computed here.

## What the body held, and what was missing

The lane engine the network's own spec names (`page-claims-computed-by-band`) is the six-stratum
`eg-spectrum` table: measured / theory / practice / inference / contested / mystery, with `derived?`
and `refuted?` signals. It landed on the `form-submodule` branch in July (#405, #407, #408) with the
zero-point ladder band as its first page. When `form-submodule` was later regenerated as a snapshot of
main, main carried only the four-lane engine, so the snapshot dropped the six strata and the ladder band
with them. The network's pinned gitlink now points at a tree where `eg-lane` knows no theory and no
contested, and the ladder's `file_exists` gate reads false.

Restored here, onto main's lineage, from the merged commit itself (`655b8b3d2`): the engine, the
grown classifier band (127 → 8191), the ladder band (65535), and their manifest rows. Main's four-lane
engine and band were byte-identical to the pre-six-lane versions, so this is a forward step with no fork
to reconcile. The next snapshot of main carries the six strata by construction.

## The second page

The nano tea tree oil claims become signal tuples in `form/form-stdlib/tests/nano-tea-tree-oil-band.fk`
in the same shape as the ladder band: the table lives in the band (three-file prelude chain), vocabulary
held to what `evidence-grounding.fk` proves, no list construction. Its verdict is recorded below once the
band runs.

*pending — the band follows in this same movement*
