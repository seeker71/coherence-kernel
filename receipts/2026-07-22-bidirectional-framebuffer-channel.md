# 2026-07-22 — the framebuffer became a bidirectional channel

## Ground

Fresh checkout witnesses returned `42`, `55`, freshness `15`, and
`[1, 2.5, [3, 4]]` before the channel ran.

## Build

The kernel primitives remain unchanged: `fb_record` appends attributed nodes,
`framebuffer-events` reads them, and `framebuffer-clear` bounds the window.
`observe/bidirectional-framebuffer-channel.fk` adds a Form-native membrane:

- typed outbound observation and inbound control envelopes;
- a correlation id shared by both directions;
- actions for continue, branch, revise, abstain, request-evidence, and
  rehearse-ground;
- an actuator whose inbound action selects the next observed state;
- an explicit alternative node for missing or mismatched responses.

No capability was added to `runtime/fkwu-uni.c`.

The fast future-session protocol witness is:

```text
./fkwu --src observe/tests/bidirectional-framebuffer-channel-band.fk
[nothing, 0, 1, 1001099007, 5, [0, 4, 17, 1], 0,
 [0, 4, 17, 1], 1001099097, 4, 1]
```

## Live witness

```text
./fkwu --src observe/tests/bidirectional-framebuffer-learning-band.fk
[nothing, 0, 1, 1001099006, 4, [2, 1, 15, 4], 5,
 [0, 4, 17, 1], 0, [0, 4, 17, 1], 1001099097, 4, 1]
```

The existing real adaptation witness supplies the observations. Round 1 sends
the error-only transition vector `[2,1,15,4]` outward. Because regressions exceed
recoveries, action `5` returns inward and the actuator selects the already-measured
rehearsal state `[0,4,17,1]`. Round 2 sends that selected state outward; action `0`
returns and the state remains stable. The two exchanges add four framebuffer
events. A missing inbound message selects node `1001099097` rather than becoming
an unrepresented timeout.

## Honest floor

This proves bidirectional information flow and control of state selection. The
adjudicator is still a bounded Form policy over transition counts; it is not yet
an asynchronously writable external channel, a learned policy, or a weight-level
actuator. Those are subsequent layers, not properties claimed here.
