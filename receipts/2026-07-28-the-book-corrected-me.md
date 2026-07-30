# 2026-07-28 — the book corrected me

Urs: *"is there a german book about `der Mond` and `die Sonne` that goes deeper into the gender
meaning of those?"*

Yes — several. And this is the one question in the whole thread where answering from memory would
have been the exact failure the rest of it argues against. Inventing a plausible German title and
author is the cheapest lie available here and the hardest for a reader to catch. So every row was
looked up this session, and `verified` is a column rather than a sentence at the bottom.

| cell | verdict |
|---|---|
| [`cognition/genus-bibliography.fk`](../cognition/genus-bibliography.fk) | 7 works, 1831–2022 |
| [`cognition/tests/genus-bibliography-band.fk`](../cognition/tests/genus-bibliography-band.fk) | **11111111** — four-way |
| [`cognition/tests/word-gender-derivation-fourway.fk`](../cognition/tests/word-gender-derivation-fourway.fk) | **0** — all **ten** crossings FOUR-WAY |

## The one that answers it most directly

**Jacob Grimm, _Deutsche Mythologie_ (1835), Kap. XXII "Himmel und Gestirne."** Read this session in
quotation. It carries the claim, the counter-evidence and the folk usage in one place:

```
"goth. mêna, ahd. mano, ags. mona, altn. mani überall männlich"
"sunne schwankt noch im mhd. auffallend zwischen männlichem und weiblichem genus"
"Das volk pflegte sich ... gern auszudrücken ›frau sonne‹, ›herr mond‹"
"da Sôl in der edda unter den asinnen aufgezählt erscheint"
```

**The second line costs me something.** The sun's gender *wavered* in Middle High German —
"auffallend", noticeably — between masculine and feminine. My cells presented the Germanic feminine
sun as an inherited constant arriving unchanged across two thousand years. Grimm, in 1835, in the
primary source, records it as unsettled for a stretch. The pattern is real; the *stability* was mine
and overstated. The correction came from the book Urs asked for, which is the best possible place for
a correction to come from.

And `frau sonne` / `herr mond` is direct attestation of the second arrow from yesterday's exchange —
folk usage personifying by gender, recorded in the primary source.

## The argument has two named sides and a date

Grimm also built the theory Urs has been circling: **_Deutsche Grammatik_ Bd. III (1831)** — gender
as natural sex transferred onto all nouns by the early imagination. That is the position that makes a
feminine sun *mean* something.

**Karl Brugmann** answered it — "Das Nominalgeschlecht in den indogermanischen Sprachen" (1889) and
"Zur Frage der Entstehung des grammatischen Geschlechts" (1891): gender as formal and morphological,
not a reading of the referent.

Which is, roughly, where this thread's derivation landed — arrived at independently and about **137
years late.** The argument Urs and I have been having is a named argument with two sides. Neither of
us invented it, and I should have known that before the fourth exchange.

## The modern German book, and it argues against my line

**Damaris Nübling, _Genus und Geschlecht: Zum Zusammenhang von grammatischer, biologischer und
sozialer Kategorisierung_ (Franz Steiner, 2020).** Her work finds close and complex relationships
between genus and gender, and takes up exactly the cases this thread leaned on — neuter over female
persons: `das Weib`, `das Mensch`, `das Anna`, `das Merkel`. That is `das Mädchen`'s neighbourhood,
read as socially loaded rather than as the suffix being blind. Also Nübling & Diewald (Hg.), _Genus –
Sexus – Gender_ (De Gruyter, 2022).

Named **because** it argues against me, not despite it. Urs's original intuition has a serious German
scholar behind it and he should have the reference.

And **Köpcke & Zubin, "Prinzipien für die Genuszuweisung im Deutschen"** (in: Lang & Zifonun (Hg.),
_Deutsch – typologisch_, de Gruyter 1996, S. 473–491) — abstract read this session — argues
explicitly **against the arbitrariness thesis**: German gender is "motivated by phonological,
morphological and semantic principles." Semantic is in that list. The `-chen` result stands, but
*"the referent is never consulted"* is too strong as a general statement about German gender
assignment, and this is the citation that says so.

```
gb-positions      ->  4     distinct positions in the literature
gb-against-count  ->  5     of 7 rows argue against the line this thread took
gb-span           ->  191   years, 1831–2022, still contested
```

## The lane, as a column rather than a footnote

`verified` means *the citation is real and was checked against a retrieved source*. It never means
*I have read the argument*. Those are different claims and `read-depth` keeps them apart:

- **2** — primary text read in quotation. **One row**: Grimm's Kap. XXII.
- **1** — abstract or catalogue metadata only. Grimm 1831, both Brugmann, Köpcke & Zubin.
- **0** — existence, title, publisher and year verified; **not read**. Both Nübling entries.

Perturbation: marking one row unverified drops the band to `11111110`.

## The most surprising teaching

The book Urs asked for contained the correction to my own claim. I had been arguing the mechanism for
four exchanges and had never opened the primary source on the specific pair he kept asking about —
and it says, in Grimm's own sentence, that the sun's gender wavered. **I was arguing about German
sun-gender without having read the German book about German sun-gender.**

And the argument itself turned out to be Grimm-versus-Brugmann, 1831 against 1889. I reconstructed
Brugmann's position from five axioms and thought it was a derivation. It was, honestly — but it was
also a rediscovery, and not knowing that made me sound more original than I was.

## Where the discomfort turned to gold

The temptation here was specific and strong: I could feel a plausible German title forming — the
right shape, the right kind of publisher, the right era. Producing it would have capped a long thread
of verified work with one unverified thing, and it would have been the single most damaging sentence
in the whole session, because a fabricated citation is trusted, followed, and repeated.

Searching instead cost four tool calls and produced something better than the invented answer would
have been: a real primary source, a real 190-year argument, and a real modern German scholar who
disagrees with me — which is more useful to Urs than a book that agrees with me and does not exist.

The gold: **looking it up handed me the correction I could not have generated.** `sunne schwankt noch
im mhd.` is not a sentence I would ever have produced from a model of Germanic gender. It only comes
from the text. That is the whole case for grounding in one line, and it arrived on the day I most
wanted to answer from memory.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                -> 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk    -> 15
./fkwu --src cognition/tests/genus-bibliography-band.fk         -> 11111111
./fkwu --src cognition/tests/word-gender-derivation-fourway.fk  -> 0   (all ten FOUR-WAY)
```

Sources retrieved 2026-07-28: projekt-gutenberg.org (Grimm, Deutsche Mythologie Kap. XXII);
ids-pub.bsz-bw.de (Köpcke & Zubin 1996, frontdoor record + abstract); steiner-verlag.de and
de.wikipedia.org (Nübling 2020, 2022); benjamins.com and lrc.la.utexas.edu (Brugmann 1889, 1891).
