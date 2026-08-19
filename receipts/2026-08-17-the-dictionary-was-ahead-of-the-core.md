# The dictionary was ahead of the core — witnessed

Asked by Urs on 2026-08-17: read the core teachings into the natural-language
space and help update the core word dictionary. Then, mid-work: add a second
expansion layer reaching from the 64 into Gene Keys, Human Design, Astrology,
Physics, Chemistry, Math, Geometry, Biology, Organic Intelligence, Somatic
Resonance, Embodied Flow.

## The measurement came first, and it changed what "update" had to mean

`form/form-stdlib/core-lexicon.fk` holds 64 words in eight families, each
defined from that vocabulary alone plus twelve declared glue words, with
`cl-closed?` walking every definition to prove it. Rather than decide by eye
which teachings the dictionary could carry, its own instrument
(`cl-bad-in-def`) was turned on this session's teachings:

| sentence | tokens outside the 64 |
|---|---|
| "nothing is not **yet**" | **0** |
| "the self that know be other and the thing say same" | **0** |
| "none say is not all say 0" | **0** |
| "a **name** is not what make a thing be" | 1 |
| "a thing is **done**" | 1 |
| "a self **give** a thing to other" | 1 |
| "a self **hold** a thing and not give" | 2 |

## The most surprising teaching

**The dictionary already said NOT-YET.**

`yet` has sat at family 4, index 5 the entire time — *"yet is when a thing is
not now and after it is"*. That is precisely the third reading of the
nothing-ground that `control/offer-ack-core.fk` lacked all of this morning and
had to be taught as a new theorem, across two homes, with a fifteen-claim
band.

Same for absence-is-not-a-verdict: *"none say is not all say 0"* is fully
grounded in the 64. The refusal that needed `oac-census` to become observable
in the control core, the natural-language space could state in seven words.

The lexicon was not behind the core. It was **ahead of it, and nobody had
asked it.** I spent the morning deriving a theorem that was already sitting in
the dictionary as an ordinary word. Corpus row 1006 names that
*inert-knowledge*.

## What it could not say, and why that is the same wound again

Family 2 (act) holds: do, be, exist, have, know, want, say, make. Eight acts,
and every one brings something **into** being. There is no act of ending and
no act of releasing in the whole vocabulary.

That is exactly the gap Urs named in the session-recording cell hours earlier
— a cell that could say "removable" and not "finished", built from a fear that
had removed a concept (corpus row 1004, *hypocognition*). Finding it again in
a dictionary written earlier and by a different hand says the absence is
**structural rather than personal**: a vocabulary of making, with no
vocabulary of completing.

## Why the 64 stay 64

`core-lexicon.fk`'s header draws a direction: a dictionary that needs a 65th
word *to explain its 64* has grounded nothing. That rule is about what the 64
**depend on**. A word the 64 themselves define is the opposite motion — the
dictionary paying out rather than borrowing. Every defining body was measured
closed over the 64 plus glue **before** either new cell was written.

So the parent set is untouched: `cl-count` 64, `cl-closed?` 1.

- **Layer 1** (`core-lexicon-derived.fk`): `name`, `done`, `give` at depth 1;
  `hold` at depth 2, because holding is only nameable once releasing is.
- **Layer 2** (`core-lexicon-domains.fk`): 36 words across all eleven domains,
  on the same single depth ladder — 28 at depth 1, 6 at depth 2, 2 at depth 3.

## What the ladder revealed, which was not planned

The deepest words are not the most technical ones. `bond` and `angle` sit at
depth 3 — and **`bond` is deep because holding is deep, which is deep because
giving had to be named first.** Chemistry's bond rests on the escrow teaching
that arrived this afternoon as a correction about unpushed work.

Likewise `gift` (Gene Keys) reaches `give`: the gift is not a thing held but a
self given, and the ladder derived that rather than being told it. And
`siddhi` sits at depth 1 — the highest frequency in Gene Keys is sayable with
the plainest words this body has, borrowing nothing.

## Where discomfort turned to gold

**A check that could not fail, and I nearly certified it.**

The grounding check is the whole worth of layer 2 — it is what makes this an
expansion rather than an import. I perturbed it by redefining `siddhi` with
two words this body cannot say. The band returned a confident **4095**,
unchanged.

The bug was mine. `cldx-token-depth` returns `-1` for an unreachable token,
and I folded that marker into a running maximum starting at `0`. Since `-1` is
never greater than `0`, the poison was silently discarded on every row. The
gate was open the entire time and reported itself shut.

Unreachable tokens are now **counted, never maxed**, and a row carrying any
has no honest depth at all. Re-perturbed: `4095 → 4071`, dropping bits 8 and
16.

What bought this was running the perturbation instead of asserting it. I had
already written the words "perturbation-verified" for the earlier bands today,
truthfully. Here the same phrase would have been false, and nothing in the
number would have shown it — the identical failure shape as this morning's
`2097151` that was a fold over `nothing`, arriving a third time in one day and
still not recognisable from the output alone.

A check that cannot fail is worth **less** than no check, because it also
reports success. Corpus row 1007 names that *vacuity*.

## The honest edges

- These are **one** reading of each domain's word, sayable in the 64. They are
  not the domains' authoritative definitions, and no claim is made that a
  practitioner would call them complete. What is claimed is what is checked:
  each is grounded, none circular, every depth earned.
- The words are **not yet wired into the NL query lane**
  (`nl-neutral-dictionary-query.fk`, `core-word-ack.fk`). They are a reachable,
  checked layer beside the dictionary, not yet answerable through the eight
  ask-lanes. That is the next step and it is not done.
- Both bands are witnessed on fkwu, preflighted and perturbed. They are not
  claimed four-way.

## Proof

```sh
./fkwu form/form-stdlib/tests/core-lexicon-domains-band.fk
```

| band | verdict | exit |
|---|---|---|
| `core-lexicon-derived-band.fk` | 4095 | 0 |
| `core-lexicon-domains-band.fk` | 4095 | 0 |
| `homecoming-distillation-corpus-band.fk` | 32767 | 0 |

Perturbation-verified both: derived `4095 → 3871` when `hold` is declared the
same depth as `give`; domains `4095 → 4071` when a body reaches ungrounded
jargon.

A `[shadowed-call]` warning also surfaced in the first draft of layer 1 — I
had named a parameter `head`, and in call position the primitive wins. It
computed correctly by luck, which is what makes it a landmine rather than a
bug. Renamed to `headword`. That is the same seam the kenosis sentinel was
written to guard **this morning**: a name winning over the thing, inside the
cell about names.

## The frontier question

**What names what a body already knows but has never been asked?**

The word is **inert-knowledge** — Whitehead's term for knowledge a system
holds without ever activating it. The 64 held `yet` and `none say`, and the
control core was taught both from scratch hours earlier. 0-hit fresh. Corpus
row 1006, with *vacuity* at 1007.

401 rows, 401 admissible, max id 1007, `hdc-dup-mid-rows` 0 — probed in one
cell reading exit 0 before the numbers were written down.
