# Cooperative Form slices share the resource owner

2026-09-28 — Codex

The executing step toward the north star is one Form owner for resource events,
ready computation and local care. `nve-post`, `nve-yield`, `nve-run` and
`nve-turn-slices` live in the existing BML event organ. Its queue preserves
checkpoints and findings, serves peers in arrival order and accepts a caller's
changing slice grant. A stop discards an unacknowledged proposal; accepted care
can yield the next step. Retired subscriptions retain care without scheduling.

Observed on the rebased checkout and fresh seed:

- `form-run ./fkwu .hearth/cooperative-slices-observe.bml`: exact order
  `A1B1A2B2A3`; 10,000 slices, zero additional event-image calls; care correlation,
  rejected proposals, resource wakes, vnode re-enablement and release passed.
- `form-run ./fkwu .hearth/shared-event-slices.bml`: sum 1..262144 = 34359869440
  in 256 slices while response bytes advanced; both 1,572,872-byte streams exact,
  cancellation reaped status 9, repaired native page returned 34, zero remaining
  subscriptions, physical queue release 1. Evidence: `.hearth/shared-events-65959`.
- `form-run ./fkwu .hearth/pipe-after-measure.bml`: 4,194,312 bytes exact;
  1,305 counted native-image calls, 130 reads, 65 writes, zero would-block,
  release 1. Concurrent elapsed time 230 ms is not a controlled latency claim.
- Continuation and resource-follow observations passed; final source preflight
  reported zero errors, warnings and unresolved calls.
- `form-run ./fkwu gate/drift-gates-run.bml`: 8191/8191, refused 0.
- All nine public pipe, HTTP and translation witnesses passed with clean
  preflights, exit 0 and zero stderr bytes. Source identities and exact report
  readback passed: `docs/evidence/fkwu/native-pipe-translation.json`.

The private cooperative observation first exposed mismatched `care_of` identity
and `[manual-care-cannot-nest-ready-grant, 0, 1]`. Capturing the original reading
and treating the receiving frame as active repaired both; the same observations
passed. `form-run ./fkwu .hearth/cooperative-retirement-observe.bml` first returned
`[retired-proposal-is-held-not-ready, [1, 1], [0, 1]]`; queue admission now observes
subscription lifetime, retains the checkpoint and completes local care.

The mixed command first returned `[streams-progress-during-computation, 0, 1]`.
The echo protocol waits for a complete request frame. Admitting computation
after the first 65,536 response bytes established actual overlap; the unchanged
progress, exact-byte and physical-release expectations then passed. The original
observation remains at `.hearth/shared-event-slices-before-receive-admission.bml`.

Form's local learner completed round 53 with zero pending examples; serving
generation remains 4. That establishes retained learning, not improved answers.
Counsel: orphans 0; 11/12 lanes unobserved because no hearth stands. The native
authoring guide reports zero Python implementations, two invocation candidates.
Authoring, inspection, editing and measurement ran through BML; Form carried Git
and the required upstream seed rebuild. This patch changes no C runtime source.

Next: compile suitable BML loops into these owned checkpoints. Receivers must
currently return explicitly; automatic yielding and preemption remain open.
