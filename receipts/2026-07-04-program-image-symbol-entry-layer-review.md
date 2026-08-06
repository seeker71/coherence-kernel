# Program Image Symbol Entry Layer Review

Date: 2026-07-04

Layer: 8h8, `form/form-stdlib/program-image-symbol-entry.fk`

## Question

How do we stop making higher layers speak raw table entry indices while keeping `.fkb` as the executable symbol truth?

Answer: Layer 8h8 is a pure PIF metadata resolver. It consumes a valid 8h program-image envelope plus a symbol request and resolves embedded executable symbol truth to a unique function-root entry. It is not 9h6 because it consumes no runtime request envelope, no 8h6 byte admission, no input value, no step budget, no 9h5 receipt, and it emits no trace or attempt.

This layer keeps table entry/root numbers as resolved carrier fields, not as the high-level calling language. The request language is symbol id, canonical key, or id+key agreement. A diagnostic node request exists only to prove that direct node ids must still route back through a non-anonymous node-symbol row and a unique function root.

## Pre-Review

Claude/Sema review: PASS_WITH_CHANGES.

Required changes:
- Place the layer at 8h8 only if it remains pure over `pif-envelope-valid?`.
- Move it to 9h6 only if it consumes 9e envelopes, 8h6 admissions, 9h5 receipts/traces, budgets, input values, attempts, observations, or starts walking.
- Canonical-key lookup must be exact and unambiguous because 8h validates contiguous ids and nonempty keys but not key uniqueness.
- Do not expose direct node id as a normal runtime calling surface.
- Ready requires a valid PIF envelope, well-shaped request, in-range symbol id or exactly one key match, id/key agreement when both are supplied, exactly one node-symbol row defining the symbol, a node id in range, and exactly one function-root match.
- Carry dependency target evidence without recursively resolving calls or walking dependencies.
- Provide distinct reasons for malformed PIF/request, invalid id, missing or duplicate key, id/key mismatch, undefined symbol, defined node not a root, duplicate root match, and invalid PIF.
- Do not use presentation lens rows, 8h6 byte admission, 9e envelopes, 9h5, trace rows, table text, generated proof-sibling tables, file IO, selector/storage/champion/corpus mutation, or C growth.

Grok-style adversarial review: PASS_WITH_CHANGES.

Required changes:
- 8h8 is correct only as a pure resolver over the embedded 8h symbol/table image.
- It becomes 9h6 the moment it touches request/admission/runtime evidence or walking.
- Raw entry-index and direct node-id must not become the public calling surface.
- Output must carry enough evidence: PIF envelope, request, resolved symbol id, canonical key, node-symbol row including dependency targets, derived entry index, root node, status, and reason.
- The proof band must cover happy paths, duplicate canonical-key ambiguity, duplicate function-root ambiguity, non-root symbols, anonymous diagnostic nodes, missing node-symbol rows, static forbids, source/mirror parity, reason coverage, and neighboring 8h/8h7/9h5 validations.

## Achieved

- Added `program-image-symbol-entry.fk` and a grammar mirror.
- Added `program-image-symbol-entry-band.fk`.
- Added the `8h8. Program-image symbol entry resolver` architecture row and current-evidence paragraph.
- Defined symbol-entry requests for symbol id, canonical key, id+key agreement, and diagnostic node proof.
- Defined a resolution row that binds PIF envelope, request, resolved symbol id, canonical key, symbol row, node-symbol row, dependency targets, entry index, root node, status, and reason.
- Made ready resolution require a valid PIF envelope, unambiguous canonical key, non-anonymous node-symbol row, and exactly one function-root match.
- Preserved dependency target rows as evidence without walking them.
- Guarded duplicate canonical-key ambiguity locally instead of pretending 8h already enforces key uniqueness.
- Kept direct node lookup diagnostic-only and required it to resolve back through embedded symbol truth.

## Rejected Alternatives

- Rejected making 9h5 accept symbol ids or strings directly; that would mix runtime walking with symbol metadata resolution.
- Rejected placing this as 9h6 before it consumes runtime request/admission evidence.
- Rejected using presentation lens rows, localized display, aliases, docs, or `.sym` parsing as executable truth.
- Rejected trusting `.tbl` text or generated proof-sibling tables as symbol authority.
- Rejected C-seed lookup, selector install, artifact IO, and `.fkb` parsing/writing in this layer.
- Rejected requiring every PIF symbol to be callable in this slice.

## Deferred

- 9h6 runtime join from request-ready envelope, admitted byte image, symbol-entry resolution, and 9h5 micro-walk remains deferred.
- Global 8h canonical-key uniqueness hardening remains deferred; 8h8 guards duplicate canonical-key ambiguity meanwhile.
- Dependency closure and call-graph walking remain deferred; this layer carries dependency target rows only.
- Cross-module symbol resolution remains deferred.
- Presentation lens parsing/rendering policy remains in 8h7 and is not executable authority here.
- Disk `.fkb` loading, cache freshness, runtime selector integration, attempts, observations, and learning ingestion remain deferred.
- OOM/killed synthesis remains deferred because this layer does not execute; actual proof-run stalls, kills, OOM, parse failures, wrong values, and wrong bitmasks must still be investigated and recorded.
- C-seed growth remains rejected unless it is a short-lived checkout-witness repair with a shrink receipt.

## Why This Boundary Holds

8h owns program-image structure and embedded executable symbol truth. 8h7 owns presentation. 9h owns runtime evidence. 8h8 belongs between them: it resolves which function-root entry a symbol denotes, but it does not load, walk, call, bridge, bind capability, emit a trace, or admit an attempt.

This keeps raw table addresses visible as evidence while removing them from the normal higher-level calling surface.

## Post-Review

Claude/Sema post-review: PASS.
- No concrete blockers found.
- Verified source/grammar parity, focused validation, no forbidden runtime/admission/trace/attempt/artifact/generated-table hooks, and alignment between source, band, receipt, and architecture map.

Grok-style adversarial post-review: PASS.
- No concrete blockers found.
- Verified that 8h8 remains a pure PIF metadata resolver, guards duplicate canonical-key ambiguity, keeps diagnostic node lookup diagnostic, resolves through node-symbol plus symbol row plus unique function root, and does not leak table language into the normal calling surface.

## Validation

Bootstrap/witness:
- `cc -O2 -o fkwu runtime/fkwu-uni.c` passed with existing checkout warnings about `fread` declaration and `getsockname` pointer sign.
- `./fkwu --src bootstrap/ground.fk` -> `42`.
- `./fkwu --src bootstrap/ground-recursive.fk 10` -> `55`.
- `./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk` -> `15`.
- `./fkwu --src /tmp/nvr.fk` -> `11111`.

Focused layer:
- `cd form && ./validate.sh form-stdlib/tests/program-image-symbol-entry-band.fk` -> `33554431`, `1 ok, 0 divergent`.
- Final focused rerun after receipt post-review update: `cd form && ./validate.sh form-stdlib/tests/program-image-symbol-entry-band.fk` -> `33554431`, `1 ok, 0 divergent`.

Neighbor bands:
- `cd form && ./validate.sh form-stdlib/tests/program-image-fkb-band.fk` -> `2147483647`, `1 ok, 0 divergent`.
- `cd form && ./validate.sh form-stdlib/tests/program-image-sym-lens-band.fk` -> `1048575`, `1 ok, 0 divergent`.
- `cd form && ./validate.sh form-stdlib/tests/runtime-program-image-fkb-micro-walker-band.fk` -> `16777215`, `1 ok, 0 divergent`.

Investigated failures and stalls:
- No focused proof failure was observed.
- No neighbor proof failure was observed.
- No proof run crossed the 30 second stall-investigation threshold.
- No OOM/killed process was observed in this slice.

Failure policy:
- OOM, killed, stalled, timeout, wrong-value, loader-missing, error, unknown, parse failures, wrong bitmasks, and stalls must be investigated and recorded. They are not ignorable noise.
