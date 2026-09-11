# Native concept source audit

`form-run ./fkwu observe/concept-source-audit-run.bml </dev/null` checks the
current canonical concept files entirely in Form. It observes 130,000 language
cells, all 10,000 English labels against their stable ranked IDs, 111 repaired
concepts, 111 alias routes, semantic coverage, provenance and eleven exact
manifest files. The 128,557 unaffected cells retain their pinned SHA-256
identity; the 1,443 overlay cells agree byte for byte with the primary surface.

The default hash policy, `form-sha256`, uses the portable Form SHA recipe. An
explicit JSON request selects the Form-emitted ARM64 CPU image:

```json
{"root":".","hash":"arm64-sha256"}
```

Send that object as one stdin line to the same door. Admission requires a
complete object with unique string `root` and `hash` fields; unknown fields or
hash policies refuse. Numeric source fields must contain complete ASCII
integers. Fixed-width labels discard only their trailing ASCII space padding;
leading spaces and trailing tabs remain meaningful bytes.

The ARM64 policy observes the existing guarded structural CPU capability
before admitting raw code. Its context belongs to the current process. An
unknown capability, unsupported observation or stale process identity refuses
before buffer or raw-image admission. The small structural probe remains in
the seed's compiled-leaf cache. Each SHA operation owns its buffer set and
program admission, performs a bulk copy from held immutable bytes, and releases
its resources without retiring another caller's admission of the same program.
This path currently uses the dynamically admitted platform adapter's owned
buffer and CPU-program doors; it does not make Metal a linked root dependency.

Every check reads held bytes. Sixteen distinct input files are read with exact
extent checks and read again before success. This detects an observed change
during the audit; it is not an atomic snapshot of multiple files. Primary
metadata and manifest bindings are content checks, not execution of their Form
definitions. Provenance markers remain source claims: `G` cells are unreviewed.

`form-run ./fkwu observe/concept-source-audit-witness.bml` exercises the real
door through retained child executions. Eleven grouped checks cover exact
portable/CPU SHA agreement through 2 MiB, padding boundaries, UTF-8 and binary
bytes, independent image survival, capability refusal, typed request refusal,
and missing/corrupted/restored private source copies. A refusal emits an
observation, correlated response and applied action before exiting nonzero.
Restoration is followed by a fresh successful full audit. The witness prints
the evidence directory and returns `11` with exit zero.

The current successful full audit emits 32 observations with portable hashing
or 33 with ARM64 hashing, including the CPU resource-count observation.
Independent review also executes malformed numeric and significant-whitespace
cases through the actual door. The audit never edits its input files.

## Native materialization

`form-run ./fkwu observe/concept-materialize-run.bml </dev/null` constructs and
publishes the canonical projection in Form. It accepts the same optional
`root`/`hash` JSON object. The exact table header, row widths and unique repair
IDs are checked before constructing the 10,000 rows, 130,000 source bytes,
10,001 byte offsets, thirteen locale hashes, metadata and manifest.

Construction holds all input bytes and checks the proposed result through the
native source audit before publication. Files first discovered during that
validation also join the freshness check. All original output files and source
dependencies are read again after staging. The five staged files must read back
exactly before their per-file renames; the manifest is last. A final full audit
observes the published generation. Each rename is atomic; the five-file change
is not one atomic snapshot.

A failed write, source freshness check or rename produces correlated
observation, response and applied rows. The owner removes its remaining private
stages and observes their disposition. Failure evidence retains all owned stage
paths and the number of destinations already changed, including a successful
rename whose subsequent readback refuses. Already published destinations remain
explicit; a fresh materialization reconstructs the complete projection from its
current inputs. A refused removal retains its path in the evidence.

The final JSON report carries `rows`, `repairs`, `cells`, `tableSha256`,
`sourcesSha256` and `sourceCounts` for `F`, `W`, `D`, `C`, `G` and `0`, plus its
schema and artifact count. The source builder invokes this native door with
closed stdin. Its other JavaScript construction responsibilities remain visible.

`form-run ./fkwu observe/concept-materializer-witness.bml` operates only on
private source copies. It verifies exact projected artifacts, executes all
10,001 offsets and all metadata functions against current runtime values,
repeats publication with portable hashing, rejects an altered header and a
changed validation dependency, and restores both before fresh publication.
Real failures after a first staged write and after a first successful rename
verify the owned-file cleanup and changed-destination count. The witness returns
`8` with exit zero and prints its retained evidence directory.

The current metadata generator expects its canonical generated function layout;
an absent or duplicate required function boundary refuses.

## Complete lexical candidate construction

`form-run ./fkwu observe/concept-roundtrip-build-run.bml </dev/null` builds the
complete 10,000-by-13 candidate index in Form. It accepts the same optional
`root`/`hash` request. The Form keyed map separates each locale and exact UTF-8
surface; every group retains all matching IDs in ascending order. Semantic
counts follow repair, primary, morphology overlay and lexical precedence.
Complete decimal fields and payload-relative spans are checked before use.

The builder emits 130,000 twelve-byte index records and 219,952 two-byte
candidate entries for the current sources. The 1,560,000-byte index and
439,904-byte candidate artifact reproduce the committed bytes exactly. Current
counts are 44,716 collision cells, 17,649 collision groups and a largest group
of seventeen. There is no candidate truncation.

Eight input files are held and hash-bound. All inputs and existing outputs are
reread after staging; the shared materializer owner publishes four artifacts,
with the manifest last. Per-file rename and cleanup have the same observed
ownership contract as canonical materialization. Generated metadata retains its
runtime values and validates all ten input/output file identities with portable
Form SHA, without command execution. Metadata itself remains direct Form so
existing proof readers can import it.

`form-run ./fkwu observe/concept-roundtrip-builder-witness.bml` constructs and
publishes a private source copy, compares both complete binary outputs and all
metadata values, checks manifest bindings, executes native file hashes, repeats
publication with portable hashing, and observes malformed decimal/span refusal
and CPU resource release. It returns `11`, exit zero, and retains the complete
evidence directory. The existing 130,000-cell operational detector remains the
independent consumer check.

## Attributed corpus reindexing

`form-run ./fkwu observe/concept-human-reindex-run.bml </dev/null` rebuilds the
attributed corpus in Form. An optional unique `root`/`hash` JSON object selects
the source root and hash policy. Three held sources supply the canonical labels,
the 1,300 attributed sentences and their archive manifest.

The byte Aho matcher validates UTF-8, folds ASCII letters only, preserves exact
surface insertion order and retains every matching concept ID in ascending
order. Chinese and Japanese use their unsegmented matching rule; other locales
retain word boundaries. Own matches precede suffix matches, including surfaces
that become equal under ASCII folding. The first observed ambiguous surface
therefore retains its identity.

Only detector evidence and role fields change. Contributor, license, source
row hash, review state, sentence and other attribution fields remain byte
exact. Row framing removes line terminators without trimming sentence spaces.
The current 429,086-byte corpus remains identical, with 11,676 detections of
3,072 concepts and role counts 550 domain, 655 open, 43 ambiguity and 52 negative.
All 1,301 offsets and metadata runtime values agree with the current consumer.
Generated file validation uses portable Form SHA directly.

`form-run ./fkwu observe/concept-human-reindex-witness.bml` witnesses complete
private publication, all preserved fields, matching order and UTF-8 boundaries,
a final sentence ending in spaces, native hashes, portable republication,
changed-source refusal and physical release of every private stage. Its final
value is `1`, exit zero. The shared owner publishes three independently atomic
files; it does not claim one atomic generation across them.

## Complete WordNet sense construction

`form-run ./fkwu observe/concept-sense-build-run.bml` receives one JSON stdin
object. The required string `dict` names the local WordNet dictionary directory;
optional `root` and `hash` select the repository root and portable or ARM64 hash
policy. Unknown or duplicate fields, incomplete paths and unknown policies
refuse. The dictionary is supplied explicitly; acquisition is separate.

The source manifest admits the full hashes of `index.sense`, `data.noun`,
`data.verb`, `data.adj` and `data.adv`. The preserved WordNet license must match
the source header. The builder holds those bytes and the semantic index and
payload, retains the original sense order, distinguishes adjective satellites,
normalizes gloss separators, and checks every projected sense count. A mapped
lemma requires a positive count and every referenced definition must exist.

The current complete result is 3,867,783 bytes, 34,244 senses, 7,371 mapped
anchors, 5,618 ambiguous anchors and a maximum of 75 senses per anchor. Its
SHA-256 is
`cda433f5de3b5cd1c7787bdbecd3e0ae4daad60b3537e9241bcb47192dc5aafb`.
All 10,001 offsets and metadata runtime values remain exact. Four staged
artifacts publish with the manifest last, after rereading both the repository
inputs and the separately held dictionary. Refusal releases owned stages and
reports any already published prefix.

`form-run ./fkwu observe/concept-sense-builder-witness.bml` receives a JSON
object containing the single string field `dict`. It compares the complete
data, every offset, every metadata value and all manifest bindings; publishes
a private copy; repeats with portable hashing; and observes malformed numeric
fields, spans, missing definitions, mapped-zero counts, duplicate requests,
NUL paths, source-identity changes and changes after staging. It returns `1`
with exit zero. The two public doors preserve their effectful preflight marker;
the witness library can be preflighted without acquiring or publishing data.

## Deterministic substantive construction

`form-run ./fkwu observe/concept-substantive-build-run.bml </dev/null` rebuilds
the substantive carriers and their downstream projections in one native
process. The optional `root`/`hash` JSON request follows the same complete,
unique string-field admission. Empty paths, NUL bytes, unknown fields and
unknown policies refuse before source work. Portable Form SHA is the default;
`arm64-sha256` selects the explicitly admitted native CPU image.

The source owner admits the ordered manifest and four exact held inputs:
ranked bytes, evidence JSONL, the translation table and its source matrix.
Complete typed evidence objects bind all 111 stable IDs and legacy labels to
their defect rows, ordered ranks, frequencies, page/revision identities and
translation rows. The recorded source hash and commit also agree with the
existing frequency provenance authority. This verifies held provenance; it
does not reacquire the frequency source or recompute absent full English
revision sections. Translated cells remain attributed and unreviewed.

Form constructs the ranked overlay, complete lexical and alias indices,
semantic index and payload, migration table, both executable metadata files
and the eleven-entry manifest. Balanced byte accumulation and native byte-order
mergesort retain every row. The six data carriers remain byte-identical to the
current source generation. Metadata carries current source descriptions and
the ordinary `current-rank` argument for IDs outside the repair set.

The proposed generation passes the complete source audit before staging. All
held dependencies and existing destinations join the freshness boundary; the
shared publication owner stages exact bytes, rereads inputs, publishes each
file with readback, and releases its owned stages. The manifest is last within
the nine-artifact substantive publication. A damaged derived artifact can be
reconstructed from admitted input evidence.

The workflow then invokes native materialization, lexical candidate
construction and attributed corpus reindexing, in that order, in the same
process. The four owners publish 21 artifacts altogether. Each owner reports
its completed prefix and stage disposition on refusal; the workflow does not
claim an atomic transaction across all files.

`form-run ./fkwu observe/concept-substantive-builder-witness.bml` prepares a
complete private source snapshot and exercises the actual public door. It
compares all 21 prepared artifacts, executes the privately generated Form
authorities, damages the alias carrier and repeats the complete workflow using
the default portable hash policy. The witness retains child stdout, stderr,
exit status and exact file evidence. Its successful final value is `1`.

`form-run ./fkwu observe/concept-substantive-runtime-witness.bml` independently
reads the evidence and checks all 10,000 generated rank and repaired-ID
queries, three boundary IDs, all 111 ID/rank list entries, every path door and
all 112 overlay offsets against actual newline boundaries. It returns `1` with
exit zero. The existing substantive semantic band remains the independent
4095 consumer check.

The existing source command delegates its non-refresh entry to this native
workflow. The JavaScript refresh owner still acquires the pinned frequency source,
selects revision-bound Wiktionary definitions, obtains translations and builds
fresh evidence. Fresh acquisition needs its own admitted receipt and complete
proposed projection before replacing current manifest-bound inputs. Those
responsibilities remain explicit until native ownership is observed.
