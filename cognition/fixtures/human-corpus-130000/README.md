# Attributed human corpus projection toward 130,000

These shards are a Form-native projection of the first eligible, attributed
rows in thirteen exact Tatoeba `sentences_detailed` exports. The archive URLs,
compressed hashes, retrieval stamp, license, projection hashes, row counts, and
the measured native intake cost are retained in `SOURCE-MANIFEST.tsv`.

Every projected row carries the upstream sentence id, contributor name, added
and modified timestamps, license, sentence page, exact upstream-row SHA-256,
and unmodified sentence bytes. The `.u32le` companion holds one little-endian
byte offset before every row plus a final end offset, allowing Form to retrieve
a row without loading a whole shard.

The selection law is deliberately content-neutral: validate six fields, exact
language, positive sentence id, named contributor, and nonempty sentence; then
retain the first 10,000 eligible rows in upstream order. It does not translate,
paraphrase, prompt, balance, duplicate, or manufacture content.

The actual total is **124,583**, not 130,000. Twelve lenses contain 10,000 rows.
The complete pinned Swahili archive contains only 4,583 eligible rows, leaving
an observed deficit of 5,417. No row was repeated or synthesized to hide that
shortfall. `human-contributed-unreviewed` does not claim native-speaker status,
review, factual correctness, parallel translation, or novel generation.
