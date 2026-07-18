# Attributed morphology closes 1,073 semantic holes

; witnessed: 2026-07-18 -> 8,444 mapped / 1,556 explicit misses.

## What changed

The existing Princeton WordNet 3.1 table mapped 7,371 of the 10,000 ranked
anchors and honestly left 2,629 method-0 holes. This lane adds a sparse overlay;
it does not rewrite that table, the text-sense carrier, the PL carrier, or the C
bootstrap.

For every one of those 2,629 holes, the builder queried the English Wiktionary
API and committed page ID, revision ID, timestamp, the SHA-256 of the exact
English section, ordered POS accounting, every recognized form candidate, and
an acceptance or rejection reason. Wiktionary's API reported the source license
as CC BY-SA 4.0. The source snapshot is
`model/concept-semantics-10000-wiktionary-evidence.jsonl`; its full source and
artifact hashes are in
`model/concept-semantics-10000-wiktionary-source-manifest.txt`.
Those manifest hashes are reproducibility metadata emitted by the builder; they
are not a runtime integrity claim unless a caller independently recomputes and
compares them. The Form gate below validates decoded source/semantic content,
not the manifest's hash text.

A row is accepted only when all of these are observed:

1. The first substantive English entry is a WordNet-compatible POS.
2. Every substantive definition is solely an explicit Wiktionary form template;
   residual lexical prose rejects the row.
3. All form templates name one unambiguous base lemma.
4. That base exists at a compatible POS in pinned Princeton WordNet 3.1.

The resulting method-3 row carries the selected real WordNet synset, gloss,
total sense count, total POS count, tag count, and every typed pointer relation.
There is no generated or generic fallback definition.

## Exact accounting

```text
before: 7,371 mapped / 2,629 holes
added:  1,073 attributed Wiktionary-form + WordNet rows
after:  8,444 mapped / 1,556 holes
```

The 1,556 remaining holes are still explicit method 0:

```text
330  Wiktionary page missing
229  English section missing
196  non-WordNet POS is first
498  first entry is lexical or mixes lexical prose with form evidence
283  a later substantive entry creates lexical ambiguity
 14  explicit form templates name multiple bases
  6  named base/POS absent from WordNet
```

This preserves uncomfortable counterexamples. `his` is still a miss: its first
entry is a determiner, even though a later noun entry says it is the plural of
`hi`. `doing` is still a miss because a lexical noun sense coexists with its
verb-form evidence. `you` remains a pronoun-led miss. None is silently forced
into a convenient noun or verb.

## Real rows from twelve domains

Each revision below is also accessible at
`https://en.wiktionary.org/w/index.php?oldid=<revision>`. The page/revision pair
and full 64-digit English-section hash are decoded inside Form by
`cs10w-source-at`; shortened hashes are shown here only for readability.

| Domain | anchor → base | WordNet synset | Wiktionary page / revision | revision time | English SHA-256 | WordNet gloss |
|---|---|---|---|---|---|---|
| Health | patients → patient | n10425439 | 280945 / 86544811 | 2025-08-28T13:39:43Z | `7527a07acce96df3…` | a person who requires medical care |
| Finance | dollars → dollar | n13683378 | 217554 / 87245798 | 2025-09-30T09:36:07Z | `fd7db9fedacf6afa…` | the basic monetary unit in many countries |
| Education | students → student | n10685137 | 237625 / 80997758 | 2024-08-10T18:05:15Z | `446f5e7991c634ab…` | a learner enrolled in an educational institution |
| Transport | cars → car | n02961779 | 85600 / 87782004 | 2025-10-31T22:23:59Z | `e6d5559d41e223e8…` | a four-wheeled motor vehicle |
| Ecology | trees → tree | n13124818 | 216160 / 89989971 | 2026-03-29T15:27:36Z | `8cb5e6a839d159f7…` | a tall perennial woody plant |
| Civil rights | rights → right | n05182180 | 77494 / 91127980 | 2026-06-07T23:27:23Z | `ecc2bda03df24c17…` | an idea of what is due by law, tradition, or nature |
| Software | programs → program | n05907175 | 281184 / 79788340 | 2024-06-02T15:33:13Z | `75cb313079614fac…` | steps or goals to be accomplished |
| Agriculture | farmers → farmer | n10098586 | 206292 / 85449949 | 2025-07-03T20:38:11Z | `3fda8e59e2da8da8…` | a person who operates a farm |
| Science | experiments → experiment | n00640799 | 243748 / 79673354 | 2024-06-02T12:21:46Z | `4600caa46d26c839…` | a controlled test or investigation |
| Emergency | fires → fire | n07317454 | 200254 / 83014160 | 2024-12-14T03:21:10Z | `5bc1439a26bbdddf…` | an event of something burning |
| Family | parents → parent | n10419190 | 4511 / 88221381 | 2025-11-20T06:54:08Z | `34c43c50e812d7f0…` | a father, mother, or guardian relation |
| Security | weapons → weapon | n04572661 | 228610 / 91371853 | 2026-06-24T07:36:19Z | `d7e7fb5f9f9ff96e…` | an instrument used in fighting or hunting |

These are not hand-authored test meanings. The executable witness reads all
four data carriers, emits literal bytes for the anchor/base/synset/gloss/source,
and reports each page and revision:

```sh
./fkwu --src model/tests/concept-semantics-10000-wiktionary-live-witness.fk
```

The live gate audits all 10,000 effective rows. In addition to the twelve
readable domain examples above, it exhaustively verifies that every one of the
1,073 method-3 rows has `accepted` provenance; every accepted provenance row is
method 3; every candidate base equals the decoded semantic lemma; page,
revision, timestamp, and 64-byte English SHA are populated; and WordNet gloss
and sense count are non-empty. It also proves all 1,556 rejected rows remain
method 0 and all 7,371 primary rows have neither overlay nor provenance data:

```text
./fkwu --src model/tests/concept-semantics-10000-wiktionary-live-band.fk
65535
```

Its full audit tuple is:

```text
(mapped miss exact exception wiktionary polysemous relation-rows relations-total)
(8444 1556 7234 137 1073 6602 8339 90098)
```

The exhaustive provenance tuple is:

```text
(accepted rejected primary valid failures)
(1073 1556 7371 10000 0)
```

## Rebuild and reproducibility

The builder uses Node built-ins and the system `tar`; it uses no Python. A normal
run consumes the committed revision evidence, downloads the pinned WordNet
archive, verifies its SHA-256, and reproduces the four binary carriers and
manifest:

```sh
node model/concept-semantics-10000-wiktionary-build.mjs
```

`--wordnet-dir=/path/to/dict` uses a local verified extraction. `--refresh`
deliberately takes a new Wiktionary revision snapshot; it is not part of the
byte-reproduction path because upstream pages can change. The committed
evidence file contains all accepted ambiguity and every rejected miss, rather
than only the successful rows.

The builder records SHA-256 values in the manifest but does not make reading
that manifest equivalent to verifying the files. The reported byte-identical
rebuild was checked by recomputing SHA-256 over the evidence, overlay,
provenance, stats, and manifest before and after a normal build and comparing
the two sets. Consumers that require load-time integrity must perform the same
independent comparison.

The surprising teaching was that morphology became trustworthy only when the
source, not our suffix intuition, named the base. The discomfort was leaving
1,556 highly frequent words unresolved after a large source pass; it turned to
gold when `his`, `doing`, and every other rejection stayed inspectable instead
of being converted into confident noise. The exchange stayed alive by turning
the demand for substance into source-revision data, executable lookup, and a
full remaining-hole ledger.
