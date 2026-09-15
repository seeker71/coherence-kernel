# Every name finds its home or says so

2026-09-15, Hati Suci, worktree agent-a478330dea803ecce. Five north-star families in
`form/form-stdlib/grammars/bml.fk`: package-namespace-resolution, import-symbol-resolution,
unresolved-names, general-calls, broader-multi-argument-methods.

## Carried

- **Source.** The general statement driver the locals sibling landed this morning
  (`bml-source-parse-general-step`) now also reads `package A.B;`, `import A.B;` and
  `import A.B.*;`. Its one expression grammar reads qualified names and calls whose arguments are
  expressions (`bml-source-parse-name-primary`), so returns, consts, locals and array items all take
  nested, qualified and zero-argument calls. Its method step takes any parameter count through
  `bml-source-parse-parameters`, and a list whose parameters do not all parse makes no method. Two
  drivers written in parallel became one on rebase.
- **Scope.** Each unit's package and imports become bnii rows (`bml-scope-unit-rows`), and
  `bml-scope-link` resolves names through them. Every unit the imports reach links into one Hati
  table: main is function 0, its own methods follow, then the linked units' methods. Each unit's
  environment holds its package key at the base, imported symbols over that, and its own methods
  on top, so a local method shadows an on-demand import. `bml-local-lookup` strips the unit's own
  package prefix. `bml-source-declaration-model` carries the same rows (`bml-model-scope`). bml.fk
  loads the carrier; the carrier loads only core.fk.
- **No silent zero.** `bml-hati-lower-name-in` and `bml-hati-lower-call-in` lower a name they cannot
  resolve to a `BML-HATI-UNSUPPORTED <reason>` string literal. `bml-hati-image` reads those reasons
  out of its own pool, so the image stays unsupported even when no pass listed them. New reasons:
  `import/unresolved`, `package/duplicate`, `package/outside-unit`, `import/outside-unit`,
  `call/receiver-dispatch` (a receiver that is a value waits for the class-dispatch wave).
- **Both lanes.** BMA inlines a callee's final return as the call's value
  (`bml-compile-callee-body-in`), so nested calls agree across BMA and Hati.

## Witnessed on fkwu

Preflight exits 0 on each new proof, on bml.fk and on the carrier.

| proof | verdict |
| --- | --- |
| `tests/bml-hati-native-package-namespace-proof.fk` | 31 of 31 |
| `tests/bml-hati-native-import-symbol-proof.fk` | 36 of 36 |
| `tests/bml-hati-native-unresolved-names-proof.fk` | 20 of 20 |
| `tests/bml-hati-native-general-calls-proof.fk` | 22 of 22 |
| `tests/bml-hati-native-many-arg-methods-proof.fk` | 23 of 23 |

The nine earlier hati proofs read what they read before: class-dispatch 17, class-field 21,
class-template-overload 23, compiler 39, floor 26, locals 20, method-calls 19, multi-arg-calls 20,
overload-resolution 19. The locals sibling's three read full marks on the merged tree:
mutable-locals 34, arrays 28, restore 17. `bml-native-interface-package-import-band` 255, with the carrier's next
code point now `bml-scope-link -> native interface dispatch lowering`. Every other bml.fk consumer
I ran reads its earlier verdict (compiler-runtime 131012, source-picture 42, lowering-carrier 41,
concrete-lowerer 31 and 34, bootstrap-ratchet 26, compiler-fkb-image 29, nineteen thesis proofs).
`kernel-core-self-proof` exits 1 before and after (unresolved `native_blueprint`, `walk_recipe`); six
thesis rule proofs stop before and after on `method_define` with a capturing closure.

## For the lead

The north-star form can move package-namespace-resolution, import-symbol-resolution,
unresolved-names, general-calls and broader-multi-argument-methods from current-unsupported to
current-supported, and its interface-package-import-metadata next code point can read the
carrier's new value. No corpus row added.

## Still open

- BMA resolves names in the caller's scope. An imported method that calls its own unit's helpers
  runs on Hati only; the transitive-import check runs there alone. Arity overloads stay Hati-only.
- Imports export methods. A unit's consts stay inside the unit.
- A property word (`native`, `public`, ...) is not a name segment, so `package A.native;` does not
  parse. Java keeps `native` out of package names as well.

## Closing

Most surprising: an AST node answers `nil?` with 1 on fkwu. My first expression parser marked "no
expression" with an empty node, so every name it parsed read as absent, and the first run scored 16
of 31. Success now travels as a flag beside the node.

Discomfort to gold: the unsupported pass already named `name/unbound`, and the zero still sat in
the lowering behind it. Now the lowering writes the reason into the table's own pool, so skipping
the pass cannot hand back a valid zero. Walked directly, the table answers with the reason.

Frontier word, 0 hits in the tree: **nilghost**, a present value that a probe reads as nothing
because it asked a question of the wrong kind. Its question: where else does the body test presence
with `nil?` on something that is not a list, and what does it fail to see there?

— Claude (Opus 5), as Sema, worktree agent-a478330dea803ecce
