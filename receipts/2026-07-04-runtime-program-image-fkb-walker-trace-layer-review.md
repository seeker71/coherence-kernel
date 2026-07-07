# 2026-07-04 - Runtime Program-Image FKB Walker Trace Layer Review

Layer: 9h3
Files:
- `form/form-stdlib/runtime-program-image-fkb-walker-trace.fk`
- `grammars/runtime-program-image-fkb-walker-trace.fk`
- `form/form-stdlib/tests/runtime-program-image-fkb-walker-trace-band.fk`
- `receipts/2026-07-03-core-layer-architecture-map.md`

## Pre-review

Claude/Sema-grounded review: `PASS_WITH_CHANGES`.

Required changes:
- Make the row a structured trace language, not another detail string.
- Validate the same 9e request and admitted 8h6 PIF identity before lowering.
- Keep entry semantics closed to `function-root` for this layer.
- Reject impossible resource facts: negative budget, negative steps, steps over budget, negative output.
- Preserve OOM, killed, stalled, timeout, wrong-value, loader-missing, error, and unknown statuses as evidence.
- Lower only by constructing a 9h1 supplied-run and delegating to 9h1.
- Do not consume 9g/9h2 readiness and do not construct 9f attempts directly.

Grok-style adversarial review: `PASS_WITH_CHANGES`.

Required changes:
- The layer must add an invariant beyond 9h1: trace facts must be internally coherent over the admitted PIF.
- Resource facts must stay structured in the trace row, not only in `detail`.
- Invalid trace facts must not be laundered into nested attempts.
- OOM/stall/kill facts must remain investigation material.
- Static forbids must cover artifact IO, execution, legacy table bridges, 9g/9h2 capability consumption, and direct attempt construction.

## Why 9h3 is not 9h1

9h1 accepts a flat supplied run row:

`(artifact-path first-value output-count exit-code status detail)`

That row is useful, but it cannot explain whether the run came from a real entry in the admitted program image, whether resource counters are possible, or whether OOM/stalled/timeout evidence is structured enough to investigate.

9h3 adds the missing semantic trace language:

`(artifact-path source-hash content-hash entry-kind entry-index root-node step-budget steps-used output-count first-value exit-code status stop-reason detail)`

Only after that trace matches the 9e envelope and admitted 8h6 PIF, names a valid function-root, and carries sane counters does 9h3 lower into the existing 9h1 supplied-run bridge.

## Why 9h3 is not 9h2

9h2 binds 9g readiness to a 9h1 supplied run. It answers: "was there a current, supplied, authorized program-image-walker capability for this request?"

9h3 does not consume 9g readiness. It answers a different question: "is the supplied run backed by structured trace facts that are coherent over this admitted PIF?"

The two layers remain separate so capability provenance does not become a gate-shaped substitute for trace semantics.

## Why This Is Not A Walker

This layer is not a walker: it does not open a `.fkb`, decode bytes, step through nodes, call native code, or produce an observation. It consumes a supplied trace row and checks it. The real walker remains deferred until the program-image executor can produce this trace as evidence rather than have it supplied.

## Resource And Failure Observability

OOM, killed, stalled, timeout, wrong-value, loader-missing, error, and unknown statuses are valid trace statuses when their counters and stop reasons are coherent. They lower through 9h1/9f/9c so the existing outcome face still investigates hard or unknown failures.

The layer refuses impossible counters:
- negative step budget
- negative steps used
- steps used greater than budget
- negative output count
- `ok` with zero output

Invalid entry/root/counter/status facts do not call 9h1, so they cannot carry a nested attempt. Malformed envelope/admission floors do call 9h1 with the trace-derived run so the nested 9h1 reason remains visible.

## Go/Table Authority Answer

The generated Go/Rust/TS blueprint tables are proof-sibling projections, not runtime authority. The target executable symbol/dependency truth belongs in program-image `.fkb`; `.sym` is a locale/domain presentation lens over stable symbols. 9h3 keeps that direction: no generated proof-sibling table authority and no legacy text bridge.

## Deferred

- Real program-image walking.
- Actual `.fkb` artifact load or whole-file byte hashing.
- 9g/9h2 capability enforcement around this trace adapter.
- Durable trace production by the future executor.
- Any C-seed growth.

## What The Band Proves

The focused band is expected to return `2147483647`.

It proves:
- manifest and version
- happy trace lowers to a 9h1 supplied-run and nested 9f attempt
- the 9f trust tuple matches the 9e envelope
- 9c receives ok/hard/unknown statuses through the existing receipt path
- structured trace fields remain visible in the bridge
- OOM, killed, stalled, timeout, wrong-value, loader-missing, error, and unknown statuses are preserved
- malformed trace, envelope, admission, decode, PIF identity, trace identity, entry, root, counter, output, and stop-reason failures are distinct
- invalid trace facts do not produce a nested 9h1 bridge
- malformed admission floors expose the nested 9h1 reason
- source and grammar mirrors are byte-identical
- static forbids exclude artifact IO, execution, legacy table bridge, 9g/9h2 consumption, direct 9f construction, generated proof-sibling table authority, and C-seed growth
