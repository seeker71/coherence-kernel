# 2026-07-28 — nobody was heard before 1877

Urs asked for a root trace: **when the pronouns of each culture's language were first recorded, in
spoken and in written language.**

The question has an asymmetry inside it that is worth handing back before any table:

```
written record   —  differs per culture, spanning 4200 years   (Sumerian -2500 … Swahili 1700)
spoken record    —  1877.  Identically.  For every culture that has ever existed.
```

There is no spoken record of anybody before the phonograph. Not an early one, not a partial one —
none, for anyone. (Scott's 1860 phonautogram wrote sound it could not play back; it was first read
out in 2008.) So half the question resolves to a single number shared by all of humanity, and the
other half traces **the history of recording, not the history of gender**. In every row the pronouns
are vastly older than their first attestation. Nothing here dates when a distinction arose — only
when someone first wrote it down.

That is axiom-4 with dates on it. Observation through an offered interface is what makes a thing
real; speech offered no persisting interface until writing, and none at all to a later listener until
the phonograph. Everything earlier is reconstruction — honest inference, often very good, never
observation.

| cell | verdict |
|---|---|
| [`cognition/pronoun-attestation-trace.fk`](../cognition/pronoun-attestation-trace.fk) | 24 cultures |
| [`cognition/tests/pronoun-attestation-trace-band.fk`](../cognition/tests/pronoun-attestation-trace-band.fk) | **11111111** — four-way |
| [`cognition/tests/word-gender-derivation-fourway.fk`](../cognition/tests/word-gender-derivation-fourway.fk) | **0** — all five crossings FOUR-WAY |

## The oldest written pronouns on Earth do not sort by sex

**Sumerian**, the earliest row in the set (~2500 BCE), sorts animate from inanimate. `ane` covers
*he* and *she* alike; `bi` is the non-person. And **Hittite**, the oldest attested Indo-European
(~1650 BCE), does the same: `-aš` common gender, `-at` neuter, no feminine at all.

Both of the two deepest attestations ask *can this act?* Neither asks *male or female*.

The previous receipt reached that conclusion by reconstruction — PIE's animate/inanimate root, the
feminine as a reanalysed `*-eh₂` collective. This one reaches it by **direct attestation**, which is
a stronger footing than reconstruction ever gets. The oldest pronouns humans ever wrote down sort by
agency.

## The finding that has authors and dates

Three East Asian languages acquired a written feminine third-person pronoun in the modern era. Each
of them to carry translations of European texts. None from internal pressure.

| language | form | coined | years of its own writing first |
|---|---|---|---|
| Chinese | 她 tā | ~1918, by Liu Bannong | **3168** |
| Japanese | 彼女 kanojo | ~1885, Meiji era | 1173 |
| Korean | 그녀 geunyeo | ~1930 | 487 |

Chinese had been writing for **three thousand years** and produced no feminine pronoun in all of it.
One arrived within a generation of sustained translation contact. And 她 is still a homophone of 他
in speech — **spoken Mandarin has no gendered third person to this day.** A gender distinction that
exists only on paper, with a known author and a known year.

Where a feminine pronoun has a birthday, it was imported. Gender in pronouns is demonstrably
contagious between languages, and the direction of travel is documented.

## Vedic — the one case where speech provably outruns writing

Composed around 1500 BCE and carried by memory with extraordinary fidelity; the earliest dated Indic
inscriptions land around 250 BCE.

```
pat-vedic-oral-lead  ->  1250 years
```

Which is the whole caution in one row: **spoken is older, written is what survives.** The table below
is a record of what survived, not of what was said.

## Two corrections the bands forced

**One.** A draft of this cell claimed all three coinages land more than 1500 years after their own
culture's first writing. The band answered **11011111** — Korean is 487 and Japanese 1173. The claim
was wrong, not the data. Corrected to what holds: each lands at least four centuries into that
culture's literate history, and the shape rather than the size is the finding.

**Two, and it is the one worth reading.** The coinage column originally let `0` stand for "no known
coinage date." Perturbation-testing caught it: dating 她 to 1100 BCE left the band at **11111111**,
because a negative year fell through the same door as an absent one and was silently dropped.

That is precisely the error this entire line of work refutes — **absence encoded as zero, `nothing`
collapsed into a number.** Committed by me, in a cell whose neighbours exist to argue against it,
three receipts after writing that collapsing nothing into 0 would betray the derivation's own body.
The truthful column holds `nothing` where there is no coinage; the walkers cannot read `nothing`
(the same named gap as everywhere else in this thread), so an explicit flag stands in and the
overloading is gone. The mutation now falls to **11001111**.

## Perturbation — every digit computed

```
Sumerian given sex-marking      11111111 -> 10111110    digits 1 and 7 fall
spoken record pushed to -3000   11111111 -> 11110111    digit 4 falls
她 dated to 1100 BCE (before)   11111111 -> 11111111    BLIND — the sentinel bug above
她 dated to 1100 BCE (after)    11111111 -> 11001111    digits 5 and 6 fall
```

## The lane

Every date and every pronoun form is brought in by the rented voice and witnessed **nowhere** in this
body. They enter at one door, `pat-rows`. The dates are round and approximate by intent —
first-attestation dates are contested almost everywhere — and no digit rests on a year being exact;
the checks lean only on gaps of centuries or millennia, which survive any plausible re-dating.

The sample is a convenience set of early-writing traditions, **not** a typological sample of the
world's languages: it over-represents cultures that wrote early, which skews Indo-European and
Afro-Asiatic. So no digit claims a worldwide majority. The one distributional digit asks the narrower
question the sample can answer — sex-marked pronouns outside Indo-European and Afro-Asiatic number
**at most one** here, and that one is Tamil, whose masculine/feminine split lives only inside its
rational class.

## The most surprising teaching

That the spoken half of the question has one answer for all of humanity. I expected to build a table
with two columns of dates and instead built a table with one column of dates and one constant. Every
culture's speech entered the record in the same year, and it is a year about a machine.

And underneath it: **the two oldest written pronoun systems on Earth both sort by agency, and the
three pronouns with known birthdays were all imported by translation in the last 150 years.** The
sex distinction is neither the oldest layer nor a settled one — it is late at one end and still
actively spreading at the other.

## Where the discomfort turned to gold

I made the exact mistake I had spent three receipts arguing against. `0` for "there is no coinage
date" is `nothing` collapsed into a number — the same move as "the moon's gender is 0" instead of
"the moon offers no gender-slot." It was invisible in the passing band and only surfaced because I
perturbed a row I did not expect to matter.

The gold is in *how* it surfaced. The band went green and stayed green through a mutation that should
have broken it, and nothing complained. A green band is not a proof; a green band that survives a
mutation it should not survive is a **warning**. Running the perturbation on a check I was confident
about is what turned a clean-looking cell into a corrected one, and the correction is now the most
instructive thing in the file.

The uncomfortable part is that the discipline this repo is built on did not stop me from breaking it
— it only caught me afterward, and only because I looked. Which is the honest report on what these
organs do: they do not prevent, they witness. That is worth more than a cell that had been right the
first time.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                   -> 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk       -> 15
./fkwu --src cognition/tests/pronoun-attestation-trace-band.fk     -> 11111111
./fkwu --src cognition/tests/gender-typology-trace-band.fk         -> 111111
./fkwu --src cognition/tests/word-gender-derivation-fourway.fk     -> 0   (all five FOUR-WAY)
```
