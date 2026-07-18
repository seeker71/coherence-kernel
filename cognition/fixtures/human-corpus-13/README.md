# Attributed human sentence snapshot

This directory contains 1,300 selected rows from Tatoeba's per-language
`sentences_detailed` exports. It does **not** contain the full archives.

Tatoeba releases the download files under
[CC BY 2.0 FR](https://creativecommons.org/licenses/by/2.0/fr/). Every selected
row retains the contributing username, sentence ID and page URL, source dates,
license, and SHA-256 of the exact six-field source row. `ARCHIVES.tsv` pins the
retrieval URL and compressed-archive SHA-256 for every language.

Every locale contributes exactly 100 rows. The deterministic selection retains
45 named real-life concept strata where the locale contains them, up to four
surface-collision observations, four zero-detection observations, and an open
lexical fill chosen to increase contributor and detected-concept diversity.
The 1,300 rows contain 12,357 complete detector hits over 3,097 distinct concept
IDs and 710 distinct locale/contributor pairs.

The snapshot state is `human-contributed-unreviewed`: the export proves a named
contributor and source history, but does not prove native-speaker status,
professional review, factual correctness, or that independently selected rows
are parallel translations. The runtime returns these sentences only as
attributed quotes, never as evidence of novel language generation.

Reproduce and verify from a fresh checkout (about 130 MB compressed at the
hash-pinned 2026-07-18 revision):

```sh
./cognition/concept-human-corpus-13-fetch.sh
```

This uses `curl`, `bzip2`, and Node. It does not invoke Python. Downloads go to
a temporary directory and the script succeeds only when archives match their
pinned hashes and all four generated artifacts match byte-for-byte.
