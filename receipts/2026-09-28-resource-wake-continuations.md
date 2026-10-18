# Resource changes return attention to held work

Codex, 2026-09-28. Implementation and local inspection/editing tools are BML,
executed through fkwu. Git remains host transport. No C seed change.

`native-events-darwin.bml` gives a held continuation ownership of dedicated
resource subscriptions. Enrollment observes once across registration; subsequent
resource events retain their complete batch before one local care attempt.
The original context, completed effects and findings stay together. Completion
withdraws the wake resources; incomplete release retains ownership. An exhausted
wake set becomes an additional need, without repeating unchanged work.

Two observations changed the implementation. Updating an existing readiness
registration with EV_CLEAR still repeated unread data; renewal plus the enrollment
observation produced a quiet wait and a wake for new data. A paused vnode failed
to wake because pause/enable erased its notification mask; those operations now
retain the original notes and data. Both offered paused resources and interrupted
primary vnode receivers woke after the repair. Multiple notifications remain
available to care in delivery order rather than losing detail through coalescing.

Observed on Darwin ARM64 through `form-run ./fkwu`:

- `.hearth/event-follow-observe.bml` and
  `.hearth/event-continuation-observe.bml`: exit 0, verdict 1. Enrollment,
  deferred care, original effects, findings, quiet waits, complete wake batches,
  paused subscriptions, exhaustion, withdrawal and stale responses are observed.
  Actual child exit was reaped once; a second wait returned -10. An owned
  descriptor returned -9 after release; borrowed descriptors remained usable.
- `.hearth/shared-event-wake.bml`: exit 0, verdict 1. Both streams retained
  exactly 1,572,872 bytes while path care, JIT publication and timed cancellation
  shared the queue. The child was reaped with status 9; retained JIT code returned
  34. All subscriptions and the queue released. 94 waits, 94 callbacks, 7 reaps,
  zero would-block results. Local stale caches announced their rebuild.
- `.hearth/pipe-after-measure.bml`: 4,194,312 exact bytes, 1,305 counted native
  calls, 130 reads, 65 writes, zero would-block results, physical release 1.
  The call count is unchanged; concurrent work precludes a latency comparison.
- Changed-source preflight: zero errors, warnings and unresolved calls.
- The nine public pipe, HTTP and translation witnesses passed with exit 0,
  zero stderr and verdict 1; their source identities and published readback
  passed. Rebase preserved every retained witness input. Evidence lives in
  `docs/evidence/fkwu/native-pipe-translation.json`.
- After rebase: `gate/drift-gates-run.bml` returned 8191/8191, refused 0;
  `form/form-stdlib/tests/form-knowledge-public-query-offer-batch-band.fk`
  returned 8388607 against the renewed upstream anchors.

A `.hearth/native-pipe-public-verify-run.bml` attempt stopped at refusal
preflight: `unexpected terminal translation diagnostic` (retained at
`.hearth/source-inventory-1180-723464702-0/preflight`). A private replay retained
the offending stale-cache warning and compiler health records in
`.hearth/translation-refusal-admission-stderr`. The BML verifier now admits all
three worker sources with `--check` before exact runtime stream observations.
Those diagnostics remain visible; the original seven-record refusal and
physical-release checks then pass unchanged. The eight completed observations
were retained and revalidated instead of rerunning their unchanged workloads.

The authoring panel reads Python implementations 0, execution candidates 2,
unread files 0. Counsel reports orphans 0; eleven lanes remain unobserved.
Local session learning completed round 52, pending 0, refusals 0; serving stays
at generation 4. The retained teaching is execution-grounded, not a promotion
or an answer-quality claim. No standing hearth supplied this movement.

The current boundary remains cooperative Darwin ARM64 execution. Non-yielding
work, fatal process exits, other host carriers and preemption have separate
lifetimes. The direction is Form-owned scheduling and resource care with a
shrinking host seed, carrying context across every choice.
