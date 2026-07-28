# Public reality populates 850 exact world identities

Date: 2026-07-18

## What changed

The completion ledger's old `world-model-concept-identities = 28` was a stale
hand-entered floor. It did not include the public Tatoeba, Lingua Libre, and
Wikimedia Commons content merged immediately before it.

`observe/concept-public-reality-world.fk` now constructs an exact set union from
the content-derived ids of all three hash-bound datasets and persists every
union member through the ordinary `cwm-persist -> wm-persist` path. It does not
add unlike modality counters and call the sum a world.

## Live observations

```text
Tatoeba human-sentence detector identities       828
Lingua Libre human-speech admitted identities      8
Commons public-photo admitted identities           24

text ∩ audio                                         4
text ∩ visual                                        6
audio ∩ visual                                       1
text ∩ audio ∩ visual                                1

exact union                                         850
persisted world entities                            850
world concept count                                 850
resolved to their known ranked cells                850
```

The arithmetic cross-check is `828 + 8 + 24 - 4 - 6 - 1 + 1 = 850`; the Form
cell independently constructs the set and reports its length.

Concrete ordinary-world examples retain a source mask (`text=1`, `audio=2`,
`visual=4`):

| id | ranked concept | observed public carriers | mask |
|---:|---|---|---:|
| 377 | water | Tatoeba text + Indonesian human speech + Commons photo | 7 |
| 571 | book | Tatoeba text + English human speech | 3 |
| 628 | hospital | Tatoeba text | 1 |
| 752 | building | Commons built-heritage photo | 4 |
| 1520 | bridge | Tatoeba text + Commons archaeology photo | 5 |
| 5547 | butterfly | Commons insect-wildlife photo | 4 |

## Integrity and admission law

- Tatoeba ids come from the stored complete 10,000-label scans over 147
  attributed sentences. The exact corpus and archive manifest hashes must
  validate or the text set becomes empty. All lexical candidates are retained;
  ambiguity is observed rather than silently resolved.
- Human audio admits only the eight expected concepts found in unprompted
  Whisper transcripts. Five misses contribute nothing. Both observed and
  source TSV hashes must validate or the audio set becomes empty.
- Public vision admits only the 24 targets read back from original-pixel model
  observations. The provenance, raw model-output, and source-snapshot hashes
  must all validate; `verify-admissions.sh` also hashes every one of the 24
  JPEGs against the pinned source snapshot and joins each provenance target
  label to its corresponding original output at the 100,000-ppm floor. Any
  mismatch empties the visual set.
- For each distinct id, Form reads its ranked label, builds the matching known
  cell, and calls the same persistent world-model engine used elsewhere. All
  850 entities resolve; none remain an unnamed `new` cell.

## Reproduction

```sh
./fkwu --src presence/concept-public-reality-world-live.fk
# public-reality-world text=828 human-audio=8 public-visual=24
# intersections=4/6/1/1 union=850 persisted=850
# examples=id:mask 377:7,571:3,628:1,752:4,1520:5,5547:4

./fkwu --src observe/concept-public-reality-world.fk
# exact-union 850; persisted-entities 850; world-concepts 850

./fkwu --src observe/tests/concept-public-reality-world-live-band.fk
# 4095

./fkwu --src observe/tests/concept-10000-13-multimodal-completion-band.fk
# 1023 (truth gate: overall remains incomplete)
```

## Honest boundary

This is real content coverage and real Form-native evidence composition. The
speech recognizer and image classifier still use host-rented learned weights.
Tatoeba lexical candidates demonstrate detection, not disambiguated sentence
understanding. The union is 850/10,000, not completion.
