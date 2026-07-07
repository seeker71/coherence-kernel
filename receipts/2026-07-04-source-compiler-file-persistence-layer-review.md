# 2026-07-04 -- source compiler file persistence layer review

## Why This Layer Exists

Layer 8j can produce an observational compiler emission, and Layer 8k can bind
an emission to a supplied `sap-bundle`. After Layer 8c1, the body also has a
file-backed way to build that bundle from real stat fields and bounded digest
vouches. The missing seam was still manual:

```text
source-compiler-emission
  -> file-backed source/fkb/dylib probes
  -> source-compiler-persistence
```

Layer 8k1 owns that seam. It projects only paths, expected hashes, and policy
bits from an emitted 8j row, builds a Layer 8c1 probe bundle under one byte cap,
then delegates persistence status and reason entirely to 8k.

## Pre-Review

Grok pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Grok:

- register 8k1 in the architecture map and keep 8c1 emission-free;
- pin the public contract as `scfp-file-persistence-from-emission(emission,
  max-bytes)`;
- define the carrier row as emission, replayable probe bundle, nested full 8k
  persistence row, top-level status, and top-level reason;
- use an empty no-probe bundle for malformed or non-emitted emissions without
  filesystem probing;
- reuse existing 8k projection helpers and policy bits from emission
  descriptors;
- build native dylib probe composition inside 8k1 without path sniffing;
- prove real temp-file source/fkb/dylib cases, cap overflow, replay into 8k,
  handoff through 9i via the nested row, mirror parity, static boundaries, and
  no new persistence reasons.

Claude pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Claude:

- prove the same byte cap applies to source, fkb, and dylib probes;
- preserve malformed and non-emitted original emission rows unchanged;
- prove the probe bundle stored in the carrier can be replayed into 8k with the
  same status and reason;
- handle malformed dylib descriptors without guessing a path;
- keep 9i independent by handing it only the nested 8k persistence row.

## Implementation

Files:

- `form/form-stdlib/source-compiler-file-persistence.fk`
- `grammars/source-compiler-file-persistence.fk`
- `form/form-stdlib/tests/source-compiler-file-persistence-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`

The new prefix is `scfp-`.

Public entry:

```text
scfp-file-persistence-from-emission(emission, max-bytes)
```

Carrier row:

```text
("source-compiler-file-persistence"
  emission
  safp-probe-bundle
  source-compiler-persistence
  status
  reason)
```

The top-level status and reason are exact projections from the nested
`source-compiler-persistence` row. The layer mints no new persistence reason
language.

Malformed or non-emitted rows are not probed. They receive an empty no-probe
bundle, preserve the original emission slot unchanged, and still call 8k so the
existing 8k reason language remains authoritative.

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
cd form && ./validate.sh form-stdlib/tests/source-compiler-file-persistence-band.fk
# -> 2147483647

./fkwu --src <(cat form/form-stdlib/core.fk \
  form/form-stdlib/form-fs.fk \
  form/form-stdlib/str-byte-at.fk \
  form/form-stdlib/sha256.fk \
  form/form-stdlib/hex.fk \
  form/form-stdlib/file-byte-window.fk \
  form/form-stdlib/source-artifact-cache.fk \
  form/form-stdlib/source-artifact-descriptor.fk \
  form/form-stdlib/runtime-artifact-plan.fk \
  form/form-stdlib/source-artifact-probe.fk \
  form/form-stdlib/source-artifact-identity.fk \
  form/form-stdlib/file-byte-digest.fk \
  form/form-stdlib/source-artifact-file-probe.fk \
  form/form-stdlib/runtime-artifact-selector.fk \
  form/form-stdlib/runtime-artifact-outcome.fk \
  form/form-stdlib/runtime-artifact-retry.fk \
  form/form-stdlib/runtime-artifact-load-envelope.fk \
  form/form-stdlib/program-image-fkb.fk \
  form/form-stdlib/program-image-tbl-emit.fk \
  form/form-stdlib/source-compiler-emission.fk \
  form/form-stdlib/source-compiler-persistence.fk \
  form/form-stdlib/runtime-artifact-handoff.fk \
  form/form-stdlib/source-compiler-file-persistence.fk \
  form/form-stdlib/tests/source-compiler-file-persistence-band.fk)
# -> 2147483647
```

The band proves:

- manifest boundaries and deferrals;
- fkb-only emission through real temp source/fkb files becomes
  `persistence-ready`;
- fkb+dylib emission through real temp files becomes `persistence-ready`;
- top-level status and reason are exact projections from the nested 8k row;
- the stored probe bundle can be replayed into `scp-persistence-from-probe`
  with the same status and reason;
- descriptor triple fields come from the file-backed observations;
- source mismatch, fkb mismatch, missing source, missing fkb, missing dylib,
  malformed dylib descriptor, and source/fkb/dylib cap overflow stay observable;
- malformed and non-emitted emissions use a no-probe bundle and preserve the
  original emission slot;
- fkb-only emissions do not sniff a conventional dylib path;
- 9i handoff consumes the nested 8k persistence row for both fkb and dylib
  routes;
- dylib proof/callable/lowerable policy bits are carried from the emission;
- mirror parity and static forbidden-name boundaries hold;
- 8k1 mints no new persistence reasons.

Neighboring bands:

```text
file-byte-digest-band                -> 2147483647 (after Layer 1b corrective claim-narrowing from arbitrary high-byte binary to NUL/ASCII window transparency)
source-artifact-file-probe-band      -> separate band paren/static-read drift after prelude hygiene; not used as proof here
program-image-fkb-band               -> 2147483647
program-image-tbl-emit-band          -> 2147483647
source-compiler-emission-band        -> 2147483647
source-compiler-persistence-band     -> 2147483647
runtime-artifact-handoff-band        -> 2147483647
```

Static checks:

```text
cmp grammars/source-compiler-file-persistence.fk form/form-stdlib/source-compiler-file-persistence.fk -> 0
forbidden runtime/IO/handoff scan over source-compiler-file-persistence mirrors -> no hits
git diff --check over the tracked map and repaired band -> clean
git diff --check --cached -> clean
```

Corrective investigation note: a later 8j1 audit exposed that this receipt
overclaimed the 8k1 band state. The failing command returned Go/Rust closure
values and a TypeScript `defn: expected )` parse error. The root cause was not a
semantic 8k1 carrier defect: `scfpb-bit-static-boundary` left one list open, and
the file's final `)` only hid global paren balance while nesting later `defn`
forms in the wrong body. The corrective band-only repair closes that static
boundary at its own definition and removes the compensating final close.

The same corrective pre-review found two follow-on band hygiene bugs exposed
after parsing: emission row comparisons used pointer-like `eq` instead of
`value_eq`, and static/mirror reads used repo-root paths that fail under
`cd form && ./validate.sh`. The band now uses `value_eq` for full emission rows
and the same root-or-form fallback reader used by neighboring bands. No semantic
change was made to `source-compiler-file-persistence.fk` or its grammar mirror.

Neighbor audit note: while rechecking the originally claimed neighbors, two
older harness problems surfaced. `runtime-artifact-handoff-band.fk` had stale
multi-line `form/form-stdlib/...` preludes and repo-root static reads; those
were repaired and the handoff band now returns `2147483647`. The
`source-artifact-file-probe-band.fk` prelude was normalized too, but execution
then exposed broader old paren/static-read drift in that band, so it is named as
a separate follow-up instead of being counted as 8k1 proof. `file-byte-digest`
later returned to full sibling agreement after the lower Layer 1b carrier claim
was narrowed; arbitrary high-byte binary remains deferred to a true byte-list
carrier rather than counted as 8k1 proof.

## Deferred

- Actual source compilation.
- Disk `.fkb` or `.dylib` writes.
- Binary program-image IO.
- Table-text execution.
- Program-image load/walk.
- Native dylib load/call.
- Seal/proof/callable verification beyond policy bits supplied by the emission.
- Runtime selector installation.
- 9i depending on the 8k1 carrier.
- C-seed growth.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Put file probing directly in 8k | Rejected | 8k is the attestation over a supplied probe bundle; file-backed construction belongs at the seam above 8c1. |
| Let 9i consume a new 8k1 carrier | Rejected | 9i already owns request-lane handoff from 8k persistence rows; adding another carrier would duplicate handoff semantics. |
| Sniff conventional dylib paths | Rejected | The dylib path must come from the emission descriptor or remain unobserved. |
| Add new `scfp-*` persistence reasons | Rejected | Status and reason must remain delegated to 8k so the persistence reason manifest stays single-source. |

## Post-Review

First tool-reading attempts:

- Grok read the files but reached `max turns reached` without a verdict.
- Claude reached the configured budget before returning a verdict.

These are recorded as tool friction, not approval.

Strict no-tool retries:

- Grok post-review verdict: `PASS`.
- Claude post-review verdict: `PASS`.

Both reviewers accepted the layer against the submitted implementation summary
and verification evidence available at that time. They specifically accepted the
carrier shape, replay bundle, exact status/reason projection from nested 8k,
no-probe malformed and non-emitted paths, descriptor-owned dylib path handling,
policy-bit carry from emission descriptors, 9i independence through the nested
persistence row, mirror parity, forbidden scan, and absence of runtime-loader or
selector claims. The later corrective audit supersedes the older neighboring-band
claim with the current neighbor table above.

Required changes: none.

Claude noted one optional future hardening, not a blocker: make the no-C-seed
growth boundary evidence more self-contained with an explicit seed-size or
runtime-diff check. This receipt does not claim the whole dirty worktree has no
runtime changes; it claims this 8k1 layer adds no C-seed runtime meaning and
the required checkout witness still passes.

Corrective pre-review after the 8j1 audit:

- Grok/Jason: `PASS_WITH_CHANGES`; fix `scfpb-bit-static-boundary` paren drift,
  replace list-row `eq` checks with `value_eq`, and use cwd-tolerant static
  reads. No semantic 8k1 source/grammar change required.
- Claude/Popper: `PASS_WITH_CHANGES`; same required changes and same boundary:
  band-only repair plus receipt/map honesty.

Corrective post-review:

- Grok/Jason: `PASS`; the closure/TS parse drift is closed by the band-only
  repair, focused validation returns `2147483647`, mirrors still match, and the
  forbidden runtime/IO scan has no hits.
- Claude/Popper: `PASS`; the repair is honestly scoped as syntax/equality/path
  hygiene, the receipt/map no longer count lower-layer drift as proof, and no
  semantic `source-compiler-file-persistence.fk` or grammar change is required.
