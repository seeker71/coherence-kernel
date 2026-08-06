# Runtime Computed Observation Ingest Guide Layer Review

Date: 2026-07-04

Layer: 10d, `form/form-stdlib/runtime-computed-observation-ingest.fk`

## Question

How does the computed 9h8 observation path enter the learning, ingesting, and
reasoning lane without pretending to be new runtime progress?

Answer: 10d is a guide. It consumes one 9h8 computed-observation row,
re-reads the nested 9h7 bind, trace, derived observation, and outcome, then
classifies the row as freeze-candidate work, investigation work, repair work,
held work, or refused work. It does not execute, mutate storage, install
selectors, or accept raw trace/observation/outcome authority.

## Pre-Review

Grok-style review: `PASS_WITH_CHANGES`.

Required changes:

- `rcoi-ingest` must accept exactly one input: a 9h8 `rpswo-row?`.
- Do not classify from the 9h8 top-level status alone; re-check the nested
  9h7 bind, bound trace, derived observation, derived outcome, and row/outcome
  agreement.
- Body-freeze candidates require a bound 9h7 row, `ok/completed` trace,
  `ok` observation, `complete` outcome, and agreeing 9h8 status/reason.
- `oom-killed`, `killed`, `stalled`, `timeout`, and `wrong-value` must preserve
  exact status and become investigation work.
- `loader-missing` and `error` become repair work. `fallback-available` is not
  success.
- Non-bound rows keep no derived observation/outcome and remain held or refused.
- Do not reuse the 10b `runtime-trace-ingest-record` shape.
- The receipt must name the current C edits as part of the shared work ledger
  and state that 10d adds no new C growth.

Claude/Sema review: `BLOCK` against the draft, then used as repair guidance.

Blockers found:

- The source did not parse because helpers before `rcoi-ingest` were missing
  close parens.
- The receipt was missing.
- The architecture map had no 10d row.
- Nested agreement was too weak: observation detail used substring checks,
  outcome fields were not fully rederived, and top-level 9h8 status/reason were
  not checked against the nested outcome.

## Achieved

- Added `runtime-computed-observation-ingest.fk` and a byte-identical grammar
  mirror.
- Added `runtime-computed-observation-ingest-band.fk`.
- Added a Layer 10d architecture map row.
- Exposed one row-producing entrypoint, `rcoi-ingest row`.
- Rejected malformed non-9h8 input without unsafe field access.
- Preserved the 9h8 row, nested 9h7 bind, bound trace, derived observation,
  derived outcome, artifact path, source hash, content hash, status fields,
  stop reason, exit code, and nested reasons.
- Rechecked row/bind envelope identity, symbol request, step budget, and input.
- Recomputed the expected 9h8 adapter row internally from the input row's
  envelope, readiness, admission, symbol request, budget, and input before
  allowing any freeze-candidate classification.
- Requires the supplied 9h8 row to be structurally equal to the internally
  recomputed `rpswo-adapt` row before freeze classification; this covers nested
  9h7 readiness/admission, 9h6 symbol-walk resolution/micro-walk provenance,
  bind/trace/observation/outcome fields, and top-level 9h8 status/reason.
- Rechecked exact observation derivation from the trace, including exact detail
  equality with `synthetic-computed-trace-observation: <trace-detail>`.
- Recomputed the expected 9c outcome from the envelope selection and derived
  observation, then compared route, attempted action, fallback, selection
  status, outcome status, observation status, selection reason, outcome reason,
  detail, and code.
- Checked the top-level 9h8 status/reason against the nested outcome
  status/reason before accepting agreement.
- Classified `ok/completed` + `complete` rows as body freeze candidates only
  after full nested agreement and recompute agreement.
- Preserved hard statuses as exact investigation work, including synthetic
  drift fixtures for OOM, killed, stalled, timeout, and wrong-value; these
  remain investigation work and never freeze candidates.
- Classified current computed error/fallback rows as repair work, never
  success.
- Held or refused non-bound and malformed rows without inventing
  `pending-observation`.
- Held real non-bound `investigate` rows as held work, while forged non-bound
  rows carrying raw observation/outcome data become nested-agreement
  investigation work.

## Deferred

- Real artifact load/walk/call remains deferred.
- Durable storage mutation, champion update, corpus insertion, and selector
  install remain deferred.
- This layer does not make `.fkb`, `.sym`, or `.dylib` runtime choices.
- OOM/killed/stalled production by the computed walker remains deferred; 10d
  currently proves those classifications with synthetic drift rows that can
  only become investigation work, not freeze candidates.
- Loader-missing production by the computed walker remains deferred; current
  soft repair validation uses computed error and fallback-available rows.
- Any C-seed capability movement remains outside 10d.

## Shared Work Ledger

The current repo contains tracked C-seed work in `runtime/fkwu-uni.c` and
`runtime/fkwu-optable.h`. That work is ours to account for, not outside noise.
Its current purpose and shrink debt are recorded in
`receipts/2026-07-04-source-runner-cseed-guide-review.md`.

10d did not edit either C file and does not depend on new C growth.

## Failure / Stall Notes

- No OOM-killed process occurred while implementing this layer.
- A focused validation ran longer than 30 seconds once and then completed; that
  was observed rather than ignored.
- The draft parse failure found by Claude/Sema was real and was repaired before
  final validation.
- A route comparison initially used string equality against a numeric route;
  Rust and TypeScript rejected it with a type-contract failure, and the layer
  now uses numeric equality for route.
- The guide-language static check was corrected so it does not reject the
  required runtime status word `investigate`.
- Post-review found that non-bound `investigate` rows were being turned into
  investigation work even though the 10d contract says non-bound rows are held
  or refused. The non-bound branch now derives held/refused classification from
  the recomputed 9h7 bind and rejects non-bound rows carrying raw derived
  observation/outcome data.
- Post-review also found that a self-consistent forged 9h8 row could reach the
  freeze path. `rcoi-ingest` now recomputes the 9h8 adapter row from the input
  row's base fields and investigates any bind/trace/observation/outcome drift
  before classification.
- A later forged-provenance fixture edit left one nested `and` unclosed; the
  focused validation stopped at parse time, and the proof file was repaired
  before rerun.
- After structural equality landed, one run returned `196607` because the
  grammar mirror still held the weaker partial comparison. The mirror was
  synced from the source and the proof returned `262143`.
- Fresh post-review validations ran for roughly 50 seconds and completed with
  full scores. They were recorded as long-running validations, not ignored as
  stalls. No OOM or killed process occurred.

## Validation

Bootstrap before this continuation:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
./fkwu --src /tmp/nvr.fk -> 11111
```

Focused validation before adding the mirror and ledger artifacts:

```text
./validate.sh form-stdlib/tests/runtime-computed-observation-ingest-band.fk
-> 65535, 1 ok, 0 divergent
```

Final focused validation:

```text
./validate.sh form-stdlib/tests/runtime-computed-observation-ingest-band.fk
-> 262143, 1 ok, 0 divergent
```

Focused validation after post-review structural recompute repair and mirror
sync:

```text
./validate.sh form-stdlib/tests/runtime-computed-observation-ingest-band.fk
-> 262143, 1 ok, 0 divergent
```

Neighbor validation:

```text
./validate.sh form-stdlib/tests/runtime-program-image-fkb-symbol-capability-bound-band.fk
-> 262143, 1 ok, 0 divergent

./validate.sh form-stdlib/tests/runtime-program-image-fkb-symbol-observation-band.fk
-> 262143, 1 ok, 0 divergent

./validate.sh form-stdlib/tests/runtime-trace-ingest-band.fk
-> 8388607, 1 ok, 0 divergent

./validate.sh form-stdlib/tests/runtime-trace-feedback-band.fk
-> 16777215, 1 ok, 0 divergent
```

## Post-Review

Claude/Sema continuity post-review: `BLOCK`.

- Non-bound `investigate` rows were being mapped to investigation work through
  generic terminal helpers, contradicting the 10d guide contract.
- Required repair: classify agreed non-bound rows as held/refused, require
  no-derived observation/outcome sentinels, and add a band case for an actual
  non-bound `investigate` row.

Grok-style adversarial post-review: `BLOCK`.

- A self-consistent forged 9h8 row could still freeze because the first repair
  re-read nested fields without recomputing the 9h8 adapter from base fields.
- Required repair: recompute `rpswo-adapt` or `rpswc-bind`, reject/investigate
  drift, force no-derived sentinels for malformed/non-bound binds, and add
  adversarial forged non-bound and forged self-consistent bound-row cases.

Claude/Sema nested-provenance post-review: `BLOCK`.

- The partial recompute equality still did not require exact equality for the
  whole supplied 9h8 row. Forged nested 9h7 readiness/admission or 9h6
  symbol-walk provenance could be missed if the selected comparison fields
  still matched.
- Required repair: compare the supplied row against the recomputed
  `rpswo-adapt` row structurally, and add forged nested readiness and
  symbol-walk provenance fixtures.

Repairs completed:

- `rcoi-ingest` now recomputes `rpswo-adapt` internally from the input row's
  base fields.
- `rcoi-row-recomputed-agrees?` now uses `value_eq` against the recomputed row
  before any freeze classification.
- Non-bound agreed rows classify as held or refused from the bind status/reason.
- Non-bound rows carrying raw observation/outcome data investigate as nested
  agreement drift.
- Self-consistent forged bound rows investigate and cannot freeze.
- Forged nested readiness and symbol-walk provenance rows investigate and
  cannot freeze.
- Hard-status drift rows still preserve the exact hard status as investigation
  work, so OOM/killed/stalled/timeout/wrong-value are not ignored.
- Focused validation after these repairs returns `262143`, `1 ok, 0 divergent`.

Final post-repair reviews:

- Claude/Sema-style final review: `PASS`. It verified the single 9h8 input,
  internal `rpswo-adapt` recompute, structural `value_eq`, non-bound
  held/refused handling, exact hard-status investigation work, soft repair
  work, byte-identical source/grammar mirror, no 10d C-seed dependence, and the
  focused band result `262143`, `1 ok, 0 divergent`.
- Grok-style adversarial final review: `PASS`. It tried the forged-row lanes
  again and found no 10d loophole for self-consistent forged 9h8 rows, altered
  nested 9h7 readiness/admission, altered 9h6 symbol-walk provenance, forged
  non-bound derived data, or hard-status drift to freeze or be ignored.
- Both final review validations ran past 30 seconds and completed normally in
  roughly 50 seconds. No OOM or killed process occurred.
