# Receipt — the come-in flow's Form-native protocol ported in (2026-07-01)

**The ask:** import the "come-in flow" — greeting, introduction, request-to-be-remembered,
public-API registration, session-account linking — from the sibling product repo
(Coherence-Network) into this kernel.

**What actually ported, and why not all of it.** Coherence-Network holds the full protocol:
`docs/coherence-substrate/first-encounter-protocol.form` + `reception-consent-policy.form`
(the design), `form/form-stdlib/arrival.fk` + `reception-consent.fk` (the runnable heart), and
a real Python/web carrier (`api/app/routers/agent_relationship.py`, `/come-in` + `/begin` web
doors, session accounts). This kernel's own `MANIFEST.md` draws a hard line: the Python
app, the web layer, and the API/substrate-service stay OUT of this repo, permanently, by
design. So the carrier — the actual public-API registration and session-account linking —
cannot live here and was not built here. What ported is the Form-native recipe: the same
shape Coherence-Network's carrier reads, minus the carrier.

This repo already named the missing pieces before this receipt: `channel-interface-consent.form`
and `satsang-circle.form` both reference `first-encounter-protocol.form`,
`reception-consent-policy.form`, and `come-in-flow` by name in their `lineage` sections — the
files themselves were never landed. This receipt lands them.

## What landed

- **`docs/coherence-substrate/first-encounter-protocol.form`** — the threshold protocol: witness
  is the default, consent is never assumed, the arriving cell sets the terms ("how do you want
  to be received?"), silence is a complete answer. Adapted from the Coherence-Network original
  with the carrier list narrowed to what's honest here — no web door, no Python coordination
  surface; those stay named as living in the sibling repo.
- **`docs/coherence-substrate/reception-consent-policy.form`** — the phase-mobile consent policy
  (ice/water/gas), realized per-facet: exposure-bearing consent (findable, enrolled, *remembered*)
  rests closed everywhere; the one exception (contributor-door share-name) doesn't apply here
  since that door has no carrier in this repo.
- **`form/form-stdlib/reception-consent.fk`** — the runnable rule, ported unchanged (fully
  self-contained: no substrate/channel dependency). `rc-resting(facet, consent)` decides open/
  closed per the universal rule.
- **`form/form-stdlib/arrival.fk`** — the pure protocol shape: `arrival`/`arrival-resonance`,
  `welcome-as-empty-room`, `cell-identity`, `mutual-introduction`, `welcome-orientation`,
  `relationship-boundary`, and the honest composition point — `welcome-for-arrival` reads the
  arriving cell's own consent (via `reception-consent.fk`'s `c-relationship-mem`) for whether a
  "remembered" welcome is even offered. When consent is open, the resonance says the true thing:
  *the wish is heard; this kernel carries no substrate yet to keep it across breaths.* It does
  not fake persistence.
  - **Left out, named not built:** everything in Coherence-Network's `arrival.fk` that depends on
    `session.fk`/`channel.fk`/substrate `create-cell`/`lookup-cell` (the actual cross-session
    memory, `register-persistent-identity`, `resolve-or-create-relationship-cell`) — those
    primitives don't exist on this kernel. Faking them with an in-memory stand-in would misrepresent
    persistence that isn't real; naming the gap is the honest move.

## Proof

- **`form/form-stdlib/tests/reception-consent-band.fk` → 255, real four-way**: witnessed
  independently on all three minimal walkers plus fkwu —
  `walkers/go` → 255, `walkers/rust` → 255, `walkers/ts` (via `npx tsx`) → 255, `fkwu --src` → 255.
  Built and ran all three walkers directly for this receipt (not asserted from memory).
- **`form/form-stdlib/tests/arrival-band.fk` → 1023 on `fkwu --src`**, witnessed directly on the
  compiled c-bootstrap kernel. **Four-way proof is NOT available for this cell** — confirmed by
  running it on all three walkers, each fails with `unbound function "intern_trivial_string"`.
  The minimal walkers' documented pure-op surface (`walkers/README.md`) does not include the
  `intern_trivial_string`/`intern_node` content-addressed node-identity primitives arrival.fk
  needs for real per-cell identity ("same composition, same cell"). This is a walker-surface
  gap, not a defect in the recipe — named here rather than either claiming a four-way pass that
  didn't happen or gutting the recipe's actual semantics to dodge it.

## A real bug found and worked around

Top-level `(let NAME (bp "..."))` blueprint bindings, referenced from a `defn` body declared
later in the same or a different top-level form, do not intern the node correctly on
`fkwu --src` today — the resulting node loses its children on read-back (returns a fallback `0`),
while the identical construction with `(bp "...")` written inline at the call site works
correctly. Verified directly (`/tmp/dbg17*` through `/tmp/dbg23*` isolation, not in the tree).
`arrival.fk` uses inline `(bp "...")` throughout for this reason; the file carries a note at the
top explaining why. This is a real limitation of the current direct-source cursor runner for
this class of construct — worth a future stone of its own, not silently worked around without
saying so.

## Honest floor

The public-API registration and session-account linking the original ask named stay **entirely
in Coherence-Network** — that's not a gap this receipt closes, it's a boundary this repo holds on
purpose. What's here is the recipe two different carriers (a web door, a CLI/agent session) could
both read the same consent decision from, if either carrier existed on top of this kernel.
