# Concept-10000 receives a pinned WordNet semantic lane

**Witnessed:** 2026-07-18
**Verdict:** 7,371 sourced semantic rows, 2,629 explicit misses; live band 1023; pure four-way band 127.

## Source and attribution

The source is the official Princeton WordNet 3.1 database archive:

- `https://wordnetcode.princeton.edu/wn3.1.dict.tar.gz`
- archive SHA-256: `3f7d8be8ef6ecc7167d39b10d66954ec734280b5bdcd57f7d9eafe429d11c22a`
- the archive's 29-line Princeton license header is preserved verbatim in
  `model/concept-semantics-10000-WORDNET-LICENSE.txt`
- the archive, every consumed source table, the preserved license, and both
  projected artifacts are pinned individually in
  `model/concept-semantics-10000-source-manifest.txt`

The authoritative WordNet source tables are not vendored. The compact runtime
projection and the license that permits its distribution are. This keeps the
body operational without disguising generated data as hand-authored meaning.

## Selection law

For each of the 10,000 real ranked lexical anchors:

1. accept an exact WordNet lemma; otherwise accept only a base lemma named by
   WordNet's own `noun.exc`, `verb.exc`, `adj.exc`, or `adv.exc` table;
2. within every available POS, select the first offset in WordNet's `index.POS`
   row — WordNet sense 1 for that lemma/POS;
3. across POS candidates, select the first-sense candidate with the greatest
   tag count from `index.sense`; a zero/count tie preserves noun, verb,
   adjective, adverb order;
4. retain total sense count and POS count so the selected primary does not hide
   lexical polysemy;
5. copy the selected synset's source gloss and every typed pointer symbol plus
   target POS/offset from `data.POS`.

This is deterministic lexical grounding, not contextual word-sense
disambiguation. The original lexical pointer source/target word numbers are not
retained; the operational relation lane is synset symbol and typed target ID.

An earlier attempt applied context-free suffix detachment and reported 8,934
rows. Audit exposed the false semantic join `his -> hi` (noun). That entire
1,563-row guessed layer was removed. Regular morphology without POS/context is
not an authoritative match. The honest final coverage is therefore smaller:

```text
anchors=10000
mapped=7371
miss=2629
exact=7234
exception=137
detached=0
polysemous=5618
relation_rows=7266
relations_total=72046
primary_n=4464
primary_v=1047
primary_a=709
primary_s=846
primary_r=305
payload_bytes=1621448
max_gloss_bytes=480
max_payload_bytes=7544
```

The POS accounting distinguishes adjective satellites (`s`) exactly as the
WordNet data rows do.

## Operational body

`model/concept-semantics-10000-index.dat` is a 350,000-byte table of 10,000
fixed 35-byte records. Each record stores mapping method, payload offset/length,
primary synset type and offset, total senses, POS count, tag count, and relation
count. SHA-256:
`73c19a474a67e813f5e406987f4a780247dd3aca377f096412de407ea49560f1`.

`model/concept-semantics-10000-payload.dat` is 1,621,448 bytes. It stores the
sourced lemma and gloss with byte lengths, followed by 11-byte typed relation
rows. SHA-256:
`bfe18c160eef14c9e4b2fd96dff6f2bccd35aa8372d3f52e393a5f2f59bf20f5`.

`model/concept-semantics-10000.fk` decodes those tables directly in Form. Its
stable entry points are:

- `cs10-at` / `cs10-at-in`
- `cs10-detect-text` / `cs10-detect-text-in`
- `cs10-synset-id`, `cs10-pos`, `cs10-lemma`, `cs10-gloss`
- `cs10-sense-count`, `cs10-pos-count`, `cs10-tag-count`
- `cs10-relation-at`, `cs10-relations`
- `cs10-audit` / `cs10-audit-in`

The public semantic record is `(anchor-id method synset-id pos lemma gloss
sense-count pos-count tag-count relation-count relation-bytes)`. Method 0 is an
explicit miss, 1 exact, and 2 WordNet exception. `cs10-detect-text` calls the
existing Concept-10000 lexical detector and then resolves that real anchor
through this semantic table; these cells are therefore called outside their
receipt and tests by an operational kernel API.

## Real live data

The full live witness decoded these source-backed rows in `fkwu`:

| anchor | method | primary | senses/POS | tag count | relations | source gloss opening |
|---|---:|---|---:|---:|---:|---|
| `you` (0) | miss | — | 0/0 | 0 | 0 | — |
| `is` (11) | exception → `be` | `v02610777` | 14/2 | 10,742 | 138 | have the quality of being… |
| `love` (122) | exact | `v01779085` | 10/2 | 43 | 12 | have a great affection or liking for… |
| `world` (227) | exact | `n09489410` | 9/2 | 49 | 11 | everything that exists anywhere… |
| `camera` (959) | exact | `n02946154` | 2/1 | 18 | 19 | equipment for taking photographs… |
| `caviar` (9999) | exact | `n07815555` | 1/1 | 0 | 2 | salted roe of sturgeon or other large fish… |

The first live relation targets were also decoded from the projected bytes:
`love + -> a01462344`, `world @ -> n00019308`, `camera @ -> n03932386`, and
`caviar @ -> n07815254`. Direct text calls returned anchor 122 / `v01779085`
for `love`, anchor 959 / `n02946154` for `camera`, and an explicit `-1` miss for
`coherence-kernel`.

`model/tests/concept-semantics-10000-live-witness.fk` emitted literal UTF-8 byte
lists for every ID, lemma, gloss opening, and relation above. Its full-index
audit returned:

```text
[7371, 2629, 7234, 137, 5618, 7266, 72046]
```

`model/tests/concept-semantics-10000-live-band.fk` asserts file sizes, that full
audit, all six representative states, real relation targets, and lexical-to-
semantic integration:

```text
./fkwu --src model/tests/concept-semantics-10000-live-band.fk
1023
```

## Independent mechanics and reproducibility

The pure decoder/integration band uses a tiny in-memory index and payload, so
the four independent walkers prove mechanics without borrowing host file I/O:

```text
fkwu       127
Go         127
Rust       127
TypeScript 127
```

A second projection with `LC_ALL=C` and the pinned WordNet tables compared
byte-for-byte equal with both workspace projections using `cmp`; it reproduced
the two SHA-256 values and all accounting above. The projector is POSIX awk:
`model/concept-semantics-10000-project.awk`. No Python and no C change was used.

Ground remained fresh before this build: `42`, `55`, `15`,
`[1, 2.5, [3, 4]]`, and native-vs-rented `11111`.

## Honest edge

The 2,629 misses carry no invented gloss. A primary is lexical, not a claim
that it is the right sense in a sentence. Inflections absent from WordNet's
explicit exception tables remain misses. Multiword composition, contextual
disambiguation, richer relation payloads, and learned semantic embeddings are
not claimed here.

The surprising teaching was that refusing 1,563 morphology-shaped matches made
the lane more useful: an explicit unknown is safer semantic material than a
confident false noun. The discomfort was watching apparent coverage fall from
89.34% to 73.71%; it turned to gold when the full-data audit made every lost row
accountable and left the remaining meanings attributable. The exchange stayed
alive by making the semantic claims executable, literal, and inspectable rather
than asking the receipt to stand in for data.
