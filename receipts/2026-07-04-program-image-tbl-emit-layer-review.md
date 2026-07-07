# 2026-07-04 -- program-image table text emitter layer review

## Why This Layer Exists

Layer 8h folded the `.tbl`-shaped payload into a program-image `.fkb` recipe
envelope, but it did not create a concrete artifact that the current checkout
can execute. The present `fkwu` still has a table executor path for numeric
`.tbl` text. This layer bridges those two facts without pretending that binary
program-image loading is already alive.

The language of this layer is table text emission:

```text
validated program-image envelope
  -> numeric stream in executor order
  -> single-space .tbl text with one trailing newline
  -> optional verified text write
```

It is not a loader, not a selector, and not a binary cache round-trip.

## Pre-Review

Proposed next layer after 8h:

- consume only a valid `pif-envelope`;
- render `pif-table-numeric-stream` to executor-compatible `.tbl` text;
- use exactly one ASCII space between integers and one trailing newline;
- optionally write the text through the current file face and read it back;
- reject invalid envelopes by returning empty text and by creating no artifact;
- prove the emitted `/tmp/program-image-tbl-emit.tbl` can be run by the
  current table executor, with the first value `42`.

Grok pre-review verdict: `PASS`.

Required condition from Grok:

- invalid or malformed writes must return `0` and avoid creating a new
  executable `.tbl` artifact at the target path.

Claude pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Claude:

- pin exact rendering order matching the executor: `nf`, function roots, `nr`,
  node rows x4, `ns`, then string rows as length plus bytes;
- use golden string comparisons, not token-set parity;
- specify whitespace exactly;
- prove invalid writes create no file on an absent path, using read/stat
  evidence;
- keep the dead binary IO probes as the grounded reason 9h remains deferred.

## Implementation

Files:

- `form/form-stdlib/program-image-tbl-emit.fk`
- `grammars/program-image-tbl-emit.fk`
- `form/form-stdlib/tests/program-image-tbl-emit-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`

The new prefix is `pite-`.

Primary functions:

- `pite-table-text`
- `pite-table-text-from-envelope`
- `pite-write-table-text`

The rendering convention is:

```text
<int> SP <int> SP ... <int> LF
```

No section newlines, labels, or padding values are inserted. The order is the
exact order of `pif-table-numeric-stream`, which mirrors the current table
executor read order.

Invalid writes are non-destructive. They return `0`, create no file when the
target is absent, and leave an existing file unchanged. They do not promise the
world has no executable artifact at that path; callers that need that stronger
condition must remove or choose a fresh path before calling this layer.

## Grounded Deferral

The reason this layer emits `.tbl` text instead of real binary `.fkb` is the
current fkwu probe recorded in Layer 8h:

```text
write_file_text/read_file control with core.fk -> ok, file exists
write_form_binary with core.fk -> nothing, no file
len(recipe_to_bytes (list 1 2 3)) with core.fk -> 0
```

So 9h is not entitled to claim binary artifact loading yet. Its entry condition
is either a live binary artifact IO path or an explicit decision to consume the
verified table-text path first.

## Witnesses

Required floor after implementation:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
# known fread/getsockname warnings only
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
native-vs-rented-check -> 11111
```

Focused bands:

```sh
./fkwu --src <(cat form/form-stdlib/core.fk \
  form/form-stdlib/source-artifact-cache.fk \
  form/form-stdlib/source-artifact-descriptor.fk \
  form/form-stdlib/program-image-fkb.fk \
  form/form-stdlib/tests/program-image-fkb-band.fk)
# -> 2147483647

./fkwu --src <(cat form/form-stdlib/core.fk \
  form/form-stdlib/form-fs.fk \
  form/form-stdlib/source-artifact-cache.fk \
  form/form-stdlib/source-artifact-descriptor.fk \
  form/form-stdlib/program-image-fkb.fk \
  form/form-stdlib/program-image-tbl-emit.fk \
  form/form-stdlib/tests/program-image-tbl-emit-band.fk)
# -> 2147483647
```

The 8i band proves:

- manifest scope and deferrals;
- exact 42 table text: `1 0 1 1 42 0 0 0\n`;
- exact negative integer text;
- exact string byte row text;
- exact numeric stream order: `nf`, roots, `nr`, node rows x4, `ns`, string
  rows as length plus bytes;
- malformed node arity and bad counts emit empty text;
- non-integer function roots and non-integer node cells emit empty text;
- bad seal and bad table envelopes emit empty text;
- valid writes return `1`, write the golden table text, and read back exactly;
- invalid writes return `0`, create no file on an absent path, and preserve a
  pre-existing sentinel file unchanged;
- the valid envelope still routes through the existing descriptor/cache policy;
- the grammar mirror is byte-identical to the stdlib file;
- the module text does not reference the unavailable binary/runtime call names.

External executor witness:

```text
/tmp/program-image-tbl-emit.tbl:
1 0 1 1 42 0 0 0

./fkwu /tmp/program-image-tbl-emit.tbl > /tmp/program-image-tbl-emit.out
first output line -> 42
output line count -> 257
```

The 257-line output is the current table runner's root value followed by its
arm counters. The important artifact witness is that the first value is `42`.

Downstream route revalidation:

```text
source-artifact-cache-band                  -> 1048575
source-artifact-descriptor-band             -> 2147483647
runtime-artifact-plan-band                  -> 67108863
runtime-artifact-selector-band              -> 2147483647
runtime-artifact-outcome-band               -> 2147483647
runtime-artifact-retry-band                 -> 2147483647
runtime-artifact-load-envelope-band         -> 2147483647
runtime-artifact-attempt-receipt-band       -> 2147483647
runtime-artifact-executor-capability-band   -> 2147483647
```

Static checks:

```text
cmp grammars/program-image-tbl-emit.fk form/form-stdlib/program-image-tbl-emit.fk -> 0
forbidden binary/runtime route scan over program-image-tbl-emit mirrors -> no hits
git diff --check -> clean
```

Investigation notes:

- The first 8i focused run returned `1879048191`, missing only the static
  scope bit. The guard was rejecting the manifest phrase
  `no-selector-install`, not executable behavior. The guard was narrowed to
  actual unavailable binary/runtime call names and the band returned the full
  mask.
- A first `runtime-artifact-load-envelope-band` run returned `1841823743`
  because the command omitted its declared `runtime-artifact-outcome.fk`
  prelude. Re-running with the band's declared prelude set returned
  `2147483647`. This was an invocation error, not a layer regression.
- A final Grok-style adversarial review found that `pif-table-valid?` checks
  table shape but not every numeric stream cell's type. A malformed row such
  as `(list 1 "bad" 0 0)` could render non-empty `.tbl` text. 8i now requires
  every `pif-table-numeric-stream` cell to have `value_kind == "int"` before
  rendering, and the band covers non-integer function roots and node cells.

## Deferred

- Real disk `.fkb` write/read.
- Binary program-image load/walk.
- Startup selector installation.
- Native `.dylib` loading/calling.
- Whole-file artifact hashing.
- 9f attempt production from real execution.
- 9h loader/executor.
- C-seed growth.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Jump straight to binary `.fkb` writing | Deferred | Current binary IO probes are red; using them would fabricate a round-trip. |
| Make 8h write files directly | Rejected | 8h is the semantic envelope; concrete artifact emission deserves its own guide surface and proof. |
| Parse existing `.tbl` text back into envelope rows | Deferred | The immediate need is to prove validated envelopes can create executor-compatible artifacts. Reverse parsing belongs to a separate scannerless table-ingest layer. |
| Install runtime selection now | Rejected | Selection already has policy layers, but no live loader/executor attempt producer. |

## Post-Review

Grok post-review verdict: `PASS`.

Grok accepted 8i as the narrow, honest bridge from validated 8h envelopes to
executor-compatible `.tbl` text. It noted that the external table run belongs
in the receipt rather than being hidden as a runtime-loader claim.

Claude post-review verdict: `PASS_WITH_CHANGES`.

Claude accepted the layer direction and scope but required one correction:
the receipt and band must not claim that an invalid write leaves no artifact at
the target path. The implementation is intentionally non-destructive: it
creates no new file when absent and preserves an existing file when present.
The band now proves both cases, and the receipt names the stronger
"no executable artifact exists at path" guarantee as not owned by this layer.

Final follow-up repairs:

- Claude/Sema-style final review blocked on residual old-framing language.
  The test bit, receipt, and architecture-map wording now use guide/scope
  language instead.
- Grok-style final review blocked on non-integer numeric-stream cells. 8i now
  refuses to render unless the stream is all integers.
- Focused validation after both repairs: `cd form && ./validate.sh
  form-stdlib/tests/program-image-tbl-emit-band.fk` -> `2147483647`, `1 ok,
  0 divergent`.
- Claude/Sema-style follow-up verdict after the guide-language repair: `PASS`.
- Grok-style follow-up verdict after the integer-stream repair: `PASS`.
- Both reviewers scoped the pass to 8i. The repo-wide dirty C-seed files remain
  accounted for through the C-seed guide receipt; 8i itself does not edit or
  depend on C-seed growth.
