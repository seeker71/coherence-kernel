# 2026-08-21 — BML is classes, not infix

Yes asked how the last lift increased high-grammar BML. It did not.
`form.lift` is a cursor dialect: infix, `allow`, `unless`. Named
constants, abstract reusable patterns, templates, generics, class
methods, Form primitives — that is BML. The census that counted
`section [form.lift]` as high BML was the costume. This sitting
names that, then builds the thing that was asked for.

## What BML actually is here

Grounded in this body and in Coherence-Network:

| Shape | Where it already lives |
| --- | --- |
| `package` / `interface` / `class` / `const` / `[get]` | `form/form-stdlib/bml/kernel-core.bml` |
| `abstract` reusable Path / filesystem | `form/form-stdlib/bml/form-fs.bml` |
| `template<THandle, TValue> interface` | `form/form-stdlib/bml/host-kernel-interface.bml` |
| generic class `FileIO<T>` that *runs* | `form/form-stdlib/file-io.bml` |
| `template RouteCell<TReq, TRes>` + class specializations | `form/apps/coherence-network/api.bml` |
| `class Cut : Primitive` / `class Rule` / named `const` | Coherence-Network thesis samples (`Cut.bml`, `Rule.bml`, `BMF.bml`) |

`grammars/form-bml.fk` is only `def` / `let` / `if` / call. The class
and template door is the line compiler in `source-compiler.fk`:
`class Name<T> { field …; def m() = … }` hoists methods to bare
fndefs and mints an abstraction descriptor. That is the FileIO door.

## What this sitting built

Two files, one organ:

- **Authority (high grammar):**
  `form/form-stdlib/bml/form-cli-flow.bml`
  — `package Form.cli.flow`, enums, `interface IChoiceWalk` /
  `IIngest`, `template BackendCell<TName>`, `template<THit,TReady>
  interface IWalkCell`, `class WalkCut : Primitive`,
  `class FormCliRouter` with named weight constants,
  `class KnowledgeIngest` with Ice / Liquid / Compost.
- **Executable (FileIO door):**
  `form/form-stdlib/form-cli-flow.bml`
  — `class FormCliFlow<T>` with `field Ice = 2` and the rest as
  named constants, `ref` to `FormCliFlowAuthority`, `thought
  fcf-pattern`, reusable `fcf-choice` / `fcf-cut` / `fcf-restore`,
  plus `template WalkCell` / `BackendCell` / `IngestCell`.

Crystallized through `form-source-compile-file` to
`form-cli-flow-xtal.fk`. The xtal carries `defn Ice () 2`,
`defn FormCliFlow () (list "abstraction" "FormCliFlow" (list "T")
…)`, and the three `language-template-with-members` lets.

## Observed

```
./fkwu form/form-stdlib/form-cli-flow-run.fk
# -> 255

./fkwu form/form-stdlib/tests/form-cli-flow-bml-band.fk
# -> 1023
```

| bit | claim |
| --- | --- |
| 1 | named grains Ice / Liquid / Compost |
| 2 | choice-walk native / local / remote |
| 4 | ingest uses those grains |
| 8 | cut: capability 0 scores 0 |
| 16 | route: learned native wins on named weights |
| 32 | route: blind native yields to agent |
| 64 | flywheel flips agent -> native |
| 128 | weak local still loses |
| 256 | class descriptor FormCliFlow, generic T, field Ice, method fcf-walk |
| 512 | authority source carries package / interface / class / const / template |

Preflight of the band: parens balanced, errors 0, unresolved 0,
chain clean.

Walk_recipe_here / write_form_binary / file_byte_at still warn on
the compile driver (the known text-lens seam). The xtal still
wrote. Same floor as the earlier lift compiles; named, not dressed
as a pass of those three ops.

## What this does not claim

- 95% of s-expr is not converted. One flow organ is in real BML.
  The 88 "high" count from yesterday still includes form.lift
  twins. That number is a form.lift count, not a BML count.
- High-grammar `package` / `interface` / `abstract` bodies do not
  yet execute as methods on fkwu. They are the authority picture,
  the same split as `form-fs.bml` vs `form-fs.fk`. The running
  methods are the generic class fields and defs.
- Callers of `form-cli-router.fk` still prelude the s-expr spine.
  Authority to *edit* the flow pattern is now the BML class.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-21 -> run 255, band 1023, preflight clean
