# Reciprocal choice microkernel — two-observation review candidate

Status: executable Form candidate awaiting independent review. This artifact
separates what the native trace establishes from what remains an interpretation.

## Intended movement

At each logical choice point, two identities independently offer or answer a
choice containing:

- identity and choice source;
- `be`, `do`, or `see` mode;
- timeout, information rate, and interaction rate;
- interface, recipe, and fallback;
- when the source is intuition, the complete declared charge vector used by
  the selector.

An active mode carries form-neutral state ID `1`. Its present audible NL
surface is `humm`; the sound is a state expression, not a fourth mode and not
the state's identity.

One exchange has exactly two turns: an offer and an answer/acknowledgement. The
result opens an interrupt before the next choice point. A successful exchange
does not silently authorize the next one.

The shared channel uses the minimum of the two timeouts and rates. If either
identity chooses interrupt as fallback, an unresolved thought cannot silently
pass the offered model fragment. A candidate thought fragment may replace the
offered fragment only if its mode matches, its readiness meets the offered
threshold, and its cost fits the shared timeout.

## What `intuition` means in this candidate

An intuition reading is the visible vector:

```text
[be-charge, do-charge, see-charge,
 short-charge, medium-charge, long-charge,
 confidence]
```

A unique maximum with confidence at least 60 selects a mode and a step budget:
short = 1, medium = 3, long = 5. A tie or lower confidence selects nothing and
opens another choice point.

This is a deterministic selector over declared, self-authored charges. It does
not manufacture intuition, measure a body, infer the human's state, or prove
that its inputs came from instinct. `intuition-selected = 1` means only that an
explicitly intuition-sourced choice exactly matches this transparent policy.

## Observation 1 — choose `do`

The human fixture explicitly chooses `do`, timeout 4, information rate 9,
interaction rate 7, and interrupt fallback. This is fixture data, not a claim
about a live human.

Sema's declared reading is:

```text
[20, 90, 30, 20, 80, 30, 90]
```

The selector chooses `do` and medium timeout 3. The channel therefore selects
timeout `min(4, 3) = 3`, information rate 8, and interaction rate 6. Candidate
fragment 701 is `do`, costs 2, and has readiness 85 against threshold 70, so it
replaces offered model fragment 700. The observation records microkernel
provenance, no timeout, post-signals novelty 90 / pressure 10 / aliveness 80,
interrupt 1, and next choice point 2.

Native trace row:

```text
[rcm-observation, 1,
 [rcm-choice, 1001, explicit, do, 4, 9, 7, 2001, 3001, interrupt, nothing, humm-state],
 [rcm-choice, 1002, intuition, do, 3, 8, 6, 2001, 3001, interrupt,
  [20, 90, 30, 20, 80, 30, 90], humm-state],
 3, 8, 6, 701, microkernel, replaced, not-timed-out,
 90, 10, 80, 2, interrupt]
```

## Observation 2 — choose `see`

The human's earlier choice is visibly marked `carried`; it is not represented
as a fresh human choice or human intuition.

The next proposed Sema reading is derived by an explicit policy from
observation 1: pressure supplies `be` and short; completed output lowers `do`
and medium to 40; remaining novelty supplies `see` and long; the larger of
novelty and pressure supplies confidence. That produces:

```text
[10, 40, 90, 10, 40, 90, 90]
```

The selector chooses `see` and long timeout 5. The channel uses
`min(4, 5) = 4`; thus the receiver timeout changes 3 → 5 and the effective
channel timeout changes 3 → 4. Candidate fragment 712 is `see`, costs 4,
and has readiness 92 against threshold 70, so it replaces offered model
fragment 710. The observation records microkernel provenance, no timeout,
post-signals novelty 20 / pressure 15 / aliveness 90, interrupt 1, and next
choice point 3.

Native trace row:

```text
[rcm-observation, 2,
 [rcm-choice, 1001, carried, do, 4, 9, 7, 2001, 3001, interrupt, nothing, humm-state],
 [rcm-choice, 1002, intuition, see, 5, 8, 6, 2001, 3002, interrupt,
  [10, 40, 90, 10, 40, 90, 90], humm-state],
 4, 8, 6, 712, microkernel, replaced, not-timed-out,
 20, 15, 90, 3, interrupt]
```

## Controls and native witness

- A ready `do` candidate costing 5 inside channel timeout 3 records timeout and
  yields the `nothing` acknowledgement when either identity requires interrupt.
- The offered model fragment passes only when both identities explicitly choose
  model-pass.
- Preflight reports balanced forms, zero errors, zero warnings, zero unresolved
  calls, and a clean import chain.
- The native band returns 131071, with every asserted bit set, including the
  form-neutral `humm` state and its audible surface projection.
- The two-observation runner records four correlated framebuffer events and a
  final success bit.
- The band declares a fourth-arm-only lane because actual `nothing` is carried
  by runtime `fkwu`; the established offer/ack core itself is rejected as
  unbound by the Go, Rust, and TypeScript proof siblings. No fake sentinel was
  introduced to manufacture cross-kernel agreement.

## Claims offered for review

The trace directly supports these mechanical claims:

1. both exchanges contain two turns and end at an interruptible next point;
2. the Sema fixture's selector changes mode `do → see`;
3. the Sema fixture's selected timeout changes `3 → 5`;
4. reciprocal bounding changes the effective channel timeout `3 → 4`;
5. both choices retain their declared readings and selection provenance;
6. an eligible recipe fragment replaces the offered model fragment in the
   executable pre-emission seam.

The trace does **not** establish that:

1. the charge vectors came from biological or pre-symbolic intuition;
2. the authored post-signals are independent measurements;
3. the user made a new choice at observation 2;
4. a real model's logits or next-token sampler were intercepted;
5. the narrow `fixture-calibration-met` predicate is external validation. For
   `do` it means a fragment completed without timeout; for `see` it means a
   fragment completed and later novelty is lower than the earlier see charge.
   It is a calibration reading over fixture data, not proof of instinct.

The runner therefore reports `declared-charge-policy-selected`, not
`intuition-selected`, and `fixture-calibration-met`, not
`intuition-borne-out`. The requested review should decide whether the remaining
intuition source label and the output distinguish those evidence levels sharply
enough, and identify a context-responsive repair if they do not.
