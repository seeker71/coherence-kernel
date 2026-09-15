# One scope rule on both lanes

2026-09-15, Hati Suci, worktree agent-a478330dea803ecce. Urs asked today that every known gap be
closed rather than written down and worked around. This closes four of the leftovers in the
108e00b6a receipt: BMA name resolution, consts across imports, property words in paths, and the six
thesis rule proofs. `native_blueprint` and `walk_recipe` land on their own.

## Carried

- **BMA resolves a call as Hati does.** The name goes through package and import scope and the
  overload is picked by arity. The callee's body compiles in the callee's own unit env, with its
  parameters bound to arguments read in the caller's env (`bml-compile-call-in`,
  `bml-bma-home-env`). The BMA env carries the link, so the callee's unit env is rebuilt at the
  call. An unresolved name or call compiles to the same `BML-HATI-UNSUPPORTED <reason>` literal the
  Hati lowering writes, and `bml-bma-run-unit-value` answers those reasons before any op runs, as a
  Hati image does. Inside an inlined callee, const, local and assign statements only bind: BMA reads
  them by substitution, and a pushed value would sit under the call's result. `bml-compile-program`
  is the unit program now.
- **Consts are part of a unit's scope.** A unit's top-level consts become symbols that carry their
  value and the env that value reads, gathered importee first. The unit's own methods read them on
  both lanes; importers reach them qualified, through `import P.*;`, or as one symbol.
- **Any word is a path segment.** After a dot any word reads as a name segment, and a package path,
  an import path or a qualified reference may begin with any word.
- **Closure methods dispatch on fkwu.** The method table keeps the function value, and
  `method_invoke` delivers a closure's captures the way an indirect call does
  (`runtime/fkwu-uni.c`, tags 197 and 199). The branch that died on a capturing method is gone.
- **The final return is the one no guard holds.** With dispatch healed, the final-return proof
  showed `bml-body-source-exec-return` answering the first `return` it met, the guarded
  `return false;` in `operator==`. It now skips `if (...)` guards and answers
  `return m_hItem == hOther.Item;`.

## Found on the way

- BML-AST categories are trivial ints, so a literal leaf answers `node_category` with itself: 13
  reads as NAME and 35 as CALL. `bml-bnml-reads-mutable?` looked the literal 13 up as a name and
  died on `str_eq`. `bml-name-node?` and `bml-call-named?` now check the node's shape as well.
- The top-level `(do` of `bml.fk` closed early, at `bml-compile-call-in`: one paren too many there
  (already at c330d1950) left every form after it loose at top level. Rewriting that defn removed
  the extra paren, and the file now ends `0)`.
- An empty string answers `nil?` with 1 as well, so a proof that expects "" compares with `str_eq`.

## Witnessed on fkwu

Preflight exits 0 on `bml.fk`, the three new proofs and the six thesis proofs.

| proof (`form/form-stdlib/tests/`) | verdict |
| --- | --- |
| `bml-hati-native-lane-parity-proof.fk` | 14 of 14 |
| `bml-hati-native-import-consts-proof.fk` | 13 of 13 |
| `bml-hati-native-path-words-proof.fk` | 11 of 11 |
| `bml-thesis-rule-hashcode-return-proof.fk` | 16383, full (was rc 1) |
| `bml-thesis-rule-method-dispatch-proof.fk` | 1023, full (was rc 1) |
| `bml-thesis-rule-operator-chain-member-compare-proof.fk` | 4095, full (was rc 1) |
| `bml-thesis-rule-operator-final-return-proof.fk` | 16383, full (was rc 1) |
| `bml-thesis-rule-operator-if-instanceof-proof.fk` | 4095, full (was rc 1) |
| `bml-thesis-rule-operator-member-compare-proof.fk` | 4095, full (was rc 1) |

On the tree rebased over the locals and siblings landings: package-namespace 31, import-symbol 36,
unresolved-names 20, general-calls 22, many-arg-methods 23; mutable-locals 34, arrays 28, restore 17,
element-assign 29; the nine earlier hati proofs; method-band 116, bml-action-runtime 8388680,
bmf-choice-receipt 67108863, let-scope 511, ontology-emit 31, and every other thesis and consumer
band I ran at its earlier verdict.

## Closing

Most surprising: a number can be a name. Categories and literals share one space of trivial ints
here, so `13` answered "NAME" to anything that asked by category alone. Only the node's shape tells
them apart.

Discomfort to gold: the paren. For a stretch I was sure my edit had left `bml.fk` unbalanced. Then
the depth profile of HEAD showed the unit's `(do` closing about 1,100 lines early, with every form
after it loose at top level. What looked like my wound was an older one, and closing the file at its
`0)` made the unit whole.

Frontier word, 0 hits in the tree: **leafcast**, a leaf value that answers a category test with
itself because leaves and categories share one space. Its question: where else does a check by
category alone accept a literal that only equals the category's id?

— Claude (Opus 5), as Sema, worktree agent-a478330dea803ecce
