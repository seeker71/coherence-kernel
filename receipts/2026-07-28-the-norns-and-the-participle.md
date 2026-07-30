# 2026-07-28 — the Norns and the participle

Urs pasted a Wikipedia-derived German summary of Urðr, Verðandi and Skuld and asked what I say
about it.

Most of it is sound. One part of it is not in the Old Norse sources at all — and the way it got
there is **exactly `der Mond`**, at a much larger scale, in an encyclopaedia, taught as the meaning
of the myth.

| cell | verdict |
|---|---|
| [`cognition/norn-temporal-overlay.fk`](../cognition/norn-temporal-overlay.fk) | 12 claims, checked one by one |
| [`cognition/tests/norn-temporal-overlay-band.fk`](../cognition/tests/norn-temporal-overlay-band.fk) | **11111111** — four-way |
| [`cognition/tests/word-gender-derivation-fourway.fk`](../cognition/tests/word-gender-derivation-fourway.fk) | **0** — all **eleven** crossings FOUR-WAY |

```
nto-attested-count  ->  7   of 12 claims attested outright
nto-overlay-count   ->  3   not in the sources
nto-gram-count      ->  3   derived from the grammar of a name
```

## Attested, and said first

The three named norns; the hall at Urðarbrunnr; Yggdrasil; the daily watering of the ash with water
and the clay around the well so its limbs do not rot; the shaping of lives; thread and weaving
imagery. All in Gylfaginning and the Poetic Edda. **The text Urs read is largely right**, and the
honest order is to say that before the correction.

## Not in the sources: the past / present / future mapping

Checked this session:

> "it has been disputed that their names really imply a temporal distinction"
> "the words do not in their own right denote chronological periods in Old Norse but rather the idea
> of past, present, and future **in terms of fate itself**"

The Eddas name the three and say what they do. They do not hand one the past, one the present and
one the future.

## Where it came from — and it is one place

**The grammar of the names. Nowhere else.**

```
Urðr       past participle of `verða`, to become   ->  read as "the past"
Verðandi   present participle of the same verb     ->  read as "the present"
Skuld      from `skulu`, shall / ought             ->  read as "the future"
```

Someone read the **participle tense** off three names and derived a cosmology of time from it.

That is the identical move to reading `der Mond` and concluding something about the moon: a property
of the **word-form** attributed to the **referent**. `das Mädchen` was the small version of this.
This is the same mechanism producing a metaphysics — and unlike the Mädchen case, this one is not a
folk intuition anybody corrects. It is the standard summary.

The cell checks the coincidence rather than asserting it: **every overlay row is grammar-derived and
every grammar-derived row is an overlay, row for row.** The temporal scheme has one source, and it
is the morphology of three names.

## The falsifier, and it was in the primary text the whole time

`das Männchen` broke the standing reading of `das Mädchen` because the mechanism kept applying where
the meaning could not follow. The Norns have the same shape of witness, already in Snorri:

> "the youngest Norn, she who is called **Skuld**, ride[s] ever to take the slain and decide fights."

**Skuld is also a valkyrie.** A temporal category does not ride to battle. If Skuld simply *is* the
future, her valkyrie office is unintelligible; under the plainer reading — her name means *debt,
obligation, what is owed* — choosing who falls is exactly her work. The sources have her doing
something the temporal scheme cannot hold.

And "die drei Hauptnornen" understates a second thing the sources say plainly: there are **many
norns** beside the three, of the kin of the Æsir, of elves, of dwarfs. Three named is not three
existing.

The spin-and-cut framing is the third softening: threads and weaving are genuinely attested in Norse
sources, but the tidy three-role division of labour — one spins, one measures, one cuts — is the
**Moirai's** shape, and it gets laid over the Norns from outside.

## Perturbation

```
valkyrie made to fit the temporal scheme     11111111 -> 11101111   digit 5 falls
Urðr-is-the-past treated as attested         11111111 -> 11111001   digits 2, 3 fall
```

## The lane

Every attestation judgement is brought in by the rented voice, checked this session against the
Norns article and the scholarship it cites, and witnessed nowhere in this body. One door,
`nto-claims`. **The primary Old Norse texts were not read directly** — this is an
encyclopaedia-and-scholarship check, one step further from the source than the Grimm chapter was,
and that distance is stated rather than blurred.

## The most surprising teaching

The thread started with a question about whether a word's gender says something about the moon. It
has now found the same error producing **a theory of time**. Urðr/Verðandi/Skuld as past/present/
future is one of the best-known things anyone knows about Norse myth, and it is a reading of three
participles.

The scale changed and the mechanism did not. That is a stronger result than the moon case, because
nobody had to be persuaded that `der Mond` was innocent — but almost everyone believes the Norns are
time.

## Where the discomfort turned to gold

Urs brought this after a long stretch of me disagreeing with him, and the text plainly meant
something to him. The easy move was to affirm it warmly — it is *mostly* accurate, so affirming it
would have cost nothing and been defensible.

But the part that is wrong is the part everyone repeats, and letting it stand while calling the rest
correct would have been the polite lie by omission. What made it possible to say plainly was that the
correction is *not* a diminishment: the Norns lose a nineteenth-century schema and keep the well, the
tree, the clay, the weaving, and a Skuld who is owed something and rides for it. **Losing the overlay
makes her more interesting, not less.**

The gold: this is the first time in the thread that the error I have been chasing turned up somewhere
neither of us was looking for it, in a text he brought to me. That is the difference between having
an argument and having an instrument.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                -> 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk    -> 15
./fkwu --src cognition/tests/norn-temporal-overlay-band.fk      -> 11111111
./fkwu --src cognition/tests/word-gender-derivation-fourway.fk  -> 0   (all eleven FOUR-WAY)
```

Sources retrieved 2026-07-28: en.wikipedia.org/wiki/Norns (temporal distinction disputed; many
lesser norns; Skuld as valkyrie), en.wikipedia.org/wiki/Urðr and /Verðandi (participle etymologies).
