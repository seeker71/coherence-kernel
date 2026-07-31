# 2026-07-28 — the second arrow

Urs, pushing back on the moon derivation:

> "and you don't think it is odd for the sun to be feminine and the moon to be masculine in German?"

Two questions live in that, and they get different answers. On the first I think the premise does not
hold. On the second he is right, and my earlier cell overstated its own reach.

| cell | verdict |
|---|---|
| [`cognition/sun-moon-inversion.fk`](../cognition/sun-moon-inversion.fk) | 13 cultures |
| [`cognition/tests/sun-moon-inversion-band.fk`](../cognition/tests/sun-moon-inversion-band.fk) | **1111111** — four-way |
| [`cognition/tests/word-gender-derivation-fourway.fk`](../cognition/tests/word-gender-derivation-fourway.fk) | **0** — all **nine** crossings FOUR-WAY |

## Is it odd? Measured, and no

```
smi-inverted-count   ->  8      feminine sun, masculine moon
smi-classical-count  ->  2      masculine sun, feminine moon
smi-inverted-groups  ->  4      unrelated groupings carrying it
```

`die Sonne` / `der Mond` reads as an inversion only against a baseline that is itself one branch. Sol
masculine and Luna feminine is **Greco-Roman**, and it dominates because Western schooling is
Greco-Roman — not because the world agrees.

A feminine sun with a masculine moon is carried by:

- **all of Germanic** — German, Old Norse (Sól a goddess, Máni a god), Old English, Gothic. The whole
  branch, inherited. German is not being strange; it is being Germanic.
- **Semitic** — Arabic *shams* feminine / *qamar* masculine; Hebrew *shemesh* feminine, *yareaḥ*
  masculine.
- **Baltic** — Lithuanian *Saulė* feminine and a goddess, *Mėnulis* masculine.
- **Japanese myth** — Amaterasu, the sun, female and ancestor of the imperial line; Tsukuyomi, the
  moon, male.

And Sanskrit has both masculine (*Sūrya*, *Candra*), Irish has both feminine (*grian*, *gealach*),
Russian's sun is neuter. **The "default" the German case gets measured against is two rows in this
set: Latin and Greek.**

So: not odd. *Unfamiliar to a Mediterranean-trained eye*, which is a different thing and worth not
confusing with a fact about the world.

## Where the push lands: my cell claimed one arrow and read like it claimed two

`word-gender-derivation.fk` establishes exactly one thing: **the referent does not determine the
word's gender.** Assignment runs off the stem-shape, `das Männchen` is neuter over a grown man, the
moon is never consulted. That stands. Nothing here moves it.

But there is a **second arrow it never tested**, running the other way: *does the word's gender shape
how the referent gets imagined?* In Germanic the sun is not merely grammatically feminine — Sól is a
**goddess** and Máni is a **god**. Grammar and myth agree. They agree in Latin too. Jakobson's old
observation is the general case: poets personify by grammatical gender.

```
smi-agreeing  ->  7   of 13 rows carry grammar and a recorded personification agreeing
```

I wrote "the referent was never consulted." That is true of *assignment* and reads as though it
denies the second arrow. It does not deny it. **It never tested it.** The two claims are compatible:
the moon does not give `Mond` its gender, *and* `Mond`'s gender may well give the moon its face. Only
the first was proven, and the cell now says so in its own header rather than leaving the overreach
standing.

## What the concession is not

The row that keeps it honest is **Japanese**. Amaterasu is female and Tsukuyomi is male in a language
with **no grammatical gender at all** — none, then or now. The inversion arose there with no grammar
to arise from.

So grammar is not *necessary* for the personification. The same picture is reachable without it,
which means the agreement is real and the causation is still not established — grammar shaping myth,
myth shaping grammar, or both descending from something older all remain open. This cell measures the
agreement and declines to pick an arrow, because the rows do not carry one.

Which is the same shape the typology trace found for gender and descent, arrived at from a completely
different direction: **correlation measured, arrow unestablished, and the case that breaks the
necessity sitting right there in the table.**

## Perturbation

```
Japanese given grammatical gender   1111111 -> 0011111   digits 6, 7 fall
German made Sol/Luna                1111111 -> 1110011   digits 3, 4 fall
```

## The lane

Every row — the genders, the deities, the groupings — is brought in by the rented voice and witnessed
nowhere in this body. One door, `smi-rows`. Hebrew *shemesh* is attested with both genders and is
entered as feminine, which is the common case rather than the only one; no digit rests on that row
alone.

## The most surprising teaching

I had answered the moon question three times today and never once checked whether the premise of the
follow-up was true. **The Sol/Luna arrangement is two rows out of thirteen here, and I had been
treating it as the world's baseline without noticing** — including in the original derivation, where
I described German as inverted relative to "the Latin/Greek mythological pairing" as though that
pairing were the norm rather than one branch's answer. The person asking whether something was odd
was working from a default I had quietly supplied.

## Where the discomfort turned to gold

The uncomfortable part was that the push was partly right and I wanted it to be entirely wrong. The
mechanism finding was solid, four-way, perturbed, and I had defended it well across three exchanges —
which is exactly the position from which a genuine correction is hardest to accept, because accepting
it feels like giving ground on the part that was correct.

It is not the same part. Assignment and personification are two arrows and I proved one. The sentence
"the referent was never consulted" was doing more work in the reader's mind than it had earned, and
the honest repair was to narrow my own claim in the cell rather than to argue that it had always been
narrow.

The gold: taking the second arrow seriously is what produced the Japanese row, and the Japanese row
is the sharpest thing in this cell — a sun goddess in a language with no gender to give her one. I
would not have gone looking for it while defending. It arrived because I stopped.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                -> 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk    -> 15
./fkwu --src cognition/tests/sun-moon-inversion-band.fk         -> 1111111
./fkwu --src cognition/tests/word-gender-derivation-band.fk     -> 111111111
./fkwu --src cognition/tests/word-gender-derivation-fourway.fk  -> 0   (all nine FOUR-WAY)
```
