# Receipt — R27: CONTROL-INVITE gets a real coordinate, the node_category claim gets a real check (2026-09-03)

## The gap, precisely

`grammars/control-invite-grammar.fk` mints ONE blueprint, `CONTROL-INVITE`, and its band
(`grammars/tests/control-invite-grammar-band.fk`) claims bit 64: "every produced node carries the
CONTROL-INVITE op as its category" — checked as
`(node_eq (node_category (run-node res)) (bp (cig-op)))`.

Before this pass, `CONTROL-INVITE` had **no registered coordinate anywhere**:

- not in `form/form-stdlib/blueprint-registry.json` (the canonical 442-row registry — confirmed by
  loading it and searching every `name` and every `aliases` entry, zero hits)
- not in `form/form-stdlib/form-ontology-bp.fk`'s `FOL-BP-BOOTSTRAP-TABLE` row list or its generated
  `fol-bp-coords` decision chain (both grepped directly, zero hits)
- not in `form/form-stdlib/bmf-core.fk`'s own local `bmf-bp-table` (the table `build-emit` actually
  resolves templates through — grepped directly, zero hits)

Three separate hand-held mirrors, and none of them carried the name.

## Why it read green (1023) before it read 959

`grammars/tests/control-invite-grammar-band.fk`'s own `cat?`-style check calls `(bp (cig-op))` for
its oracle side. On `fkwu`, `bp` is a native (flt-ops tag 45) that **passes its string argument
through unchanged** — `form-ontology-bp.fk`'s own header documents this exactly ("`bp` is a native in
fkwu's optable... in call position the primitive wins... the native is a pass-through: tag 45
evaluates to its argument unchanged"). So `(bp "CONTROL-INVITE")` on fkwu answers the STRING
`"CONTROL-INVITE"`, never a NodeID.

At the grammar's original writing (2026-07-01, `receipts/2026-07-01-choice-lane-control-invites.md`,
verdict 1023), the emitting side (then `grammars/bmf-core.fk`, pre-twin-reunion) apparently ran the
SAME unregistered name through the SAME native pass-through when building the node's category — so
BOTH sides of `node_eq` were the identical interned string `"CONTROL-INVITE"`. `node_eq` of two
identical strings reads true. The claim passed, but it never compared a NodeID — a green band that
was really a `str_eq` in a NodeID-shaped disguise.

On 2026-08-14, `bmf-core.fk` gained its own `bmf-bp` (used by `build-emit`, the actual template
constructor) as a fix for a DIFFERENT bug — the natural-language pivot vocabulary silently colliding
at NodeID `{0,0,0,0}` on every miss (see `bmf-core.fk`'s own comment at that table, and
`form/form-stdlib/tests/natural-language-band.fk`). That fix made `bmf-bp` answer an unregistered
name with a real, non-string `(make_nodeid 0 0 0 0)` instead of the raw string. Correct for that bug,
but it broke CONTROL-INVITE's accidental string coincidence: the emitted node's category became a
real (if null) NodeID, the oracle side stayed a string, `node_eq` correctly read false, and the band
dropped from 1023 to 959 — the regression `release-ledger.bml`'s R27 row records.

## The fix (scoped to CONTROL-INVITE only)

1. **`form/form-stdlib/blueprint-registry.json`** — added the canonical row: `CONTROL-INVITE`,
   `pkg=1 level=2 type=99 inst=1950`. This is new-shape registration, not a mirror mint — the
   registry's own header names exactly this path ("To add a shape: add a row here, then route usage
   through the reviewed bootstrap set..."), and `control-invite-grammar.fk`'s own header already
   calls CONTROL-INVITE "the ONE blueprint this grammar mints." Placed beside the `OAC-*` family
   (`1946`-`1949`) it sits downstream of — `control/offer-ack-core.fk` and
   `control/choice-lane-core.fk` act on the same eight tokens this grammar recognizes
   (`receipts/2026-07-01-choice-lane-control-invites.md`). Verified no coordinate or name collision
   (script check below); registry count 442 → 443.
2. **`form/form-stdlib/form-ontology-bp.fk`** — mirrored the SAME coordinate into both
   `FOL-BP-BOOTSTRAP-TABLE` (the row list) and `fol-bp-coords` (the generated decision chain `bp`/
   `fol-bp` actually resolve through), following the file's own documented UUID/VALUE-NONE precedent.
   Purely additive — every existing row/branch is untouched, confirmed by diff.
3. **`form/form-stdlib/bmf-core.fk`** — mirrored the SAME coordinate into `bmf-bp-table`, the local
   table `build-emit` resolves templates through. Also purely additive.
4. **`grammars/tests/control-invite-grammar-band.fk`** — the c7 claim now compares against
   `(fol-bp (cig-op))`, not `(bp (cig-op))`. `fol-bp` is form-ontology-bp.fk's documented "second
   door whose name no primitive shadows" — the only way to get a real NodeID out of this registry on
   fkwu. Added `form/form-stdlib/form-ontology-bp.fk` to the band's own prelude line (it wasn't
   preluded before); `form-ontology-bp.fk` is the arm-neutral split that does NOT carry the
   `walk_recipe_here` seam (`form-ontology-loader.fk` does), so this prelude addition is safe and
   free of the unresolved-call floor documented at the top of that file.

Both resolution paths (`bmf-bp` at emission, `fol-bp` at the oracle) now answer `CONTROL-INVITE`
with the identical registered coordinate `1/2/99/1950` — the claim is now a real NodeID-to-NodeID
identity check, not a string coincidence.

## Evidence

Build: `rm -f fkwu && cc -O2 -o fkwu runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m form/native/mlx/fk-mlx-carrier.c -framework Metal -framework Foundation -fobjc-arc -I/opt/homebrew/include -L/opt/homebrew/lib -lmlxc -Wl,-rpath,/opt/homebrew/lib` (silent success).

```
./fkwu bootstrap/ground.fk                                        -> 42   exit 0
./fkwu form/form-stdlib/tests/binary-freshness-band.fk </dev/null -> 31   exit 0
./fkwu gate/structural-gate-run.fk                                -> 1    exit 0  (after the printed histogram line)
```

Registry integrity (python3, load + scan): 443 blueprints, zero `(pkg,level,type,inst)` collisions,
zero name collisions, `CONTROL-INVITE` present exactly once at `1/2/99/1950`.

Preflight (`observe/preflight-run.fk` against `/tmp/preflight-target`) clean — 0 errors, 0 warnings,
0 unresolved — on: `grammars/tests/control-invite-grammar-band.fk`,
`form/form-stdlib/tests/bml-band.fk`, `form/form-stdlib/tests/bml-generics-band.fk`.

Bands, fresh (`.fkb`/`.sym` removed repo-wide first):

| band | before | after | exit |
|---|---|---|---|
| `grammars/tests/control-invite-grammar-band.fk` | 959 | **1023** | 0 |
| `form/form-stdlib/tests/bml-band.fk` | 37752852 | 37752852 (unchanged) | 0 |
| `form/form-stdlib/tests/bml-generics-band.fk` | 306356 | 306356 (unchanged) | 0 |
| `form/form-stdlib/tests/natural-language-band.fk` | (not re-baselined; ran once post-fix) | 262143 — its own documented FULL verdict | 0 |
| `form/form-stdlib/tests/bmf-core-band.fk` | (not re-baselined) | 700 | 0 |
| `form/form-stdlib/tests/bmf-grammar-band.fk` | (not re-baselined) | 2047 | 0 |

`bml-band`/`bml-generics-band` were the two bands the task named as R2's prior regression signal
("gained 5/3 red bits"). Both are bit-for-bit identical to their pre-fix baseline here. That is
expected, not coincidental: every edit to `blueprint-registry.json`, `form-ontology-bp.fk`, and
`bmf-core.fk` is a pure *addition* of one new named row/branch (confirmed by `git diff` — no existing
line in any of the three files was changed or removed, and the `fol-bp-coords` paren count grew by
exactly one to match the one new `if` branch). An addition-only change cannot alter resolution for
any name other than `CONTROL-INVITE`, so nothing that does not reference `CONTROL-INVITE` can regress
from this pass — and grepping the whole repo for `CONTROL-INVITE` turns up exactly four files, all
already accounted for above (`grammars/control-invite-grammar.fk`, its band, this receipt, and the
`release-ledger.bml` row this closes).

## Why R2's attempt likely broke bml/bml-generics (inferred, not witnessed directly)

This pass did not have R2's actual diff to inspect — that attempt lives in a different session. But
`form/form-stdlib/tests/bml-band.fk` and `bml-generics-band.fk`'s own `cat?` helper
(`(node_eq (node_category node) (bp name))`, line 15 of each) makes the likely shape of R2's mistake
visible: `bml.fk` emits many universal ops — `UE-CLASS`, `UE-INTERFACE`, `UE-WRITE-ASSIGN`,
`UE-LOOP`, `UE-SWITCH`, `UE-CASE`, `UE-TRY`, `UE-CATCH`, `UE-THROW`, `UE-JUMP-*` — that are **not**
in `bmf-core.fk`'s local `bmf-bp-table` (only `do`/`let`/`if-then`/`fncall`/`fndef`/`ident`/the
arithmetic and comparison ops/three `UE-*` leaves/the six NL-pivot names were there before this
pass). If a fix routed `build-emit` through the fuller `fol-bp-coords` instead of the local
`bmf-bp-table` — rather than adding one new row to the local table, as this pass did — every one of
those names would start resolving to real, *different* coordinates than before, changing what
`bml-band`/`bml-generics-band` see for dozens of pre-existing checks at once, not just one. That is a
plausible way to "gain 5/3 red bits" from a single change. This pass deliberately avoided touching
the resolution *mechanism* — only new *data* (one row in three tables, one coordinate) was added, and
the two bml bands' unchanged verdicts confirm that scoping held.

## Files touched

- `form/form-stdlib/blueprint-registry.json` — new row, `CONTROL-INVITE` at `1/2/99/1950`
- `form/form-stdlib/form-ontology-bp.fk` — mirrored into `FOL-BP-BOOTSTRAP-TABLE` and `fol-bp-coords`
- `form/form-stdlib/bmf-core.fk` — mirrored into `bmf-bp-table`
- `grammars/tests/control-invite-grammar-band.fk` — c7 claim now uses `fol-bp`; prelude line gained
  `form/form-stdlib/form-ontology-bp.fk`
- this receipt
