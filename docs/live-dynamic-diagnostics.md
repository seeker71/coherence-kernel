# Live dynamic diagnostics through the bidirectional framebuffer

The framebuffer is a bidirectional channel, not only an append/read log. The kernel supplies
the small carrier—`fb_record`, `framebuffer-events`, `node_source`, and
`framebuffer-clear`—while Form supplies a bidirectional protocol that can change
the next selected execution state and then observe that state again.

## The loop

```text
execute → signal → local choice → act → observe → learn
             ↑          │                 │
             └──────────┴── next choice ──┘
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

## Follow the live signal

A surprise, missing result or changed resource invites a local choice. Carry the
smallest useful causal reading: the affected operation, source, observed state
and available action. Apply the response and observe what actually changed.
An aggregate can guide attention; the operation's own evidence guides repair.

The loop stays with the execution that raised the signal. Its original inputs,
resource owners, intent, completed effects and findings remain available to the
next choice. `oac-backtrack-walk` carries original arguments and option memory;
`oc-hear` calls the running organ's supplied provider and observation callbacks
with its retained context. Event text never creates another execution context.
Handle an available repair there immediately. If a needed resource is external,
retain the continuation with its owner and resume when its relevant signal
changes. An unchanged failure supplies no reason for another identical attempt.

The same practice applies while developing these organs: keep the repair in the
active conversation and working context. Token savings do not justify another
chat, subagent or fresh model session that must reconstruct the same problem.
Put the expectation and its response at the executing boundary; retire a
redundant regression band after observing that replacement. Required release
checks remain independent verification of the resulting behavior.

For ordinary successful work with no meaningful branch or surprise, a new window
is optional. The practice exists to increase diagnostic resolution, not to add
ceremony to every command.

The demand-JIT owner receives cache care directly through `bdjo-hear(owner,
response)`. A response made with `oh-response(reading, "publish-retained-image")`
selects that exact waiting operation. Publication and readback run immediately
with its retained image and runtime identity, including while telemetry waits.
A failed attempt backtracks locally and retains a fresh reading; an old response
cannot replay it. Explicit holds remain owned by their release control. The
ordered telemetry queue carries observations without delaying this care or
restoring old execution state. The [native resource event owner](native-resource-events.md)
feeds Darwin vnode notifications and lease timers into this receiver. Quiet
waits perform no resource polling; registration changes preserve the owning
continuation. Attribute events can resume a permission repair even when the
existing size/mtime observation is unchanged.

## Health belongs to the running organ

`form/form-stdlib/organ-health.bml` carries a shared event language. An organ
names its identity, flow and aspect; its expectation and observation; its own
health reading (`1`, `0`, or `null` for unknown); needed resources; offered
responses; and source evidence. The transport does not maintain an error-name
catalogue or decide what health means for another organ.

`oh-reading` derives a fresh identity from the native process owner clock.
An organ that already owns a unique retained event may instead call
`oh-reading-with-id(id, organ, flow, aspect, expected, observed, health, needs,
offers, evidence)`. This keeps the same event schema, transport and response
correlation without requiring host process-birth inspection. The caller owns
uniqueness and replay identity: use a retained event key in an exclusively
claimed run, not a reused label. An empty identity is refused. This is an
explicit identity contract, not a fallback that guesses a process birth time.

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

Every newly consumed nonempty stderr span also emits a live availability
observation, even when it has no recognized prefix or final newline. Its health
and interpretation stay unknown. Exact byte ranges and private evidence
references support a correlated evidence request while the child is running.
Diagnostic availability does not count as semantic progress or decide the
child's result. Exit and resource release retain their separate observations.

Final output draining has its own bound as well. After a child stops, each
stream receives the number of final read slices named by
`hearth-process-final-drain-chunks`. A large backlog stays in its original raw
file. The process record's `output_drains` fields distinguish complete byte
traversal from deferred work, retaining the byte offset, partial-line length,
raw size and remaining bytes. A `process-output-drain-deferred` event exposes
that boundary; process release does not imply all output was interpreted.

Native callers can resume with `fhn-retained-reader` and `fhn-drain-final`.
The saved cursor reconstructs a partial line from the retained raw bytes.
The final line is observed even without a newline, and a completed cursor
does not replay it. The witness is
`form/form-stdlib/tests/form-native-process-drain-band.bml`.

The native care organ receives unease at execution boundaries and routes
attention and offered resources immediately. The core `care` command and Glass
care door view that exchange through retained native readers. They expose
observation age, source state, incomplete delivery and open resources. The
[native care contract](native-core-care.md) defines signal handling, discovery
coverage, stream generations and caller-adjustable work budgets.

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

Keep source attribution and a fresh correlation identity with the observation.
The receiving owner selects from its offered actions, preserves absent or
mismatched evidence, and observes the action's result in the same context.
Retain a small receipt when that behavior becomes relied-on ground. Clear the
framebuffer only when opening a new bounded window, preserving an active walk.

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
