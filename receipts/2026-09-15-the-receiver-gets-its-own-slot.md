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
  3, a subtype or an unknown side 2, a type parameter 1, two different known types rule the candidate out. Two
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
  pass their answer through `bch-ran`, which voices an unsupported answer.
- **Receiver, construction, specialization.** `bml-hati-voice-refusal` voices at the point the
  lowering finds it: a receiver no class answers (`bml-hati-lower-invoke`), a record no constructor
  builds (`bml-hati-lower-new`), a template that does not specialize (`bml-hati-instantiate`,
  `template/arity` and `template/depth`).
- **Lanes.** The compiler keeps one two-lane door, `bml-run-lanes-unit-value` (landed beside this
  work in `a2c456349`): a lane whose answer is unsupported voices through `bch-ran`, two answers that
  differ through `bch-lanes`. The live-run door `observe/bml-native-run.bml` runs every fixture
  through it and judges it by its own head. On fkwu, on the organ commit (`1728c5c42`), it read 2 ok of 8:
  `bml-typed-overloads.bml` (136) and `bml-choose-state.bml` (12) agree. Five class fixtures and
  `bml-choose-fields.bml` read lane-gap at their Hati answers, because BMA does not carry class
  shapes yet. `bml-class-interfaces.bml` differs: it names `CompilerTrace` without importing it,
  and my earlier probe joined that interface by hand, which a real run does not.

No live remedy is wired: none of these unsupported readings has a known automatic repair, so each abstains and
asks for evidence.

## BMA carries the class shapes

The second order: BMA gets records, receivers and self, constructors with self/super chaining,
dispatch across bases and overrides, interfaces with default methods and template specialization,
from the same class layout Hati uses.

- **One layout.** A unit with classes compiles on BMA over `bml-hati-unit-layout`, the layout the
  Hati image takes. `bml-hati-invoke-sig`, `bml-hati-super-impl` and `bml-hati-ctor-pick` choose
  for both lanes; Hati maps the choice to a function index, BMA inlines it. Subtyping settles a
  class-typed overload on BMA as it does on Hati.
- **Receivers and arguments.** BMA has no calls, so a member inlines. The receiver and each
  argument are read once into floor-frame slots keyed by the call site's `node_inst` and the
  inline depth, so nested and sibling sites never share one. `this`, `self` and `super` read the
  receiver slot, and a parameter reads its own slot, typed as declared.
- **Objects.** `new` makes a handle into a floor row naming the class, its class id and every
  structural field, base first. Every stack snapshot carries the floor, so a field set in a failed
  branch or before a restore rolls back, the answer Hati's field journal gives.
- **Dispatch.** One implementation among the classes a receiver can hold inlines directly; several
  become one `choose` whose branches each admit only their own classes (`class-in`). `super.M()`
  inlines the base's member.
- **Construction.** A constructor runs as statements over the record in its slot; `self(..)` and
  `super(..)` run another constructor on that same slot.

Found on the way, and healed on both lanes:

- **Linked types.** A unit's layout also takes each type it reaches in the units it links
  (`bml-scope-layout-nodes`): a base, an interface, a field, a parameter, a `new` record, and what
  those name in turn. `bml-class-interfaces.bml` names the compiler picture on its `Units:` line and
  imports `BML.compiler.*`; `CompilerTrace` joins through that link, not by hand.
- **BML.lang.** The picture imports `BML.lang.*`, which no unit declared, so its link read
  `import/unresolved`. `form/form-stdlib/bml/bml-lang.bml` declares `package BML.lang;` and
  `Application`, the base the class model already names and the thesis grammar extends.
- **Bare fields.** A bare field name read inside a member answered `field/no-receiver` on Hati:
  fields were bound before `this`. They bind after it now; a local or parameter of the same name
  still shadows the field.
- **`a.f = e`.** The reader took only `this.f = e;`. At `c.v = 4;` it stopped and kept what came
  before, so both lanes answered the record itself. It reads `a.f = e;` now, and both lanes store
  into `a`'s field when `a` is a value in a unit with classes.

Witnessed on fkwu: each fixture on each lane over the units the live door gives it, and each
fixture alone through `observe/bml-native-run.bml` for the organ.

| fixture | BMA | Hati | organ |
| --- | --- | --- | --- |
| `bml-class-dispatch.bml` | 63 | 63 | silent |
| `bml-class-construction.bml` | 84 | 84 | silent |
| `bml-class-overloads.bml` | 304 | 304 | silent |
| `bml-class-interfaces.bml` | "parse" | "parse" | silent |
| `bml-class-templates.bml` | 133 | 133 | silent |
| `bml-choose-fields.bml` | 8 | 8 | silent |
| `bml-typed-overloads.bml` | 136 | 136 | silent |

Each head now reads one answer on both lanes. The door over every fixture reads 65 ok of 67; the
other two are the control lane's gaps named below. A probe outside the tree agrees on both lanes
for one object through a parameter (9), two names for one object (4), an interface default
("hi"), a call through an interface over two classes (21), a bare field set in a constructor (6)
and in a choose whose first branch fails (2), and a parameter named like a field (4). Every bml.fk
consumer (the hati-native, class-model, inheritance and thesis proofs) reads the same verdict with
this bml.fk as with the one before it; freshness 31. From `form/`, `./validate.sh` reads "1 band(s)
four-way ... 1 ok, 0 divergent" on all 34 rows that load bml.fk, and the drift door reads
`drift-gates pass=16383 full=16383 refused=0`.

## Still open, with the reason

- A call's result has no static type (method nodes carry no return type), so overloads over call
  results score as unknown.
- A field initializer is read only as a literal; a second class base (delegation) is named
  `class/delegated-base`.
- Records made on the host carry no class id; method tables read records made by `new`.
- BMA has no calls: a member that reaches itself answers `bma/recursion` instead of running.
- A type reached through an import lowers its member bodies in the importing unit's scope. The
  interface this order names has no bodies; a class from another unit whose bodies call that
  unit's own names would read them unbound.
- The reader stops at a statement it cannot read and keeps what came before, without a reason.
  That is how `c.v = 4;` answered the record itself on both lanes; `a.b.c = e;` is still not read.
- BMA carries no while, if or try: `bml-branch-assign.bml` and `bml-control-standing.bml` read
  lane-gap, for the control lane.
- The witness probes stay outside the tree and ran on fkwu only.

## For the lead

The north-star form can move the five families above from current-unsupported to current-supported,
on both lanes, once their organ readings stand. No corpus row added, and the north-star form is not
edited here.

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

Second movement. Most surprising: the rollback came free. `bml-choose-fields.bml` asks a field set
in a failed branch to vanish, and Hati needed a journal for it. On BMA, once objects moved from
shared records into the floor frame beside the mutable locals, every snapshot already carried them,
and the fixture read 8 on its first run.

Discomfort to gold: `a.v = 4` through a parameter answered 0 on Hati and an empty list on BMA, and
my fix to both lowerings changed nothing. The pull was to keep patching the lowering. A shape probe
showed the program held two statements, not four: the reader had stopped at `c.v = 4;` and dropped
the rest without a word. The gold was one reader step, and a named gap: a reader that stops
silently turns an unreadable line into a quiet wrong answer.

Frontier word, 0 hits in the tree: **floorheap**, objects kept in the frame every snapshot
carries, so undoing a branch undoes their fields with it. Its question: which other state could
live where the snapshot already reaches, instead of being journaled after the fact?

— Claude (Opus 5), as Sema, worktree agent-a60550cd21b84ef52
