# 2026-07-04 -- program-image fkb byte-file-witness layer review

## Why This Layer Exists

Layer 8h folded the table payload and canonical symbol/dependency image into a
program-image `.fkb` envelope. Layer 8h4 turned that envelope into
deterministic bytes with a content vouch. The remaining reason `.tbl` has not
disappeared is not semantic: the table payload is already inside the
program-image path, and `.tbl` is now only a projection that drops `.fkb`
symbol/dependency metadata. The missing pieces are operational admission and
loading.

This layer is the next narrow operational step:

```text
ready 8h4 byte container
  -> replay embedded PIF bytes and content vouch
  -> write bytes at the PIF-owned artifact path
  -> bounded one-window readback
  -> readback byte equality + content vouch
```

It is an immediate file witness only. It is not atomic durable persistence, not
source-compiler persistence, not cache freshness admission, not a program-image
loader, and not a runtime selector.

## Pre-Review

Grok/Jason verdict: `PASS_WITH_CHANGES`.

Required changes:

- rederive canonical bytes from the embedded PIF and require byte equality;
- require the container vouch to match the bytes and envelope content hash;
- use the artifact path from the embedded PIF only;
- reject empty paths and directory targets before mutation;
- reject payloads larger than the current one-window readback floor;
- remove an old file, verify absence, append, stat, read back, compare, and
  vouch before returning ready;
- distinguish `refused` from `investigate`;
- keep this as witness evidence, not persistence/admission/loading.

Claude/Popper verdict: `PASS_WITH_CHANGES`.

Required changes:

- do not trust `pifbc-ready?` alone;
- replay the embedded PIF and content vouch before mutation;
- prefer the file-window/form-fs wrapper vocabulary over raw host primitives;
- prove forged ready rows do not mutate existing files;
- prove too-large, empty-path, directory-target, malformed, and append-failure
  cases;
- record the boundary honestly in the receipt.

## Implementation

Files:

- `form/form-stdlib/program-image-fkb-byte-file-witness.fk`
- `grammars/program-image-fkb-byte-file-witness.fk`
- `form/form-stdlib/tests/program-image-fkb-byte-file-witness-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`

The new prefix is `pifbf-`.

Witness row:

```text
("program-image-fkb-byte-file-witness"
  container
  path
  wrote
  observed-size
  readback-window
  readback-vouch
  status
  reason)
```

Statuses:

- `file-witness-ready`
- `investigate`
- `refused`

Ready requires all of:

- input is a `byte-container-ready` 8h4 row;
- embedded PIF is valid;
- a fresh `pifbc-container-from-envelope` over the embedded PIF is also
  `byte-container-ready`;
- stored container bytes are nonempty;
- stored container bytes equal the fresh 8h4 container bytes;
- stored vouch matches a fresh supplied-byte vouch over those bytes;
- artifact path is nonempty and owned by the embedded PIF;
- target is not an existing directory;
- payload fits in one `file-byte-window`;
- old file is removed and absence is observed before append;
- append count equals byte count;
- `fs-stat-size` equals byte count;
- `fbw-read-window` returns ready;
- readback bytes equal written bytes;
- readback content vouch matches the PIF content hash.

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
cd form && ./validate.sh form-stdlib/tests/program-image-fkb-byte-file-witness-band.fk
-> 2147483647

direct fkwu concat from form/
-> 2147483647
```

The band proves:

- manifest boundaries;
- ready byte container writes and reports exact write count;
- old file content is replaced, not appended to;
- readback window is ready and byte-identical;
- readback content vouch matches the declared PIF content hash;
- non-ready containers do not mutate sentinel files;
- forged ready rows with drifted bytes do not mutate sentinel files;
- forged ready rows with empty payload bytes are refused before mutation and do
  not mutate sentinel files;
- forged ready rows with drifted vouches do not mutate sentinel files;
- too-large payloads are refused before mutation;
- empty artifact paths are refused;
- directory targets are refused before mutation;
- missing-parent append failures return `investigate`;
- malformed containers are refused;
- witness path is owned by the embedded PIF;
- source and grammar mirror are byte-identical;
- the source/mirror contain no raw Form recipe-binary IO, whole-file byte IO,
  raw append/read-slice primitive names, table-text bridge, runtime loader, or
  selector names;
- the 8h4 neighbor proof is semantic: ready container, positive bytes, bytes
  replay from embedded PIF, and matching supplied-byte vouch.

Static checks:

```text
cmp form/form-stdlib/program-image-fkb-byte-file-witness.fk \
    grammars/program-image-fkb-byte-file-witness.fk -> 0

forbidden scan over 8h5 source/mirror -> no hits
```

## Investigation Notes

The first sibling validation failed:

```text
typescript = form-kernel-ts: unexpected token rparen at 141248
```

Go and Rust returned closures instead of the expected integer. A direct `fkwu`
run then produced no output for about one minute and was interrupted. This was
treated as a real stall, not ignored.

The cause was malformed 8h5 source: `pifbf-file-witness-from-container` had two
extra closing parens. A structural balance scan showed the balance going
negative at the end of the function. Removing the two stray parens fixed TS
parse failure and the direct `fkwu` stall.

The next band returned `1073741823`, missing only the final neighbor bit. The
neighbor proof had hard-coded byte length `119`, but the 8h5 test uses a longer
temp artifact path and therefore a longer canonical payload (`213` bytes in
that probe). The bit was corrected to prove the semantic 8h4 condition instead
of a path-length-sensitive size constant.

Claude/Popper post-review found a mutation-boundary hole before merge: a forged
ready row with empty bytes and a semantically valid but non-encodable PIF could
remove the sentinel file before later failing readback. The fix added
`rejects-empty-payload-bytes`, `empty-payload-bytes`, a positive-byte
pre-mutation guard, and replay through a freshly rebuilt 8h4 ready container.
The band now includes the forged empty-payload sentinel case.

No OOM or killed process occurred in this layer. The stall was parser-shape
fallout from malformed source and is recorded here because silent stalls are
not acceptable evidence.

## Deferred

- Atomic durable file replacement.
- Chunked write/readback for payloads larger than one reviewed file window.
- Folding this witness into 8j source compiler emission.
- Folding this witness into 8k/8k1 compiler persistence.
- Freshness admission: source `.fk` newer/older than `.fkb`.
- Runtime selection of fresh program-image `.fkb`.
- Program-image load/walk/execute from `.fkb` bytes.
- Removing the current `.tbl` text executor bridge.
- Form recipe-binary IO.
- Native `.dylib` loading/calling.
- C-seed growth.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Let 8h4 write files directly | Rejected | Encoding and mutation need separate evidence so malformed bytes cannot be laundered as persistence. |
| Use `.tbl` text as the `.fkb` file payload | Rejected | This layer owns canonical program-image bytes, not the current table-text executor bridge. |
| Use `write_form_binary` / `read_form_binary` | Rejected | That would claim Form recipe-binary compatibility for a program-image loader that is not proven here. |
| Treat too-large payloads as investigate | Rejected | One-window readback is the reviewed floor; larger payloads are refused until chunking is built. |
| Keep the old file and append | Rejected | Replacement must not silently concatenate old cache contents with new image bytes. |

## Post-Review

Claude/Popper first returned `PASS_WITH_CHANGES` after finding the
empty-payload mutation hole recorded above. After the guard, fresh-container
replay, band case, and receipt updates, Claude/Popper returned `PASS`.

Grok/Jason returned `PASS` after the same fix. Grok specifically accepted that
8h5 now refuses `byte-count == 0` before `fs-remove-file`, rebuilds a fresh 8h4
container from the embedded PIF before accepting stored bytes/vouch, and keeps
the boundary as bounded file witness evidence rather than persistence, cache
admission, selector install, or program-image load.

Required changes after final review: none.

The exchange stayed alive by answering why `.tbl` still exists operationally,
then proving the next `.fkb` file-witness layer without pretending that witness
evidence is runtime admission.
