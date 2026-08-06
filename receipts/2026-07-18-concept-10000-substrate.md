# 2026-07-18 — 10,000 ranked concept anchors become addressable

## Ground and source

The substrate is derived from HermitDave's `FrequencyWords` repository at
commit `525f9b560de45753a5ea01069454e72e9aa541c6`, file
`content/2018/en/en_50k.txt`. Its README attributes the 2018 lists to the
OpenSubtitles2018 tokenized source and licenses repository content under
CC-BY-SA-4.0 (code is MIT).

- upstream repository: <https://github.com/hermitdave/FrequencyWords>
- pinned source: <https://raw.githubusercontent.com/hermitdave/FrequencyWords/525f9b560de45753a5ea01069454e72e9aa541c6/content/2018/en/en_50k.txt>
- upstream full-file SHA-256:
  `5351ff405b1126ef555791dd4d9798a48e3e9a501a9fc481a9da957752cfb458`
- first-10,000-line slice SHA-256:
  `8697e086895f913fcc57aec28038a35fca619560349ee573755abdba6ef48f11`

The source rows are ranked surface word forms plus observed corpus counts. They
are not definitions and do not disambiguate senses. This cell therefore claims
**10,000 substantive ranked lexical concept anchors**, not 10,000 learned or
word-sense-disambiguated concepts. OpenSubtitles frequency also carries dialogue,
proper-name, spelling, and corpus-bias effects; rank is corpus frequency, never
importance or truth.

## Representation

`model/concept-10000-ranked.dat` is a 300,000-byte fixed-width rank table:

```text
label[20 bytes, space padded] | frequency[8 decimal bytes] ;
```

Its slot is the zero-based concept ID, making `c10-concept-at` arithmetic O(1)
without allocating a 10,000-row Form list. SHA-256:
`b790d53d43d2a651ed8e9e52deffc91805546abc012177dc98b8f7b33e9381e4`.

`model/concept-10000-lexical-index.dat` is a 260,000-byte byte-sorted index:

```text
label[20 bytes, space padded] | id[4 decimal bytes] ;
```

`c10-detect-text` uses Form's byte comparator and binary search, taking at most
14 decisions across 10,000 labels. SHA-256:
`f825944c54675fd7e4a94b2655fd9fd5fe2b3e4f6df6d3cc9a5653c24944dc4f`.

The projection used only `curl`, `head`, `awk`, `sort`, `fold`, `cmp`, and
`shasum`; no Python and no runtime C changes. Decoding the fixed rank table back
to `label count` rows was byte-identical to the pinned 10,000-line source slice.
The secondary index contains 10,000 rows and is byte-sorted.

## Stable Form API

- `c10-count()`
- `c10-concept-at(id)`
- `c10-detect-text(text)`
- `c10-detect-tokens(tokens)`
- `c10-detect-features(features)`
- `c10-concept-id(record)`
- `c10-concept-label(record)`
- `c10-concept-rank(record)`
- `c10-concept-frequency(record)`
- `c10-concept-features(record)`

Feature rows carry `(id label-byte-hash byte-length first-byte last-byte
frequency-band)`. Detection indexes by ID and verifies the whole signature, so a
corrupted predicted feature row is refused rather than silently relabeled.

## Witnesses

Required checkout ground returned:

```text
42
55
15
[1, 2.5, [3, 4]]
11111
```

The pure fixed-width parser, rank lookup, binary-search detector, integrity
features, missing-label path, and corrupt-feature refusal returned:

```text
fkwu / Go / Rust / TypeScript = 127 / 127 / 127 / 127
```

The full native live witness returned:

```text
[[300000, 10000], [260000, 10000],
 [0, [121, 111, 117], 1, 28787591, [0, 308566, 3, 121, 117, 0]],
 [122, [108, 111, 118, 101], 123, 830324, [122, 197162, 4, 108, 101, 1]],
 [227, [119, 111, 114, 108, 100], 228, 370620, [227, 249931, 5, 119, 100, 1]],
 [959, [99, 97, 109, 101, 114, 97], 960, 56757, [959, 917466, 6, 99, 97, 2]],
 [9999, [99, 97, 118, 105, 97, 114], 10000, 2510, [9999, 216656, 6, 99, 114, 3]],
 [122, 227, 959, 1], 959]
```

Nested strings print as intern IDs on the direct-source result path, so the live
witness emits literal UTF-8 bytes for labels. Those rows spell `you`, `love`,
`world`, `camera`, and `caviar`. Exact token detection resolved
`love/world/camera` to IDs `122/227/959`, rejected `coherence-kernel`, and
feature detection round-tripped camera to `959`.

## Honest remaining gap

The catalog supplies lexical recognition and stable cross-modal IDs. It does not
yet supply definitions, senses, multilingual equivalence, learned embeddings,
visual exemplars, or relations between all 10,000 anchors. Existing authored
concept recipes remain deeper tissue; this substrate widens addressability and
frequency-grounded detection without pretending breadth is understanding.

## Closing

What kept this alive was refusing both generated `concept-N` placeholders and a
10,000-arm constant list: attributed corpus data stays a carrier while lookup and
detection remain Form meaning. The most surprising teaching was that the honest
10,000-wide structure is two compact fixed-width views, not 10,000 allocated
cells. Discomfort turned to gold when a linear Form string scan ran for minutes;
that pressure revealed the proper architecture — arithmetic rank access plus a
14-step lexical binary search.
