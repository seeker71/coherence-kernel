# 2026-07-04 -- runtime table-text attempt adapter layer review

## Why This Layer Exists

Layer 8i can emit exact `.tbl` text from a valid 8h program-image envelope, and
the current checkout can run that text through the existing table executor.
The runtime lane still had a gap: 9e creates request-ready envelopes, 9g names
supplied capability readiness, and 9f consumes supplied attempts, but nothing
in the body could name how an external `.tbl` run becomes a 9f supplied attempt
without laundering it as a live binary program-image load.

This layer is a supplied-observation adapter:

```text
9e request-ready program-image envelope
  + valid 8h program-image envelope
  + exact 8i table-text witness
  + supplied table-run observation
  -> 9f supplied-attempt row
```

It is not an executor. It does not launch the table runner, walk a binary
program image, call native artifacts, install a selector, or grow the C seed.

## Pre-Review

Grok pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Grok:

- do not require output lines for OOM/killed/stalled/timeout/wrong-value or
  unknown statuses; otherwise zero-output kills would be suppressed;
- bind the supplied run path to the table-text witness path;
- choose deterministic refused/investigate outcomes for wrong envelope kinds;
- keep bridge status `attempt-ready` when identity/text binding succeeds, even
  if the supplied run status is hard or unknown.

Claude pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Claude:

- same zero-output hard-status correction;
- deterministic status/reason mapping: malformed rows refuse, well-formed
  mismatches investigate;
- mark attempts as `table-text-witness` so a content-equivalent table run is
  not recorded as a silent binary `.fkb` load;
- state that binding is by exact table text and a supplied witness path, while
  true binary program-image loading remains deferred.

## Implementation

Files:

- `form/form-stdlib/runtime-table-text-attempt.fk`
- `grammars/runtime-table-text-attempt.fk`
- `form/form-stdlib/tests/runtime-table-text-attempt-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`

The new prefix is `rtta-`.

Rows:

```text
("runtime-table-text-witness" table-path table-text)

("runtime-table-text-supplied-run" table-path first-value
  line-count exit-code status detail)

("runtime-table-text-attempt-bridge" envelope pif-envelope
  witness run attempt status reason)
```

`rtta-bridge-from-run` emits an attempt only when:

- the 9e envelope is request-ready, action `run-program-image`, route
  `sac-run-fkb`, artifact kind `program-image-fkb`;
- the 8h program-image envelope is valid;
- the 8h source hash, artifact path, and content hash match the 9e envelope;
- the table-text witness text equals `pite-table-text-from-envelope`;
- the supplied run path equals the witness path;
- `ok` status has at least one output line.

Hard and unknown statuses do not require an output line. They still become
attempts after identity/text/path binding so 9f and 9c can investigate them.

Attempt details are prefixed with `table-text-witness`, making the proxy
witness explicit downstream.

## Witnesses

Required floor before edits:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
# known fread/getsockname warnings only
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
native-vs-rented-check -> 11111
```

Focused band:

```sh
./fkwu --src <(cat form/form-stdlib/core.fk \
  form/form-stdlib/source-artifact-cache.fk \
  form/form-stdlib/source-artifact-descriptor.fk \
  form/form-stdlib/runtime-artifact-plan.fk \
  form/form-stdlib/runtime-artifact-selector.fk \
  form/form-stdlib/runtime-artifact-outcome.fk \
  form/form-stdlib/runtime-artifact-retry.fk \
  form/form-stdlib/runtime-artifact-load-envelope.fk \
  form/form-stdlib/runtime-artifact-attempt-receipt.fk \
  form/form-stdlib/program-image-fkb.fk \
  form/form-stdlib/program-image-tbl-emit.fk \
  form/form-stdlib/runtime-table-text-attempt.fk \
  form/form-stdlib/tests/runtime-table-text-attempt-band.fk)
# -> 2147483647
```

The 9h0 band proves:

- manifest boundaries and deferrals;
- valid request-ready program-image envelope + valid 8h envelope + exact 8i
  table-text witness + supplied `ok` table run -> `attempt-ready`;
- the emitted 9f supplied attempt takes action, route, kind, path, source hash,
  and content hash from the 9e envelope;
- 9f accepts the attempt and 9c completes the `ok` outcome;
- the attempt detail carries `table-text-witness`, and that marker survives
  through the 9f receipt into the 9c outcome detail;
- `oom-killed` with zero output lines still becomes an attempt and 9c
  investigates it as a hard observation;
- `stalled` with zero output lines still becomes an attempt and 9c
  investigates it as a hard observation;
- `wrong-value` still becomes an attempt and 9c investigates it;
- an unknown zero-output status still becomes an attempt and 9c investigates it
  as an unknown observation;
- table-text mismatch and table-path mismatch investigate with no attempt;
- 8h artifact path, source-hash, or content-hash mismatch investigates with no
  attempt;
- invalid 8h envelope investigates with no attempt;
- compile-source/non-program-image envelope investigates with no attempt;
- malformed envelope refuses with no attempt;
- malformed 8h program-image envelope refuses with no attempt;
- malformed table-text witness refuses with no attempt;
- malformed table-run row refuses with no attempt;
- `ok` with zero output lines investigates with no attempt;
- empty table-text witness investigates with no attempt;
- mirror parity and static forbidden-name scan.

Neighboring layer revalidation:

```text
program-image-fkb-band                    -> 2147483647
program-image-tbl-emit-band               -> 2147483647
runtime-artifact-load-envelope-band       -> 2147483647
runtime-artifact-attempt-receipt-band     -> 2147483647
runtime-artifact-executor-capability-band -> 2147483647
```

Static checks:

```text
cmp grammars/runtime-table-text-attempt.fk form/form-stdlib/runtime-table-text-attempt.fk -> 0
forbidden binary/runtime route scan over runtime-table-text-attempt mirrors -> no hits
git diff --check -> clean
```

## Deferred

- Real binary `.fkb` write/read.
- Binary program-image load/walk.
- Launching the table executor from Form.
- Startup selector installation.
- Native `.dylib` loading/calling.
- Whole-file artifact hashing.
- Hidden fallback or retry execution.
- C-seed growth.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Treat the `.tbl` run as binary `.fkb` execution | Rejected | It would launder a content-equivalent proxy into a capability that is still deferred. |
| Suppress hard zero-output runs until they have a first line | Rejected | OOM/killed/stalled runs often have no output; suppression would violate the investigation rule. |
| Bind the `.tbl` path to the `.fkb` artifact path | Rejected | The paths are intentionally different. Binding is by exact table text plus a supplied witness path. |
| Install the real 9h executor now | Deferred | This layer only adapts supplied observations. A real executor must be a separate controlled producer. |

## Post-Review

Grok post-review verdict: `PASS`.

Grok accepted the layer as a supplied-observation adapter only, with identity
coming from the 9e envelope, table text equality binding the proxy witness, and
hard/unknown zero-output statuses flowing through to 9f/9c investigation. Grok
requested no code changes.

Claude post-review verdict: `PASS_WITH_CHANGES`.

Claude accepted the design and implementation but found two unexercised refusal
branches: malformed 8h program-image envelope rows and malformed table-text
witness rows. Both are now asserted inside the focused band's composite
boundary bit. The repaired focused band still returns `2147483647`, the mirror
check still returns `0`, the forbidden-name scan still has no hits, and
`git diff --check` remains clean.

Follow-up after Layer 10a reason coverage: 9h0 now exposes
`rtta-reason-manifest`, and its band uses `rcov-coverage` over reasons produced
by actual `rtta-bridge-from-run` branch invocations. The 9h0 band still returns
`2147483647` with the new structural reason-coverage guard.
