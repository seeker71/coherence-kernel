# Changing observations have an owned value lifetime

The retained organ reader now parses and reduces events as tagged Form values.
Care and Glass render those values directly. Replacing the current observation
releases its old list/string references; a deliberately retained report keeps
its independent values. The handwritten C seed is unchanged.

One JSON parser selects either primary-node or value constructors. One health
reducer owns validity, extraction, observation time, attention and response
correlation. Existing node APIs wrap their inputs and explicitly materialize
their results. Ordered duplicate keys, first-key lookup, integer/float/boolean
distinctions, null, arbitrary nested evidence and ordered need equality survive.
Current updates copy surviving pairs instead of retaining an overlay chain.

Each reader owns a diagnostic identity and a monotonic sequence that survives
publisher renewal. Admission, reset and unmatched-control diagnostics reuse
that owner. The framebuffer still interns its timestamp; float boxes and the
collector still belong to the seed. Ordinary `health` text output still uses
the explicit node API. These are concrete remaining crossings.

## Observed execution

[Changing-event evidence](../docs/evidence/fkwu/care-changing-lifetime.json)
binds the selected source/runtime bytes and retained local write/readback events.
There are 32 append, renewal and quiet intervals:

| Interval | Primary mints | Record constructor dispatches | Read bytes | Elapsed ms |
| --- | ---: | ---: | ---: | ---: |
| Append | 0 | 0 | 18,432 | 52 |
| Declared renewal | 0 | 0 | 339,456 | 54 |
| Quiet | 0 | 0 | 0 | 28 |

The separate 32 malformed-event diagnostic intervals minted 32 framebuffer
timestamp nodes and made no record-constructor dispatch. Initial reader
admission made four constructor dispatches; dispatches are not an allocation
count. Declared renewal reused the reader, cleared its mutable state and kept
its diagnostic sequence. Shared append, unfinished records, another source,
ordinary publisher replacement and truncation preserved their contracts.

One actual collector cycle reclaimed 267,159 temporary strings while a report
held across changing generations remained byte-exact. This establishes the
observed rooting behavior. It does not measure retained RAM or prove that
every obsolete value was reclaimed. The measured stages include reading and
serialization; declaration preparation, event production and collection
exercise are outside their intervals. Timings are observations, not a matched
before/after speed comparison. No primary mints does not mean no allocation.

[Protocol evidence](../docs/evidence/fkwu/care-value-contract.json) compares
twelve transitions with a namespace-only copy of the parent implementation at
`26863337f`. It discriminates six typed/order-sensitive attention needs, missing
control result, stale delivery removal, late attention and unbound controls.
Parsing/serialization/materialization agree on duplicate keys, escaped NUL and
Unicode, large integers, precise floats, signed zero and depth 96. NaN equality
and borrowed booleans have separate checks. Float node admission uses the
existing `make_float64` operation directly, preserving exact finite values,
overflow to either infinity, canonical NaN and zero without a decimal
conversion. Nonfinite JSON emission retains its explicit refusal.

The real retained 44,397-byte organ exchange (26 split lines) produces the same
current state, with no primary mints or float boxes during parsing/reduction.
Three separate floating inputs create three seed float boxes and no primary
nodes. Explicit admission of a fresh object creates three primary nodes. Repeated
unbound controls create neither primary nodes nor record dispatches in the
measured reader interval and keep distinct diagnostic identities across renewal.

The first repeated explicit-admission check failed: its fixed object had already
been interned by the preceding run. The exact
[refusal](artifacts/2026-09-23-care-value-ownership/care-values-contract-refusal.txt)
is retained. The witness now prepares a fresh owner identity before measurement.
A correlated framebuffer revise exchange selected the repaired observation;
the subsequent run passed. Deduplication is a real behavior, not evidence that
explicit node admission has disappeared.

[Resident execution](artifacts/2026-09-23-care-value-ownership/care-view-resident.json)
runs two actual care commands, one health command and the public Glass care
door. They retain needs/supply evidence and share the 44,397/0/0-byte cursor.
CLI and Glass both exit zero with empty stderr. Owned process groups and
declarations are released; the private observation home launches no learner.
After rebasing onto `263980753`, the first repeated resident check refused its
clean-stderr claim: `production CLI and Glass stderr are clean after explicit
cache admission` (exit 1). The retained
[Glass diagnostic](artifacts/2026-09-23-care-value-ownership/rebase-glass-refusal.err)
reports a stale BML cache and its automatic rebuild; the private home observer
likewise rebuilt. Both observation roots were explicitly preflighted again
before the final resident execution. The response/ownership checks were kept.

The public `observe/form-care-changing-witness.bml` takes a new absolute
directory on stdin and retains another independent execution. The receipt's
protocol helper is a retained local observation, not a standalone fixture:
it expects `.hearth/organ-health-reference.bml` and the failed-child input
selected by `.hearth/live-process-care-report.json`. The resident artifact binds
that input's path, byte size and digest. The exact namespace-only
[reference](artifacts/2026-09-23-care-value-ownership/organ-health-reference.bml),
[helper](artifacts/2026-09-23-care-value-ownership/care-values-contract.bml) and
[native preparation](artifacts/2026-09-23-care-value-ownership/care-values-prepare.bml)
are retained. Preparation reads a raw `git show
26863337f:form/form-stdlib/organ-health.bml` capture at
`.hearth/organ-health-before.bml`. Replay requires restoring those local inputs;
the historical reference is never a production prelude.

## Review and grounding

The AI review board found no remaining source blocker and verified the scope
of the evidence. Its observations led to shared parser construction, one
protocol authority, NaN equality, direct borrowed-boolean reads and reader-owned
diagnostic identities. Its proposed signed-zero concern was withdrawn after
checking the seed interner's existing canonicalization; the parity case remains.
The final numeric review found that decimal-based materialization refused
nonfinite node content the existing interner accepts. Direct `make_float64`
admission closed that gap without growing C. The full changing observation,
protocol comparison and resident execution were repeated on that source.

Preflight passes. JSON returns `1023`, precise JSON `3`, organ identity `1`, and
the required thirteen drift gates return `8191`, exit zero. No kernel source
moved, so the sibling conformance lane was correctly excluded. Native authoring
reports zero Python implementations, two remaining voice invocation candidates
and zero unread files.

Glass reports 229 MiB logical cell columns, including a 22 MiB identity subset,
and one tensor-allocation observation gap. These are not process RSS or savings.
Hearth reports no standing resident. The arriving Codex agent composed this
movement from native executions and a read-only AI review. Share remains
declared/unmeasured; no percentage is inferred.

The verified teaching completed local learning as candidate generation 40.
The queue is empty, the worker completed, and the corpus retains 40 examples
across 27 sessions. Serving generation 4 is unchanged; an observed adapter
update does not establish improved serving quality.

The next caller transition is ordinary health text emission. Complete ownership
then requires reclaimable float/record lifetimes, Form-owned collection and a
signal/response path that works when its ordinary allocator cannot grow.

— Codex
