# Runtime Trace Ingest Layer Review

Date: 2026-07-04

Layer: 10b, `form/form-stdlib/runtime-trace-ingest.fk`

## Question

What invariant exists here after 9h4?

Answer: Layer 10b consumes 9h4 evidence as learning material without extending runtime authority. It does not produce traces, attempts, capability joins, or observations. It preserves the carried trace and 9h4 provenance, then separates memory lane from reasoning action.

This is not runtime progress. It is a learning/ingest/reasoning layer that does not execute.

## Pre-Review

Claude/Sema review: PASS_WITH_CHANGES.

Required changes:
- Name the layer as Layer 10b, with source, grammar mirror, focused band, receipt, and architecture row.
- Public input must be a 9h4 join row, not raw independent trace/run input.
- Preserve carried trace fields: artifact path, source hash, content hash, entry kind/index, root node, step budget/used, output count, first value, exit code, status, stop reason, and detail.
- Preserve 9h4 provenance: join status/reason, trace-bridge status/reason, capability-join status/reason, and whether the top attempt exists.
- Keep memory lane separate from action: `body` and `liquid` answer whether evidence may freeze, while `investigate`, `quarantine`, and `refused` answer what reasoning should do.
- `ok/completed` may become body-freeze eligible only when capability-bound.
- Non-bound rows stay liquid/witnessed, never body.
- Hard statuses become high-priority investigation prompts.
- Soft statuses become repair/fallback prompts.
- Unknown status quarantines.
- Malformed join or malformed carried trace evidence refuses without unsafe accessors.

Grok-style adversarial review: PASS_WITH_CHANGES.

Required changes:
- This layer is valid only because it changes axis from runtime to learning.
- Public input should be 9h4 join evidence only; standalone traces are not equivalent authority.
- Hard failures must not be ignored just because the join is not bound.
- `body` means body-freeze eligible, not storage mutation.
- The band must include source/mirror static forbids and reason coverage from actual invocations.
- The receipt and architecture map must state that this consumes 9h4 evidence, is not runtime progress, and does not execute.

## Achieved

- Added `runtime-trace-ingest.fk` and a grammar mirror.
- Added `runtime-trace-ingest-band.fk`.
- Added a Layer 10b architecture row.
- Added a record shape that keeps the original 9h4 join and carried trace reachable.
- Preserved normalized trace fields and 9h4 provenance fields in every valid trace record.
- Split `memory-lane` from `action`.
- `ok/completed` plus 9h4 `traced-capability-bound` becomes `body` with `body-freeze-eligible`.
- Non-bound ok rows become `liquid` plus `witness`; non-bound rows stay liquid/witnessed.
- OOM, killed, stalled, timeout, and wrong-value traces become high-priority investigations.
- Loader-missing and error traces become repair prompts.
- Unknown status quarantines.
- Invalid trace evidence carried by a non-ready trace bridge refuses as `trace-not-ready`.
- Malformed joins, malformed traces, and malformed trace bridges refuse without unsafe field access.

## Deferred

- Real trace producer remains deferred.
- Real program-image execution remains deferred.
- Durable storage mutation remains deferred.
- Learning feedback loops that update a champion or corpus remain deferred.
- Any source or binary front-door selector remains deferred.
- C-seed expansion remains rejected unless it is a short-lived checkout-witness repair with a shrink receipt.

## Why This Guide Holds

Layer 9h4 was the last honest supplied-evidence runtime join. Another runtime wrapper would be ceremony. Layer 10b is allowed because it does not add runtime authority; it makes existing evidence observable to learning and reasoning. The next runtime-progress layer still has to be a real trace producer or walker.

## Post-Review

Claude/Sema post-review: FAIL, then PASS after the API guide repair.

Initial blocker:
- Internal record helpers accepted separately supplied trace and trace-bridge values. That violated the public 9h4-join-only requirement.

Repair:
- Record-producing helpers now accept only the 9h4 join and derive the carried trace and trace-bridge internally.

Grok-style adversarial post-review: PASS after the same repair.

## Validation

Actual:
- `./validate.sh form-stdlib/tests/runtime-trace-ingest-band.fk` -> `8388607`, `1 ok, 0 divergent`.
- `./validate.sh form-stdlib/tests/runtime-program-image-fkb-traced-capability-bound-band.fk` -> `2147483647`, `1 ok, 0 divergent`.

Investigated failures before pass:
- First focused validation failed because the band expected `rpifc-reason-bound`, but 9h2 exposes the actual accessor as `rpifc-reason-capability-bound`. Go/Rust/TypeScript all reported the same unbound function. The band now uses the real 9h2 vocabulary.
- Post-review found a real API guide leak: internal record helpers accepted separately supplied trace and trace-bridge values. Those helpers now accept only the 9h4 join and derive the carried trace and trace-bridge internally before building records. The focused band still returns `8388607`.
- No OOM, killed process, hard stall, or timeout occurred in this slice.

Failure policy:
- OOM, killed, stalled, timeout, wrong-value, loader-missing, error, unknown, parse failures, wrong bitmasks, and stalls must be investigated and recorded. They are not ignorable noise.
