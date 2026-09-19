# Observe provider usage during a task

The optional Codex App Server protocol exposes `thread/tokenUsage/updated`
notifications with a thread ID, turn ID, latest usage and cumulative usage.
The [official protocol documentation](https://learn.chatgpt.com/docs/app-server)
names that notification. The installed CLI can export its precise schema:

```sh
form-run codex app-server generate-json-schema --out <owned-schema-directory>
```

The native owned-session door below consumes these events during execution.
The existing response-synthesis resource continues to use `codex exec --json`.
Native local generation runs on the C-bootstrap; an offered provider is an
optional resource for a bounded task.

## Native reading

`form/form-stdlib/bml/form-cli-provider-notification.bml` reads the exact
notification position. Start with `fpn-empty(thread, turn)`, offer complete JSONL
rows through `fpn-line`, and read `fpn-report`. `fpn-lines` preserves an unfinished
row between chunks. `fpn-read` reads a bounded prefix of a captured event file.
The command door takes three stdin lines: owned thread ID, owned turn ID and
captured App Server stdout path.

```sh
form-run ./fkwu observe/form-cli-provider-usage-run.bml
```

The reader selects the latest validated cumulative snapshot; it never adds
snapshots together. Another thread or turn, or a usage-shaped value nested in
tool output, cannot update that state. A regression or malformed matching
snapshot retains the preceding observation and its new error status; later
events do not silently restore validity. Missing quantities remain null.
The optional cumulative cache-write counter retains its last known floor even
when intervening snapshots omit it. Its reported value remains null during
that absence; a later decrease below the known floor exposes a regression.

The reported scope is cumulative thread usage observed during the selected
turn. It may include earlier turns of that thread. `whole_session_tokens` and
`model_calls` remain null: notification count is not request count, and a
snapshot cannot account for unfinished or unreported work. The report includes
cached, uncached, reasoning and unattributed quantities separately. A caller
must retain the source and lifecycle evidence before using it in a comparison.

`fpn-contract()` offers the reader's field roles as native JSON for context
preparation: selected identities, cumulative `total`, per-call `last`, the
defined input/cache-hit partition, optional cache writes and measurement
scope. Offer this meaning alongside source when asking a model to reason
about the reader. It states the intended contract; executing the reader and
examining the answer remain separate observations.

`cache_write_input_relation` is explicitly null. This contract supplies no
subset or additive relationship between cache writes and input tokens.
Absence of that relation neither establishes nor disproves a provider-specific
relationship. A proposed additional check needs its supporting evidence before
it becomes a defect finding. The contract's `relation_scope` carries this
distinction into native review context.

`observe/form-cli-provider-usage-probe-run.bml` takes the exported notification
schema path on stdin. It executes one valid two-event sequence per declared
usage field: the selected cumulative quantity decreases by one and the other
quantities stay equal. The report names accepted decreases, detected regressions
and fields without an available probe value. This small observation can supply
concrete evidence before model review; it does not certify complete coverage.
The report's `answer` is deterministic wording from those observed rows,
produced by `fpup-explain()`. It preserves the probe's coverage boundary and
does not represent a model review.

```sh
form-run ./fkwu observe/form-cli-provider-usage-probe-run.bml <<'SCHEMA'
receipts/artifacts/2026-09-20-provider-notification-schema.json
SCHEMA
```

## Replay a proposed finding before accepting its trigger

`observe/form-cli-provider-usage-replay-run.bml` accepts one JSON request or
`@request-file` on stdin. The request has nonempty `thread_id` and `turn_id`,
an `events` array of complete notification objects, and optional nonempty
`claimed_status`. It executes those events in order through the linked
`fpn-event`, retaining each parsed event and its before/after status. The
report includes the actual final reading and `claim_matches` (null when no
claim was supplied). Requests are bounded to 64 events and 131,072 bytes.
The file door reads at most that bound plus one byte.

The linked reader is the source version loaded by this fkwu process. A caller
reviewing a historical source must execute that source version and retain its
identity; presenting historical text to the current reader does not change
which implementation runs. The replay starts no model, provider or server.

The existing `form-cli code` review door can apply the same observation on
report submission. Add this row to `report_checks`:

```json
{"kind":"provider-usage-sequences"}
```

The report supplies `findings`, with a `sequence` request as described above
inside each finding. A contradictory or absent status claim enters the
existing review repair path with actual per-event results. Other source and
report checks remain in force. Native callers can compose
`fpur-check-report` with their own review callback in the same way.

A match establishes the behavior of the exact events. It does not establish
a contract violation, accurate surrounding prose, or complete review coverage.
An empty findings array performs zero sequence checks and can still miss a
real defect. The result keeps `defect_verified` null.

## Native transport observation

`observe/form-cli-provider-protocol-probe.bml` takes one fresh owned directory
on stdin. On Darwin ARM64 it uses the existing Form-generated pipe machinery
to start `codex app-server --stdio`, send one `initialize` request and await its
correlated response. It then closes and reaps its owned process and pipes,
retaining the request, stdout, stderr and compact result in that directory.
This probe sends neither `thread/start` nor `turn/start`.

The initialization probe establishes reachability. The owned-session work
below separately observes generation, live usage and process-tree release.

## One Form-owned provider turn

`observe/form-cli-provider-session-run.bml` accepts one JSON request or an
`@absolute-request-file` line. Use the file door for evidence packets: the
native line reader carries at most 8,191 bytes, while the full-file door checks
complete JSON up to one MiB before starting a provider.

```sh
form-run ./fkwu observe/form-cli-provider-session-run.bml <<'FORM_REQUEST'
@/absolute/owned/request-to-offer.json
FORM_REQUEST
```

The request contains:

| Field | Meaning |
| --- | --- |
| `allowed` | Boolean `true`, recording the caller's offered provider resource. |
| `root` | Fresh absolute owned evidence directory, already created. |
| `cwd` | Existing directory beneath `root` for provider work. |
| `prompt` | Bounded question and relevant evidence. |
| `seconds` | Positive integer deadline for the entire supervised process. |
| `program` | Optional absolute provider CLI path; defaults to `codex` on PATH. |
| `dry_run` | Optional boolean; `true` opens a thread and inspects its configuration without requesting generation. |

The current transport uses the existing Form-generated Darwin ARM64 pipes.
The native process organ owns the worker, provider and descendant process
group. The worker checks the returned cwd, workspace roots, additional write
roots, approval mode and network setting before sending `turn/start`. Paths
are checked lexically; this is an owned-workspace workflow, not a new
filesystem isolation mechanism. Host temporary-directory access follows the
provider's reported sandbox settings.

Form retains the request, correlated protocol events, first usage observation,
latest usage, command exits, answer, worker report and supervisor lifecycle.
Missing command exits have their own count. `summary.json` and its evidence
hashes support replay: offering identical request bytes returns the retained
result with `provider_processes_new=0`. An admitted unfinished run stays
available for inspection; it is not silently restarted. Changed evidence or
request bytes cannot trigger another call through that identity.

Keep raw requests, events and answers in the local owned directory. The
command emits compact metadata. The latest completed agent-message text is
in `answer.md`; inspect `answer_phase` and `turn_status` before treating it as
a final answer. A successful process exit and a completed turn establish
execution facts; answer quality needs its own observation.

For source reviews, let native Form gather the relevant bytes and perform the
checks first. Offer that frozen packet from an empty owned working directory
outside repository ancestry when repository discovery is unnecessary. A
2026-09-20 trial completed with 34,267 reported tokens; the preceding
tool-driven review timed out with an 820,030-token observed prefix. These
different contexts establish the packet route's useful result for that task,
not whole-session parity. The [session receipt](../receipts/2026-09-20-provider-owned-session.md)
retains the attempts, scopes, repairs and open questions.
