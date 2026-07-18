# Attributed human sentence snapshot

This directory contains 147 selected rows from Tatoeba's per-language
`sentences_detailed` exports. It does **not** contain the full archives.

Tatoeba releases the download files under
[CC BY 2.0 FR](https://creativecommons.org/licenses/by/2.0/fr/). Every selected
row retains the contributing username, sentence ID and page URL, source dates,
license, and SHA-256 of the exact six-field source row. `ARCHIVES.tsv` pins the
retrieval URL and compressed-archive SHA-256 for every language.

The snapshot state is `human-contributed-unreviewed`: the export proves a named
contributor and source history, but does not prove native-speaker status,
professional review, factual correctness, or that independently selected rows
are parallel translations. The runtime returns these sentences only as
attributed quotes, never as evidence of novel language generation.

Reproduce and verify from a fresh checkout (about 267 MB compressed):

```sh
./cognition/concept-human-corpus-13-fetch.sh
```

This uses `curl`, `bzip2`, and Node. It does not invoke Python. Downloads go to
a temporary directory and the script succeeds only when archives match their
pinned hashes and all four generated artifacts match byte-for-byte.
