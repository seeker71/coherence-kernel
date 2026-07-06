# 2026-07-04 -- source compiler emission receipt layer review

## Why This Layer Exists

Layers 8h and 8i made the program-image `.fkb` payload concrete and gave the
current table executor an exact `.tbl` text bridge. Layer 9e can produce a
request-ready compile-source envelope, and Layer 9a already has a
`sac-compile-output` intent row that says a source compile should write a
program-image `.fkb`, include the table payload, and maybe write a native
`.dylib`.

The missing layer was the receipt between those facts:

```text
compile-source request
  + sac compile-output intent
  + validated program-image .fkb envelope
  + exact table-text witness
  + optional native dylib descriptor
  -> source-compiler-emission receipt
```

This is observational only. It validates consistency among caller-supplied
rows. It does not parse, compile, write binary artifacts, load artifacts,
execute table text, install a selector, or grow the C seed.

## Pre-Review

Grok pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Grok:

- say explicitly that this layer is observational and does not prove compile,
  disk write, or load occurred;
- treat the no-dylib happy path as `compile-emission-ready`, and make a
  supplied-but-unrequested dylib a distinct diagnostic branch;
- delegate freshness, seal/proof/callable, and current checks to existing
  `sad-*`/route helpers rather than re-encoding selector eligibility;
- require a real `sac-compile-output`-shaped row and refuse missing or
  inconsistent output intent;
- extend static scans beyond the older binary/runtime names.

Claude pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Claude:

- close reason-manifest gaps by distinguishing stale program-image envelopes,
  bad seals, bad tables, and native policy failures instead of overloading one
  reason;
- derive the `fkb-descriptor` output from the validated 8h envelope, not from a
  caller-supplied descriptor;
- state that current/freshness checks are pure field consistency over supplied
  rows, not filesystem truth;
- extend forbidden-name scans to include native symbol and raw file IO names;
- define the supplied-but-unrequested dylib case explicitly.

## Implementation

Files:

- `form/form-stdlib/source-compiler-emission.fk`
- `grammars/source-compiler-emission.fk`
- `form/form-stdlib/tests/source-compiler-emission-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`

The new prefix is `sce-`.

Receipt row:

```text
("source-compiler-emission" compile-envelope compile-output
  pif-envelope table-text dylib fkb-descriptor status reason)
```

The `fkb-descriptor` field is always derived from the 8h program-image
envelope by `pif-descriptor-from-envelope`. It is not an input.

Statuses:

- `emitted`
- `investigate`
- `refused`

Reason coverage is explicit through `sce-reason-manifest`, and the band checks
it with `rcov-coverage` over actual `sce-emission-from-compile` invocations.

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
  form/form-stdlib/source-artifact-cache.fk \
  form/form-stdlib/source-artifact-descriptor.fk \
  form/form-stdlib/runtime-artifact-plan.fk \
  form/form-stdlib/runtime-artifact-selector.fk \
  form/form-stdlib/runtime-artifact-outcome.fk \
  form/form-stdlib/runtime-artifact-retry.fk \
  form/form-stdlib/runtime-artifact-load-envelope.fk \
  form/form-stdlib/program-image-fkb.fk \
  form/form-stdlib/program-image-tbl-emit.fk \
  form/form-stdlib/reason-coverage.fk \
  form/form-stdlib/source-compiler-emission.fk \
  form/form-stdlib/tests/source-compiler-emission-band.fk)
# -> 2147483647
```

The band proves:

- manifest boundaries and deferrals;
- explicit reason manifest with structural `rcov-coverage`;
- compile-source request + `sac-compile-output` + valid 8h envelope + exact
  8i table text -> `emitted`;
- requested native dylib descriptor can be emitted when existing `sad-route`
  policy accepts the fkb+dylib bundle;
- the fkb descriptor is derived from the program-image envelope and can route
  through existing `sad-route`;
- malformed compile envelope, non-compile envelope, malformed output, malformed
  pif envelope, and malformed dylib descriptor refuse;
- missing fkb write intent, missing table inclusion, C-growth output flag, bad
  pif seal, bad pif table, stale pif mtime, source mismatch, table-text
  mismatch, unrequested dylib, missing requested dylib, dylib source mismatch,
  and dylib policy failure investigate;
- mirror parity;
- forbidden-name static scan over the source file.

Investigation note: the first focused run returned `1945894932`, caused by test
manifest bits summing the same bit repeatedly and carrying through the mask.
After fixing the bit arithmetic, the band returned `1945894911`, missing only
the stale pif and reason-coverage bits. That exposed a real implementation
error: `sce-pif-current?` used `sad-program-image-fkb-current?`, which does not
check mtime freshness. The fix delegates freshness to `sad-route`; the band
then returned the full mask.

## Deferred

- Actual source parsing or compilation.
- Disk `.fkb` write/read.
- Disk `.dylib` write/read.
- Filesystem stat/hash freshness truth.
- Table execution.
- Program-image load/walk.
- Native dylib load/call.
- Selector installation.
- C-seed growth.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Put compile emission into 9e load envelopes | Rejected | 9e intentionally does not consume `rap-plan` or emit compiler output; it only names request readiness. |
| Let callers pass an fkb descriptor directly | Rejected | The receipt must prove the descriptor was derived from the 8h envelope, not asserted independently. |
| Re-implement freshness locally | Rejected | The first run proved this is error-prone; freshness belongs to `sad-route`/`sac-route`. |
| Claim binary `.fkb` persistence now | Rejected | Current grounded probes still do not support an honest binary artifact round-trip. |

## Post-Review

Grok post-review verdict: `PASS`.

Grok required changes: none.

Grok accepted the layer as satisfying the pre-review contract: the header and
manifest keep the layer observational, the no-dylib and with-dylib happy paths
are distinct, supplied-but-unrequested dylib is investigated, compile-output
shape is pinned to the `source-compile-output` row, the fkb descriptor is
derived from the 8h envelope, pif and dylib policy delegate through
`sad-route`, and the focused plus neighboring witnesses all pass.

Claude post-review verdict: `PASS`.

Claude required changes: none.

Claude accepted all pre-review requirements as satisfied, including the
19-reason manifest and `rcov-coverage` over real
`sce-emission-from-compile` invocations. Claude also called out the stale-pif
investigation as the important proof of the layer: the first correct bitmask
run exposed that `sad-program-image-fkb-current?` does not check mtime
freshness, and the final implementation moved that responsibility to
`sad-route`, where it belongs.

Claude noted one non-blocking reader concern: refused rows still carry the
derived fkb field. That field is produced only by `pif-descriptor-from-envelope`;
for invalid pif envelopes it is `sad-no-artifact`, not a caller-supplied claim.
No code change was required.
