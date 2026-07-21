# Live dynamic diagnostics through the bidirectional framebuffer

The framebuffer is no longer only an append/read log. The kernel still supplies
the small carrier—`fb_record`, `framebuffer-events`, `node_source`, and
`framebuffer-clear`—while Form supplies a bidirectional protocol that can change
the next selected execution state and then observe that state again.

## The loop

```text
execute → observe → outbound frame → adjudicate → inbound control
   ↑                                                    ↓
   └──────────── re-observe ← apply/actuate ←───────────┘
```

Every message in `observe/bidirectional-framebuffer-channel.fk` carries:

```text
[direction, exchange-id, kind, payload, alternative-node]
```

- `direction`: `0` outbound observation, `1` inbound control;
- `exchange-id`: correlates the response with the observation it addresses;
- `kind`: transition observation or execution control;
- `payload`: measured information or a control action;
- `alternative-node`: the explicit result for nothing, timeout, or mismatch.

Current control actions are:

| Value | Action |
|---:|---|
| 0 | continue |
| 1 | branch |
| 2 | revise |
| 3 | abstain |
| 4 | request evidence |
| 5 | rehearse ground |

The action vocabulary is extensible Form data. A consumer must implement the
actuator for every action it admits and must re-observe the selected state.

## When every future session uses it

Use a bounded bidirectional diagnostic window whenever execution encounters:

- `nothing`, no result, timeout, stall, or an unresolved call;
- a result that differs from the prediction;
- a regression, tamper rejection, or failed success vector;
- a learning/model/state update whose effect is being claimed;
- a choice between retry, alternate node, evidence request, revision, abstention,
  or restoring/rehearsing established ground.

Do not stop at an aggregate. Put the smallest causal boundary available into the
outbound payload: per-row transitions, stage/resource counters, changed node ids,
source coordinates, margins, or hashes. The inbound message must select a real
next action. Then re-run and emit a second outbound observation. A logged event
without an applied response is observation, not bidirectionality.

For ordinary successful work with no meaningful branch or surprise, a new window
is optional. The practice exists to increase diagnostic resolution, not to add
ceremony to every command.

## Fast checkout witness

After the normal ground and freshness checks:

```sh
./fkwu --src observe/tests/bidirectional-framebuffer-channel-band.fk
```

Expected final field: `1`. This is a fast protocol regression, not learning
evidence.

## Real learning integration

```sh
./fkwu --src observe/tests/bidirectional-framebuffer-learning-band.fk
```

The integration trains the existing language learner and feeds its real per-row
transition observations through two control rounds. It is intentionally slower.
The witnessed vector is documented in
`receipts/2026-07-22-bidirectional-framebuffer-channel.md`.

## Integration pattern

1. Clear only at the beginning of a bounded diagnostic window.
2. Record the observation with source attribution.
3. Emit an outbound envelope with a fresh correlation id.
4. Adjudicate from observed fields; preserve `nothing` when evidence is absent.
5. Validate direction, kind, and correlation before applying a response.
6. For no response or mismatch, select the offered alternative node.
7. Apply the action to execution—not merely to a report string.
8. Re-observe the resulting state in the same window.
9. Compare before/after at row or stage resolution before naming a cause.
10. Store a receipt when the behavior becomes relied-on ground.

## Boundaries and safety

- Framebuffer events should carry opaque ids, measurements, hashes, and source
  coordinates—not prompts, answers, secrets, or private content.
- Keep windows bounded; do not turn the framebuffer into an unbounded transcript.
- Correlation prevents a stale response from controlling a new observation.
- An alternative node is required for nothing/timeout/mismatch paths.
- Replay integrity does not prove semantic truth.
- Today the controller is synchronous Form policy. Asynchronous external writes,
  learned control policy, and direct weight actuation remain future layers.

## Canonical files

- `observe/bidirectional-framebuffer-channel.fk` — protocol and actuator.
- `observe/tests/bidirectional-framebuffer-channel-band.fk` — fast protocol band.
- `observe/tests/bidirectional-framebuffer-learning-band.fk` — real integration.
- `observe/thought-framebuffer.fk` — token/margin trace and divergence helpers.
- `observe/framebuffer-runtime-observation.fk` — richer runtime/stage observation.
- `form/form-stdlib/form-cli-surface-inquiry.fk` — bounded CLI read/inquiry surface.
- `cognition/native-cognition-cycle.fk` — full knowledge → inquiry → awareness →
  recognition → action → response → routing composition, including a
  representation-diverse recognition witness.
