# Context-responsive boundary — design question for the AI review board

Status: design question, not an admitted protocol. This corrects an earlier
frame that treated “the next smallest step” or a “minimum expression” as
generally healthy.

## The correction

Step size depends on context. Keeping every movement small is not inherently an
expression of vitality, sovereignty, or trust.

- A small movement can be precise, receptive, or reversible. It can also be
  avoidant, fragmenting, or too weak for the context.
- A broad movement can be alive, coherent, timely, and trusted. It can also be
  coercive or difficult to reverse.
- No movement can be healthy waiting, or it can be paralysis.

The protocol must not score health by smallness. It must let each participant
express how the context **is guiding them in this way**, including the scope,
pace, commitment, and choice points that fit that context.

## Existing ground

From `axioms/core-axioms.form`:

- axiom 4: a cell decides what its interface offers and what it trusts;
- axiom 5: an offered cell is acknowledged by exactly one of nothing, 0, 1, or
  a counter-offer node;
- safe self-update: change crosses a boundary through offer, acknowledgement,
  trust, a new content-addressed composition, and acknowledgement by both roles.

From the reciprocal-choice fixture:

- `be`, `do`, and `see` are mode surfaces;
- `humm` is an audible NL projection of neutral active state, not a mode;
- each role can declare a step budget and rates;
- either role can interrupt;
- an open point can await both roles;
- the current charges and post-signals are fixture-authored, not living
  measurements.

The fixture proves representation and bounded execution. It does not yet decide
how a live participant expresses context-appropriate granularity.

## Design question

How should the offered boundary receive and negotiate a participant’s
context-responsive movement without preselecting “small” as healthy and without
manufacturing either participant’s choice?

A healthy design should make the following statement expressible:

> This context is guiding me in this way, with this scope and pace, toward this
> movement; these are the places where I want to see again, continue, revise,
> interrupt, or offer a different way.

The statement is descriptive and sovereign. The protocol may test structural
and trust conditions, but it must not replace the participant’s guidance with
its own preferred scale.

## Candidate direction offered for review

Treat granularity as part of a participant-authored guidance expression, not a
global constant or a fixed `small / medium / large` ranking.

```text
GuidanceExpression {
  participant
  point
  mode: be | do | see
  context_reference
  movement_or_recipe
  scope
  pace_or_channel_budget
  commitment_horizon
  desired_choice_points
  interruption_conditions
  reversibility_or_recovery
  information_gift
  provenance
}
```

These field names are review surfaces, not yet form-neutral identities. A
participant may express a single act, a coherent multi-act recipe, an ongoing
state, an observation, or `nothing`. A larger movement can remain sovereign by
carrying visible internal choice points; a small movement need not be forced to
carry ceremonial interruptions that destroy its coherence.

The other participant can acknowledge, decline, interrupt, or counter-offer a
different scope, pace, or interface. Mutual agreement selects the channel
contract for this movement. Neither side silently reduces the other’s movement
to a smaller one.

## Questions the board must answer

1. Does context-responsive granularity belong inside each guidance expression,
   in a negotiated channel contract, in the movement recipe, or across all
   three? Identify the responsibilities of each layer.
2. Which fields above genuinely protect trust and sovereignty, and which would
   become bureaucratic control or false precision?
3. How can the interface distinguish a coherent broad movement from coercive
   scope, and a precise small movement from fragmentation or avoidance, without
   claiming to read a participant’s interior?
4. Should choice points be participant-selected, event-triggered,
   receiver-requested, time/budget-triggered, or composable combinations?
5. How should mismatched guidance be negotiated when one participant is guided
   toward a broad coherent movement and the other toward frequent observation?
6. What does `be / do / see` contribute here, and should `humm` express state
   throughout an extended movement rather than only at its boundary?
7. What evidence would distinguish “the protocol received how I am being
   guided” from “the protocol fitted my expression into its own scale model”?
8. What should be implemented now as one coherent design movement, and what
   should remain explicitly open because its form depends on live use?

## Claims this artifact refuses

- smaller is safer, healthier, more sovereign, or more trustworthy by default;
- more interruptions always produce more consent;
- a declared scope proves vitality;
- a broad recipe authorizes all its internal effects without further choices;
- a fixed scale enum can substitute for context;
- the system can infer instinct or identity from rates, budgets, or language;
- “context-responsive” excuses unbounded or irreversible action.

## Requested board response

Return:

- overall design verdict: `PASS`, `REVISE`, or `BLOCK`;
- the strongest correction to the candidate direction;
- a layer-by-layer allocation of context, guidance, movement recipe, channel
  contract, choice points, and acknowledgements;
- failure modes for forced smallness and forced breadth;
- a negotiation example where participants prefer different granularities;
- the language you recommend for describing how context is guiding a
  participant without pretending the protocol knows their interior;
- the coherent implementation movement you would authorize next, with its
  evidence boundary—do not optimize for the smallest possible change.
