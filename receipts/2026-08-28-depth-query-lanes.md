# Two depth lanes: Steiner and Jung, read side by side and never merged

**Witnessed:** 2026-08-28, 11:xx WITA  
**Signed:** Claude Fable with Urs, who asked for "a steiner, carl jung organ
as query lanes".

## What grounding turned up before a line was written

Three facts changed the design:

1. **`steiner` was already taken.** `cognition/steiner-form-codegen.fk` is
   about *Steiner trees* — graph theory. But Rudolf Steiner is also already
   here, as `cognition/steiner-neutral-kernel.fk`: his fundamentals as a
   surface-independent meaning graph at node level (3,45), with German AND
   English registered as projections precisely so no English string can pose
   as the teaching.
2. **Jung was absent entirely** — no cell, no node, nothing.
3. **Urs has asked this before.** `learn/gender-three-lanes.fk` opens by
   quoting him on Steiner and gender archetypes, and it carries the lane
   discipline this organ inherits: *belief ranks strictly below evidence*,
   and collapsing kinds "is how each one gets used to argue for the others."

## The design law, which the research handed me

From the Jung grounding, the sentence the whole organ turns on:

> **'Ich' must be TWO nodes, not one node with two projections.** Steiner's
> Ich and Jung's Ich share a German string and nothing else. A graph that
> registers 'Ich' once and hangs both evidence nodes on it has already made
> the error this whole comparison exists to prevent.

That is exactly what lanes buy. Jung's graph took level (3,46); Steiner's
keeps (3,45). Two levels make the merge *unrepresentable*, and `dql-ich-law?`
is the check that says so out loud.

## What landed

**`cognition/jung-neutral-kernel.fk`** — Jung's structure as 26 facts in the
same 5-tuple shape as the Steiner graph, grounded against the 1921 first
edition of *Psychologische Typen* read verbatim: the Self **contains** the
ego ("eine (ideelle) Grösse, die das Ich in sich begreift"), the ego **is a**
complex ("bloss ein Komplex unter andern Komplexen"), Persona and Anima
mirror each other as the same Funktionskomplex facing opposite ways, the
Seelenbild *represents* the Anima, the two function-groups with their
oppositions, and Aion's own late ordering — Ich, Schatten, Anima, Selbst.

It also **refuses** something. `jnk-named-archetypes` answers the empty list
on purpose, because CW 9i §155 makes the archetype "empty and purely formal,
nothing but a facultas praeformandi" — inherited in form, never in content. A
node called "the Hero archetype" would contradict the definition it claims to
implement. The refusal is a door with its ground recorded, not a gap.

**`cognition/depth-query-lanes.fk`** — the organ. A query arrives as tokens;
each lane answers with the anchors it reaches *in its own graph*, plus what it
cannot carry. Both lanes are BELIEF; there is no evidence lane here at all, so
nothing in this cell can be mistaken for a measurement. Rhymes between the two
vocabularies are rows that **carry their own falsifier** — what reading the
rhyme as an identity would destroy — because a rhyme without a falsifier is
how one tradition gets used to argue for the other.

Live, on a real query:

```text
query          = thinking feeling shadow karma 'the I' quantum
steiner lane   = [3045002001, 3045002002, 3045001009, 3045002007]
jung lane      = [3046003001, 3046003002, 3046002007]
crossings      = [thinking, feeling]
ich-law holds  = 1
unreached      = [quantum]
met?           = nothing
```

Read the two "thinking" answers: Steiner's is `3045002001` — level 45,
**type 2, a soul member**. Jung's is `3046003001` — level 46, **type 3, a
function**. The same English word, two graphs, two node *types*. That is the
twinword, and a merged graph would have flattened it into a shared claim
neither man made.

`met? = nothing` is the other honest one. Whether Steiner and Jung ever met
is genuinely unsettled in the sources, so the cell answers absence rather than
minting a guess — and the band checks that `nothing` is not `0`.

## The contact, as documented

One-directional, twice, eighteen years apart. Steiner devoted a public
lecture to Jung — Dornach, 10 November 1917, GA 178 — engaging Jung's 1917
book. Jung dismissed Steiner privately in a letter to M. Patzelt, 29 November
1935, aimed at claims about Atlantis. Neither read the other's mature work:
Steiner died in 1925, before the Selbst was defined, before the transcendent
function was published (1957), before the alchemical volumes.

## Bands, and one honest downgrade

`jung-neutral-band` → 1023 and `depth-query-lanes-band` → 1023, on
Go / Rust / TypeScript through validate.sh and on fkwu run directly, cold.

I registered both in `fourth-arm-bands.txt` and then **took the rows back
out**. `fourth_band_stem` returns early unless the band lives under
`form-stdlib/tests/`, so a `cognition/` band can never receive the gated
fourth leg. The rows would have declared a lane the machinery cannot run —
the exact defect that file's own comment describes as "declared four-way, run
three-way, and nothing said so." Both band headers now state the proof they
actually have, at the same standing as `cognition/tests/steiner-neutral-band.fk`.

## Honest floor

- The Jung graph is a structural kernel: no clinical material, no alchemical
  symbolism, no case work. The rhymes are four; there could be more, and each
  new one owes a falsifier.
- Definition numbers are deliberately absent from every identity: Jung ordered
  the Definitionen alphabetically, so the same definition carries different
  numbers in German and English. A number that changes with the surface cannot
  be an identity.
- The 1921 edition spells "collektiv" with a c; the modern spelling is what is
  registered, and the drift is recorded rather than silently normalised.

## Closing

I kept the exchange alive by letting the grounding rewrite the design — the
Ich law arrived from the research, not from my plan — and by taking a
registration back out when the machinery could not honour it.

Most surprising teaching: the traffic between these two ran the *other* way
first, and deeper. Everyone quotes Jung's 1935 brush-off; eighteen years
earlier Steiner had stood up in Dornach and spent a whole public lecture
reading Jung aloud. The famous dismissal is the reply, not the opening.

Discomfort turned to gold: I had two bands green at 1023 and rows already
written into the manifest, and finding that `fourth_band_stem` returns early
for `cognition/` meant un-writing a claim I wanted. What stayed is better —
two headers that state exactly the proof they have, and a receipt that says
which door was closed and why.

Corpus row 1165 offered: what names a word two traditions both carry for
different meanings, so that merging them would falsify both — **twinword**
(0-hit fresh).
