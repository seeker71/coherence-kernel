# Runtime Program-Image FKB Micro-Walker Layer Review

Date: 2026-07-04

Layer: 9h5, `form/form-stdlib/runtime-program-image-fkb-micro-walker.fk`

## Question

What is the first honest runtime-progress step after the terminal supplied-evidence 9h4 join?

Answer: Layer 9h5 is the first real trace producer after 9h4. 9h4 remains terminal supplied-evidence wrapping. Layer 9h5 computes rpiwt-trace from admitted PIF table data instead of accepting supplied runs or traces.

It walks only the in-memory PIF table carried by an admitted 8h6 program-image byte admission and only after a request-ready 9e program-image envelope matches that PIF identity. It does not accept supplied runs or traces, does not delegate to 9h1/9h3/9h4, does not execute host/native code, does not read or write artifacts, does not use table text, does not consult generated proof-sibling tables as authority, does not mutate storage/selectors/champions/corpus, and does not grow the C seed.

## Pre-Review

Claude/Sema review: PASS_WITH_CHANGES.

Required changes:
- Use Layer 9h5 after naming clarification.
- State explicitly that 9h4 remains terminal supplied-evidence wrapping.
- Consume only a request-ready program-image 9e envelope plus an admitted 8h6 PIF byte admission.
- Compute the trace from `pifbd-admission-pif -> pif-envelope-table -> pif-table-node-rows`.
- Produce an existing `rpiwt-trace` row plus a new micro-walk receipt row.
- Do not accept supplied run rows, supplied trace rows, bridges, or 9h4 joins.
- Do not call downstream supplied-trace lowering paths.
- Mirror 9h1/9h3 identity requirements: request-ready program-image envelope, admitted admission, decoded payload matches admitted PIF, valid PIF, and artifact/source/content identity matches the 9e envelope.
- Pin flat PIF row semantics: `(tag a b c)`, LIT uses `a`, binary ops use `a` and `b`, IF uses `a=cond`, `b=then`, `c=else`.
- Cover LIT, ADD, SUB, MUL, LE, and IF.
- Every recursive walk must consume bounded fuel.

Grok-style adversarial review: PASS_WITH_CHANGES.

Required changes:
- This is genuine runtime progress only if it actually computes from the admitted PIF table.
- Place it as Layer 9h5 because it is still the binary program-image runtime route.
- Unsupported tags must be loud, not default-to-zero.
- Budget exhaustion must become computed timeout evidence, not an ignored hang.
- Emit a receipt binding envelope, admission, entry, root, budget, steps, computed trace, status, and reason.
- Source must not call supplied-run/supplied-trace/bridge/join constructors, recipe walkers, host/native doors, artifact IO, table-text paths, generated table authority, selector/storage/champion/corpus mutation, or C growth.

Naming reconciliation:
- Initial reviews disagreed between 9j and 9h5.
- Both reviewers accepted `9h5. Runtime program-image .fkb micro-walker` after the map/receipt wording was tightened: 9h4 remains terminal supplied-evidence wrapping; 9h5 is the first real trace producer after it.

## Achieved

- Added `runtime-program-image-fkb-micro-walker.fk` and a grammar mirror.
- Added `runtime-program-image-fkb-micro-walker-band.fk`.
- Added a Layer 9h5 architecture row and map paragraph.
- Added a bounded PIF table evaluator over admitted in-memory PIF rows.
- Implemented LIT, ARG, ADD, SUB, MUL, LE, and IF over flat `(tag a b c)` rows.
- Emitted existing `rpiwt-trace` rows with computed first value, output count, exit code, trace status, stop reason, entry index, root node, step budget, steps used, and detail.
- Emitted a micro-walk receipt binding envelope, admission, entry index, root node, budget, input value, trace, result, status, and reason.
- Preserved request-ready program-image 9e envelope validation.
- Preserved admitted 8h6 PIF admission validation, decoded-payload/PIF checks, and envelope/PIF identity checks.
- Made zero/exhausted budget produce timeout traces.
- Made unsupported tags and child out-of-range produce error traces with no fake output.
- Made malformed inputs and invalid envelope/admission/entry/root cases refuse before unsafe accessors.

## Deferred

- Full program-image runtime remains deferred.
- General calls, closures, lists, strings, NodeID construction, record ops, source-map/deopt, and native callability remain deferred.
- Disk `.fkb` loading and startup selector integration remain deferred.
- 9g capability-bound use of this producer remains deferred.
- Durable storage mutation, champion update, corpus insertion, and freeze application remain deferred.
- OOM/killed synthesis remains deferred because this layer is pure Form table walking; actual process OOM/killed events must be observed externally and classified by 10b/10c.
- C-seed expansion remains rejected unless it is a short-lived checkout-witness repair with a shrink receipt.

## Why This Boundary Holds

Layer 9h5 is not another evidence wrapper. It does not package a supplied trace. It computes a trace by walking admitted PIF rows under bounded fuel. That makes it runtime progress while keeping the runtime surface narrow enough to prove.

The layer is intentionally small because the previous wrappers proved the artifact route and trace vocabulary, not execution. This slice creates the first executable bridge from admitted PIF table data to structured trace evidence.

## Post-Review

Claude/Sema post-review: PASS.
- No concrete blockers found.
- Verified source/grammar parity, computed trace production from admitted PIF table rows, no supplied-run/supplied-trace/bridge/join consumption, coverage of happy paths and error paths, and receipt/map boundary wording.

Grok-style adversarial post-review: PASS.
- No concrete blockers found.
- Verified no fake execution, no hidden supplied-evidence lowering path, no silent default values, identity/admission/decode checks, source/mirror parity, focused validation, and architecture placement as the first real trace producer after terminal supplied-evidence 9h4.

## Validation

Bootstrap/witness:
- `cc -O2 -o fkwu runtime/fkwu-uni.c` passed with existing checkout warnings about `fread` declaration and `getsockname` pointer sign.
- `./fkwu --src bootstrap/ground.fk` -> `42`.
- `./fkwu --src bootstrap/ground-recursive.fk 10` -> `55`.
- `./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk` -> `15`.
- `./fkwu --src /tmp/nvr.fk` -> `11111`.

Focused layer:
- `cd form && ./validate.sh form-stdlib/tests/runtime-program-image-fkb-micro-walker-band.fk` -> `16777215`, `1 ok, 0 divergent`.
- Final focused rerun after receipt post-review update: `cd form && ./validate.sh form-stdlib/tests/runtime-program-image-fkb-micro-walker-band.fk` -> `16777215`, `1 ok, 0 divergent`.

Neighbor bands:
- `cd form && ./validate.sh form-stdlib/tests/program-image-fkb-byte-decode-band.fk` -> `536870911`, `1 ok, 0 divergent`.
- `cd form && ./validate.sh form-stdlib/tests/runtime-program-image-fkb-attempt-band.fk` -> `2147483647`, `1 ok, 0 divergent`.
- `cd form && ./validate.sh form-stdlib/tests/runtime-program-image-fkb-walker-trace-band.fk` -> `2147483647`, `1 ok, 0 divergent`.
- `cd form && ./validate.sh form-stdlib/tests/runtime-program-image-fkb-traced-capability-bound-band.fk` -> `2147483647`, `1 ok, 0 divergent`.
- `cd form && ./validate.sh form-stdlib/tests/runtime-trace-ingest-band.fk` -> `8388607`, `1 ok, 0 divergent`.
- `cd form && ./validate.sh form-stdlib/tests/runtime-trace-feedback-band.fk` -> `16777215`, `1 ok, 0 divergent`.

Investigated failures and stalls:
- Initial focused validation returned `16777087` instead of `16777215`; missing bit `128` isolated to the LE/IF proof. Root cause was the test expectation, not the walker: IF over LE visits IF, LE, ARG, comparison literal, and selected branch literal, so the expected step count is `5`, not `4`.
- A source parse failure in `rpmw-walk-entry` was traced to one extra closing parenthesis; the source and grammar mirror were repaired together.
- The 8h6 byte-decode neighbor ran longer than 30 seconds and completed after about 43 seconds with the expected value.
- The 9h1 attempt neighbor ran longer than 30 seconds and completed after about 42 seconds with the expected value.
- No OOM/killed process was observed in this slice.

Failure policy:
- OOM, killed, stalled, timeout, wrong-value, loader-missing, error, unknown, parse failures, wrong bitmasks, and stalls must be investigated and recorded. They are not ignorable noise.
