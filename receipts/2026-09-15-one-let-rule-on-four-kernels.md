# One let rule on four kernels, and what stood behind the Hati proofs

2026-09-15, afternoon, this Mac, Hati Suci. The lead's ask: the proof siblings adopt the let rule
fkwu landed as c330d1950, prove it four-way, and read the BML Hati proofs again afterwards.

## Carried

- **A do is a lexical scope on Go, Rust and TS.** Its lets and defns bind for the rest of that do,
  in a child frame opened at the first binding form, so a do that binds nothing costs no frame. A
  let met anywhere else (an if arm, an argument, a do's last form) answers its value and binds no
  name. Rust `RB_BLOCK` arm with `block_kind` / `bind_let` (form-kernel-rust/src/main.rs), Go
  `walkInner` block arm with `blockKind` / `bindLet` (form-kernel-go/main.go), TS `walkBlock` with
  `bindLet` and a per-recipe kind cache (form-kernel-ts/src/kernel.ts).
- **The unit's sequence reads flat through `walk_unit` / `walkUnit`,** as fkwu's `fk_parse_top`
  reads it: a column-0 let and a let in a column-0 do bind the unit's frame; a do before the first
  let or expression is a top-level do too (fkwu answers `(do (do (let x 1)) x)` with 1); a later
  do is lexical. The runners, walk_recipe, walk_recipe_here (a recipe's lets still land in the
  caller's frame), walk-cached, route startup, name-check loading and the trace doors enter
  through it.
- **The readers name the unit root.** The one `(do ...)` of a source and the implicit do around
  several column-0 forms are the same node once interned, yet fkwu reads a later nested do as
  lexical in the first and as a unit do in the second. So `read_root_from_source` /
  `readRootFromSource` / `readAll` mark which one they built; a root nobody named (a deserialized
  recipe, a combined program) keeps its top flat, as before.
- **The five TS backends scope the same way.** wasm, wgsl, cuda, metal and mlir kept one variable
  map per function; a nested do now restores it and an if-arm let binds nothing past itself. Each
  backend's tests carry a proof that passes on the new lowering and fails on the old one (exit 1,
  the last read naming the inner local).
- **The siblings read a node's category from its recipe row.** A composite sits at its category's
  level, so a node interned over `dialect-categories.fk`'s trivial-int BML-AST-* categories sits at
  level 1, and `category()` on Go, Rust and TS answered any level-1 NodeID as itself. Every BML AST
  node fell through `bml-hati-unsupported-node-in` (grammars/bml.fk) to "unknown".
- **The siblings keep a record blueprint verbatim.** native-recipe-record.bml's `nrr-new` builds a
  record whose blueprint is its owner record; fkwu keeps `record_new`'s operand as given, and the
  siblings took only a NodeID or 0 and stopped (`as_nid: <record ...>`). `record_new` now keeps a
  record operand beside the NodeID blueprint, `record_blueprint` answers it, the printer names it,
  and method dispatch stays on NodeIDs.
- **let-scope-band** (form/form-stdlib/tests/let-scope-band.fk), nine bits, joins the fourth-arm
  manifest as `let-scope fks 511`, and all eighteen bml-hati-native proofs join at fkwu's marks.

Three commits carry it: "The siblings read let as fkwu does", "The siblings read a node's category
from its recipe row", "The siblings keep a record blueprint verbatim".

## Witnessed

Probes, fkwu / Go / Rust / TS, after each change and after each rebase:

| probe | reads |
| --- | --- |
| p1 `(do (let K 7) (defn f () K) (f))` | 7 / 7 / 7 / 7 |
| p2 `(let K 7)` `(defn f () K)` `(f)` | 7 / 7 / 7 / 7 |
| p3 `(defn f () K)` `(let K 7)` `(f)` | 7 / 7 / 7 / 7 |
| p4 `(do (defn f () K) (let K 7) (f))` | 7 / 7 / 7 / 7 |
| p5 `(defn g () (do (let t 0) (do (let t 5)) t))` `(g)` | 0 / 0 / 0 / 0 (siblings read 5 before) |
| p6 `(do (let t 0) (do (let t 5)) t)` | 0 / 0 / 0 / 0 (siblings read 5 before) |
| p7 `(do (defn f () nope) (f))` | fkwu `[unbound-name] 'nope'` exit 1; Go, Rust, TS stop at the read, exit 1 |
| p8 `(do (let x 1) (print x) (let x 2) x)` | prints 1, answers 2, on all four |
| if arm `(do (let y 0) (if 1 (let y 9) 0) y)` | 0 / 0 / 0 / 0 (siblings read 9 before) |
| category `(node_eq (node_category (intern_node (intern_trivial_int 11) kids)) (intern_trivial_int 11))` | 1 / 1 / 1 / 1 (siblings read 0 before) |
| owner `(record_blueprint (record_new owner "packet" 5))` is `owner` | 1 / 1 / 1 / 1 (siblings stopped before) |

let-scope-band reads 511 on fkwu, Go, Rust and TS, and through ./validate.sh: ✓ 511, fourth arm
four-way, 1 ok, 0 divergent. tsc --noEmit clean. Backend tests: wasm 16 of 16, mlir 18 of 18,
cuda, metal and wgsl green. Old against new on a let-heavy loop, quiet: Go 0.16-0.17 s both, Rust
0.24-0.25 s both, TS 0.44-0.45 s on origin/main and 0.45-0.46 s here.

The Hati proofs did not move with the let change: Go = Rust = TS still read 16 on floor and 8 on
locals. Floor sums 26 checks and fkwu passes all; 16 is its fifteen text checks plus the one that
expects "unsupported", so every AST node was reading "unknown". That arithmetic led to
`category()`. Through ./validate.sh, each line "1 band(s) four-way, 1 ok, 0 divergent", at fkwu's
full marks: floor 26, locals 20, method-calls 19, multi-arg-calls 20, overload-resolution 19,
class-dispatch 17, class-field 21, compiler 39, class-template-overload 23, restore 17,
package-namespace 31, import-symbol 36, unresolved-names 20, general-calls 22, many-arg-methods 23,
after the record heal mutable-locals 34 and arrays 28, and origin's newest, element-assign 29, read
again with the rest after the last rebase.

Neighbours read the same through ./validate.sh after the changes: let-effect-once 111,
recursive-let-clobber 1, call-head-reading 255, vector-ops 511, rounding-ops 255, record-band 176,
record-blueprint-band 7, method-band 116, bml-native-mutable-locals-band 1023,
bml-native-interface-package-import-band, glass-sensor-body-scope 1023, both class-model proofs,
four-way; nested-defn-scope 63, source-runner-do-defn 127, binary-local-node-scope 607,
primitive-registry, the concrete-source lowerer and three thesis rule proofs, three-way (not in
the manifest). Freshness 31. Drift door `pass=16383 full=16383 refused=0` after each validate run.

## Still open, with the reason

- **string-join's TS leg does not finish.** It passed 48 GB resident after 7½ minutes and I stopped
  it; the TS kernel from origin/main e0e3be4e7, without these changes, passes 6.5 GB within 9 s on
  the same three inputs. Go and Rust answer 255, as fkwu does. The TS walker recurses where the
  others loop; that is its own movement.
- **A let answers 0 on fkwu when nothing follows it, its value on the siblings:** `(do (let x 5))`,
  `(defn f () (do (let x 1)))`, and an if-arm let's value. That is the let's value, not its scope;
  this movement did not choose between the two readings.
- **A closure over a name its own block rebinds later:** `(defn f (x) (do (defn g () x) (let x 2) (g)))`
  answers 1 on fkwu (capture by slot) and 2 on Go, Rust and TS, before and after these changes.

## Closing

Most surprising: interning folds the explicit top-level do and the implicit do around several forms
into one node, and fkwu still reads them differently. The reading lives in where the root came
from, not in its shape, so the reader has to say which one it built.

Discomfort to gold: the let change landed four-way and the Hati proofs did not move. The lead's
hypothesis had been mine as well, and the unmoved 16 felt like a fix that missed. Reading the
floor proof's own checks turned the 16 into arithmetic, fifteen text checks and one "unsupported",
and a four-line probe put the cause in `category()`. The same happened twice more: a crash stack
led to `record_new`, and a 48 GB TS leg I was ready to blame on my own frames read 6.5 GB on the
old kernel too. Each time the probe answered before the guess could.

Frontier word: **whencemark** (0 hits in the tree). Its question: when one content-addressed node
carries two readings depending on where it came from, does the reading belong in a mark the reader
keeps, or should the shape change so the node says it itself?

— Claude (Opus 5), as Sema, worktree agent-a4e17ec567397b8e7
