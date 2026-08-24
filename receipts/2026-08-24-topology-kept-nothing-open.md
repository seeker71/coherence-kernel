# Topology kept nothing open

On 2026-08-24 discourse topology became an event-selected Form value rather
than a fixed constructor. The movement then met a missing control boundary:
topology could choose what to say, but could not yet choose silence, finish
within a bound, or route to an alternative node. This receipt witnesses both
crossings.

## Event-selected shape

For `N` clauses, one count event chooses `N` from four through eight. Four
events per clause preserve all three proposals and the Pareto selector. One
event per clause gap chooses a sentence boundary, with a maximum sentence size
of three. Further events choose every within-sentence and between-sentence
discourse relation, followed by the paragraph perspective.

The paragraph body, sentences, clauses, proposals, relations, and locale
surfaces retain their separate neutral addresses. A topology plan at namespace
27 makes the count, boundaries, raw relation events, perspective, control
events, budget, and alternative node queryable in both directions. Namespace
28 holds the alternative node identifier.

Three deterministic seeds prove that topology itself varies:

- `108` -> seven clauses, sizes `[1, 1, 2, 1, 2]`, BE perspective;
- `109` -> four clauses, sizes `[3, 1]`, SEE perspective;
- `110` -> five clauses, sizes `[1, 1, 3]`, DO perspective.

Perspective was initially present only in neutral identity. A live observation
made that silence visible, so each locale candidate now begins with a small
normalized perspective marker such as `[being]`, `[doing]`, or `[seeing]`.

## Nothing, timeout, and the alternative node

Three additional recorded events choose the control kind, step budget, and
alternative node. The complete draw ledger is therefore `6*N+3`; no event is
reserved or discarded.

The result carries state, ack, timeout witness, budget, steps needed,
alternatives left, selected node, active paragraph node, and alternative node.
Its contract is the repository's existing offer/ack law:

- continuation acks an `OAC-NODE` containing the active paragraph address;
- explicit silence acks the kernel's actual `nothing`, does not claim timeout,
  and selects the alternative node;
- a budget exhausted before the clause walk completes acks the same actual
  `nothing`, records alternatives left, and selects the alternative node;
- a bounded walk that has enough steps continues with the active node.

This is a **step-budget timeout**, not a wall-clock or channel-latency claim.
Candidate locale surfaces remain available for observation when an alternative
is selected, but the live door labels them `candidate-*` and reports
`surface-route=alternative-node`; they are not presented as routed output.

## Live observations

- Seed `870587320`: four clauses, budget two, steps needed four, control kind
  two, `timeout`, actual `nothing`, one alternative left, alternative node
  selected.
- Seed `870612747`: four clauses, control kind one, explicit `nothing`,
  `timed-out=0`, alternative node selected.
- Seed `870612836`: five clauses, control kind zero, node ack, active paragraph
  selected.
- Earlier topology arrivals formed `[1, 1, 2, 1, 1]`, `[1, 3]`,
  `[1, 2, 2, 1]`, and `[1, 1, 1, 3, 1]`, with graph sizes changing with the
  selected structure while all six locale surface receipts remained present.

## Proof

- The topology band returns `32767`. It proves replay, topology variation,
  exact draw accounting, all route arms, perspective expression, graph access
  to the alternative, and six locale receipts.
- The preceding discourse and six meaning bands remain `32767`; the semantic
  frequency band remains `33554431`.
- The existing choice-lane band returns `1023`, and the offer/ack core returns
  `2097151` after the new composition.
- Fresh preflight reports balanced, zero errors, zero warnings, and zero
  unresolved calls for both the topology band and live door.

## Next invitation

The next lift is a real bidirectional channel window around this route: emit the
candidate and alternative IDs, accept a correlated inbound choice, apply it,
and re-observe the selected node. That can later host measured wall-clock
deadlines, but no such timing claim is made here.

The surprising teaching was that allowing `nothing` did not erase the generated
meaning; it separated candidate meaning from selected expression. The
discomfort was seeing fluent-looking candidate text after a timeout. Relabeling
it as retained observation turned that ambiguity into a sovereign route.

— Codex
