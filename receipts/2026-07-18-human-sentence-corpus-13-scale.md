# Thirteen-locale human corpus — non-toy scale witness

Date: 2026-07-18

## Observed movement

The attributed Tatoeba carrier grew from 147 to **1,300 real contributed
sentences**, exactly 100 in every declared NL lens. The committed snapshot is
429,086 bytes; its Form byte-offset index, generated metadata, and 13-archive
manifest bring the compact carrier to 448,543 bytes. Full upstream archives
remain outside git and are reproducible from the hash-pinned 130 MB compressed
source revision.

Every selected row retains the Tatoeba sentence ID and page URL, contributor,
source dates, CC BY 2.0 FR license, exact six-field source-row SHA-256, role,
matched surface, and the complete concept-ID result of a 10,000-label scan.
The fixed source-row selection began with up to 45 named daily-life strata,
four zero-detection rows, up to four surface collisions, then an open lexical
fill favoring new contributors and concepts. Canonical reindexing may move a
row between open/ambiguity/negative while never changing its source identity.
Stored results are expectations; Form independently replayed all 1,300 rows.

| lens | rows | contributors | detected concepts | detections | domain | open | ambiguity | negative |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| en | 100 | 72 | 379 | 759 | 45 | 51 | 0 | 4 |
| id | 100 | 44 | 634 | 988 | 39 | 53 | 3 | 5 |
| es | 100 | 60 | 398 | 794 | 44 | 48 | 4 | 4 |
| fr | 100 | 63 | 548 | 997 | 45 | 47 | 4 | 4 |
| pt-br | 100 | 45 | 449 | 865 | 45 | 47 | 4 | 4 |
| sw | 100 | 41 | 731 | 1,125 | 42 | 52 | 2 | 4 |
| de | 100 | 48 | 402 | 666 | 45 | 47 | 4 | 4 |
| ru | 100 | 60 | 313 | 450 | 45 | 49 | 2 | 4 |
| zh | 100 | 56 | 766 | 1,949 | 42 | 50 | 4 | 4 |
| ja | 100 | 61 | 580 | 1,441 | 41 | 51 | 4 | 4 |
| ar | 100 | 60 | 259 | 369 | 34 | 59 | 4 | 3 |
| hi | 100 | 44 | 416 | 716 | 38 | 54 | 4 | 4 |
| tr | 100 | 56 | 388 | 557 | 45 | 47 | 4 | 4 |
| **union / total** | **1,300** | **710 locale/contributor pairs** | **3,072 union IDs** | **11,676** | **550** | **655** | **43** | **52** |

There are 656 globally distinct contributor names. “Contributor” is an
attribution fact, not a native-speaker, editorial-review, or correctness claim.
Human-reviewed count remains exactly zero.

## Live evidence

`concept-human-corpus-13-live.fk` now prints source content rather than returning
an opaque string handle. Examples observed through the live complete detector:

```text
en | id=1292 | by=Ramses | detected=8 | attributed-domain-match | I don't know if I have the time.
id | id=331310 | by=umarsaid | detected=10 | ambiguity-retained | Ibu sedang masak di dapur.
es | id=2481 | by=Shishir | detected=3 | ambiguity-retained | ¡Intentemos algo!
fr | id=1115 | by=TRANG | detected=15 | ambiguity-retained | Lorsqu'il a demandé qui avait cassé la fenêtre, tous les garçons ont pris un air innocent.
pt-br | id=146680 | by=Leonroz | detected=3 | ambiguity-retained | Uma menina chorando abriu a porta.
sw | id=338673 | by=Sprachprofi | detected=0 | expected-zero-detection | Ninakupenda.
de | id=77 | by=ludoviko | detected=5 | ambiguity-retained | Lass uns etwas versuchen!
ru | id=243 | by=sugisaki | detected=11 | attributed-domain-match | Один раз в жизни я делаю хорошее дело... И оно бесполезно.
zh | id=5 | by=Zifre | detected=21 | ambiguity-retained | 今天是６月１８号，也是Muiriel的生日！
ja | id=1297 | by=xtofu80 | detected=16 | ambiguity-retained | きみにちょっとしたものをもってきたよ。
ar | id=331919 | by=hashimi | detected=1 | attributed-open-match | ويل للكافرين
hi | id=440811 | by=minshirui | detected=5 | ambiguity-retained | मैं मोहन के साथ गेंद खेलने जा रहा हूँ।
tr | id=170564 | by=Adopter | detected=3 | ambiguity-retained | Devenin belini kıran son saman çöpüdür.
```

Indonesian sentence 365975, `Kameraku tahan air.`, independently detects water
ID 377 and enters the world model. Sentence 331367, `Rumahku dirancang agar
tahan gempa.`, returns zero candidates and abstains. The world gate is `15`.

## Exact exhaustive replay

The complete 1,300 × 10,000 walk was divided into thirteen disjoint contiguous
100-row shards using `hcnl13-runtime-scan-range`. This is operational
partitioning only: every shard calls the same Form occurrence law over all
10,000 locale labels, and stored candidates never enter detection. The observed
sum was **1,300/1,300 valid, 0 mismatches, 11,676 detections**. The bounded shard
gate returns `15`; all thirteen raw shard witnesses returned 100 valid rows.

```sh
HCNL13_START=1200 HCNL13_END=1300 \
  ./fkwu --src cognition/tests/concept-human-corpus-13-range-band.fk
# 15

./fkwu --src cognition/tests/concept-human-corpus-13-world-live-band.fk
# 15
```

Snapshot SHA-256:
`2912908cd4c6efd67e546f6114862d9e53b32c20b58133d8c9f9d939b4fdd2f1`.
Archive-manifest SHA-256:
`85c9de11503654e88496adc3feb44182d3cf9ae146cf8d09ae1bac168fab296d`.
The selected source rows remain archive-hash-bound. Their detector fields were
deterministically reindexed without Python after the 111 canonical labels were
materialized into the primary table.

## Human-audio floor, not inflated

The independently hash-pinned Lingua Libre corpus remains **13 recordings,
13 locales, 13 speakers, 8 unprompted Whisper successes and 5 retained
misses**. Its offline complete-detector replay is `255`; source-integrity gate
is `31`. No newly discovered audio row was admitted without hash-pinned bytes,
source metadata, neutral-path ASR, and a complete detector result, so the hoped
five-recording-per-locale scale is still owed. These numbers do not claim native
acoustic recognition.

The movement stayed alive by turning “more text” into 13 million actual label
comparisons over attributed human speech acts, while preserving all zeroes.
The surprising teaching was that Swahili's small 4,583-row upstream archive
still exposed 737 local concept IDs in a balanced 100-row slice. Discomfort
turned to gold when audio acquisition could not meet the same verification
standard: the count stayed at thirteen instead of laundering unpinned files
into apparent progress.
