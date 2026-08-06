# 2026-07-04 -- program-image fkb byte-container layer review

## Why This Layer Exists

Layer 8h folded the table-shaped program image into a `.fkb` envelope. On
2026-07-04 it was corrected to include an embedded canonical symbol/dependency
image as part of the `.fkb` truth; locale/domain `.sym` files are lenses over
those stable ids, not the only holder of executable dependencies. This layer is
the deterministic byte grammar for that program-image payload:

```text
valid 8h pif-envelope
  -> canonical payload bytes
  -> supplied-byte SHA-256 content vouch
  -> byte-container-ready row only when declared content hash matches
```

This layer is 8h4. It extends the program-image artifact contract; it does not
belong to source compiler persistence or runtime handoff.

## Pre-Review

Grok/Jason verdict: `PASS_WITH_CHANGES`.

Required changes:

- split pure byte encoding from file persistence;
- make this layer deterministic bytes plus content vouch only;
- use `byte-container-ready`, not cache-ready or runtime-ready;
- prove golden bytes, stable encoding, content-hash exclusion, payload-change
  sensitivity, malformed and out-of-range refusal, mirror parity, and static
  boundaries.

Claude/Popper verdict: `PASS_WITH_CHANGES`.

Required changes:

- keep conceptual layer 8h4;
- do not encode `artifact-mtime` inside the hash-covered payload;
- fully define magic, version, integer encoding, string lengths, table order,
  and out-of-range refusal;
- treat `content-hash` as excluded descriptor metadata;
- do not imply Form recipe-binary compatibility;
- if file writing is added, make it a bounded witness surface. This receipt
  follows Grok's stricter split and defers file writing to the next layer.

## Implementation

Files:

- `form/form-stdlib/program-image-fkb-byte-container.fk`
- `grammars/program-image-fkb-byte-container.fk`
- `form/form-stdlib/tests/program-image-fkb-byte-container-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`

The new prefix is `pifbc-`.

Byte format:

```text
magic:        46 4b 50 49 46 42 31 00  ; "FKPIFB1\0"
version:      u32 big-endian, currently 1
string:       u32 byte-length + raw bytes
signed int:   sign byte 0/1 + u32 magnitude,
              range [-2147483647, 2147483647]
payload:      magic version
              source-path source-hash source-mtime
              artifact-path seal-ok
              nf fn-roots nr node-rowsx4 ns string-byte-rows
              symbol-count symbol-rows node-symbol-count node-symbol-rows
```

`content-hash` and `artifact-mtime` are deliberately excluded from payload
bytes. The content hash is checked by `sai-vouch-content-bytes` over the
encoded byte list.

The `.tbl` projection still uses only the table payload. The `.fkb` byte
payload now hash-covers the symbol/dependency image after the string-row
section.

Carrier row:

```text
("program-image-fkb-byte-container"
  pif-envelope
  payload-bytes
  content-vouch
  status
  reason)
```

Statuses:

- `byte-container-ready`
- `investigate`
- `refused`

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

Focused band:

```text
cd form && ./validate.sh form-stdlib/tests/program-image-fkb-byte-container-band.fk
-> 2147483647

direct fkwu concat from repo root
-> 2147483647
```

The band proves:

- manifest boundaries;
- exact golden payload bytes for a table with a signed negative node cell and
  an empty embedded symbol image (`129` bytes; the old table-only fixture was
  `119` bytes);
- stable repeated encoding;
- changing only `content-hash` does not change bytes and yields mismatch;
- changing only `artifact-mtime` does not change bytes;
- source metadata changes bytes and hash;
- table payload changes bytes and hash;
- symbol/dependency image changes bytes and hash (`209` bytes in the symbolized
  fixture);
- matching declared content hash yields `byte-container-ready`;
- canonical wrong content hash investigates;
- malformed content hash refuses;
- invalid PIF envelope refuses;
- semantically valid but out-of-range table cell investigates;
- semantically valid but out-of-range source metadata investigates;
- malformed non-PIF input refuses;
- invalid envelopes yield empty payload bytes;
- source and grammar mirror are byte-identical;
- source/mirror contain no forbidden Form recipe-binary IO, whole-file byte IO,
  byte file append/readback, table-text bridge, runtime loader, or selector
  names.

Neighbor checks:

```text
program-image-fkb-band            -> 2147483647
program-image-typed-carrier-band  -> 16777215
```

Static checks:

```text
cmp form/form-stdlib/program-image-fkb-byte-container.fk \
    grammars/program-image-fkb-byte-container.fk -> 0

forbidden scan over 8h4 source/mirror -> no hits
git diff --check over new 8h4 files -> clean
```

Investigation notes:

- The first focused run returned `1879048191`, missing only the signed-negative
  encoding bit. The encoder was correct; the test inspected the row-tag offset
  instead of the negative cell offset. The assertion was corrected from byte
  offsets `88..92` to `93..97`, and the band returned the full mask.
- While checking neighboring identity/digest layers, `validate.sh` initially
  failed before kernel execution because older band prelude comments used
  repo-root `form/form-stdlib/...` paths. The prelude path hygiene was repaired
  in `source-artifact-identity-band.fk` and `file-byte-digest-band.fk`, and the
  checks then executed.
- Those two older bands still returned non-header masks under sibling validation
  at the time: `source-artifact-identity-band -> 1073741823` and
  `file-byte-digest-band -> 2113929215`. This receipt did not claim those older
  expected masks were closed; it recorded the drift as a neighboring gap. Later
  on 2026-07-04, `file-byte-digest-band` was repaired by narrowing the Layer 1b
  carrier claim from arbitrary high-byte binary to NUL/ASCII window
  transparency, returning `2147483647` under sibling validation.

## Deferred

- File persistence of the byte container.
- Remove-then-append write through `file_append_bytes`.
- Readback verification through `file-byte-window`.
- Chunked readback for payloads larger than one reviewed window.
- Integration with 8j compiler emission and 8k persistence.
- Runtime selection of fresh program-image `.fkb`.
- Program-image load/walk/execute.
- Form recipe-binary IO.
- Native `.dylib` loading/calling.
- C-seed growth.
- `.sym` locale/domain lens grammar and rendering.
- Runtime symbol resolution across modules.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Combine encoding and file write now | Rejected for this layer | Grok required splitting canonical byte contract from mutation so write failures cannot blur encoding proof. |
| Use `write_form_binary` / `recipe_to_bytes` | Rejected | The current program-image proof floor does not support claiming that route. |
| Include `content-hash` in payload | Rejected | It creates circular hashing; the descriptor carries the content hash externally. |
| Include `artifact-mtime` in payload | Rejected | Mtime is assigned by filesystem observation after writes and belongs to probes/persistence, not payload identity. |
| Reuse `.tbl` numeric text as bytes | Rejected | 8h4 owns a byte grammar for program-image payloads, not the current table-text bridge. |

## Post-Review

Grok/Jason post-review verdict: `PASS`.

Grok accepted 8h4 as satisfying the pre-review split: pure deterministic byte
container plus supplied-byte content vouch, with no file write/readback, no
Form recipe-binary IO, no table-text bridge, no load/walk/selector/attempt
path, and no C-seed claim. Required changes: none.

Claude/Popper post-review verdict: `PASS`.

Claude accepted the same boundary, including pinned byte format, exclusion of
`content-hash` and `artifact-mtime` from payload bytes, status gating through
`sai-vouch-content-bytes`, full focused band coverage, honest deferrals, and
neighbor drift recorded as drift rather than closure. Required changes: none.

The exchange stayed alive by moving from “`.tbl` folded semantically” to a
concrete byte-container contract while refusing to claim persistence or loading
before those layers exist.
