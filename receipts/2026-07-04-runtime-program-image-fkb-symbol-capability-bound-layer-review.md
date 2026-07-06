# 2026-07-04 -- Runtime program-image .fkb symbol capability bind layer review

## Layer

Layer 9h7: `form/form-stdlib/runtime-program-image-fkb-symbol-capability-bound.fk`.

This layer capability-binds the computed Layer 9h6 symbol walk. It consumes a
request-ready 9e program-image envelope, a 9g program-image-walker readiness
row, an admitted 8h6 PIF byte admission, and an 8h8 symbol request plus budget
and input. It proves the 9g readiness row, then recomputes
`rpsw-walk-symbol` internally. That recomputation is the only source of a
capability-bound trace.

It has no supplied 9h6 receipt authority and no independent trace input.

## Pre-Review

Claude/Sema and Grok-style adversarial pre-review both returned
`PASS_WITH_CHANGES`.

Required changes embodied:

- 9h7 is a new computed join, not a reuse of old 9h1/9h2/9h3/9h4
  supplied-evidence surfaces.
- The public entry point accepts envelope/readiness/admission/symbol-request/
  budget/input and recomputes `rpsw-walk-symbol`; it does not accept a finished
  9h6 receipt as authority.
- 9g readiness is checked by full envelope identity, required
  `program-image-walker`, `ready/capability-ready/supplied-current/
  attempt-supplier/current` fields, nested capability tuple/evidence, and
  recomputed `raec-readiness-from-envelope-capability`.
- `rpsw-receipt-agrees?` is necessary after recomputation but is not treated as
  provenance by itself.
- Non-ready readiness preserves unavailable/investigate/refused status and the
  9g reason without exposing a bound trace.
- Non-produced 9h6 walks preserve the 9h6 status/reason without relabeling as
  capability success.
- The source forbids supplied run/trace/attempt authority, old capability-bound
  supplied joins, table text, `.tbl`, `.sym`, generated proof-sibling table
  authority, artifact IO, selector mutation, and C-seed growth.

## Achieved

- Added `runtime-program-image-fkb-symbol-capability-bound.fk`.
- Added exact grammar mirror
  `grammars/runtime-program-image-fkb-symbol-capability-bound.fk`.
- Added focused band
  `runtime-program-image-fkb-symbol-capability-bound-band.fk`.
- Added architecture row:
  `9h7. Runtime program-image .fkb symbol capability bind`.
- The focused band covers happy binding, internal 9h6 recomputation, non-ready
  9g statuses, forged readiness fields, nested capability drift, recomputed
  readiness mismatch, readiness envelope drift, malformed envelope/readiness/
  capability cases, symbol-resolution refusal, diagnostic refusal, negative
  budget refusal, timeout/error computed traces, no bound trace on failure,
  static forbidden-authority scans, source/grammar parity, and map/receipt
  evidence.

## Deferred

- 9f attempt and 9c observation production remain deferred; this layer exposes
  a capability-bound computed trace, not an attempt.
- Artifact loading, byte hashing, filesystem freshness selection, selector
  installation, dependency closure walking, cross-module symbol resolution,
  Layer 10 ingest, and native/dylib calling remain outside this layer.
- Downstream layers still need to decide how a capability-bound computed trace
  becomes an attempt/observation without reintroducing supplied-evidence
  authority.

## Failure And Stall Notes

- Initial focused validation returned `260095` instead of expected `262143`.
  Kernels agreed; this was not OOM, not a killed process, and not a stall. The
  missing bit was the symbol-failure preservation branch.
- Debug scoring of that branch returned `62`, showing that the missing
  canonical-key reason was preserved but the test expected `investigate` while
  8h8 correctly marks `canonical-key-not-found` as `refused`. Fixed the band to
  preserve the lower-layer status exactly.
- A source guard-order issue was also repaired: non-produced 9h6 receipts are
  now preserved before asking for produced-trace agreement. Agreement is
  required only for trace-produced 9h6 receipts that might expose a bound trace.
- Post-review found a real global-`defn` authority leak: `rpswc-bound` accepted
  a caller-supplied 9h6 receipt and exposed `rpsw-receipt-trace` without
  recomputing 9h6 or proving readiness. A second review also rejected the
  no-bound row helpers as global 9h7 row-manufacturing surfaces.
- Fixed by removing every row-producing helper from the 9h7 source:
  `rpswc-bound`, `rpswc-join-row`, `rpswc-terminal`, `rpswc-refused`,
  `rpswc-investigate`, `rpswc-unavailable`, `rpswc-readiness-terminal`, and
  `rpswc-symbol-walk-terminal`. `rpswc-bind` is now the only function that
  constructs a 9h7 row, and the bound-trace row is built inline only after 9g
  readiness and recomputed 9h6 trace agreement have both passed.
- The first helper-removal repair failed focused validation with parser errors:
  Go/Rust reported an unclosed top-level list and TypeScript reported
  `unterminated list`. Root cause was one missing close for the source-level
  `(do ...)`, found with a read-only parenthesis scanner that ignores strings
  and comments. Fixed in source and grammar mirror, then reran the focused band
  successfully.

## Validation

Focused validation:

```text
./validate.sh form-stdlib/core.fk ... form-stdlib/runtime-program-image-fkb-symbol-capability-bound.fk form-stdlib/tests/runtime-program-image-fkb-symbol-capability-bound-band.fk
=> 262143
```

Result: `1 ok, 0 divergent`; fkwu, Go, Rust, and TypeScript agree.

Neighbor validation:

```text
9g runtime artifact executor capability => 2147483647
9h6 runtime program-image .fkb symbol walk => 268435455
```

The 9h6 neighbor crossed 30 seconds and completed successfully. No OOM, killed
process, or endless stall was observed.

## Post-Review

Claude/Sema and Grok-style adversarial post-review both returned
`PASS_WITH_CHANGES` on the first implementation. Both found the same authority
class: global row-producing helpers could manufacture 9h7 rows outside the
single recomputing bind path. The fix removed all row-producing helpers and
tightened the static band so those helper names are forbidden in the runtime
source.

Final Claude/Sema recheck returned `PASS`: `rpswc-bind` is the only runtime
source function constructing 9h7 rows; removed row-producing helpers are absent
from source and grammar; the bound trace is inline only after 9g checks,
recomputed readiness, internal `rpsw-walk-symbol`, `trace-produced`,
`rpsw-receipt-agrees?`, and trace shape; no old supplied-evidence or table/
artifact/C surfaces remain; source and grammar are byte-identical.

Final Grok-style adversarial recheck returned `PASS`: no remaining global
row-producing helper leak was found; the only `rpsw-receipt-trace` exposure is
inside `rpswc-bind` after the full computed path; static forbids cover the
removed helper class plus old supplied-evidence, table, symbol, IO, native, and
C-seed strings; source and grammar are byte-identical.
