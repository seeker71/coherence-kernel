# Host events carry the retained choice

Form now emits Darwin ARM64 event-queue calls in RAM. One owner batches path,
timer and descriptor-readiness events, grows its storage and releases its
registrations, descriptors and native admissions. The form-cli demand path
uses this lifetime to resume JIT care in its existing owner. No C grew and no
new regression band was added.

`form-run ./fkwu .hearth/native-events-observe.bml` observed 154 events in eight
waits, including a 150-timer batch, buffer growth, full-width token delivery,
borrowed descriptor survival and physical closure of owned descriptors. The
event image made 332 native calls; its memory owner made 316. These are exercise
counts, not a throughput claim.

`form-run ./fkwu .hearth/jit-host-events-observe.bml` passed actual asynchronous
source arrival, unrelated writes, missing ancestors, failed resumption and
repair, permission changes, lease expiry/renewal, explicit holds, parent
replacement and delayed telemetry. Every repair retained the exact image and
runtime identity; native calls remained 34 with no repeated materialization or
installation. Closing events preserved the caller's native pages.

Two observations corrected their causes in this same context. The enrollment
exercise first returned `[enrollment-repair-reported, 0, 1]` with exit 1; wait now
counts enrollment's repair and returns it immediately. The existing demand
check first returned 120259084287 instead of 137438953471. The focused command
`form-run ./fkwu .hearth/jit-held-observe.bml` exposed
`[held-duration, 0, integrity-duration, 0, retained-stage-duration, 5]` and
`fkwu: form_error: held observation lost its elapsed time`. Both held paths now
retain their measured stage duration; the same command returned 5, 5, 5 and 1.

The five existing demand/owner/projection/hold/identity checks returned
137438953471, 1048575, 4095, 8191 and 31; raw captures remain under
`.hearth/source-inventory-62282-*`. The form-cli check returned 16383, and its
actual runner returned Tensor/CPU/Metal 22 with event release 1. Both new source
units passed preflight without errors, warnings or unresolved names. Source
changes renewed their local caches through the compiler's existing care flow.

The closing spend meter exposed a growing-file race: decided extent 189614773
exceeded its observed size 189607451. The focused command
`form-run ./fkwu .hearth/meter-live-snapshot.bml` reproduced it as
`[snapshot-stops-at-observed-extent, [20, 83], [7, 41]]`, exit 1. The BML reader
now bounds each read to that snapshot and grows an incomplete row's slice
instead of skipping it. Snapshot, append, partial-row, large-row and repeat
observations passed; existing meter/movement checks returned 8191 and 15.
The actual meter resumed at its saved cursor and reported decided extent
189822220 within 189822252 bytes, with session output 2549728 tokens. This is
the session's accumulated reading, not this movement's token count.
The final release checks passed 8191 with no refusals; no kernel source moved.

Counsel observed orphans 0; 11/12 lanes remain unobserved without a standing
hearth. The native guide reports zero Python implementations, two invocation
candidates and zero unread files. The useful lesson was concrete: a permission
repair left size/mtime observations unchanged, but its actual attribute event
brought the retained choice forward. Other hosts, preemptive scheduling and
volume-capacity notification remain separate work.

This is Codex's implementation and review, grounded and executed through Form.
The verified teaching was retained locally as
`codex-fkwu-native-metal-jit / 2026-09-28-native-resource-events`.

— Codex
