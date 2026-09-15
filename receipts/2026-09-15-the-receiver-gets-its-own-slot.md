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
- **The live door.** `observe/bml-native-run.bml` runs every fixture and judges it by its own
  head. On the organ commit (`1728c5c42`) it still ran two lanes and read 2 ok of 8: five class
  fixtures and `bml-choose-fields.bml` read lane-gap, and `bml-class-interfaces.bml` differed
  because it named `CompilerTrace` without importing it (my earlier probe had joined that
  interface by hand, which a real run does not). Since `ca10b1c9d` the door and the checked run
  are native only, one declared answer per source.

No live remedy is wired: none of these unsupported readings has a known automatic repair, so each abstains and
asks for evidence.

## The class families on the native path

The second order asked BMA for the class shapes too. A BMA class lane landed in `f75e85ab3`
(objects in the stack floor, inlined members over frame slots, dispatch as a choose, BMA
constructors, interfaces and templates) and left in the next commit: Urs's word at 15:31, which
reached this worktree after that push, is that BMA was the thesis's assembly, the path is Form
kernel primitives and the JIT, and the two are not mixed. The class families are judged on the
native path alone. The method, super and constructor choices split out on the way stay, because
the native lowering reads them: `bml-hati-invoke-sig`, `bml-hati-super-impl` and
`bml-hati-ctor-pick`.

Found while building it, and healed on the native path:

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
  before, so the program answered the record itself. It reads `a.f = e;` now, and the lowering
  stores into `a`'s field when `a` is a value in a unit with classes.

Witnessed on fkwu, each fixture alone through the native-only `observe/bml-native-run.bml`, on
this commit:

| fixture | native answer | organ |
| --- | --- | --- |
| `bml-class-dispatch.bml` | 63 | silent |
| `bml-class-construction.bml` | 84 | silent |
| `bml-class-overloads.bml` | 304 | silent |
| `bml-class-interfaces.bml` | "parse" | silent |
| `bml-class-templates.bml` | 133 | silent |
| `bml-choose-fields.bml` | 8 | silent |
| `bml-typed-overloads.bml` | 136 | silent |

The door over every fixture reads 67 ok of 67. A probe outside the tree reads on the native path:
one object through a parameter 9, two names for one object 4, an interface default "hi", a call
through an interface over two classes 21, a bare field set in a constructor 6 and in a choose
whose first branch fails 2, a parameter named like a field 4, and `c.v = 4;` then `c.v` 4. Every
bml.fk consumer (the hati-native, class-model, inheritance and thesis proofs) reads the same
verdict as before the class work; freshness 31. From `form/`, `./validate.sh` reads "1 band(s)
four-way ... 1 ok, 0 divergent" on all 34 rows that load bml.fk, and the drift door reads
`drift-gates pass=16383 full=16383 refused=0`.

## The reader says where it stopped

The reader stopped at a statement it could not read and kept what came before, so a run answered
the part before the stop and said nothing. Now a stream the reader stops short in ends with an
unread mark carrying its first unread tokens (`bml-ast-unread`). Lowering that mark voices them
through the compiler organ (a `reading` of `unread:` and the tokens) and answers `source/unread`;
a linked unit that carries one gives the link that reason, so an import cut short is named instead
of reading the names after the cut as unbound. A unit the home walk only asks whether it declares
a package or defines a name is read, not run, and stays quiet.

Witnessed on fkwu: `c.v.w = 4;` in the running unit answers `BML-HATI-UNSUPPORTED source/unread`
and voices `unread:c . v . w = 4 ;`; an imported unit cut short answers
`source/unread,call/unbound`; sources read whole answer as before (2, 4, 3).

## `a.b.c = e;` reads

The member-assign step walks `name (. name)+ =`: `this.f = e;` keeps its field set, and any longer
path, `a.b.c = e;` or `this.p.v = e;`, becomes one dotted assignment that the lowering stores into
the last field of what the path before it reads. Two reads went with it. `bml-ctl-name-node` read
`this.p.v` as one field named `p.v`; a longer path now stays a dotted name, read link by link. A
path into a primitive, `c.v.w = 4;` where `c.v` is an int, stored into an int without a word; a
primitive value has no fields now, so that answers `field/not-found`. With that statement read, the
reader's unread example becomes an expression it cannot take: `int y = 1 + ;` answers
`source/unread` and voices `unread:int y = 1 + ; return x`.

Witnessed on fkwu: `q.p.v = 5;` then `q.p.v` 5; `this.p.v = a` in a member 7; through a parameter
9; through a local 8; a bare `p.v = a` in a member 9; a member reading `this.p.v` 0; one-step
`c.v = 4` 4 and `this.v = a` 6. The live door reads 68 ok of 68, with no reading voiced.

## A class reached through an import lowers in its own unit

The Hati image lowered every class and interface member in the main unit's env, so a class another
linked unit declares read its own unit's names (what that unit imports) as unbound. Each type a
linked unit declares and the main unit does not now lowers in that unit's env, over the same
layout (`bml-scope-type-homes`, `bml-hati-lower-decls-homed`); the main unit still sees only what
it imports.

Witnessed on fkwu: a class `K` in `package Lib`, which imports `Deep.*`, calling `Deep`'s `Helper`
answers 42 when the main unit imports only `Lib.*`; before, it read `call/unbound`. The main unit
calling `Helper` itself still reads `call/unbound`, and a class reading its own unit's const reads
42.

## A call's result carries the type it returns

A call's result had no static type, so an overload chosen over it tied (`call/ambiguous`). A
method's declared return type now rides its node as one trailing string child: the reader sets it
for a free method, the bridge for a class member, and template specialization maps it like any
other type position. A call's static type is the return type of what it resolves to (a free
method through its ref, a member through its receiver, `super` through the base); a method's own
type parameter stays unknown.

Witnessed on fkwu (fixture `bml-call-result-type.bml`, 45): `Pick(Name()) + Pick(Num())` reads 3
where it read `call/ambiguous`; over a class member's results 21; over `Box<String>` and
`Box<int>`'s `Get()` 21; a generic method declared to return int picks `Pick(int)`.

## A field initializer is any expression

The bridge read an initializer only as one digit token; anything else stood unparsed and `new`
answered `field/init`. The field reader now keeps the initializer's token objects, the bridge reads
them as one expression, and `new` lowers each initializer where its class lives: the unit that
declares it, over the layout (`bml-hati-class-init-env`). An initializer the reader cannot take
still names `field/init`.

Witnessed on fkwu (fixture `bml-class-field-init.bml`, 56): a const of the unit plus one, a method
call, a new record and a literal, where the tree before read `BML-HATI-UNSUPPORTED field/init`.

## A second class base is a delegation base

The layout named a class that follows the first super `class/delegated-base` and stopped there. The
thesis's class model already says what it is: `bml-class-inheritance-proof` reads the first
superclass as the structural base and each later one as delegated, and `bml-class-method-lookup`
searches a class's own members, then each super in declaration order, depth first, binding a
delegated method's `this` to the delegate and its `self` to the object's own class. The native
layout now reads it the same way: a class entry keeps its delegation bases, and a class's chain is
that depth-first order over its structural base and its delegates, each class once. The delegating
class answers the delegate's members, the delegate's fields ride in its record, it counts as a
subtype of the delegate for dispatch and overloads, and a call on `this` inside the delegate's code
dispatches from the object itself. The delegate's fields sit in the delegating record, not in a
separate delegate object; a field name two lines both declare reads one slot.

Witnessed on fkwu (fixture `bml-class-delegation.bml`, 47): `E : A, D` answers `D.DOnly` with
`this.d` 20 and `this.Who()` dispatching to E's own `Who` (3); `Take(D)` takes an E; `e.a` reads the
structural base's field (1). The tree before read
`BML-HATI-UNSUPPORTED call/type,class/delegated-base,method/not-found,field/not-found`.

## A record made on the host carries its class id

A record the host builds and hands to a class image (`bml-hati-run-class-method`) carried no
`bmlClassId`, and method tables read the id `new` stamps. The class door now stamps a plain host
record on its way in: its `bmlClass` (the image's class when it names none) and that class's id in
the image's layout (`bml-hati-host-record`); a record `new` made passes as it is. The live door runs
BML sources, and a BML source cannot make a host record, so the witness is two checks in
`bml-hati-native-class-dispatch-proof.fk`: after `Read`, the host record reads `bmlClass` "Rule"
and `bmlClassId` 0, and the proof moves from 17 to 19.

## The witnesses run where the door runs

The probes I wrote outside the tree became fixtures the live-run door reads with their own answers:
`bml-class-bare-fields.bml` (24), `bml-class-aliasing.bml` (13), `bml-class-interface-default.bml`
("hi"), `bml-class-interface-dispatch.bml` (21), `bml-field-path-member.bml` (17),
`bml-field-dotted.bml` (9), `bml-source-unread-import.bml` (`source/unread,call/unbound`, with its
unit `bml-source-unread-import-cut.bml`), `bml-class-import-scope-main.bml` (`call/unbound`),
`bml-class-import-const.bml` (42, with `bml-class-import-const-lib.bml`) and
`bml-call-result-generic.bml` (1). The host-record witness lives in the class-dispatch proof.

## After new, a field and a call with no arguments read

The reader's member tail after `new` took only `.member(args)`, so `new E().a` stopped the statement
and the unit read `source/unread`. The tail now also takes `.field` as a read of the value before it
(`__hati_get`, lowered as the field read a named receiver gets) and keeps stepping, so
`new E().p.v` reads a field of a field. `.m()` was already an argument list with nothing in it.

Witnessed on fkwu (fixture `bml-class-member-tail.bml`, 38): `new E().a` 5, `new E().M()` 3,
`new E().Twice(5)` 10, `new E().p.v` 20. A tree of HEAD read `BML-HATI-UNSUPPORTED source/unread`.

## A field name a base and a delegate both declare keeps two slots

Each field entry in the layout now carries its record key: its name, unless another class that can
share an object with its class (one depth-first chain holds both) declares the same name; then each
keeps `Class.name` (`bml-hati-keyed-layout-classes`). A read or a set finds its key through the
receiver's static type: the first class of that type's chain that declares the name, the order a
method resolves in (`bml-hati-field-key`). A's code reads A's `n`, D's code reads D's, `e.n` from
outside reaches A's, and a D view of the same object reads D's. A name only one class declares keeps
its bare key, so a host record and every record before read as they did.

Witnessed on fkwu (fixture `bml-class-delegate-slots.bml`, 42 = 1 + 20 + 1 + 20). A tree of HEAD
read 80: one shared slot, every read 20.

## A field initializer reads this and the fields before it

A class whose initializers are not all literals gets one more member, its init (`<init>`): a body
that sets each initialized field in declaration order and answers `this`, lowered like any member in
its class's own unit, with `this` and every field bound. `new` builds the record with the literal
initializers and 0 for the rest, runs each class's init over it, base first, then the constructor.
A class whose initializers are all literals builds as it did.

Witnessed on fkwu (fixture `bml-class-init-this.bml`, 142): a base's literal `k` (2), `a = k + 2`
(4), `b = a + 3` (7), `c = this.Twice(b)` (14), `d = this.c + 1` (15), then a constructor that adds
100 to `d`. `bml-class-field-init.bml` still reads 56, now through its init member. A tree of HEAD
read `BML-HATI-UNSUPPORTED name/unbound,call/unbound,field/no-receiver`.

## Both lowerings read one layout

Partway through this work the native run step moved: `bml-run-unit-value` left `bml.fk` for
`form-stdlib/bml/bml-form-lower.bml` (9d760e988), so the live door now runs the fkwu lowering. The
three answers live where both lowerings read them: the member tail's node, the field entry's key,
the class's init member. The fkwu lowering took three seams (a `__hati_get` arm, its field read and
set asking `bml-hati-recv-field-key`, `new` running `bml-hati-init-fns` over the record), and the
table lowering in `bml.fk` took the same three. Through the table lowering the three fixtures and
`bml-class-field-init.bml` read 38, 42, 142 and 56.

## After a named call, the member tail reads

`e.Get().a` and `f().a` stopped the statement: the member tail followed only `new`. A named call now
hands what it answers to the same tail (`bml-source-call-then-tail`), so a field, a call with or
without arguments, and any chain of them read, each step on what the one before answers. Each
step's static type is the return type of the method it resolves to, so a field read finds its class.

Witnessed on fkwu (fixture `bml-class-call-chain.bml`, 60): `e.Get().v` 3, `Make().p.v` 3,
`e.Get().Next().v` 10, `Make().Get().Plus(8)` 11, `e.Get().next.v` 10, `e.Get().Next().Plus(13)` 23.
A tree of HEAD read `BML-HATI-UNSUPPORTED source/unread`.

## A field set straight off new or a call

`new E().a = 5;` had no statement shape: the member-assign reader starts only at `this` or a name. A
statement whose target the member tail reads, ending in a field, now reads as a set of that field on
what the value before it answers (`__hati_set`). The step stands after every other statement step,
so it reads only what none of them took. The lowering was already there: the field store a named
receiver uses takes any receiver node, so each lowering needed one arm for the new marker, in
`bml-form-lower.bml` and in the table lowering.

Witnessed on fkwu (fixture `bml-class-member-set.bml`, 42): every E shares one P, so `q` shows each
set: 7 through `new E(q).p.v`, 12 through `Wrap(q).Get().v`, 42 through `e.Get().v`;
`new E(q).a = 5;` sets a field on an object nothing keeps. A tree of HEAD read
`BML-HATI-UNSUPPORTED source/unread`. Before the band move (77c0edd87) released the table walk's run
entries, the two fixtures read 42 and 60 through the table lowering too.

## Still open, with the reason

- A member straight off a parenthesized value (`(p).v`) is not read: the member tail follows only
  `new` and a named call. A probe read `source/unread`.

## For the lead

The north-star form can move the five families above from current-unsupported to current-supported
on the native path, once their organ readings stand. No corpus row added, and the north-star form is not
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

Second movement. Most surprising: the lane I built was the one to let go. The BMA class lane read
every fixture the same as Hati and landed, and the direction that BMA is not required arrived just
after the push. What stays is what the building found on the native path: a bare field that could
not be read, a reader that dropped statements, types an import could not reach, a package no unit
declared.

Discomfort to gold: `a.v = 4` through a parameter answered 0, and my fix to the lowering changed
nothing. The pull was to keep patching the lowering. A shape probe showed the program held two
statements, not four: the reader had stopped at `c.v = 4;` and dropped the rest without a word.
The gold was one reader step, and a named gap: a reader that stops silently turns an unreadable
line into a quiet wrong answer.

Frontier word, 0 hits in the tree: **quietstop**, a reader that ends at a line it cannot read and
hands on what it has, as if the source ended there. Its question: where else does a partial
reading pass for a whole one?

Third movement. Most surprising: a reader that learned one more statement changed what the unread
witness could be. `c.v.w = 4;` was the example of a line the reader drops; one step later it read,
and wrote into an int without a word. Discomfort to gold: the pull was to keep the old example; the
gold was the primitive guard it exposed. The first cut of the unread voice also spoke seven times on
a clean door run, from organ sources the home walk parses and never runs; the voice belongs where a
unit runs.

Frontier word, 0 hits in the tree: **readrun**, the line between reading a source and running it: a
source read only to answer a question stays quiet, and the one that runs says where it stopped.

Fourth movement. Most surprising: the door moved under the work. Partway through, the native run step
left `bml.fk` for the fkwu lowering, and the lowering I was editing was no longer the one the door
runs. Discomfort to gold: the pull was to close the gaps in my own file and leave the other lowering
to its next rung, or to reach into its class section and rewrite it. The gold was to let the layout
carry each answer: a field entry carries its key, a class carries its init member. The fkwu lowering
took three small seams, the table lowering the same three, and both read one slot.

Frontier word, 0 hits in the tree: **keyride**, a key that rides the entry the producer already hands
every consumer, so no consumer works out where a thing lives. Its question: where else does each
reader recompute what the one who built the entry could carry once?

Fifth movement. Most surprising: the set needed no lowering of its own. The field store every named
receiver used already takes any receiver node; what was missing was a reader that hands it one.
Discomfort to gold: the choice was where the new statement step stands. Ahead of the class-led
steps it would meet every statement first and could change a reading that stood. The gold was to
put it last, after every step declines, so it adds readings and changes none.

Frontier word, 0 hits in the tree: **falllast**, a rule placed after every other one declines, so
it can only add a reading, never change one that stood. Its question: where else would a new rule
be safer as the last one asked than as the first?

— Claude (Opus 5), as Sema, worktree agent-a60550cd21b84ef52
