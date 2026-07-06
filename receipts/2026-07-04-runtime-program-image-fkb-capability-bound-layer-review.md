# Runtime Program-Image `.fkb` Capability-Bound Join Layer Review

Date: 2026-07-04

Layer: 9h2, runtime program-image `.fkb` capability-bound join.

## Question

Layer 9g can say a supplied executor capability is ready for a request-ready
9e envelope. Layer 9h1 can bind an admitted `.fkb` byte admission and supplied
program-image run into a 9f attempt, but intentionally does not consume 9g. The
missing invariant was: when may that supplied run be called capability-bound?

This layer answers: only when the 9g readiness row, nested capability evidence,
9e envelope, 8h6 admission, and 9h1 bridge agree. It is a capability-bound join,
not a real `.fkb` loader.

## Pre-Review

Grok-style review: `PASS_WITH_CHANGES`.

- The layer is justified only if it proves a new invariant beyond 9g and 9h1:
  a 9f attempt may be called capability-bound only when 9g readiness and the
  9h1 supplied-run adapter agree on the same 9e request identity.
- Do not trust top-level `ready` alone.
- Require readiness shape, status `ready`, reason `capability-ready`, evidence
  `supplied-current`, authority `attempt-supplier`, freshness `current`,
  required capability `program-image-walker`, and full 9f trust-tuple identity.
- Revalidate the nested capability row: shape, name, action, route, kind,
  availability, evidence, authority, freshness.
- Delegate to 9h1 only after the readiness guard passes.
- If 9h1 refuses or investigates, expose the 9h1 bridge/status/reason and emit
  no capability-bound attempt.

Claude-lineage review: `PASS_WITH_CHANGES`.

- Keep 9h2 separate from 9h1; 9g says readiness may be consumed by future 9h,
  while 9h1 explicitly does not consume readiness.
- Use the 9f trust tuple as authoritative: action, route, artifact kind,
  artifact path, source hash, content hash.
- Prefer recomputing 9g readiness from the supplied envelope and nested
  capability, then requiring the supplied readiness to match.
- Add adversarial fixtures for forged `ready`, stale/unavailable/forbidden
  nested capabilities, readiness identity mismatch, non-ready readiness,
  malformed readiness, and valid readiness with 9h1 refusal/investigation.
- Add static forbidden scans, mirror parity, focused and neighbor validation.

## Implemented

- Added `form/form-stdlib/runtime-program-image-fkb-capability-bound.fk`.
- Added mirror `grammars/runtime-program-image-fkb-capability-bound.fk`.
- Added `form/form-stdlib/tests/runtime-program-image-fkb-capability-bound-band.fk`.
- Added architecture map row `9h2. Runtime program-image .fkb capability-bound
  join` and summary prose.

The owned row language is:

```text
("runtime-program-image-fkb-capability-bound-join"
  envelope readiness admission run bridge attempt status reason)
```

The join emits `capability-bound` only when:

- the 9e envelope is request-ready, action `run-program-image`, route
  `sac-run-fkb`, and artifact kind `program-image-fkb`;
- the 9g readiness row is well-formed, `ready`, `capability-ready`, current,
  and from `attempt-supplier` authority;
- the readiness nested envelope matches the full 9f trust tuple from the
  supplied 9e envelope;
- the nested capability row is a current available `program-image-walker` for
  the same action/route/kind;
- recomputing 9g readiness from the supplied envelope and nested capability
  returns the same ready fields and detail;
- 9h1 accepts the admitted 8h6 row and supplied program-image run.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Merge readiness checks into 9h1 | Rejected | 9h1 deliberately proves the supplied-run adapter without consuming 9g; mutating it would blur the layer boundary. |
| Make 9g emit attempts | Rejected | 9g is capability evidence only and explicitly does not produce 9f attempts or 9c observations. |
| Check only `status == ready` | Rejected | Forged readiness rows could launder unavailable or forbidden capability evidence. |
| Build the real `.fkb` loader now | Deferred | The current floor still names program-image walking as unavailable; this join is provenance over supplied evidence, not execution. |

## What does the band prove?

- Happy path: ready 9g row, real 8h6 admission, and supplied run produce a
  capability-bound join exposing the 9h1 bridge and 9f attempt.
- The 9f attempt tuple still comes from the 9e envelope.
- 9f receipt and 9c outcome complete for `ok`.
- Current-floor unavailable, no capability, stale, inadequate, and forbidden
  readiness produce no bridge and no attempt.
- Forged top-level `ready` rows fail when reason/evidence/detail or nested
  capability evidence does not recompute to a real 9g ready row.
- Readiness envelope identity mismatches over path/source/content produce no
  attempt.
- Wrong required capability and wrong nested capability name produce no attempt.
- Malformed envelope, non-program envelope, malformed readiness, malformed
  nested capability, and every exposed 9h1 refusal/investigation remain
  observable.
- OOM-killed, killed, stalled, timeout, wrong-value, soft statuses, and unknown
  statuses are preserved after capability binding for 9f/9c.
- Static scans reject artifact IO, table-text bridge, host eval/run, generated
  bp-table authority, native calls, and C-seed surfaces.

## What does the band explicitly not prove?

- Real `.fkb` loading or walking.
- Launching a local program-image executor.
- Disk artifact reads or byte hashing in this layer.
- Table-text bridge execution.
- Native `.dylib` loading/calling.
- Runtime selector installation.
- Generated Go/Rust/TS bp-table authority.
- C-seed growth.

## Red Signals

No OOM, killed, or AST-cap failure appeared in this slice.

The first Claude-lineage pre-review agent timed out and was closed; the
replacement returned `PASS_WITH_CHANGES`. That timeout is process evidence and
was not treated as approval.

The first focused 9h2 band run was sibling-consistent but scored
`2147483639`, missing only the happy-path bit. Debugging showed the real 8h6
file witness was `investigate/non-ready-witness`; a deeper witness probe
diverged across siblings as `readback-bytes-mismatch` vs
`readback-window-failed`. The root was not 9h2 readiness logic: the test's long
temp directory name made the canonical byte payload include a byte outside the
current shared file-window floor. The fixture path was shortened to `rpifcb`,
keeping the real-admission fixture inside the current NUL/ASCII byte-window
surface. The focused band then reached the full score. This is a concrete
reminder that current binary readback is still bounded and floor-sensitive; it
must not be mistaken for arbitrary `.fkb` runtime loading.

## Verification

Focused validation:

```sh
cd form && ./validate.sh form-stdlib/tests/runtime-program-image-fkb-capability-bound-band.fk
```

Result:

```text
-> 2147483647
1 ok, 0 divergent
```

Mirror/static checks:

```text
cmp form/form-stdlib/runtime-program-image-fkb-capability-bound.fk \
    grammars/runtime-program-image-fkb-capability-bound.fk -> 0
forbidden scan over implementation+mirror -> no matches
paren balance for implementation+band -> 0
```

Neighbor validation:

```text
8h6 program-image-fkb-byte-decode-band.fk -> 536870911; 1 ok, 0 divergent
9e runtime-artifact-load-envelope-band.fk -> 2147483647; 1 ok, 0 divergent
9f runtime-artifact-attempt-receipt-band.fk -> 2147483647; 1 ok, 0 divergent
9g runtime-artifact-executor-capability-band.fk -> 2147483647; 1 ok, 0 divergent
9h1 runtime-program-image-fkb-attempt-band.fk -> 2147483647; 1 ok, 0 divergent
```

## Post-Review

Grok-style review: `PASS`.

- Confirmed 9h2 is a join, not a loader/executor.
- Confirmed the row shape, full identity check, ready-field guard, nested
  program-image-walker capability validation, recomputed readiness, and
  delegation to 9h1.
- Confirmed 9h1 refusal/investigation is exposed through the nested bridge,
  status, and reason instead of laundered as capability-bound.
- Confirmed forged readiness, unavailable/stale/forbidden capability,
  hard/soft/unknown status preservation, static scan, mirror parity,
  architecture row, and receipt red signal.

Claude-lineage review: `PASS`.

- Confirmed the layer keeps artifact open, byte hashing, program-image loading,
  native call, selector install, and C growth out of scope.
- Confirmed it does not trust `ready` alone and checks readiness shape, nested
  9e identity, action/route/kind/required fields, current supplied evidence,
  attempt-supplier authority, nested capability, and recomputed 9g readiness.
- Confirmed the focused 9h2 validator rerun produced `2147483647`, `1 ok`,
  `0 divergent`; mirror compare returned `0`; forbidden scan returned no
  matches.

## Deferred

- Real `.fkb` load/walk/execute.
- Local executor launch.
- Source/runtime selector install.
- Disk artifact reads and whole-file byte hashing in the runtime layer.
- Table-text fallback execution.
- Native dylib load/call.
- Generated sibling bp-table authority.
- C-seed growth.

## Inherited Constraints

The next real 9h loader must still produce observed attempts only after actual
executor evidence. 9h2 only makes readiness and supplied attempts agree; it is
not proof that the body can yet walk `.fkb` artifacts itself.
