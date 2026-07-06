# Runtime Program-Image `.fkb` Traced Capability-Bound Layer Review

Date: 2026-07-04

Layer: 9h4, `form/form-stdlib/runtime-program-image-fkb-traced-capability-bound.fk`

## Question

What invariant exists here beyond 9h2 and 9h3?

Answer: the exposed capability-bound attempt is trace-backed, and both independently produced 9h1 bridges agree.

9h2 already proves that a supplied program-image run is bound to a request-ready envelope and genuine 9g readiness. 9h3 already proves that structured walker trace facts lower into the existing 9h1 program-image attempt bridge. 9h4 adds only the terminal join between those two facts: first derive the 9h3 trace bridge, then ask 9h2 to bind exactly the run carried by that trace bridge, then expose a top-level attempt only if the 9h3 nested bridge and the 9h2 nested bridge agree on run fields, bridge status/reason, and the full 9f attempt tuple.

This is the last honest supplied-evidence join, not runtime progress. Another supplied-evidence wrapper should fail unless it brings new runtime evidence. The next honest movement is a real trace producer, a real program-image walker/loader, or moving the evidence into Layer 10 trace ingestion where observed traces become durable learning material.

## Pre-Review

Claude/Sema review: PASS_WITH_CHANGES.

Required changes:
- Make the join trace-first.
- Do not call 9h2 when the trace bridge is not trace-ready.
- Use only the trace-derived run for the capability join.
- Compare both independently produced 9h1 bridges before exposing a top-level attempt.
- Preserve trace and capability rows when the top-level attempt is withheld.

Grok-style adversarial review: PASS_WITH_CHANGES.

Required changes:
- Treat this as the terminal supplied-evidence layer, not a new runtime primitive.
- Reject any independent supplied-run input.
- Keep OOM, killed, stalled, timeout, wrong-value, loader-missing, error, and unknown statuses structured through trace -> 9h1 -> 9h2 -> top row.
- Add static guards against file IO, table text, generated proof-sibling tables, native calls, direct 9f construction, and 9g readiness construction.
- Name the boundary: another wrapper after this is not progress.

## Achieved

- Added a narrow 9h4 join row:
  `("runtime-program-image-fkb-traced-capability-bound-join" envelope readiness admission trace trace-bridge capability-join attempt status reason)`.
- `rpiftc-join-from-trace` asks 9h3 to produce the trace-backed bridge before it asks 9h2 anything.
- Invalid or non-ready trace evidence emits no 9h2 capability join and no top-level attempt.
- Ready trace evidence calls 9h2 with exactly `(rpiwt-bridge-run trace-bridge)`.
- The trace bridge run, the 9h2 join run, and the run and attempt stored inside both nested 9h1 bridges must agree before a top-level attempt is exposed.
- Non-ready readiness preserves the 9h2 unavailable/investigate/refused status and reason while withholding the top-level attempt.
- Mismatch rows keep both nested rows visible and withhold the top-level attempt.
- The nested attempt inside a trace bridge is trace evidence only. It is not capability-bound evidence unless 9h2 also binds the same run and both nested 9h1 bridges agree.
- Hard and soft observation statuses remain structured instead of being hidden in text detail.

## Deferred

- Real `.fkb` load/walk remains deferred.
- Real trace producer remains deferred.
- Durable Layer 10 trace ingestion remains deferred.
- Native `.dylib` execution remains deferred.
- Any C-seed expansion remains rejected unless it is a short-lived checkout-witness repair with a shrink receipt.
- Folding `.tbl` into `.fkb` and locale-specific `.sym` dependency material remains the executable artifact direction; this layer does not make generated Go/Rust/TS proof tables authoritative.

## Why This Boundary Holds

The repository already has enough supplied evidence wrappers to name the shape. Another supplied-evidence wrapper would add ceremony without producing new runtime truth. 9h4 is allowed because it closes one missing invariant between 9h2 and 9h3: the exposed capability-bound attempt is trace-backed. After this, the architecture must move toward producing traces from the actual program-image walker, or toward Layer 10 trace ingestion that can learn from observed traces without pretending to execute them.

## Validation

Actual:
- `./validate.sh form-stdlib/tests/runtime-program-image-fkb-traced-capability-bound-band.fk` -> `2147483647`, `1 ok, 0 divergent`.
- `./validate.sh form-stdlib/tests/runtime-artifact-executor-capability-band.fk` -> `2147483647`, `1 ok, 0 divergent`.
- `./validate.sh form-stdlib/tests/runtime-program-image-fkb-attempt-band.fk` -> `2147483647`, `1 ok, 0 divergent`.
- `./validate.sh form-stdlib/tests/runtime-program-image-fkb-capability-bound-band.fk` -> `2147483647`, `1 ok, 0 divergent`.
- `./validate.sh form-stdlib/tests/runtime-program-image-fkb-walker-trace-band.fk` -> `2147483647`, `1 ok, 0 divergent`.

Investigated failures before pass:
- First focused validation failed because `rpiftc-run-agrees?` missed one closing paren and the source accidentally paid that close back at the end of `rpiftc-join-from-trace`. Go/Rust reported `rpiftc-join-from-trace` as unbound; TypeScript reported `defn: expected )`. The repair moved the close to the correct helper and restored per-function nesting.
- Second focused validation returned `2147483391`, missing only bit `256`. The source withheld capability join and top-level attempt correctly; the band incorrectly demanded `investigate` for a trace that 9h3 classifies as `refused` because `steps-used-exceeds-budget` is impossible resource evidence. The band now accepts either non-ready terminal status while still requiring `trace-not-ready`, no capability join, and no top-level attempt.
- Post-review adversarial validation found a real invariant gap: the source compared the trace-derived run with the 9h2 join run, but not the run stored inside the 9h2 nested 9h1 bridge. The repair added a nested bridge run comparison and a band fixture where only that nested run drifts while the join run and attempt remain matching.
- Local follow-up tightened the same invariant for the nested 9h1 bridge attempt: both nested bridge attempts must agree, and the band now also forges a capability join where only the nested bridge attempt drifts while the exposed 9h2 attempt still matches.
- No OOM, killed process, hard stall, or timeout occurred in this slice. 9h1 and 9h2 validations ran longer than 30 seconds, were explicitly polled, and completed normally.

Failure policy:
- OOM, killed, stalled, timeout, wrong-value, loader-missing, error, unknown, parse failures, and validation wrong-values must be investigated and recorded. They are not ignorable noise.
