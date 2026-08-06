# 2026-07-04 -- source compiler persistence attestation layer review

## Why This Layer Exists

Layer 8j can say a compile-source request emitted a validated program-image
`.fkb` envelope, exact table-text witness, and optional native dylib descriptor.
That still does not say a post-write artifact observation matches the emission
or that downstream cache/probe routing may consume it.

This layer adds that missing attestation:

```text
source-compiler-emission
  + supplied source-artifact-probe bundle
  -> persistence-ready descriptor triple
```

The status is deliberately `persistence-ready`, not `persisted`. The layer
does not assert durable filesystem truth. It binds supplied observations to an
emission and hands the resulting descriptor triple to the cache/probe lane.

## Pre-Review

Grok pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Grok:

- make the status wording observational rather than claiming durable
  persistence;
- pin the public entry point, row shape, and compared fields;
- state the downstream handoff as a descriptor triple for cache/probe routing;
- build test bundles through existing `sap-*` constructors;
- assert route outcomes only through `sap-descriptor-triple` and `sad-route`.

Claude pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Claude:

- remove exact mtime matching from 8k; freshness/currency belongs to
  `sad-route`;
- add a distinct `dylib-route-not-current` reason;
- rename the happy status to `persistence-ready`;
- specify the descriptor-triple slot for investigate/refused rows;
- confirm fresh prefix and reason vocabulary before implementation.

The frequency check over `scp-`, `source-compiler-persistence`, and the new
reason strings returned no hits before edits.

## Implementation

Files:

- `form/form-stdlib/source-compiler-persistence.fk`
- `grammars/source-compiler-persistence.fk`
- `form/form-stdlib/tests/source-compiler-persistence-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`

The new prefix is `scp-`.

Public entry:

```text
scp-persistence-from-probe(emission, sap-bundle)
```

Receipt row:

```text
("source-compiler-persistence" emission bundle descriptor-triple status reason)
```

The descriptor triple is `sap-descriptor-triple` only for `persistence-ready`
rows. Investigate and refused rows carry a sentinel triple:

```text
(sad-source "" "" 0), sad-no-artifact, sad-no-artifact
```

Compared fields:

- source observation: role, presence, path, source hash;
- fkb observation: role, presence, path, source hash, content hash, seal bit,
  includes-tbl bit;
- native observation when expected: role, presence, path, source hash, content
  hash, seal/proof/callable/lowerable policy fields.

Mtime is intentionally not compared directly. Currency is checked only through
`sad-route` over the descriptor triple derived from the supplied observations.

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
  form/form-stdlib/source-artifact-probe.fk \
  form/form-stdlib/runtime-artifact-selector.fk \
  form/form-stdlib/runtime-artifact-outcome.fk \
  form/form-stdlib/runtime-artifact-retry.fk \
  form/form-stdlib/runtime-artifact-load-envelope.fk \
  form/form-stdlib/program-image-fkb.fk \
  form/form-stdlib/program-image-tbl-emit.fk \
  form/form-stdlib/reason-coverage.fk \
  form/form-stdlib/source-compiler-emission.fk \
  form/form-stdlib/source-compiler-persistence.fk \
  form/form-stdlib/tests/source-compiler-persistence-band.fk)
# -> 2147483647
```

The band proves:

- manifest boundaries and deferrals;
- explicit 20-reason manifest with structural `rcov-coverage`;
- fkb-only emission + matching supplied probe bundle -> `persistence-ready`;
- fkb+dylib emission + matching supplied probe bundle -> `persistence-ready`;
- resulting descriptor triples route through existing `sad-route` as
  `sac-run-fkb` and `sac-run-dylib`;
- malformed emission, bundle, source observation, fkb observation, and dylib
  observation refuse;
- non-emitted emission, absent/mismatched source, absent/mismatched fkb, bad
  fkb seal, missing table flag, stale fkb route, unrequested dylib, missing
  requested dylib, dylib identity mismatch, dylib policy failure, and stale
  dylib route investigate;
- investigate/refused rows keep a total descriptor-triple sentinel;
- mirror parity;
- forbidden-name static scan over the source file.

## Deferred

- Actual `.fkb` or `.dylib` writes.
- Durable filesystem truth.
- Byte hashing from disk.
- Seal/proof/callable verification.
- Table execution.
- Program-image load/walk.
- Native dylib load/call.
- Selector installation.
- C-seed growth.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Claim `persisted` status | Rejected | The layer sees supplied observations, not durable write truth. |
| Compare emission and observation mtimes directly | Rejected | Freshness/currency belongs to `sad-route`; exact mtime equality would make real post-write observations brittle. |
| Rebuild descriptor routing locally | Rejected | 8c already owns observation-to-descriptor and `sad-route` delegation. |
| Perform binary writes here | Rejected | Current grounded probes still do not support honest program-image binary persistence. |

## Post-Review

Grok post-review verdict: `PASS_WITH_CHANGES`.

Grok required one semantic tightening: every `investigate` and `refused` row
must carry the sentinel descriptor triple, not a routable triple derived from a
shape-valid bundle. The implementation now reserves real descriptor triples for
`persistence-ready` rows only, and the band checks the sentinel on both refused
and investigate examples.

Claude post-review verdict: `PASS_WITH_CHANGES`.

Claude initially accepted the implementation and required only receipt wording,
but Grok's stricter reading better matches the pre-review requirement and
prevents downstream consumers from accidentally routing non-ready observations.
The code, band, and receipt were updated to the stricter contract before final
verification.

Follow-up Grok verdict: `PASS`.

Follow-up Claude verdict: `PASS`.

Both reviewers accepted the stricter sentinel contract: only
`persistence-ready` rows carry `sap-descriptor-triple`, while `investigate` and
`refused` rows carry `scp-empty-triple`. The band now proves this on both a
refused malformed-bundle row and an investigate source-mismatch row. Claude
noted a wording nuance: the sentinel source slot is a `sad-source` row rather
than a separately asserted invalid-source predicate; the artifact slots are the
important no-observation signal because both are `sad-no-artifact`.
