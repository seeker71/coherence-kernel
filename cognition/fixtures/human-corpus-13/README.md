# Attributed human sentence snapshot

This directory holds the source pins for a bounded snapshot of Tatoeba's
per-language `sentences_detailed` exports: 13 locales, 100 selected rows each.
It does **not** contain the full archives, and the tree carries the recipe and
its pins, not the snapshot rows. A run of the recipe writes them.

Tatoeba releases the download files under
[CC BY 2.0 FR](https://creativecommons.org/licenses/by/2.0/fr/). `ARCHIVES.tsv`
pins the retrieval URL, retrieval stamp, license, and compressed-archive SHA-256
for every language. Every row the recipe selects retains the contributing
username, sentence ID and page URL, source dates, license, and SHA-256 of the
exact six-field source row.

Every locale contributes exactly 100 rows (`rowsPerLocale` in
`cognition/concept-human-corpus-13-build.mjs`). The deterministic selection
retains named real-life concept strata where the locale contains them, up to
four surface-collision observations, four zero-detection observations, and an
open lexical fill chosen to increase contributor and detected-concept
diversity. The concept labels come from the OMW label table the recipe reads,
which is produced from the OMW pins in
`cognition/concept-nl-semantic-13-source-manifest.txt`.

The snapshot state is `human-contributed-unreviewed`: the export proves a named
contributor and source history, but does not prove native-speaker status,
professional review, factual correctness, or that independently selected rows
are parallel translations. The runtime returns these sentences only as
attributed quotes, never as evidence of novel language generation.

Regenerate and verify (about 130 MB compressed at the hash-pinned 2026-07-18
revision):

```sh
./cognition/concept-human-corpus-13-fetch.sh
```

This uses `curl`, `bzip2`, and Node. It does not invoke Python. Downloads go to
a temporary directory and the script succeeds only when every archive matches
its pinned hash and the four products (the sentence snapshot, the offsets and
metadata cells, and `ARCHIVES.tsv`) match what the recipe produces byte-for-byte. To write the products, run
`node cognition/concept-human-corpus-13-build.mjs <archive-dir>` over a directory
holding the pinned archives as `<lang>.tsv.bz2`; the same command with
`--verify` compares instead of writing.
