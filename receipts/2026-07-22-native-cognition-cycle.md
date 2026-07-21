# 2026-07-22 — knowledge became inquiry, awareness, recognition, action, response, and routing

## Ground

The fresh branch returned `42`, `55`, freshness `15`, and the numeric-list
witness before this round executed.

## First complete cycle

`cognition/native-cognition-cycle.fk` composes the existing learned language
model and neutral inquiry graph into one bounded route:

```text
knowledge prediction → seven-lane inquiry → model-agreement awareness
→ recognition → control action → proof-shaped response → native/evidence route
```

Every held-out row emits a correlated outbound observation and receives an
inbound action through the bidirectional framebuffer. Agreement selects native
response (`action 0`); disagreement selects evidence request (`action 4`).

```text
./fkwu --src cognition/tests/native-cognition-cycle-band.fk
[nothing, 0, 1, 1001099008, 22, 18, 4, 17, 1, 4,
 126, 126, 18, 4, 44, [...], 1]
```

Eighteen rows were agreement-recognized, four were routed to evidence, 17/18
native responses were correct, all 126 native inquiry receipts replayed, and 44
directional frames were retained. One shared error remained: content hash
`459284`, `existence precedes every question i could ask about it`, truth class
2, was predicted as class 3 by both networks.

## Dynamic diagnostics changed the next attempt

The framebuffer then observed best/second prototype distances for all 18
agreement rows. The shared wrong row's margin was `1.4312345678584597`; correct
rows had margins as low as `0.4750239591775931` and `0.51172530070276734`.
Confidence thresholding therefore could not isolate the error.

A nearest-training-example reader was tried next. It used the same hashed
embedding, agreed with the shared error, reduced native coverage to 14, and
still served one wrong response:

```text
[nothing, 0, 1, 1001099010, 22, 14, 8, 13, 1, 0,
 98, 98, 14, 8, 44, [...], 0]
```

This observed that another reader of the same representation was not sufficient
independence; no causal claim about internal weights is made.

## Representation-diverse recognition

The third reader was replaced by exact-token evidence summed across all 160
training rows. It uses neither hash buckets nor either network transform. Native
response now requires agreement among base network, rehearsal network, and the
exact-token corpus reader.

```text
./fkwu --src cognition/tests/native-cognition-triangulated-recognition-band.fk
[nothing, 0, 1, 1001099010, 22, 16, 6, 16, 0, 1,
 112, 112, 16, 6, 44, [...], 1]
```

Measured result: 16/22 native coverage, 16/16 correct native responses, six
evidence routes, the former shared error caught, 112/112 inquiry replays, and 44
bidirectional frames. Relative to two-network agreement, this exchanges two
native responses for zero observed false native responses on this partition.

## Honest floor

The result is one deterministic 22-row evaluation over four meanings. It does
not establish perfect precision outside that partition. “Recognition” means
agreement across three representation paths, not semantic certainty. Evidence
routing is an explicit response, not a completed answer. The controller remains
synchronous Form logic; direct weight actuation remains outside this claim.
