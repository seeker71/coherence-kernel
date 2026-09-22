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

Health and care share the retained reader. Care and Glass construct ages,
source metadata and report composition as reclaimable Form values through
`json-view.bml`, borrowing immutable observation nodes with an explicit tag.
They serialize those values directly. `occ-view-request-at` returns this view;
`occ-request-at` explicitly materializes primary nodes for a caller requesting
content identity. The working lists and strings remain visible to the current
collector. A view creates no new record owner; callers retaining a view in a
record release that reference explicitly because seed records remain roots.

The [current observation](evidence/fkwu/care-view-lifetime.json) includes eight
alternating comparisons preserving all fields except elapsed time and derived
age. Complete warmed explicit-source commands, including request parsing and
serialization, mint zero primary nodes in that workload. Retained views remain
exact through observed collection while temporary strings are reclaimed.
Send a nonempty retained organ JSONL path on stdin to
`./fkwu observe/form-care-view-witness.bml` to re-observe this boundary. The
witness observes actual collection within its finite exercise budget; its
sample count does not constrain the reader.

New event admission, discovery, reader records, framebuffer publication and the
collector still depend on the seed. Default discovery and newly arriving events
are outside the zero-mint observation. Complete native ownership and signaling
when primary allocation cannot grow remain open.

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

## Coding reply syntax

The coding organ observes malformed reply syntax through `organ-health.bml`.
The shared JSON validator retains the first failing byte and expected token.
Its live boundary emits the need, applied action and a correlated observation;
reply content stays in the retained reply file.

For a typed tool command, an extra mismatched closing delimiter immediately
before the expected delimiter can be removed, or the missing expected closer
can be inserted before the mismatched closer. Exactly one of these single-byte
candidates must pass strict JSON admission. Every other byte stays unchanged;
unfinished output at end-of-input is never completed. The command
then enters the existing role, writable-path, guarded-edit and verification
rules. `native_json_repairs` in the result and checkpoint identifies the byte,
before/after hashes and actual tool/check counts. This establishes syntax
recovery; completion still depends on the original caller checks.

Other malformed replies receive precise correction feedback. Repeated syntax
failures are counted; rejected replies execute no tools. A later valid reply
clears that syntax need. Ordinary valid replies emit no syntax-health rows.
The [retained native coding failure](../receipts/2026-09-22-native-json-attention.md)
shows an actual tool recovery and the remaining implementation refusal.

## Guarded edit care

Resident edit failures carry `guard_evidence`: actual match count, whether the
resident documents changed, and a bounded byte comparison of the old argument
against the whole resident source. The comparison identifies its alignment;
it does not select an approximate edit. Existing error codes stay intact.
An existing-path `write` refusal repeats the create-only contract. After either
guard failure, the next `read` of that same path supplies full current bytes
even when those bytes were already supplied at admission. Other reads retain
their normal context references. Tool evidence stays in the private coding
context and does not assert a filesystem mutation.

`edit` also accepts `[path, "sha256", expected_hash, new_whole_document_text]`.
Single-document `read`, `cat` and guard failures supply `resident_sha256` for the
exact resident bytes. The tool checks that hash before changing any byte; a
stale identity returns `edit-sha256-mismatch` and preserves the document. This
form avoids repeating the old source inside a whole-document replacement.
The same role, writable-path, review and caller-verification checks still apply.
It edits the resident document; publication remains the caller's responsibility.

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
