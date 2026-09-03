# Receipt — R40: the whole family bml.fk emits, given real coordinates in bmf-core.fk (2026-09-03)

## The gap, precisely

`form/form-stdlib/bml.fk` — the BML compiler — is the ONE emitter that drives every `t-emit "<op>"`
through `build-emit`/`g-build`, which both resolve every op through `(bmf-bp op)`
(`form/form-stdlib/bmf-core.fk`). Grepping every `t-emit "..."` call in `bml.fk` (72 call sites, 23
distinct op names) against `bmf-bp-table` — the LOCAL, hand-held mirror `build-emit`/`g-build` actually
resolve through — found thirteen names with no row there: `UE-CLASS`, `UE-INTERFACE`, `UE-LOOP`,
`UE-SWITCH`, `UE-CASE`, `UE-TRY`, `UE-CATCH`, `UE-THROW`, `UE-JUMP-RETURN`, `UE-JUMP-BREAK`,
`UE-JUMP-CONTINUE`, `UE-WRITE-ASSIGN`, and `"if"` (BML's `if c then t else e` keyword-form ternary,
distinct from the `if-then` statement and the `?:` `UE-COND-TERNARY` operator). Every one of these
fell to the same zero-fallback `(make_nodeid 0 0 0 0)` documented at that table for the six NL-pivot
rows and for R27's `CONTROL-INVITE` — silently miscategorizing every `UE-CLASS`/`UE-LOOP`/etc. node the
BML compiler ever built.

The R40 ledger row (release-ledger.bml) assumed this would need R27's exact treatment: mint fresh
coordinates in `blueprint-registry.json`, then mirror into `form-ontology-bp.fk` and `bmf-core.fk`.
Cross-referencing all three files (not just `bmf-core.fk`) found that assumption only half true:

- **Twelve of the thirteen** (`UE-CLASS`, `UE-INTERFACE`, `UE-LOOP`, `UE-SWITCH`, `UE-CASE`, `UE-TRY`,
  `UE-CATCH`, `UE-THROW`, `UE-JUMP-RETURN`, `UE-JUMP-BREAK`, `UE-JUMP-CONTINUE`, `UE-WRITE-ASSIGN`)
  already had real coordinates in **both** `blueprint-registry.json` (as canonical name or, for the
  four jump/assign ops, as an alias of an older name — `UE-WRITE-ASSIGN` alongside `F-ASN`,
  `UE-JUMP-RETURN` alongside `F-RET`, `UE-JUMP-BREAK`/`UE-JUMP-CONTINUE` alongside `PY-BRK`/`PY-CONT`)
  and `form-ontology-bp.fk`'s `FOL-BP-BOOTSTRAP-TABLE`/`fol-bp-coords` (explicit rows under the exact
  `UE-*` name bml.fk emits, at the same coordinate as the canonical alias target) — registered earlier
  alongside the seedbank `universal-emit.fk` work for Go/Python/Rust/TypeScript. Only `bmf-core.fk`'s
  local table had never been brought current.
- `"if"` was the one genuine registry gap: present in `form-ontology-bp.fk` (`1/2/11/2`, beside
  `if-then`=`1/2/11/1` and `UE-COND-TERNARY`=`1/2/11/3` — same type-11 conditional family) but never
  mirrored into `blueprint-registry.json` at all, and (like the twelve above) absent from
  `bmf-core.fk`.

So the real shape of this pass is narrower than the ledger row anticipated: one new canonical row
(`if`), zero new `form-ontology-bp.fk` rows (all thirteen were already there), thirteen new
`bmf-core.fk` rows.

## The fix

1. **`form/form-stdlib/blueprint-registry.json`** — added ONE new row: `"if"` at `pkg=1 level=2
   type=11 inst=2`. Not a fresh mint — `form-ontology-bp.fk` already carried this exact coordinate;
   this row formalizes it into the canonical source it should always have come from. Registry count
   443 → 444. Verified zero `(pkg,level,type,inst)` collisions and zero name collisions across all 444
   rows (python3 load + scan); `git diff` shows 13 insertions, 0 deletions in this file.
2. **`form/form-stdlib/form-ontology-bp.fk`** — **no edit.** Confirmed by direct grep of both
   `FOL-BP-BOOTSTRAP-TABLE` and `fol-bp-coords` that all thirteen names already resolve there, at the
   coordinates listed above. `git diff` on this file is empty.
3. **`form/form-stdlib/bmf-core.fk`** — added thirteen rows to `bmf-bp-table`, coordinates copied
   unchanged from `form-ontology-bp.fk` (never minted here — same discipline the file's own header
   documents for the six NL-pivot rows and `CONTROL-INVITE`). `git diff` shows only insertions; the
   sole `-` line is the pre-existing `CONTROL-INVITE` row's trailing `))` moving because it is no
   longer the last row in the list — the row's own content (`(bmf-bp-row "CONTROL-INVITE" 1 2 99
   1950)`) is byte-identical before and after, same structural artifact R27's own receipt describes
   for `fol-bp-coords`'s paren count.
4. **`form/form-stdlib/tests/bml-band.fk`** and **`form/form-stdlib/tests/bml-generics-band.fk`** —
   both bands' `cat?` oracle read `(node_eq (node_category node) (bp name))` — `bp` being fkwu's
   pass-through native (tag 45), the exact same trap R27 diagnosed and fixed for
   `control-invite-grammar-band.fk`. On these two bands the effect was total: EVERY `cat?` check in
   both files compares a real NodeID (built via `bmf-bp` at parse time) against a raw string (`bp`'s
   pass-through) and reads false unconditionally, registration gap or not — confirmed by decomposing
   the pre-fix verdicts bit-by-bit: the only bits ever set were the `node-name?` checks (string-vs-
   string, unaffected by this bug); literally every `cat?` bit, for both unregistered ops (`UE-CLASS`,
   `UE-WRITE-ASSIGN`, `UE-JUMP-RETURN`) and already-registered ones (`do`, `fndef`, `if-then`,
   `fncall`, `ident`), read zero. Both preludes already carried `form-ontology-bp.fk`, so the fix is
   the same one-line swap R27 made: `(bp name)` → `(fol-bp name)`. No prelude edit needed.
5. **`form/form-stdlib/tests/bmf-bp-r40-family-band.fk`** (new) — a standalone proof band, independent
   of the two repaired bands above, with two parts per op: (a) direct resolver agreement —
   `(bmf-bp op)` equals `(fol-bp op)` AND neither is the null `(0,0,0,0)` fallback (agreement that
   isn't a shared miss); (b) end-to-end — real BML source for each construct
   (`class C { }`, `interface I { }`, `for(...)...`, an assignment, a `switch`/`case`, a `try`/`catch`,
   `throw`, `return`, `break`, `continue`, and an `if...then...else` expression), parsed through the
   actual `bml-grammar` via `g-parse-full`, produces a node whose `node_category` is the same real
   coordinate. 26 checks total, one bit each.

## Why bml-band / bml-generics-band were "already passing around the gap some other way" — they weren't

R27's own receipt speculated these two bands were "the two bands R2's prior regression signal" named
("gained 5/3 red bits"), implicating this exact op family. Re-running them fresh (pre-fix) found
something more severe than a partial regression: **every single `cat?` bit in both bands read zero**,
registered ops and unregistered ops alike — `bml-band.fk` scored `37752852`
(only the five `node-name?` bits: `a-f`, `a-x`, `b-g`, `c-C`, `c-m`), `bml-generics-band.fk` scored
`306356`. Both are bit-for-bit identical to R27's own recorded baseline for these two files, confirming
this is not something my registry pass could have caused or R27's CONTROL-INVITE pass regressed — it
predates both. The honest answer to "did they gain new green bits from having real coordinates" is:
**not from the registry fix alone** — registering all thirteen ops and re-running fresh (before
touching either band file) left both verdicts bit-for-bit unchanged at `37752852`/`306356`. They only
came alive once the SEPARATE `cat?`/`bp` mechanism bug was fixed (item 4 above), at which point BOTH
climbed to their full documented totals in one step — `268435455` (`bml-band.fk`) and `16777215`
(`bml-generics-band.fk`).

## Evidence

Build: `rm -f fkwu && cc -O2 -o fkwu runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m
form/native/mlx/fk-mlx-carrier.c -framework Metal -framework Foundation -fobjc-arc
-I/opt/homebrew/include -L/opt/homebrew/lib -lmlxc -Wl,-rpath,/opt/homebrew/lib` (silent success).

```
./fkwu bootstrap/ground.fk                                        -> 42   exit 0
./fkwu form/form-stdlib/tests/binary-freshness-band.fk </dev/null -> 31   exit 0
./fkwu gate/structural-gate-run.fk                                -> 1    exit 0  (after the printed histogram line)
```

Registry integrity (python3, load + scan): 444 blueprints, zero `(pkg,level,type,inst)` collisions,
zero name collisions, `if` present exactly once at `1/2/11/2`.

Preflight (`observe/preflight-run.fk`) clean — 0 errors, 0 warnings, 0 unresolved — on:
`form/form-stdlib/tests/bmf-bp-r40-family-band.fk`, `form/form-stdlib/tests/bml-band.fk`,
`form/form-stdlib/tests/bml-generics-band.fk`.

Bands, fresh (`.fkb`/`.sym` removed repo-wide before every run):

| band | before | after registry fix (bands untouched) | after `cat?` fix | exit |
|---|---|---|---|---|
| `form/form-stdlib/tests/bml-band.fk` | 37752852 | 37752852 (unchanged) | **268435455** (full) | 0 |
| `form/form-stdlib/tests/bml-generics-band.fk` | 306356 | 306356 (unchanged) | **16777215** (full) | 0 |
| `form/form-stdlib/tests/bmf-core-band.fk` | 700 | 700 (unchanged) | — | 0 |
| `form/form-stdlib/tests/bmf-grammar-band.fk` | 2047 | 2047 (unchanged) | — | 0 |
| `form/form-stdlib/tests/natural-language-band.fk` | 262143 | 262143 (unchanged) | — | 0 |
| `grammars/tests/control-invite-grammar-band.fk` | 1023 | 1023 (unchanged) | — | 0 |
| `form/form-stdlib/tests/bmf-bp-r40-family-band.fk` (new) | — | — | **67108863** (full, 26/26 bits) | 0 |

The four bands not touched in this pass (`bmf-core-band`, `bmf-grammar-band`, `natural-language-band`,
`control-invite-grammar-band`) stayed bit-for-bit identical across every run in this pass — expected,
since every edit to the three registry files is a pure *addition* of new named rows (confirmed by
`git diff`: 13 insertions/0 deletions in `blueprint-registry.json`, 38 insertions/1 deletion — the
paren-relocation artifact above — in `bmf-core.fk`, 0/0 in `form-ontology-bp.fk`), and none of those
four bands reference any of the thirteen newly-registered names.

## Found but not touched

- **`if-then` is also unregistered in `blueprint-registry.json`.** Searched the full registry for
  `"if-then"` as a name or alias: zero hits — the same gap `"if"` had, and it sits in the same type-11
  family (`inst=1`). Unlike `"if"`, this is not part of the family causing a live bug: `if-then`
  already resolves correctly through both `bmf-core.fk` (pre-existing row, `1/2/11/1`) and
  `form-ontology-bp.fk` — the two mirrors that matter for resolution already agree, so nothing is
  broken. Left unregistered rather than folded into this additive pass, since it is not one of the
  names `bml.fk` emits with no working coordinate; a future pass can close it on its own terms.
- **`defined_in` is stale on several pre-existing registry rows this pass exercises.** `UE-CLASS`,
  `UE-INTERFACE`, `UE-LOOP`, `UE-SWITCH`, `UE-CASE`, `UE-TRY`, `UE-CATCH`, `UE-THROW` all carry
  `"defined_in": []` in `blueprint-registry.json`, even though `form/form-stdlib/bml.fk` is now a
  confirmed, live emitter of every one of them. Not touched — editing an existing row's array value is
  an edit to an existing line, which this pass's additive-only discipline forbids; named here instead.
- **The header comment in `bml.fk` documents `-e → UE-MATH-NEGATE`, but the `negx` rule (line 109)
  actually emits `"sub" (0 - e)`.** `UE-MATH-NEGATE` is registered (`1/2/12/6`, already in
  `bmf-bp-table`) but nothing in `bml.fk` ever emits it — a doc/implementation mismatch, not a
  coordinate gap. Not touched; out of scope for a registration pass.
- **A repo-wide grep for the general `(node_eq (node_category node) (bp name))` idiom** (the exact
  broken oracle shape) found no remaining instances beyond the three bands now fixed
  (`control-invite-grammar-band.fk` by R27, `bml-band.fk`/`bml-generics-band.fk` by this pass). A
  broader grep for any `(bp "...")` call turns up ~120 files repo-wide; the large majority use `bp` for
  its documented pass-through purpose elsewhere (not as a NodeID-shaped category oracle) and were not
  individually audited — that is outside this pass's scope.

## Files touched

- `form/form-stdlib/blueprint-registry.json` — new row, `if` at `1/2/11/2`
- `form/form-stdlib/bmf-core.fk` — thirteen new rows mirrored into `bmf-bp-table`
- `form/form-stdlib/tests/bml-band.fk` — `cat?` now uses `fol-bp`, not the native `bp` pass-through
- `form/form-stdlib/tests/bml-generics-band.fk` — same one-line `cat?` fix
- `form/form-stdlib/tests/bmf-bp-r40-family-band.fk` — new proof band (26 checks, direct resolver
  agreement + end-to-end parse, all thirteen registered ops)
- this receipt

`form/form-stdlib/form-ontology-bp.fk` was read and checked but not edited — every name this pass
needed was already there.
