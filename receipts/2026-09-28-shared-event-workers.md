# One queue keeps each operation's context

Pipe workers and JIT care now share a Form-owned native event queue. Each
subscription retains its receiver and context. Pipe readiness carries byte
extent and EOF, process exit prompts physical reaping, and an owned timer
advances cancellation while peers continue. The pipe poll-array builder,
per-stream readiness probes and per-turn child-status probes are removed.
The C seed is unchanged.

`form-run ./fkwu .hearth/shared-event-observe.bml` passed a 1,572,872-byte
binary exchange while repairing the retained JIT image and cancelling a child
that held SIGTERM. The grace timer escalated to SIGKILL, the child was reaped,
both other workers completed, and the original native image still returned 34.
Independent consumer release left the queue alive with zero subscriptions.
Unbound events remained retrievable. Quiet process-only waits performed no
reap calls; EOF stayed distinct from process completion. Duplicate process
subscription could not replace its owner. The final reading was 91 waits,
92 callbacks, seven reap observations and zero would-block writes.

The first escalation attempt stopped a child, which this host still terminated
on SIGTERM: `[event-driven-escalation, [0, 15, 1], [1, 9, 1]]`, exit 1. The
same owned exercise then blocked that signal explicitly and observed escalation.
The private admission command `.hearth/pipe-event-admission.bml` also exposed
`fkwu: form_error: worker admission incomplete` under its test-only 20-second
deadline. Admission now waits for owned completion. All three workers exited
zero and released; startup compiler diagnostics remain in
`.hearth/pipe-admission-90951-*.stderr`. Exact message-stream checks stay intact.

`form-run ./fkwu .hearth/pipe-retained-write.bml` observed EPIPE with the
original 4,194,304-byte request and accepted offset retained, a valid organ
reading and complete release. The raw event exercise passed 154 events,
buffer growth, wide tokens, borrowed descriptor survival and physical closure.
The existing JIT resource exercise passed source/parent arrival, permission
repair, lease renewal, explicit holds and delayed telemetry.
`form-run ./fkwu .hearth/event-dispatch-boundary.bml` observed both signed
integer edges, absent out-of-range data, and a real two-timer batch whose first
receiver retired the second. The retired entry stayed quiet and its replacement
received only its own later event; every subscription and the queue released.

The same 4,194,312-byte framed transfer ran against the preceding pipe owner
and the event owner through `.hearth/pipe-{before,after}-measure.bml`. Counted
native-image calls, including both memory owners, were 2,615 and 1,305; elapsed
times were 805 and 517 ms. The final-source repeat retained 1,305 calls and took
516 ms. Both paths delivered exact stdout/stderr and fully released. These
are bounded parent-side observations, not all membrane costs or maximum bandwidth.
The release checks returned 8191 with no refusals; no kernel source moved.
All nine public pipe, HTTP and translation witnesses passed with exit zero and
empty stderr, and their source identities matched again before publication.
The refreshed `docs/evidence/fkwu/native-pipe-translation.json` includes all
10,000 labels / 120,000 retained cells and three resident translation workers
across two rounds, with complete reaping and physical descriptor release.

Counsel reports orphans 0; 11/12 lanes remain unobserved without a standing
hearth. Native authoring reports zero Python implementations, two invocation
candidates and zero unread paths. Current docs carry shared ownership and its
Darwin ARM64/cooperative boundary. The verified teaching is retained locally as
`codex-fkwu-native-metal-jit / 2026-09-28-shared-event-workers`.

— Codex, implementation and review grounded and executed through Form
