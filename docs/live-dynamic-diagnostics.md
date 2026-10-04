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
restoring old execution state. The [native resource event owner](../receipts/2026-09-28-native-resource-events.md)
feeds Darwin vnode notifications and lease timers into this receiver. Quiet
waits perform no resource polling; registration changes preserve the owning
continuation. Attribute events can resume a permission repair even when the
existing size/mtime observation is unchanged.

The same event owner now dispatches pipe readiness, child exit and cancellation
timers alongside JIT care. Each subscription carries its receiving context.
Quiet turns do not probe child status or rebuild readiness arrays. Failed writes
retain the submitted bytes and accepted offset with their organ reading; process
exit and fully drained streams remain separate completion observations.
Native event receivers now enter the same offer/backtrack flow. A stopped
receiver keeps its event, captured version, context and findings while the rest
of the batch proceeds. Local options can complete its continuation immediately;
otherwise its readiness rests until a correlated response arrives. Each attempt
renews the observation, and explicit event release remains distinct from
receiver completion. The [event owner](../receipts/2026-09-28-native-resource-events.md) carries this
boundary for the existing pipe and JIT consumers.

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
bytes remain referenced. Organ framebuffer events carry the `organ-frame-v1`
projection: correlation, organ/flow/aspect, stage, health, surprise, declared
resource names, offers, selected action, attention and observation time. Live
steps also carry their measured operation duration. Expected/observed values,
need details, evidence and action results stay with the owning organ. Use public
operational names for the projected fields; private samples and conversation
content belong in owned evidence. The projection lets attention follow a need
through provider selection and fresh observation without copying that content.
Constructed JSON values do not automatically enter the framebuffer. Source
parsing retains its separately attributed syntax nodes; that is a distinct
source-observation path, not an organ event.
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
does not replay it. The reader lives in
`form/form-stdlib/bml/form-cli-heal-native-process.bml`.

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

## Steps run live

A running step meets its surprise where it happens, not in a band or a gate read
afterwards. `form/form-stdlib/bml/live.bml` runs a step inside `attempt`, so a
stop is an observation and not an exit, and speaks it at once on stderr as an
organ-health reading (organ `live`, the flow, the step, what it answered, whether
it held, what it offers). A step that does not hold is a choice point, and what it
answered chooses what happens next: each repair names the need it meets, and the
step is offered only the repairs that meet what it answered, so nothing is retried
blind. The offered repairs walk through `oac-backtrack-walk`: each acts, is spoken
as applied, and the step runs again in the context it made. A repair that cannot
act earns no retry. A surprise no repair meets is spoken with its need and handed
back to the flow.

The flows run on it:

- The walk's carry (`form/form-stdlib/bml/host-walk.bml`, `hw-carry`): follow
  main, read the gates, fast-forward main, each a live step. A fast-forward
  refused because main moved meanwhile is met at once: the branch follows main
  again, the gates read again, and the push goes again, twice at most. A push
  refused for any other reason, or a rebase that conflicts in a file the body
  does not know (named, the rebase aborted), meets no repair and holds. A rebase
  that stops on a river of rows or on a page the body draws is settled inside the
  follow itself (`host-walk-settle.bml`, the reunion's rounds): the sync says
  `healed:` and `redrawn:` before `sync=0`, and the turns go on.
- The gates (`gate/drift-gates.bml`): a landing runs only the rows whose ground
  moved since origin/main. Each kernel row names the paths it guards; a row whose
  ground no path touches sits out, said on its own line and absent from the fold.
  structural-gate, binary-freshness, ground, door-links and release-ledger guard
  the whole tree and always run. A stale binary is the one need a gate's repair
  meets (`rebuild-fkwu`). A gate that did not arrive reads nothing, not a code.
- The body's own turn (`observe/native-turn-run.bml`) runs its band as a live
  step against the gap's full: a band short of full needs code, and the repair
  that meets it is the turn's own walk through the voice, which hears each step
  of the band that did not hold.

Two carriers meet their surprises the same way outside `live.bml`:

- The landing (`form/form-stdlib/bml/form-cli-landing.bml`, `ld-land`): the
  witness runs first as an argv child (`/bin/sh -c <witness>`, through
  `form-stdlib/bml/host-child.bml`, `hch-run`), and its exit is the host's own answer
  (`host_wait`), never a printed mark: a witness that prints `[witness] exit=0`
  cannot answer for its own exit, and the line the landing shows ends with the
  exit it read. A named witness that does not exit 0, or whose launch was refused
  at birth (no exit arrived), holds the landing, and the report says
  `held: the witness refused (exit N)` (`form-cli-landing-band` 262143). The walk
  reads its own children the same way: a turn, plan, review or gate has an exit
  from `host_wait` or reads nothing with its reason (`[host-child] deadline: ...`).
- The sweep (`./fkwu gate/band-sweep-run.bml`, `form-stdlib/bml/band-sweep.bml`):
  every leg is one `cell_run` call in the sweep's own process with a deadline of its own
  (1800 s, `ceiling_s`); it answers a record, `{value, out, diag, stopped, errors, resident, ms, cpu_us}`
  (`docs/in-process-cells.md`), so the diagnostics of a leg are the same organ-health rows as
  values in `diag` (no JSON line is parsed back), a stop is `stopped` with the recipe and the
  unit, and the deadline is the call's: a leg past it comes back stopped "deadline", its band fails
  naming itself, what it printed and said is kept as its evidence under `.hearth`, and the sweep
  moves on. The work is shared by shard (`{"shard":i,"of":n}`), not by children. A parent that reads a child's stderr for
  `form-organ health` rows, its last stdout line as a number, or its exit, is reading a text
  protocol the callee could have returned as a value; add no new one.

## A process that dies by a signal says so

A SIGSEGV, SIGBUS, SIGILL, SIGFPE or SIGABRT no longer ends the seed in silence (rc 138/139 and an empty stderr was the sweep shard that lay dead
for fifty minutes). Each thread's alternate-stack handler writes one organ-health row at the moment of death, to stderr as a `form-organ health` line
and appended to `.hearth/fatal-signal.jsonl` (`runtime/fkwu-uni.c`, "THE FATAL-SIGNAL ORGAN"): organ `fkwu-seed`, aspect `fatal-signal`, health 0,
surprise 1, the need `fatal-signal`, the offers `request-evidence` and `revise`; its evidence is the signal, the fault address, the resident bytes,
the `cell_run` units in flight, the walker's frame chain (function and unit, innermost first), the collector's depth and the C chain with symbols.
Then the signal is raised again and the exit stays 128 + n. A stack that ends is a stop before it is a signal: `cell_run` answers `stopped` kind
`"stack-depth"` with the same frames and the process stands (`docs/in-process-cells.md`, "A stack that ends is a stop").

The row reaches attention through the care every organ reading takes. `form/form-stdlib/bml/fatal-signal-attend.bml` speaks each retained group
(the last 24 hours; one group per signal and innermost frame, counted; at most six listed) as a `form-organ health` line, which enters the
framebuffer through `oh-frame` (organ-frame-v1: health 0, the declared need, the offers), and hands it to `oc-hear`: no provider claims a fatal signal,
so the attention row says `unclaimed` (the repair is code, read from the frames). `observe/local-flow-review.bml` runs it before its review, so the
next flow's reading opens with what died. The readers that judge a leg take a signalled leg as a finding, never a bare number:
`form/form-stdlib/bml/host-child.bml` sets the finding (the signal, the cell, the frames) at the end of a child's text and in `<io>/<step>.finding`
when its exit is 128 + n; the band sweep and the drift gates (`form/form-stdlib/bml/band-sweep.bml`, `gate/drift-gates.bml`) say the last day's findings
before their first leg and read a `stack-depth` leg as its stop with its frames. Proof: `form/form-stdlib/tests/fatal-signal-band.fk`
(16383), which begins the faulting seed on purpose through `hch-run-in`, the one band whose subject is a process's death.

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
`observe/tests/import-carry-band.fk` is the regression band for this pulse. It
makes the pair's home itself, a fresh directory under `temp_dir` holding copies
of the fixture unit and floor, and asks a child `./fkwu` to walk the pair there
twice: cold through the import-lane door (door 1) and warm through the cached
door (door 2). It answers 255 whatever images another build left in the
checkout. A pair compiled beside a fixture image that a different fkwu build
wrote reads 15 on that run: stderr names the foreign `.fkb`, the import lane
steps aside, and bits 16 and 32 (the pulse) do not hold until the next run. The
band's own home never holds such an image; a copy of the band that lets another
build walk the home first reads 143.

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
