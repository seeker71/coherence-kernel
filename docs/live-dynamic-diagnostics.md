# Live dynamic diagnostics through the bidirectional framebuffer

The framebuffer is a bidirectional channel, not only an append/read log. The kernel supplies
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
| 6 | admit candidate |
| 7 | defer candidate |
| 8 | reject candidate |

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

## Health belongs to the running organ

`form/form-stdlib/organ-health.bml` carries a shared event language. An organ
names its identity, flow and aspect; its expectation and observation; its own
health reading (`1`, `0`, or `null` for unknown); needed resources; offered
responses; and source evidence. The transport does not maintain an error-name
catalogue or decide what health means for another organ.

The process organ observes its actual exit and owned-process release. The
generation organ senses output-byte delivery, prediction refusal and context
pressure where they occur. The session learner reads each actual before/after
loss and asks for rehearsal or related training evidence when it needs care.
Held-out assessment rows remain excluded from training. These observations
flow during execution, without a separate fixture tally.

Responses correlate with the organ's observation and offered action. Applied
care retains that observation's health; only a new observation can change it.
The generic reader retains a mismatched response as a request for the missing
correlated observation. A healthy process exit cannot erase another organ's
pain. Resource names are open data, so an unfamiliar organ can ask through the
same flow. A request does not claim the resource was supplied.
Readings retain their observation time and expose their age. Recording care
does not refresh the health observation's age. The latest recorded reading is
an observation at that time, not a claim of continuous monitoring afterward.

The native process runner consumes `form-organ health` events while its child
runs, including a final line without a newline. It gathers available evidence
when offered that action and leaves unsupported needs open. Original output
bytes remain referenced; framebuffer events carry only opaque numeric data.
Stage timing and choices retain their actual observations. The current JSON
timing report projects the health of each organ/flow/aspect.

Read a flow by sending its event-file path to
`./fkwu observe/organ-health-run.bml`. The reader holds one input chunk, the
unfinished event and the current organ map. An empty flow yields no readings;
it cannot establish that every organ is healthy.

`form/form-stdlib/organ-care.bml` connects a resource need to a native provider
callback and the asking organ's observation callback in the same process.
Providers declare supported resources and an action the asker must have offered.
The response names the provider before it acts; delivery records actual supply
time. The asker re-observes through its own callback, with the original identity
linked as `care_of`. The carrier accepts no callback or executable command from
event text. Its first production provider is verified session memory, consumed
by the learner during normal preparation and worker drains. Resource delivery
and loss recovery are separate observations.

`./fkwu observe/form-cli-heal-process-run.bml` accepts one JSON request with
`argv`, optional `input`, and optional `seconds` (zero means dynamic progress).
It runs that real command through the process organ and returns its actual
status, evidence directory and health readings. This is a general execution
door, with no fixture list or expected verdict.

## Kernel protocol witness

After the normal ground and freshness checks:

```sh
./fkwu observe/tests/bidirectional-framebuffer-channel-band.fk
```

Expected final field: `1` (re-run 2026-09-04). This is a fast protocol regression,
not learning evidence.

## Real learning integration

```sh
./fkwu observe/tests/bidirectional-framebuffer-learning-band.fk
```

The integration trains the existing language learner and feeds its real per-row
transition observations through two control rounds. It is intentionally slower.
The witnessed vector is documented in
`receipts/2026-07-22-bidirectional-framebuffer-channel.md`.

## The admission pulse: which door did this run enter through

The kernel records how the running program itself was admitted, and the
program reads it back through the same `kernel_stat` door as the dispatch
and pool counters — always on, no toggle:

| Key | Reading |
|---:|---|
| 15 | door: `0` flat whole-program compile, `1` import lane, `2` cached image replay, `3` native `.dylib` |
| 16 | units imported as standalone `.fkb` images (door 1) |
| 17 | units carried as source beside the imports (door 1) |
| 18 | why the import lane last stepped aside (`0` it did not; codes in `runtime/fkwu-uni.c` at the counter declarations) |

This exists because a whole wound family (icetide, corpus 1217) reproduced by
door decision rather than by source bytes, and the only witness was a static
conf toggle printing to stderr — unreadable by the program and uncorrelatable
in a diagnostic window. When a run surprises, put `(kernel_stat 15)` through
`(kernel_stat 18)` into the outbound payload before bisecting bytes.
`observe/tests/import-carry-band.fk` is the regression band for this pulse: 63
through the import-lane door cold and the cached door warm (re-run 2026-09-04; it
prints its verdict and then the `.bml` floor's trailing `0`, so read the first
line, not the last).

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
- `cognition/native-self-orientation.fk` — derive floor and north-star invariants,
  predict two movements, walk the first available movement, and re-orient from
  the resulting live witness without clearing the parent diagnostic window.
- `cognition/native-node-ontogenesis.fk` — derive a candidate node from live
  unresolved evidence; build its blueprint, executable recipe, language-neutral
  identity, names, and seven inquiry interfaces; validate, admit/defer/reject,
  test held-out transfer, and re-orient from the changed recognition floor.
- `cognition/tests/native-node-ontogenesis-band.fk` — end-to-end admission and
  reversibility witness.
- `cognition/native-three-round-walk.fk` — sequentially admit, defer, or reject
  evidence-derived proposals while carrying explicit `[native, local, remote]`
  routing vectors between rounds.
- `cognition/tests/native-three-round-walk-band.fk` — three-round source and
  adjudication replay.
- `cognition/concept-crystallization-contract.fk` — an offered readiness profile
  whose node may move among gas, water, and ice while retaining content identity;
  exposes aliases, recipe, composition, lineage, inquiries, transfer, freshness,
  axiom compatibility, and an abstaining frequency reading when unmeasured.
- `cognition/tests/concept-crystallization-contract-band.fk` — ready/candidate
  facets and gas/water/ice identity-stability witness.
