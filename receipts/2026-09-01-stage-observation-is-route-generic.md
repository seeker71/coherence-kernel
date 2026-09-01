# A stage is observed where it begins

Date: 2026-09-01  
Author: Codex

## Ground

The standing glass was correctly able to show a previous peer down, but its
flight lane was narrower than the body: it searched only for
`route=direct-answer`.  A model-admission, source, or other Form-native
route could emit a begin event and still disappear from the panel's present
tense flight reading.

The display loop remains a one-second human frame; it is not the detection
clock.  `print_str` emits the stage into the resident's observed stream at
the transition itself, with a monotonic millisecond stamp.  Glass derives
age from that stamp without touching the model thread.  Thus a stage begins
as an event before any long native call; the next display frame is only its
egress to a human.

## Movement

`hearth-glass.bml` now reads the newest typed `form-peer stage` regardless
of its route.  It preserves the distinction:

- a latest `status=begin` has route, phase, and measured age;
- a terminal status yields route/phase `nothing` and age `0`;
- zero is never used to mean an absent flight.

The flight lane shows that route and phase.  Lane motion and the hearth
telemetry action read the same generic flight value.

`form-cli-model-fleet.bml` now emits the common stage surface at the native
admission boundary:

```
form-peer stage route=model-fleet phase=admit status=begin seat=<name> stamp-ms=<ms>
```

Its returned stage uses the same route and phase with the actual native
status.  No prompt bytes, answer bytes, or new authority enter this event.

This does not claim that a terminated resident can restart itself.  A dead
process has no remaining execution seat.  The correction is that a live
resident's active work is evidenced at the source transition, and every
route is visible to glass; a later birth/restoration is a distinct, explicit
act.

## Witnesses

```
./fkwu observe/preflight-run.fk                                  -> clean
./fkwu form/form-stdlib/tests/hearth-glass-band.fk               -> 16777215
./fkwu form/form-stdlib/tests/form-cli-model-fleet-band.fk       -> 2047
./fkwu form/form-stdlib/tests/form-cli-peer-direct-answer-action-band.fk -> 8191
./fkwu form/form-stdlib/form-cli-bml-cache-run.fk                -> bounded=1
./fkwu form/form-stdlib/tests/form-cli-author-high-band.fk       -> 4095
```

The generic-flight witness supplies a `model-fleet` begin stamped at 1200,
observes its 500 ms age at 1700, and then observes `nothing` and zero age
after its terminal stage.  Existing direct-answer behavior remains covered
in the same band.

I kept the exchange alive by treating the dashboard as egress, not the
source of truth.  The surprising teaching was that the begin event already
had the right clock; the glass was simply looking through a single-route
keyhole.  The discomfort of stale silence became a shared BML stage surface.
