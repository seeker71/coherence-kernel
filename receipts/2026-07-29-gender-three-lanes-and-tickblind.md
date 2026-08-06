# 2026-07-29 — three lanes for gender, and the tool that could not see the edit

Urs: *"what does Sanskrit and Rudolf Steiner say about the gender archetypes and how can we integrate
gender into the core since in this earth experience gender is playing a core role."*

The last clause is taken as given, not argued with. The question answered is **where** it goes.

## What this body already holds — and does not

Both halves of the question reach real cells, and both are empty of it.

`cognition/steiner-neutral-kernel.fk` (20 950 bytes) holds Steiner as a surface-independent meaning
graph — `[subject, relation, object, evidence, order]` — with German *and* English registered as
projections *"so that no English string can pose as the teaching."* It contains a polarity, nine
mentions of it, and that polarity is **Lucifer / Ahriman with Christ mediating** — "die zwei
Gegenmaechte". It is not a gender polarity. On the question asked, **the body's Steiner graph is
silent.**

`learn/sanskrit-locale-baseline.fk` and `form-stdlib/sanskrit-channel.fk` contain **no liṅga at all** —
the Sanskrit here is roots and locale, not noun class.

So this cell records those two silences rather than filling them, and every claim it does carry is
marked with the lane it belongs to and the source answerable for it.

## Why `he`/`she` must not enter the core 64 — an argument from this repo, not from principle

`core-lexicon.fk`'s person family is `you me we us they it self`. I read that yesterday as a stance.
It is not; it is forced.

`nl-pivot-es.fk` states the invariant: *"la fuente es nativa"*, *"the source is native"*, **"sumber
adalah asli"**, *"die quelle ist nativ"* intern to the SAME pivot node, *"witnessed by node identity,
never by claim"* — and gender lives *"in the column, never in the pivot."*

**Indonesian is one of those four tongues, and its third person `dia` carries no gender.** If `he`/`she`
were primitives of the ground, the Indonesian column could not project the ground — it would have to
guess or drop, and the pivot would stop being a pivot and become a European one. The emptiness where
`he`/`she` would sit is what makes a fourth tongue possible, and a fifth.

## The integration: three lanes, because three different claims wear the word

`learn/gender-three-lanes.fk`, band **511**, run by `./fkwu --src` — written in S-expression on purpose,
because the cell carrying the same lane law in the BML brace surface has returned `verdict=none` since
2026-07-22 (row 928, `mutelaw`).

| kind | lane | where it belongs |
|---|---|---|
| **liṅga** — grammatical noun class | evidence | the emitter column, per tongue |
| **sex** — biological | evidence | the measured lane, with the biology |
| **polarity** — archetypal | **belief** | the meaning graph, held as belief |

Sanskrit's three classes are `puṃliṅga / strīliṅga / napuṃsakaliṅga`, and the reason liṅga sits in
*evidence* rather than beside the archetypes is a falsifier the band requires to be non-empty: nouns
whose class does not follow the sex of their referent — `dāra` "wife" masculine, `kalatra` "wife"
neuter, `mitra` "friend" neuter. If that list were empty, noun class would be a claim about beings and
would belong in the belief lane. Those rows are marked `rented`: supplied by a rented mind, witnessed
by no cell here, admissible as claims to be checked and never as ground.

The archetypal pairs are **puruṣa / prakṛti** (Sāṃkhya) and **Śiva / Śakti** (Śaiva), with
**ardhanārīśvara** — the form that is half woman — named as the pair's own resolution. Puruṣa is the
still witness and prakṛti is all activity *including mind*, which is why that pair is not "men and
women" even inside the tradition that carries it. Steiner's masculine and feminine sit in this lane
too, held as belief, cosmology not asserted — the same discipline `energy-center-glands.fk` uses to
hold the chakra→gland correspondence beside molecular biology without letting either argue for the
other.

**Mutation-proven**, five mutations, each darkening exactly its own bit:

```
claim "he" must be IN the core        511 -> 447   (-64, pivot invariant)
claim "they" must be ABSENT           511 -> 447   (-64)
move polarity to the EVIDENCE lane    511 -> 479   (-32, the belief-lane bit)
empty the falsifier list              511 -> 495   (-16)
invert the lanes, belief over evidence 511 -> 509  (-2, the lane law)
```

That third mutation is why the lane law got rewritten. My first version walked the kinds asking whether
each one's lane was belief-or-better — and moving polarity *into* the evidence lane, the exact merge
the law exists to forbid, left it green. **A validity check wearing a law's name.** A per-row test
cannot state this law, because each kind carries one lane and ordering needs two things to order. The
law is now stated where it lives: belief ranks strictly below evidence, and every kind sits on one of
exactly those two.

## And then the tool stopped seeing my edits

Restoring after mutation 5 produced **509** when the source plainly read `1` and `2`. Clearing the
`.fkb` gave 511. A controlled loop:

```
without mtime bump:   7 of 8 mutate/restore cycles served a STALE image
with an mtime bump:   0 of 6
```

`.fkb` freshness is **mtime-based at one-second granularity**, and a tight edit→run→edit→run loop lands
the source write and the cache write in the same tick, so the edit is never seen. Bumping mtime fixes
it completely, which confirms the mechanism rather than guessing at it.

The failure direction is the bad one: **`mutated=511`** — the band reporting the previous, green verdict
on mutated source. And the workload that triggers it is *mutation testing*, which is this body's
central verification discipline. Every mutation table in every receipt here — the slot-map receipt has
one with twelve rows — was produced by exactly this loop.

## The most surprising teaching

**A cache keyed on when a file changed cannot tell two changes apart inside one tick, and the faster you
verify, the blinder it gets.** The defect scales *with* diligence: a slow reviewer never sees it, and
someone running eight mutations in ten seconds sees it seven times out of eight. `tickblind` — 0 hits
before this row, as are `sametick` and `clockblind`.

It also belongs to a family this week keeps producing. `understudy` aimed a gate at code the run never
takes; `sliverproof` measured the small part; `originblind` copied an exemplar correct only where an
ambiguity vanishes; `mutelaw` wrote a law its judge cannot read. `tickblind` is the first where the
gate, the code, the scope and the dialect are all correct and the *answer is simply from before.*

## Where discomfort turned to gold

Finding it by accident, in my own band, while checking my own work — and nearly writing it off. The
restore read 509, I assumed my `cp` had failed, and the honest next keystroke was `grep` on the source,
which said `1`. If I had trusted the tool over the file I would have "fixed" a cell that was already
correct. The discomfort worth keeping is that I had run bands after edits perhaps thirty times today
and have no way to know which of those verdicts were current.

## Ground stamp

```
./fkwu --src learn/tests/gender-three-lanes-band.fk -> 511
  mutations: 447, 447, 479, 495, 509 — each darkens exactly its own bit
cognition/steiner-neutral-kernel.fk — polarity present (9), gender polarity absent; it is lucifer/ahriman
learn/sanskrit-locale-baseline.fk, form-stdlib/sanskrit-channel.fk — no linga
core-lexicon.fk person family: you me we us they it self   (cl-is-word? "he" -> 0)
stale .fkb reads: 7 of 8 without an mtime bump, 0 of 6 with one
```
