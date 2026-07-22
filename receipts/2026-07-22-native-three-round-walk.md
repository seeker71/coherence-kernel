# Three native learning rounds — witnessed 2026-07-22

## Accounting

Every source vector is `[native recipe routes, local evidence routes, remote
oracle routes]`. “Local” is not shell execution: it is a route whose answer still
comes from the on-device evidence/rehearsal path. No network or remote model is
called by this witness.

## Result

```sh
./fkwu --src cognition/tests/native-three-round-walk-band.fk
```

```text
[nothing, 0, 1, 1001099300,
 [2000830772, 3, 6, [16, 6, 0], [19, 3, 0], 2],
 [2001099302, 1, 7, [19, 3, 0], [19, 3, 0], 1],
 [2001099303, 2, 8, [19, 3, 0], [19, 3, 0], 0],
 1, 2, 6, [19, 3, 0], 1]
```

### Round 1 — admit

The existing content-derived independent-consensus candidate has three live
supporting observations, including held-out transfer. Action `6` admits it.
The source vector changes from `[16,6,0]` to `[19,3,0]`.

### Round 2 — defer

The residual evidence proposes a three-way-disagreement recipe. It has one
supporting observation and therefore no independent transfer surface. It remains
lesson `1`; action `7` defers it. The vector remains `[19,3,0]`.

### Round 3 — reject

Two residual observations propose “base and rehearsal agreement defeats lexical
dissent.” Validation on one invites the rule, but the held-out observation
contradicts it: correct/support is `1/2`. Status `0` and action `8` reject/lapse
the proposal. The vector remains `[19,3,0]`; no false native gain is booked.

Six framebuffer events are present: one outbound observation and one inbound
control for each round. Final success is `1`.

## Surprise

The first replay produced all expected measurements but success `0`. The cause
was the success predicate using `eq` on separately constructed lists. Replacing
that assumption with component-wise vector comparison made the proof match the
already-correct observations. This changed proof logic, not measured routing.

## Honest floor

These rounds operate over the current 22-row recognition witness. Round 1 grows
native routing; rounds 2 and 3 improve adjudication by refusing unsupported or
contradicted recipes. They do not yet generate new evidence for the remaining
three local routes, and remote review is not exercised here.
