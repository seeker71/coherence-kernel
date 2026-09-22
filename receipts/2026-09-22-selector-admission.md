# A malformed selector stays outside the collector

Urs asked that a failure receive attention and healing through the native cells.
This movement repaired the observed selector fault and a syntax-care gap exposed
while the local model worked on that same repair. Source changes below are by
Codex; the model's work is separately retained.

## Actual behavior before and after

A JSON cost report had been bound as a turn identifier, sending the collector
through repeated transcript scans. The original entry reproduced this in an
isolated directory: a valid historical identifier worked, then the saved report
was accepted and changed the six collector files. The
[baseline](artifacts/2026-09-22-selector-admission/baseline.json) retains that
failure.

The existing entry now runs as high-grammar BML. It admits an empty line for
latest, or an identifier containing ASCII letters, digits, hyphen, underscore,
dot and colon. EOF and invalid input receive a diagnostic before state writes.
Its existing target and five reset writes remain.

The same actual-entry checker now passes all
[12 cases](artifacts/2026-09-22-selector-admission/coordinator-check.json): the
original report, empty/latest, historical identifiers, EOF, spaces, tabs,
quotes, arrays, slash and non-ASCII input. Every rejected case preserves all six
seeded files byte-for-byte. Valid cases set the target and clear the five other
files. The checker uses an owned directory and checks that compilation has not
silently changed the candidate. The live collector was not used as a fixture.

The existing cursor band's source assertions now inspect Form text lowered in
memory, so BML authoring preserves the same required reset-call checks.

## Native work and the next failure it exposed

The source-backed coding core admitted Qwen3.8-27B-Q8_0 through the in-process
Metal carrier, with an owned checkpoint, full target source, a BML entry example
and the same caller checker. The
[original request](artifacts/2026-09-22-selector-admission/request.json) records
that context. This is the native coding core with a custom behavior callback;
it is not a public JSON `code` request or a provider call.

The model read, then returned a malformed edit twice, byte-identically. Its
3167-byte reply lacks an array closer at byte 3166. The exact
[reply bytes](artifacts/2026-09-22-selector-admission/native-reply-bytes.json)
remain available. The edit also uses unsupported source syntax and mismatches
the original text. It never reached a source check.

Syntax care now considers inserting the missing expected closer before a
mismatched closer as well as removing an extra closer. Exactly one single-byte
candidate must pass strict JSON. Existing bytes remain intact. Tool shape,
role, writable paths and guarded edits still decide what can execute. EOF
completion and report rewriting remain outside this care.

[Re-observing that actual reply](artifacts/2026-09-22-selector-admission/syntax-reobservation.json)
inserted `]`, emitted correlated healthy syntax evidence, and reached the
guarded edit once. The guard returned `edit-not-found`; no document changed and
no source check ran. This is a syntax repair with an exposed implementation
failure. The receipt distinguishes those outcomes.

After the repeated reply, the owned process was stopped through its control
door. It returned cancellation status 130 after 623594 ms; its group was
[verified released](artifacts/2026-09-22-selector-admission/cancelled-process.json).
The same checkpoint resumed with the newly executable syntax care and the
actual guarded-edit refusal. The
[resumed request](artifacts/2026-09-22-selector-admission/resumed-request.json)
records repair stage at four consumed coding steps. This continuing native
work has not established a completed repair. It retains the original documents
and receives no credit for Codex's later selector implementation.

The [resumed model diagnosis](artifacts/2026-09-22-selector-admission/resumed-diagnosis.json)
was valid JSON and returned the process to implementation. Its reasoning still
needs repair: it speculated that syntax care modified the file, although the
replay established zero document changes. It also proposed using `write` to
overwrite the existing document, contrary to that tool's supplied contract.
The next completed native step read the original resident source. Returning to
execution establishes that transition, not a correct diagnosis or implementation.

## Verification and instruments

- Clean preflights for the selector, cursor, syntax, policy and request checks.
- Actual selector execution: 12/12; cursor band: 33554431.
- Coding syntax: 1; coding policy: 65535; request: 255, all exit 0.
- Drift gates: 8191/8191, refused=0, exit 0. No kernel source changed.
- Glass first frame: 35 ms. Counsel: 0 orphans; 11/12 lanes unobserved because
  no hearth resident stands. No all-healthy inference follows.
- Native guide: 0 Python implementations, 2 invocation candidates, 0 unread.
- Share: declared, percentage withheld while appended carrier bytes reconcile.

The [previous completed coordinator turn](artifacts/2026-09-22-selector-admission/preceding-coordinator-cost.json)
used 4579052 rented tokens, including 4445568 cached-input tokens; 24 model calls
and 23 reconciled tool events. The current open turn is excluded. The local
model made no provider call. Setup, diagnosis and implementation here still
consume coordinating tokens, so local execution is not a claim of zero overall
cost or session parity. The separate output-only session meter read 4077480.

The useful crossing: syntax care made the next real failure visible without
weakening its boundary. The collector repair is verified; native task completion
and overall session parity remain open.

— Codex
