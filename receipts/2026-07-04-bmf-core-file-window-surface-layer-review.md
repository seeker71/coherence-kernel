# 2026-07-04 -- bmf-core file-window surface layer review

## Why This Layer Exists

`file-byte-window` and `bmf-byte-cursor` gave the body a bounded file byte path,
but `bmf-core` still defined:

```text
surface-file(path) = surface-string(read_file(path))
```

That left the default file grammar entry on the whole-file read path. The next
honest movement was to make `cursor-file` itself use the reviewed byte-window
cursor, not add a second optional route while preserving the old one.

## Reviewer Pre-Review

Grok returned `PASS`, requiring:

- flip `cursor-file` now rather than leaving the old `read_file` route alive;
- prove string equivalence after changing `match-lit`;
- audit `surf-len` consumers;
- document the 4th cursor state slot;
- cover `cur-win2` comment/trivia at window boundaries;
- name the bounded capture limit and mirror edits.

Claude returned `PASS`, requiring:

- no silent truncation in `cur-slice`;
- pin byte-vs-codepoint comparison units at a byte boundary; after the later
  Layer 1a/1b sibling correction, the shared proof uses an ASCII boundary and
  defers multibyte UTF-8 file-window proof;
- centralize cursor construction;
- audit `surf-len` paths;
- require `cur-win2` boundary coverage;
- run existing regression gates before landing.

## Implemented Surface

Files:

- `form/form-stdlib/bmf-core.fk`
- `grammars/bmf-core.fk`
- `form/form-stdlib/tests/bmf-core-file-window-band.fk`

The two `bmf-core` mirrors remain byte-identical.

Cursor rows now have a centralized constructor:

```text
("cursor" surface pos state)
```

`cur-surface = nth 1` and `cur-pos = nth 2` remain stable. String cursors carry
an empty state. File cursors carry a `bbc` cursor state.

File surfaces now use:

```text
("surface" "file-window" path window-size)
```

`cursor-file(path)` constructs a file-window cursor with the default `bbc`
window size. `cursor-file-window(path, size)` is exposed for focused boundary
tests.

`match-lit` now compares literal bytes with `cur-peek` and advances through the
cursor abstraction. This removes the whole-surface length requirement and makes
literal matching work across file-window boundaries.

`cur-slice` for file windows assembles captures by looping over bounded
`fbw-read-window` chunks. It does not silently truncate at `fbw-max-window`.
Captures still materialize a string of their full captured length; streaming
capture values are deferred.

## Band

Command:

```sh
./fkwu --src <(cat form/form-stdlib/core.fk \
  form/form-stdlib/str-byte-at.fk \
  form/form-stdlib/form-fs.fk \
  form/form-stdlib/file-byte-window.fk \
  form/form-stdlib/bmf-byte-cursor.fk \
  form/form-stdlib/bmf-core.fk \
  form/form-stdlib/tests/bmf-core-file-window-band.fk)
```

Expected:

```text
32767
```

The band proves:

- `surface-file` is a `file-window` surface with unknown length sentinel;
- `cursor-file` and `cursor-file-window` construct `bbc`-backed cursors;
- literal matching works across a two-byte window boundary;
- a two-byte ASCII payload is matched across a window boundary;
- `cur-win2` sees `//` across the file-window path;
- comment skip crosses the boundary and reaches the next expression;
- capture/literal matching over the file cursor produces `12` and `34`;
- string cursor matching still works;
- `cur-slice` assembles a capture longer than `fbw-max-window`;
- `bmf-core` no longer contains the old `surface-string(read_file(...))`
  route or raw byte-at name;
- cursor row construction is centralized;
- the two mirrors are byte-identical;
- EOF advance is idempotent on file-window cursors.

## Verification

- `bmf-core-file-window-band` returned `32767`.
- `bmf-cursor-language-band` returned `1023`; the string cursor path stayed
  green with only `core.fk` plus `bmf-core.fk`.
- `bmf-core-band` returned `600`.
- `bmf-grammar-band` returned `2047`.
- `form-ontology-parity-band` returned `1497`.
- `grammar-loader-band` returned `65535`.
- A `surf-len` audit found only string-kind branches using concrete
  `surf-len` arithmetic. File-window EOF and matching use `bbc` byte/EOF state.

Additional non-gate observations:

- `bml-realfiles-band` points at
  `../docs/field/urs/artifacts/master-thesis-2000/companion/source-samples`,
  which is absent in this checkout, so its `0` result is not parser evidence.
- `bml-band` returned `133170177` against its historical `268435455` expected
  value. That broader BML semantic-lowering surface is not folded into this
  core cursor layer; it remains a separate layer to review rather than hidden
  under the cursor-file migration.
- A provenance check ran `bml-band` against the committed `HEAD` version of
  `bmf-core.fk` in this same checkout and returned the same `133170177`, so the
  observed BML partial predates this cursor-file flip.

## Sibling Boundary Correction

A later Layer 1a/1b sibling review narrowed the file-byte floor to NUL/ASCII
string-slice bytes. The earlier BMF core file-window band still used bytes
`195 169` and treated a multibyte UTF-8 boundary as shared proof. That was too
broad for the corrected lower layer.

The band was corrected:

- the fixture now uses an ASCII boundary payload, `65 89 90 66 ...`;
- `bcfwb-bit-lit-multibyte-boundary` became
  `bcfwb-bit-lit-two-byte-boundary`;
- the byte-cursor source check now expects `90` at offset `2`;
- the validator prelude header is a single parseable line;
- source and mirror static reads work from either repo root or `form/`;
- static checks require nonempty loaded files before negative scans or mirror
  parity can pass.

Corrected gate:

```sh
cd form && ./validate.sh form-stdlib/tests/bmf-core-file-window-band.fk
# -> 32767
```

No OOM or killed process occurred during this correction.

Corrective post-review:

- Grok/Jason returned `PASS`, with no required changes. Grok confirmed the old
  UTF-8 boundary proof was replaced by the ASCII `89 90` fixture, the renamed
  bit is live, and nonempty static reads close the false-green path.
- Claude/Popper returned `PASS`, with no required changes. Claude confirmed the
  stale broad terms are historical/deferred only, single-line preludes and
  cwd-safe reads are in place, and `bmf-core` mirror parity remains intact.

## Reviewer Post-Review

Grok returned `PASS`, with no required changes. Residual risks: the BML partial
is a follow-on semantic-layer review item, absent real-file samples mean the
real-file corpus gate is environmental here, and downstream consumer migration,
streaming captures, mid-parse mutation, and 9h remain deferred.

Claude returned `PASS`, with no blocking required changes. Residual risks: the
BML partial needed provenance, real-file parsing evidence is synthetic in this
checkout because the sample directory is absent, chunk-and-concat capture has
no performance evidence, mid-parse mutation remains undefined, and downstream
consumers should not treat cursor state slot 4 as a public shape before their
own migration review. The BML provenance check above was run in response.

## Deferred

- Streaming capture values that do not materialize full captured text.
- Migrating every downstream grammar/prelude test to include the file-window
  dependencies where `cursor-file` is used.
- Multibyte UTF-8 file-window boundary proof on the shared sibling floor.
- Mid-parse file mutation detection.
- Reviewing the broader `bml-band` partial result as its own BML semantic layer.
- Runtime artifact 9h executor.

The exchange stayed alive by removing the easy whole-file route from the core
entrypoint instead of preserving it as a hidden fallback.
