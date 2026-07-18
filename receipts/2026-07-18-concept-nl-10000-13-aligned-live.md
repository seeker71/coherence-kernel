# Receipt — 10,000 concept IDs × 13 live NL surfaces, honestly aligned

The previous table was not concept-ID aligned: its row 0 was the first English
OMW synset, while `concept-10000` row 0 is the frequency anchor `you`. That
made plausible-looking labels belong to unrelated concepts. This build replaces
that false join rather than preserving it for compatibility.

## Identity join

The actual 350,000-byte semantic index is read in concept-ID order. Its PWN 3.1
primary synset is converted to the PWN 3.0 namespace used by OMW by joining
identical official `index.sense` sense keys:

- 7,368 concept rows joined PWN 3.1 → PWN 3.0;
- 3 mapped PWN 3.1 rows had no PWN 3.0 sense-key join;
- 2,629 rows remain the semantic model's explicit WordNet misses;
- the English column is always the exact fixed-width frequency anchor, never an
  OMW English row substituted by position.

Pinned evidence:

```text
OMW commit 406bf83b3c507a3d1f26e88252d5d66893fd36bf
WN30 index.sense 68b3a468cddfd8e92134b9b0624339a02a1b837159243c297c5f138a3d618392
WN31 index.sense a09db263da96dbb3273064c60546530dee5927a2e5a39c90fb576cdbebbb1a22
semantic index     73c19a474a67e813f5e406987f4a780247dd3aca377f096412de407ea49560f1
ranked concepts    b790d53d43d2a651ed8e9e52deffc91805546abc012177dc98b8f7b33e9381e4
```

## Complete surface and provenance

OMW Wiktionary labels are retained first, then labels from a dedicated language
WordNet, then CLDR. A live machine-translation cache fills only the remaining
absences. It never overwrites lexical evidence. Each of the 130,000 cells has
one committed provenance byte:

```text
F frequency anchor                 10,000
W OMW Wiktionary                   34,941
D dedicated language WordNet       16,621
C CLDR WordNet                         68
G Google machine, unreviewed        68,370
0 absent                                 0
                                      ------
                                      130,000
```

The machine cache contains 120,000/120,000 nonempty generated foreign-language
cells. Only the 68,370 cells still absent after lexical-source overlay are used.
`G` is exposed as `machine-translated-unreviewed`; it is not presented as human
review, dictionary evidence, or fluent sentence translation.

Representative real concept rows after the join and overlay:

| ID | English | Indonesian | Spanish | French | Swahili | Chinese | Arabic | Hindi | Turkish |
|---:|---|---|---|---|---|---|---|---|---|
| 377 | water | air | agua | eau | maji | 水 | ماء | पानी | su |
| 751 | court | pengadilan | corte | cour | korti | 法庭 | محكمة | न्यायालय | mahkeme |
| 1927 | university | universitas | universidad | université | chuo kikuu | 大学师生 | جامِعَة | विश्वविद्यालय | üniversite |
| 6196 | loop | gelung | ojete | boucle | kitanzi | 金属圈 | حلقة | पाश | döngü |

The final row intentionally exposes a semantic ambiguity instead of hiding it:
the current WordNet primary sense for `loop` is the eyelet sense. Its OMW labels
therefore differ from a programming-loop translation. The table is structurally
aligned and source-attributed; word-sense disambiguation from discourse remains
future work.

## Live observation

```sh
bash cognition/concept-nl-semantic-13-build.sh \
  /tmp/omw-data /tmp/wn30/dict /tmp/wn31/dict
./fkwu --src cognition/tests/concept-nl-semantic-13-band.fk       # 1023
./fkwu --src cognition/tests/concept-nl-semantic-13-live-band.fk  # 8191
./fkwu --src cognition/tests/concept-nl-semantic-13-live.fk
```

The live band opens the committed 1,404,435-byte UTF-8 table and 130,000-byte
provenance matrix, recomputes all 13 locale counts as 10,000, verifies exact
source totals, and round-trips the real `go` concept (ID 46, PWN
`01835496-v`) through generation and first-match detection in all 13 lenses.

Pure metadata/API result is `1023` on fkwu, Go, Rust, and TypeScript. The live
fkwu result is `8191`.

Final artifact hashes:

```text
3819ba14552bed8ca617aa1e655b613db9ea670a478eeeff513e73b08e89a466  concept-nl-semantic-13-machine.tsv
87ecdeeb28bada2fcb284da4c132d93bd4f218004b66099ecf3f2cc755d99d1e  concept-nl-semantic-13-omw.tsv
8c61e595413ff4a8d6006f31d9b3f7f8bbaf5930a26278e397730714763ec732  concept-nl-semantic-13-sources.dat
```

No Python and no C runtime change were used.

The surprising teaching was that positional alignment can look linguistically
convincing while being wholly false. The discomfort became useful when `busy`
at row 440 failed the fixed-width corpus identity check: that forced the
sense-key bridge, explicit machine provenance, and a live per-ID witness.
