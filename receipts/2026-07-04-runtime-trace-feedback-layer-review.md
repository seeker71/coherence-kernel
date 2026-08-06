# Runtime Trace Feedback Layer Review

Date: 2026-07-04

Layer: 10c, `form/form-stdlib/runtime-trace-feedback.fk`

## Question

What can learning do after Layer 10b without pretending to be runtime progress?

Answer: Layer 10c consumes 10b ingest records and turns them into a pure feedback agenda. It is not runtime progress. It does not execute, load artifacts, freeze storage, update champions, insert corpus rows, install selectors, consult generated proof-sibling tables as authority, or grow the C seed.

This is a learning/reasoning row language. `propose-freeze only` means a body-lane witness can be named for future review, not applied.

## Pre-Review

Claude/Sema review: PASS_WITH_CHANGES.

Required changes:
- Name the layer as Layer 10c, with source, grammar mirror, focused band, receipt, and architecture row.
- Public input must be only `runtime-trace-ingest-record` rows via `rti-record?`.
- Do not accept raw joins, raw traces, attempts, readiness rows, or runtime rows.
- Preserve 10b fields: artifact path, source hash, content hash, trace status, stop reason, detail, prompt class, priority, memory lane, ingest action, freeze eligibility, and ingest reason.
- Use distinct feedback kind and agenda action fields.
- Body plus witness plus freeze eligibility becomes `freeze-candidate` / `propose-freeze`.
- Liquid plus witness becomes `witness-retain` / `retain-liquid`.
- OOM, killed, stalled, timeout, and wrong-value prompts become `investigation-work-item` / `investigate-runtime-failure`.
- Loader-missing and error prompts become `repair-work-item` / `repair-runtime-route`.
- Quarantine remains `quarantine-work-item` / `hold-for-review`.
- Refused or malformed evidence becomes `refused-evidence` / `reject-evidence`.
- The agenda selector must be pure, highest-priority first, and stable on ties.
- Reason coverage must come from real 10c invocations.

Grok-style adversarial review: PASS_WITH_CHANGES.

Required changes:
- This layer is valid only as a pure agenda/feedback layer over 10b records.
- It must not look below 10b or become another runtime wrapper.
- OOM must never be ignored or collapsed into an unobservable generic failure.
- The focused band must cover hard failures, soft repair, quarantine, refused/malformed input, no-work, priority order, stable tie behavior, source/mirror forbids, and the upstream 10b neighbor.

## Go Table Guide

The generated Go/Rust/TypeScript blueprint tables are proof-sibling projections used by validation siblings, not runtime authority. They are not a core architecture primitive, not `.fkb` symbol/dependency truth, and not a reason for this layer to exist. Layer 10c explicitly forbids generated table authority.

## Achieved

- Added `runtime-trace-feedback.fk` and a grammar mirror.
- Added `runtime-trace-feedback-band.fk`.
- Added a Layer 10c architecture row.
- Added a feedback row that preserves the original 10b ingest record and the normalized 10b fields needed by learning.
- Added explicit `feedback-kind` and `agenda-action` fields.
- Body-lane witnessed rows become `freeze-candidate` with `propose-freeze only`.
- Liquid witnessed rows become retained witnesses, not body candidates.
- Hard prompts, including OOM, become high-priority investigation work.
- Soft prompts become repair work.
- Quarantine and refused evidence stay held or rejected.
- Malformed/non-record input refuses as `malformed-ingest-record`.
- Added a pure agenda selector that chooses the highest-priority feedback row and keeps the first row on equal priority.

## Deferred

- Real trace producer remains deferred.
- Real program-image execution remains deferred.
- Durable freeze/storage mutation remains deferred.
- Champion updates and corpus insertion remain deferred.
- Runtime selector installation remains deferred.
- C-seed expansion remains rejected unless it is a short-lived checkout-witness repair with a shrink receipt.

## Why This Guide Holds

Layer 10b already normalized supplied runtime evidence into learning rows. Layer 10c is allowed because it consumes those rows and speaks a higher-level agenda language over them. It does not create evidence, recompute evidence, or act on the agenda.

If runtime progress is required next, the next layer must be a real trace producer or walker, not another wrapper.

## Post-Review

Claude/Sema post-review: PASS.

Verified:
- Source/mirror parity is exact.
- Source gates public input with `rti-record?`.
- Classification uses 10b accessors only.
- Forbidden runtime authority strings are absent from source/mirror.
- Feedback rows preserve the 10b record and normalized fields.
- OOM is explicitly covered as high-priority investigation work.
- Stable agenda tie behavior and reason coverage are covered by the band.
- The architecture map and receipt keep 10c in learning/feedback, not runtime progress.

Grok-style adversarial post-review: PASS.

Verified:
- Raw joins/traces fall to `malformed-ingest-record`.
- The implemented layer emits feedback/work rows only.
- The agenda selector is pure and stable by strict greater-than priority comparison.
- The band uses lower-layer constructors only as fixtures; the implemented layer itself stays above 10b.
- The generated Go/Rust/TypeScript tables remain validation projections, not authority.

## Validation

Actual:
- `./validate.sh form-stdlib/tests/runtime-trace-feedback-band.fk` from `form/` -> `16777215`, `1 ok, 0 divergent`.
- `./validate.sh form-stdlib/tests/runtime-trace-ingest-band.fk` from `form/` -> `8388607`, `1 ok, 0 divergent`.

Failure policy:
- OOM, killed, stalled, timeout, wrong-value, loader-missing, error, unknown, parse failures, wrong bitmasks, and stalls must be investigated and recorded. They are not ignorable noise.

Investigated failures before pass:
- First validation command was run from the repository root as `./validate.sh form-stdlib/tests/runtime-trace-feedback-band.fk` and failed with `zsh:1: no such file or directory: ./validate.sh`. The validator lives at `form/validate.sh`; reran from `form/`.
- First focused run from `form/` failed before execution with a parser error: unclosed `(do ...)`. Parenthesis balance showed the new band was missing one closing paren.
- An initial repair put the extra close in the upstream-neighbor bit. The next run exposed the real bug: `rtfb-bit-preserved-fields` made `rtfb-score` receive one argument because its boolean expression was not closed before the bit value.
- Fixed `rtfb-bit-preserved-fields`, then reverted the earlier overcorrection in the upstream-neighbor bit. The focused band then passed.
- No OOM, killed process, hard stall, or timeout occurred in this slice.
