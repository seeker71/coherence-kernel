# Runtime Program-Image `.fkb` Attempt Adapter Layer Review

Date: 2026-07-04

Layer: 9h1, runtime program-image `.fkb` supplied-attempt adapter.

## Question

Layer 9h0 could turn an exact table-text witness plus a supplied table run into
a 9f attempt. That kept the checkout witness observable, but it still named the
old text projection path. This layer asks for the binary sibling: can a
request-ready program-image `.fkb` envelope and an admitted 8h6 byte admission
bind a supplied program-image run into the same 9f/9c observation lane without
claiming that the body can already load or walk `.fkb` artifacts itself?

## Pre-Review

Claude-lineage review: `PASS_WITH_CHANGES`.

- Name the layer as a supplied program-image `.fkb` attempt adapter, not a
  loader/executor.
- Bind against `pifbd-admission-pif`; content hash and artifact mtime are
  external metadata, not decoded payload fields.
- Emit the exact 9f supplied-attempt shape and use the 9e envelope identity
  tuple.
- Add a detail marker so the attempt is not laundered as table text or native
  execution.
- Preserve every non-`ok` status after identity binding; only `ok` needs a
  positive output indicator.
- Keep current-floor program-image walking unavailable in 9g; 9h1 is not 9g
  enforcement.

Grok-style review: `PASS_WITH_CHANGES`.

- Do not trust `pifbd-admitted?` alone; require admission row shape, admitted
  status/reason, decoded decode row, valid admitted PIF, and decode/PIF
  consistency.
- Build attempts from the 9e envelope tuple, not from supplied run identity.
- Preserve soft, hard, and unknown supplied statuses after identity binding so
  9f/9c can normalize them.
- Add adversarial fixtures for malformed/non-program envelopes, malformed and
  non-admitted admissions, forged admitted rows, identity mismatches, malformed
  runs, zero-output `ok`, and zero-output hard/unknown statuses.
- Add exact mirror parity, static forbidden-name scans, architecture update, and
  receipt answers to the standard layer questions.

## Implemented

- Added `form/form-stdlib/runtime-program-image-fkb-attempt.fk`.
- Added mirror `grammars/runtime-program-image-fkb-attempt.fk`.
- Added `form/form-stdlib/tests/runtime-program-image-fkb-attempt-band.fk`.
- Added architecture map row `9h1. Runtime program-image .fkb attempt adapter`
  and summary prose.

The owned language is adapter rows:

```text
("runtime-program-image-fkb-supplied-run"
  artifact-path first-value output-count exit-code status detail)

("runtime-program-image-fkb-attempt-bridge"
  envelope admission run attempt status reason)
```

The adapter emits `raar-supplied-attempt` only when:

- the 9e envelope is request-ready, action `run-program-image`, route
  `sac-run-fkb`, and artifact kind `program-image-fkb`;
- the 8h6 admission row is well-formed, admitted with admitted reason, decoded,
  carries a valid PIF, and the decoded payload still matches that PIF;
- admitted PIF artifact path, source hash, and content hash match the 9e
  envelope;
- supplied run path equals the admitted PIF artifact path;
- `ok` status has positive output count.

Attempts are marked with `program-image-fkb-admission`. Every non-`ok` status
including `loader-missing`, `error`, OOM-killed, killed, stalled, timeout,
wrong-value, and unknown statuses is preserved after identity binding.

## Alternatives

- Real `.fkb` loading/walking in this layer: rejected. That belongs to future
  9h and would require actual executor capability, disk bytes, and C-seed
  pressure that this layer must not claim.
- Reusing 9h0 table-text rows: rejected. It would hide that the evidence is now
  an admitted binary `.fkb` path, not a `.tbl` projection.
- Trusting `pifbd-admitted?` alone: rejected. The adapter now checks admission
  shape, status, reason, decode status, PIF validity, and decode/PIF consistency.
- Using the supplied run identity fields for the attempt tuple: rejected. The
  9f trust tuple is taken from the 9e envelope.

## What does the band prove?

- A real 8h6 readback/admission can feed the adapter.
- A valid bridge emits a 9f supplied attempt whose identity tuple equals the 9e
  envelope.
- The 9f receipt and 9c outcome paths complete for `ok`.
- OOM-killed, killed, stalled, timeout, wrong-value, unknown, and soft statuses
  remain attempts after identity binding and normalize downstream.
- Empty-output `ok` investigates with no attempt.
- Malformed/non-program envelopes, malformed/non-admitted admissions, invalid
  PIFs, nondecoded decode rows, decode/PIF mismatch, identity mismatch,
  malformed runs, negative output, and run-path mismatch produce no attempt.
- Reason coverage is branch-based through `rcov-covered?`.
- The implementation and grammar mirror are byte-identical.
- Static scans reject table-text bridge names, binary IO helpers, host eval/run
  names, generated bp-table authority, and C-seed surfaces.

## What does the band explicitly not prove?

- Real `.fkb` loading or walking.
- Launching a local program-image executor.
- Consuming or enforcing 9g readiness.
- Disk reads or whole-file binary hashing at this layer.
- Selector installation or fallback execution.
- Native `.dylib` calls.
- Generated Go/Rust/TS bp-table authority.
- C-seed growth.

## Red Signals

No OOM, killed, or AST-cap failure appeared while implementing this slice.

One adjacent-stack probe was initially run with multiple band files in a single
`validate.sh` invocation and failed with an unbound prelude symbol. That was
command-shape misuse, not a semantic failure; the bands were rerun individually.

The 9h0 neighbor then exposed a real static-mirror harness bug: the band read
`form/form-stdlib/runtime-table-text-attempt.fk` while validation runs from
`form/`, so Rust and TypeScript received `Null` from `read_file` and crashed in
`str_find`; Go's path behavior masked the issue. The fix was to make the static
read helper null-safe and add root/form path fallback for both the implementation
and grammar mirror paths. A follow-up parse/arity mistake in that nested boolean
chain was also repaired and revalidated. This was not an OOM, but it is the same
class of lesson: a sibling divergence is evidence, not noise.

The focused 9h1 rerun passed after one 30-second poll and completed cleanly on
the next wait. It was not classified as a stall because the command exited
successfully without intervention.

## Verification

```sh
cd form && ./validate.sh form-stdlib/tests/runtime-program-image-fkb-attempt-band.fk
```

Result:

```text
→ 2147483647
1 ok, 0 divergent
```

Neighbor validation:

```sh
cd form && ./validate.sh form-stdlib/tests/program-image-fkb-byte-decode-band.fk
cd form && ./validate.sh form-stdlib/tests/runtime-artifact-load-envelope-band.fk
cd form && ./validate.sh form-stdlib/tests/runtime-artifact-attempt-receipt-band.fk
cd form && ./validate.sh form-stdlib/tests/runtime-artifact-executor-capability-band.fk
cd form && ./validate.sh form-stdlib/tests/runtime-table-text-attempt-band.fk
```

Results:

```text
8h6 program-image-fkb-byte-decode-band.fk -> 536870911; 1 ok, 0 divergent
9e runtime-artifact-load-envelope-band.fk -> 2147483647; 1 ok, 0 divergent
9f runtime-artifact-attempt-receipt-band.fk -> 2147483647; 1 ok, 0 divergent
9g runtime-artifact-executor-capability-band.fk -> 2147483647; 1 ok, 0 divergent
9h0 runtime-table-text-attempt-band.fk -> 2147483647; 1 ok, 0 divergent
```

The 9e, 9f, 9g, and 9h0 neighbor bands also had stale multi-line/root-style
prelude headers tightened to single-line `form-stdlib/...` headers so
`validate.sh` loads the intended modules from the `form/` working directory.

## Post-Review

Claude-lineage review: `PASS`.

- Confirmed 9h1 is adapter-only: no loader/executor claim, `.fkb` open/walk,
  selector install, 9g enforcement, or C/runtime growth dependency.
- Confirmed admission readiness is stronger than `pifbd-admitted?` and checks
  row shape, admitted status/reason, decoded row, valid admitted PIF, and
  decode/PIF consistency.
- Confirmed the 9f attempt tuple comes from the 9e envelope, not the supplied
  run row.
- Confirmed marker, reason coverage, status preservation, mirror parity, and
  forbidden scan.
- Independently reran the focused validator and observed `2147483647`, `1 ok`,
  `0 divergent`.

Grok-style review: `PASS`.

- Confirmed the same adapter-only boundary and stronger admission checks.
- Confirmed OOM, killed, stalled, timeout, wrong-value, soft, and unknown
  statuses are preserved as supplied attempts after identity binding, while
  zero-output `ok` investigates with no attempt.
- Confirmed 9h0 static-read path hygiene and single-line prelude repair.
- Noted the broader worktree still contains dirty `runtime/fkwu-*` and generated
  bp-table files, but found no 9h1 dependency on those changes.

## Deferred

- Real `.fkb` loading/walking.
- Local executor launch.
- 9g readiness consumption/enforcement.
- Selector install.
- Fallback execution.
- Disk reads and whole-file binary hashing at this layer.
- Table-text parsing/emission.
- Native dylib calls.
- Generated sibling bp-table authority.
- C-seed growth.

## Inherited Constraints

The next layer must keep the 9f identity tuple authoritative, preserve hard
attempt statuses for investigation, and only claim execution after a real
program-image executor has supplied an observed attempt. 9h1 is an honest
adapter, not proof that the body can yet walk `.fkb` artifacts itself.
