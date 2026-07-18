# 1,445 real Wiktionary definitions close the non-WordNet semantic floor

; witnessed: 2026-07-18 -> 9,889 represented / 111 explicit holes.

## What this lane means

The Princeton WordNet projection plus explicit Wiktionary morphology represented
8,444 of the 10,000 frequency-ranked anchors. Its 1,556 remaining method-0 rows
were not all missing words. They included pronouns, contractions, discourse
markers, proper names, non-WordNet parts of speech, and lexical entries rejected
by the morphology lane's intentionally narrow acceptance law.

This lane queried every one of those 1,556 anchors from the live English
Wiktionary API. It accepts the first substantive English definition at the exact
title; when that title has no English definition, it retries the capitalized
title so a source-defined proper name can remain a proper name. Every accepted
row stores the original anchor, lookup law, source title, part-of-speech heading,
verbatim definition wikitext, definition count, page ID, revision ID, timestamp,
and SHA-256 of the complete English section. Wiktionary reported the snapshot
license as CC BY-SA 4.0.

Method 4 means **revision-pinned Wiktionary lexical definition**. It never means
WordNet synset, resolved contextual sense, or generated fallback. WordNet methods
1–3 keep precedence in `cs10l-at`; only their explicit misses can reach method 4.

## Exact result

```text
before                           8,444 represented / 1,556 holes
exact-title Wiktionary             997 definitions
capitalized-title Wiktionary        448 definitions
after                            9,889 represented /   111 holes
```

The 1,445 source-defined rows span real lexical classes rather than one suffix
template:

```text
noun 493                 proper noun 438
verb 188                 adjective 110
interjection 77          pronoun 39
adverb 19                preposition 19
determiner 17            conjunction 16
contraction 10           prefix 8
particle 4               numeral 3
phrase 2                 article 1
symbol 1
```

## Real rows

Definitions below are the exact committed Wiktionary wikitext, not paraphrases.
The revision link makes each source independently inspectable.

| anchor / class | source definition | page revision |
|---|---|---|
| `you` / pronoun | `{{lb|en|object pronoun}} The [[people]] spoken, or written to, as an object. {{defdate|from 9th c.}}` | [you, revision 91459351](https://en.wiktionary.org/w/index.php?oldid=91459351) |
| `'s` / contraction | `{{contraction of|en|is}}.` | ['s, revision 91576145](https://en.wiktionary.org/w/index.php?oldid=91576145) |
| `gonna` / colloquial verb | `{{lb|en|colloquial}} {{non-gloss|A [[modal]] used to express a future action that is being planned or prepared for in the present.}}` | [gonna, revision 90991819](https://en.wiktionary.org/w/index.php?oldid=90991819) |
| `roger` / radio interjection | `{{lb|en|radio|_|telecommunications|procedure word}} [[received|Received]] {{qualifier|used in radio communications to acknowledge that a message has been received and understood}}` | [roger, revision 91533704](https://en.wiktionary.org/w/index.php?oldid=91533704) |
| `richard` → `Richard` / proper noun | `{{given name|en|male|from=Germanic languages}}.` | [Richard, revision 91623084](https://en.wiktionary.org/w/index.php?oldid=91623084) |
| `amy` → `Amy` / proper noun | `{{given name|en|female|from=Latin}}.` | [Amy, revision 90359092](https://en.wiktionary.org/w/index.php?oldid=90359092) |
| `heads` / warning interjection | `{{non-gloss|A shouted [[warning]] that something is falling from above}}, mind your heads; [[heads-up]].` | [heads, revision 91122236](https://en.wiktionary.org/w/index.php?oldid=91122236) |
| `carmen` / noun form | `{{plural of|en|carman}}` | [carmen, revision 89743580](https://en.wiktionary.org/w/index.php?oldid=89743580) |

These are operational data. `cs10l-source-at` decodes the page evidence and
`cs10l-at` selects the three source layers in Form. `cs10l-open` opens the seven
large carriers once so production scans do not reread them for every anchor.

## Native witness and reproducibility

The builder emits a 150,000-byte source index, 357,719-byte definition payload,
and a 1,200-byte audit carrier with one exact/capitalized/rejected/valid row per
canonical block of 100 anchors. The bounded Form audit composes all 100 blocks:

```sh
./fkwu --src model/tests/concept-semantics-10000-wiktionary-lexical-live-band.fk
# 4095
```

Its decoded totals are:

```text
(exact-title capitalized-title rejected valid)
(997 448 111 1556)

(represented holes exact-title capitalized-title rejected valid failures)
(9889 111 997 448 111 1556 0)
```

The committed revision snapshot is rebuilt without Python:

```sh
node model/concept-semantics-10000-wiktionary-lexical-build.mjs
```

`--refresh` deliberately queries a new upstream snapshot. A normal run consumes
the committed evidence and reproduces the index, payload, audit blocks, exact
stats, remaining-hole table, and source manifest. Their independently computed
SHA-256 values are recorded in
`model/concept-semantics-10000-wiktionary-lexical-source-manifest.txt`.

## The 111-row floor is a substrate correction, not a definition prompt

`model/concept-semantics-10000-wiktionary-lexical-remaining.tsv` contains every
unresolved anchor. Most are subtitle-tokenization artifacts rather than stable
English lexical concepts: truncated negatives (`didn`, `doesn`, `wasn`), cut
turns (`you-`, `that-`, `and-`), fused tokens (`ofthe`, `ifyou`), dropped-g
spellings (`thinkin`, `workin`), encoding/confusable forms (`yöu`, `yοu`, `tο`),
or nonlexical artifacts (`chffffff`, `ch00ffff`). Acronyms and names such as
`nypd`, `bbc`, `lapd`, and `roberto` also need a different attributed source,
not a guessed Wiktionary definition.

The honest next path is to replace those 111 frequency-token anchors with 111
real source-defined concepts while preserving the old IDs as aliases and a
reversible migration table. Normalizing a corrupt token directly into a meaning
would hide the dataset failure and could merge distinct senses. This patch does
not do that.

The exchange stayed alive by treating WordNet rejection as a taxonomy question
rather than an invitation to manufacture glosses. The most surprising teaching
was that 438 frequent “holes” were source-defined proper names recovered by one
case-aware lookup law. The discomfort was discovering that the last 111 are not
an inference failure at all; it turned to gold when they became a concrete
replacement ledger instead of 111 confident fictions.
