# 2026-07-04 -- Runtime program-image .fkb symbol walk layer review

## Layer

Layer 9h6: `form/form-stdlib/runtime-program-image-fkb-symbol-walk.fk`.

This layer is the symbol-addressed runtime surface over the Layer 9h5 computed
micro-walker. It consumes a request-ready 9e program-image envelope, an admitted
8h6 PIF byte admission, and an 8h8 symbol-id/canonical-key/id+key request. It
rejects `diagnostic-node` as a runtime request, resolves the symbol internally
over `pifbd-admission-pif`, then calls `rpmw-walk-entry`.

## Pre-Review

Claude/Sema and Grok-style review both returned `PASS_WITH_CHANGES`.

Required changes embodied:

- `diagnostic-node` rejected before resolution; diagnostic-node rejected means
  no resolution row, no micro-walk receipt, and no trace.
- 9g remains deferred; capability binding belongs in a later layer over the
  computed 9h6 trace.
- Resolution is computed internally, never accepted as supplied authority.
- 9h5 is called only after 8h8 returns `ready/symbol-entry-ready`.
- The receipt preserves top-level step budget and input value, the 8h8
  resolution, the 9h5 micro-walk receipt, and the computed trace so row
  agreement is observable.
- 9h5 timeout/error/refusal reasons are preserved instead of relabeled as
  symbol success.
- table authority rejected: no table text, presentation lens, generated
  proof-sibling table, or raw entry-index runtime API is authority here.

## Achieved

- Added `runtime-program-image-fkb-symbol-walk.fk`.
- Added exact grammar mirror `grammars/runtime-program-image-fkb-symbol-walk.fk`.
- Added focused band `runtime-program-image-fkb-symbol-walk-band.fk`.
- Added architecture row: `9h6. Runtime program-image .fkb symbol walk`.
- Removed the globally callable post-guard walk helpers after post-review
  showed that Form `defn`s are authority surface, not private helpers.
- The focused band covers symbol-id, canonical-key, and id+key runtime requests;
  malformed and unresolved 8h8 symbol cases; diagnostic request refusal; 9h5
  inherited envelope/admission/budget/timeout/error cases; receipt agreement;
  downstream trace compatibility; static forbidden authority scans; source/
  grammar parity; and reason coverage.

## Deferred

- 9g capability binding remains deferred to a later 9h7-style join.
- Dependency closure walking remains deferred; 8h8 dependency targets are
  carried as evidence only.
- Cross-module symbol resolution remains deferred.
- Artifact loading, freshness selection, selector integration, Layer 10 ingest,
  and native/dylib calling remain outside this layer.

## Failure And Stall Notes

- Initial focused validation failed with TypeScript parser errors:
  `unexpected token rparen` and then `defn: expected )`. Root cause was local
  malformed parenthesis/`defn` shape in the new source and band, not OOM or a
  killed process. Fixed by flattening the agreement helper and repairing
  malformed `defn` closure shape.
- A direct `../fkwu --src /tmp/rpsw-debug.fk` debug attempt failed with
  `fk_run_src: source exceeds FK_SOURCE_TEXT_CAP`. This is the known C seed
  source-size cap, not an OOM/killed event.
- Focused validation consistently crosses 30 seconds because the band performs
  repeated symbol-walk, audit, and reason-coverage invocations. It completes
  successfully; no OOM, killed process, or endless stall was observed.
- The first post-review authority-surface repair failed focused validation with
  `form-kernel-ts: unexpected token rparen at 358303`. Root cause was one extra
  closing parenthesis in the newly inlined walk branch, not OOM or a killed
  process. Fixed by balancing `rpsw-walk-after-admission-ready` in source and
  grammar mirror, then rerunning the focused band successfully.
- The adversarial recheck found the same authority issue one layer deeper:
  `rpsw-walk-after-admission-ready` could be called directly and reach
  `pifbd-admission-pif` / `pise-resolve` without repeating the outer admission
  and envelope guards. Fixed by removing that helper too and keeping PIF access,
  symbol resolution, and 9h5 delegation inside `rpsw-walk-symbol` only.

## Validation

Focused validation:

```text
./validate.sh form-stdlib/core.fk ... form-stdlib/runtime-program-image-fkb-symbol-walk.fk form-stdlib/tests/runtime-program-image-fkb-symbol-walk-band.fk
=> 268435455
```

Neighbor validation:

```text
8h6 program-image byte decode => 536870911
8h8 program-image symbol entry => 33554431
9h5 runtime program-image .fkb micro-walker => 16777215
```

The 9h6 focused band and 8h6 neighbor both crossed 30 seconds but completed
successfully with no OOM, killed process, or endless stall observed.

## Post-Review

Claude/Sema read-only review returned `PASS`: diagnostic requests are refused
before symbol resolution, 8h8 resolution is computed internally, 9h5 is called
only after ready resolution, the receipt carries agreement evidence, and the
map/receipt align.

Grok-style adversarial review returned `PASS_WITH_CHANGES`: it found that the
globally callable `rpsw-walk-after-ready-resolution` accepted caller-supplied
resolution authority and delegated to `rpmw-walk-entry`. That was a real blocker
because Form `defn`s are globally callable. The first fix removed that helper.
The narrow recheck then found the same issue in
`rpsw-walk-after-admission-ready`: a direct caller could bypass
`rpsw-walk-symbol` preguards and still obtain a carried symbol resolution before
9h5 refused the walk. The final fix removed both helpers entirely. The only walk
path now checks envelope/admission/decode/identity, rejects diagnostic mode,
computes `pise-resolve` from the admitted PIF, requires ready status, and
delegates to 9h5 inside `rpsw-walk-symbol`. The proof band now statically
forbids both removed helper names in the runtime source.

Final adversarial recheck returned `PASS`: the removed helpers are absent from
runtime source and grammar, source and grammar are byte-identical, the only
`rpmw-walk-entry` call is inside `rpsw-walk-symbol` after the full guard chain
and ready symbol resolution, the band forbids both helper names, and the receipt
matches the repair.
