# 2026-07-04 -- program-image fkb byte-decode layer review

## Why This Layer Exists

Layer 8h4 now produces canonical `.fkb` payload bytes for a program-image
envelope. Layer 8h5 proves those bytes can be written and read back through the
current one-window file floor. The next narrow step is not runtime loading. It
is byte admission:

```text
8h4 canonical payload bytes
  -> cursor-style decode into encoded payload fields
  -> rebuild table + embedded symbol/dependency image
  -> require EOF and canonical re-encode equality
  -> admit only matching 8h5 readback witnesses
```

This layer also records the corrected `.fkb`/`.sym` boundary: `.fkb` carries
the executable table plus canonical symbol/dependency truth. `.sym` is a
locale/domain lens over stable symbol ids; it is not the only place executable
symbol dependencies live.

## Pre-Review

Grok/Jason verdict: `PASS_WITH_CHANGES`.

Required changes:

- keep this as Layer 8h6, before runtime load/execution;
- split pure `bytes -> decoded-payload` from witness admission;
- decode only the hash-covered payload fields, not content hash or artifact
  mtime;
- use `fbw-window-bytes (pifbf-witness-window w)` as the admission byte source;
- reject malformed/nonready witnesses, nonready windows, size drift, byte drift,
  vouch drift, decoded metadata drift, and container replay drift;
- require explicit EOF and canonical decode->encode equality;
- reject bad magic/version, truncated fields, malformed signed ints, negative
  counts, impossible counts, invalid rows, and tail garbage;
- keep table text, 9f/9c attempts/observations, selector install, Form binary
  IO, runtime load, and C growth out.

Claude/Popper verdict: `PASS_WITH_CHANGES`.

Required changes:

- keep decoder and admission separate;
- do not reconstruct a full PIF from bytes alone; inject/check external
  metadata during witness admission;
- check witness ready status, window ready status, wrote/observed size,
  readback vouch, readback bytes, and canonical bytes;
- keep the manifest vocabulary as decoder/admission, not loader;
- statically forbid table-text bridge/runtime names and native/selector/load
  surfaces.

User correction during implementation:

- `.fkb` must not rely on `.sym` for executable dependency truth. It must embed
  node-level symbol/dependency data. `.sym` remains locale/domain presentation.

## Implementation

Files:

- `form/form-stdlib/program-image-fkb.fk`
- `grammars/program-image-fkb.fk`
- `form/form-stdlib/program-image-fkb-byte-container.fk`
- `grammars/program-image-fkb-byte-container.fk`
- `form/form-stdlib/program-image-fkb-byte-decode.fk`
- `grammars/program-image-fkb-byte-decode.fk`
- `form/form-stdlib/tests/program-image-fkb-band.fk`
- `form/form-stdlib/tests/program-image-fkb-byte-container-band.fk`
- `form/form-stdlib/tests/program-image-fkb-byte-decode-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`

The base 8h image now includes:

```text
("program-image-symbol-image"
  symbol-count
  symbol-rows
  node-symbol-count
  node-symbol-rows)
```

Symbol rows are ordered stable ids with canonical keys:

```text
(symbol-id canonical-key)
```

Node symbol rows are ordered by node id:

```text
(node-id defined-symbol-id dependency-symbol-ids)
```

`defined-symbol-id` may be `-1` for an anonymous node that still has symbol
dependencies. Dependency ids must reference embedded `.fkb` symbols. Locale and
domain names belong to `.sym` lenses over these ids.

Layer 8h4 now hash-covers the symbol image after the table section:

```text
magic version
source-path source-hash source-mtime
artifact-path seal-ok
nf fn-roots nr node-rowsx4 ns string-byte-rows
symbol-count symbol-rows node-symbol-count node-symbol-rows
```

Layer 8h6 adds prefix `pifbd-` and two rows:

```text
("program-image-fkb-byte-decode" payload next status reason)

("program-image-fkb-byte-admission" witness decode pif status reason)
```

`pifbd-decode-bytes` is pure and only returns decoded payload fields. It does
not decode content hash or artifact mtime because 8h4 deliberately excludes
those from hash-covered payload bytes. `pifbd-admit-witness` injects them back
only after matching the decoded payload against the embedded PIF and the 8h5
readback evidence.

The first decoder body was a giant nested form and caused the exact low-level
source-shape risk this review is meant to remove. It was replaced with staged
helpers by semantic section: metadata, table roots, node rows, string rows, and
symbol rows.

## Witnesses

Required floor before implementation:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
# known fread/getsockname warnings only
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
native-vs-rented-check -> 11111
```

Focused bands:

```text
cd form && ./validate.sh form-stdlib/tests/program-image-fkb-band.fk
-> 2147483647

cd form && ./validate.sh form-stdlib/tests/program-image-fkb-byte-container-band.fk
-> 2147483647

cd form && ./validate.sh form-stdlib/tests/program-image-fkb-byte-decode-band.fk
-> 268435455

direct fkwu concat for 8h6 band
-> 268435455
```

Neighbor checks:

```text
cd form && ./validate.sh form-stdlib/tests/program-image-fkb-byte-file-witness-band.fk
-> 2147483647

cd form && ./validate.sh form-stdlib/tests/program-image-tbl-emit-band.fk
-> 2147483647

cd form && ./validate.sh form-stdlib/tests/source-compiler-fkb-file-emission-band.fk
-> 2147483647
```

The 8h6 band proves:

- decoder manifest boundaries;
- empty embedded symbol image decodes canonically;
- nonempty symbol/dependency image decodes canonically and changes bytes;
- EOF is required and tail garbage refuses;
- bad magic, bad version, truncated magic, and truncated length refuse with
  explicit reasons;
- reason manifest includes `invalid-symbol-image`;
- ready 8h5 file witnesses admit;
- nonready witnesses investigate;
- malformed witnesses refuse;
- malformed embedded containers investigate;
- nonready readback windows investigate before byte access;
- readback origin drift investigates when witness path, window path, or window
  offset do not match the PIF artifact path and offset zero;
- write/readback size drift investigates;
- readback vouch drift investigates;
- decoded source metadata drift investigates;
- decoded symbol/dependency image drift investigates with
  `decoded-symbol-mismatch`;
- source and grammar mirror are byte-identical;
- the source/mirror contain no Form recipe-binary IO, raw byte-file IO,
  table-text bridge names, runtime-table attempt names, `fk_run`, or native
  loader names.

Static checks:

```text
cmp form/form-stdlib/program-image-fkb.fk \
    grammars/program-image-fkb.fk -> 0

cmp form/form-stdlib/program-image-fkb-byte-container.fk \
    grammars/program-image-fkb-byte-container.fk -> 0

cmp form/form-stdlib/program-image-fkb-byte-decode.fk \
    grammars/program-image-fkb-byte-decode.fk -> 0
```

## Investigation Notes

During implementation, a direct `fkwu --src` load of the first 8h6 source
shape produced no output after about 30 seconds and was interrupted. This was
not OOM and not a killed process. It was investigated as a real stall.

The first paren-depth script was itself wrong because it counted string
literals as closing parens; rerunning with string/comment matches skipped
showed the real source shape. The original `pifbd-decode-bytes` body was a
large nested form with extra closing parens near the table/string boundary.
That form was replaced with staged helpers. After the rewrite, paren depth
returned to zero and direct source loading returned `1` for the manifest probe.

The first 8h4 symbol-section edit also produced a source-load stall. A balance
trace showed two extra closing parens at the byte-container tail. Removing
those parens restored source loading. This is recorded because stalls and
silent hangs are not soft failures.

The first 8h6 focused band returned `268173311`, missing exactly `262144`.
The decoder was correct; the band expected `invalid-table` and
`invalid-symbol-image` at the old reason indices. Updating those indices after
adding the new reason returned the full mask.

No OOM-killed process occurred in this layer. The observed stalls were
source-shape failures and were investigated before continuing.

## Deferred

- Runtime load/walk/call from `.fkb`.
- Producing 9f attempt receipts or 9c observations.
- Cache freshness admission based on source `.fk` vs `.fkb`.
- Chunked decode/readback for payloads larger than the reviewed one-window
  floor.
- `.sym` file grammar, locale packs, aliases, documentation, and domain
  presentation.
- Symbol resolution policy across multi-file/module dependencies.
- Removing the `.tbl` text executor bridge.
- Native `.dylib` loading/calling.
- C-seed growth.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Keep symbols only in `.sym` | Rejected | A locale sidecar cannot be the only holder of executable dependency truth; `.fkb` must remain self-describing enough to load and reason without reparsing source. |
| Put symbol ids directly into the table node x4 rows | Rejected for now | The x4 row remains the exact table executor projection. The symbol image is embedded in `.fkb` beside the table so `.tbl` compatibility stays a derived projection. |
| Decode `.tbl` text as the runtime path | Rejected | This layer owns canonical `.fkb` bytes and witness admission, not the legacy table-text bridge. |
| Treat the 8h5 witness container bytes as source of truth | Rejected | Admission uses `fbw-window-bytes` from the readback window, then checks against the container, vouch, and decoded payload. |
| Include content hash and artifact mtime in decoded bytes | Rejected | 8h4 explicitly excludes them from the hash-covered payload to avoid circular hashes and filesystem-time coupling. |

## Post-Review

Grok/Jason post-review verdict: `PASS`.

Grok accepted that `.fkb` now embeds canonical symbol/dependency rows while
`.sym` remains a locale/domain lens, `.tbl` remains a projection, and 8h6 stays
decode/admission only.

Claude/Popper post-review verdict: `PASS_WITH_CHANGES`.

Finding:

- `pifbd-admit-witness` checked ready status and sizes before decoding
  `fbw-window-bytes`, but did not require `pifbf-witness-path`,
  `fbw-window-path`, and `fbw-window-offset` to match the embedded PIF artifact
  path and offset `0`. A forged witness with correct bytes/vouch but a false
  path/offset could still admit.

Fix:

- Added `pifbd-reason-readback-origin-mismatch`.
- Added `pifbd-witness-origin-ok?`.
- Admission now investigates before byte access unless witness path and window
  path equal the PIF artifact path and window offset is `0`.
- Added `pifbdb-forged-origin-witness` to the focused band under the existing
  window-origin bit.

Post-fix evidence:

```text
cd form && ./validate.sh form-stdlib/tests/program-image-fkb-byte-decode-band.fk
-> 268435455
```

Grok/Jason follow-up verdict: `PASS`.

Grok confirmed the origin gap is closed and the code/test/receipt evidence now
match.

Claude/Popper receipt-only follow-up verdict: `PASS_WITH_CHANGES`, with the
only remaining change being the stale follow-up placeholder. This line is now
updated to the actual verdict; the architecture duplicate-summary finding is
closed.

Claude/Popper final follow-up verdict: `PASS`.
