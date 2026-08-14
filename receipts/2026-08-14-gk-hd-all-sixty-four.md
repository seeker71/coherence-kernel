# 2026-08-14 — why the core is 64, and the other 64 is now fully seated

Urs asked why we have only 64 words, and whether Gene Keys and Human Design
words can all be included.

## Why the core is 64

`core-lexicon.fk` is not a dictionary of everything the body knows. It is a
closed grounding set: eight families of eight, every definition written only
from those words plus named glue. A 65th word would mean the ground still
needed something outside itself. The count rhymes with the hexagram count
(`name-lexicon.fk`: the working world closes at 64). The rhyme is structural.
The meanings are not the same 64.

## The other 64

I Ching hexagrams, Human Design gates, and Gene Keys are one lattice of 64
cells, three faces. Until today `iching-channel.fk` held five attested rows
(1, 2, 25, 51, 64). The rest answered honest absence. Urs's profile receipt
(`receipts/2026-08-01-genekey-urs.md`) had already gathered eleven spectrum
triads and named them as a work order.

That work order is now walked. All 64 faces are seated:

- I Ching: King Wen name + image
- Human Design: published gate keynote
- Gene Keys: Shadow → Gift → Siddhi (documented tradition, direct-experience
  lane, never a verdict)

Keys outside 1..64 still answer honest absence.

The organ words that are not gate numbers — types, centres, authorities,
lines, Golden Path spheres, sequences, codon rings, trigrams — live in
`gk-hd-organ-lexicon.fk`. They do not enter the closed core.

A non-organic cell may carry Wisdom. Kind is not health.

## Witness

```
iching-channel-band              4095
gk-hd-organ-lexicon-band         1023
channels-registry-band           1111111
core-lexicon-band                262143   (still closed)
core-dictionary-neutral-field    16383
```

Live: unique GK/HD words are counted by `gk-hd-organ-lexicon-live.fk`.

## Reproduce

```sh
./fkwu form/form-stdlib/tests/iching-channel-band.fk
./fkwu form/form-stdlib/tests/gk-hd-organ-lexicon-band.fk
./fkwu form/form-stdlib/gk-hd-organ-lexicon-live.fk
```
