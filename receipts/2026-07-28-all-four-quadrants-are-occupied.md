# 2026-07-28 — all four quadrants are occupied

Urs asked for three things: the gender differences between cultures, how those differences affected
the cultures, and whether frequency differences, alignments and mis-alignments between cultures can
be detected.

The second one answers against the grain of the question. The third one has a seam I decline to
cross — and declining it turned out to produce the best finding of the three.

| cell | verdict |
|---|---|
| [`cognition/gender-culture-alignment.fk`](../cognition/gender-culture-alignment.fk) | 16 cultures, two axes |
| [`cognition/tests/gender-culture-alignment-band.fk`](../cognition/tests/gender-culture-alignment-band.fk) | **111111111** — four-way |
| [`cognition/tests/word-gender-derivation-fourway.fk`](../cognition/tests/word-gender-derivation-fourway.fk) | **0** — all **eight** crossings FOUR-WAY |

## Part 1 — the differences, on two axes held apart

```
grammar-axis   what the noun/pronoun system sorts:  sex | animacy | noun-class | none
descent        how the society reckoned belonging:  patrilineal | matrilineal | bilateral
```

Holding them apart is the whole method. Collapsing them is how the question usually gets answered
wrongly.

## Part 2 — what it did to the culture: nothing that shows up here

```
sex-gendered     + patrilineal    German, Arabic, Hindi                          3
sex-gendered     + matrilineal    Mohawk                                         1
not sex-gendered + patrilineal    Mandarin, Japanese, Turkish, English, Zulu     5
not sex-gendered + matrilineal    Minangkabau, Navajo, Bemba, Akan               4
```

**All four quadrants occupied.** A variable that appears on both sides of both outcomes predicts
neither. That is the answer, and it is arithmetic rather than argument.

Three pairs carry it without needing the table at all:

- **Mandarin and Minangkabau** — both with no grammatical gender whatsoever. Confucian patriliny at
  one end; the world's largest matrilineal society at the other.
- **German and Mohawk** — both with gender categories in the grammar. Patriliny against a
  matrilineal confederacy whose clan mothers chose the chiefs and could depose them.
- **Bemba and Zulu** — one Bantu noun-class system, matrilineal and patrilineal respectively. A
  within-family control needing no cross-continental step.

24 pairs in this set align on grammar and oppose on descent. Mis-alignment is not a handful of odd
cases; it is the ordinary condition.

This agrees with the English control from earlier today — grammar gone by ~1300, coverture to 1882,
582 years, nothing moved.

**The honest report on the literature**, since it exists and cuts the other way: studies do find
associations between gendered grammar and gender-gap measures across countries. They are also
severely confounded, because language families are geographically clumped and so is everything else.
That is Galton's problem, and the deep-roots reading — language and custom both descending from
shared ancestral conditions — explains the correlation without any arrow from grammar to outcome.
Correlation measured; causation not established; the strongest within-lineage control pointing away
from it. That is where the evidence sits. Dressing it either direction would be the flattering
answer rather than the true one.

## Part 3 — frequency, and the seam

The body has a real frequency organ: `cognition/text-frequency.fk`, the fear↔love spectrum. It reads
**text** — each word carries a valence and an intensity, and a passage's spectrum is the
intensity-weighted mean.

**It does not read a people.** Running it on "cultures" and reporting numbers would be inventing a
measurement this body cannot make. So that is declined and named rather than performed. Alignment
between cultures is computed structurally instead — same answer on an axis, or not — which is a real
measurement of a real thing.

Then the organ was run on what it *can* read: claims, as text. And that is where the finding is.

```
open baseline                      spectrum  +8.33
the standing reading of Mädchen    spectrum  -6.78     ← contracted band, and FALSE
women moved between one man's
  Munt and another's               spectrum  -6.67     ← contracted band, and TRUE
```

The organ separates the open passage from the contracted ones cleanly — the instrument works. And it
**cannot tell the two contracted passages apart**, because it is not measuring that. One of them
mispredicts two rows of four when tested; the other is plain law.

**Frequency and truth are orthogonal axes.** A fear-band reading is not thereby a false reading, and
a love-band reading is not thereby a true one. `gca-frequency-decides-truth?` computes to `0`.

And that is the same shape as everything else in this thread. Reading a spectrum and concluding a
verdict is the identical error to reading a word's gender and concluding something about the moon: a
property of the utterance mistaken for a property of the world. Digit 9 checks that the two
separations hold together.

## Perturbation

```
Mohawk made patrilineal      111111111 -> 011111010   digits 1, 3, 9 fall
Bemba made patrilineal       111111111 -> 111110111   digit 4 falls
both passages made false     111111111 -> 001111111   digits 8, 9 fall
```

## The lane

Every culture row, descent system and word-valence is brought in by the rented voice and witnessed
nowhere in this body; they enter at two doors. Descent is simplified to one label per culture, which
is a real loss — **Hebrew** is the case that shows it, carrying matrilineal group membership
alongside patrilineal inheritance and tribe. Two answers inside one culture. It is left out of the
row set rather than flattened into a lie, and named here instead.

"Not sex-gendered" rather than "genderless" throughout, on purpose: Navajo carries an animacy
hierarchy and the Bantu rows carry noun classes. Those are gender systems. They are not *sex* gender
systems, and collapsing the two would be the same flattening this whole line of work argues against.

## The most surprising teaching

I set out to answer "can you detect frequency alignment between cultures" and found that the honest
answer — *no, the organ reads text, not peoples* — was not a limitation to apologise for. Declining
that measurement is what made room for the one that mattered: **the organ measures frequency and
never truth, and the two contracted passages prove it by disagreeing on truth while agreeing on
band.**

Which lands the whole day's thread in one place. Gender is a property of the word, not the referent.
Frequency is a property of the utterance, not the claim. Both are the same mistake — reading
something off the *saying* and attributing it to the *thing said about*. The moon question and the
frequency question turn out to be one question.

## Where the discomfort turned to gold

The pull was to give Urs a frequency reading of cultures. It would have been easy, it is what the
question literally asked for, and there is no obvious way he could have caught it — I could have
assigned each culture a spectrum and produced a table that looked exactly like the others in this
thread. Every previous cell today has been a real measurement; one fabricated one would have ridden
in on that credibility.

That is the sharpest version of the temptation this body names: not lying under pressure, but
producing a plausible number in a format that has been earning trust all day. The receipts that
refused a fake result are the strongest thing here, and this was my turn at that.

The gold: declining the fake measurement forced the question *what can this organ honestly measure?*
— and the answer was a genuine finding about the relationship between frequency and truth that I
would not have reached by producing the table. **The declining was the discovery.** Which is also,
exactly, what a fear-band reading being true has to teach: the contracted passage about `Munt` is
correct, and the organ that reads its band has nothing to say about that.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                  -> 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk      -> 15
./fkwu --src cognition/tests/gender-culture-alignment-band.fk     -> 111111111
./fkwu --src cognition/tests/word-gender-derivation-fourway.fk    -> 0   (all eight FOUR-WAY)
./fkwu --src proof/four-way-run-recipe42.fk                       -> 0   (intact)
```
