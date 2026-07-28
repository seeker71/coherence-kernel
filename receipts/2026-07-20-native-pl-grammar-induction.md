# Native PL grammar induction — observed held-out lane

Witnessed: 2026-07-20

## What became real

The lane in `model/pl-grammar-induction.fk` receives raw Python and Go program
strings paired with neutral typed assignment IR.  Native Form:

1. scans identifiers, integer constants, and assignment operators;
2. aligns changing surface values with typed IR slots;
3. evaluates exact-row, typed-skeleton, and untyped candidates;
4. admits the lowest-cost candidate that covers both observations and remains
   type-faithful;
5. lowers the selected skeleton to a BMF grammar value;
6. parses held-out source through `g-parse-full` into neutral typed IR; and
7. walks the other induced rule to emit the target programming language.

No Bash, TypeScript, Python, Go, Rust, or C performs any step above.  A shell
`cat` only composes Form preludes for the existing `fkwu --src` checkout door.
The C seed did not grow.

## Non-training observations

Training rows:

- Python: `retry_budget = 7`, `batch_limit = 19`
- Go: `retryBudget := 7`, `batchLimit := 19`

Held out from induction:

- Python `warehouse_reorder_point = 314159` parsed to identifier
  `warehouse_reorder_point`, typed integer `314159`, then emitted as Go
  `warehouse_reorder_point := 314159`.
- Go `remainingInventory := 271828` parsed to identifier
  `remainingInventory`, typed integer `271828`, then emitted as Python
  `remainingInventory = 271828`.

The exact-row candidates cover only one of two training rows.  The selected
Python typed skeleton costs 6 description units; the selected Go skeleton
costs 7 because `:=` has one additional literal byte.  Both cover 2/2 pairs.
The untyped candidate also covers token shapes but is rejected because it does
not preserve typed IR roles.  The admitted rules retain zero training names or
constants.

## Live framebuffer evidence

The direct native door printed these per-stage dispatch deltas in one run:

| Form stage | Observed value | Native dispatches |
|---|---:|---:|
| paired-surfaces-observed | 4 paired programs | 1,436 |
| minimal-rules-selected | 6 candidates | 65,160 |
| bmf-grammars-materialized | 2 grammar values | 1,749 |
| held-out-programs-parsed | 2 programs | 40,598 |
| target-programs-emitted | 2 programs | 7,737 |
| row-memorization-rejected | 0 retained values | 3,054 |

Whole-window native dispatches: `130637`.  Outcome:
`held-out-parse-and-cross-emission-complete`.

All stage rows are attributed framebuffer events built from exact before/after
`kernel_stat` snapshots.  Zero-millisecond wall times are honestly retained;
the operations completed below clock resolution, while dispatch counts expose
their nonzero work.

## Acceptance

The Form-native band returned `32767` (all fifteen bits): scanners, candidate
coverage/cost, typed selection, BMF grammar materialization, both held-out
parses, both cross-language emissions, unseen-value survival, invalid
identifier rejection, wrong-operator rejection, malformed-IR rejection, and
trace-first admission.

## Honest floor

This is one reusable statement family: typed integer assignment.  It does not
claim whole-language grammar discovery, control-flow induction, expression
precedence induction, or semantic equivalence beyond this neutral IR family.
Those remain named work, not inferred completion.

The surprising teaching was that the smallest faithful grammar differs by one
description unit solely because Go's learned invariant is two bytes.  The
discomfort was the earlier temptation to call fixed authored templates
“induction”; it turned to gold when exact-row memorization became a competing
candidate that the same native selector visibly rejects.
