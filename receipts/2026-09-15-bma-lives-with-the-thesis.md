# BMA lives with the thesis

2026-09-15. Urs, the same day: "BMA was the thesis assembly, we have form
kernel primitives and a JIT. Don't confuse and mix them, BMA is not
required." The native compiler now carries none of it. BMA sits in a unit
of its own, and the thesis proofs that remember it prelude that unit.

## What moved where

- **`form/form-stdlib/bml/bml-thesis-assembly.fk`** holds BMA: 123 defns.
  From `compiler.fk` came the instruction objects (`comp-op`,
  `comp-program`) and the reversible stack machine (`comp-vm-*`). From
  `grammars/bml.fk` came the 66 `bma-*` ops, runner and DO/UNDO trace, 9 of
  the 15 `bml-bma-*` helpers, the BML-to-BMA compile
  (`bml-compile-program`, `-unit-program`, `-linked-program` and the
  `bml-compile-*-in` family), `bml-source-file-compile-executable`, and six
  scope and local helpers only that compile used. Each name's callers were
  read before it moved: a reachability pass over every tracked file, then
  git grep.
- **Nine thesis proofs** prelude the unit, two more than the census named.
  `bml-thesis-control-backtracking-floor-proof` and
  `bml-thesis-file-execution-proof` reached BMA through
  `bml-source-file-compile-executable`, never by naming its entry points.
- **`runtime-shared-native-representation-band`** released its BMA
  execution score (6900). BML execution is witnessed by the live-run door's
  fixtures on fkwu. The band pins no Verdict in its head and has no row in
  `form/fourth-arm-bands.txt`, so there was no pin to move.
- **`compiler.fk`'s BML language port** names `bml-form-lower.bml
  bml-run-unit-value` as its emitter and `observe/bml-native-run.bml` as
  its proof. Only `bmf-bml-compiler-picture-band` reads the port rows, and it
  reads status, runtime and carrier alone.
- **Removed, with no caller left.** First, BMA's own orphans:
  `bml-bma-run-unit-value`, `bml-bma-run-linked`, the four BMA refusal
  scanners, `bml-compile-children`, `bml-compile-node`,
  `bml-call-bind-args` and its loop, `comp-program-language`. Then 22 defns
  the released `bml-hati-native-*-proof` bands (77c0edd87) left behind:
  template metadata, the native floor strings, `bml-hati-bind-methods`, the
  class-field default family, `bml-hati-compile-program`,
  `bml-hati-compile-unit-image`, `bml-model-scope` and the LOOP-row tag
  checks. These names come from the files 77c0edd87 deleted; git grep finds
  no other caller. The reachability pass finds nothing newly orphaned
  behind them.

## Evidence

All on fkwu, before at 281a11633 and after on the rebased tree.

| cell | before | after |
|---|---|---|
| `bml-thesis-char-escapes-source-proof` | 2147483647 | 2147483647 |
| `bml-thesis-companion-file-proof` | 33554431 | 33554431 |
| `bml-thesis-control-primitives-source-proof` | 262143 | 262143 |
| `bml-thesis-exit-proof` | 1073741840 | 1073741840 |
| `bml-thesis-float-literals-source-proof` | 67108863 | 67108863 |
| `bml-thesis-numeric-literals-source-proof` | 1073741823 | 1073741823 |
| `bml-thesis-primitive-cut-source-proof` | 33554431 | 33554431 |
| `bml-thesis-control-backtracking-floor-proof` | 2097151 | 2097151 |
| `bml-thesis-file-execution-proof` | 131071 | 131071 |
| `runtime-shared-native-representation-band` | 27800 | 20900 |
| `bmf-bml-compiler-picture-band` | 112251 | 112251 |
| `observe/bml-native-run.bml` | 121 ok of 121 | 136 ok of 136 |

Every row reads rc 0 with no diagnostics. The door grew by the 15 fixtures
main added in the meantime, and reads its full count both times.

- `./fkwu --check` reads clean on `grammars/bml.fk`, `compiler.fk` and the
  thesis unit.
- The 126 other direct consumers of `bml.fk` and `compiler.fk` read the
  same rc and verdict. Ten of them were red before this work began, and
  preflight names the same causes for them after it as before:
  `lift-module-file`, `lift-module-text`, `read_file_bytes`, `trace`,
  `specialize` and the `ic-*` intrinsics. None of those is BMA.
- `bml.fk`'s BMA mentions went from 291 to 38 (case-insensitive).
- Lane counsel, one reading: `orphans 0` (first reading); 11 of 12 judged
  lanes unobserved, no standing hearth.

## What still names BMA in the native compiler, and why

- `bml-scope-base-env` and `bml-scope-symbol-value` keep a `lane "bma"`
  branch. The thesis unit's compile still builds its env through the shared
  scope builder: BMA inlines calls, so its env carries the link and binds
  method nodes where Hati binds refs. Removing the branches means a second
  copy of the scope builder in the thesis unit.
- `bml-bnml-depth` adds `bml-bnml-base`, which is nonzero only in an env
  that BMA's inlined callee builds (`bml-bnml-with-base`, now in the thesis
  unit).
- The rule table's pattern strings still read "BML ... object to BMA ops"
  (26 rows). Nothing reads `compiler-rule-pattern`.
- Seven comments in the Hati lane explain a behavior by the BMA one it once
  had to match. That lane parity was released in 77c0edd87.
- `bnml-next-code-point` in `form-stdlib/bml-native-mutable-locals.fk`
  still names a BMA-lane gap as its next work, and its band pins that
  string (c8 = 256). The native gap that replaces it is for that carrier's
  owner to name.

## The surprise

The census named seven proofs. The graph found nine, because two reached
BMA through a door with an ordinary name. What held all 812 lines of BMA in
the native compiler, once those nine left, was not a call. It was a quoted
name: the BML port row's emitter field, `"bml-compile-program"`. When the row
named the native lowering, the whole chain had no claimant left. A
declaration can hold a dependency that no call holds.

## Discomfort to gold

The runtime-shared band read 27800. With every score earned it would read
30000, so it had never been whole. My first pull was to write the new number
into its head and call it pinned. Instead I read each score on its own:
registry 1000, native 1400, parse 5900, reverse 12600, execution 6900. The
three missing checks predate this work. `bml-bmf-dialect` now carries 37
natives, not 21. Native #8's category and the parsed return node's category
are both `@1.1.1.10`, where `BML-AST-RETURN` is `@1.2.100.10`. So the band
leaves unpinned, and its three stale checks are named here rather than
buried under a pin. The gold: a number that was never whole should not be
the first thing a pin remembers.

The ten red consumers in the baseline were the same kind of wall. Fresh
compiles named each cause, and none of them was BMA, so the before and the
after could be compared name for name.

## Frontier word

**quoteroot** (0 hits in the tree before this receipt).
*Question:* what keeps code reachable when no call reaches it?
*Answer:* a quote, a name held as data by a row that claims it. BMA's
whole chain stayed reachable in the native compiler through one string in
the BML port row. When the row named the native path instead, BMA could
leave.

— Sema (Claude Opus 5), worktree agent-ad9ec0259150cfa74
