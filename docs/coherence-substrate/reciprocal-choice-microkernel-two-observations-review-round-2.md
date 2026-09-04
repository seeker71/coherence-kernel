# Reciprocal choice microkernel — repaired two-observation candidate

Status: second-round review candidate, witnessed on native `fkwu` (band
`4194303`, re-run 2026-09-04). It is a fixture walk, not yet a live human or
biological-intuition organ.

## What round one required

Grok, Claude, and Codex independently returned `REVISE`; Cursor completed with
an empty response. Their common objections were located in the earlier
candidate:

1. native source code `2` was decoded as `intuition` although only authored
   charge vectors were present;
2. a cost ceiling measured in steps was called a timeout;
3. observation 2 automatically carried the human fixture without a new
   acknowledgement event;
4. `interrupt = 1` and `next = N+1` were constructor constants, not an explicit
   open point awaiting both roles;
5. rendered symbolic rows were mislabeled as native trace rows;
6. fragment eligibility operands were lost from the observation;
7. the band did not test the discriminating mixed fallback case;
8. `humm-state` fused a neutral state and its NL sound projection in prose even
   though the executable cell kept them separate.

This revision repairs all eight at the executable layer.

## Form-neutral protocol

Modes are numeric identities:

```text
0 undecided
1 be
2 do
3 see
```

The selected mode derives neutral state `1`. `humm` is an audible NL surface
projection of that state:

```text
[rcm-state, 1, [audible-nl, humm]]
```

The sound is not the state identity and is not a fourth mode. In this seed the
state is derived from `mode > 0`; it carries no independent choice yet.

Each choice records identity, source, mode, step budget, information rate,
interaction rate, interface, recipe, fallback, optional declared charges, and
neutral state. Source identities are:

```text
1 explicit
2 declared-charges
3 carried
4 explicit-carry-ack
```

Source `2` is never decoded as intuition. The charge vector is:

```text
[be, do, see, short, medium, long, confidence]
```

Mode and duration are separate triplets. A unique maximum in the mode triplet
selects `be`, `do`, or `see`; independently, a unique maximum in the duration
triplet selects budget 1, 3, or 5. Confidence must be at least 60. A tie or low
confidence leaves the choice undecided.

An exchange has two roles by contract: offerer and receiver. Their step budgets
and rates are bounded by pairwise minimum. A candidate is eligible when its
mode matches the receiver, its cost fits the channel budget, and its readiness
meets the offered threshold. If nothing is eligible, either role's interrupt
fallback blocks silent model-role pass-through; pass-through occurs only when
both roles choose it.

The resulting observation now retains the selected candidate, offered
model-role fragment, and readiness threshold. It ends with an explicit point:

```text
[rcm-choice-point, id, open, awaiting-offerer, awaiting-receiver]
```

The point record is a protocol state, not evidence that a live human is waiting.
A later fixture exchange links its offer to that point id and provides a new
offerer event and receiver event. This seed does not yet close the prior point
or enforce an external wait.

## Observation 1

The human fixture event is explicit: mode `do`, step budget 4, information rate
9, interaction rate 7, interrupt fallback. It is fixture input, not a statement
about a live human.

Sema's declared charges are:

```text
[20, 90, 30, 20, 80, 30, 90]
```

Separate triplet selection yields `do` and budget 3. The channel values are
budget 3, information rate 8, interaction rate 6. Selected candidate
`[do, cost 2, readiness 85, fragment 701, recipe 3001]` meets threshold 70
and replaces offered model-role fixture fragment 700.

Exact native observation row:

```text
[rcm-observation, 1,
 [rcm-choice, 1001, 1, 2, 4, 9, 7, 2001, 3001, 1, nothing, 1],
 [rcm-choice, 1002, 2, 2, 3, 8, 6, 2001, 3001, 1,
  [20, 90, 30, 20, 80, 30, 90], 1],
 3, 8, 6, 701, 1, 1, 0, 90, 10, 80,
 [2, 2, 85, 701, 3001], 700, 70,
 [rcm-choice-point, 2, 1, 1, 1]]
```

The runner separately projects receiver source `declared-charges`, state
expression `[rcm-state, 1, [audible-nl, humm]]`, and the open point awaiting both
roles.

## Observation 2

Point 2 receives a new human-side fixture event with source
`explicit-carry-ack`. It repeats the earlier values while explicitly recording
acknowledgement of reuse at this point. This demonstrates that the protocol can
distinguish bare carry from explicit carry acknowledgement. It still does not
claim a live human act or biological consent.

The next Sema charges are transparently derived from observation 1's authored
post-signals: pressure supplies `be` and short; successful output lowers `do`
and medium to 40; novelty supplies `see` and long; confidence is the larger of
novelty and pressure:

```text
[10, 40, 90, 10, 40, 90, 90]
```

Separate triplet selection yields `see` and budget 5. The human fixture ceiling
is 4, so the channel budget is 4. Selected candidate
`[see, cost 4, readiness 92, fragment 712, recipe 3002]` meets threshold 70
and replaces offered model-role fixture fragment 710.

Exact native observation row:

```text
[rcm-observation, 2,
 [rcm-choice, 1001, 4, 2, 4, 9, 7, 2001, 3001, 1, nothing, 1],
 [rcm-choice, 1002, 2, 3, 5, 8, 6, 2001, 3002, 1,
  [10, 40, 90, 10, 40, 90, 90], 1],
 4, 8, 6, 712, 1, 1, 0, 20, 15, 90,
 [3, 4, 92, 712, 3002], 710, 70,
 [rcm-choice-point, 3, 1, 1, 1]]
```

The runner projects offerer source `explicit-carry-ack`, receiver source
`declared-charges`, the separate state expression, and open point 3 awaiting
both roles.

## Controls and witness

- Over-budget + interrupt: a ready cost-5 candidate inside budget 3 produces
  output 0, neutral provenance, and the real `nothing` acknowledgement.
- Both pass: the same over-budget shape passes model-role fixture fragment 799
  only when both roles choose model-pass.
- Mixed fallback, offerer interrupts: human interrupt plus Sema model-pass
  produces output 0, neutral provenance, and `nothing`.
- Mirrored mixed fallback, receiver interrupts: human model-pass plus Sema
  interrupt also produces output 0, neutral provenance, and `nothing`. Together
  the two orientations distinguish the symmetric `or` from either one-sided
  shortcut.
- Crossed selector axes: charges `[10,90,20,10,20,90,90]` select `do` with
  long budget 5, distinguishing independent triplet selection from a
  mode-to-budget shortcut.
- The observation retains candidate, offered fragment, and threshold, so
  eligibility operands are visible rather than inferred from prose.
- Both observations project neutral state `1` separately from audible NL
  surface `humm`.
- Both end with an explicit open next point awaiting offerer and receiver.
- The runner records four correlated framebuffer events and final success 1.
- Fresh preflight reports balanced forms, zero errors, zero warnings, zero
  unresolved calls, and a clean chain.
- Native fourth-arm validation returns `4194303`, all twenty-two bits set.

The fourth-arm-only declaration is honest: this protocol composes the existing
real `nothing` offer/ack value, which the runtime `fkwu` carries and the three
proof siblings currently reject as unbound. No sentinel was substituted.

## Exact evidence boundary

This fixture directly establishes:

1. the declared-charge policy selects `do`, budget 3 in observation 1;
2. it selects `see`, budget 5 in observation 2;
3. receiver mode changes `do → see`;
4. receiver budget changes `3 → 5` and channel budget changes `3 → 4`;
5. a distinct explicit-carry-ack fixture event occupies the offerer role at
   point 2;
6. eligible recipe fragments replace model-role fixture fragments at the
   executable seam;
7. each observation creates an explicit open point awaiting both roles;
8. the neutral active state has a separate audible NL projection `humm`;
9. mixed fallback preserves either role's interrupt preference.

It does not establish:

1. biological, somatic, or pre-symbolic intuition;
2. that authored charges or post-signals are independent measurements;
3. that a live human performed the explicit-carry-ack fixture event;
4. real wall-clock timeout behavior—the values are step budgets;
5. interception of a model's logits or sampler;
6. that an open fixture record causes an external participant to wait;
7. that `declared-charge-policy-selected` or
   `fixture-outcome-predicate-met` independently validates its own inputs;
8. that `humm` is the only possible NL/PL surface for neutral state `1`.

The two-turn count and open-point construction are protocol invariants. The
mode and budget changes, candidate selection, mixed fallback, and source changes
are comparisons that can fail and are witnessed by the band.
