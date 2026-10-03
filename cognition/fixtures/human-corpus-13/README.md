# Attributed human sentence snapshot

This directory holds the source pins for Tatoeba's per-language `sentences_detailed`
exports: 13 locales. It does **not** contain the archives. Its pins are read by
`cognition/concept-human-corpus-130000-ingest.fk`, which projects the first eligible
rows of each pinned archive (see `../human-corpus-130000/README.md`).

Provenance, 2026-10-04: a bounded snapshot of 100 selected rows per locale was once
rebuilt by a Node recipe and a curl fetch script that lived beside this directory
(`cognition/concept-human-corpus-13-build.mjs`, `concept-human-corpus-13-fetch.sh`). Nothing
in the body read its products (the sentence snapshot, an offsets cell and a metadata cell,
all untracked), the Form-native intake above reads the same pinned archives, and both files
left the tree; git keeps them. The selection described below is the snapshot's, kept as
the record of what its rows were.

Tatoeba releases the download files under
[CC BY 2.0 FR](https://creativecommons.org/licenses/by/2.0/fr/). `ARCHIVES.tsv`
pins the retrieval URL, retrieval stamp, license, and compressed-archive SHA-256
for every language. Every row the recipe selects retains the contributing
username, sentence ID and page URL, source dates, license, and SHA-256 of the
exact six-field source row.

Every locale contributed exactly 100 rows. The deterministic selection
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

The archives are about 130 MB compressed at the hash-pinned 2026-07-18 revision. Fetching one
from its `ARCHIVES.tsv` URL and decompressing it for the intake's stdin are the OS programs
`curl` and `bzip2`, run by hand; no script of this tree does either.
