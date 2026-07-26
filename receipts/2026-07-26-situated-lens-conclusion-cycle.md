# Situated lens conclusion cycle — bidirectional witness

Date: 2026-07-26

## The next gap

The situated conclusion relation correctly returned `nothing` for missing or
ambiguous time/lens evidence. Stopping there would leave the diagnostic
responsibility outside the cell. The repository’s bidirectional framebuffer
law requires a bounded exchange: observe, correlate control, apply a real
action, and re-observe.

The continuation is
`observe/situated-lens-conclusion-cycle.fk`, with its carrier-free law in
`form/form-stdlib/situated-lens-conclusion-control.fk`.

## The cycle

The first settlement becomes an outbound framebuffer envelope. Its payload is
bounded diagnostic data:

```
[kind, reason, time, lens-rank, match-count, conclusion-node]
```

It contains no prompt, answer, or private content.

The controller maps the observed state to an existing action:

- settled value → `continue`
- invalid relation → `revise`
- ambiguous evidence → `abstain`
- missing evidence → `request-evidence`

The action is applied before the second observation:

- `request-evidence` may add one caller-offered row only when the row is valid,
  matches the exact missing time/lens coordinate, and cannot create ambiguity;
- `abstain` leaves the field untouched and selects the explicit alternative
  node rather than choosing between conflicting rows;
- a mismatched evidence offer is rejected, leaves the field untouched, and
  selects the alternative node;
- correlated direction, kind, and exchange id are checked before any action.

The conclusion is then settled again and emitted through a second correlated
framebuffer round. Every cycle retains four attributed events: outbound and
inbound before actuation, then outbound and inbound after it.

## The honest carrier seam

The first attempted four-way run of the actual cycle was rejected. Go and Rust
reported unbound `nothing?`; TypeScript reported unbound `fb_record`. That
failure named the real boundary: the existing actual framebuffer carrier is an
fkwu witness, not a proof-sibling operation.

The response was `revise`, not a weakened gate:

- controller policy, bounded payload, exact evidence admission, revision,
  abstention, and rejection moved into the carrier-free
  `situated-lens-conclusion-control.fk`;
- `situated-lens-conclusion-control-band.fk` proves that law four-way;
- the observe cell binds the proven law to actual fkwu `nothing?`, correlation,
  framebuffer recording, and re-observation.

The rejected run is retained here as the reason for the split; it is not
presented as passing evidence.

## Observed movements

The band carries three independent windows:

1. Missing evidence requests a matching row, accepts source node `9001`, and
   re-observes conclusion `1`; the second action is `continue`.
2. Two rows at one coordinate produce `abstain`; the alternative node is
   selected and ambiguity remains visible after re-observation.
3. Evidence for the wrong lens coordinate is rejected; the alternative node
   is selected and the missing state remains visible.

## Honest floor

- The cycle admits offered evidence; it does not discover, measure, or verify
  that evidence.
- Source attribution makes causality inspectable but does not prove truth.
- `revise` can replace an invalid field with one valid offered row; it cannot
  repair invalid scope, time, lens, base, or margin by itself.
- The policy is synchronous Form data, not a learned controller.
- The generic framebuffer protocol remains unchanged; this organ implements
  the actuator for the actions it admits.

## Witness

Direct source:

```
./fkwu --src observe/tests/situated-lens-conclusion-cycle-band.fk
-> 131071
```

Four-way:

```
cd form
./validate.sh form-stdlib/tests/situated-lens-conclusion-control-band.fk
-> 65535
-> 1 ok, 0 divergent
```

No native operation or C surface was added.
