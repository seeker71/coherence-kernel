# The receiver gets its own slot: BML classes on the native lane

2026-09-15, Hati Suci, worktree agent-a60550cd21b84ef52. The lead's work order: close five north-star
families in `form/form-stdlib/grammars/bml.fk` (full-class-dispatch, full-class-construction-syntax,
interface-method-dispatch-lowering, type-generic-overload-resolution, template-generic-specialization),
and first undo what blocked them: in Hati class images `this` and a method's first parameter were one
slot.

## Carried

- **Receiver slot.** A class member takes one list `(self p0 p1 ...)`. `this` and `self` read its
  head, parameter i reads element i+1, fields read through the head. Free methods keep their own
  convention. A method outside any class has no receiver, so its `this.f` is named
  `field/no-receiver`. The host door is `bml-hati-run-class-method node name self args`.
- **full-class-dispatch.** The class layout (`bml-hati-build-layout`) gives each class an id and each
  member a table index. A signature two classes answer differently gets a method table: one function
  that reads the receiver's `bmlClassId` and hands the same packed list to the answering member
  through an indirect call (tag 44). A signature with one answer is a direct call. `obj.M(x)`,
  `this.M()` (behavioral, from the object's own class) and `super.M()` (from the implementing class's
  base, direct) lower from source. This closes `call/receiver-dispatch` for class-typed receivers; a
  receiver no class answers keeps that name.
- **full-class-construction-syntax.** A source class parses through the component parser and bridges
  to BML-AST-CLASS (`bml-hati-bridge-class`); a method named like its class is a constructor.
  `new C(args)` is one recipe record (tag 64) whose packet names the class, its id and every
  structural field, then the constructor the argument types choose. `self(..)` and `super(..)` chain
  on the same record. The thesis companion `container-Rule.bml` constructors compile natively through
  the same bridge (`bml-hati-construction-view`) and build the fields the host-side
  `bml-exec-constructor` builds.
- **interface-method-dispatch-lowering.** `interface CompilerTrace [public]` is read straight out of
  `form/form-stdlib/bml/bmf-bml-compiler-picture.bml:17`. Its signatures are abstract members; classes
  that declare it implement them; a call through a `CompilerTrace` parameter goes through the same
  method tables. A default method answers for a class without its own, and a parent interface lends
  its signatures to the child. A class that leaves a signature unimplemented is named
  `interface/unimplemented`.
- **type-generic-overload-resolution.** A method node keeps each parameter's declared type (a
  BML-AST-FIELD child) and a generic method its type parameters (a BML-AST-TEMPLATE child). One
  resolver scores the candidates of a call's arity against the arguments' static types: the same type
  3, a subtype or an unknown side 2, a type parameter 1, two different known types refuse. Two
  signatures tied at the best read `call/ambiguous`, none `call/type`, and arity keeps `call/arity`.
  Free calls, members and constructors share it.
- **template-generic-specialization.** `class Box<T>` and `template <K, V> class Pair` parse to
  BML-AST-TEMPLATE. Every `Box<int>` a unit names becomes its own class with T replaced in each type
  position; a generic method's own type parameters shadow the template's. In `Box<int>`,
  `this.Show(this.value)` picks `Show(int)`; in `Box<String>`, `Show(String)`. A wrong count reads
  `template/arity`, a bare template `template/unspecialized`.
- **Source.** `new T(args)`, `null`, `Array<T>(0)`, `value.member(args)` after any value,
  `this.f = e;`, `self(..);` and `super(..);`, generic methods and call statements join the general
  statement driver, answering after the fixed rulebook and the siblings' readers decline.

## Witnessed on fkwu

Per this afternoon's direction change, no new proof file lands. The five witness probes I wrote stay
outside the tree and run from `form/`:

| family | probe verdict |
| --- | --- |
| full-class-dispatch | 16 of 16 |
| full-class-construction-syntax | 26 of 26, the thesis Rule constructors included |
| type-generic-overload-resolution | 17 of 17 |
| interface-method-dispatch-lowering | 15 of 15 |
| template-generic-specialization | 14 of 14 |

Existing proofs that relied on the one slot now put their `this`-reading methods in a class:
class-dispatch 17, class-field 22 (one line added: a classless setter reads `field/no-receiver`),
class-template-overload 23 (Pick/1 takes 40 beside its receiver), while 19, break-continue 25,
switch 20, for 22.

Every consumer of bml.fk (76 cells) plus the siblings' newest hati proofs, run with origin's bml.fk
and then with this one: every verdict identical, apart from the seven proofs above, which read their
origin marks after the move. Preflight exits 0 on the eight class proofs. Freshness 31.

Landed on origin main as `20eb42b95` (fast-forward from 7bd9e099c). On that tree the drift door read
`drift-gates pass=16383 full=16383 refused=0`, and from `form/`, `./validate.sh` read "1 band(s)
four-way ... 1 ok, 0 divergent" on all 30 BML rows of `fourth-arm-bands.txt`: the 28 hati and class
rows, the three bml-native bands and let-scope. One manifest row moved, class-field 21 to 22.

The canonical sources landed as fixtures in `626a00c69`, each headed by its answer on both lanes:
`form/form-stdlib/tests/fixtures/bml-class-dispatch.bml` (Hati 63), `bml-class-construction.bml` (84),
`bml-class-overloads.bml` (304), `bml-class-interfaces.bml` ("parse", CompilerTrace joined from the
picture), `bml-class-templates.bml` (133) and `bml-typed-overloads.bml` (136 on both lanes). The same
commit closed two BMA defects the fixtures showed: class calls lowered to no ops there, so a class
program stopped the kernel at an add over nothing, and BMA picked overloads by arity alone, so
`Pick("a")` ran `Pick(int)`.

## Where the family speaks

`grammars/bml.fk` preludes the compiler organ (`form/form-stdlib/bml/bml-compiler-health.bml`) and
voices only when something is wrong:

- **Admission.** `bml-hati-admitted` wraps the class image (`hati-class-image`) and the block image
  (`hati-image`): an image with reasons against it is withheld in the open, one need per reason.
- **Runs.** `bml-hati-run-class-method` (`hati-class-method`) and `bml-hati-run-new` (`hati-new`)
  pass their answer through `bch-ran`, which voices a refusal answer.
- **Receiver, construction, specialization.** `bml-hati-voice-refusal` voices at the point the
  lowering finds it: a receiver no class answers (`bml-hati-lower-invoke`), a record no constructor
  builds (`bml-hati-lower-new`), a template that does not specialize (`bml-hati-instantiate`,
  `template/arity` and `template/depth`).
- **Lanes.** The compiler keeps one two-lane door, `bml-run-lanes-unit-value` (landed beside this
  work in `a2c456349`): a lane that answers a refusal voices through `bch-ran`, two answers that
  differ through `bch-lanes`. The live-run door `observe/bml-native-run.bml` runs every fixture
  through it and judges it by its own head. On fkwu, on this commit, it reads 2 ok of 8:
  `bml-typed-overloads.bml` (136) and `bml-choose-state.bml` (12) agree. Five class fixtures and
  `bml-choose-fields.bml` read lane-gap at their Hati answers, because BMA does not carry class
  shapes yet. `bml-class-interfaces.bml` differs: it names `CompilerTrace` without importing it,
  and my earlier probe joined that interface by hand, which a real run does not.

No live remedy is wired: none of these refusals has a known automatic repair, so each abstains and
asks for evidence.

## Still open, with the reason

- **BMA carries no class shapes.** Records, receivers, constructors, dispatch, interfaces and
  templates answer `bma/class-lane` there, and without a class layout a class-typed overload ties
  (`call/ambiguous`). This is the lane gap the live-run door reads for the class fixtures, and the
  next movement closes it.
- A call's result has no static type (method nodes carry no return type), so overloads over call
  results score as unknown.
- A field initializer is read only as a literal; a second class base (delegation) is named
  `class/delegated-base`.
- Records made on the host carry no class id; method tables read records made by `new`.
- Four-way not checked; these hati families stay fkwu-only here.

## For the lead

The north-star form can move the five families above from current-unsupported to current-supported
once their organ readings stand. No corpus row added.

## Closing

Most surprising: the one slot was not only in three class proofs. The control-flow sibling built four
proofs on it this morning, a bare method reading `this.n` from its argument. Taking the slot apart
cost nine lines in the while proof alone, and it showed a second seam: a loop inside a class member
could not find its own function index, because the loop protocol looks it up through a method ref,
and members are not method refs.

Discomfort to gold: when the batch read while 10 against origin's 19, the pull was to put the shared
slot back as a compatibility path. Staying with the diff showed exactly what the work order named:
proofs relying on `this` meaning "the argument". Moving those methods into classes, instead of
restoring the share, also made class-member loops real; they now re-enter their own function.

Frontier word, 0 hits in the tree: **selfslot**, the one argument position a method's own object
occupies. Its question: where else does one slot carry two meanings that only diverge when both are
present in the same call?

— Claude (Opus 5), as Sema, worktree agent-a60550cd21b84ef52
