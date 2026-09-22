# Native care from organ signals

The [current source execution](evidence/fkwu/native-core-care.json) observes 13
resource scenarios, actual diagnostic care while children run, and two resident
CLI reads that preserve open needs. The
[installed CLI execution](evidence/fkwu/native-startup-care.json) reaches the
same care flow through its compiled default entry, without a source argument.
Its two reads consume 44,397 event bytes and then zero, preserving three signals,
two open needs and one supplied resource. The child exits zero with empty stderr;
its process group and both declarations are released. These observations bind
the actual runtime, source and companion artifact bytes.
Field recovery must be launched from a dedicated working directory with
`FK_FIELD_OFF 1` in its `fkwu.conf`, keeping working memory outside the shared
field. The door does not set or enforce that configuration. Every other
kernel must settle before reset. The Form admission predicate accepts only a
singleton roster containing this process's positive integer PID. Empty readings,
missing self, additional owners and malformed values refuse. The
[pure admission observations](evidence/fkwu/field-reset-admission.json) exercise
that predicate without resetting a field. The registered-kernel roster remains
bounded; it cannot prove complete host liveness. A refusal prints the observed
roster and exits nonzero. Recovery
does not change the normal runtime's shared-memory configuration.

Executing organs send signals when they encounter unease, uncertainty or a
needed resource. `organ-care.bml` receives those signals at their execution
boundaries, directs attention to each need, and calls an available native
provider whose action the asking organ offered. The asking organ observes what
arrived and what remains needed. Care follows the signal without a central
schedule of health checks.

The core `care` command and `./fkwu observe/form-core-care-run.bml` show this
exchange: current unease, directed attention, needed resources and supply
outcomes. Glass uses the same view. Viewing the exchange does not invoke an
actuator, a model or a remote service. The `health` command retains the underlying
observation format. Care policy lives in Form; this interface adds no
handwritten C.

An organ emits its observation and calls
`oc-hear(context, reading, providers, emit, observe)`. Providers and callbacks
are supplied by executing Form code. Resource names and offered actions are
open data; the care organ has no fixed list of organs, errors or verdicts.
Each declared need receives attention. An unmatched need stays open. Actual
supply, a correlated new observation and recovery remain distinct. Current
observations retain their provider and delivery evidence so the interface can
show what nourishment reached its owner.

`supply_outcomes` retains the requested need, provider and actual callback
result. `awaiting-organ-observation` means supply returned but the organ has
not supplied a fresh reading; `organ-reobserved` means it has. Neither state
interprets an arbitrary provider result as successful delivery or recovery.
Attention annotations bind to their original observation identity. A late
annotation cannot replace the organ's newer reading.

An organ declares its event-file reference when it creates that flow. The native
process runner and session memory do this at their existing lifecycle boundaries.
Other organs can use the same `organ-health-discovery.bml` API:

```text
ohd-register(root, source, capabilities)
ohd-renew(root, source, capabilities)
ohd-retire(root, source)
```

Registration returns its actual publisher, generation and carrier outcome.
Retained callers can pass an explicit `ohd-open()` owner to the corresponding
`*-with` functions. The short lifecycle API recovers its exact process-birth
frame when called again; allocator compaction cannot silently discard it.
Explicit owners add the observed record-construction counter to that birth
identity. They do not derive identity from a record's display string.
The shared `native-owner-clock.bml` owns that counter reading. Lifetime health
retains its own birth-and-construction identity on the resource owner, so
releasing one context cannot replace another context's unresolved observation.
These identities are cooperative within the current process; they do not
establish concurrent record access or an address-space boundary.
Unchanged explicit-owner registration performs no shared-memory operation.
The short API reads the shared frame but skips an unchanged publication.

Discovery transports references and capability strings, not event contents.
The existing local shared-memory roster has 511 entries, 119-byte names and
least-recent-registration eviction. Reports expose those limits and any carrier
refusal. Coverage is **currently advertised**, not every organ. Retained frames
can outlive their publisher; PID existence, source availability and observation
age remain separate. Cooperative declarations are not an isolation boundary.

The resident CLI holds an explicit reader context across commands. Each source
gets a positive, caller-adjustable work budget, 65536 bytes by default. JSONL
records enter the current reading only after their terminating newline arrives.
An unchanged flow reads zero history bytes. Backlog, incomplete records,
malformed records and absent sources remain visible needs. A reader retains the
unfinished record and current organ map; the work budget is not a memory cap.

```text
care status
care {"sources":["/absolute/events.jsonl"],"budget_bytes":262144}
```

Explicit `sources` selects paths under a declared append-only stream contract;
the reader does not verify ownership of those paths. Otherwise discovery
uses the current working directory as its root. A standalone invocation opens a
new snapshot; retained native callers use `ohc-open()` and `occ-command-at`.
The reported elapsed time covers discovery, event reading and projection up to
the timing field. Request parsing and final JSON serialization sit outside that
interval. Read-byte counters
describe event payload consumption, not shared-memory discovery traffic.

Health and care share the retained reader and each projects its requested result
directly. Eight alternating-order paired calls over the same retained exchange
preserve every output field except elapsed time and derived observation age.
The [retained paired observation](evidence/fkwu/native-core-care.json) records
269 nodes for care versus 317 through an intermediate health view; both consume
zero repeated event bytes. Measurement brackets each actual call,
excluding output serialization and comparison. Changing observation values still
allocate nodes; pressure-resilient signaling and reclamation require further work.

Shrinking files reset the reader. A known publisher's renewed generation also
resets it, including replacements of equal or greater size. A new publisher at
an already read path resets by default. The declared
`append-only-shared-source` capability permits a new writer to join an unchanged
shared append flow without rereading history; session memory uses that contract.
Unannounced equal-size replacement, inode aliases and undeclared writers remain
outside this append protocol.

The result keeps pain first and unknown health or open resource needs visible.
Every projected organ includes its source state and observation age. Applied
care preserves the original observation's health and time. Recovery requires
the owning organ to re-observe its actual result. Offered responses are data;
the core never executes a command supplied by an event.

Every newly read nonempty stderr span in the process organ produces a live
`diagnostic-availability` signal asking for evidence and interpretation. Its
local provider supplies owned diagnostic bytes, and the organ reads them again
to observe delivery. Evidence received does not establish interpretation,
semantic progress or recovery. Exit, release and the child's own observations
keep their meanings. Raw diagnostic text stays private, outside the framebuffer.

Verified session memory uses the same care entry when the learner needs
admitted training evidence. Receipt of eligible evidence leaves loss recovery
to a subsequent measured learning outcome. Assessment exclusions remain active.

Healing defaults to local routes and requires an explicit `remote` field before
even probing an external assistant. Each retained attempt has an actual local
experience identity; frontier returns bind that evidence and real execution
outcome. Retention, checked success, a learner update and serving improvement
remain distinct. See [healing](form-cli-healing.md) and
[session learning](native-session-learning.md).

The view's zero model/remote counts describe that read only. They are
not global crossing totals. Full discovery of every organ, interpretation of
every diagnostic, comprehensive resource-cost routing and measured outward
gifts remain the [north star](fkwu-form-native-north-star.md). The active care
callbacks run synchronously in their owning process. Cross-process declarations
make their exchange visible; they do not grant the view remote actuators.

## Review context care

When a review lacks a caller or ownership contract, the executing caller can
offer selected source sections to
[`frcc-prepare`](../form/form-stdlib/bml/form-cli-review-context-care.bml).
It emits the coverage gap, calls `oc-hear`, supplies the declared sections and
re-reads them before returning the packet. An unavailable or changed selection
leaves the need open and returns `nothing`. The source packet stays outside the
shared events; those events carry coverage and delivery metadata.

The stdin door is:

```sh
form-run ./fkwu observe/form-cli-review-context-care-run.bml < review-context-request.json
```

The caller supplies one JSON object with nonempty string fields `flow`, `events`,
`before_path`, `goal`, `packet_path`, and a nonempty `parts` array. Each part is
`[path, opening_marker, following_closing_marker]`. Source selection includes
the opening marker and stops before the closing marker. Paths belong to the
caller; relative paths resolve from the current directory. Create the output
directories first and choose a fresh packet path. The door writes that packet,
checks its retained bytes and returns metadata. An unavailable packet or failed
write exits nonzero. Callers must check the exit before consuming the output.

The [actual ownership review request](../receipts/artifacts/2026-09-22-prefill-code-review/context-care-request.json)
and [receipt](../receipts/2026-09-22-prefill-code-review.md) carry its first use.
The caller selects what the review needs; the cell does not discover a complete
call graph or judge the answer. This door invokes no model or remote service.
Its supplied packet can serve the next model call or a retained-answer check.
