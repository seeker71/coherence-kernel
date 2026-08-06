# 2026-07-04 -- source compiler fkb-file emission layer review

## Why This Layer Exists

Layer 8j still requires exact `.tbl` text because it is the stable emission row
used by the current table-text attempt lane. Layers 8h, 8h4, and 8h5 now prove
the program-image table payload can live as a canonical `.fkb` byte container
and as a bounded file witness. The next honest move is not to mutate 8j out from
under its consumers. It is to add a sibling emission receipt:

```text
compile-source request
  + source-compile-output intent
  + valid program-image PIF
  + ready 8h5 .fkb byte-file witness
  + optional native dylib descriptor
  -> source-compiler-fkb-file-emission row
```

This layer is 8j1. It closes compiler emission to a ready `.fkb` file witness.
It is not durable persistence, freshness admission, `.fkb` loading, execution,
selector installation, removal of 9h0, or C-seed growth.

## Pre-Review

Grok/Jason verdict: `PASS_WITH_CHANGES`.

Required changes:

- add a new row/prefix/predicate family; do not return `sce-emission`;
- do not make `sce-emission?` polymorphic;
- replay the 8h5 witness, not only its ready status;
- compare supplied PIF to witness-embedded PIF by descriptor fields plus
  canonical bytes;
- keep `sac-output-includes-tbl` as contained table payload, not a `.tbl`
  sidecar;
- add a general `fkb-witness-drift` reason;
- prove no table-text rendering or table-text attempt code is used.

Claude/Popper verdict: `PASS_WITH_CHANGES`.

Required changes:

- do not call `sce-emission-from-compile`;
- do not feed existing 8k/9h consumers yet;
- require ready witness status/reason, embedded container replay, supplied PIF
  match including content hash/artifact mtime, path coherence, write/size/window
  equality, readback bytes, and readback vouch;
- keep malformed inputs as `refused`, drift/policy issues as `investigate`;
- prove forged path, forged PIF, forged content hash/vouch, non-ready witness,
  and malformed witness cases.

## Implementation

Files:

- `form/form-stdlib/source-compiler-fkb-file-emission.fk`
- `grammars/source-compiler-fkb-file-emission.fk`
- `form/form-stdlib/tests/source-compiler-fkb-file-emission-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`

The new prefix is `scffe-`.

Row:

```text
("source-compiler-fkb-file-emission"
  compile-envelope
  compile-output
  pif-envelope
  fkb-witness
  dylib
  fkb-descriptor
  status
  reason)
```

Statuses:

- `emitted`
- `investigate`
- `refused`

The row is intentionally not an `sce-emission`. It is a sibling lane for the
`.fkb` file witness path.

Witness acceptance requires:

- `pifbf-witness?`;
- `file-witness-ready` status and reason;
- embedded 8h4 byte container replay through `pifbf-container-replay-ok?`;
- supplied PIF and embedded PIF match on source path/hash/mtime, artifact path,
  content hash, artifact mtime, seal bit, and canonical payload bytes;
- witness path, embedded PIF artifact path, derived fkb descriptor path,
  readback-window path, and readback-vouch path all match;
- write count, observed size, requested window length, observed window length,
  and byte length all match;
- readback bytes equal container bytes;
- readback content vouch matches the freshly computed supplied-byte content
  vouch, including target, path, evidence kind, expected hash, actual hash,
  status, observed size, and material size.

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
cd form && ./validate.sh form-stdlib/tests/source-compiler-fkb-file-emission-band.fk
-> 2147483647

direct fkwu concat from form/
-> 2147483647
```

The band proves:

- manifest boundaries;
- reason manifest coverage for all 24 reasons;
- happy `.fkb`-only emission;
- happy `.fkb` plus dylib emission;
- fkb route derivation from the PIF descriptor;
- the witness row is carried and the output is not an `sce-emission`;
- no table-text argument slot is used;
- compile-output no-fkb, missing contained table payload, and C-growth cases;
- malformed witness refusal;
- non-ready witness investigation;
- forged drift through write-count mismatch;
- forged readback-vouch evidence-kind drift;
- forged embedded PIF mismatch;
- forged witness path mismatch;
- forged content hash/readback vouch mismatch;
- malformed PIF refusal;
- bad PIF seal, bad PIF table, source mismatch, and stale PIF investigation;
- dylib not requested, missing requested dylib, malformed dylib descriptor,
  dylib source mismatch, and dylib policy failure;
- source/grammar mirror identity;
- static boundary: no `pite-table-text`, `sce-table-text-match`,
  `runtime-table-text`, `rtta-`, `pitw-`, `sce-emission-from-compile`,
  Form binary IO, or `fk_run` in the 8j1 source/mirror;
- neighbor 8h5 witness replay.

Static checks:

```text
cmp form/form-stdlib/source-compiler-fkb-file-emission.fk \
    grammars/source-compiler-fkb-file-emission.fk -> 0

forbidden scan over 8j1 source/mirror -> no hits
git diff --check over touched 8j1 and neighbor repair files -> clean
```

Neighbor checks after hygiene repairs:

```text
source-compiler-emission-band     -> 2147483647
source-compiler-persistence-band  -> 2147483647
program-image-fkb-byte-file-witness-band -> 2147483647
```

`source-compiler-file-persistence-band` still has neighbor drift: after
prelude/path hygiene and a mirrored over-close repair in
`source-compiler-file-persistence.fk`, sibling validation still returns closure
values in Go/Rust and a TS parse error. This layer does not depend on 8k1; the
remaining 8k1 drift is named for the next persistence adapter pass.

Later same-day note: the 8k1 drift above was subsequently repaired as a
band-only syntax, row-equality, and cwd path-hygiene correction in
`receipts/2026-07-04-source-compiler-file-persistence-layer-review.md`; no
semantic 8k1 source/grammar change was required.

## Investigation Notes

While checking neighbors, three older bands had multi-line prelude comments that
`validate.sh` only partially read, so they ran as `core+core+band`. The prelude
comments for `source-compiler-emission-band.fk`,
`source-compiler-persistence-band.fk`, and
`source-compiler-file-persistence-band.fk` were converted to single-line
prelude declarations.

The first rerun then exposed real drift:

- `source-compiler-emission-band` used repo-root `read_file` paths for static
  checks; Rust/TS returned null where Go tolerated it. The band now uses a
  root-or-form fallback reader.
- `source-compiler-persistence.fk` built `observed-fkb` before proving the fkb
  observation valid. Rust/TS correctly rejected the malformed observation case.
  The descriptor construction now happens only after the validity, presence,
  identity, seal, and contained-table checks pass. Source and grammar mirrors
  were updated together.
- `source-compiler-file-persistence.fk` and its grammar mirror had one extra
  final close. That over-close was removed, but the 8k1 band still has separate
  parse/closure drift recorded above.

The first 8j1 focused band returned `2013265919`, missing exactly the
reason-coverage bit. The missing reason was `malformed-fkb-witness`; the
coverage sample accidentally paired the malformed witness with a malformed PIF,
so the PIF reason won first. The sample was corrected and the band returned the
full mask.

No OOM or killed process occurred. The 8j1 validator took about 54 seconds; it
completed with sibling agreement, so this was observed cost rather than a
stall.

## Deferred

- Feeding 8j1 rows into 8k persistence.
- Replacing the 8j table-text row contract.
- Removing 8i/8i1/9h0 table-text bridge.
- Durable atomic persistence and freshness admission.
- Chunked `.fkb` file witnesses beyond one window.
- Program-image `.fkb` load/walk/execute.
- Runtime selector installation.
- Native `.dylib` loading/calling.
- C-seed growth.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Mutate 8j to accept fkb witnesses | Rejected | 8j has live consumers expecting `sce-emission` and table-text fields. |
| Return an `sce-emission` with witness stuffed into the table-text slot | Rejected | That would launder a different row language through an old accessor name. |
| Drop `includes-tbl` entirely | Rejected for this layer | The bit still means the program-image `.fkb` contains table-shaped payload sections. |
| Trust `pifbf-status-ready` alone | Rejected | Reviewers required replay of embedded bytes, window, sizes, and vouch. |
| Feed 8j1 directly to 8k | Deferred | A separate adapter/persistence review is needed so the existing 8k language is not blurred. |

## Post-Review

Claude/Popper returned `PASS_WITH_CHANGES`. The code path was accepted, but the
architecture map still overclaimed that the older
`source-compiler-file-persistence-band` was green. The map now names 8k1 as
remaining neighbor drift instead of claiming `2147483647`.

Grok/Jason returned `PASS_WITH_CHANGES`. The blocker was real: 8j1 replayed the
readback vouch's target/path/hash/size and required match status, but did not
compare the vouch evidence kind and status field-for-field against the freshly
computed `sai-vouch-content-bytes` row. That allowed a forged ready witness with
the wrong evidence language. The fix tightens `scffe-witness-vouch-ready?` to
compare target, path, evidence kind, expected hash, actual hash, status,
observed size, and material size against the expected supplied-byte content
vouch. The focused band now includes a forged evidence-kind witness that must
investigate as `fkb-witness-drift`.

Post-fix verification:

```text
cd form && ./validate.sh form-stdlib/tests/source-compiler-fkb-file-emission-band.fk
-> 2147483647

cmp form/form-stdlib/source-compiler-fkb-file-emission.fk \
    grammars/source-compiler-fkb-file-emission.fk -> 0
```

Final narrow re-review:

- Claude/Popper: `PASS`; architecture-map overclaim is closed and the receipt
  remains honest about the deferred 8k1 drift.
- Grok/Jason: `PASS`; the vouch replay blocker is closed and the forged
  evidence-kind witness now investigates as `fkb-witness-drift`.

The exchange stayed alive by turning the `.tbl` question into a concrete
sibling compiler-emission lane: `.fkb` file witness is now sufficient for an
8j1 emission row, while the old table-text lane remains explicitly separate.
