# Native Wiktionary source acquisition

Form can reacquire all 111 retained Wiktionary revisions and reproduce their
stored English-section hashes and selected meanings from complete source text.
The observed acquisition used five requests. It changed no corpus artifact and
made no translation request.

| Authority | Responsibility |
| --- | --- |
| `form/form-stdlib/bml/concept-wiktionary-meaning.bml` | English-section boundaries, Unicode whitespace, part-of-speech headings, inflection exclusions and the first substantive definition |
| `form/form-stdlib/bml/concept-wiktionary-admission.bml` | Complete UTF-8 JSON, integer representation, typed unique fields, exact evidence writes and correlated observations |
| `form/form-stdlib/bml/concept-wiktionary-retained.bml` | Held metadata, requested revision sets, HTTP exchange, full source retention, page/revision binding and source rechecks |

Run the current acquisition through:

```sh
form-run ./fkwu observe/concept-wiktionary-retained-run.bml
```

The door requests the revisions already named by
`model/concept-10000-substantive-repair-evidence.jsonl`. Each private evidence
directory retains the complete HTTP requests and responses, transport results,
revision text and a source manifest. The current source file is held throughout
the operation and reread before completion.

Form admits the metadata schema and selected count, positive page and revision
identities, unique response pages and exact response cardinality. Each page must
match its requested title, revision and timestamp. Interpretation must reproduce
both the English-section SHA-256 and the stored section/definition pair.
Response ordering does not determine the match. A missing, repeated, changed or
incomplete source refuses with its retained evidence.

The HTTP body must have complete supported framing. Form uses the existing
synchronous dynamic TLS carrier and the shared
[HTTP response authority](native-pipe-workers.md). A batch of 25 revisions is
request policy. Full source text is retained without a line length cutoff.

The [source evidence](evidence/fkwu/wiktionary-source.json) distinguishes actual
network acquisition from replay of those captured bytes. All five replay
batches reproduce the 111 revisions; sixteen altered-input cases refuse at
their specific admission boundary. The independent meaning witness exercises
192 observations, including Unicode whitespace, inflection-only definitions,
source text beyond 128 KiB and complete retained definitions:

```sh
form-run ./fkwu observe/concept-wiktionary-meaning-witness.bml
```

Fresh ranked-candidate selection, current rights acquisition, translation and
publication of a newly selected corpus remain separate responsibilities of the
active refresh implementation. Re-observing pinned revisions does not establish
those operations. The native destination is one complete acquisition and
publication owner whose source identities, selection, translations and derived
artifacts can all be checked before replacing a current generation.
