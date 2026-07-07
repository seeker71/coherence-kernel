# 2026-07-04 -- runtime artifact handoff layer review

## Why This Layer Exists

Layer 8k can produce a `persistence-ready` descriptor triple from a source
compiler emission plus a supplied probe bundle. Layers 9a, 9b, and 9e already
know how to turn descriptor routes into a plan, a coherent selection, and a
request-ready load envelope.

The missing layer was the explicit handoff between those facts:

```text
source-compiler-persistence persistence-ready row
  -> descriptor triple
  -> runtime artifact plan
  -> runtime artifact selection
  -> runtime artifact load envelope
  -> runtime artifact handoff row
```

This layer is not a loader and not execution. `handoff-ready` means only that a
previously attested artifact triple composes into a request-ready artifact
envelope. It does not mean `.fkb` walk success, `.dylib` call success, or table
execution success.

## Pre-Review

Grok pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Grok:

- document the fixed admission order;
- keep the reason manifest reachable, or use a narrow helper for impossible
  downstream drift branches;
- require `rale` request-ready status and exact envelope route/action/kind
  alignment before `handoff-ready`.

Claude pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Claude:

- check non-ready persistence before descriptor-triple shape, because 8k
  non-ready rows intentionally carry a sentinel triple;
- keep impossible downstream branches only through an injected helper;
- classify non-artifact route from the selection route field, not by
  re-inspecting descriptors;
- compare defensive envelope drift against the selection's route/action claim,
  not against a re-derived policy.

The implementation follows those requirements.

## Implementation

Files:

- `form/form-stdlib/runtime-artifact-handoff.fk`
- `grammars/runtime-artifact-handoff.fk`
- `form/form-stdlib/tests/runtime-artifact-handoff-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`

The new prefix is `rah-`.

Public entry:

```text
rah-handoff-from-persistence(persistence)
```

Receipt row:

```text
("runtime-artifact-handoff" persistence descriptor-triple
  plan selection envelope status reason)
```

Public admission order is fixed:

1. malformed persistence -> `refused / malformed-persistence`
2. non-ready persistence -> `investigate / non-ready-persistence`
3. malformed descriptor triple -> `refused / malformed-descriptor-triple`
4. downstream composition through `rap-plan-from-descriptors`,
   `ras-selection-from-plan`, and `rale-envelope-from-selection`

Only `sac-run-fkb` and `sac-run-dylib` selections can become
`handoff-ready`. Compile-source and invalid routes investigate as
`descriptor-route-not-artifact`.

The narrow helper:

```text
rah-handoff-from-components(persistence, triple, plan, selection, envelope)
```

exists only for bands to inject impossible downstream drift cases after public
composition. It does not widen the public handoff input.

`rah-empty-triple` delegates to 8k's `scp-empty-triple`, so the non-ready and
malformed-public-input sentinel shape has one owner.

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
  form/form-stdlib/runtime-artifact-handoff.fk \
  form/form-stdlib/tests/runtime-artifact-handoff-band.fk)
# -> 2147483647
```

The band proves:

- manifest boundaries and deferrals;
- explicit 12-reason manifest with structural `rcov-coverage`;
- fkb-only `persistence-ready` -> `handoff-ready-program-image`;
- fkb+dylib `persistence-ready` -> `handoff-ready-native`;
- request-ready fields align with selection route/action and expected artifact
  kind;
- fkb handoff skips parse but not recompute;
- dylib handoff skips parse and recompute;
- malformed persistence refuses;
- non-ready persistence investigates before descriptor-triple inspection;
- malformed descriptor triples refuse only after persistence is ready;
- compile-source route investigates as non-artifact from the selection route;
- injected selection investigate/refused branches propagate;
- injected envelope investigate/refused branches propagate;
- injected route/action and artifact-kind drift investigate;
- the narrow helper exists for unreachable drift only;
- no runtime attempt or observation is produced;
- static scan over the source file does not find disk IO, byte hashing,
  loader/call, table-run, or observation-production names;
- `grammars/runtime-artifact-handoff.fk` mirrors the stdlib file exactly.

Investigation note: the first focused band run returned `2147483666`, which is
larger than the full mask. The cause was bad test arithmetic: the manifest
bits added the same bit once per feature. The band was corrected to score each
manifest group as one boolean bit, then returned the full mask. No runtime OOM,
stall, killed process, or low-mask semantic failure occurred in this layer.

## Deferred

- Actual `.fkb` load/walk.
- Actual `.dylib` load/call.
- Actual source compilation.
- Disk `.fkb` or `.dylib` writes.
- Whole-file byte hashing.
- Seal/proof/callable reverification.
- Attempt production.
- Runner observation production.
- Fallback execution.
- Runtime selector installation.
- C-seed growth.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Route non-ready rows by their descriptor triple | Rejected | 8k reserves real triples for `persistence-ready`; non-ready rows carry a sentinel and must investigate before triple shape checks. |
| Re-derive route policy inside 9i | Rejected | 9i composes 9a/9b/9e; route policy remains in `sad-route`/`sac-route`. |
| Treat compile-source as handoff-ready | Rejected | This layer is artifact handoff only. Compile-source belongs to the compiler front door, not artifact load handoff. |
| Remove unreachable drift reasons | Rejected for now | The helper gives a controlled way to prove envelope drift guards without widening the public input. |
| Produce a 9f attempt here | Rejected | Attempt production belongs after capability/executor evidence and must not be hidden in a composition layer. |

## Post-Review

Grok post-review verdict: `PASS`.

Grok accepted all pre-review requirements as satisfied: fixed admission order,
non-ready before triple shape, public composition through 9a/9b/9e, artifact
routes only, request-ready not load success, selection-route classification,
selection-anchored envelope drift checks, full reason coverage, and grammar
mirror parity.

Claude post-review verdict: `PASS`.

Claude independently re-ran the recorded checks and accepted the layer. Claude
had no required changes. It raised one useful non-blocking note: the original
`rah-empty-triple` rebuilt the same sentinel shape owned by 8k. The
implementation now delegates to `scp-empty-triple`.

Follow-up verification after the sentinel delegation:

```text
runtime-artifact-handoff-band -> 2147483647
source-compiler-persistence-band -> 2147483647
cmp form/form-stdlib/runtime-artifact-handoff.fk grammars/runtime-artifact-handoff.fk -> 0
forbidden static scan -> no hits
git diff --check -> clean
```

Follow-up Claude verdict: `PASS`.

Follow-up Grok verdict: `PASS`.

Required changes: none.
