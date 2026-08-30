# Resident context carries the next local capsule

Date: 2026-08-30  
Witness: `observe/form-local-plan-resident-live.fk` on this Mac's
`Qwen3.8-27B-Q8_0.gguf`, through `fkwu` and the linked Form/Metal carrier.

## What moved

`form-cli-model-generate.fk` formerly opened a new Qwen residence at the
first prompt's exact required context.  A later, longer local capsule was
honestly outside that residence and therefore released, resealed, and reopened
the model.  That made a long-lived Form agent only a claim.

`form-cli-resident-continuity.bml` now gives the living-agent profile its
selected GGUF's own declared context capacity.  This is an artifact fact, not a
fixed token quota.  Ordinary generation remains exact-need.  A request beyond
the artifact context remains a typed local gap; it does not grant a remote
fallback.

## Live receipt

The first local plan emitted:

```
model-seal-begin → model-seal-complete → model-open-begin →
model-open-complete → model-state-begin → model-prefill-begin →
model-prefill-complete → model-decode-begin → model-decode-complete
first-wall-ms=127545
local-plan-status=ready
local-plan-move=preflight
```

The second request then emitted only:

```
model-state-begin → model-prefill-begin → model-prefill-complete →
model-decode-begin → model-decode-complete
second-wall-ms=80458
residence-held-before-release=1
released=1
local-plan-status=ready
local-plan-move=land
```

The missing second `model-seal-*` and `model-open-*` stages are the direct
evidence that the same local Form residence carried the later capsule.  Neither
request crossed to a provider route.

## Proof

```
./fkwu form/form-stdlib/bml/form-cli-resident-continuity.bml  # 0
./fkwu form/form-stdlib/tests/form-cli-resident-continuity-band.fk  # 2047
./fkwu form/form-stdlib/tests/form-cli-local-plan-band.fk  # 2047
```

## Honest next stone

The carrier remains synchronous while inside Metal prefill/decode.  It exposes
stage frames before each call but cannot yet receive a timeout/cancel control
frame during that call.  A live local agent needs that native future/poll seam
before it can be treated as a bounded answer service.  This receipt proves
context continuity, not asynchronous cancellation.
