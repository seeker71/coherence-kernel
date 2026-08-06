# 2026-07-04 -- program-image table text witness layer review

## Why This Layer Exists

Layer 8i can render a valid program-image envelope into exact `.tbl` text and
can write/readback verify that text through `form-fs`, but it returns only
`1` or `0`. Layer 9h0 consumes a structured table-text witness row plus a
supplied table-run row. The missing seam was still manual:

```text
program-image envelope
  -> write/readback verified .tbl text file
  -> table-text witness row for 9h0
```

Layer 8i1 owns that seam. It packages the verified text file into an
observable carrier and emits the exact witness row shape consumed by 9h0. The
row tag is shared data, not a code dependency; the band proves the handshake.

## Pre-Review

Grok pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Grok:

- implement this as a new 8i1 layer, not as an 8i extension or 9h1 bridge;
- keep 8i1 free of a runtime-table-text-attempt dependency;
- define a local witness constructor structurally compatible with 9h0;
- define a no-witness sentinel;
- pin the carrier row shape;
- prove invalid pif inputs are nondestructive for absent and existing paths;
- prove mirror parity, static boundaries, and neighboring bands.

Claude pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Claude:

- emit the exact 9h0 witness row shape as data, without importing 9h0;
- pay for duplicated row shape with an explicit cross-layer handshake band;
- pin status/reason mapping:
  `witness-ready/table-text-file-witness-ready`,
  `refused/malformed-pif-envelope`,
  `refused/invalid-pif-envelope`,
  `refused/empty-table-text`, and
  `investigate/write-readback-failed`;
- force a write-failure path, for example a file in a missing directory;
- state that write failure and readback mismatch are collapsed because 8i
  currently returns only `1` or `0`;
- assert that a real witness row appears only when `wrote=1`.

## Implementation

Files:

- `form/form-stdlib/program-image-table-text-witness.fk`
- `grammars/program-image-table-text-witness.fk`
- `form/form-stdlib/tests/program-image-table-text-witness-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`

The new prefix is `pitw-`.

Public entry:

```text
pitw-file-witness-from-envelope(path, pif-envelope)
```

Carrier row:

```text
("program-image-table-text-file-witness"
  path
  pif-envelope
  rendered-table-text
  wrote
  witness
  status
  reason)
```

Ready rows carry:

```text
("runtime-table-text-witness" table-path readback-text)
```

Non-ready rows carry:

```text
("program-image-table-text-no-witness")
```

The source layer does not import or call 9h0. The row tag is data, and the band
proves that the 9h0 adapter accepts the produced witness.

## Witnesses

Required floor before edits:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
# known fread/getsockname warnings only
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
native-vs-rented-check -> 11111
```

Focused band:

```sh
./fkwu --src <(cat form/form-stdlib/core.fk \
  form/form-stdlib/form-fs.fk \
  form/form-stdlib/source-artifact-cache.fk \
  form/form-stdlib/source-artifact-descriptor.fk \
  form/form-stdlib/runtime-artifact-plan.fk \
  form/form-stdlib/runtime-artifact-selector.fk \
  form/form-stdlib/runtime-artifact-outcome.fk \
  form/form-stdlib/runtime-artifact-retry.fk \
  form/form-stdlib/runtime-artifact-load-envelope.fk \
  form/form-stdlib/runtime-artifact-attempt-receipt.fk \
  form/form-stdlib/program-image-fkb.fk \
  form/form-stdlib/program-image-tbl-emit.fk \
  form/form-stdlib/program-image-table-text-witness.fk \
  form/form-stdlib/reason-coverage.fk \
  form/form-stdlib/runtime-table-text-attempt.fk \
  form/form-stdlib/tests/program-image-table-text-witness-band.fk)
# -> 2147483647
```

The band proves:

- manifest boundaries and deferrals;
- valid pif writes exact golden `.tbl` text;
- witness path and text match the on-disk readback;
- carrier row field order is pinned;
- invalid pif leaves an absent target absent;
- invalid pif preserves a pre-existing sentinel file;
- malformed pif refuses with no witness;
- a forced write failure into a missing directory investigates as
  `write-readback-failed`;
- witness rows appear only when `wrote=1`;
- current valid pif envelopes render non-empty table text, so
  `empty-table-text` is a defensive branch for future 8i changes rather than a
  reachable public-path case today;
- a pitw-produced witness is accepted by 9h0 and becomes a 9f supplied attempt;
- the 9f/9c path completes for the supplied ok table run;
- runtime path mismatch still investigates through 9h0;
- source/mirror contain no 9h0 code dependency or forbidden binary/runtime
  calls;
- grammar mirror parity holds.

Investigation note: the first focused run returned `1879048191`, missing only
the static no-runtime-dependency bit. The guard was catching the manifest
phrase `no-runtime-table-text-attempt-dependency`, not executable dependency.
The manifest was renamed to `no-9h0-code-dependency`, and the focused band
returned the full mask.

Neighboring bands:

```text
program-image-fkb-band                 -> 2147483647
program-image-tbl-emit-band            -> 2147483647
runtime-table-text-attempt-band        -> 2147483647
runtime-artifact-attempt-receipt-band  -> 2147483647
runtime-artifact-outcome-band          -> 2147483647
reason-coverage-band                   -> 2147483647
```

Static checks:

```text
cmp grammars/program-image-table-text-witness.fk form/form-stdlib/program-image-table-text-witness.fk -> 0
forbidden 9h0/binary/runtime dependency scan over program-image-table-text-witness mirrors -> no hits
trailing whitespace scan over new/touched files -> no hits
git diff --check over the tracked architecture map -> clean
```

## Deferred

- Binary `.fkb` write/read.
- Program-image load/walk.
- Table execution.
- Supplied table-run generation.
- 9f attempt production outside the band handshake.
- 9c observation production outside the band handshake.
- Runtime selector installation.
- C-seed growth.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Extend 8i directly | Rejected | 8i owns text rendering and write/readback mechanics; 8i1 owns the witness carrier language. |
| Build this as 9h1 | Rejected | File writing belongs on the program-image side; 9h0 and later runtime layers consume witnesses and attempts. |
| Import 9h0 into 8i1 | Rejected | That would invert the layer dependency. The shared witness row is data, and the band proves the handshake. |
| Split write failure from readback mismatch now | Deferred | `pite-write-table-text` returns only `1` or `0`; the honest current reason is `write-readback-failed`. |

## Post-Review

Strict no-tool post-review:

- Grok post-review verdict: `PASS`.
- Claude post-review verdict: `PASS_WITH_CHANGES`.

Grok required changes: none.

Claude required one coverage confirmation: the `empty-table-text` reason had
to be either exercised or proven defensive. The current 8h/8i contract makes a
valid pif render at least the table section counts, so a valid public input
cannot currently produce empty table text. The focused band now asserts that a
current valid pif renders non-empty text while keeping the `empty-table-text`
reason as a future-proof defensive branch.

Focused follow-up:

```text
program-image-table-text-witness-band -> 2147483647
```

Follow-up reviewer verdicts:

- Grok follow-up verdict: `PASS`.
- Claude follow-up verdict: `PASS`.

Both accepted the defensive coverage framing. The reason identity is pinned,
and the band asserts the current live-path invariant that a valid pif renders
non-empty table text. If a future 8i change makes `empty-table-text` reachable
through the public path, this defensive framing must be retired and the branch
must receive a direct public-path case.
