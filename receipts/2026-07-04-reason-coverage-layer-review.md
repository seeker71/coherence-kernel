# 2026-07-04 -- reason coverage observability layer review

## Why This Layer Exists

Layer 9h0 returned a full bitmask, but Claude post-review still found two
unexercised refusal branches by comparing reason constants against band cases.
The fix was applied, but the failure mode was broader: a green mask can prove
many facts while still not proving that every declared reason branch is covered.

This layer gives bands a pure reason-coverage row:

```text
expected reason manifest
  + observed reasons produced by actual band invocations
  -> missing / unexpected / duplicate diagnostics
```

It is not reflection. It does not inspect defined names, generate tests, read
files, execute artifacts, or grow the C seed. Each consuming layer still owns an
explicit `*-reason-manifest` as the source of truth.

## Pre-Review

Grok pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Grok:

- define deterministic aggregate status precedence;
- define `rcov-covered?` exactly;
- do not leave this as a library-only layer: retrofit 9h0 with a reason
  manifest and require `rcov-covered?` in the 9h0 band;
- make the consuming layer's reason manifest the source of truth;
- clarify that observed 9h0 reasons are produced from actual pure branch
  invocations, not hand-written observed strings;
- record that reason coverage supplements bitmasks; it does not prove branch
  logic correctness by itself.

Claude pre-review verdict: `PASS_WITH_CHANGES`.

Required changes from Claude:

- observed lists must come from real invocations, not hand-typed strings;
- expected lists must reference reason constants/manifests, not duplicate
  literal strings;
- exercise status precedence with simultaneous failures;
- decide duplicate-observed semantics;
- test empty expected/observed and duplicate expected cases;
- make the 9h0 retrofit explicit.

## Implementation

Files:

- `form/form-stdlib/reason-coverage.fk`
- `grammars/reason-coverage.fk`
- `form/form-stdlib/tests/reason-coverage-band.fk`
- architecture update in `receipts/2026-07-03-core-layer-architecture-map.md`
- 9h0 retrofit in `form/form-stdlib/runtime-table-text-attempt.fk`
- 9h0 band retrofit in `form/form-stdlib/tests/runtime-table-text-attempt-band.fk`

The new prefix is `rcov-`.

Coverage row:

```text
("reason-coverage" expected observed missing unexpected
  duplicate-expected duplicate-observed status)
```

Status precedence is deterministic:

```text
missing > unexpected > duplicate-expected > duplicate-observed > covered
```

`rcov-covered?` is the pass predicate. It requires no missing reasons, no
unexpected reasons, and no duplicate expected reasons. Duplicate observed
reasons are retained as diagnostic information but do not make coverage fail,
because a legitimate band may exercise the same reason through multiple
branches.

Layer 9h0 now exposes `rtta-reason-manifest`, and its focused band compares
that manifest with reasons produced by actual `rtta-bridge-from-run`
invocations.

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

Focused reason coverage band:

```sh
./fkwu --src <(cat form/form-stdlib/core.fk \
  form/form-stdlib/reason-coverage.fk \
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
  form/form-stdlib/tests/reason-coverage-band.fk)
# -> 2147483647
```

The band proves:

- manifest boundaries and deferrals;
- exact covered case;
- missing reason detection;
- unexpected reason detection;
- duplicate observed diagnostic with `rcov-covered?` still true;
- duplicate expected failure;
- simultaneous missing, unexpected, duplicate-expected, and duplicate-observed
  status precedence;
- empty expected and observed lists are covered;
- row accessors;
- self-coverage over `rcov` status constants;
- 9h0 reason coverage from actual branch invocations;
- incomplete 9h0 observed reasons report the missing reason;
- duplicate 9h0 observed reasons are diagnostic but do not fail coverage;
- duplicate 9h0 expected reasons fail coverage;
- mirror parity and static forbidden-name scan.

The first focused run returned `2145386495`, missing the multi-failure
precedence bit. Investigation found the test expected one unexpected `z`, while
the row correctly preserved both observed unexpected `z` entries. The witness
was corrected to the row semantics and the band returned `2147483647`.

9h0 revalidation with the new `rcov-covered?` guard:

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
  form/form-stdlib/reason-coverage.fk \
  form/form-stdlib/runtime-table-text-attempt.fk \
  form/form-stdlib/tests/runtime-table-text-attempt-band.fk)
# -> 2147483647
```

## Deferred

- Dynamic reflection over defined names.
- Test generation.
- Proving branch logic is semantically correct.
- File IO.
- Runtime/process/artifact execution.
- Selector installation.
- C-seed growth.

## Alternatives

| Alternative | Decision | Reason |
| --- | --- | --- |
| Keep reason coverage in reviewer prose | Rejected | 9h0 already showed a full bitmask can miss unexercised reason branches. |
| Use dynamic reflection over function names | Deferred | Current Form layer has no trusted reflection contract for defined names, and reflection is not needed when manifests are explicit. |
| Treat duplicate observed reasons as failure | Rejected | A valid band may exercise the same reason through multiple branches. Duplicates are diagnostic. |
| Let each layer hand-roll reason coverage | Rejected | The comparison rules should be one small shared observability helper. |

## Post-Review

Grok post-review verdict: `PASS`.

Grok required changes: none.

Grok accepted the layer because the explicit `*-reason-manifest` to observed
reason comparison is pure, consumes real band invocations, preserves the
defined duplicate semantics, retrofits 9h0, keeps the C seed untouched, and is
backed by focused, downstream, mirror-parity, static-scan, and `git diff
--check` witnesses.

Claude post-review verdict: `PASS`.

Claude required changes: none.

Claude accepted the layer and specifically called out the corrected first
focused witness: the multi-failure test was aligned to the row semantics that
preserve both unexpected `z` observations, rather than weakening the row.
Claude also flagged a durable consumer footgun: future bands should gate on
`rcov-covered?`, not `rcov-status == "covered"`, because duplicate observed
reasons are diagnostic-only. That note was added at the predicate definition in
both the stdlib file and grammar mirror.
