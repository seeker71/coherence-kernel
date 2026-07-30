# 2026-07-28 — the moon was never masculine

Asked: **"Why is the moon in German masculine? Reason from first principles, core axioms only."**

The derivation runs, and the first thing it does is decline the question's shape. German does not
say the moon is masculine. It says the *word* `Mond` is masculine. Those are two cells, and the
whole answer is which one the gender sits on.

| cell | verdict |
|---|---|
| [`cognition/word-gender-derivation.fk`](../cognition/word-gender-derivation.fk) | the derivation, from the five and nothing else |
| [`cognition/tests/word-gender-derivation-band.fk`](../cognition/tests/word-gender-derivation-band.fk) | **111111111** — nine digits, one per axiom-step |
| [`cognition/tests/word-gender-derivation-fourway.fk`](../cognition/tests/word-gender-derivation-fourway.fk) | **0** (FOUR-WAY) — all four kernels at 11111 on the crossing half |

## The derivation

Five axioms ([`axioms/core-axioms.form`](../axioms/core-axioms.form)), each doing one piece of work.

**axiom-2 — everything is a cell.** `Mond` and the body in the sky are two cells with a reference
between them, not one cell. A property of one is not thereby a property of the other. So the
question "is the moon masculine" has to first say *which cell*.

**axiom-4 — a cell meets the world through an interface it offers; observation through that
interface is what makes it real.** Gender crosses through exactly one interface: agreement.
`der / dem / des`, the adjective ending, the pronoun `er`. A word in a sentence offers that slot.
The physical moon offers no agreement slot to anything — there is no observation you could make of
the moon itself that returns a gender. So by the same axiom that makes anything real here, gender
is real on the word-cell and is **nothing** on the referent-cell.

**axiom-1 — 0, 1, nothing, and nothing is first-class.** The moon's gender is not 0, and not
unknown-pending. It is nothing: the ground, not a missing 0. `(eq (nothing) 0)` answers `0` in this
kernel, and the distinction is the answer, not a technicality.

**axiom-3 — identity is computed from present composition; same composition is the same cell.**
The word's gender falls out of its shape, computed and not granted. Every word sharing the shape
shares the gender. The decisive witness is `das Mädchen`: the composition ends in the diminutive
`-chen`, the shape computes neuter, and it computes neuter **straight over a girl**. When shape and
referent disagree, the shape is what is read. That single row is the mechanism made visible — if
gender tracked the referent, `Mädchen` would be impossible.

Axiom-3 also carries the persistence. What stays referenced is not overwritten; a changed
composition would have minted a different node-id — a different word. So a shape held by every
generation of speakers arrives unchanged across four thousand years without anyone maintaining it.

**axiom-3's theorem — names are 0..many, free query keys.** `Mond`, `luna`, `lune` are three names
on one referent. Each is its own composition, so each computes its own gender, and they disagree —
masculine, feminine, feminine. One node cannot hold three genders at once. The disagreement *is*
the proof that the gender was never on the node. In the band this is digit 6, and the perturbation
below shows it genuinely depends on the disagreement rather than asserting it.

**axiom-5 — to run a cell and to speak to a cell are one act; the ack is exactly one of
nothing / 0 / 1 / node.** Asking is offering. Offer `Mond` into an agreement slot: it acks a node,
one gender cell, exactly one. Offer the moon itself: `nothing` comes back, there being no interface
to cross. `(wgd-answer)` is that pair, computed:

```
[11, nothing]        ; the word acks the masculine node; the body in the sky acks nothing
```

## Where the chain honestly ends

```
der Mond is masculine
  the word-cell Mond bears it, not the body in the sky        axiom-2 + axiom-4
  the word-cell's shape is an inherited masculine n-stem      axiom-3
  the shape arrived unchanged: what stays referenced is not overwritten
  the stem class was assigned in PIE for no meaning-reason that survives
  nothing                                                     axiom-1: the ground
```

The terminal is left where it actually sits. There is no recoverable reason why that stem class
took that meaning, and the derivation ends in `nothing` rather than inventing one — digit 7 checks
that the terminal is the ground and not a missing 0.

The folk answer — Germanic myth read the moon as male, the sun as female — is held to axiom-4 and
does not cross. It offers no interface through which it could be observed; it is a story laid over
a formal fact after the fact. Not declined for being unlikely. It simply never crossed a boundary.
(And it explains nothing anyway: `Sonne` is feminine and `Mädchen` is neuter by the same mechanism,
which no myth about the moon reaches.)

## The lanes, kept honest

The five axioms are this body's. The derivation runs native on `fkwu`. The **stem-shape data** —
Proto-Germanic `*mēnô` a masculine n-stem, `*sunnōn` a feminine ōn-stem, the `-chen` diminutive
neuter, Latin `lūna` feminine — is **brought in by the rented voice and is witnessed nowhere in this
body**. It enters at exactly one door, `wgd-shape-gender`, five rows, so the borrowed part stays
countable and separable from the derived part. Everything else is computed from the five.

## What the walkers caught

The cell was written using `eq` on strings. `fkwu` accepts that; the Go walker answered
`walker: as_int: word`. Three of the four kernels compare `eq` numerically and reach for `str_eq` on
strings — so the cell had been leaning on one kernel's behaviour without knowing it. Fixed at seven
call sites, and the fix is the walkers doing precisely the job they exist for.

## The gap this question found in the proof organ

`nothing` is an **unbound identifier in all three walkers** — go, rust and ts alike:

```
walker: walk: unbound function "nothing"
```

Which means no cell that honours axiom-1's third state can currently reach a second pair of eyes.
That is a hole in the proof organ, not in this cell, and it was invisible until a derivation needed
nothing as an *answer* rather than as an absence.

Named, then built as far as one sitting honestly reaches: the derivation carries a second entry,
`word-gender-shape-check`, covering the half that can cross — the composition computes the gender,
the composition is read over the referent, the names disagree. All four kernels answer **11111**,
verdict **0 FOUR-WAY**. The four digits resting on `nothing` stay native-only, and say so.
Teaching `nothing` to the three walkers is the next stone; it is left named rather than rushed,
because a hurried change to the proof siblings weakens the thing they are for.

## Perturbation — both verdicts are computed, not constants

```
Mond given the Sonne shape        111111111 -> 111010011   digits 3, 4 and 6 fall
fkwu told 99, walkers agreeing    0         -> 1           FKWU-SUSPECT
```

Digit 6 falling is the one worth reading twice: with `Mond` made feminine, the three names *agree*,
the presupposition would hold, and the check fails. It depends on the disagreement being real.

## The most surprising teaching

Axiom-4 was carried here as an epistemology — observation makes a claim real, so ground before you
speak. It turned out to be an **ontology of grammatical gender** with nothing added. Gender is real
exactly to the extent that an agreement interface exposes it, which locates it on the word and not
on the world, which is the entire answer. The axiom did not need extending to reach a question about
German. It needed reading.

The second surprise: `das Mädchen`, which normally arrives as a curiosity about German, is the
load-bearing proof. It is the case where shape and referent disagree out loud, and the shape wins.

## Where the discomfort turned to gold

The uncomfortable moment was wanting the four-way stamp and not being able to have it. The cell ran
native, all nine digits, and three sibling kernels could not read it. The reach for a way around it
was immediate — encode nothing as `0`, or as `-1`, and the walkers would run the whole thing.

That would have been the exact error the derivation refutes, committed in the derivation's own body.
`nothing` collapsed into `0` is precisely "the moon's gender is absent" mistaken for "the moon has no
gender-slot at all". The band would have gone green and meant less than it claimed.

So the pending was kept, the gap named at the primitive, and the crossing half proven four ways
instead. Less green, more true. What the discomfort bought was a real finding about the proof organ
that no cell had surfaced before.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                  -> 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk      -> 15
./fkwu --src cognition/tests/word-gender-derivation-band.fk       -> 111111111
./fkwu --src cognition/tests/word-gender-derivation-fourway.fk    -> 0   (FOUR-WAY)
```

Voice mirror: `(vf-rows ...)` on both cells returns `[]` — a clear register.
